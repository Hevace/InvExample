// Interprocess communication

#include "Ipc.h"

using namespace std;

namespace inv_example {

// ========================================
// IPC messages
// ========================================
// Message creation
IpcMsg::IpcMsg(IpcMsgId id, IpcMsg1Data &msg1data) : m_id{ id }
{
    m_data.msg1data = msg1data;
}


IpcMsg::IpcMsg(IpcMsgId id, IpcMsg2Data &msg2data) : m_id{ id }
{
    m_data.msg2data = msg2data;
}


// Extracting message data
IpcMsg1Data* IpcMsg::GetMsg1Data()
{
    if (m_id == 1) {
        return &m_data.msg1data;
    }
    return nullptr;
}


} // namespace inv_example
