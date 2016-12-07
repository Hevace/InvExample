// Example program

#define NOMINMAX
#include <windows.h>

#include <iostream>
#include "comms.h"
#include "timestamp.h"

#include <iomanip>
#include <ctime>
#include <chrono>
#include <string>

using namespace std;
using namespace inv_example;

int main(int argc, char *argv[])
{
    vector<uint8_t> bad_length{ 0xaa, static_cast<uint8_t>(MsgID::ForceCmdMsgID), 1 };
    vector<uint8_t> bad_id{ 0xaa, 0xfe, 1, 0 };
    vector<uint8_t> force_cmd{ 0xaa, static_cast<uint8_t>(MsgID::ForceCmdMsgID), sizeof(double), 0xC0, 0x5E, 0xDC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCD };    // -123.45
    vector<uint8_t> bad_data{ 0xaa, static_cast<uint8_t>(MsgID::ForceCmdMsgID), sizeof(double), 0x5E, 0xDC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCD };           // missing 1st byte
    vector<uint8_t> cart_data{ 0xaa, static_cast<uint8_t>(MsgID::CartDataMsgID), 2 * sizeof(double), 0xC0, 0x5E, 0xDC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCD, 0x40, 0x09, 0x21, 0xFB, 0x4D, 0x12, 0xD8, 0x4A };  // -123.45, 3.1415926

    auto msg1 = CartForceCmd(force_cmd, InvTimestamp());

    //cout << "Force Cmd, Force = " << msg1.Force << ", Timestamp = " << msg1.GetToa() << endl;

    cout << "Steady Clock" << endl;
    cout << "min " << chrono::steady_clock::duration::min().count()
        << ", max " << chrono::steady_clock::duration::max().count()
        << ", " << (chrono::treat_as_floating_point<chrono::steady_clock::duration>::value ? "FP" : "integer") << endl;
    cout << (chrono::steady_clock::is_steady ? "steady" : "not steady") << endl;
    cout << (chrono::steady_clock::is_monotonic ? "monotonic" : "not monotonic") << endl;

      auto t1 = chrono::steady_clock::now();
      auto t2 = chrono::steady_clock::now();
      while (t2 == t1) t2 = chrono::steady_clock::now();
      auto int_ms = chrono::duration_cast<chrono::milliseconds>(t2 - t1);
      chrono::duration<double, milli> fp_ms = t2 - t1;
      cout << fp_ms.count() << " ms, " << int_ms.count() << " whole ms" << endl;

      chrono::duration<double, milli> fp_ms2 = t1.time_since_epoch();
      double frac = fp_ms2.count() - floor(fp_ms2.count());

      auto tnow = chrono::steady_clock::to_time_t(chrono::steady_clock::now());
      tm tmnow;
      localtime_s(&tmnow, &tnow);
      cout << "Now: " << put_time(&tmnow, "%c") << endl;
      cout << endl;

      cout << "Windows File Time" << endl;
      FILETIME filetime1, filetime2;
      GetSystemTimeAsFileTime(&filetime1);
      filetime2 = filetime1;
      while (filetime1.dwLowDateTime == filetime2.dwLowDateTime) GetSystemTimeAsFileTime(&filetime2);
      cout << "High Diff: " << filetime2.dwHighDateTime - filetime1.dwHighDateTime << endl;
      cout << "Low Diff: " << filetime2.dwLowDateTime - filetime1.dwLowDateTime << endl;
      cout << endl;

      cout << "Timestamp" << endl;
      auto ts = InvTimestamp();
      cout << ts.to_string() << endl;
      cout << ts << endl;

    return 0;
}
