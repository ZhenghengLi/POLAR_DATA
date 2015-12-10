#include <fstream>
#include <iostream>
#include <cstdlib>
#include <cstddef>
#include <cassert>
#include <cstring>
#include <list>
#include <stdint.h>

#define BUFFER_SIZE 6156

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
	int remain_counter_;
	bool found_start_frame_;
private:
	void shift_left_();
	bool check_valid_();
public:
	char data_buffer[BUFFER_SIZE];
public:
	FileList();
	~FileList();
	bool initialize(string listfile);
	void set_start();
	bool next();
	const char* cur_file();

	bool next_file();
	bool next_frame();
	
};
