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

    TFile* t_decoded_file = new TFile(options_mgr.decoded_data_filename.Data(), "read");
    if (t_decoded_file->IsZombie()) {
        cout << "root file open failed: " << options_mgr.decoded_data_filename.Data() << endl;
        return 1;
    }

    // open t_trigger
    TTree* t_trigger_tree = static_cast<TTree*>(t_decoded_file->Get("t_trigger"));
    if (t_trigger_tree == NULL) {
        cout << "Cannot find TTree: t_trigger." << endl;
        return 1;
    }

    if (options_mgr.entry >= t_trigger_tree->GetEntries()) {
        cout << "the input entry number is too large, it should be less than " << t_trigger_tree->GetEntries() << endl;
        return 1;
    }

 	struct {
        Long64_t pkt_start;
        Int_t pkt_count;
        Int_t lost_count;
    } t_trigger;

    t_trigger_tree->SetBranchAddress("pkt_start", &t_trigger.pkt_start);
    t_trigger_tree->SetBranchAddress("pkt_count", &t_trigger.pkt_count);
    t_trigger_tree->SetBranchAddress("lost_count", &t_trigger.lost_count);

    // open t_modules
    TTree* t_modules_tree = static_cast<TTree*>(t_decoded_file->Get("t_modules"));
    if (t_modules_tree == NULL) {
        cout << "Cannot find TTree: t_modules." << endl;
        return 1;
    }

    struct {
        Int_t ct_num;
        Bool_t trigger_bit[64];
        Float_t energy_adc[64];
    } t_modules;

    t_modules_tree->SetBranchAddress("ct_num", &t_modules.ct_num);
    t_modules_tree->SetBranchAddress("trigger_bit", t_modules.trigger_bit);
    t_modules_tree->SetBranchAddress("energy_adc", t_modules.energy_adc);

    // draw event
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

    t_trigger_tree->GetEntry(options_mgr.entry);
    if (t_trigger.pkt_start < 0) {
        cout << "The selected event is bad, do not draw it." << endl;
        return 1;
    }
    if (t_trigger.lost_count > 0) {
        cout << "Warning: there is lost packet." << endl;
    }

    for (Long64_t q = t_trigger.pkt_start; q < t_trigger.pkt_start + t_trigger.pkt_count; q++) {
        t_modules_tree->GetEntry(q);
        int i = t_modules.ct_num - 1;
        for (int j = 0; j < 64; j++) {
            trigger_map->SetBinContent(ijtox(i, j) + 1, ijtoy(i, j) + 1, static_cast<int>(t_modules.trigger_bit[j]));
            energy_map->SetBinContent(ijtox(i, j) + 1, ijtoy(i, j) + 1, static_cast<int>(t_modules.energy_adc[j]));
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
