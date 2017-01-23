// Windows implementation of interprocess communication functions

#ifdef _WIN32
#include <windows.h>
#endif

#ifdef __linux__
#include <thread>
#include <sys/timerfd.h>
#include <cstring>
#include <unistd.h>
#endif

#include "System.h"
#include "Ipc.h"
#include "Error.h"

namespace inv_example {
#ifdef _WIN32
// ================================================================================
// Windows implementation
// ================================================================================
// ========================================
// High-resolution timer
// Calls a callback function at a periodic rate
// ========================================
// timer callback function for windows. See LPTIMECALLBACK
extern "C" void CALLBACK win_timer_callback(UINT uTimerID, UINT uMsg, DWORD_PTR dwUser, DWORD_PTR dw1, DWORD_PTR dw2)
{
    reinterpret_cast<IpcHighResTimer*>(dwUser)->callback();
}

// Create and start a timer with the given period and callback function
IpcHighResTimer::IpcHighResTimer(unsigned int period_ms, IpcHighResTimerCallback proc)
{
    TIMECAPS tc;
    MMRESULT mr = timeGetDevCaps(&tc, sizeof(TIMECAPS));
    if (mr) {
        throw NewInvError(SYSERR_RESOURCE_ALLOCATION_FAILED);
    }
    if (period_ms < tc.wPeriodMin || period_ms > tc.wPeriodMax) {
        throw std::invalid_argument("High Res Timer period out of range");
    }
    if (proc == nullptr) {
        throw std::invalid_argument("High Res Timer has a null callback");
    }

    m_proc = proc;
    m_timerid = timeSetEvent(period_ms, 0, win_timer_callback, reinterpret_cast<DWORD_PTR>(this), TIME_PERIODIC);
    if (m_timerid == NULL) {
        throw NewInvError(SYSERR_RESOURCE_ALLOCATION_FAILED);
    }
}

// Cancel the timer
IpcHighResTimer::~IpcHighResTimer()
{
    timeKillEvent(m_timerid);
}
#endif    // end of Windows implementation


#ifdef __linux__
// ================================================================================
// Linux implementation
// ================================================================================
// ========================================
// High-resolution timer
// Calls a callback function at a periodic rate
// Create and start a timer with the given period and callback function
// ========================================
IpcHighResTimer::IpcHighResTimer(unsigned int period_ms, IpcHighResTimerCallback proc)
{
    if (proc == nullptr) {
        throw std::invalid_argument("High Res Timer has a null callback");
    }
    m_proc = proc;

    // Create the timer
    int fd = timerfd_create(CLOCK_MONOTONIC, 0);
    if (fd < 0) {
        InvError e(SYSERR_RESOURCE_ALLOCATION_FAILED, __LINE__, __FILE__);
        throw e;
    }
    else {
        m_timerid = static_cast<unsigned int>(fd);
    }

    // Create and start the thread
    m_pthread = std::unique_ptr<std::thread>(new std::thread(IpcHighResTimer::timer_thread, this));

    // Make the timer periodic
    unsigned int sec = period_ms/1000;
    unsigned int ns = (period_ms - 1000*sec) * 1000;
    struct itimerspec itval;
    itval.it_interval.tv_sec = sec;         // timer period
    itval.it_interval.tv_nsec = ns;
    itval.it_value.tv_sec = sec;            // first timer expiration time
    itval.it_value.tv_nsec = ns;
    int ret = timerfd_settime(m_timerid, 0, &itval, 0);
    if (ret == -1) {
        InvError e(SYSERR_RESOURCE_ALLOCATION_FAILED, __LINE__, __FILE__);
        throw e;
    }
}

// Cancel the timer
IpcHighResTimer::~IpcHighResTimer()
{
    // tell the thread to quit on the next tick
    m_run = false;
    m_pthread->join();
    close(m_timerid);
}

// Thread to wait for the timer
void IpcHighResTimer::timer_thread(IpcHighResTimer *pthis)
{
    uint64_t expirations = 0;
    uint64_t total_expirations = 0;
    int iterations = 0;
    int retval = 0;

    while (pthis->m_run) {
        if ((retval = read(pthis->m_timerid, &expirations, sizeof(expirations))) > 0) {
            total_expirations += expirations;
            iterations++;
            // call timer callback
            (*pthis->m_proc)();
        }
    }

    // Shut down the timer
    struct itimerspec itval;
    std::memset(&itval, 0, sizeof(itval));
    timerfd_settime(pthis->m_timerid, 0, &itval, 0);
}

#endif // __linux__

} // namespace inv_example
