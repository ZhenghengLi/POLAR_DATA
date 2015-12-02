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

	Int_t pkt_count;
	Int_t lost_count;
	Int_t mode;
	t_trigg->SetBranchAddress("pkt_count", &pkt_count);
	t_trigg->SetBranchAddress("lost_count", &lost_count);
	t_trigg->SetBranchAddress("mode", &mode);

	int tot_cnt = 0;
	int tot_lost_cnt = 0;
	Long64_t tot_entries = t_trigg->GetEntries();
	for (Long64_t i = 0; i < tot_entries; i++) {
		t_trigg->GetEntry(i);
		tot_cnt += pkt_count + lost_count;
		tot_lost_cnt += lost_count;
		if ((i + 1) % 1000 == 0) {
			cout << i + 1 / 1000 << " " << tot_cnt << " " << tot_lost_cnt << endl;
			tot_cnt = 0;
			tot_lost_cnt = 0;
		}
	}


	return 0;
}
