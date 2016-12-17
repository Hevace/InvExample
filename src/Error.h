// Error types
// with fault codes, error messages, and error levels

#ifndef __ERROR_H__
#define __ERROR_H__

#include <stdexcept>
#include <map>
#include "timestamp.h"

namespace inv_example {
// ========================================
// Numeric error codes
// application defines error codes of this type
// ========================================
typedef int InvErrorCode;

// ========================================
// special error code values
// negative numbers and zero are used for special cases
// ========================================
enum class SpecialErrCode : InvErrorCode {
    NULL_ERROR = 0,
    EXCEPTION = -1
};

// ========================================
// Error Levels
// ========================================
enum class InvErrorLevel {
    NONE = 0,
    FATAL,      // the program will terminate
    WARNING,    // something wrong but program continues
    INFO        // informational, not necessarily a problem
};


// ========================================
// Error message object
// represents a single error condition
// ========================================
class InvError : std::exception
{
public: // constructors
    InvError::InvError(void) = delete;
    // Construct an error report that can be queued or thrown as an exception
    InvError::InvError(InvErrorCode code, int line, const std::string file)
        : m_time(),             // time is now
        m_code(code),
        m_line(line),
        m_file(file),
        m_exp_msg("")
    {};

    // Construct an error report for an exception thrown by the std library
    InvError::InvError(std::exception e, int line, const std::string file)
        : m_time(),             // time is now
        m_code(static_cast<InvErrorCode>(SpecialErrCode::EXCEPTION)),
        m_line(line),
        m_file(file),
        m_exp_msg(e.what())     // error message from the exception
    {};

public: // methods
    InvErrorCode get_code(void) const { return m_code; };
    std::string get_exp_msg(void) const { return m_exp_msg; };
    std::string to_string(void) const;

private: // data
    const InvTimestamp m_time;      // time of occurrence
    const InvErrorCode m_code;      // error identifier
    const int m_line;               // line number
    const std::string m_file;       // File name
    const std::string m_exp_msg;    // Exception message, or "" if not an exception
};


// ========================================
// Error message table
// lookup table containing severity level and error message for each code
// ========================================
class InvErrorTable
{
public: // types
    // ========================================
    // Error definition table entry
    // ========================================
    struct InvErrorInfo {
        InvErrorLevel level;    // severity of error
        std::string msg;        // error message
    };

    // ========================================
    // Error definition initializer
    // ========================================
    struct InvErrorInit {
        InvErrorCode code;      // error code
        InvErrorLevel level;    // severity of error
        std::string msg;        // error message
    };

public: // constructors
    InvErrorTable() {};                         // creates an empty error table
    InvErrorTable(std::initializer_list<const InvErrorInit> init);   // initializes the error table

public: // methods
    void AddErrorCode(std::initializer_list<const InvErrorInit> init);   // add error codes and messages to the table
    // static methods to look up error information
    InvErrorLevel LookupErrorLevel(const InvError& err) const;
    std::string LookupErrorMsg(const InvError& err) const;
    std::string to_string(const InvError& err) const;

private: // data
    std::map<int, InvErrorInfo> m_table;     // table of error codes and messages
};


// ========================================
// Error creation macro records the filename and line number
// ========================================
#define NewInvError(code) InvError::InvError((code), __LINE__, __FILE__)        // arg must be InvErrorCode
#define NewInvErrorException(e) InvError::InvError((e), __LINE__, __FILE__)     // arg must be std::exception or derivative

} // namespace inv_example


// ========================================
// formatted output of error messages
// ========================================
std::ostream& operator<<(std::ostream& os, const inv_example::InvError& err);

#endif // __ERROR_H__
