#include <iostream>
#include "RootInc.hpp"
#include "OptionsManager.hpp"
#include "CooConv.hpp"

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

    // open xtalk_data_file
    TFile* xtalk_data_file = new TFile(options_mgr.xtalk_data_filename.Data(), "read");
    if (xtalk_data_file->IsZombie()) {
        cout << "ped_data_file open failed." << endl;
        return 1;
    }
    TTree* t_xtalk_data_tree = static_cast<TTree*>(xtalk_data_file->Get(Form("t_xtalk_data_ct_%02d", options_mgr.ct_num)));
    if (t_xtalk_data_tree == NULL) {
        cout << "cannot find TTree " << Form("t_xtalk_data_ct_%02d", options_mgr.ct_num) << endl;
        return 1;
    }
    struct {
        Double_t event_time;
        Int_t    jx;
        Int_t    jy;
        Float_t  x;
        Float_t  y;
        Float_t  fe_temp;
        Float_t  fe_hv;
        Float_t  aux_interval;
    } t_xtalk_data;
    t_xtalk_data_tree->SetBranchAddress("event_time",     &t_xtalk_data.event_time     );
    t_xtalk_data_tree->SetBranchAddress("jx",             &t_xtalk_data.jx             );
    t_xtalk_data_tree->SetBranchAddress("jy",             &t_xtalk_data.jy             );
    t_xtalk_data_tree->SetBranchAddress("x",              &t_xtalk_data.x              );
    t_xtalk_data_tree->SetBranchAddress("y",              &t_xtalk_data.y              );
    t_xtalk_data_tree->SetBranchAddress("fe_temp",        &t_xtalk_data.fe_temp        );
    t_xtalk_data_tree->SetBranchAddress("fe_hv",          &t_xtalk_data.fe_hv          );
    t_xtalk_data_tree->SetBranchAddress("aux_interval",   &t_xtalk_data.aux_interval   );
    cout << options_mgr.xtalk_data_filename.Data() << " { " << "ct_num = " << options_mgr.ct_num << " }" << endl;

    // open output file
    TFile* xtalk_result_file = new TFile(options_mgr.xtalk_result_filename.Data(), "recreate");
    if (xtalk_result_file->IsZombie()) {
        cout << "xtalk_result_file open failed." << endl;
        return 1;
    }

    // prepare histogram

    // reading and selecting data
    int pre_percent = 0;
    int cur_percent = 0;
    cout << "reading and selecting data ... " << endl;
    cout << "[ " << flush;
    for (Long64_t q = 0; q < t_xtalk_data_tree->GetEntries(); q++) {
        cur_percent = static_cast<int>(q * 100.0 / t_xtalk_data_tree->GetEntries());
        if (cur_percent - pre_percent > 0 && cur_percent % 2 == 0) {
            pre_percent = cur_percent;
            cout << "#" << flush;
        }
        t_xtalk_data_tree->GetEntry(q);

        // filter start
        if (t_xtalk_data.aux_interval > 20) continue;
        if (t_xtalk_data.fe_temp < options_mgr.low_temp) continue;
        if (t_xtalk_data.fe_temp > options_mgr.high_temp) continue;
        if (t_xtalk_data.fe_hv < options_mgr.low_hv) continue;
        if (t_xtalk_data.fe_hv > options_mgr.high_hv) continue;
        // filter stop


    }
    cout << " DONE ]" << endl;

    gROOT->SetBatch(kTRUE);
    gErrorIgnoreLevel = kWarning;
    gStyle->SetOptStat(0);

    // draw and save result
    xtalk_result_file->cd();
    TNamed("ct_num", Form("%d", options_mgr.ct_num)).Write();


    TNamed("low_temp", Form("%f", options_mgr.low_temp)).Write();
    TNamed("high_temp", Form("%f", options_mgr.high_temp)).Write();
    TNamed("low_hv", Form("%f", options_mgr.low_hv)).Write();
    TNamed("high_temp", Form("%f", options_mgr.high_temp)).Write();

    return 0;
}
