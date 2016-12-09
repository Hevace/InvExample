// Interprocess Communication

#ifndef __IPC_H__
#define __IPC_H__

#include <queue>
#include <condition_variable>

namespace inv_example {

typedef int IpcMsgId;  // message id

// ========================================
// IPC message data structures
// ========================================
struct IpcMsg1Data
{
    int i1;
    int i2;
};

struct IpcMsg2Data
{
    double d1;
    double d2;
};

// ========================================
// IPC messages
// ========================================
class IpcMsg
{
public: // constructor
    IpcMsg(IpcMsgId id) : m_id{ id } {};
    IpcMsg(IpcMsgId id, IpcMsg1Data &msg1data) : m_id{ id } { m_data.msg1data = msg1data; };
    IpcMsg(IpcMsgId id, IpcMsg2Data &msg2data) : m_id{ id } { m_data.msg2data = msg2data; };
    IpcMsg() = delete;                  // must provide id and data
public: // methods
    IpcMsgId GetId() { return m_id; };
    IpcMsg1Data* GetMsg1Data() { return &m_data.msg1data; };
    IpcMsg2Data* GetMsg2Data() { return &m_data.msg2data; };
private: // data
    IpcMsgId m_id;                           // message id
    union {
        IpcMsg1Data msg1data;
        IpcMsg2Data msg2data;
    } m_data;                                // message data
};

// ========================================
// IPC message queue
// ========================================
template <typename T>
class IpcQueue
{
public: // methods
    void Send(T& msg);     // enqueue the message
    bool Try();                 // return true if a message is available
    T Wait();              // wait for a message to become available
private: // data
    std::queue<T> m_q;
    std::condition_variable m_cond;
    std::mutex m_mtx;
};

// enqueue a message
template <typename T>
void IpcQueue<T>::Send(T& msg)
{
    unique_lock<mutex> lock{ m_mtx };
    m_q.push(msg);
    m_cond.notify_one();
}


// return true if a message is available
template <typename T>
bool IpcQueue<T>::Try()
{
    unique_lock<mutex> lock{ m_mtx };
    return !m_q.empty();
}


// wait for a message to become available
template <typename T>
T IpcQueue<T>::Wait()
{
    unique_lock<mutex> lock{ m_mtx };
    m_cond.wait(lock, [this]{return !m_q.empty(); });    // keep waiting until queue is not empty
    auto msg = std::move(m_q.front());
    m_q.pop();
    return msg;
}


} // namespace inv_example

#endif __IPC_H__

