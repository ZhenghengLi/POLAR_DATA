#ifndef COMPTONEDGECALC_H
#define COMPTONEDGECALC_H

#include <iostream>
#include <cstdlib>
#include <iomanip>
#include <cstdio>
#include "RootInc.hpp"
#include "PhyEventFile.hpp"
#include "CooConv.hpp"

#define CE_BINS 200

using namespace std;

class ComptonEdgeCalc {

public:
    virtual void CloseWindow();
    virtual void ProcessAction(Int_t event,
                               Int_t px,
                               Int_t py,
                               TObject* selected);

private:
    TFile* spec_file_;
    TF1*  f_erfc_[25][64];
    TH1F* h_spec_[25][64];
    TH2F* h_trigger_cnts_;
    TH2F* h_accepted_cnts_;
    TH2F* h_adc_per_kev_;
    TCanvas* canvas_spec_;
    TCanvas* canvas_cnts_;
    TCanvas* canvas_adc_;
    char name_[80];
    char title_[80];
    bool done_flag_;
    char mode_;    // 'w' | 'r'

public:
    TVectorF adc_per_kev[25];
    TVectorF trigger_cnts[25];
    TVectorF accepted_cnts;
    
private:
    void clear_counts_();
public:
    ComptonEdgeCalc();
    virtual ~ComptonEdgeCalc();

    bool open(const char* filename, char m);
    void close();
    void do_fill(PhyEventFile& phy_event_file);
    void do_fit(Float_t energy = 1);
    
    void do_move_trigg(PhyEventFile& phy_event_file_w,
                       const PhyEventFile& phy_event_file_r) const;
    void do_calibrate(PhyEventFile& phy_event_file_w,
                      const PhyEventFile& phy_event_file_r) const;
    void show_spec(int ct_num);
    void show_counts();
    void show_adc_per_kev();

    bool write_kvec(const char* filename);
    bool read_kvec(const char* filename);
};

#endif
