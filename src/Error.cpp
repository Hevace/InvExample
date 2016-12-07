// Error handling via exceptions
// with fault codes, error messages, and error levels

#include "Error.h"

using namespace std;
namespace InvExample {

  // ========================================
  // Error Table
  // ========================================
  const map<InvErrorCode, InvErrorInfo> InvError::ErrorTable = {
    { InvErrorCode::NO_ERROR, { InvErrorLevel::NO_ERROR, "No Error" } },
    { InvErrorCode::INVALID_MSG, { InvErrorLevel::FATAL, "Unable to decode a parsed message" } },
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

} // namespace InvExample
