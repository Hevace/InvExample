// Timestamp implementation

#include <chrono>
#include <sstream>
#include <iomanip>
#include <ctime>
#include "timestamp.h"

namespace inv_example {

// ================================================================================
// Timestamp implementation for Windows
// ================================================================================
// ========================================
// formatted output as a short string HH:MM:SS.ffff
// ========================================
std::string InvTimestamp::to_string(void) const
{
    std::stringstream outstr;   // return value

    // get the fractional part of seconds
    double t_sec = std::chrono::duration<double, std::ratio<1, 1>>(m_t.time_since_epoch()).count();     // time in sec since epoch
    int t_frac = static_cast<int>((t_sec - floor(t_sec))*10000.0);      // number of 0.1 msec since last second

    // use a std function to print hours:minutes:seconds in local time zone
    auto tnow = std::chrono::steady_clock::to_time_t(m_t);
    tm tmnow;
    localtime_s(&tmnow, &tnow);   // time in local time zone
    char str1[9];
    std::strftime(str1, sizeof(str1), "%H:%M:%S", &tmnow);

    outstr << str1 << "." << std::setw(4) << std::setfill('0') << t_frac;
    return outstr.str();
}

} // namespace inv_example

// ========================================
// output formatted timestamp to a stream
// ========================================
std::ostream& operator<<(std::ostream& os, const inv_example::InvTimestamp& timestamp) { os << timestamp.to_string(); return os; };
