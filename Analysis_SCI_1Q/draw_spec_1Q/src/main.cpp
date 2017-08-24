#include <iostream>
#include "OptionsManager.hpp"
#include "POLEvent.hpp"
#include "CooConv.hpp"
#include "SpectrumCanvas.hpp"

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

    TApplication* rootapp = new TApplication("POLAR", NULL, NULL);

    double vthr_mean_0 = 150.0;
    double vthr_sigma_0 = 15.0;
    double vthr_max = 4096;
    double vthr_min = -1024.0;

    if (options_mgr.energy_flag) {
        vthr_mean_0 = 10.0;
        vthr_sigma_0 = 30.0;
        vthr_max = 512;
        vthr_min = -128;
    }

    // open pol_event file
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
    if (options_mgr.begin_met_time > 0) {
        begin_entry = t_pol_event.find_entry(t_pol_event_tree, options_mgr.begin_met_time);
        if (begin_entry < 0) {
            cout << "cannot find begin_entry." << endl;
            return 1;
        }
    }
    Long64_t end_entry = t_pol_event_tree->GetEntries();
    if (options_mgr.end_met_time > 0) {
        end_entry = t_pol_event.find_entry(t_pol_event_tree, options_mgr.end_met_time);
        if (end_entry < 0) {
            cout << "cannot find end_entry." << endl;
            return 1;
        }
    }
    t_pol_event.active(t_pol_event_tree, "event_time");
    t_pol_event.active(t_pol_event_tree, "time_aligned");
    t_pol_event.active(t_pol_event_tree, "is_ped");
    t_pol_event.active(t_pol_event_tree, "trigger_bit");
    t_pol_event.active(t_pol_event_tree, "energy_value");
    t_pol_event.active(t_pol_event_tree, "channel_status");
    t_pol_event.active(t_pol_event_tree, "multiplicity");
    t_pol_event_tree->GetEntry(begin_entry);
    int begin_time = static_cast<int>(t_pol_event.event_time);
    t_pol_event_tree->GetEntry(end_entry - 1);
    int end_time = static_cast<int>(t_pol_event.event_time);
    cout << options_mgr.pol_event_filename.Data()
         << " { " << begin_time << "[" << begin_entry << "] => "
         << end_time << "[" << end_entry - 1 << "] }" << endl;


    int mod_count[25];
    for (int i = 0; i < 25; i++) mod_count[i] = 0;
    TH1D* tri_spec[25][64];
    TH1D* all_spec[25][64];
    TH1D* ped_spec[25][64];
    TF1*  fun_spec[25][64];
    for (int i = 0; i < 25; i++) {
        for (int j = 0; j < 64; j++) {
            tri_spec[i][j] = new TH1D(Form("tri_spec_%02d_%02d", i + 1, j + 1), Form("tri_spec_%02d_%02d", i + 1, j + 1), 128, vthr_min, vthr_max);
            tri_spec[i][j]->SetDirectory(NULL);
            tri_spec[i][j]->SetLineColor(kRed);
            all_spec[i][j] = new TH1D(Form("all_spec_%02d_%02d", i + 1, j + 1), Form("all_spec_%02d_%02d", i + 1, j + 1), 128, vthr_min, vthr_max);
            all_spec[i][j]->SetDirectory(NULL);
            all_spec[i][j]->SetLineColor(kBlue);
            ped_spec[i][j] = new TH1D(Form("ped_spec_%02d_%02d", i + 1, j + 1), Form("ped_spec_%02d_%02d", i + 1, j + 1), 128, vthr_min, vthr_max);
            ped_spec[i][j]->SetDirectory(NULL);
            ped_spec[i][j]->SetLineColor(kGreen);
            fun_spec[i][j] = new TF1(Form("fun_spec_%02d_%02d", i + 1, j + 1), "(TMath::Erf((x - [0]) / TMath::Sqrt(2) / [1]) + 1.0) / 2.0", vthr_min, vthr_max);
            fun_spec[i][j]->SetParameters(vthr_mean_0, vthr_sigma_0);
        }
    }

    int pre_percent = 0;
    int cur_percent = 0;
    cout << "reading data ..." << endl;
    cout << "[ " << flush;
    for (Long64_t q = begin_entry; q < end_entry; q++) {
        cur_percent = static_cast<int>((q - begin_entry) * 100.0 / (end_entry - begin_entry));
        if (cur_percent - pre_percent > 0 && cur_percent % 2 == 0) {
            pre_percent = cur_percent;
            cout << "#" << flush;
        }
        t_pol_event_tree->GetEntry(q);

        if (t_pol_event.is_ped) {
            for (int i = 0; i < 25; i++) {
                if (!t_pol_event.time_aligned[i]) continue;
                mod_count[i]++;
                for (int j = 0; j < 64; j++) {
                    ped_spec[i][j]->Fill(t_pol_event.energy_value[i][j]);
                }
            }
            continue;
        }
        for (int i = 0; i < 25; i++) {
            if (!t_pol_event.time_aligned[i]) continue;
            mod_count[i]++;
            for (int j = 0; j < 64; j++) {
                if (t_pol_event.channel_status[i][j] > 0 && t_pol_event.channel_status[i][j] != 0x4) continue;
                if (t_pol_event.multiplicity[i] - t_pol_event.trigger_bit[i][j] < 2) continue;
                all_spec[i][j]->Fill(t_pol_event.energy_value[i][j]);
                if (t_pol_event.trigger_bit[i][j]) {
                    tri_spec[i][j]->Fill(t_pol_event.energy_value[i][j]);
                }
            }
        }

    }
    cout << " DONE ]" << endl;
    pol_event_file->Close();
    delete pol_event_file;
    pol_event_file = NULL;

    TH1D* tri_eff[25][64];
    for (int i = 0; i < 25; i++) {
        for (int j = 0; j < 64; j++) {
            tri_eff[i][j] = static_cast<TH1D*>(tri_spec[i][j]->Clone(Form("tri_eff_%02d_%02d", i + 1, j + 1)));
            tri_eff[i][j]->SetLineColor(kAzure);
            tri_eff[i][j]->SetTitle(Form("tri_eff_%02d_%02d", i + 1, j + 1));
            tri_eff[i][j]->Divide(all_spec[i][j]);
            // for (int k = 1; k < tri_eff[i][j]->GetNbinsX(); k++) {
            //     if (tri_eff[i][j]->GetBinCenter(k) < 0) {
            //         tri_eff[i][j]->SetBinContent(k, 0);
            //     } else {
            //         break;
            //     }
            // }
        }
    }


    // drawing
    gStyle->SetOptStat(0);

    SpectrumCanvas spec_canvas;
    spec_canvas.set_tri_spec(tri_spec);
    spec_canvas.set_all_spec(all_spec);
    spec_canvas.set_ped_spec(ped_spec);
    spec_canvas.set_tri_eff(tri_eff);
    spec_canvas.set_fun_spec(fun_spec);
    spec_canvas.set_count_map(mod_count);

    spec_canvas.draw_count_map();

    rootapp->Run();
    return 0;
}
