// Error handling via exceptions
// with fault codes, error messages, and error levels

#include "error.h"

using namespace std;
namespace inv_example {

// ========================================
// Error Table
// ========================================
const map<InvErrorCode, InvErrorInfo> InvError::ErrorTable = {
    { InvErrorCode::NONE, { InvErrorLevel::NONE, "No Error" } },
    { InvErrorCode::INVALID_MSG, { InvErrorLevel::FATAL, "Unable to decode a parsed message" } },
    { InvErrorCode::RESOURCE_ALLOCATION_FAILED, { InvErrorLevel::FATAL, "Unable to create or allocate a resource" } }
};


// ========================================
// Look up information in the Error Table
// ========================================
const InvErrorLevel InvError::LookupErrorLevel(InvErrorCode code)
{
    try {
        return ErrorTable.at(code).Level;
    }
    catch (out_of_range) {
        return InvErrorLevel::WARNING;          // undefined error codes return as a warning
    }
}


const std::string InvError::LookupErrorMsg(InvErrorCode code)
{
    try {
        return ErrorTable.at(code).Msg;
    }
    catch (out_of_range) {
        return "Undefined error";               // error code is not in the table
    }
}

} // namespace inv_example
