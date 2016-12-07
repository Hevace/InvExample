// Definitions for timestamps

#ifndef __TIMESTAMP_H__
#define __TIMESTAMP_H__

#include <iostream>
#include <chrono>

namespace inv_example {

// ================================================================================
// Implement timestamps using the std time library
// In windows the resolution is 15.6 msec
// ================================================================================
class InvTimestamp
{
public:
    // Create a new timestamp with the current time
    InvTimestamp() : m_t(std::chrono::steady_clock::now()) {};

    // formatted output
    std::string to_string(void) const;             // Short string HH:MM:SS.sss in local time zone

private:
    std::chrono::steady_clock::time_point m_t;      // timestamp stored in the std library type
};


} // namespace inv_example

// ========================================
// formatted output of timestamp
// ========================================
std::ostream& operator<<(std::ostream& os, const inv_example::InvTimestamp& timestamp);

#endif // __TIMESTAMP_H__