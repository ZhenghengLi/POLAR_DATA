#include "FileList.hpp"

using namespace std;

FileList::FileList() {

}

FileList::~FileList() {

}

bool FileList::initialize(const char* listfile) {
    ifstream infile(listfile, ios::in);
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

void FileList::clear() {
    fileList_.clear();
}

void FileList::add_back(const char* filename) {
    string str_file(filename);
    fileList_.push_back(str_file);
}

void FileList::set_start() {
    fileIter_ = fileList_.begin();
    start_flag_ = true;
    buffer_is_first_ = true;
    vernier_begin_ = 0;
    vernier_end_ = 0;
    reach_file_end_ = false;
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

bool FileList::check_header_() {
    uint16_t frame_header = 0;
    for (int i = 0; i < 2; i++) {
        frame_header <<= 8;
        frame_header += static_cast<uint8_t>(data_buffer[vernier_begin_ + 0 + i]);
    }
    if (frame_header != 0x009F)
        return false;
    uint16_t frame_length = 0;
    for (int i = 0; i < 2; i++) {
        frame_length <<= 8;
        frame_length += static_cast<uint8_t>(data_buffer[vernier_begin_ + 4 + i]);
    }
    if (frame_length != 0x07F9)
        return false;
    uint32_t sci_starter = 0;
    for (int i = 0; i < 4; i++) {
        sci_starter <<= 8;
        sci_starter += static_cast<uint8_t>(data_buffer[vernier_begin_ + 6 + i]);
    }
    if (sci_starter != 0x706F6C61)
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
    cout << cur_file() << endl;
    datafile_.open(cur_file(), ios::in|ios::binary);
    if (!datafile_.is_open()) {
        cerr << "Data file open failed: " << cur_file() << endl;
        return false;
    }
    buffer_is_first_ = true;
    reach_file_end_ = false;
    found_start_frame_ = false;
    int check_size = 0;
    while (!reach_file_end_) {
        datafile_.read(data_buffer + vernier_end_, BUFFER_SIZE - vernier_end_);
        vernier_end_ += datafile_.gcount();
        if (datafile_.eof()) {
            reach_file_end_ = true;
            break;
        }
        check_size += datafile_.gcount();
        if (check_size / 2052 > CHECK_MAX) {
            cerr << "INVALID POLAR SCI FILE." << endl;
            return false;
        }
        if (check_header_()) {
            found_start_frame_ = true;
            while (vernier_begin_ < 2051) {
                vernier_begin_++;
                if (check_header_()) {
                    shift_left_();
                    found_start_frame_ = false;
                    break;
                }
            }
            vernier_begin_ = 0;
        } else {
            vernier_begin_++;
            shift_left_();
        }
        if (found_start_frame_) {
            break;
        }
    }
    if (found_start_frame_) {
        return true;
    } else {
        cerr << "Could not find the first frame: " << cur_file() << endl;
        return false;
    }
}

bool FileList::next_frame() {
    if (!found_start_frame_) {
        vernier_begin_ = 0;
        vernier_end_ = 0;
        return false;
    }

    if (buffer_is_first_) {
        buffer_is_first_ = false;
        return true;
    }
    if (vernier_end_ - vernier_begin_ < 2052)
        return false;
    vernier_begin_ += 2052;
    shift_left_();
    if (vernier_end_ - vernier_begin_ < 2052)
        return false;
    bool found_header = false;
    while (!reach_file_end_) {
        datafile_.read(data_buffer + vernier_end_, BUFFER_SIZE - vernier_end_);
        vernier_end_ += datafile_.gcount();
        if (datafile_.eof()) {
            reach_file_end_ = true;
            break;
        }
        if (check_header_()) {
            found_header = true;
            while (vernier_begin_ < 2051) {
                vernier_begin_++;
                if (check_header_()) {
                    shift_left_();
                    found_header = false;
                    break;
                }
            }
            vernier_begin_ = 0;
        } else {
            vernier_begin_++;
            shift_left_();
        }
        if (found_header) {
            break;
        }
    }
    if (!found_header) {
        while (vernier_end_ - vernier_begin_ >= 2052) {
            if (check_header_()) {
                found_header = true;
                while (vernier_begin_ < 2042) {
                    vernier_begin_++;
                    if (check_header_()) {
                        shift_left_();
                        found_header = false;
                        break;
                    }
                }
                vernier_begin_ = 0;
            } else {
                vernier_begin_++;
                shift_left_();
            }
            if (found_header) {
                break;
            }
        }
    }
    if (found_header)
        return true;
    else
        return false;
}

