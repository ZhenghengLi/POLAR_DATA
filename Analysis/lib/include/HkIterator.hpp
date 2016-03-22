#ifndef HKITERATOR_H
#define HKITERATOR_H

#include "HkType.hpp"

class HkIterator: private HkType {
private:
    TFile* t_file_in_;
    TTree* t_hk_obox_;
    TTree* t_hk_ibox_;

    Long64_t hk_obox_tot_entries_;
    Long64_t hk_obox_cur_index_;
    bool     hk_obox_reach_end_;

    Long64_t hk_ibox_tot_entries_;
    Long64_t hk_ibox_cur_index_;
    bool     hk_ibox_reach_end_;

public:
    Hk_Obox_T hk_obox;
    Hk_Ibox_T hk_ibox;

public:
    HkIterator();
    ~HkIterator();

    bool open(const char* filename);
    void close();

    void hk_obox_set_start();
    bool hk_obox_next();
    bool hk_obox_select(Long64_t entry_index);
    Long64_t get_hk_obox_cur_index() {
        if (t_file_in_ == NULL)
            return -1;
        else
            return hk_obox_cur_index_;
    }
    Long64_t get_hk_obox_entries() {
        if (t_file_in_ == NULL)
            return -1;
        else
            return hk_obox_tot_entries_;
    }

    void hk_ibox_set_start();
    bool hk_ibox_next();
    bool hk_ibox_select(Long64_t entry_index);
    Long64_t get_hk_ibox_cur_index() {
        if (t_file_in_ == NULL)
            return -1;
        else
            return hk_ibox_cur_index_;
    }
    Long64_t get_hk_ibox_entries() {
        if (t_file_in_ == NULL)
            return -1;
        else
            return hk_ibox_tot_entries_;
    }

};

#endif
