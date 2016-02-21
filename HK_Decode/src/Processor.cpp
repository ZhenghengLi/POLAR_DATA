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

    bool cur_is_odd = (frame.get_pkt_tag() > 0);
    int cur_is_bad = 0;
    
    if (!frame.check_valid()) {
        cnt.frm_invalid++;
        cur_is_bad = 1;
    } else {
        cnt.frm_valid++;
        if (!frame.check_crc()) {
            cnt.frm_crc_error++;
            cur_is_bad = 2;
        } else {
            cnt.frm_crc_passed++;
            cur_is_bad = 0;
        }
    }
   
    if (odd_is_ready) {
        if (cur_is_odd) {
            odd_packet_.update(cur_is_bad);
            datafile.write_odd_packet_alone(ready_odd_packet_);
            ready_odd_packet_ = odd_packet_;
        } else {
            even_packet_.update(cur_is_bad);
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
        if (cur_is_odd) {
            odd_packet_.update(cur_is_bad);
            ready_odd_packet_ = odd_packet_;
            odd_is_ready = true;
        } else {
            even_packet_.update(cur_is_bad);
            datafile.write_even_packet_alone(even_packet_);
            even_packet_.clear_all_info();
        }
    }
}

void Processor::do_the_last_work(HkDataFile& datafile) {
    if (odd_is_ready) {
        datafile.write_odd_packet_alone(ready_odd_packet_);
    }
}

void Processor::write_meta_info(FileList& filelist, HkDataFile& datafile) {
    // dattype
    datafile.write_meta("m_dattype", "POLAR HK DIRECTLY DECODED DATA");
    // version
    datafile.write_meta("m_version", "HK_Decode v1.0");
    // gentime
    TTimeStamp* cur_time = new TTimeStamp();
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
}
