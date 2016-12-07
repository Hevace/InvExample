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
vector<uint8_t>::iterator DoubleToBytes(vector<uint8_t>::iterator pdest, double d)
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
pair<double, vector<uint8_t>::const_iterator> BytesToDouble(vector<uint8_t>::const_iterator p)
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
const map<MsgID, unsigned int> InvCommParser::MsgIdTable{
    { MsgID::ForceCmdMsgID, 8 },
    { MsgID::CartDataMsgID, 16 },
    { MsgID::PollCmdMsgID, 0 },
    { MsgID::LockCmdMsgID, 1 },
    { MsgID::KeepaliveMsgID, 0 },
    { MsgID::PendDataMsgID, 10 },
};

// ========================================
// Look up the message length from the table
// ========================================
unsigned int InvCommParser::LookupDataLen(MsgID id)
{
    try {
        return MsgIdTable.at(id);
    }
    catch (out_of_range) {
        return 0;                     // return zero if ID is not recognized  TODO handle error
    }
}

// ========================================
// Check if a string of bytes is a valid message
// ========================================
bool InvCommParser::ValidateMsg(std::vector<uint8_t> msg)
{
    if (msg.size() < HEADER_LEN || msg[0] != HEADER) return false;  // no header or bad header
    MsgID id = static_cast<MsgID>(msg[1]);
    unsigned int len = msg[2];
    auto p = MsgIdTable.find(id);
    if (p == MsgIdTable.end()) return false;            // undefined type
    if (p->second != len) return false;                 // unexpected length
    if (msg.size() < HEADER_LEN + len) return false;    // not all bytes received
    return true;
}

// ========================================
// Create message template with ID and correct length
// ========================================
InvMessage::InvMessage(MsgID id)
{
    Msg.clear();
    Msg.push_back(InvCommParser::HEADER);
    Msg.push_back(static_cast<unsigned int>(id)& 0xff);
    unsigned int data_len = InvCommParser::LookupDataLen(id);
    Msg.push_back(data_len & 0xff);
    Msg.insert(Msg.end(), data_len, 0);       // clear data section

    Toa = InvTimestamp();                     // timestamp with current time
}


// ========================================
// Cart Force Cmd
// ========================================
// create a message from received bytes
CartForceCmd::CartForceCmd(std::vector<uint8_t> msg, InvTimestamp toa)
: InvMessage(msg, toa)
{
    if (!InvCommParser::ValidateMsg(msg) || Id() != MsgID::ForceCmdMsgID) {
        throw NewInvError(InvErrorCode::INVALID_MSG);
    }
    // parse data members
    Force = BytesToDouble(Data()).first;
}


// create a message from data
CartForceCmd::CartForceCmd(double force)
: InvMessage(MsgID::ForceCmdMsgID)
{
    DoubleToBytes(Data(), force);
}


// ========================================
// Cart Data Msg
// ========================================
// Create a message from received bytes
CartDataMsg::CartDataMsg(std::vector<uint8_t> msg, InvTimestamp toa)
: InvMessage(msg, toa)
{
    if (!InvCommParser::ValidateMsg(msg) || Id() != MsgID::CartDataMsgID) {
        throw NewInvError(InvErrorCode::INVALID_MSG);
    }
    // parse data members
    vector<uint8_t>::const_iterator p = Data();    // point to start of data
    tie(CartPos, p) = BytesToDouble(p);
    tie(CartVel, p) = BytesToDouble(p);
}


// Create a message from data
CartDataMsg::CartDataMsg(double cart_pos, double cart_vel)
: InvMessage(MsgID::CartDataMsgID)
{
    auto p = Data();      // point to start of data
    p = DoubleToBytes(p, cart_pos);
    p = DoubleToBytes(p, cart_vel);
}

} // namespace inv_example
