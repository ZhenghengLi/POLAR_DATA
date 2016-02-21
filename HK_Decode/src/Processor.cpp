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
    if (frame.check_valid())
        cnt.frm_valid++;
    else
        cnt.frm_invalid++;
    if (frame.check_crc())
        cnt.frm_crc_passed++;
    else
        cnt.frm_crc_error++;
    if (!frame.can_connect())
        cnt.frm_con_error++;
}

void Processor::write_meta_info(FileList& filelist, HkDataFile& datafile) {

}
