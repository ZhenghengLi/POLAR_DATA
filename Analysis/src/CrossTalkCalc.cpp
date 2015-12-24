#include "CrossTalkCalc.hpp"

CrossTalkCalc::CrossTalkCalc() {
    xtalk_file_ = NULL;
    for (int i = 0; i < 25; i++) {
        for (int jx = 0; jx < 64; jx++) {
            for (int jy = 0; jy < 64; jy++) {
                g_xtalk_[i][jx][jy] = NULL;
                f_xtalk_[i][jx][jy] = NULL;
                g_n_[i][jx][jy] = 0;
            }
        }
    }
    h_xtalk_res_ = NULL;
    h_xtalk_mod_ = NULL;
    canvas_res_ = NULL;
    canvas_mod_ = NULL;
    canvas_cha_ = NULL;
    done_flag_ = false;
}

CrossTalkCalc::~CrossTalkCalc() {

}

bool CrossTalkCalc::open(const char* filename, char m) {
    if (m == 'w' || m == 'W')
        mode_ = 'w';
    else if (m == 'r' || m == 'R')
        mode_ = 'r';
    else
        return false;

    if (mode_ == 'r')
        xtalk_file_ = new TFile(filename, 'READ');
    else
        xtalk_file_ = new TFile(filename, "UPDATE");
    if (xtalk_file_->IsZombie())
        return false;

    for (int i = 0; i < 25; i++) {
        for (int jx = 0; jx < 64; jx++) {
            for (int jy = 0; jy < 64; jy++) {
                if (jx == jy)
                    continue;
                if (mode_ == 'r') {
                    sprintf(name_, "f_xtalk_%d_f%dt%d", i + 1, jx + 1, jy + 1);
                    f_xtalk_[i][jx][jy] = new TF1(name_, "[0] * x", 0, 4096);
                    f_xtalk_[i][jx][jy]->SetParName(0, "k");
                }
                sprintf(name_, "g_xtalk_%d_f%dt%d", i + 1, jx + 1, jy + 1);
                g_xtalk_[i][jx][jy] = static_cast<TGraph*>(xtalk_file_->Get(name_));
                if (g_xtalk_[i][jx][jy] == NULL) {
                    if (mode_ == 'r') {
                        return false;
                    } else {
                        sprintf(title_, "Cross Talk of CT %d: %d => %d", i + 1, jx + 1, jy + 1);
                        g_xtalk_[i][jx][jy] = new TGraph();
                        g_n_[i][jx][jy] = 0;
                        g_xtalk_[i][jx][jy]->SetName(name_);
                        g_xtalk_[i][jx][jy]->SetTitle(title_);
                        g_xtalk_[i][jx][jy]->SetMarkerColor(9);
                        g_xtalk_[i][jx][jy]->SetMaximum(2048);
                    }
                } else {
                    g_n_[i][jx][jy] = g_xtalk_[i][jx][jy]->GetN();
                }
            }
        }
    }
    done_flag_ = false;
    return true;
}

void CrossTalkCalc::close() {
    if (xtalk_file_ == NULL)
        return;
    for (int i = 0; i < 25; i++) {
        for (int jx = 0; jx < 64; jx++) {
            for (int jy = 0; jy < 64; jy++) {
                if (jx == jy)
                    continue;
                if (mode_ == 'r') {
                    delete f_xtalk_[i][jx][jy];
                    f_xtalk_[i][jx][jy] = NULL;
                }
                if (mode_ == 'w')
                    g_xtalk_[i][jx][jy]->Write();
                delete g_xtalk_[i][jx][jy];
                g_xtalk_[i][jx][jy] = NULL;
            }
        }
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
                        g_xtalk_[idx][jx][jy]->SetPoint(g_n_[idx][jx][jy]++,
                                                        phy_event_file.event.energy_ch[jx],
                                                        phy_event_file.event.energy_ch[jy]);
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
    for (int i = 0; i < 25; i++) {
        for (int jx = 0; jx < 64; jx++) {
            for (int jy = 0; jy < 64; jy++) {
                if (jx == jy) {
                    xtalk_matrix[i](jy, jx) = 1.0;
                } else {
                    g_xtalk_[i][jx][jy]->Fit(f_xtalk_[i][jx][jy], "NQ");
                    xtalk_matrix[i](jy, jx) = f_xtalk_[i][jx][jy]->GetParameter(0);
                }
            }
        }
    }
    for (int i = 0; i < 25; i++) {
        xtalk_matrix_inv[i] = xtalk_matrix[i];
        xtalk_matrix_inv[i].Invert();
    }
    done_flag_ = true;
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
                               const PhyEventFile& phy_event_file_r) const {
    phy_event_file_w.event.trigg_index = phy_event_file_r.event.trigg_index;
    phy_event_file_w.event.mode = phy_event_file_r.event.mode;
    phy_event_file_w.event.ct_num = phy_event_file_r.event.ct_num;
    for (int i = 0; i < 64; i++)
        phy_event_file_w.event.trigger_bit[i] = phy_event_file_r.event.trigger_bit[i];
    int idx = phy_event_file_r.event.ct_num - 1;
    energy_vec.Use(64, phy_event_file_r.event.energy_ch);
    energy_vec *= xtalk_matrix_inv[idx];
    for (int j = 0; j < 64; j++)
        phy_event_file_w.event.energy_ch[j] = energy_vec(j);
}

void CrossTalkCalc::show(int ct_num) {

}

void CrossTalkCalc::show_xtalk() {

}
