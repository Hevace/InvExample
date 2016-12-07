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
enum class PacketId {                      // message types
    // Cart interface messages
    FORCE_CMD = 0x10,
    CART_DATA = 0x11,
    POLL_CMD = 0x12,
    LOCK_CMD = 0x13,
    KEEPALIVE_CMD = 0xff,

    // Pendulum interface messages
    PEND_DATA = 0x20,
};

class InvPacket;
// ========================================
// Communications packet parser state machine
// ========================================
class InvCommParser
{
public: // constructors
    InvCommParser() : m_state(ParserState::HEADER) {};

public: // methods
    bool next(uint8_t b);                                 // parse the next byte, return true if a msg is ready
    InvPacket get_next_packet(void);                      // return the latest valid message

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
class InvPacket
{
protected: // constructors
    // Create a new packet with raw received message bytes and the time when the first byte was received
    InvPacket(std::vector<uint8_t> packet, InvTimestamp toa) : m_raw(packet), m_toa(toa) {};
    // Create a new outgoing packet template with ID and correct length
    InvPacket(PacketId id);

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
// Cart Force Cmd
// ========================================
class CartForceCmd : public InvPacket
{
public: // constructors
    CartForceCmd(std::vector<uint8_t> packet, InvTimestamp toa); // decode the data from received bytes
    CartForceCmd(double force);                                  // encode a packet from data
    CartForceCmd() = delete;                                     // cannot construct empty message

public: // data
    // packet contents data
    double m_force;       // cart force, N
};


// ========================================
// Cart Data Msg
// ========================================
class CartDataMsg : public InvPacket
{
public: // constructors
    CartDataMsg(std::vector<uint8_t> packet, InvTimestamp toa);  // decode the data from received bytes
    CartDataMsg(double cart_pos, double cart_vel);               // encode a packet from data
    CartDataMsg() = delete;                                      // cannot construct empty message

public: // data
    // message data
    double m_pos;         // cart position, m
    double m_vel;         // cart speed, m/s
};

} // namespace inv_example
#endif // __COMMS__
