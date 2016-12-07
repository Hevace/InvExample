// Communications interface message definitions

#ifndef __COMMS__
#define __COMMS__

#include <cstdint>
#include <vector>
#include <map>
#include "timestamp.h"

namespace inv_example {

// ========================================
// Message type definitions
// ========================================
enum class MsgID {                      // message types
    // Cart interface messages
    ForceCmdMsgID = 0x10,
    CartDataMsgID = 0x11,
    PollCmdMsgID = 0x12,
    LockCmdMsgID = 0x13,
    KeepaliveMsgID = 0xff,

    // Pendulum interface messages
    PendDataMsgID = 0x20,
};

class InvMessage;
// ========================================
// Message parser state machine
// ========================================
class InvCommParser
{
private:
    // parser data
    static const std::map<MsgID, unsigned int> MsgIdTable;     // lookup table of msg length vs msg ID
    enum class ParserState {
        HEADER,
        TYPE,
        LENGTH,
        DATA
    } State;
    std::vector<uint8_t> Buf; // raw bytes as they are received
    InvTimestamp Toa;         // time of arrival of first byte of message

public:
    InvCommParser() : State(ParserState::HEADER) {};
    bool Next(uint8_t b);                                 // parse the next byte, return true if a msg is ready
    InvMessage GetNextMsg(void);                          // return the latest valid message

    // static methods for message creation and validation
    static unsigned int LookupDataLen(MsgID id);          // look up the length of the data part of the message given an ID
    static bool ValidateMsg(std::vector<uint8_t> msg);    // return true if msg is a valid message

    // Message protocol constants
    static const uint8_t HEADER = 0xaa;     // first byte of every msg
    static const int HEADER_LEN = 3;        // minimum message size
};


// ================================================================================
// Message definitions
// ================================================================================
// ========================================
// message base class
// ========================================
class InvMessage
{
private:
    std::vector<uint8_t> Msg;         // raw message bytes
    InvTimestamp Toa;                 // time of arrival of the first byte of the message

public:
    unsigned int Header(void) { return Msg[0]; };                 // get the message header
    MsgID Id(void) { return static_cast<MsgID>(Msg[1]); };        // get the message type
    unsigned int DataLen(void) { return Msg[2]; };                // get number of bytes in the data portion
    std::vector<uint8_t>::iterator Data(void) { return DataLen() != 0 ? Msg.begin() + 3 : Msg.end(); };   // get pointer to start of data
    InvTimestamp GetToa(void) { return Toa; };                    // get the timestamp

protected:
    // Create a new message with raw the received message bytes and the time when the first byte was received
    InvMessage(std::vector<uint8_t> msg, InvTimestamp toa) : Msg(msg), Toa(toa) {};
    // Create a new outgoing message template with ID and correct length
    InvMessage(MsgID id);
};


// ========================================
// Cart Force Cmd
// ========================================
class CartForceCmd : public InvMessage
{
public:
    CartForceCmd(std::vector<uint8_t> msg, InvTimestamp toa); // create a message from received bytes
    CartForceCmd(double force);                               // create a message from data
    CartForceCmd() = delete;                                  // cannot construct empty message

    // message data
    double Force;       // cart force, N
};


// ========================================
// Cart Data Msg
// ========================================
class CartDataMsg : public InvMessage
{
public:
    CartDataMsg(std::vector<uint8_t> msg, InvTimestamp toa);  // create a message from received bytes
    CartDataMsg(double cart_pos, double cart_vel);            // create a message from data
    CartDataMsg() = delete;                                   // cannot construct empty message

    // message data
    double CartPos;         // cart position, m
    double CartVel;         // cart speed, m/s
};

} // namespace inv_example
#endif // __COMMS__
