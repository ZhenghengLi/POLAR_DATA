#include <iostream>
#include "RootInc.hpp"
#include "OptionsManager.hpp"
#include "CooConv.hpp"

#define PED_BINS 256
#define PED_MAX  1024

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

    // open ped_data_file
    TFile* ped_data_file = new TFile(options_mgr.ped_data_filename.Data(), "read");
    if (ped_data_file->IsZombie()) {
        cout << "ped_data_file open failed." << endl;
        return 1;
    }
    TTree* t_ped_data_tree = static_cast<TTree*>(ped_data_file->Get(Form("t_ped_data_ct_%02d", options_mgr.ct_num)));
    if (t_ped_data_tree == NULL) {
        cout << "cannot find TTree " << Form("t_ped_data_ct_%02d", options_mgr.ct_num) << endl;
        return 1;
    }
    struct {
        Double_t event_time;
        Bool_t   trigger_bit[64];
        Float_t  ped_adc[64];
        Float_t  fe_temp;
        Float_t  aux_interval;
    } t_ped_data;
    t_ped_data_tree->SetBranchAddress("event_time",     &t_ped_data.event_time     );
    t_ped_data_tree->SetBranchAddress("trigger_bit",     t_ped_data.trigger_bit    );
    t_ped_data_tree->SetBranchAddress("ped_adc",         t_ped_data.ped_adc        );
    t_ped_data_tree->SetBranchAddress("fe_temp",        &t_ped_data.fe_temp        );
    t_ped_data_tree->SetBranchAddress("aux_interval",   &t_ped_data.aux_interval   );
    cout << options_mgr.ped_data_filename.Data() << " { " << "ct_num = " << options_mgr.ct_num << " }" << endl;

    // open output file
    TFile* ped_result_file = new TFile(options_mgr.ped_result_filename.Data(), "recreate");
    if (ped_result_file->IsZombie()) {
        cout << "ped_result_file open failed." << endl;
        return 1;
    }

    // prepare histogram
    TH1D* ped_hist[64];
    for (int j = 0; j < 64; j++) {
        ped_hist[j] = new TH1D(Form("ped_hist_%02d_%02d", options_mgr.ct_num, j + 1),
                Form("Pedestal of CH %02d_%02d", options_mgr.ct_num, j + 1),
                PED_BINS, 0, PED_MAX);
        ped_hist[j]->SetDirectory(ped_result_file);
    }
    TH1D* common_noise_hist;
    common_noise_hist = new TH1D(Form("common_noise_%02d", options_mgr.ct_num),
            Form("Common noise of Module CT_%02d", options_mgr.ct_num), 256, -128, 128);
    common_noise_hist->SetDirectory(ped_result_file);
    TH1D* ped_shift_hist[64];
    for (int j = 0; j < 64; j++) {
        ped_shift_hist[j] = new TH1D(Form("ped_shift_hist_%02d_%02d", options_mgr.ct_num, j + 1),
                Form("Pedestal shift of CH %02d_%02d", options_mgr.ct_num, j + 1),
                256, -128, 128);
        ped_shift_hist[j]->SetDirectory(ped_result_file);
    }
    TVectorF ped_mean_vec(64);
    TVectorF ped_sigma_vec(64);
    TVectorF ped_shift_mean_vec(64);
    TVectorF ped_shift_sigma_vec(64);
    Float_t  common_noise;

    // reading and selecting data
    int pre_percent = 0;
    int cur_percent = 0;
    cout << "reading and selecting data ... " << endl;
    cout << "[ " << flush;
    for (Long64_t q = 0; q < t_ped_data_tree->GetEntries(); q++) {
        cur_percent = static_cast<int>(q * 100.0 / t_ped_data_tree->GetEntries());
        if (cur_percent - pre_percent > 0 && cur_percent % 2 == 0) {
            pre_percent = cur_percent;
            cout << "#" << flush;
        }
        t_ped_data_tree->GetEntry(q);

    }

    return 0;
}
