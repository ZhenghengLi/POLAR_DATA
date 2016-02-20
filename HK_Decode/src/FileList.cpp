#include "FileList.hpp"

using namespace std;

FileList::FileList() {

}

FileList::~FileList() {

}

bool FileList::initialize(string listfile) {
    ifstream infile(listfile.c_str(), ios::in);
    if (!infile) {
        cerr << "Can not open file: " << listfile << endl;
        return false;
    }
    string filename;
    fileList_.clear();
    while (true) {
        getline(infile, filename);
        if (infile.eof()) {
            break;
        }
        fileList_.push_back(filename);
    }
    infile.close();
    if (fileList_.empty()) {
        return false;
    } else {
        set_start();
        return true;
    }
}

void FileList::set_start() {
    fileIter_ = fileList_.begin();
    start_flag_ = true;
    buffer_is_first_ = true;
    vernier_begin_ = 0;
    vernier_end_ = 0;
    reach_file_end_ = false;
    remain_counter_ = 2;
    found_start_frame_ = false;
}

bool FileList::next() {
    if (fileList_.empty())
        return false;
    if (start_flag_)
        start_flag_ = false;
    else
        ++fileIter_;
    if (fileIter_ == fileList_.end())
        return false;
    else
        return true;
}

const char* FileList::cur_file() {
    return (*fileIter_).c_str();
}

void FileList::shift_left_() {
    memmove(data_buffer, data_buffer + vernier_begin_, vernier_end_ - vernier_begin_);
    vernier_end_ -= vernier_begin_;
    vernier_begin_ = 0;
}

bool FileList::check_valid_() {
    uint16_t frame_header = 0;
    for (int i = 0; i < 2; i++) {
        frame_header <<= 8;
        frame_header += static_cast<uint8_t>(data_buffer[vernier_begin_ + 0 + i]);
    }
    if (frame_header != 0x0749)
        return false;
    uint16_t frame_length = 0;
    for (int i = 0; i < 2; i++) {
        frame_length <<= 8;
        frame_length += static_cast<uint8_t>(data_buffer[vernier_begin_ + 4 + i]);
    }
    if (frame_length != 0x00F9)
        return false;
    uint32_t hk_header = 0;
    for (int i = 0; i < 4; i++) {
        hk_header <<= 8;
        hk_header += static_cast<uint8_t>(data_buffer[vernier_begin_ + 6 + i]);
    }
    if (hk_header != 0xD9A4C2EA)
        return false;
    uint16_t next_frame_header = 0;
    for (int i = 0; i < 2; i++) {
        next_frame_header <<= 8;
        next_frame_header += static_cast<uint8_t>(data_buffer[vernier_begin_ + 260 + 0 + i]);
    }
    if (next_frame_header != 0x0749)
        return false;
    uint16_t next_frame_length = 0;
    for (int i = 0; i < 2; i++) {
        next_frame_length <<= 8;
        next_frame_length += static_cast<uint8_t>(data_buffer[vernier_begin_ + 260 + 4 + i]);
    }
    if (next_frame_length != 0x00F9)
        return false;
    uint32_t next_hk_header = 0;
    for (int i = 0; i < 4; i++) {
        next_hk_header <<= 8;
        next_hk_header += static_cast<uint8_t>(data_buffer[vernier_begin_ + 260 + 6 + i]);
    }
    if (next_hk_header != 0xD9A4C2EA)
        return false;
    return true;
}

bool FileList::next_file() {
    datafile_.close();
    if (start_flag_)
        start_flag_ = false;
    else
        ++fileIter_;
    if (fileIter_ == fileList_.end())
        return false;
    // prepare the first data_buffer
    datafile_.open(cur_file(), ios::in|ios::binary);
    if (!datafile_.is_open()) {
        cerr << "Data file open failed: " << cur_file() << endl;
        return false;
    }
    buffer_is_first_ = true;
    remain_counter_ = 2;
    found_start_frame_ = false;
    while (!found_start_frame_) {
        datafile_.read(data_buffer + vernier_end_, BUFFER_SIZE - vernier_end_);
        if (datafile_.eof()) {
            vernier_end_ += datafile_.gcount();
            break;
        } else {
            vernier_end_ = BUFFER_SIZE;
        }
        while (vernier_begin_ < 260) {
            if (check_valid_()) {
                found_start_frame_ = true;
                shift_left_();
                break;
            }
            vernier_begin_++;
        }
        if (!found_start_frame_)
            shift_left_();
    }
    if (found_start_frame_) {
        datafile_.read(data_buffer + vernier_end_, BUFFER_SIZE - vernier_end_);
        vernier_end_ += datafile_.gcount();
        if (datafile_.eof())
            found_start_frame_ = false;
    }
    return true;
}

bool FileList::next_frame() {
    if (!found_start_frame_) {
        vernier_begin_ = 0;
        vernier_end_ = 0;
        return false;
    }
    if (remain_counter_ < 1) {
        vernier_begin_ += 260;
        shift_left_();
        return false;
    }
    if (buffer_is_first_) {
        buffer_is_first_ = false;
        reach_file_end_ = false;
    } else {
        vernier_begin_ += 260;
        shift_left_();
        if (reach_file_end_) {
            remain_counter_--;
        } else {
            datafile_.read(data_buffer + vernier_end_, BUFFER_SIZE - vernier_end_);
            vernier_end_ += datafile_.gcount();
            if (datafile_.eof()) {
                reach_file_end_ = true;
                remain_counter_--;
            }
        }
    }
    return true;
}


