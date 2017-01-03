// Main loop

#include <iostream> // DEBUG
#include <string> // DEBUG
#include <iomanip> // DEBUG
#include <ctime> // DEBUG
#include <chrono> // DEBUG
#include <string> // DEBUG
#include <memory> // DEBUG

#include "System.h"
#include "Error.h"
#include "Ipc.h"
#include "Messages.h"

// DEBUG includes for test code in main
#include "Comms.h"    // DEBUG
#include "Timestamp.h"    // DEBUG


using namespace std;
using namespace inv_example;

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
            if (msg.GetId() == IpcMsgId::MSG_MOVE_CMD) {
                mode = SysMode::MOVING;
            }
            else if (msg.GetId() == IpcMsgId::MSG_RESET_CMD) {
                mode = SysMode::LOCKED;
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
            cout << g_sys_err_table.to_string(*m.second) << endl;       // DEBUG

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

} // namespace inv_example


// DEBUG dummy callback for testing timer
int dbg_count = 0;
void dbg_callback(void)
{
    dbg_count++;
}

// ================================================================================
// Program entry point
// initialization and main loop
// ================================================================================
int main(int argc, char *argv[])
{
    // DEBUG test code in main
    vector<uint8_t> bad_length{ 0xaa, static_cast<uint8_t>(PacketId::FORCE_CMD), 1 };
    vector<uint8_t> bad_id{ 0xaa, 0xfe, 1, 0 };
    vector<uint8_t> force_cmd{ 0xaa, static_cast<uint8_t>(PacketId::FORCE_CMD), sizeof(double), 0xC0, 0x5E, 0xDC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCD };    // -123.45
    vector<uint8_t> bad_data{ 0xaa, static_cast<uint8_t>(PacketId::FORCE_CMD), sizeof(double), 0x5E, 0xDC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCD };           // missing 1st byte
    vector<uint8_t> cart_data{ 0xaa, static_cast<uint8_t>(PacketId::CART_DATA), 2 * sizeof(double), 0xC0, 0x5E, 0xDC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCD, 0x40, 0x09, 0x21, 0xFB, 0x4D, 0x12, 0xD8, 0x4A };  // -123.45, 3.1415926

    auto msg = CartForceCmdPacket(force_cmd, InvTimestamp());
    cout << "Force Cmd, Force = " << msg.m_force << ", Timestamp = " << msg.get_toa() << endl;
    cout << endl;

    cout << "Timestamp" << endl;
    auto ts = InvTimestamp();
    cout << ts << endl;
    cout << endl;

    try {
        IpcHighResTimer hrt(10, dbg_callback);
    }
    catch (InvError e) {
        cout << "High Res Timer Error" << endl;
        cout << e << endl;
    }
    catch (exception e) {
        cout << "High Res Timer System Error" << endl;
        cout << e.what() << endl;
        InvError local_error(e, __LINE__, __FILE__);
        cout << local_error << endl;
    }

    // DEBUG add a test to print smallest timestamp increment

    // Run the system
    system_init();
    main_loop();


    // DEBUG test code in main
    cout << "dbg_count " << dbg_count << endl;
    auto tend = InvTimestamp();
    cout << tend << endl;
    cout << "elapsed " << tend - ts << endl;

    return 0;
}

