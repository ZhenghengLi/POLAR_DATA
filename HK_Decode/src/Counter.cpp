#include "Counter.hpp"

using namespace std;

void Counter::clear() {
    frame = 0;
    frm_valid = 0;
    frm_invalid = 0;
    frm_crc_passed = 0;
    frm_crc_error = 0;
    frm_con_error = 0;
    obox_packet = 0;
    obox_valid = 0;
    obox_invalid = 0;
    obox_crc_passed = 0;
    obox_crc_error = 0;
}

void Counter::print(ostream& os) {
    cout << "=============================================================================================" << endl;
    cout << left;
    cout << setw(28) <<        "total frame count: " << setw(20) << frame
         << setw(28) <<  "total obox packet count: " << setw(20) << obox_packet << endl;
    cout << setw(28) <<        "frame valid count: " << setw(20) << frm_valid
         << setw(28) <<         "obox valid count: " << setw(20) << obox_valid << endl;
    cout << setw(28) <<      "frame invalid count: " << setw(20) << frm_invalid
         << setw(28) <<       "obox invalid count: " << setw(20) << obox_invalid << endl;
    cout << setw(28) <<         "frame crc passed: " << setw(20) << frm_crc_passed
         << setw(28) <<          "obox crc passed: " << setw(20) << obox_crc_passed << endl;
    cout << setw(28) <<    "frame crc error count: " << setw(20) << frm_crc_error
         << setw(28) <<     "obox crc error count: " << setw(20) << obox_crc_error << endl;
    cout << setw(28) << "frame interruption count: " << setw(20) << frm_con_error << endl;
    cout << right;
    cout << "---------------------------------------------------------------------------------------------" << endl;
}
