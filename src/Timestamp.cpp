// Timestamp implementation

#include <chrono>
#include <sstream>
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

    std::chrono::duration<double, std::milli> t_fp = m_t.time_since_epoch();
    double t_ms = t_fp.count();
    int ms = static_cast<int>((t_ms - floor(t_ms))*10000.0);  // number of 0.1 msec since last second

    auto tnow = std::chrono::steady_clock::to_time_t(m_t);
    tm tmnow;
    localtime_s(&tmnow, &tnow);   // time in local time zone
    char str1[9];
    std::strftime(str1, sizeof(str1), "%H:%M:%S", &tmnow);

    outstr << str1 << "." << ms;
    return outstr.str();
  }

} // namespace inv_example

// ========================================
// output formatted timestamp to a stream
// ========================================
std::ostream& operator<<(std::ostream& os, const inv_example::InvTimestamp& timestamp) { os << timestamp.to_string().c_str(); return os; };
