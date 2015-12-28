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
    RQ_OBJECT("CrossTalkCalc")
public:  // slots
    virtual void CloseWindow();
    virtual void ProcessAction(Int_t event,
                               Int_t px,
                               Int_t py,
                               TObject* selected);
public:
    struct Xtalk_Point_T {
        int i;       // 0 - 24
        int jx;      // 0 - 63
        int jy;      // 0 - 63
        Float_t x;   // 0 - 4095
        Float_t y;   // 0 - 4095
    };

private:
    TFile* xtalk_file_;
    TTree* t_xtalk_;
    Xtalk_Point_T xtalk_point_;
    TH2F* h_xtalk_[64][64];
    TF1* f_xtalk_[64][64];
    TH2F* h_xtalk_res_;
    TH2F* h_xtalk_mod_;
    TCanvas* canvas_res_;
    TLine* lineH_[4];
    TLine* lineV_[4];
    TCanvas* canvas_mod_;
    char name_[80];
    char title_[80];
    bool done_flag_;
    char mode_;    // 'w' | 'r'

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
                    const PhyEventFile& phy_event_file_r);
    void show_mod(int ct_num);
    void show_xtalk();

    bool write_xmat(const char* filename);
    bool read_xmat(const char* filename);

};

#endif
