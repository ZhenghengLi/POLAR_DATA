#include <iostream>
#include "OptionsManager.hpp"
#include "RootInc.hpp"
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
        return 1;
    }

    TApplication* rootapp = new TApplication("POLAR", NULL, NULL);

    TFile* t_event_file = new TFile(options_mgr.event_data_filename.Data(), "read");
    if (t_event_file->IsZombie()) {
        cout << "root file open failed: " << options_mgr.event_data_filename.Data() << endl;
        return 1;
    }

    TTree* t_rec_event_tree = static_cast<TTree*>(t_event_file->Get("t_rec_event"));
    if (t_rec_event_tree == NULL) {
        cout << "Cannot find TTree: t_rec_event." << endl;
        return 1;
    }

    if (options_mgr.entry >= t_rec_event_tree->GetEntries()) {
        cout << "the input entry number is too large, it should be less than " << t_rec_event_tree->GetEntries() << endl;
        return 1;
    }

 	struct {
        Int_t    abs_gps_week;
        Double_t abs_gps_second;
        Bool_t   abs_gps_valid;
        Double_t abs_ship_second;
        Int_t    type;
        Bool_t   trig_accepted[25];
        Bool_t   trigger_bit[1600];
        Int_t    trigger_n;
        Int_t    multiplicity[25];
        Float_t  energy_dep[1600];
    } t_rec_event;

    t_rec_event_tree->SetBranchAddress("trig_accepted", t_rec_event.trig_accepted);
    t_rec_event_tree->SetBranchAddress("trigger_bit", t_rec_event.trigger_bit);
    t_rec_event_tree->SetBranchAddress("energy_dep", t_rec_event.energy_dep);

    t_rec_event_tree->GetEntry(options_mgr.entry);

    TH2F* trigger_map = new TH2F("trigger_map", "Trigger Map", 40, 0, 40, 40, 0, 40);
    trigger_map->SetDirectory(NULL);
    trigger_map->GetXaxis()->SetNdivisions(40);
    trigger_map->GetYaxis()->SetNdivisions(40);
    for (int i = 0; i < 40; i++) {
        if (i % 8 == 0) {
            trigger_map->GetXaxis()->SetBinLabel(i + 1, Form("%02d", i));
            trigger_map->GetYaxis()->SetBinLabel(i + 1, Form("%02d", i));
        }
    }

    TH2F* energy_map = new TH2F("energy_map", "Energy Map", 40, 0, 40, 40, 0, 40);
    energy_map->SetDirectory(NULL);
    energy_map->GetXaxis()->SetNdivisions(40);
    energy_map->GetYaxis()->SetNdivisions(40);
    for (int i = 0; i < 40; i++) {
        if (i % 8 == 0) {
            energy_map->GetXaxis()->SetBinLabel(i + 1, Form("%02d", i));
            energy_map->GetYaxis()->SetBinLabel(i + 1, Form("%02d", i));
        }   
    }

    for (int i = 0; i < 25; i++) {
        if (!t_rec_event.trig_accepted[i]) {
            continue;
        }
        for (int j = 0; j < 64; j++) {
            int k = i * 64 + j;
            trigger_map->SetBinContent(ijtox(i, j) + 1, ijtoy(i, j) + 1, static_cast<int>(t_rec_event.trigger_bit[k]));
            energy_map->SetBinContent(ijtox(i, j) + 1, ijtoy(i, j) + 1, 
                    (t_rec_event.energy_dep[k] > 0 ? t_rec_event.energy_dep[k] : 0 ));
        }
    }

    gStyle->SetOptStat(0);

    TCanvas* canvas_map = new TCanvas("canvas_map", "Event Map", 1600, 800);
    canvas_map->Divide(2, 1);
    canvas_map->cd(1);
    trigger_map->Draw("lego2");
    canvas_map->cd(2);
    energy_map->Draw("lego2");

    rootapp->Run();

    return 0;
}
