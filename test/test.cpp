#include <iostream>
#include <cstddef>
#include <cstring>
#include <cstdio>
#include <stdint.h>
#include <stdlib.h>
#include "RootInc.hpp"

using namespace std;

int main(int argc, char** argv) {
	TApplication* rootapp = new TApplication("POLAR", &argc, argv);
	TFile* fin = new TFile("../output/test.root", "READ");
	TTree* t_event = static_cast<TTree*>(fin->Get("t_event"));
	UInt_t time_align;
	t_event->SetBranchAddress("time_align", &time_align);	
	Int_t tot_entries = t_event->GetEntries();
	TGraph* gr = new TGraph();	
	for(int i = 0; i < tot_entries; i++) {
		t_event->GetEntry(i);
		gr->SetPoint(i, static_cast<Double_t>(i), static_cast<Double_t>(time_align));
	}
	new TCanvas("c1", "time_align", 600, 400);
	gr->Draw();
	rootapp->Run();
	return 0;
}
