#ifndef PEDMEANCALC_H
#define PEDMEANCALC_H

#include <iostream>
#include <cstdlib>
#include <iomanip>
#include <cstdio>
#include "RootInc.hpp"
#include "EventIterator.hpp"
#include "PhyEventFile.hpp"
#include "CooConv.hpp"

#define PED_BINS 100
#define PED_MAX 1024

using namespace std;

class PedMeanCalc {
	RQ_OBJECT("PedMeanCalc")

public:  // slots
	virtual void CloseWindow();
	virtual void ProcessAction(Int_t event,
							   Int_t px,
							   Int_t py,
							   TObject *selected);
	
private:
	EventIterator* eventIter_;
	TF1* f_gaus_[25][64];
	TH1F* h_ped_[25][64];
	TH2F* h_ped_map_;
	TCanvas* canvas_mod_;
	TCanvas* canvas_res_;
	char name_[80];
	char title_[80];
	bool done_flag_;

public:
	Float_t mean[25][64];
	Float_t sigma[25][64];
	Float_t mean_0[25][64];
	Float_t sigma_0[25][64];
	
private:
	void create_();
	void destroy_();

public:
	PedMeanCalc();
	virtual ~PedMeanCalc();

	void init(EventIterator* iter);
	void do_calc();
	void show(int ct_num);
	void do_move_trigg(PhyEventFile& phy_event_file,
					   const EventIterator& event_iterator) const;
	void do_subtruct(PhyEventFile& phy_event_file, 
					 const EventIterator& event_iterator) const;
	void print(bool sigma_flag = true);
	void show_mean();

};

#endif
