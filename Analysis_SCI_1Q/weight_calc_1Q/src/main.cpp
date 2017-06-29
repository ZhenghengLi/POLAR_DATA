#include <iostream>
#include <fstream>
#include <sstream>
#include "RootInc.hpp"
#include "OptionsManager.hpp"
#include "POLEvent.hpp"
#include "BarPos.hpp"

using namespace std;

int main(int argc, char** argv) {
    OptionsManager options_mgr;
    if (!options_mgr.parse(argc, argv)) {
        if (options_mgr.get_version_flag()) {
            options_mgr.print_version();
        } else {
            options_mgr.print_help();
        }
        return 2;
    }

    bool bar_mask[25][64];
    for (int i = 0; i < 25; i++) {
        for (int j = 0; j < 64; j++) {
            bar_mask[i][j] = false;
        }
    }
    // read bar_mask
    if (!options_mgr.bar_mask_filename.IsNull()) {
        char line_buffer[100];
        ifstream infile;
        infile.open(options_mgr.bar_mask_filename.Data());
        if (!infile.is_open()) {
            cout << "bar_mask_file open failed." << endl;
            return 1;
        }
        stringstream ss;
        int ct_num;
        int ch_idx;
        while (true) {
            infile.getline(line_buffer, 100);
            if (infile.eof()) break;
            if (string(line_buffer).find("#") != string::npos) {
                continue;
            } else {
                ss.clear();
                ss.str(line_buffer);
                ss >> ct_num >> ch_idx;
                if (ct_num < 1 || ct_num > 25) {
                    cout << "ct_num out of range" << endl;
                    return 1;
                }
                if (ch_idx < 0 || ch_idx > 63) {
                    cout << "ch_idx out of range" << endl;
                    return 1;
                }
                cout << "kill bar ct_" << ct_num << ", ch_" << ch_idx << endl;
                bar_mask[ct_num - 1][ch_idx] = true;
            }
        }
        infile.close();
    }

    // open pol_event_file
    TFile* pol_event_file = new TFile(options_mgr.pol_event_filename.Data(), "read");
    if (pol_event_file->IsZombie()) {
        cout << "pol_event_file open failed: " << options_mgr.pol_event_filename.Data() << endl;
        return 1;
    }
    TTree* t_pol_event_tree = static_cast<TTree*>(pol_event_file->Get("t_pol_event"));
    if (t_pol_event_tree == NULL) {
        cout << "cannot find TTree t_pol_event." << endl;
        return 1;
    }
    POLEvent t_pol_event;
    t_pol_event.bind_pol_event_tree(t_pol_event_tree);
    t_pol_event.deactive_all(t_pol_event_tree);
    Long64_t begin_entry = 0;
    Long64_t end_entry = t_pol_event_tree->GetEntries();
    t_pol_event.active(t_pol_event_tree, "event_time");
    t_pol_event.active(t_pol_event_tree, "time_aligned");
    t_pol_event.active(t_pol_event_tree, "trigger_bit");
    t_pol_event.active(t_pol_event_tree, "energy_value");

    // open vthr_file
    cout << "reading vthr file and calculating cut value ..." << endl;
    TFile* vthr_file = new TFile(options_mgr.vthr_filename.Data(), "read");
    TVectorF vthr_adc_value_CT_[25];
    TVectorF vthr_adc_sigma_CT_[25];
    TVectorF* tmp_vec;
    for (int i = 0; i < 25; i++) {
        vthr_adc_value_CT_[i].ResizeTo(64);
        vthr_adc_sigma_CT_[i].ResizeTo(64);
        tmp_vec = static_cast<TVectorF*>(vthr_file->Get(Form("vthr_adc_value_CT_%02d", i + 1)));
        if (tmp_vec == NULL) {
            cout << "cannot find TVector " << Form("vthr_adc_value_CT_%02d", i + 1) << endl;
        } else {
            vthr_adc_value_CT_[i] = *tmp_vec;
        }
        tmp_vec = static_cast<TVectorF*>(vthr_file->Get(Form("vthr_adc_sigma_CT_%02d", i + 1)));
        if (tmp_vec == NULL) {
            cout << "cannot find TVector " << Form("vthr_adc_sigma_CT_%02d", i + 1) << endl;
        } else {
            vthr_adc_sigma_CT_[i] = *tmp_vec;
        }
    }
    vthr_file->Close();
    TF1* eff_fun_CT_[25][64];
    for (int i = 0; i < 25; i++) {
        for (int j = 0; j < 64; j++) {
            eff_fun_CT_[i][j] = new TF1(Form("eff_fun_CT_%02d_%02d", i + 1, j + 1), "(TMath::Erf((x - [0]) / TMath::Sqrt(2) / [1]) + 1.0) / 2.0", -128, 4096);
            eff_fun_CT_[i][j]->SetParameters(vthr_adc_value_CT_[i](j), vthr_adc_sigma_CT_[i](j));
        }
    }

    // calculate cut_value;
    double vthr_value[1600];
    double cut_value = 0;
    int n = 0;
    for (int i = 0; i < 25; i++) {
        for (int j = 0; j < 64; j++) {
            if (bar_mask[i][j]) continue;
            vthr_value[n++] = vthr_adc_value_CT_[i](j);
        }
    }
    cut_value = TMath::Median(n, vthr_value);
    cout << "vthr cut_value = " << cut_value << endl;

    // open weight file
    TFile* pol_weight_file = new TFile(options_mgr.output_filename.Data(), "recreate");
    if (pol_weight_file->IsZombie()) {
        cout << "pol_weight_file open failed." << endl;
        return 1;
    }
    struct {
        Double_t event_time_2;
        Float_t  ch_weight[25][64];
        Float_t  mod_weight[25];
        Float_t  event_weight;
    } t_pol_weight;
    TTree* t_pol_weight_tree = new TTree("t_pol_weight", "weight in channel, module and event level");
    t_pol_weight_tree->Branch("event_time_2",     &t_pol_weight.event_time_2,     "event_time_2/D"         );
    t_pol_weight_tree->Branch("ch_weight",         t_pol_weight.ch_weight,        "ch_weight[25][64]/F"    );
    t_pol_weight_tree->Branch("mod_weight",        t_pol_weight.mod_weight,       "mod_weight[25]/F"       );
    t_pol_weight_tree->Branch("event_weight",     &t_pol_weight.event_weight,     "event_weight/F"         );

    t_pol_event_tree->GetEntry(begin_entry);
    int begin_time = static_cast<int>(t_pol_event.event_time);
    t_pol_event_tree->GetEntry(end_entry - 1);
    int end_time = static_cast<int>(t_pol_event.event_time);
    cout << options_mgr.pol_event_filename.Data()
         << " { " << begin_time << "[" << begin_entry << "] => "
         << end_time << "[" << end_entry - 1 << "] }" << endl;

    // calculate angle
    double min_eff = 0.1;
    double tmp_mod_weight;
    double tmp_event_pol_weight;
    int cur_percent = 0;
    int pre_percent = 0;
    cout << "reading data and calculating angle ... " << endl;
    cout << "[ " << flush;
    for (Long64_t q = begin_entry; q < end_entry; q++) {
        cur_percent = static_cast<int>(q * 100.0 / (end_entry - begin_entry));
        if (cur_percent - pre_percent > 0 && cur_percent % 2 == 0) {
            pre_percent = cur_percent;
            cout << "#" << flush;
        }
        t_pol_event_tree->GetEntry(q);

        t_pol_weight.event_time_2 = t_pol_event.event_time;
        t_pol_weight.event_weight = 0;
        for (int i = 0; i < 25; i++) {
            t_pol_weight.mod_weight[i] = 0;
            for (int j = 0; j < 64; j++) {
                t_pol_weight.ch_weight[i][j] = 0;
            }
        }
        tmp_event_pol_weight = 1.0;
        int sum_mod = 0;
        for (int i = 0; i < 25; i++) {
            if (!t_pol_event.time_aligned[i]) {
                continue;
            }
            tmp_mod_weight = 1.0;
            int sum_ch = 0;
            for (int j = 0; j < 64; j++) {
                if (!t_pol_event.trigger_bit[i][j]) continue;
                if (t_pol_event.trigger_bit[i][j] && bar_mask[i][j]) continue;
                double ch_eff = eff_fun_CT_[i][j]->Eval(t_pol_event.energy_value[i][j]);
                if (ch_eff < min_eff || t_pol_event.energy_value[i][j] < cut_value) {
                    t_pol_weight.ch_weight[i][j] = 0.0;
                } else {
                    t_pol_weight.ch_weight[i][j] = 1.0 / ch_eff;
                    tmp_mod_weight *= 1 / ch_eff;
                    sum_ch++;
                }
            }
            if (sum_ch > 0) {
                t_pol_weight.mod_weight[i] = tmp_mod_weight;
                tmp_event_pol_weight *= tmp_mod_weight;
                sum_mod++;
            } else {
                t_pol_weight.mod_weight[i] = 0.0;
            }
        }
        if (sum_mod > 0) {
            t_pol_weight.event_weight = tmp_event_pol_weight;
        } else {
            t_pol_weight.event_weight = 0.0;
        }
        t_pol_weight_tree->Fill();

    }
    cout << " DONE ]" << endl;

    pol_weight_file->cd();
    t_pol_weight_tree->Write();
    pol_weight_file->Close();

    pol_event_file->Close();

    return 0;
}
