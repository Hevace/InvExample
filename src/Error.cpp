// Implementation of error types
// with fault codes, error messages, and error levels

#include <iostream>
#include <iomanip>
#include <sstream>
#include "Error.h"

using namespace std;
namespace inv_example {

// ========================================
// Error message object
// represents a single error condition
// ========================================
// Create a printable string
std::string InvError::to_string(void) const
{
    stringstream str;
    //HH:MM:SS.ssss c2345 f2345678901234567890:l2345
    str << m_time << " ";
    str << setw(6) << m_code << " ";
    str << setw(20) << setfill(' ') << m_file.substr(20) << ":" << setw(5) << m_line;
    return str.str();
}


// ========================================
// Error message table
// lookup table containing severity level and error message for each code
// ========================================
// initializes the error table
InvErrorTable::InvErrorTable(std::initializer_list<const InvErrorInit> init)
{
    AddErrorCode(init);
}


// add error codes and messages to the table
void InvErrorTable::AddErrorCode(std::initializer_list<const InvErrorInit> init)
{
    for (auto p = init.begin(); p < init.end(); p++)
    {
        InvErrorInfo info{ p->level, p->msg };
        m_table.insert({ p->code, info });
    }
}


// look up error level for given code
InvErrorLevel InvErrorTable::LookupErrorLevel(const InvError& err) const
{
    switch (static_cast<SpecialErrCode>(err.get_code())) {
    case SpecialErrCode::EXCEPTION:
        // Exceptions are always Info level
        return InvErrorLevel::INFO;
        break;

    case SpecialErrCode::NULL_ERROR:
        // No error or message
        return InvErrorLevel::NONE;
        break;

    default:
        try {
            return m_table.at(err.get_code()).level;
        }
        catch (out_of_range) {
            return InvErrorLevel::WARNING;          // undefined error codes return as a warning
        }
    }
}


// look up error message for a given code
std::string InvErrorTable::LookupErrorMsg(const InvError& err) const
{
    switch (static_cast<SpecialErrCode>(err.get_code())) {
    case SpecialErrCode::EXCEPTION:
        // Exception errors store their own message
        return err.get_exp_msg();
        break;

    case SpecialErrCode::NULL_ERROR:
        return "No error";
        break;

    default:
        try {
            return m_table.at(err.get_code()).msg;
        }
        catch (out_of_range) {
            return "Undefined error";               // error code is not in the table
        }
        break;
    }
}


// Get a printable string
std::string InvErrorTable::to_string(const InvError& err) const
{
    string level;
    switch (LookupErrorLevel(err)) {
    case InvErrorLevel::NONE:       level = "NONE";
    case InvErrorLevel::FATAL:      level = "FATAL";
    case InvErrorLevel::WARNING:    level = "WARN";
    case InvErrorLevel::INFO:       level = "INFO";
    default:                        level = "UNDEF";
    }

    stringstream str;
    str << err << " ";
    str << setw(5) << setfill(' ') << level << " ";
    str << LookupErrorMsg(err);
    return str.str();
}

} // namespace inv_example

// ========================================
// output formatted error to a stream
// ========================================
std::ostream& operator<<(std::ostream& os, const inv_example::InvError& err) { os << err.to_string(); return os; };

