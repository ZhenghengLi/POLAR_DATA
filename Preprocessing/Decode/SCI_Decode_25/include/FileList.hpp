#ifndef FILELIST_H
#define FILELIST_H

#include <fstream>
#include <iostream>
#include <cstdlib>
#include <cstddef>
#include <cassert>
#include <cstring>
#include <list>
#include <stdint.h>

#define BUFFER_SIZE 4104
#define CHECK_MAX 100

using namespace std;

class FileList {
private:
    list<string> fileList_;
    list<string>::const_iterator fileIter_;
    bool start_flag_;
    bool buffer_is_first_;
    ifstream datafile_;
    int vernier_begin_;
    int vernier_end_;
    bool reach_file_end_;
    bool found_start_frame_;
private:
    void shift_left_();
    bool check_header_();
public:
    char data_buffer[BUFFER_SIZE];
public:
    FileList();
    ~FileList();
    bool initialize(const char* listfile);
    void add_back(const char* filename);
    void set_start();
    void clear();
    bool next();
    const char* cur_file();

    bool next_file();
    bool next_frame();
    
};

#endif
