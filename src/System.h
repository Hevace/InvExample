// System-wide definitions

#include "Error.h"
#include "Ipc.h"

#ifndef __SYSTEM_H__
#define __SYSTEM_H__

namespace inv_example {
// ================================================================================
// System mode definitions
// ================================================================================
enum class SysMode {
    LOCKED,
    MOVING,
    HOLDING,
    FAILED
};


// ================================================================================
// Error Fault Codes
// each code corresponds to an entry in the Error Table
// ================================================================================
// communications errors
const InvErrorCode SYSERR_CART_FORCE_MSG_PARSE              = 1001;
const InvErrorCode SYSERR_CART_DATA_MSG_PARSE               = 1002;
const InvErrorCode SYSERR_CART_POLL_MSG_PARSE               = 1003;
const InvErrorCode SYSERR_CART_KEEPALIVE_MSG_PARSE          = 1004;
const InvErrorCode SYSERR_PEND_DATA_MSG_PARSE               = 1011;
// system resource allocation errors
const InvErrorCode SYSERR_RESOURCE_ALLOCATION_FAILED        = 5000;


// ================================================================================
// Global table of error messages
// ================================================================================
extern InvErrorTable g_sys_err_table;


// ================================================================================
// Report a system error
// ================================================================================
void enqueue_error(InvError& err);


} // namespace inv_example
#endif // __SYSTEM_H__
