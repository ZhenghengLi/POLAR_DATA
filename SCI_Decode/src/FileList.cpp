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
		fileIter_ = fileList_.begin();
		start_flag = true;
		return true;
	}
}

void FileList::set_start() {
	fileIter_ = fileList_.begin();
	start_flag = true;
}

bool FileList::next() {
	assert(!fileList_.empty());
	if (start_flag) 
		start_flag = false;
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
