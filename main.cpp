#include <iostream>
#include<cstdlib>
#include "FileList.hpp"

using namespace std;

int main(int argc, char** argv) {
	FileList filelist;
	if (!filelist.initialize("listfile.txt")) {
		cout << "Error occurred!" << endl;
		exit(1);
	}
	while(filelist.next()) {
		cout << filelist.cur_file() << endl;
	}

	return 0;
}
