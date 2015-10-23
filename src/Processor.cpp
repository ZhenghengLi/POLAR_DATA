#include "Processor.hpp"

Processor::Processor() {
	cnt.clear();
	t_file_out_ = NULL;
	t_event_tree_ = NULL;
	t_ped_event_tree_ = NULL;
	t_trigg_tree_ = NULL;
	t_ped_trigg_tree_ = NULL;
	b_trigg_index_ = 0;
	b_ped_trigg_index_ = 0;
	log_flag_ = false;
}

Processor::~Processor() {
	if (t_file_out_ != NULL)
		rootfile_close();
	if (os_logfile_.is_open())
		logfile_close();
}

void Processor::initialize() {	
	cnt.clear();
	b_trigg_index_ = 0;
	b_ped_trigg_index_ = 0;
	log_flag_ = false;
	if (t_file_out_ != NULL)
		rootfile_close();
	if (os_logfile_.is_open())
		logfile_close();
}

bool Processor::rootfile_open(const char* filename) {
	if (t_file_out_ != NULL)
		return false;
	// open file and tree
	t_file_out_ = new TFile(filename, "RECREATE");
	if (t_file_out_->IsZombie())
		return false;
	t_event_tree_ = new TTree("t_event", "event packet of 25 modules");
	t_event_tree_->SetDirectory(t_file_out_);
	t_ped_event_tree_ = new TTree("t_ped_event", "pedestal event packet of 25 modules");
	t_ped_event_tree_->SetDirectory(t_file_out_);
	t_trigg_tree_ = new TTree("t_trigg", "trigger packet with index");
	t_trigg_tree_->SetDirectory(t_file_out_);
	t_ped_trigg_tree_ = new TTree("t_ped_trigg", "pedestal trigger packet with index");
	t_ped_trigg_tree_->SetDirectory(t_file_out_);
	// set branch for each tree
	// for event
	t_event_tree_->Branch("trigg_index", &b_trigg_index_, "trigg_index/L");
	t_ped_event_tree_->Branch("trigg_index", &b_ped_trigg_index_, "trigg_index/L");
	t_event_tree_->Branch("mode", &b_event_mode_, "mode/I");
	t_ped_event_tree_->Branch("mode", &b_event_mode_, "mode/I");
	t_event_tree_->Branch("ct_num", &b_event_ct_num_, "ct_num/I");
	t_ped_event_tree_->Branch("ct_num", &b_event_ct_num_, "ct_num/I");
	t_event_tree_->Branch("timestamp", &b_event_timestamp_, "timestamp/i");
	t_ped_event_tree_->Branch("timestamp", &b_event_timestamp_, "timestamp/i");
	t_event_tree_->Branch("time_align", &b_event_time_align_, "time_align/i");
	t_ped_event_tree_->Branch("time_align", &b_event_time_align_, "time_align/i");
	t_event_tree_->Branch("trigger_bit", b_event_trigger_bit_, "trigger_bit[64]/O");
	t_ped_event_tree_->Branch("trigger_bit", b_event_trigger_bit_, "trigger_bit[64]/O");
	t_event_tree_->Branch("energy_ch", b_event_energy_ch_, "energy_ch[64]/I");
	t_ped_event_tree_->Branch("energy_ch", b_event_energy_ch_, "energy_ch[64]/I");
	t_event_tree_->Branch("rate", &b_event_rate_, "rate/I");
	t_ped_event_tree_->Branch("rate", &b_event_rate_, "rate/I");
	t_event_tree_->Branch("deadtime", &b_event_deadtime_, "deadtime/I");
	t_ped_event_tree_->Branch("deadtime", &b_event_deadtime_, "deadtime/I");
	t_event_tree_->Branch("common_noise", &b_event_common_noise_, "common_noise/I");
	t_ped_event_tree_->Branch("common_noise", &b_event_common_noise_, "common_noise/I");
	// for trigger
	t_trigg_tree_->Branch("trigg_index", &b_trigg_index_, "trigg_index/L");
	t_ped_trigg_tree_->Branch("trigg_index", &b_ped_trigg_index_, "trigg_index/L");
	t_trigg_tree_->Branch("mode", &b_trigg_mode_, "mode/I");
	t_ped_trigg_tree_->Branch("mode", &b_trigg_mode_, "mode/I");
	t_trigg_tree_->Branch("timestamp", &b_trigg_timestamp_, "timestamp/i");
	t_ped_trigg_tree_->Branch("timestamp", &b_trigg_timestamp_, "timestamp/i");
	t_trigg_tree_->Branch("time_align", &b_trigg_time_align_, "time_align/i");
	t_ped_trigg_tree_->Branch("time_align", &b_trigg_time_align_, "time_align/i");
	t_trigg_tree_->Branch("packet_num", &b_trigg_packet_num_, "packet_num/I");
	t_ped_trigg_tree_->Branch("packet_num", &b_trigg_packet_num_, "packet_num/I");
	t_trigg_tree_->Branch("trig_accepted", b_trigg_trig_accepted_, "trig_accepted[25]/O");
	t_ped_trigg_tree_->Branch("trig_accepted", b_trigg_trig_accepted_, "trig_accepted[25]/O");
	t_trigg_tree_->Branch("trig_rejected", b_trigg_trig_rejected_, "trig_rejected[25]/O");
	t_ped_trigg_tree_->Branch("trig_rejected", b_trigg_trig_rejected_, "trig_rejected[25]/O");
	// return 
	return true;
}

void Processor::rootfile_close() {
	t_event_tree_->Write();
	delete t_event_tree_;
	t_event_tree_ = NULL;
	t_ped_event_tree_->Write();
	delete t_ped_event_tree_;
	t_ped_event_tree_ = NULL;
	t_trigg_tree_->Write();
	delete t_trigg_tree_;
	t_trigg_tree_ = NULL;
	t_ped_trigg_tree_->Write();
	delete t_ped_trigg_tree_;
	t_ped_event_tree_ = NULL;
	t_file_out_->Close();
	delete t_file_out_;
	t_file_out_ = NULL;
}

void Processor::br_trigg_update_(const SciTrigger& trigger) {
	b_trigg_mode_ = static_cast<Int_t>(trigger.mode);
	b_trigg_timestamp_ = static_cast<UInt_t>(trigger.timestamp);
	b_trigg_time_align_ = static_cast<UInt_t>(trigger.time_align);
	b_trigg_packet_num_ = static_cast<Int_t>(trigger.packet_num);
	for (int i = 0; i < 25; i++) {
		if (trigger.trig_accepted[i] == 1)
			b_trigg_trig_accepted_[i] = kTRUE;
		else
			b_trigg_trig_accepted_[i] = kFALSE;
	}
	for (int i = 0; i < 25; i++) {
		if (trigger.trig_rejected[i] == 1)
			b_trigg_trig_rejected_[i] = kTRUE;
		else
			b_trigg_trig_rejected_[i] = kFALSE;
	}
}

void Processor::br_event_update_(const SciEvent& event) {
	b_event_mode_ = static_cast<Int_t>(event.mode);
	b_event_ct_num_ = static_cast<Int_t>(event.ct_num);
	b_event_timestamp_ = static_cast<UInt_t>(event.timestamp);
	b_event_time_align_ = static_cast<UInt_t>(event.time_align);
	for (int i = 0; i < 64; i++) {
		if (event.trigger_bit[i] == 1)
			b_event_trigger_bit_[i] = kTRUE;
		else
			b_event_trigger_bit_[i] = kFALSE;
	}
	for (int i = 0; i < 64; i++) {
		b_event_energy_ch_[i] = static_cast<Int_t>(event.energy_ch[i]);
	}
	b_event_rate_ = static_cast<Int_t>(event.rate);
	b_event_deadtime_ = static_cast<Int_t>(event.deadtime);
	b_event_common_noise_ = static_cast<Int_t>(event.common_noise);
}

void Processor::trigg_write_tree_(const SciTrigger& trigger) {
	br_trigg_update_(trigger);
	b_trigg_index_++;           // write trigger first!
	t_trigg_tree_->Fill();
}

void Processor::ped_trigg_write_tree_(const SciTrigger& trigger) {
	br_trigg_update_(trigger);
	b_ped_trigg_index_++;       // write trigger first!
	t_ped_trigg_tree_->Fill();
}

void Processor::event_write_tree_(const SciEvent& event) {
	br_event_update_(event);
	t_event_tree_->Fill();
}

void Processor::ped_event_write_tree_(const SciEvent& event) {
	br_event_update_(event);
	t_ped_event_tree_->Fill();
}

bool Processor::logfile_open(const char* filename) {
	os_logfile_.open(filename);
	return os_logfile_.is_open();
}

void Processor::logfile_close() {
	os_logfile_.close();
}

void Processor::set_log(bool flag) {
	log_flag_ = flag;
}

bool Processor::can_log() {
	return log_flag_ && os_logfile_.is_open();
}

bool Processor::process_frame(SciFrame& frame) {
	cnt.frame++;
	bool result = true;
	if (!frame.check_valid()) {
		if (can_log())
			os_logfile_ << "This frame is invalid! " << frame.get_index() << endl;
		result = false;
	} else if (!frame.check_crc()) {
		if (can_log())
			os_logfile_ << "frame CRC Error! " << frame.get_index() << endl;
		result = false;
	}
	if (!frame.can_connect()) {
		cnt.frm_con_error++;
		if (can_log())
			os_logfile_ << "frame connection error " << frame.get_index() << " " << cnt.frame << endl;
		if (!frame.find_start_pos()) {
			if (can_log())
				os_logfile_ << " find_start_pos error" << endl;
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
		if (can_log()) {
			os_logfile_ << endl;
			os_logfile_ << "packet invalid: " << cnt.packet << endl;
			frame.cur_print_packet(os_logfile_);
		}
        cnt.pkt_invalid++;
    }
    if (tmp_crc) {
        cnt.pkt_crc_passed++;
    } else {
		if (can_log()) {
			os_logfile_ << endl;
			os_logfile_ << "packet crc error: " << cnt.packet << endl;
			frame.cur_print_packet(os_logfile_);
		}
        cnt.pkt_crc_error++;
    }
    if (frame.get_cur_pkt_len() < 28) {
		if (can_log()) {
			os_logfile_ << "packet too short: " << cnt.packet << endl;
			frame.cur_print_packet(os_logfile_);
		}
        cnt.pkt_too_short++;
    }
    if (!(tmp_valid & tmp_crc))
        return;
    
    // start process packet
    if (is_trigger) {
        sci_trigger.update(frame.get_cur_pkt_buf(), frame.get_cur_pkt_len());
		if (sci_trigger.mode == 0x00F0) {
			for (int i = 0; i < 25; i++) {
				if (sci_trigger.trig_accepted[i] == 1)
					cnt.ped_trig[i]++;
			}
			if (can_log())
				sci_trigger.print(cnt, os_logfile_);
			ped_trigg_write_tree_(sci_trigger);			
		} else {
			for (int i = 0; i < 25; i++)
				if (sci_trigger.trig_accepted[i] == 1)
					cnt.noped_trig[i]++;
			if (can_log())
				sci_trigger.print(cnt, os_logfile_);
			trigg_write_tree_(sci_trigger);			
		}
    } else {
        sci_event.update(frame.get_cur_pkt_buf(), frame.get_cur_pkt_len());
		if (sci_event.mode == 2) {
			cnt.ped_event[sci_event.ct_num - 1]++;
			if (can_log())
				sci_event.print(cnt, os_logfile_);
			ped_event_write_tree_(sci_event);
		} else {
			cnt.noped_event[sci_event.ct_num - 1]++;
			if (can_log())
				sci_event.print(cnt, os_logfile_);
			event_write_tree_(sci_event);
		}
    }
}

