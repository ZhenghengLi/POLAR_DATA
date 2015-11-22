#include <iostream>
#include <fstream>
#include <cstddef>
#include <cstring>
#include <cstdio>
#include <stdint.h>
#include <stdlib.h>
#include "RootInc.hpp"

using namespace std;

int main(int argc, char** argv) {
	TFile* root_file = new TFile("./output/sci_test.root", "READ");
	TTree* t_event = static_cast<TTree*>(root_file->Get("t_event"));
	TTree* t_trigg = static_cast<TTree*>(root_file->Get("t_trigg"));
	TTree* t_ped_event = static_cast<TTree*>(root_file->Get("t_ped_event"));
	TTree* t_ped_trigg = static_cast<TTree*>(root_file->Get("t_ped_trigg"));

	cout << "t_event: " << t_event->GetEntries() << endl;
	cout << "t_trigg: " << t_trigg->GetEntries() << endl;
	cout << "t_ped_event: " << t_ped_event->GetEntries() << endl;
	cout << "t_ped_trigg: " << t_ped_trigg->GetEntries() << endl;

	return 0;
}
