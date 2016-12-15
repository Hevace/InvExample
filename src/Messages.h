// Interprocess message definitions

#ifndef __MESSAGES_H__
#define __MESSAGES_H__

#include "Comms.h"

namespace inv_example {

enum IpcMsgId {
    // Application messages
    MSG_EXIT = 256,
    MSG_MOVE_CMD,
    MSG_RESET_CMD,
    MSG_ARRIVED,
    MSG_KEEPALIVE,

    // communication messages
    MSG_CART_DATA = PacketId::CART_DATA,    // Cart interface messages
    MSG_PEND_DATA = PacketId::PEND_DATA,    // Pendulum interface messages
};

// ========================================
// IPC messages
// ========================================
class IpcMsg
{
public: // constructor
    IpcMsg(IpcMsgId id) : m_id{ id } {};
    IpcMsg(IpcMsgId id, std::vector<uint8_t> raw_msg) : m_id{ id }, m_raw_msg(raw_msg) { };
    IpcMsg() = delete;                  // must provide id and data
public: // methods
    IpcMsgId GetId() { return m_id; };
    std::vector<uint8_t> GetRawMsg() { return m_raw_msg; };
private: // data
    IpcMsgId m_id;                           // message id
    std::vector<uint8_t> m_raw_msg;
};


}

#endif // __MESSAGES_H__
