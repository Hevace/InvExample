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
public: // constructors
    // Create a new timestamp with the current time
    InvTimestamp() : m_t(std::chrono::system_clock::now()) {};

public: // methods
    // formatted output
    std::string to_string(void) const;             // Short string HH:MM:SS.ssss in local time zone
    std::chrono::system_clock::duration to_duration(const InvTimestamp& t0) const { return m_t - t0.m_t; }; // difference as a duration

    // operators
    double operator-(const InvTimestamp &t0) { return std::chrono::duration<double, std::ratio<1, 1>>(to_duration(t0)).count(); }; // difference in seconds

private: // data
    std::chrono::system_clock::time_point m_t;      // timestamp stored in the std library type
};


} // namespace inv_example

// ========================================
// formatted output of timestamp
// ========================================
std::ostream& operator<<(std::ostream& os, const inv_example::InvTimestamp& timestamp);

#endif // __TIMESTAMP_H__
