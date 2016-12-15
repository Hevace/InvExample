// Communications interface packet definitions

#ifndef __COMMS__
#define __COMMS__

#include <cstdint>
#include <vector>
#include <map>
#include "timestamp.h"

namespace inv_example {

// ========================================
// Communications packet type definitions
// ========================================
enum PacketId {                      // message types
    // Cart interface messages
    FORCE_CMD = 0x10,
    CART_DATA = 0x11,
    POLL_CMD = 0x12,
    LOCK_CMD = 0x13,
    KEEPALIVE_CMD = 0xff,

    // Pendulum interface messages
    PEND_DATA = 0x20,
};

class CommPacketBase;
// ========================================
// Communications packet parser state machine
// ========================================
class InvCommParser
{
public: // constructors
    InvCommParser() : m_state(ParserState::HEADER) {};

public: // methods
    bool next(uint8_t b);                                 // TODO: // parse the next byte, return true if a msg is ready
    CommPacketBase get_next_packet(void);                      // TODO: // return the latest valid message

    // static methods for message creation and validation
    static unsigned int lookup_data_len(PacketId id);          // look up the length of the data part of the message given an ID
    static bool validate_packet(std::vector<uint8_t> packet);  // return true if packet has a valid format

public: // data
    // Message protocol constants
    static const uint8_t m_HEADER = 0xaa;     // first byte of every msg
    static const int m_HEADER_LEN = 3;        // minimum message size

private: // data
    // parser data
    enum class ParserState {
        HEADER,
        TYPE,
        LENGTH,
        DATA
    } m_state;
    static const std::map<PacketId, unsigned int> m_packet_id_table;     // lookup table of packet length vs msg ID
    std::vector<uint8_t> m_buf; // raw bytes as they are received
    InvTimestamp m_toa;         // time of arrival of first byte of message
};


// ================================================================================
// Communications packet definitions
// ================================================================================
// ========================================
// Communications packet base class
// ========================================
class CommPacketBase
{
protected: // constructors
    // Create a new packet with raw received message bytes and the time when the first byte was received
    CommPacketBase(std::vector<uint8_t> packet, InvTimestamp toa) : m_raw(packet), m_toa(toa) {};
    // Create a new outgoing packet template with ID and correct length
    CommPacketBase(PacketId id);

public: // methods
    unsigned int get_header(void) { return m_raw[0]; };                 // get the message header
    PacketId get_id(void) { return static_cast<PacketId>(m_raw[1]); };  // get the message type
    unsigned int get_data_len(void) { return m_raw[2]; };               // get number of bytes in the data portion
    std::vector<uint8_t>::iterator get_data(void) { return get_data_len() != 0 ? m_raw.begin() + 3 : m_raw.end(); };   // get pointer to start of data
    InvTimestamp get_toa(void) { return m_toa; };                       // get the timestamp

private: // data
    std::vector<uint8_t> m_raw;         // raw message bytes
    InvTimestamp m_toa;                 // time of arrival of the first byte of the message
};


// ========================================
// Cart Force Cmd Packet
// ========================================
class CartForceCmdPacket : public CommPacketBase
{
public: // constructors
    CartForceCmdPacket(std::vector<uint8_t> packet, InvTimestamp toa); // decode the data from received bytes
    CartForceCmdPacket(double force);                                  // encode a packet from data
    CartForceCmdPacket() = delete;                                     // cannot construct empty message

public: // data
    // packet contents data
    double m_force;       // cart force, N

    // data conversion factors and limits
    const double m_MAX_FORCE = DBL_MAX;         // N
    const double m_MIN_FORCE = -DBL_MAX;        // N
    const double m_SCALE_FORCE = 1.0;           // raw to N
};


// ========================================
// Cart Data Packet
// ========================================
class CartDataPacket : public CommPacketBase
{
public: // constructors
    CartDataPacket(std::vector<uint8_t> packet, InvTimestamp toa);  // decode the data from received bytes
    CartDataPacket(double cart_pos, double cart_vel);               // encode a packet from data
    CartDataPacket() = delete;                                      // cannot construct empty message

public: // data
    // message data
    double m_pos;         // cart position, m
    double m_vel;         // cart speed, m/s

    // data conversion factors and limits
    const double m_MAX_POS = DBL_MAX;           // m
    const double m_MIN_POS = -DBL_MAX;          // m
    const double m_SCALE_POS = 1.0;             // raw to m
    const double m_MAX_VEL = DBL_MAX;           // m/s
    const double m_MIN_VEL = -DBL_MAX;          // m/s
    const double m_SCALE_VEL = 1.0;             // raw to m/s
};


// ========================================
// Cart Poll Cmd Packet
// ========================================
class CartPollCmdPacket : public CommPacketBase
{
public: // constructors
    CartPollCmdPacket(std::vector<uint8_t> packet, InvTimestamp toa);  // decode the data from received bytes
    CartPollCmdPacket();                                               // encode a packet from data
};


// ========================================
// Cart Keepalive Cmd Packet
// ========================================
class CartKeepaliveCmdPacket : public CommPacketBase
{
public: // constructors
    CartKeepaliveCmdPacket(std::vector<uint8_t> packet, InvTimestamp toa);  // decode the data from received bytes
    CartKeepaliveCmdPacket();                                               // encode a packet from data
};


// ========================================
// Pendulum Data Packet
// ========================================
class PendDataPacket: public CommPacketBase
{
public: // constructors
    PendDataPacket(std::vector<uint8_t> packet, InvTimestamp toa);  // decode the data from received bytes
    PendDataPacket(double pos);                                     // encode a packet from data

public: // data
    double m_pos;           // pendulum position, deg

    // conversion factor and limits
    const double m_SCALE_POS = 360.0 / 65536.0;         // raw to deg
    const double m_MAX_POS = INT16_MAX * m_SCALE_POS;   // deg
    const double m_MIN_POS = INT16_MIN * m_SCALE_POS;   // deg
};

} // namespace inv_example
#endif // __COMMS__
