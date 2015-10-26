#include "Counter.hpp"

void Counter::clear() {
    frame = 0;
    packet = 0;
    trigger = 0;
    event = 0;
    frm_con_error = 0;
    pkt_valid = 0;
    pkt_invalid = 0;
    pkt_crc_passed = 0;
    pkt_crc_error = 0;
    pkt_too_short = 0;
    for (int i = 0; i < 25; i++) {
        ped_trig[i] = 0;
        ped_event[i] = 0;
        noped_trig[i] = 0;
        noped_event[i] = 0;
    }   
}

void Counter::print() {
	const int mod_id[25] = {405, 639, 415, 522, 424,
							640, 408, 638, 441, 631,
							411, 505, 503, 509, 410,
							507, 402, 602, 414, 524,
							423, 601, 406, 520, 413};
	cout << "===============================================================================================" << endl;
	cout << left;
    cout << setw(32) << "frame count: " << frame << endl;
    cout << setw(32) << "packet count: " << packet << endl;
    cout << setw(32) << "trigger count: " << trigger << endl;
    cout << setw(32) << "event count: " << event << endl;
    cout << setw(32) << "packet valid count: " << pkt_valid << endl;
    cout << setw(32) << "packet invalid count: " << pkt_invalid << endl;
    cout << setw(32) << "packet crc pass count: " << pkt_crc_passed << endl;
    cout << setw(32) << "packet crc err count: " << pkt_crc_error << endl;
    cout << setw(32) << "small length count: " << pkt_too_short << endl;
    cout << setw(32) << "frame connection error count: " << frm_con_error << endl;
	cout << right;
    cout << "-----------------------------------------------------------------------------------------------" << endl;
    int64_t trigg_sum = 0;
    int64_t event_sum = 0;
    cout << setfill(' ');
	cout << setw(4) << "ct"
		 << setw(5) << "mod"
		 << setw(3) << " | "
		 << setw(13) << "ped_trig"
         << setw(13) << "ped_event"
         << setw(13) << "ped_lost"
         << setw(5) << "   | "
         << setw(13) << "noped_trig"
         << setw(13) << "noped_event"
         << setw(13) << "noped_lost"
         << endl;
            
    for (int i = 0; i < 25; i++) {
		cout << setw(4) << i + 1
			 << setw(5) << mod_id[i]
			 << setw(3) << " | "
			 << setw(13) << ped_trig[i]
             << setw(13) << ped_event[i]
             << setw(13) << ped_trig[i] - ped_event[i]
             << setw(5) << "   | "
             << setw(13) << noped_trig[i]
             << setw(13) << noped_event[i]
             << setw(13) << noped_trig[i] - noped_event[i]
             << endl;
        trigg_sum += (ped_trig[i] + noped_trig[i]);
        event_sum += (ped_event[i] + noped_event[i]);
    }
    cout << "-----------------------------------------------------------------------------------------------" << endl;
	cout << left;
    cout << setw(12) << "trigg_sum: " << trigg_sum << endl;
    cout << setw(12) << "event_sum: " << event_sum << endl;
	cout << right;
	cout << "===============================================================================================" << endl;
}
