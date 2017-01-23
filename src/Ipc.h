// Interprocess Communication

#ifndef __IPC_H__
#define __IPC_H__

#include <queue>
#include <condition_variable>
#include <thread>
#include <chrono>
#include <memory>

namespace inv_example {

// ========================================
// IPC message queue
// ========================================
template <typename T>
class IpcQueue
{
public: // methods
    void Send(const T& msg);            // enqueue the message
    bool Try(void);                     // return true if a message is available
    T Wait(void);                       // wait for a message to become available
    std::pair<bool, std::unique_ptr<T>> TryGet(void);    // return <true,entry> if one is available, otherwise return <false,nullptr>
private: // data
    std::queue<T> m_q;
    std::condition_variable m_cond;
    std::mutex m_mtx;
};

// enqueue a message
template <typename T>
void IpcQueue<T>::Send(const T& msg)
{
    std::unique_lock<std::mutex> lock{ m_mtx };
    m_q.push(msg);
    m_cond.notify_one();
}


// return true if a message is available
template <typename T>
bool IpcQueue<T>::Try(void)
{
    std::unique_lock<std::mutex> lock{ m_mtx };
    return !m_q.empty();
}


// wait for a message to become available
template <typename T>
T IpcQueue<T>::Wait(void)
{
    std::unique_lock<std::mutex> lock{ m_mtx };
    m_cond.wait(lock, [this]{return !m_q.empty(); });    // keep waiting until queue is not empty
    auto msg = std::move(m_q.front());
    m_q.pop();
    return msg;
}


// return a message without waiting
template <typename T>
std::pair<bool, std::unique_ptr<T>> IpcQueue<T>::TryGet(void)
{
    std::pair<bool, std::unique_ptr<T>> msg(false, nullptr);     // default return value

    std::unique_lock<std::mutex> lock{ m_mtx };
    if (!m_q.empty()) {
        msg.first = true;                       // indicate the object is valid
        msg.second.reset(new T(m_q.front()));   // get a pointer to a copy of the object pulled from the queue
        m_q.pop();
    }
    return msg;
}


// ========================================
// Timer to generate periodic messages
// low-resolution and low-accuracy timer intended for timeouts and keepalives
// ========================================
template <typename T>
class IpcTimer
{
public: // constructors
    IpcTimer(unsigned int period_ms, T msg, IpcQueue<T>& q);    // send the specified msg to the q with the given period
    IpcTimer() = delete;                                        // must provide period
    ~IpcTimer();                                                // cancel the timer

private: // methods
    void timer_thread(void);

private: // data
    unsigned int m_period;  // timer period in ms
    T m_msg;            // copy of the message to be sent
    IpcQueue<T>& m_q;   // destination queue
    bool m_run = true;  // cancel timer when false
    std::unique_ptr<std::thread> m_pthread;   // pointer to thread object
};

// Create and start a timer that sends the specified msg to the given queue with the given period
template <typename T>
IpcTimer<T>::IpcTimer(unsigned int period_ms, T msg, IpcQueue<T>& q)
    : m_period(period_ms), m_msg(msg), m_q(q)
{
    // Create and start the thread
    m_pthread = std::unique_ptr<std::thread>(new std::thread(&IpcTimer<T>::timer_thread, this));
}

// timer thread
template <typename T>
void IpcTimer<T>::timer_thread(void)
{
    std::this_thread::sleep_for(std::chrono::milliseconds{ m_period }); // wait one period before first msg
    while (m_run) {
        // send msg first so cancelling the timer doesn't result in one additonal msg sent
        m_q.Send(m_msg);
        std::this_thread::sleep_for(std::chrono::milliseconds{ m_period });
    }
}

// Cancel the timer. May wait for as long as the timer period before returning
template <typename T>
IpcTimer<T>::~IpcTimer()
{
    m_run = false;
    m_pthread->join();
}

// ========================================
// High-resolution timer
// Calls a callback function at a periodic rate
// ========================================
class IpcHighResTimer
{
public: // types
    typedef void (*IpcHighResTimerCallback)(void);

public: // constructors
    IpcHighResTimer(unsigned int period_ms, IpcHighResTimerCallback proc);  // Create and start a timer with the given period and callback function
    IpcHighResTimer() = delete;                                             // must specify period and callback
    ~IpcHighResTimer();                                                     // cancel the timer

private: // methods
    static void timer_thread(IpcHighResTimer *pthis);

private: // data
    IpcHighResTimerCallback m_proc;         // pointer to callback function
    unsigned int m_timerid;                 // timer ID returned by windows
    std::unique_ptr<std::thread> m_pthread;   // pointer to thread object
    bool m_run = true;                      // thread is running
};


} // namespace inv_example

#endif // __IPC_H__


