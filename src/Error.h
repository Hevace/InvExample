// Error handling via exceptions
// with fault codes, error messages, and error levels

#ifndef __ERROR_H__
#define __ERROR_H__

#include <stdexcept>
#include <map>
#include "timestamp.h"

namespace inv_example {


// ========================================
// Error Fault Codes
// each code corresponds to an entry in the Error Table
// ========================================
enum class InvErrorCode {
    NONE = 0,
    INVALID_MSG,                    // invalid serial message
    RESOURCE_ALLOCATION_FAILED,     // class constructor failed
};


// ========================================
// Error Levels
// ========================================
enum class InvErrorLevel {
    NONE = 0,
    FATAL = 1,     // the program will terminate
    WARNING = 2,   // something wrong but program continues
    INFO = 3       // informational, not necessarily a problem
};


// ========================================
// Error definition table entry
// ========================================
struct InvErrorInfo
{
    InvErrorLevel Level;    // severity of error
    std::string Msg;        // error message

    InvErrorInfo(InvErrorLevel level, std::string msg) : Level(level), Msg(msg) {};
    InvErrorInfo() = delete;    // cannot construct empty error
};


// ========================================
// Error message object
// derived from a standard exception type
// ========================================
class InvError : public std::runtime_error
{
private:
    const InvTimestamp Time;    // time of occurrence
    const InvErrorCode Code;    // error identifier
    const InvErrorLevel Level;  // indicates severity
    const int Line;             // line number
    const std::string File;     // File name
    // the error message text is accessed through the what() method of the underlying exception type

    // table of error codes
    static const std::map<InvErrorCode, InvErrorInfo> ErrorTable;

public:
    InvError::InvError(InvErrorCode code, int line, const std::string file)
        : runtime_error(LookupErrorMsg(code)),          // store error message text in the base exception type
        Time(),                                         // time is now
        Code(code),
        Level(LookupErrorLevel(code)),
        Line(line),
        File(file)
    {};

    // static methods to look up error information
    static const InvErrorLevel LookupErrorLevel(InvErrorCode code);
    static const std::string LookupErrorMsg(InvErrorCode code);
};


// ========================================
// Error creation macro records the filename and line number
// ========================================
#define NewInvError(code) InvError::InvError(code, __LINE__, __FILE__)

} // namespace inv_example
#endif // __ERROR_H__
