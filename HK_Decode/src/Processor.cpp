#include "Processor.hpp"

using namespace std;

Processor::Processor() {
    initialize();
}

Processor::Processor(const char* frame_ptr) {
    initialize(frame_ptr);
}

Processor::~Processor() {

}

void Processor::initialize() {
    cnt.clear();
    odd_is_ready = false;
}

void Processor::initialize(const char* frame_ptr) {
    cnt.clear();
    odd_is_ready = false;
    set_frame_ptr_(frame_ptr);
}

void Processor::set_frame_ptr_(const char* frame_ptr) {
    frame.setdata(frame_ptr);
    odd_packet_.setdata(frame_ptr);
    even_packet_.setdata(frame_ptr);
}

void Processor::set_log(bool flag) {
    log_flag_ = flag;
}

bool Processor::can_log() {
    return log_flag_ && os_logfile_.is_open();
}

bool Processor::logfile_open(const char* filename) {
    os_logfile_.open(filename);
    return os_logfile_.is_open();
}

void Processor::logfile_close() {
    os_logfile_.close();
}

void Processor::process(HkDataFile& datafile) {
    cnt.frame++;
    
    if (!frame.check_valid()) {
        cnt.frm_invalid++;
        // is_bad = 1;
    } else {
        cnt.frm_valid++;
        if (!frame.check_crc()) {
            cnt.frm_crc_error++;
            // is_bad = 2;
        } else {
            cnt.frm_crc_passed++;
            // is_bad = 0;
        }
    }
    
    if (odd_is_ready) {
        if (frame.get_pkt_tag() > 0) {
            datafile.write_odd_packet_alone(ready_odd_packet_);
            ready_odd_packet_ = odd_packet_;
        } else {
            even_packet_.update();
            if (frame.can_connect()) {
                datafile.write_two_packet(ready_odd_packet_, even_packet_);
                ready_odd_packet_.clear_all_info();
                even_packet_.clear_all_info();
            } else {
                cnt.frm_con_error++;
                datafile.write_odd_packet_alone(ready_odd_packet_);
                ready_odd_packet_.clear_all_info();
                datafile.write_even_packet_alone(even_packet_);
                even_packet_.clear_all_info();
            }
            odd_is_ready = false;
        }
    } else {
        if (frame.get_pkt_tag() > 0) {
            ready_odd_packet_ = odd_packet_;
            odd_is_ready = true;
        } else {
            even_packet_.update();
            datafile.write_even_packet_alone(even_packet_);
            even_packet_.clear_all_info();
        }
    }
}

void Processor::write_meta_info(FileList& filelist, HkDataFile& datafile) {

}
