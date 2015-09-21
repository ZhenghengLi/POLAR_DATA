#include <fstream>
#include <iostream>
#include <cstddef>
#include <cassert>
#include <list>

using namespace std;

class FileList {
private:
	list<string> fileList_;
	list<string>::const_iterator fileIter_;
	bool start_flag;

public:
	FileList();
	~FileList();
	bool initialize(string listfile);
	void set_start();
	bool next();
	const char* cur_file();
	
};
