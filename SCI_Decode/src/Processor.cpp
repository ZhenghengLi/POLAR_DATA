#include "Processor.hpp"

using namespace std;

Processor::Processor() {
    initialize();
}

Processor::~Processor() {
    if (os_logfile_.is_open())
        logfile_close();
}

void Processor::initialize() {  
    cnt.clear();
    log_flag_ = false;
    pre_ped_trigg_time_ = 0;
    start_flag_ = true;
    if (os_logfile_.is_open())
        logfile_close();

    cur_trigg_num_g_ = 0;
    cur_trigg_pre_is_bad_ = 0;
    cur_trigg_pre_time_stamp_ = 0;
    cur_trigg_time_period_ = 0;
    cur_trigg_pre_raw_dead_ = 0;
    cur_trigg_is_first_ = true;

    for (int i = 0; i < 25; i++) {
        cur_event_num_g_[i] = 0;
        cur_event_pre_is_bad_[i] = 0;
        cur_event_pre_time_stamp_[i] = 0;
        cur_event_time_period_[i] = 0;
        cur_event_pre_raw_dead_[i] = 0;
        cur_event_is_first_[i] = true;
    }
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

bool Processor::interruption_occurred(SciFrame& frame) {
    if (frame.can_connect()) {
        return false;
    } else {
        cnt.frm_con_error++;
        if (can_log()) {
            os_logfile_ << "## FRAME: ";
            os_logfile_ << cnt.frame << " (" << frame.get_index() << ") ";
            os_logfile_ << "| INTERRUPTION " << "########" << endl;
        }
        return true;
    }
}

bool Processor::process_start(SciFrame& frame, SciDataFile& datafile) {
    if (frame.find_start_pos()) {
        while (frame.next_packet())
            process_packet(frame, datafile);
        return true;
    } else {
        cnt.frm_start_error++;
        if (can_log()) {
            os_logfile_ << "## FRAME: ";
            os_logfile_ << cnt.frame << " (" << frame.get_index() << ") ";
            os_logfile_ << "| START_ERROR " << "########" << endl;
        }
        return false;
    }
}

bool Processor::process_restart(SciFrame& frame, SciDataFile& datafile) {
    do_the_last_work(datafile);
    evtMgr_.all_clear();
    frame.reset();
    return process_start(frame, datafile);
}

bool Processor::process_frame(SciFrame& frame) {
    cnt.frame++;
    bool frm_valid = frame.check_valid();
    if (frm_valid) {
        cnt.frm_valid++;
    } else {
        cnt.frm_invalid++;
    }
    bool frm_crc = frame.check_crc();
    if (frm_crc) {
        cnt.frm_crc_passed++;
    } else {
        cnt.frm_crc_error++;
    }

    if (!frm_valid || !frm_crc) {
        if (can_log()) {
            os_logfile_ << "## FRAME: ";
            os_logfile_ << cnt.frame << " (" << frame.get_index() << ") ";
            if (!frm_valid)
                os_logfile_ << "| INVALID ";
            if (!frm_crc)
                os_logfile_ << "| CRC_ERROR ";
            os_logfile_ << "########" << endl;
        }
        return false;
    } else {
        frame.update_time();
        return true;
    }
}

void Processor::process_packet(SciFrame& frame, SciDataFile& datafile) {
    cnt.packet++;
    // check packet
    if (frame.get_cur_pkt_len() < 4) {
        return;
    }
    bool is_trigger = frame.cur_is_trigger();
    // check bad and update and add extra info for this packet
    if (is_trigger) {
        cnt.trigger++;
        sci_trigger.set_frm_time(frame.get_ship_time(), frame.get_gps_time());                    
        if (frame.get_cur_pkt_len() < 50) {
            cnt.pkt_too_short++;
            if (can_log()) {
                os_logfile_ << "== PACKET: " << cnt.packet << " | TOO_SHORT ======== " << endl;
                os_logfile_ << "--------------------------------------------------------------" << endl;
                frame.cur_print_packet(os_logfile_);
                os_logfile_ << "--------------------------------------------------------------" << endl;
            }
            sci_trigger.clear_all_info();
            sci_trigger.is_bad      = 3;
            sci_trigger.pre_is_bad  = cur_trigg_pre_is_bad_;
            cur_trigg_pre_is_bad_   = sci_trigger.is_bad;
            sci_trigger.mode        = frame.cur_get_mode();
            if (sci_trigger.mode == 0x00F0) {
                datafile.write_ped_trigger_alone(sci_trigger);
            } else {
                datafile.write_trigger_alone(sci_trigger);
            }
            return;
        } else {
            
            sci_trigger.update(frame.get_cur_pkt_buf(), frame.get_cur_pkt_len());
            sci_trigger.trigg_num_g = 0;
            sci_trigger.time_period = 0;
            sci_trigger.time_wait   = 0;
            sci_trigger.dead_ratio  = 0;
            
            if (!frame.cur_check_valid()) {
                cnt.pkt_invalid++;
                if (can_log()) {
                    os_logfile_ << "== PACKET: " << cnt.packet << " | INVALID ======== " << endl;
                    os_logfile_ << "--------------------------------------------------------------" << endl;
                    frame.cur_print_packet(os_logfile_);
                    os_logfile_ << "--------------------------------------------------------------" << endl;
                }
                sci_trigger.is_bad      = 2;
                sci_trigger.pre_is_bad  = cur_trigg_pre_is_bad_;
                cur_trigg_pre_is_bad_   = sci_trigger.is_bad;
                if (sci_trigger.mode == 0x00F0) {
                    datafile.write_ped_trigger_alone(sci_trigger);
                } else {
                    datafile.write_trigger_alone(sci_trigger);
                }
                return;
            } else {
                cnt.pkt_valid++;
                if (!frame.cur_check_crc()) {
                    cnt.pkt_crc_error++;
                    if (can_log()) {
                        os_logfile_ << "== PACKET: " << cnt.packet << " | CRC_ERROR ======== " << endl;
                        os_logfile_ << "--------------------------------------------------------------" << endl;
                        frame.cur_print_packet(os_logfile_);
                        os_logfile_ << "--------------------------------------------------------------" << endl;
                    }
                    sci_trigger.is_bad      = 1;
                    sci_trigger.pre_is_bad  = cur_trigg_pre_is_bad_;
                    cur_trigg_pre_is_bad_   = sci_trigger.is_bad;
                    if (sci_trigger.mode == 0x00F0) {
                        datafile.write_ped_trigger_alone(sci_trigger);
                    } else {
                        datafile.write_trigger_alone(sci_trigger);
                    }
                    return;
                } else {
                    cnt.pkt_crc_passed++;
                    sci_trigger.is_bad      = 0;
                    sci_trigger.pre_is_bad  = cur_trigg_pre_is_bad_;
                    cur_trigg_pre_is_bad_   = sci_trigger.is_bad;
                    if (cur_trigg_is_first_) {
                        cur_trigg_is_first_ = false;
                        cur_trigg_pre_time_stamp_ = sci_trigger.timestamp;
                        cur_trigg_time_period_    = 0;
                        sci_trigger.time_period   = cur_trigg_time_period_;
                        sci_trigger.time_wait     = 0;
                        cur_trigg_pre_raw_dead_   = static_cast<int32_t>(sci_trigger.deadtime);
                        cur_trigg_num_g_ = 0;
                        sci_trigger.trigg_num_g   = cur_trigg_num_g_;
                        cur_trigg_num_g_++;
                        sci_trigger.dead_ratio    = 0;
                    } else {
                        int64_t tmp_time_wait = static_cast<int64_t>(sci_trigger.timestamp) - static_cast<int64_t>(cur_trigg_pre_time_stamp_);
                        cur_trigg_pre_time_stamp_ = sci_trigger.timestamp;
                        if (tmp_time_wait < -1 * PedCircle * LSB_Value) {
                            tmp_time_wait += 4294967296;
                            cur_trigg_time_period_++;
                        }
                        sci_trigger.time_period  = cur_trigg_time_period_;
                        sci_trigger.time_wait    = static_cast<uint32_t>(tmp_time_wait);
                        int32_t tmp_dead_diff = static_cast<int32_t>(sci_trigger.deadtime) - cur_trigg_pre_raw_dead_;
                        cur_trigg_pre_raw_dead_ = static_cast<int32_t>(sci_trigger.deadtime);
                        tmp_dead_diff = (tmp_dead_diff > 0 ? tmp_dead_diff : tmp_dead_diff + 65536);
                        sci_trigger.dead_ratio = static_cast<float>(static_cast<double>(tmp_dead_diff) / static_cast<double>(sci_trigger.time_wait));
                        sci_trigger.trigg_num_g = cur_trigg_num_g_;
                        cur_trigg_num_g_++;
                    }
                }
            }
        }
    } else {
        cnt.event++;
        int idx = frame.cur_get_ctNum() - 1;
        if (idx < 0) {
            return;
        }
        if (frame.get_cur_pkt_len() < 28) {
            cnt.pkt_too_short++;
            if (can_log()) {
                os_logfile_ << "== PACKET: " << cnt.packet << " | TOO_SHORT ======== " << endl;
                os_logfile_ << "--------------------------------------------------------------" << endl;
                frame.cur_print_packet(os_logfile_);
                os_logfile_ << "--------------------------------------------------------------" << endl;
            }
            sci_event.clear_all_info();
            sci_event.is_bad            = 3;
            sci_event.pre_is_bad        = cur_event_pre_is_bad_[idx];
            cur_event_pre_is_bad_[idx]  = sci_event.is_bad;
            sci_event.ct_num            = idx + 1;
            if (frame.get_cur_pkt_len() < 8) {
                sci_event.mode = 4;
                datafile.write_modules_alone(sci_event);
            } else {
                sci_event.mode = frame.cur_get_mode();
                if (sci_event.mode == 2) {
                    datafile.write_ped_modules_alone(sci_event);
                } else {
                    datafile.write_modules_alone(sci_event);
                }
            }
            return;
        } else {
            sci_event.mode = frame.cur_get_mode();
            if (sci_event.mode == 0 || sci_event.mode == 2) {
                if (frame.get_cur_pkt_len() < 122) {
                    cnt.pkt_too_short++;
                    if (can_log()) {
                        os_logfile_ << "== PACKET: " << cnt.packet << " | TOO_SHORT for mode 0 or 2 ======== " << endl;
                        os_logfile_ << "--------------------------------------------------------------" << endl;
                        frame.cur_print_packet(os_logfile_);
                        os_logfile_ << "--------------------------------------------------------------" << endl;
                    }
                    sci_event.clear_all_info();
                    sci_event.is_bad            = 3;
                    sci_event.pre_is_bad        = cur_event_pre_is_bad_[idx];
                    cur_event_pre_is_bad_[idx]  = sci_event.is_bad;
                    sci_event.ct_num            = idx + 1;
                    if (sci_event.mode == 2) {
                        datafile.write_ped_modules_alone(sci_event);
                    } else {
                        datafile.write_modules_alone(sci_event);
                    }
                    return;
                }
            }
            
            sci_event.update(frame.get_cur_pkt_buf(), frame.get_cur_pkt_len());
            sci_event.event_num_g = 0;
            sci_event.time_period = 0;
            sci_event.time_wait   = 0;
            sci_event.dead_ratio  = 0;

            if (!frame.cur_check_valid()) {
                cnt.pkt_invalid++;
                if (can_log()) {
                    os_logfile_ << "== PACKET: " << cnt.packet << " | INVALID ======== " << endl;
                    os_logfile_ << "--------------------------------------------------------------" << endl;
                    frame.cur_print_packet(os_logfile_);
                    os_logfile_ << "--------------------------------------------------------------" << endl;
                }
                sci_event.is_bad            = 2;
                sci_event.pre_is_bad        = cur_event_pre_is_bad_[idx];
                cur_event_pre_is_bad_[idx]  = sci_event.is_bad;
                if (sci_event.mode == 2) {
                    datafile.write_ped_modules_alone(sci_event);
                } else {
                    datafile.write_modules_alone(sci_event);
                }
                return;
            } else {
                cnt.pkt_valid++;
                if (!frame.cur_check_crc()) {
                    cnt.pkt_crc_error++;
                    if (can_log()) {
                        os_logfile_ << "== PACKET: " << cnt.packet << " | CRC_ERROR ======== " << endl;
                        os_logfile_ << "--------------------------------------------------------------" << endl;
                        frame.cur_print_packet(os_logfile_);
                        os_logfile_ << "--------------------------------------------------------------" << endl;
                    }
                    sci_event.is_bad            = 1;
                    sci_event.pre_is_bad        = cur_event_pre_is_bad_[idx];
                    cur_event_pre_is_bad_[idx]  = sci_event.is_bad;
                    if (sci_event.mode == 2) {
                        datafile.write_ped_modules_alone(sci_event);
                    } else {
                        datafile.write_modules_alone(sci_event);
                    }
                    return;
                } else {
                    cnt.pkt_crc_passed++;
                    sci_event.is_bad            = 0;
                    sci_event.pre_is_bad        = cur_event_pre_is_bad_[idx];
                    cur_event_pre_is_bad_[idx]  = sci_event.is_bad;
                    if (cur_event_is_first_[idx]) {
                        cur_event_is_first_[idx]        = false;
                        cur_event_pre_time_stamp_[idx]  = sci_event.timestamp;
                        cur_event_time_period_[idx]     = 0;
                        sci_event.time_period           = cur_event_time_period_[idx];
                        sci_event.time_wait             = 0;
                        cur_event_pre_raw_dead_[idx]    = static_cast<int32_t>(sci_event.deadtime);
                        cur_event_num_g_[idx]           = 0;
                        sci_event.event_num_g           = cur_event_num_g_[idx];
                        cur_event_num_g_[idx]++;
                        sci_event.dead_ratio            = 0;
                    } else {
                        int64_t tmp_time_wait = static_cast<int64_t>(sci_event.timestamp) - static_cast<int64_t>(cur_event_pre_time_stamp_[idx]);
                        cur_event_pre_time_stamp_[idx] = sci_event.timestamp;
                        if (tmp_time_wait < -1 * PedCircle) {
                            tmp_time_wait += 16777216;
                            cur_event_time_period_[idx]++;
                        }
                        sci_event.time_period  = cur_event_time_period_[idx];
                        sci_event.time_wait    = static_cast<uint32_t>(tmp_time_wait);
                        int32_t tmp_dead_diff = static_cast<int32_t>(sci_event.deadtime) - cur_event_pre_raw_dead_[idx];
                        cur_event_pre_raw_dead_[idx] = static_cast<int32_t>(sci_event.deadtime);
                        tmp_dead_diff = (tmp_dead_diff > 0 ? tmp_dead_diff : tmp_dead_diff + 65536);
                        sci_event.dead_ratio = static_cast<float>(static_cast<double>(tmp_dead_diff) / static_cast<double>(sci_event.time_wait));
                        sci_event.event_num_g = cur_event_num_g_[idx];
                        cur_event_num_g_[idx]++;
                    }
                }
            }
        }
    }
    
    // start process packet
    if (is_trigger) {
        if (sci_trigger.mode == 0x00F0) {
            cnt.ped_trigger++;
            if (start_flag_) {
                start_flag_ = false;
                pre_ped_trigg_time_ = sci_trigger.time_align;
            } else {
                int time_diff = sci_trigger.time_align - pre_ped_trigg_time_;
                if (time_diff < 0)
                    time_diff += CircleTime;
                if (time_diff < PedCircle / 2)
                    cnt.tin_ped_trigger++;
                else
                    cnt.sec_ped_trigger++;
                pre_ped_trigg_time_ = sci_trigger.time_align;
            }
            for (int i = 0; i < 25; i++) {
                if (sci_trigger.trig_accepted[i] == 1)
                    cnt.ped_trig[i]++;
            }
            // ===========================================
            evtMgr_.add_ped_trigger(sci_trigger);
            if (evtMgr_.ped_check_valid()) {
                evtMgr_.ped_move_result(true);
                evtMgr_.ped_update_time_diff();
                while (!evtMgr_.alone_ped_queue.empty()) {
                    datafile.write_ped_modules_alone(evtMgr_.alone_ped_queue.front());
                    evtMgr_.alone_ped_queue.pop();
                }
                datafile.write_ped_event_align(evtMgr_.get_result_ped_trigger(), evtMgr_.get_result_ped_events_vec());
                evtMgr_.ped_clear_result();
            } else {
                evtMgr_.ped_move_result(false);
                while (!evtMgr_.alone_ped_queue.empty()) {
                    datafile.write_ped_modules_alone(evtMgr_.alone_ped_queue.front());
                    evtMgr_.alone_ped_queue.pop();
                }
                if (evtMgr_.get_result_ped_events_vec().empty()) {
                    if (evtMgr_.get_result_ped_trigger().is_bad == 0) {
                        datafile.write_ped_trigger_alone(evtMgr_.get_result_ped_trigger());
                    }   
                } else {
                    if (evtMgr_.get_result_ped_trigger().is_bad == 0) {
                        datafile.write_ped_event_align(evtMgr_.get_result_ped_trigger(), evtMgr_.get_result_ped_events_vec());
                    } else {
                        for (vector<SciEvent>::const_iterator vecItr = evtMgr_.get_result_ped_events_vec().begin();
                             vecItr != evtMgr_.get_result_ped_events_vec().end(); vecItr++) {
                            datafile.write_ped_modules_alone(*vecItr);
                        }   
                    }   
                    evtMgr_.ped_clear_result();                        
                }   
            }
            // -------------------------------------------
        } else {
            cnt.noped_trigger++;
            for (int i = 0; i < 25; i++)
                if (sci_trigger.trig_accepted[i] == 1)
                    cnt.noped_trig[i]++;
            // ===========================================
            evtMgr_.add_noped_trigger(sci_trigger);
            if (evtMgr_.noped_do_merge()) {
                while (!evtMgr_.before_lost_queue.empty()) {
                    datafile.write_modules_alone(evtMgr_.before_lost_queue.front());
                    evtMgr_.before_lost_queue.pop();
                }
                if (evtMgr_.get_result_noped_events_vec().empty()) {
                    datafile.write_trigger_alone(evtMgr_.get_result_noped_trigger());
                } else {
                    datafile.write_event_align(evtMgr_.get_result_noped_trigger(), evtMgr_.get_result_noped_events_vec());
                    cnt.aligned_event_sum += evtMgr_.get_result_noped_trigger().get_pkt_count();
                    evtMgr_.noped_clear_result();
                }
            }
            // -------------------------------------------
        }
    } else {
        if (sci_event.mode == 2) {
            cnt.ped_event[sci_event.ct_num - 1]++;
            // ===========================================
            evtMgr_.add_ped_event(sci_event);
            // -------------------------------------------
        } else {
            cnt.noped_event[sci_event.ct_num - 1]++;
            // ===========================================
            evtMgr_.add_noped_event(sci_event);
            if (evtMgr_.noped_do_merge()) {
                while (!evtMgr_.before_lost_queue.empty()) {
                    datafile.write_modules_alone(evtMgr_.before_lost_queue.front());
                    evtMgr_.before_lost_queue.pop();
                }
                if (evtMgr_.get_result_noped_events_vec().empty()) {
                    datafile.write_trigger_alone(evtMgr_.get_result_noped_trigger());
                } else {
                    datafile.write_event_align(evtMgr_.get_result_noped_trigger(), evtMgr_.get_result_noped_events_vec());
                    cnt.aligned_event_sum += evtMgr_.get_result_noped_trigger().get_pkt_count();
                    evtMgr_.noped_clear_result();
                }
            }
            // -------------------------------------------
        }
    }
}

void Processor::do_the_last_work(SciDataFile& datafile) {
    // ===========================================
    if (evtMgr_.ped_check_valid()) {
        evtMgr_.ped_move_result(true);
        evtMgr_.ped_update_time_diff();
        while (!evtMgr_.alone_ped_queue.empty()) {
            datafile.write_ped_modules_alone(evtMgr_.alone_ped_queue.front());
            evtMgr_.alone_ped_queue.pop();
        }
        datafile.write_ped_event_align(evtMgr_.get_result_ped_trigger(), evtMgr_.get_result_ped_events_vec());
        evtMgr_.ped_clear_result();
    } else {
        evtMgr_.ped_move_result(false);
        while (!evtMgr_.alone_ped_queue.empty()) {
            datafile.write_ped_modules_alone(evtMgr_.alone_ped_queue.front());
            evtMgr_.alone_ped_queue.pop();
        }
        if (evtMgr_.get_result_ped_events_vec().empty()) {
            if (evtMgr_.get_result_ped_trigger().is_bad == 0) {
                datafile.write_ped_trigger_alone(evtMgr_.get_result_ped_trigger());
            }   
        } else {
            if (evtMgr_.get_result_ped_trigger().is_bad == 0) {
                datafile.write_ped_event_align(evtMgr_.get_result_ped_trigger(), evtMgr_.get_result_ped_events_vec());
            } else {
                for (vector<SciEvent>::const_iterator vecItr = evtMgr_.get_result_ped_events_vec().begin();
                     vecItr != evtMgr_.get_result_ped_events_vec().end(); vecItr++) {
                    datafile.write_ped_modules_alone(*vecItr);
                }   
            }   
            evtMgr_.ped_clear_result();                        
        }   
    }
    // -------------------------------------------
    if (!evtMgr_.global_start())
        return;
    // ===========================================
    while (!evtMgr_.noped_trigger_empty()) {
        if (evtMgr_.noped_do_merge(true)) {
            while (!evtMgr_.before_lost_queue.empty()) {
                datafile.write_modules_alone(evtMgr_.before_lost_queue.front());
                evtMgr_.before_lost_queue.pop();
            }
            if (evtMgr_.get_result_noped_events_vec().empty()) {
                datafile.write_trigger_alone(evtMgr_.get_result_noped_trigger());
            } else {
                datafile.write_event_align(evtMgr_.get_result_noped_trigger(), evtMgr_.get_result_noped_events_vec());
                cnt.aligned_event_sum += evtMgr_.get_result_noped_trigger().get_pkt_count();
                evtMgr_.noped_clear_result();
            }
        }
    }
    // -------------------------------------------
}

void Processor::write_meta_info(FileList& filelist, SciDataFile& datafile) {
    // dattype
    datafile.write_meta("m_dattype", "POLAR SCI DIRECTLY DECODED DATA AFTER TIME ALIGNED");
    // version
    datafile.write_meta("m_version", "SCI_Decode v1.0");
    // gentime
    TTimeStamp * cur_time = new TTimeStamp();
    datafile.write_meta("m_gentime", cur_time->AsString("lc"));
    delete cur_time;
    cur_time = NULL;
    // rawfile
    string raw_data_file_list;
    bool is_first = true;
    TSystem sys;
    filelist.set_start();
    while (filelist.next()) {
        if (is_first) {
            is_first = false;
            raw_data_file_list += sys.BaseName(filelist.cur_file());
        } else {
            raw_data_file_list += "; ";
            raw_data_file_list += sys.BaseName(filelist.cur_file());
        }
    }
    datafile.write_meta("m_rawfile", raw_data_file_list.c_str());
    // dcdinfo
    char str_buffer[120];
    sprintf(str_buffer,
            "time_span: %d secs, mean_rate: %d cnts/sec, lost_percent: %4.2f%%, aligned_percent: %4.2f%%",
            cnt.get_time_span(),
            cnt.get_mean_rate(),
            cnt.get_lost_percent(),
            cnt.get_aligned_percent());
    datafile.write_meta("m_dcdinfo", str_buffer);
}
