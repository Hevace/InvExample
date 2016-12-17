// Main loop

#include <iostream> // DEBUG
#include <string> // DEBUG

#include "System.h"
#include "Error.h"
#include "Ipc.h"
#include "Messages.h"

using namespace std;

namespace inv_example {
// ================================================================================
// System error codes and messages
// ================================================================================
initializer_list<const InvErrorTable::InvErrorInit> error_definitions = {
    // communications errors
    { SYSERR_CART_FORCE_MSG_PARSE,          InvErrorLevel::WARNING, "Unable to decode Cart Force msg" },
    { SYSERR_CART_DATA_MSG_PARSE,           InvErrorLevel::WARNING, "Unable to decode Cart Data msg" },
    { SYSERR_CART_POLL_MSG_PARSE,           InvErrorLevel::WARNING, "Unable to decode Cart Poll msg" },
    { SYSERR_CART_KEEPALIVE_MSG_PARSE,      InvErrorLevel::WARNING, "Unable to decode Cart Keepalive msg" },
    { SYSERR_PEND_DATA_MSG_PARSE,           InvErrorLevel::WARNING, "Unable to decode Pend Data msg" },
    // system resource allocation errors
    { SYSERR_RESOURCE_ALLOCATION_FAILED,    InvErrorLevel::FATAL,   "Unable to create or allocate a resource" },
};

// global storage for the error table
InvErrorTable g_sys_err_table{ error_definitions };


// ================================================================================
// System error queue
// ================================================================================
IpcQueue<InvError> g_sys_err_queue;


// ================================================================================
// Report a system error
// ================================================================================
void enqueue_error(InvError& err)
{
    g_sys_err_queue.Send(err);
}


// ================================================================================
// Main event loop
// ================================================================================
void main_loop(void)
{
    SysMode mode = SysMode::LOCKED;
    IpcQueue<IpcMsg> msgq;
    IpcMsg keepalive_msg(IpcMsgId::MSG_KEEPALIVE);
    IpcTimer<IpcMsg> keepalive(500, keepalive_msg, msgq);       // slow timeout timer

    // DEBUG timer testing
    int dbg_count = 0;
    IpcMsg debug_exit(IpcMsgId::MSG_EXIT);

    for (;;) {
        auto msg = msgq.Wait();         // block waiting for a message
        if (msg.GetId() == IpcMsgId::MSG_EXIT) break;   // quit the application

        switch (mode) {
        case SysMode::LOCKED:
            if (msg.GetId() == IpcMsgId::MSG_MOVE_CMD) {
                mode = SysMode::MOVING;
            }

            // DEBUG timer testing
            if (msg.GetId() == IpcMsgId::MSG_KEEPALIVE) {
                cout << "Tick " << dbg_count << endl;
                if (++dbg_count > 5) {
                    msgq.Send(debug_exit);
                }
            }
            break;

        case SysMode::MOVING:
            if (msg.GetId() == IpcMsgId::MSG_ARRIVED) {
                mode = SysMode::HOLDING;
            }
            break;

        case SysMode::HOLDING:
            switch (msg.GetId()) {
            case IpcMsgId::MSG_MOVE_CMD:
                mode = SysMode::MOVING;
                break;
            case IpcMsgId::MSG_RESET_CMD:
                mode = SysMode::LOCKED;
                break;
            }
            break;

        case SysMode::FAILED:
            break;

        default:
            mode = SysMode::FAILED;
            break;
        } // message processing

        // process errors in the queue
        auto m = g_sys_err_queue.TryGet();
        while (m.first) {
            // TODO log errors to file
            // TODO display errors on console
            cout << g_sys_err_table.to_string(*m.second) << endl;

            // quit on a fatal error
            if (g_sys_err_table.LookupErrorLevel(*m.second) == InvErrorLevel::FATAL) {
                msgq.Send(IpcMsg(IpcMsgId::MSG_EXIT));          // send a message to terminate the system
            }

            // try to get the next error
            m = g_sys_err_queue.TryGet();
        } // error processing
    }   // main loop
}


// ================================================================================
// System initialization
// ================================================================================
void system_init(void)
{

}


// ================================================================================
// Program entry point
// initialization and main loop
// ================================================================================
void entry_point(void)
{
    system_init();
    main_loop();
}

} // namespace inv_example
