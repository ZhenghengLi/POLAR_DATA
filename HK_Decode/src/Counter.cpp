#include "Counter.hpp"

using namespace std;

void Counter::clear() {
    frame = 0;
    frm_valid = 0;
    frm_invalid = 0;
    frm_crc_passed = 0;
    frm_crc_error = 0;
    frm_con_error = 0;
}

void Counter::print(ostream& os) {
    cout << setw(28) << right <<        "total frame count: " << left << frame << endl;
    cout << setw(28) << right <<        "frame valid count: " << left << left << frm_valid << endl;
    cout << setw(28) << right <<      "frame invlaid count: " << left << frm_invalid << endl;
    cout << setw(28) << right <<   "frame crc passed count: " << left << frm_crc_passed << endl;
    cout << setw(28) << right <<    "frame crc error count: " << left << frm_crc_error << endl;
    cout << setw(28) << right << "frame interruption count: " << left << frm_con_error << endl;
    cout << right;
}
