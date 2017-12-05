#include <iostream>
#include "OptionsManager.hpp"
#include "POLEvent.hpp"
#include "CooConv.hpp"

using namespace std;

#define BIN_SIZE 4

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

    // open output file
    TFile* output_file = new TFile(options_mgr.output_filename.Data(), "recreate");
    if (output_file->IsZombie()) {
        cout << "output_file open failed: " << options_mgr.output_filename << endl;
        return 1;
    }
    TH2F* max_ADC_spec[25][64];
    for (int i = 0; i < 25; i++) {
        for (int j = 0; j <64; j++) {
            max_ADC_spec[i][j] = new TH2F(Form("max_ADC_spec_%02d_%02d", i + 1, j), Form("max_ADC_spec_%02d_%02d", i + 1, j),
                    768 / BIN_SIZE, 0, 768, 4096 / BIN_SIZE, 0, 4096);
        }
    }

    for (size_t i = 0; i < options_mgr.pol_event_filename_list.size(); i++) {
        TFile* pol_event_file = new TFile(options_mgr.pol_event_filename_list[i].Data(), "read");
        if (pol_event_file->IsZombie()) {
            cout << "pol_event_file open failed: " << options_mgr.pol_event_filename_list[i] << endl;
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
        t_pol_event.active(t_pol_event_tree, "time_aligned");
        t_pol_event.active(t_pol_event_tree, "is_ped");
        t_pol_event.active(t_pol_event_tree, "type");
        t_pol_event.active(t_pol_event_tree, "multiplicity");
        t_pol_event.active(t_pol_event_tree, "trigger_bit");
        t_pol_event.active(t_pol_event_tree, "energy_value");
        t_pol_event.active(t_pol_event_tree, "channel_status");
        t_pol_event.active(t_pol_event_tree, "common_noise");

        int pre_percent = 0;
        int cur_percent = 0;
        cout << "reading file: " << options_mgr.pol_event_filename_list[i] << " ..." << endl;
        cout << "[ " << flush;
        for (Long64_t q = 0; q < t_pol_event_tree->GetEntries(); q++) {
            cur_percent = static_cast<int>(q * 100.0 / t_pol_event_tree->GetEntries());
            if (cur_percent - pre_percent > 0 && cur_percent % 2 == 0) {
                pre_percent = cur_percent;
                cout << "#" << flush;
            }
            t_pol_event_tree->GetEntry(q);

            // skip pedestal event
            if (t_pol_event.is_ped) continue;
            // skip cosmic events
            if (t_pol_event.type == 0xFF00) continue;
            for (int i = 0; i < 25; i++) {
                if (!t_pol_event.time_aligned[i]) continue;
                // calculate max ADC
                double cur_max_ADC = 0;
                int cur_max_j = 0;
                for (int j = 0; j < 64; j++) {
                    if (cur_max_ADC < t_pol_event.energy_value[i][j]) {
                        cur_max_ADC = t_pol_event.energy_value[i][j];
                        cur_max_j = j;
                    }
                }
                if (!t_pol_event.trigger_bit[i][cur_max_j]) continue;
                for (int j = 0; j < 64; j++) {
                    if (t_pol_event.channel_status[i][j] > 0) continue;
                    if (j != cur_max_j && t_pol_event.trigger_bit[i][j]) {
                        max_ADC_spec[i][j]->Fill(t_pol_event.energy_value[i][j], cur_max_ADC);
                    }
                }
            }
        }
        cout << " DONE ]" << endl;

        pol_event_file->Close();
        delete pol_event_file;
        pol_event_file = NULL;

    }

    // save result
    gROOT->SetBatch(kTRUE);
    gStyle->SetOptStat(11);
    gStyle->SetOptFit(111);

    output_file->cd();
    for (int i = 0; i < 25; i++) {
        output_file->mkdir(Form("non_linearity_%02d", i + 1))->cd();
        for (int j = 0; j < 64; j++) {
            max_ADC_spec[i][j]->Write();
        }
    }
    output_file->Close();
    delete output_file;
    output_file = NULL;

    return 0;
}
