// Main loop

#include <iostream> // DEBUG

#include "Ipc.h"
#include "Messages.h"

using namespace std;

namespace inv_example {

// ================================================================================
// System mode definitions
// ================================================================================
enum class SysMode {
    MODE_LOCKED,
    MODE_MOVING,
    MODE_HOLDING,
    MODE_FAILED
};

// ================================================================================
// Main event loop
// ================================================================================
void main_loop(void)
{
    SysMode mode = SysMode::MODE_LOCKED;
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
        case SysMode::MODE_LOCKED:
            if (msg.GetId() == IpcMsgId::MSG_MOVE_CMD) {
                mode = SysMode::MODE_MOVING;
            }

            // DEBUG timer testing
            if (msg.GetId() == IpcMsgId::MSG_KEEPALIVE) {
                cout << "Tick " << dbg_count << endl;
                if (++dbg_count > 5) {
                    msgq.Send(debug_exit);
                }
            }
            break;

        case SysMode::MODE_MOVING:
            if (msg.GetId() == IpcMsgId::MSG_ARRIVED) {
                mode = SysMode::MODE_HOLDING;
            }
            break;

        case SysMode::MODE_HOLDING:
            switch (msg.GetId()) {
            case IpcMsgId::MSG_MOVE_CMD:
                mode = SysMode::MODE_MOVING;
                break;
            case IpcMsgId::MSG_RESET_CMD:
                mode = SysMode::MODE_LOCKED;
                break;
            }
            break;

        case SysMode::MODE_FAILED:
            break;

        default:
            mode = SysMode::MODE_FAILED;
            break;
        }

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
