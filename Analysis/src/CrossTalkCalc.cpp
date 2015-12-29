#include "CrossTalkCalc.hpp"

CrossTalkCalc::CrossTalkCalc() {
    xtalk_file_ = NULL;
    t_xtalk_ = NULL;
    h_xtalk_res_ = NULL;
    h_xtalk_mod_ = NULL;
    h_xtalk_cha_ = NULL;
    canvas_res_ = NULL;
    canvas_mod_ = NULL;
    canvas_cha_ = NULL;
    done_flag_ = false;
    for (int jx = 0; jx < 64; jx++) {
        for (int jy = 0; jy < 64; jy++) {
            h_xtalk_[jx][jy] = NULL;
            f_xtalk_[jx][jy] = NULL;
        }
    }
    f_xtalk_cha_ = NULL;
    for (int i = 0; i < 25; i++) {
        xtalk_matrix[i].ResizeTo(64, 64);
        xtalk_matrix_inv[i].ResizeTo(64, 64);
    }
    for (int i = 0; i < 4; i++) {
        lineH_[i] = NULL;
        lineV_[i] = NULL;
    }
}

CrossTalkCalc::~CrossTalkCalc() {
    if (xtalk_file_ != NULL)
        close();
    if (h_xtalk_mod_ != NULL) {
        delete h_xtalk_mod_;
        h_xtalk_mod_ = NULL;
    }
    if (h_xtalk_res_ != NULL) {
        delete h_xtalk_res_;
        h_xtalk_res_ = NULL;
    }
    if (h_xtalk_cha_ != NULL) {
        delete h_xtalk_cha_;
        h_xtalk_cha_ = NULL;
    }
}

bool CrossTalkCalc::open(const char* filename, char m) {
    if (m == 'w' || m == 'W')
        mode_ = 'w';
    else if (m == 'r' || m == 'R')
        mode_ = 'r';
    else
        return false;

    if (mode_ == 'r')
        xtalk_file_ = new TFile(filename, "READ");
    else
        xtalk_file_ = new TFile(filename, "UPDATE");
    if (xtalk_file_->IsZombie())
        return false;

    t_xtalk_ = static_cast<TTree*>(xtalk_file_->Get("t_xtalk"));
    if (t_xtalk_ == NULL) {
        if (mode_ == 'w') {
            t_xtalk_ = new TTree("t_xtalk", "Cross Talk Data of all modules");
            t_xtalk_->Branch("xtalk_point", &xtalk_point_.i, "i/I:jx/I:jy/I:x/F:y/F");
            t_xtalk_->SetDirectory(xtalk_file_);
        } else {
            return false;
        }
    } else {
        t_xtalk_->SetBranchAddress("xtalk_point", &xtalk_point_.i);
    }

    if (mode_ == 'r') {
        for (int jx = 0; jx < 64; jx++) {
            for (int jy = 0; jy < 64; jy++) {
                sprintf(name_, "f_xtalk_%d_%d", jx + 1, jy + 1);
                f_xtalk_[jx][jy] = new TF1(name_, "[0] * x", 0, 4096);
                f_xtalk_[jx][jy]->SetParameter(0, 0.1);
                sprintf(name_, "h_xtalk_%d_%d", jx + 1, jy + 1);
                sprintf(title_, "Cross Talk of %d_%d", jx + 1, jy + 1);
                h_xtalk_[jx][jy] = new TH2F(name_, title_, 256, 0, 4096, 128, 0, 1024);
                h_xtalk_[jx][jy]->SetDirectory(NULL);
            }
        }
        f_xtalk_cha_ = new TF1("f_xtalk_cha", "[0] * x", 0, 4096);
        f_xtalk_cha_->SetParameter(0, 0.1);
    }
    
    done_flag_ = false;
    return true;
}

void CrossTalkCalc::close() {
    if (xtalk_file_ == NULL)
        return;
    if (mode_ == 'w')
        t_xtalk_->Write();
    delete t_xtalk_;
    t_xtalk_ = NULL;
    if (mode_ == 'r') {
        for (int jx = 0; jx < 64; jx++) {
            for (int jy = 0; jy < 64; jy++) {
                delete h_xtalk_[jx][jy];
                h_xtalk_[jx][jy] = NULL;
                delete f_xtalk_[jx][jy];
                f_xtalk_[jx][jy] = NULL;
            }
        }
        delete f_xtalk_cha_;
        f_xtalk_cha_ = NULL;
    }
    xtalk_file_->Close();
    delete xtalk_file_;
    xtalk_file_ = NULL;
}

void CrossTalkCalc::do_fill(PhyEventFile& phy_event_file) {
    if (xtalk_file_ == NULL)
        return;
    if (mode_ != 'w')
        return;
    phy_event_file.trigg_restart();
    while (phy_event_file.trigg_next()) {
        while (phy_event_file.event_next()) {
            int idx = phy_event_file.event.ct_num - 1;
            for (int jx = 0; jx < 64; jx++) {
                if (!phy_event_file.event.trigger_bit[jx])
                    continue;
                for (int jy = 0; jy < 64; jy++) {
                    if (jx == jy)
                        continue;
                    if (phy_event_file.event.trigger_bit[jy])
                        continue;
                    if ((jy + 1 != jx && jy + 1 <= 63 && phy_event_file.event.trigger_bit[jy + 1]) ||
                        (jy - 1 != jx && jy - 1 >= 0  && phy_event_file.event.trigger_bit[jy - 1]) ||
                        (jy + 7 != jx && jy + 7 <= 63 && phy_event_file.event.trigger_bit[jy + 7]) ||
                        (jy + 8 != jx && jy + 8 <= 63 && phy_event_file.event.trigger_bit[jy + 8]) ||
                        (jy + 9 != jx && jy + 9 <= 63 && phy_event_file.event.trigger_bit[jy + 9]) ||
                        (jy - 7 != jx && jy - 7 >= 0  && phy_event_file.event.trigger_bit[jy - 7]) ||
                        (jy - 8 != jx && jy - 8 >= 0  && phy_event_file.event.trigger_bit[jy - 8]) ||
                        (jy - 9 != jx && jy - 9 >= 0  && phy_event_file.event.trigger_bit[jy - 9]))
                        continue;
                    if (phy_event_file.event.energy_ch[jy] / phy_event_file.event.energy_ch[jx] > 0.4)
                        continue;
                    if (phy_event_file.event.energy_ch[jy] > 0) {
                        xtalk_point_.i = idx;
                        xtalk_point_.jx = jx;
                        xtalk_point_.jy = jy;
                        xtalk_point_.x = phy_event_file.event.energy_ch[jx];
                        xtalk_point_.y = phy_event_file.event.energy_ch[jy];
                        t_xtalk_->Fill();
                    }
                }
            }
        }
    }
    phy_event_file.trigg_restart();
}

void CrossTalkCalc::do_fit() {
    if (xtalk_file_ == NULL)
        return;
    if (mode_ != 'r')
        return;
    
    Long64_t tot_entries = t_xtalk_->GetEntries();
    for (int i = 0; i < 25; i++) {
        cout << "Processing module CT_" << i + 1 << " ... " << endl;
        for (int jx = 0; jx < 64; jx++) {
            for (int jy = 0; jy < 64; jy++) {
                h_xtalk_[jx][jy]->Reset();
            }
        }
        for (Long64_t j = 0; j < tot_entries; j++) {
            t_xtalk_->GetEntry(j);
            if (xtalk_point_.i == i)
                h_xtalk_[xtalk_point_.jx][xtalk_point_.jy]->Fill(xtalk_point_.x, xtalk_point_.y);
        }
        for (int jx = 0; jx < 64; jx++) {
            for (int jy = 0; jy < 64; jy++) {
                if (jx == jy) {
                    xtalk_matrix[i](jy, jx) = 1.0;
                    continue;
                }
                if (h_xtalk_[jx][jy]->GetEntries() < 5) {
                    cerr << "CT_" << i + 1 << " : " << jx + 1 << " => " << jy + 1
                         << "    " << "number of entries is too small" << endl;
                    xtalk_matrix[i][jy][jx] = 0.0001;
                } else {
                    h_xtalk_[jx][jy]->Fit(f_xtalk_[jx][jy], "QN");
                    xtalk_matrix[i](jy, jx) = f_xtalk_[jx][jy]->GetParameter(0);
                }
            }
        }
        xtalk_matrix_inv[i] = xtalk_matrix[i];
        xtalk_matrix_inv[i].Invert();
    }

    done_flag_ = false;
}

void CrossTalkCalc::do_move_trigg(PhyEventFile& phy_event_file_w,
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
    phy_event_file_w.trigg.level_flag = 2;
}

void CrossTalkCalc::do_correct(PhyEventFile& phy_event_file_w,
                               const PhyEventFile& phy_event_file_r) {
    phy_event_file_w.event.trigg_index = phy_event_file_r.event.trigg_index;
    phy_event_file_w.event.mode = phy_event_file_r.event.mode;
    phy_event_file_w.event.ct_num = phy_event_file_r.event.ct_num;
    for (int i = 0; i < 64; i++)
        phy_event_file_w.event.trigger_bit[i] = phy_event_file_r.event.trigger_bit[i];
    int idx = phy_event_file_r.event.ct_num - 1;
    energy_vec.Use(64, phy_event_file_r.event.energy_ch);
    energy_vec *= xtalk_matrix_inv[idx];
    for (int j = 0; j < 64; j++)
        phy_event_file_w.event.energy_ch[j] = (energy_vec(j) > 0 ? energy_vec(j) : 0);
}

void CrossTalkCalc::show_mod(int ct_num) {
    if (!done_flag_)
        return;
    int idx = ct_num - 1;
    gStyle->SetOptStat(0);
    canvas_mod_ = static_cast<TCanvas*>(gROOT->FindObject("canvas_mod"));
    if (canvas_mod_ == NULL) {
        canvas_mod_ = new TCanvas("canvas_mod", "Cross Talk", 1000, 1000);
    }
    sprintf(title_, "Cross Talk Matrix of module CT_%d", ct_num);
    canvas_mod_->SetTitle(title_);
    h_xtalk_mod_ = static_cast<TH2F*>(gROOT->FindObject("h_xtalk_mod"));
    if (h_xtalk_mod_ == NULL) {
        h_xtalk_mod_ = new TH2F("h_xtalk_mod", title_, 64, 0, 64, 64, 0, 64);
        h_xtalk_mod_->SetDirectory(NULL);
    }
    h_xtalk_mod_->Reset();
    h_xtalk_mod_->SetTitle(title_);
    for (int jx = 0; jx < 64; jx++) {
        for (int jy = 0; jy < 64; jy++) {
            h_xtalk_mod_->SetBinContent(jx + 1, 64 - jy, xtalk_matrix[idx](jx, jy));
        }
    }
    canvas_mod_->cd();
    h_xtalk_mod_->Draw("LEGO2");
    canvas_mod_->Update();
}

void CrossTalkCalc::show_cha(int ct_num, int jx1, int jy1) {
    if (xtalk_file_ == NULL)
        return;
    if (mode_ != 'r')
        return;
    if (ct_num < 1 || ct_num > 25 || jx1 < 1 || jx1 > 64 || jy1 < 1 || jy1 > 64 || jx1 == jy1)
        return;
    int idx = ct_num - 1;
    int jx = jx1 - 1;
    int jy = jy1 - 1;
    h_xtalk_cha_ = static_cast<TH2F*>(gROOT->FindObject("h_xtalk_cha"));
    if (h_xtalk_cha_ == NULL) {
        h_xtalk_cha_ = new TH2F("h_xtalk_cha", "Cross Talk", 256, 0, 4096, 256, 0, 2048);
        h_xtalk_cha_->SetDirectory(NULL);
        h_xtalk_cha_->SetMarkerColor(9);
        h_xtalk_cha_->SetMarkerStyle(31);
    }
    sprintf(title_, "Cross Talk of CT_%d: %d => %d", ct_num , jx1, jy1);
    h_xtalk_cha_->SetTitle(title_);
    h_xtalk_cha_->Reset();
    
    Long64_t tot_entries = t_xtalk_->GetEntries();
    for (Long64_t i = 0; i < tot_entries; i++) {
        t_xtalk_->GetEntry(i);
        if (xtalk_point_.i == idx && xtalk_point_.jx == jx && xtalk_point_.jy == jy)
            h_xtalk_cha_->Fill(xtalk_point_.x, xtalk_point_.y);
    }

    canvas_cha_ = static_cast<TCanvas*>(gROOT->FindObject("canvas_cha"));
    if (canvas_cha_ == NULL) {
        canvas_cha_ = new TCanvas("canvas_cha", "Cross Talk", 1000, 600);
        canvas_cha_->Connect("Closed()", "CrossTalkCalc", this, "CloseWindow()");
    }
    canvas_cha_->SetTitle(title_);
    h_xtalk_cha_->Fit(f_xtalk_cha_);
    canvas_cha_->Update();
}

void CrossTalkCalc::show_xtalk() {
    if (!done_flag_)
        return;
    if (h_xtalk_res_ != NULL) {
        delete h_xtalk_res_;
        h_xtalk_res_ = NULL;
    }
    gStyle->SetOptStat(0);
    canvas_res_ = static_cast<TCanvas*>(gROOT->FindObject("canvas_res"));
    if (canvas_res_ == NULL) {
        canvas_res_ = new TCanvas("canvas_res", "Cross Talk Matrix of All Modules", 1000, 1000);
        canvas_res_->Connect("Closed()", "CrossTalkCalc", this, "CloseWindow()");
        canvas_res_->Connect("ProcessedEvent(Int_t,Int_t,Int_t,TObject*)", "CrossTalkCalc",
                             this, "ProcessAction(Int_t,Int_t,Int_t,TObject*)");
    }
    h_xtalk_res_ = new TH2F("h_xtalk_res", "Cross Talk Matrix of All Modules", 320, 0, 320, 320, 0, 320);
    h_xtalk_res_->SetDirectory(NULL);

    char str_buffer[10];
    for (int i = 0; i < 320; i++) {
        if (i % 64 == 0) {
            sprintf(str_buffer, "%d", i);
            h_xtalk_res_->GetXaxis()->SetBinLabel(i + 1, str_buffer);
            h_xtalk_res_->GetYaxis()->SetBinLabel(i + 1, str_buffer);
        }
    }

    for (int i = 0; i < 25; i++) {
        for (int jx = 0; jx < 64; jx++) {
            for (int jy = 0; jy < 64; jy++) {
                h_xtalk_res_->SetBinContent((i / 5) * 64 + jx + 1,
                                            (4 - i % 5) * 64 + (63 - jy) + 1,
                                            xtalk_matrix[i](jx, jy));
            }
        }
    }
    
    canvas_res_->cd();
    h_xtalk_res_->Draw("COLZ");

    for (int i = 0; i < 4; i++) {
        if (lineH_[i] != NULL)
            delete lineH_[i];
        lineH_[i] = new TLine(0, (i + 1) * 64, 320, (i + 1) * 64);
        lineH_[i]->SetLineColor(kWhite);
        lineH_[i]->Draw("SAME");
        if (lineV_[i] != NULL)
            delete lineV_[i];
        lineV_[i] = new TLine((i + 1) * 64, 0, (i + 1) * 64, 320);
        lineV_[i]->SetLineColor(kWhite);
        lineV_[i]->Draw("SAME");
    }
}

void CrossTalkCalc::CloseWindow() {
    cout << "Quitting by user request." << endl;
    gApplication->Terminate(0);
}

void CrossTalkCalc::ProcessAction(Int_t event,
                                      Int_t px,
                                      Int_t py,
                                      TObject* selected) {
    if (event != kButton1Down)
        return;
    canvas_res_ = static_cast<TCanvas*>(gROOT->FindObject("canvas_res"));
    if (canvas_res_ == NULL)
        return;
    int x = static_cast<int>(canvas_res_->AbsPixeltoX(px));
    int y = static_cast<int>(canvas_res_->AbsPixeltoY(py));
    show_mod(x / 64 * 5 + 4 - y / 64 + 1);
}

bool CrossTalkCalc::write_xmat(const char* filename) {
    TFile* tfile = new TFile(filename, "RECREATE");
    if (tfile->IsZombie())
        return false;
    tfile->cd();
    for (int i = 0; i < 25; i++) {
        sprintf(name_, "xtalk_matrix_%d", i + 1);
        xtalk_matrix[i].Write(name_);
    }
    tfile->Close();
    delete tfile;
    return true;
}

bool CrossTalkCalc::read_xmat(const char* filename) {
    TFile* tfile = new TFile(filename, "READ");
    if (tfile->IsZombie())
        return false;
    tfile->cd();
    for (int i = 0; i < 25; i++) {
        sprintf(name_, "xtalk_matrix_%d", i + 1);
        xtalk_matrix[i] = *(static_cast<TMatrixF*>(tfile->Get(name_)));
        xtalk_matrix_inv[i] = xtalk_matrix[i];
        xtalk_matrix_inv[i].Invert();
    }
    tfile->Close();
    delete tfile;

    done_flag_ = true;
    return true;
}
    
