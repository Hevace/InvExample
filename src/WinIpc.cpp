// Windows implementation of interprocess communication functions

#include <windows.h>

#include "System.h"
#include "Ipc.h"
#include "Error.h"

namespace inv_example {
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

} // namespace inv_example
