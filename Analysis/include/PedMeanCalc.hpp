#ifndef PEDMEANCALC_H
#define PEDMEANCALC_H

#include <iostream>
#include <cstdlib>
#include <iomanip>
#include <stdio.h>
#include "RootInc.hpp"
#include "EventIterator.hpp"
#include "PhyEventFile.hpp"

#define PED_BINS 100
#define PED_MAX 1024

using namespace std;

class PedMeanCalc {
private:
	EventIterator* eventIter_;
	TF1* f_gaus_[25][64];
	TH1D* h_ped_[25][64];
	TGraph2D* g_ped_;
	TCanvas* canvas_[25];
	TCanvas* canvas_res_;
	char name_[80];
	char title_[80];
	bool done_flag_;

public:
	Double_t mean[25][64];
	Double_t sigma[25][64];
	Double_t mean_0[25][64];
	Double_t sigma_0[25][64];
	
private:
	void create_();
	void destroy_();
public:
	PedMeanCalc();
	~PedMeanCalc();

	void init(EventIterator* iter);
	void do_calc();
	void show(int ct_num);
	void do_move_trigg(PhyEventFile& phy_event_file,
					   const EventIterator& event_iterator) const;
	void do_subtruct(PhyEventFile& phy_event_file, 
					 const EventIterator& event_iterator) const;
	void print(bool sigma_flag = true);
	void show_all();
	void show_mean();

};

#endif
