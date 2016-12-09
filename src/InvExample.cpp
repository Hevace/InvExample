// Example program

#define NOMINMAX
#include <windows.h>

#include <iostream>
#include "comms.h"
#include "timestamp.h"
#include "ipc.h"

#include <iomanip>
#include <ctime>
#include <chrono>
#include <string>
#include <memory>

using namespace std;
using namespace inv_example;

int main(int argc, char *argv[])
{
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

    cout << "Queue" << endl;
    IpcQueue<IpcMsg> q;
    IpcMsg1Data msg1_data;
    msg1_data.i1 = 1;
    msg1_data.i2 = 2;
    IpcMsg msg1(1, msg1_data);
    q.Send(msg1);
    auto msg_out = q.Wait();
    auto msg_out_data = msg_out.GetMsg1Data();
    cout << "Sent " << msg1_data.i1 << "," << msg1_data.i1 << endl;
    cout << "Rcvd " << msg_out_data->i1 << "," << msg_out_data->i2 << endl;


    return 0;
}
