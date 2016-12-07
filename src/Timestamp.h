// Definitions for timestamps

#ifndef __TIMESTAMP_H__
#define __TIMESTAMP_H__

#include <iostream>
#include <chrono>

namespace InvExample {

  // ================================================================================
  // Implement timestamps using the std time library
  // In windows the resolution is 15.6 msec
  // ================================================================================
  class InvTimestamp
  {
  public:
    // Create a new timestamp with the current time
    InvTimestamp() : t(std::chrono::steady_clock::now()) {};

    // formatted output
    std::string ToString(void) const;             // Short string HH:MM:SS.sss in local time zone

  private:
    std::chrono::steady_clock::time_point t;      // timestamp stored in the std library type
  };


} // namespace InvExample

// ========================================
// formatted output of timestamp
// ========================================
std::ostream& operator<<(std::ostream& os, const InvExample::InvTimestamp& timestamp);

#endif // __TIMESTAMP_H__