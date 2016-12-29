// Implementation of interface messages

#include <algorithm>
#include "System.h"
#include "Comms.h"
#include "Error.h"

using namespace std;
namespace inv_example {

// ================================================================================
// Endian conversion routines
// ================================================================================
// Convert double to a network-order vector of bytes
// assumes host is little-endian
// returns pointer to next available data byte
vector<uint8_t>::iterator convert_to_bytes_double(vector<uint8_t>::iterator pdest, double d, double max, double min, double scale)
{
    union {
        double d;
        uint8_t v[sizeof(double)];
    } conv;

    d = std::min(d, max);
    d = std::max(d, min);
    conv.d = d / scale;             // convert double to raw
    reverse_copy(conv.v, &conv.v[sizeof(conv.v)], pdest);

    return pdest + sizeof(conv.v);
}

// Convert signed 16-bit int to a network-order vector of bytes
// assumes host is little-endian
// returns pointer to next available data byte
vector<uint8_t>::iterator convert_to_bytes_i16(vector<uint8_t>::iterator pdest, double d, double max, double min, double scale)
{
    union {
        int16_t  d;
        uint8_t v[sizeof(int16_t)];
    } conv;

    d = std::min(d, max);
    d = std::min(d, static_cast<double>(INT16_MAX));
    d = std::max(d, min);
    d = std::max(d, static_cast<double>(INT16_MIN));
    conv.d = static_cast<int16_t>(d / scale);
    reverse_copy(conv.v, &conv.v[sizeof(conv.v)], pdest);

    return pdest + sizeof(conv.v);
}

// Convert an array of bytes in network order to a double
// assumes host is little-endian
// returns decoded double and pointer to the next data byte
pair<double, vector<uint8_t>::const_iterator> bytes_to_double(vector<uint8_t>::const_iterator p, double max, double min, double scale)
{
    union {
        double d;
        uint8_t v[sizeof(double)];
    } conv;

    reverse_copy(p, p + sizeof(conv.v), conv.v);
    double dout = conv.d * scale;
    dout = std::min(dout, max);
    dout = std::max(dout, min);
    return make_pair(dout, p += sizeof(conv.v));
}

// Convert an array of bytes in network order to a signed 16-bit int
// assumes host is little-endian
// returns decoded int and pointer to the next data byte
pair<int16_t, vector<uint8_t>::const_iterator> bytes_to_i16(vector<uint8_t>::const_iterator p, double max, double min, double scale)
{
    union {
        int16_t d;
        uint8_t v[sizeof(int16_t)];
    } conv;

    reverse_copy(p, p + sizeof(conv.v), conv.v);
    double dout = conv.d * scale;
    dout = std::min(dout, max);
    dout = std::max(dout, min);
    return make_pair(conv.d, p += sizeof(conv.v));      // implicit cast
}

// ================================================================================
// Communication Packet Parser implementation
// ================================================================================
// ========================================
// Lookup table of valid msg length vs msg ID
// length is for the data portion only
// ========================================
const map<PacketId, unsigned int> InvCommParser::m_packet_id_table{
    { PacketId::FORCE_CMD, 8 },
    { PacketId::CART_DATA, 16 },
    { PacketId::POLL_CMD, 0 },
    { PacketId::LOCK_CMD, 1 },
    { PacketId::KEEPALIVE_CMD, 0 },
    { PacketId::PEND_DATA, 10 },
};

// ========================================
// Look up the message length from the table
// ========================================
unsigned int InvCommParser::lookup_data_len(PacketId id)
{
    try {
        return m_packet_id_table.at(id);
    }
    catch (out_of_range) {
        return 0;                     // return zero if ID is not recognized  TODO handle error
    }
}

// ========================================
// Check if a string of bytes is a valid message
// ========================================
bool InvCommParser::validate_packet(std::vector<uint8_t> packet)
{
    if (packet.size() < m_HEADER_LEN || packet[0] != m_HEADER) return false;  // no header or bad header
    PacketId id = static_cast<PacketId>(packet[1]);
    unsigned int len = packet[2];
    auto p = m_packet_id_table.find(id);
    if (p == m_packet_id_table.end()) return false;         // undefined type
    if (p->second != len) return false;                     // unexpected length
    if (packet.size() < m_HEADER_LEN + len) return false;   // not all bytes received
    return true;
}

// ========================================
// Create message template with ID and correct length
// ========================================
CommPacketBase::CommPacketBase(PacketId id)
{
    m_raw.clear();
    m_raw.push_back(InvCommParser::m_HEADER);
    m_raw.push_back(static_cast<unsigned int>(id)& 0xff);
    unsigned int data_len = InvCommParser::lookup_data_len(id);
    m_raw.push_back(data_len & 0xff);
    m_raw.insert(m_raw.end(), data_len, 0);     // clear data section

    m_toa = InvTimestamp();                     // timestamp with current time
}


// ========================================
// Cart Force Cmd
// ========================================
// decode the data from received bytes
CartForceCmdPacket::CartForceCmdPacket(std::vector<uint8_t> packet, InvTimestamp toa)
    : CommPacketBase(packet, toa)
{
    if (!InvCommParser::validate_packet(packet) || get_id() != PacketId::FORCE_CMD) {
        throw NewInvError(SYSERR_CART_FORCE_MSG_PARSE);
    }
    // parse data members
    m_force = bytes_to_double(get_data(), m_MAX_FORCE, m_MIN_FORCE, m_SCALE_FORCE).first;
}


// encode a packet from data
CartForceCmdPacket::CartForceCmdPacket(double force)
    : m_force(force), CommPacketBase(PacketId::FORCE_CMD)
{
    convert_to_bytes_double(get_data(), m_force, m_MAX_FORCE, m_MIN_FORCE, m_SCALE_FORCE);
}


// ========================================
// Cart Data Packet
// ========================================
// decode the data from received bytes
CartDataPacket::CartDataPacket(std::vector<uint8_t> packet, InvTimestamp toa)
    : CommPacketBase(packet, toa)
{
    if (!InvCommParser::validate_packet(packet) || get_id() != PacketId::CART_DATA) {
        throw NewInvError(SYSERR_CART_DATA_MSG_PARSE);
    }
    // parse data members
    vector<uint8_t>::const_iterator p = get_data();    // point to start of data
    tie(m_pos, p) = bytes_to_double(p, m_MAX_POS, m_MIN_POS, m_SCALE_POS);
    tie(m_vel, p) = bytes_to_double(p, m_MAX_VEL, m_MIN_VEL, m_SCALE_VEL);
}


// encode a packet from data
CartDataPacket::CartDataPacket(double cart_pos, double cart_vel)
    : m_pos(cart_pos), m_vel(cart_vel), CommPacketBase(PacketId::CART_DATA)
{
    auto p = get_data();      // point to start of data
    p = convert_to_bytes_double(p, m_pos, m_MAX_POS, m_MIN_POS, m_SCALE_POS);
    p = convert_to_bytes_double(p, m_vel, m_MAX_VEL, m_MIN_VEL, m_SCALE_VEL);
}


// ========================================
// Cart Poll Cmd Packet
// ========================================
// decode the data from received bytes
CartPollCmdPacket::CartPollCmdPacket(std::vector<uint8_t> packet, InvTimestamp toa)
    : CommPacketBase(packet, toa)
{
    if (!InvCommParser::validate_packet(packet) || get_id() != PacketId::POLL_CMD) {
        throw NewInvError(SYSERR_CART_POLL_MSG_PARSE);
    }
    // no data members
}


// encode a packet from data
CartPollCmdPacket::CartPollCmdPacket()
    : CommPacketBase(PacketId::POLL_CMD)
{
    // no data
}


// ========================================
// Cart Keepalive Cmd Packet
// ========================================
// decode the data from received bytes
CartKeepaliveCmdPacket::CartKeepaliveCmdPacket(std::vector<uint8_t> packet, InvTimestamp toa)
    : CommPacketBase(packet, toa)
{
    if (!InvCommParser::validate_packet(packet) || get_id() != PacketId::KEEPALIVE_CMD) {
        throw NewInvError(SYSERR_CART_KEEPALIVE_MSG_PARSE);
    }
    // no data members
}


// encode a packet from data
CartKeepaliveCmdPacket::CartKeepaliveCmdPacket()
    : CommPacketBase(PacketId::KEEPALIVE_CMD)
{
    // no data
}


// ========================================
// Pendulum Data Packet
// ========================================
// decode the data from received bytes
PendDataPacket::PendDataPacket(std::vector<uint8_t> packet, InvTimestamp toa)
    : CommPacketBase(packet, toa)
{
    if (!InvCommParser::validate_packet(packet) || get_id() != PacketId::PEND_DATA) {
        throw NewInvError(SYSERR_PEND_DATA_MSG_PARSE);
    }
    // parse data members
    vector<uint8_t>::const_iterator p = get_data();            // points to start of data
    tie(m_pos, p) = bytes_to_i16(p, m_MAX_POS, m_MIN_POS, m_SCALE_POS);
}


// encode a packet from data
PendDataPacket::PendDataPacket(double pos)
    : m_pos(pos), CommPacketBase(PacketId::PEND_DATA)
{
    convert_to_bytes_i16(get_data(), m_pos, m_MAX_POS, m_MIN_POS, m_SCALE_POS);
}

} // namespace inv_example
