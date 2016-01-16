#include "ComptonEdgeCalc.hpp"

ComptonEdgeCalc::ComptonEdgeCalc() {
    spec_file_ = NULL;
    canvas_spec_ = NULL;
    canvas_cnts_ = NULL;
    canvas_adc_ = NULL;
    h_trigger_cnts_ = NULL;
    h_accepted_cnts_ = NULL;
    h_adc_per_kev_ = NULL;
    for (int i = 0;i < 25; i++) {
        for (int j = 0; j < 64; j++) {
            f_erfc_[i][j] = NULL;
            h_spec_[i][j] = NULL;
        }
    }
    done_flag_ = false;
    for (int i = 0; i < 25; i++) {
        adc_per_kev[i].ResizeTo(64);
        trigger_cnts[25].ResizeTo(64);
    }
    accepted_cnts.ResizeTo(25);
    clear_counts_();
}

ComptonEdgeCalc::~ComptonEdgeCalc() {
    if (spec_file_ != NULL)
        close();
    if (h_trigger_cnts_ != NULL) {
        delete h_trigger_cnts_;
        h_trigger_cnts_ = NULL;
    }
    if (h_accepted_cnts_ != NULL) {
        delete h_accepted_cnts_;
        h_accepted_cnts_ = NULL;
    }
    if (h_adc_per_kev_ != NULL) {
        delete h_adc_per_kev_;
        h_adc_per_kev_ = NULL;
    }
}

boid ComptonEdgeCalc::clear_counts_() {
    for (int i = 0; i < 25; i++) {
        accepted_cnts(i) = 0;
        for (int j = 0; j < 64; j++) {
            trigger_cnts[i](j) = 0;
            adc_per_kev[i](j) = 1;
        }
    }
}

bool ComptonEdgeCalc::open(const char* filename, char m) {
    if (m == 'w' || m == 'W')
        mode_ = 'w';
    else if (m == 'r' || m == 'R')
        mode_ = 'r';
    else
        return false;

    if (mode_ == 'r')
        spec_file_ = new TFile(filename, "READ");
    else
        spec_file_ = new TFile(filename, "UPDATE");
    if (spec_file_->IsZombie())
        return false;

    if (mode_ == 'r') {
        TVectorF* tmp_vec_p;
        tmp_vec_p = static_cast<TVectorF*>(spec_file_->Get("accepted_cnts"));
        if (tmp_vec_p == NULL)
            return false;
        else
            accepted_cnts = *tmp_vec_p;
        for (int i = 0; i < 25; i++) {
            sprintf(name_, "trigger_cnts_%d", i + 1);
            tmp_vec_p = static_cast<TVectorF*>(spec_file_->Get(name_));
            if (tmp_vec_p == NULL)
                return false;
            else
                trigger_cnts[i] = *tmp_vec_p;
        }
    }
    for (int i = 0; i < 25; i++) {
        for (int j = 0; j < 64; j++) {
            if (mode_ == 'r') {
                sprintf(name_, "f_erfc_%d_%d", i + 1, j + 1);
                f_erfc_[i][j] = new TF1(name_, "[0]+[1]*TMath::Erfc((x-[2])/[3])", 0, 4096);
                f_erfc_[i][j]->SetParName(2, "CE");
                f_erfc_[i][j]->SetParameters(1, 50, 2000, 250);
            }
            sprintf(name_, "h_spec_%d_%d", i + 1, j + 1);
            h_spec_[i][j] = static_cast<TH1F*>(spec_file_->Get(name_));
            if (h_spec_[i][j] == NULL) {
                if (mode_ == 'r') {
                    return false;
                } else {
                    sprintf(title_, "Spectrum of CH %d_%d", i + 1, j + 1);
                    h_spec_[i][j] = new TH1F(name_, title_, CE_BINS, 0, 4096);
                    h_spec_[i][j]->SetDirectory(spec_file_);
                }
            }
        }
    }
    done_flag_ = false;
    return true;
}

void ComptonEdgeCalc::close() {
    if (spec_file_ == NULL)
        return;
    if (mode_ == 'w')
        accepted_cnts.Write("accepted_cnts");
    for (int i = 0; i < 25; i++) {
        if (mode_ == 'w') {
            sprintf(name_, "trigger_cnts_%d", i + 1);
            trigger_cnts[i].Write(name_);
        }
        for (int j = 0; j < 64; j++) {
            if (mode_ == 'r') {
                delete f_erfc_[i][j];
                f_erfc_[i][j] = NULL;
            }
            if (mode_ == 'w')
                h_spec_[i][j]->Write();
            delete h_spec_[i][j];
            h_spec_[i][j] = NULL;
        }
    }
    spec_file_->Close();
    delete spec_file_;
    spec_file_ = NULL;
}

void ComptonEdgeCalc::do_fill(PhyEventFile& phy_event_file) {
    if (spec_file_ == NULL)
        return;
    if (mode_ != 'w')
        return;
    phy_event_file.trigg_restart();
    while (phy_event_file.trigg_next()) {
        for (int i = 0; i < 25; i++) {
            if (phy_event_file.trigg.trig_accepted[i]) {
                accepted_cnts(i)++;
            }
        }
        while (phy_event_file.event_next()) {
            int idx = phy_event_file.event.ct_num - 1;
            for (int j = 0; j < 64; j++) {
                if (phy_event_file.event.trigger_bit[j]) {
                    trigger_cnts[idx](j)++;
                    h_spec_[idx][j]->Fill(phy_event_file.event.energy_ch[j]);
                }
            }
        }
    }
    phy_event_file.trigg_restart();
}

void ComptonEdgeCalc::do_fit(Float_t energy) {
    if (spec_file_ == NULL)
        return;
    if (mode_ != 'r')
        return;
    for (int i = 0; i < 25; i++) {
        for (int j = 0; j < 64; j++) {
            // Do fitting here
        }
    }
}

void ComptonEdgeCalc::do_move_trigg(PhyEventFile& phy_event_file_w,
                                    const PhyEventFile& phy_event_file_r) const {
    
}

void ComptonEdgeCalc::do_calibrate(PhyEventFile& phy_event_file_w,
                                   const PhyEventFile& phy_event_file_r) const {

}

void ComptonEdgeCalc::show_spec(int ct_num) {

}

void ComptonEdgeCalc::show_counts() {

}

void ComptonEdgeCalc::show_adc_per_kev() {

}

void ComptonEdgeCalc::CloseWindow() {

}

void ComptonEdgeCalc::ProcessAction(Int_t event,
                                    Int_t px,
                                    Int_t py,
                                    TObject* selected) {

}

bool ComptonEdgeCalc::write_kvec(const char* filename) {

    return true;
}

bool ComptonEdgeCalc::read_kvec(const char* filename) {

}


