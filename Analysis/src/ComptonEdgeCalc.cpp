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
        trigger_cnts[i].ResizeTo(64);
    }
    accepted_cnts.ResizeTo(25);
    clear_counts_();
    mode_ = ' ';
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

void ComptonEdgeCalc::clear_counts_() {
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

    TVectorF* tmp_vec_p;
    bool first_open = false;
    tmp_vec_p = static_cast<TVectorF*>(spec_file_->Get("accepted_cnts"));
    if (tmp_vec_p == NULL) {
        if (mode_ == 'r') {
            return false;
        } else {
            first_open = true;
        }
    } else {
        accepted_cnts = *tmp_vec_p;
    }
    for (int i = 0; i < 25; i++) {
        sprintf(name_, "trigger_cnts_%d", i + 1);
        tmp_vec_p = static_cast<TVectorF*>(spec_file_->Get(name_));
        if (tmp_vec_p == NULL) {
            if (mode_ == 'r') {
                return false;
            } else {
                first_open = true;
                break;
            }
        } else {
            trigger_cnts[i] = *tmp_vec_p;
        }
    }
    if (first_open)
        clear_counts_();
    
    for (int i = 0; i < 25; i++) {
        for (int j = 0; j < 64; j++) {
            if (mode_ == 'r') {
                sprintf(name_, "f_erfc_%d_%d", i + 1, j + 1);
                f_erfc_[i][j] = new TF1(name_, "[0]+[1]*TMath::Erfc((x-[2])/[3])", 900, 4000);
                f_erfc_[i][j]->SetParName(2, "CE");
                f_erfc_[i][j]->SetParameters(1, 40, 1700, 700);
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
        if (phy_event_file.trigg.mode != 0x00FF)
            continue;
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
    done_flag_ = true;
}

void ComptonEdgeCalc::do_move_trigg(PhyEventFile& phy_event_file_w,
                                    const PhyEventFile& phy_event_file_r) const {
    if (!done_flag_)
        return;
    phy_event_file_w.trigg.trigg_index = phy_event_file_r.trigg.trigg_index;
    phy_event_file_w.trigg.mode = phy_event_file_r.trigg.mode;
    for (int i = 0; i < 25; i++)
        phy_event_file_w.trigg.trig_accepted[i] = phy_event_file_r.trigg.trig_accepted[i];
    phy_event_file_w.trigg.start_entry = phy_event_file_r.trigg.start_entry;
    phy_event_file_w.trigg.pkt_count = phy_event_file_r.trigg.pkt_count;
    phy_event_file_w.trigg.lost_count = phy_event_file_r.trigg.lost_count;
    phy_event_file_w.trigg.level_flag = 3;
}

void ComptonEdgeCalc::do_calibrate(PhyEventFile& phy_event_file_w,
                                   const PhyEventFile& phy_event_file_r) const {
    if (!done_flag_)
        return;
    phy_event_file_w.event.trigg_index = phy_event_file_r.event.trigg_index;
    phy_event_file_w.event.mode = phy_event_file_r.event.mode;
    phy_event_file_w.event.ct_num = phy_event_file_r.event.ct_num;
    for (int j = 0; j < 64; j++)
        phy_event_file_w.event.trigger_bit[j] = phy_event_file_r.event.trigger_bit[j];
    int idx = phy_event_file_r.event.ct_num - 1;
    for (int j = 0; j < 64; j++)
        phy_event_file_w.event.energy_ch[j] = phy_event_file_r.event.energy_ch[j] / adc_per_kev[idx](j);
}

void ComptonEdgeCalc::show_spec(int ct_num) {
    if (spec_file_ == NULL)
        return;
    if (mode_ != 'r')
        return;
    int idx = ct_num - 1;
    gStyle->SetOptStat(11);
    gStyle->SetOptFit(111);
    canvas_spec_ = static_cast<TCanvas*>(gROOT->FindObject("canvas_spec"));
    if (canvas_spec_ == NULL) {
        canvas_spec_ = new TCanvas("canvas_spec", "Spectrum of one mod", 800, 600);
        canvas_spec_->Divide(8, 8);
    }
    sprintf(title_, "Spectrum of CT %d", ct_num);
    canvas_spec_->SetTitle(title_);
    for (int j = 0; j < 64; j++) {
        canvas_spec_->cd(jtoc(j));
        canvas_spec_->GetPad(jtoc(j))->SetLogy();
        if (h_spec_[idx][j]->GetEntries() < 1000)
            continue;
        h_spec_[idx][j]->Draw();   // should do fitting here
        h_spec_[idx][j]->Fit(f_erfc_[idx][j], "QR");
    }
    canvas_spec_->Update();
}

void ComptonEdgeCalc::show_counts() {
    if (spec_file_ == NULL)
        return;
    if (mode_ != 'r')
        return;
    if (h_trigger_cnts_ != NULL) {
        delete h_trigger_cnts_;
        h_trigger_cnts_ = NULL;
    }
    if (h_accepted_cnts_ != NULL) {
        delete h_accepted_cnts_;
        h_accepted_cnts_ = NULL;
    }
    gStyle->SetOptStat(0);
    canvas_cnts_ = static_cast<TCanvas*>(gROOT->FindObject("canvas_cnts"));
    if (canvas_cnts_ == NULL) {
        canvas_cnts_ = new TCanvas("canvas_cnts", "Trigger Counts of 1600 Channels", 1600, 800);
        canvas_cnts_->Divide(2, 1);
        canvas_cnts_->GetPad(1)->SetGrid();
        canvas_cnts_->GetPad(2)->SetGrid();
        canvas_cnts_->Connect("Closed()", "ComptonEdgeCalc", this, "CloseWindow()");
        canvas_cnts_->Connect("ProcessedEvent(Int_t,Int_t,Int_t,TObject*)", "ComptonEdgeCalc",
                              this, "ProcessAction(Int_t,Int_t,Int_t,TObject*)");
    }
    
    h_trigger_cnts_ = new TH2F("h_trigger_cnts", "Trigger Counts of 1600 Channels", 40, 0, 40, 40, 0, 40);
    h_trigger_cnts_->SetDirectory(NULL);
    h_trigger_cnts_->GetXaxis()->SetNdivisions(40);
    h_trigger_cnts_->GetYaxis()->SetNdivisions(40);
    char str_buffer[10];
    for (int i = 0; i < 40; i++) {
        if (i % 8 == 0) {
            sprintf(str_buffer, "%d", i);
            h_trigger_cnts_->GetXaxis()->SetBinLabel(i + 1, str_buffer);
            h_trigger_cnts_->GetYaxis()->SetBinLabel(i + 1, str_buffer);
        }
    }
    for (int i = 0; i < 25; i++) {
        for (int j = 0; j < 64; j++) {
            h_trigger_cnts_->SetBinContent(ijtox(i, j) + 1, ijtoy(i, j) + 1, trigger_cnts[i](j));
        }
    }
    canvas_cnts_->cd(1);
    h_trigger_cnts_->Draw("COLZ");

    h_accepted_cnts_ = new TH2F("h_accepted_cnts", "Accepted Counts of 25 modules", 5, 0, 5, 5, 0, 5);
    h_accepted_cnts_->SetDirectory(NULL);
    h_accepted_cnts_->GetXaxis()->SetNdivisions(5);
    h_accepted_cnts_->GetYaxis()->SetNdivisions(5);
    for (int i = 0; i < 5; i++) {
        sprintf(str_buffer, "%d", i);
        h_accepted_cnts_->GetXaxis()->SetBinLabel(i + 1, str_buffer);
        h_accepted_cnts_->GetYaxis()->SetBinLabel(i + 1, str_buffer);
    }
    for (int i = 0; i < 25; i++) {
        h_accepted_cnts_->SetBinContent(i / 5 + 1, 4 - i % 5 + 1, accepted_cnts(i));
    }
    canvas_cnts_->cd(2);
    h_accepted_cnts_->Draw("COLZ");
}

void ComptonEdgeCalc::show_adc_per_kev() {
    if (!done_flag_)
        return;
    if (h_adc_per_kev_ != NULL) {
        delete h_adc_per_kev_;
        h_adc_per_kev_ = NULL;
    }
    gStyle->SetOptStat(0);
    canvas_adc_ = static_cast<TCanvas*>(gROOT->FindObject("canvas_adc"));
    if (canvas_adc_ == NULL) {
        canvas_adc_ = new TCanvas("canvas_adc", "ADC/keV of 1600 Channels", 800, 800);
        canvas_adc_->SetGrid();
    }
    h_adc_per_kev_ = new TH2F("h_adc_per_kev", "ADC/keV of 1600 Channels", 40, 0, 40, 40, 0, 40);
    h_adc_per_kev_->SetDirectory(NULL);
    h_adc_per_kev_->GetXaxis()->SetNdivisions(40);
    h_adc_per_kev_->GetXaxis()->SetNdivisions(40);
    char str_buffer[10];
    for (int i = 0; i < 40; i++) {
        if (i % 8 == 0) {
            sprintf(str_buffer, "%d", i);
            h_adc_per_kev_->GetXaxis()->SetBinLabel(i + 1, str_buffer);
            h_adc_per_kev_->GetYaxis()->SetBinLabel(i + 1, str_buffer);
        }
    }
    for (int i = 0; i < 25; i++) {
        for (int j = 0; j < 64; j++) {
            h_adc_per_kev_->SetBinContent(ijtox(i, j) + 1, ijtoy(i, j) + 1, adc_per_kev[i](j));
        }
    }
    canvas_adc_->cd();
    h_adc_per_kev_->Draw("COLZ");
}

void ComptonEdgeCalc::CloseWindow() {
    cout << "Quitting by user request." << endl;
    if (spec_file_ != NULL)
        close();
    gApplication->Terminate(0);
}

void ComptonEdgeCalc::ProcessAction(Int_t event,
                                    Int_t px,
                                    Int_t py,
                                    TObject* selected) {
    if (event != kButton1Down)
        return;
    TString title = selected->GetTitle();
    canvas_cnts_ = static_cast<TCanvas*>(gROOT->FindObject("canvas_cnts"));
    if (canvas_cnts_ == NULL)
        return;
    int x, y;
    if (title == "Trigger Counts of 1600 Channels") {
        x = static_cast<int>(canvas_cnts_->GetPad(1)->AbsPixeltoX(px));
        y = static_cast<int>(canvas_cnts_->GetPad(1)->AbsPixeltoY(py));
    } else {
        return;
    }
    show_spec(xytoi(x, y) + 1);
}

bool ComptonEdgeCalc::write_kvec(const char* filename) {
    TFile* tfile = new TFile(filename, "RECREATE");
    if (tfile->IsZombie())
        return false;
    tfile->cd();
    for (int i = 0; i < 25; i++) {
        sprintf(name_, "adc_per_kev_%d", i + 1);
        adc_per_kev[i].Write(name_);
    }
    tfile->Close();
    delete tfile;
    return true;
}

bool ComptonEdgeCalc::read_kvec(const char* filename) {
    TFile* tfile = new TFile(filename, "READ");
    if (tfile->IsZombie())
        return false;
    tfile->cd();
    for (int i = 0; i < 25; i++) {
        sprintf(name_, "adc_per_kev_%d", i + 1);
        adc_per_kev[i] = *(static_cast<TVectorF*>(tfile->Get(name_)));
    }
    tfile->Close();
    delete tfile;
    
    done_flag_ = true;
    return true;
}

