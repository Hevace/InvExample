// Implementation of interface messages

#include <algorithm>
#include "comms.h"
#include "error.h"

using namespace std;
namespace inv_example {

// ================================================================================
// Endian conversion routines
// ================================================================================
// Convert double to a network-order vector of bytes
// assumes host is little-endian
// returns pointer to next available data byte
vector<uint8_t>::iterator double_to_bytes(vector<uint8_t>::iterator pdest, double d)
{
    union {
        double d;
        uint8_t v[sizeof(double)];
    } conv;

    conv.d = d;
    vector<uint8_t> out(sizeof(double));
    reverse_copy(conv.v, &conv.v[sizeof(conv.v)], pdest);

    return pdest + sizeof(double);
}

// Convert an array of bytes in network order to a double
// assumes host is little-endian
// returns decoded double and pointer to the next data byte
pair<double, vector<uint8_t>::const_iterator> bytes_to_double(vector<uint8_t>::const_iterator p)
{
    union {
        double d;
        uint8_t v[sizeof(double)];
    } conv;

    reverse_copy(p, p + sizeof(conv.v), conv.v);
    return make_pair(conv.d, p += sizeof(conv.v));
}

// ================================================================================
// Message Parser implementation
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
InvPacket::InvPacket(PacketId id)
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
CartForceCmd::CartForceCmd(std::vector<uint8_t> packet, InvTimestamp toa)
: InvPacket(packet, toa)
{
    if (!InvCommParser::validate_packet(packet) || get_id() != PacketId::FORCE_CMD) {
        throw NewInvError(InvErrorCode::INVALID_MSG);
    }
    // parse data members
    m_force = bytes_to_double(get_data()).first;
}


// encode a packet from data
CartForceCmd::CartForceCmd(double force)
: m_force(force), InvPacket(PacketId::FORCE_CMD)
{
    double_to_bytes(get_data(), force);
}


// ========================================
// Cart Data Msg
// ========================================
// decode the data from received bytes
CartDataMsg::CartDataMsg(std::vector<uint8_t> packet, InvTimestamp toa)
: InvPacket(packet, toa)
{
    if (!InvCommParser::validate_packet(packet) || get_id() != PacketId::CART_DATA) {
        throw NewInvError(InvErrorCode::INVALID_MSG);
    }
    // parse data members
    vector<uint8_t>::const_iterator p = get_data();    // point to start of data
    tie(m_pos, p) = bytes_to_double(p);
    tie(m_vel, p) = bytes_to_double(p);
}


// encode a packet from data
CartDataMsg::CartDataMsg(double cart_pos, double cart_vel)
: m_pos(cart_pos), m_vel(cart_vel), InvPacket(PacketId::CART_DATA)
{
    auto p = get_data();      // point to start of data
    p = double_to_bytes(p, cart_pos);
    p = double_to_bytes(p, cart_vel);
}

} // namespace inv_example
