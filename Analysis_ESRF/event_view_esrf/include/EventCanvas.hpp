#ifndef EventCanvas_H
#define EventCanvas_H

#include <iostream>
#include <iomanip>
#include <vector>
#include <queue>
#include <cmath>
#if !(defined(__ROOTCLING__) || defined(__CINT__))
#include "RootInc.hpp"
#include "CooConv.hpp"
#endif /* __ROOTCLING__ || __CINT __ */
#include "RQ_OBJECT.h"

using namespace std;

class EventCanvas {
    RQ_OBJECT("EventCanvas");

public: // slots
    virtual void CloseWindow();
    virtual void ProcessAction(Int_t event,
                               Int_t px,
                               Int_t py,
                               TObject* selected);

private:
#if !(defined(__ROOTCLING__) || defined(__CINT__))
    TCanvas* canvas_event_;
    TH2F*    trigger_map_;
    TH2F*    energy_map_;
    THStack* h_stack_;
    TFile*   t_pol_event_file_;
    TTree*   t_pol_event_tree_;
    int      entry_step_;
    Long64_t entry_current_;
    bool            queue_flag_;
    queue<Long64_t> entry_queue_;

    struct {
        Bool_t          time_aligned[25];
        Int_t           lost_count;
        Bool_t          trigger_bit[25][64];
        Int_t           multiplicity[25];
        Float_t         energy_value[25][64];
    } t_pol_event_;

#endif /* __ROOTCLING__ || __CINT __ */

public:
    EventCanvas();
    virtual ~EventCanvas();
#if !(defined(__ROOTCLING__) || defined(__CINT__))
    bool open(const char* filename, int start, int step);
    void close();
    void draw_event();
    bool read_entry_queue(const char* filename);
#endif /* __ROOTCLING__ || __CINT __ */

};

#endif
