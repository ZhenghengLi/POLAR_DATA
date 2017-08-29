#include <iostream>
#include "RootInc.hpp"

using namespace std;

int main(int argc, char** argv) {
    if (argc < 4) {
        cout << "USAGE: " << argv[0] << " <event.root> <vthr.root> <weight.root>" << endl;
        return 2;
    }
    string event_filename = argv[1];
    string vthr_filename = argv[2];
    string weight_filename = argv[3];

    // read event data
    TFile* event_file = new TFile(event_filename.c_str(), "read");
    if (event_file->IsZombie()) {
        cout << "event_file open failed." << endl;
        return 1;
    }
    TTree* t_event_tree = static_cast<TTree*>(event_file->Get("t_event"));
    struct {
        Double_t ct_time_second;
        Bool_t   time_aligned[25];
        Bool_t   trigger_bit[25][64];
        Float_t  energy_value[25][64];
    } t_event;
    t_event_tree->SetBranchAddress("ct_time_second", &t_event.ct_time_second );
    t_event_tree->SetBranchAddress("time_aligned",    t_event.time_aligned   );
    t_event_tree->SetBranchAddress("trigger_bit",     t_event.trigger_bit    );
    t_event_tree->SetBranchAddress("energy_value",    t_event.energy_value   );
    t_event_tree->SetBranchStatus("*", false);
    t_event_tree->SetBranchStatus("ct_time_second", true);
    t_event_tree->SetBranchStatus("time_aligned", true);
    t_event_tree->SetBranchStatus("trigger_bit", true);
    t_event_tree->SetBranchStatus("energy_value", true);

    // read vthr data
    TFile* vthr_file = new TFile(vthr_filename.c_str(), "read");
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

    // open weight file
    TFile* weight_file = new TFile(weight_filename.c_str(), "recreate");
    if (weight_file->IsZombie()) {
        cout << "weight_file open failed." << endl;
        return 1;
    }
    struct {
        Double_t ct_time_second_4;
        Float_t  ch_weight[25][64];
        Float_t  mod_weight[25];
        Float_t  event_weight;
    } t_weight;
    TTree* t_weight_tree = new TTree("t_weight", "weight in channel, module and event level");
    t_weight_tree->Branch("ct_time_second_4", &t_weight.ct_time_second_4, "ct_time_second_4/D"     );
    t_weight_tree->Branch("ch_weight",         t_weight.ch_weight,        "ch_weight[25][64]/F"    );
    t_weight_tree->Branch("mod_weight",        t_weight.mod_weight,       "mod_weight[25]/F"       );
    t_weight_tree->Branch("event_weight",     &t_weight.event_weight,     "event_weight/F"         );

    // calculate weight
    double min_eff = 0.1;
    double max_weight = 100.0;
    double tmp_mod_weight;
    double tmp_event_weight;
    int pre_percent = 0;
    int cur_percent = 0;
    cout << "reading data ..." << endl;
    cout << "[ " << flush;
    for (Long64_t q = 0; q < t_event_tree->GetEntries(); q++) {
        cur_percent = static_cast<int>(q * 100 / t_event_tree->GetEntries());
        if (cur_percent - pre_percent > 0 && cur_percent % 2 == 0) {
            pre_percent = cur_percent;
            cout << "#" << flush;
        }
        t_event_tree->GetEntry(q);
        t_weight.ct_time_second_4 = t_event.ct_time_second;
        t_weight.event_weight = 0;
        for (int i = 0; i < 25; i++) {
            t_weight.mod_weight[i] = 0;
            for (int j = 0; j < 64; j++) {
                t_weight.ch_weight[i][j] = 0;
            }
        }

        bool is_bad = false;
        for (int i = 0; i < 25; i++) {
            if (t_event.time_aligned[i]) {
                if (i == 1 || i == 7 || i == 8 || i == 3) { // need to change according to real data
                    is_bad = true;
                }
                for (int j = 0; j < 64; j++) {
                    if (i == 5 && t_event.trigger_bit[i][j]) {
                        if (j == 13 || j == 5 || j == 12 || j == 4) {
                            is_bad = true;
                        }
                    }
                }
            }
        }
        if (is_bad) {
            t_weight_tree->Fill();
            continue;
        }

        tmp_event_weight = 1.0;
        int sum_mod = 0;
        for (int i = 0; i < 25; i++) {
            if (!t_event.time_aligned[i]) {
                continue;
            }
            tmp_mod_weight = 1.0;
            int sum_ch = 0;
            for (int j = 0; j < 64; j++) {
                if (!t_event.trigger_bit[i][j]) continue;
                double ch_eff = eff_fun_CT_[i][j]->Eval(t_event.energy_value[i][j]);
                if (ch_eff < min_eff) {
                    t_weight.ch_weight[i][j] = 0.0;
                } else {
                    t_weight.ch_weight[i][j] = 1.0 / ch_eff;
                    tmp_mod_weight *= 1 / ch_eff;
                    sum_ch++;
                }
            }
            if (sum_ch > 0 && tmp_mod_weight < max_weight) {
                t_weight.mod_weight[i] = tmp_mod_weight;
                tmp_event_weight *= tmp_mod_weight;
                sum_mod++;
            } else {
                t_weight.mod_weight[i] = 0.0;
            }
        }
        if (sum_mod > 0 && tmp_event_weight < max_weight) {
            t_weight.event_weight = tmp_event_weight;
        } else {
            t_weight.event_weight = 0.0;
        }
        t_weight_tree->Fill();

    }
    cout << " DONE ]" << endl;
    weight_file->cd();
    t_weight_tree->Write();
    weight_file->Close();

    return 0;
}
