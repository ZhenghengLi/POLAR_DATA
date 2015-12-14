#ifndef PEDMEANCALC_H
#define PEDMEANCALC_H

#include <iostream>
#include <cstdlib>
#include <stdio.h>
#include "RootInc.hpp"
#include "EventIterator.hpp"

#define PED_BINS 80
#define PED_MAX 1024

using namespace std;

class PedMeanCalc {
private:
	EventIterator* eventIter_;
	TF1* f_gaus_[25][64];
	TH1F* h_ped_[25][64];
	TCanvas* canvas_[25];
	char name_[80];
	char title_[80];
	bool done_flag_;
	Long64_t tot_ch_[25][64];
	Int_t num_ch_[25][64];
	int mean_0_[25][64];
	
public:
	Float_t mean[25][64];
	Float_t sigma[25][64];
	
private:
	void create_();
	void destroy_();
public:
	PedMeanCalc();
	~PedMeanCalc();

	void init(EventIterator* iter);
	void do_calc();
	void show(int ct_num);

};

#endif
