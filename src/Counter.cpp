#include "Counter.hpp"

void Counter::clear() {
    frame = 0;
    packet = 0;
    trigger = 0;
	ped_trigger = 0;
	noped_trigger = 0;
    event = 0;
    frm_con_error = 0;
	frm_valid = 0;
	frm_invalid = 0;
	frm_crc_passed = 0;
	frm_crc_error = 0;
	frm_start_error = 0;
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
	char str_buffer1[50];
	char str_buffer2[50];
	cout << "===========================================================================================================" << endl;
	cout << left;
	cout << setfill(' ') << fixed << setprecision(2);
    cout << setw(28) << "total frame count: " <<setw(20) << frame
		 << setw(28) << "total packet count: " << setw(10) << packet << endl;
	cout << setw(28) << "frame invalid count: " << setw(20) << frm_invalid
		 << setw(28) << "- trigger packet count: " << setw(10) << trigger << endl;
	sprintf(str_buffer1, "%4.2f%%", static_cast<double>(frm_invalid) / frame * 100);
	cout << setw(28) << "frame invalid percent: " << setw(20) << str_buffer1
		 << setw(28) << "- event packet count: " << setw(10) << event << endl;
	cout << setw(28) << "frame crc error count: " << setw(20) << frm_crc_error
		 << setw(28) << "packet invalid count: " << setw(10) << pkt_invalid << endl;
	sprintf(str_buffer1, "%4.2f%%", static_cast<double>(frm_crc_error) / frame * 100);
	sprintf(str_buffer2, "%4.2f%%", static_cast<double>(pkt_invalid) / packet * 100);
	cout << setw(28) << "frame crc error percent: " << setw(20) << str_buffer1
		 << setw(28) << "packet invalid percent: " << setw(10) << str_buffer2 << endl;
	cout << setw(28) << "frame interruption count: " << setw(20) << frm_con_error
		 << setw(28) << "packet crc error count: " << setw(10) << pkt_crc_error << endl;
	sprintf(str_buffer2, "%4.2f%%", static_cast<double>(pkt_crc_error) / packet * 100);
	cout << setw(28) << "frame start error count: " << setw(20) << frm_start_error
		 << setw(28) << "packet crc error percent: " << setw(10) << str_buffer2 << endl;
	cout << setw(28) << " " << setw(20) << " "
	     << setw(28) << "packet too short count: " << setw(20) << pkt_too_short << endl;
	cout << right;
    cout << "-----------------------------------------------------------------------------------------------------------" << endl;
    int64_t trigg_sum = 0;
    int64_t event_sum = 0;
	cout << setw(4) << "ct"
		 << setw(5) << "mod"
		 << setw(3) << " >"
		 << setw(11) << "ped_trig"
         << setw(11) << "ped_event"
         << setw(11) << "ped_lost"
		 << setw(9) << "percent"
         << setw(5) << "   |"
         << setw(13) << "noped_trig"
         << setw(13) << "noped_event"
         << setw(13) << "noped_lost"
		 << setw(9) << "percent"
         << endl;
    for (int i = 0; i < 25; i++) {
		cout << setw(4) << i + 1
			 << setw(5) << ModuleIndex[i]
			 << setw(3) << " >"
			 << setw(11) << ped_trig[i]
             << setw(11) << ped_event[i]
             << setw(11) << ped_trig[i] - ped_event[i]
			 << setw(8) << static_cast<double>(ped_trig[i] - ped_event[i]) / ped_trig[i] * 100 << "%"
             << setw(5) << "   |"
             << setw(13) << noped_trig[i]
             << setw(13) << noped_event[i]
             << setw(13) << noped_trig[i] - noped_event[i]
			 << setw(8) << static_cast<double>(noped_trig[i] - noped_event[i]) / noped_trig[i] * 100 << "%"
             << endl;
        trigg_sum += (ped_trig[i] + noped_trig[i]);
        event_sum += (ped_event[i] + noped_event[i]);
    }
    cout << "-----------------------------------------------------------------------------------------------------------" << endl;
	cout << left;
    cout << setw(21) << "trigg expected sum: " << setw(20) << trigg_sum 
		 << setw(18) << "ped_trigger: " << setw(20) << ped_trigger << endl;
    cout << setw(21) << "event received sum: " << setw(20) << event_sum 
		 << setw(18) << "noped_trigger: " << setw(20) << noped_trigger << endl;
	sprintf(str_buffer1, "%4.2f%%", static_cast<double>(trigg_sum - event_sum) / trigg_sum * 100);
	sprintf(str_buffer2, "%d counts/sec", static_cast<int>(noped_trigger / (ped_trigger * 1)));
	cout << setw(21) << "total lost percent: " << setw(20) << str_buffer1
		 << setw(18) << "mean event rate: "  << setw(20) << str_buffer2 << endl;
	cout << right;
	cout << "===========================================================================================================" << endl;
}
