#include "Processor.hpp"

Processor::Processor() {
	cnt.frame = 0;
	cnt.packet = 0;
	cnt.trigger = 0;
	cnt.event = 0;
	cnt.frm_con_error = 0;
	cnt.pkt_valid = 0;
	cnt.pkt_invalid = 0;
	cnt.pkt_crc_passed = 0;
	cnt.pkt_crc_error = 0;
	cnt.pkt_too_short = 0;
}

Processor::~Processor() {

}

void Processor::initialize() {	
	cnt.frame = 0;
	cnt.packet = 0;
	cnt.trigger = 0;
	cnt.event = 0;
	cnt.frm_con_error = 0;
	cnt.pkt_valid = 0;
	cnt.pkt_invalid = 0;
	cnt.pkt_crc_passed = 0;
	cnt.pkt_crc_error = 0;
	cnt.pkt_too_short = 0;
}

bool Processor::process_frame(SciFrame& frame) {
	cnt.frame++;
	bool result = true;
	if (!frame.check_valid()) {
		cout << "This frame is invalid! " << frame.get_index() << endl;
		result = false;
	} else if (!frame.check_crc()) {
		cout << "frame CRC Error! " << frame.get_index() << endl;
		result = false;
	}
	if (!frame.can_connect()) {
		cnt.frm_con_error++;
		cout << "frame connection error" << endl;
		if (!frame.find_start_pos()) {
			cout << " find_start_pos error" << endl;
			result = false;
		}
	}
	return result;
}

void Processor::process_packet(SciFrame& frame) {
    cnt.packet++;
    // check packet
    bool is_trigger = frame.cur_is_trigger();
    if (is_trigger) {
        cnt.trigger++;
    } else {
        cnt.event++;
    }
    bool tmp_valid = frame.cur_check_valid();
    bool tmp_crc = frame.cur_check_crc();
    if (tmp_valid) {
        cnt.pkt_valid++;
    } else {
        cout << endl;
        cout << "packet invalid: " << cnt.packet << endl;
        frame.cur_print_packet();
        cnt.pkt_invalid++;
    }
    if (tmp_crc) {
        cnt.pkt_crc_passed++;
    } else {
        cout << endl;
        cout << "packet crc error: " << cnt.packet << endl;
        frame.cur_print_packet();
        cnt.pkt_crc_error++;
    }
    if (frame.get_cur_pkt_len() < 28) {
        cout << "packet too short: " << cnt.packet << endl;
        frame.cur_print_packet();
        cnt.pkt_too_short++;
    }
    if (!(tmp_valid & tmp_crc))
        return;
    
    // start process packet
    if (is_trigger) {
        sci_trigger.update(frame.get_cur_pkt_buf(), frame.get_cur_pkt_len());
        sci_trigger.print(cnt);
    } else {
        sci_event.update(frame.get_cur_pkt_buf(), frame.get_cur_pkt_len());
        sci_event.print(cnt);
    }
}
