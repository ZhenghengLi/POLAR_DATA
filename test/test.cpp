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
	TTree* t_trigg = static_cast<TTree*>(root_file->Get("t_trigg"));

	ULong64_t b_trigg_frm_gps_time;
	t_trigg->SetBranchAddress("frm_gps_time", &b_trigg_frm_gps_time);
	t_trigg->GetEntry(0);
	ULong64_t start_gps_time = b_trigg_frm_gps_time;
	t_trigg->GetEntry(t_trigg->GetEntries() - 70000);
	ULong64_t end_gps_time = b_trigg_frm_gps_time;
	cout << start_gps_time << endl;
	cout << end_gps_time << endl;

	start_gps_time &= 0xFFFFF000;
	start_gps_time >>= 12;
	end_gps_time &= 0xFFFFF000;
	end_gps_time >>= 12;
	cout << "GPS time span: " << end_gps_time - start_gps_time << endl;
	cout << start_gps_time << endl;
	cout << end_gps_time << endl;

	return 0;
}
