#ifndef CROSSTALKCALC
#define CROSSTALKCALC

#include <iostream>
#include <iomanip>
#include <cstdio>
#include "RootInc.hpp"
#include "PhyEventFile.hpp"
#include "CooConv.hpp"

using namespace std;

class CrossTalkCalc {

public:  // slots
    virtual void CloseWindow();
    virtual void ProcessAction(Int_t event,
                               Int_t px,
                               Int_t py,
                               TObject* selected);
    
private:
    TFile* xtalk_file_;
    TGraph* g_xtalk_[25][64][64];
    int g_n_[25][64][64];
    TF1* f_xtalk_[25][64][64];
    TH2F* h_xtalk_res_;
    TH2F* h_xtalk_mod_;
    TCanvas* canvas_res_;
    TCanvas* canvas_mod_;
    TCanvas* canvas_cha_;
    char name_[80];
    char title_[80];
    bool done_flag_;
    char mode_;    // 'w' | 'r'
    int selected_mod_;

public:
    TMatrixF xtalk_matrix[25];
    TMatrixF xtalk_matrix_inv[25];
    TVectorF energy_vec;

private:

public:
    CrossTalkCalc();
    virtual ~CrossTalkCalc();

    bool open(const char* filename, char m);
    void close();
    void do_fill(PhyEventFile& phy_event_file);
    void do_fit();

    void do_move_trigg(PhyEventFile& phy_event_file_w,
                       const PhyEventFile& phy_event_file_r) const;
    void do_correct(PhyEventFile& phy_event_file_w,
                    const PhyEventFile& phy_event_file_r) const;
    void show(int ct_num);
    void show_xtalk();

};

#endif
