#include <iostream>
#include <fstream>
#include <sstream>
#include "RootInc.hpp"
#include "OptionsManager.hpp"
#include "POLEvent.hpp"
#include "BarPos.hpp"
#include "PosConv.hpp"

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

    // open angle file
    struct {
        Double_t  event_time;
        Int_t     first_ij[2];
        Int_t     second_ij[2];
        Float_t   rand_angle;
        Float_t   rand_distance;
        Float_t   first_energy;
        Float_t   second_energy;
        Bool_t    is_adjacent;
        Bool_t    is_na22;
        Bool_t    is_bad_calib;
        Bool_t    with_badch;
        Float_t   weight;
        Float_t   latitude;
        Float_t   longitude;
    } t_pol_angle;

    TFile* angle_file = new TFile(options_mgr.angle_filename.Data(), "read");
    if (angle_file->IsZombie()) {
        cout << "angle_file open failed: " << options_mgr.angle_filename.Data() << endl;
        return 1;
    }
    TTree* t_pol_angle_tree = static_cast<TTree*>(angle_file->Get("t_pol_angle"));
    if (t_pol_angle_tree == NULL) {
        cout << "cannot find TTree t_pol_angle." << endl;
        return 1;
    }

    t_pol_angle_tree->SetBranchAddress("event_time",        &t_pol_angle.event_time           );
    t_pol_angle_tree->SetBranchAddress("first_ij",           t_pol_angle.first_ij             );
    t_pol_angle_tree->SetBranchAddress("second_ij",          t_pol_angle.second_ij            );
    t_pol_angle_tree->SetBranchAddress("rand_angle",        &t_pol_angle.rand_angle           );
    t_pol_angle_tree->SetBranchAddress("rand_distance",     &t_pol_angle.rand_distance        );
    t_pol_angle_tree->SetBranchAddress("first_energy",      &t_pol_angle.first_energy         );
    t_pol_angle_tree->SetBranchAddress("second_energy",     &t_pol_angle.second_energy        );
    t_pol_angle_tree->SetBranchAddress("is_adjacent",       &t_pol_angle.is_adjacent          );
    t_pol_angle_tree->SetBranchAddress("is_na22",           &t_pol_angle.is_na22              );
    t_pol_angle_tree->SetBranchAddress("is_bad_calib",      &t_pol_angle.is_bad_calib         );
    t_pol_angle_tree->SetBranchAddress("with_badch",        &t_pol_angle.with_badch           );
    t_pol_angle_tree->SetBranchAddress("weight",            &t_pol_angle.weight               );
    t_pol_angle_tree->SetBranchAddress("latitude",          &t_pol_angle.latitude             );
    t_pol_angle_tree->SetBranchAddress("longitude",         &t_pol_angle.longitude            );

    // read first/last time
    t_pol_angle_tree->GetEntry(0);
    double first_time = t_pol_angle.event_time;
    Long64_t total_entries = t_pol_angle_tree->GetEntries();
    t_pol_angle_tree->GetEntry(total_entries - 1);
    double last_time = t_pol_angle.event_time;

    // range check
    if (first_time > options_mgr.bkg_before_start) {
        cout << "bkg_before_start < first_time" << endl;
        return 1;
    }
    if (last_time < options_mgr.bkg_after_stop) {
        cout << "bkg_after_stop > last_time" << endl;
        return 1;
    }

    // print time range
    string grb_range = Form("GRB: %15.6f => %15.6f",
            options_mgr.grb_start,
            options_mgr.grb_stop);
    string bkg_range = Form("BKG: %15.6f => %15.6f; %15.6f => %15.6f",
            options_mgr.bkg_before_start,
            options_mgr.bkg_before_stop,
            options_mgr.bkg_after_start,
            options_mgr.bkg_after_stop);
    cout << grb_range << endl;
    cout << bkg_range << endl;

    // open output file
    TFile* output_file = new TFile(options_mgr.output_filename.Data(), "recreate");
    if (output_file->IsZombie()) {
        cout << "output_file open failed: " << options_mgr.output_filename.Data() << endl;
        return 1;
    }

    // prepare histograms
    TH1D* modcur_bkg = new TH1D("modcur_bkg", "modcur_bkg", options_mgr.nbins, 0, 360);
    TH1D* modcur_grb_with_bkg = new TH1D("modcur_grb_with_bkg", "modcur_grb_with_bkg", options_mgr.nbins, 0, 360);
    TH1D* modcur_grb_sub_bkg = new TH1D("modcur_grb_sub_bkg", "modcur_grb_sub_bkg", options_mgr.nbins, 0, 360);

    // read angle
    int pre_percent = 0;
    int cur_percent = 0;
    cout << "reading angle_data ... " << endl;
    cout << "[ " << flush;
    for (Long64_t q = 0; q < total_entries; q++) {
        cur_percent = static_cast<int>(q * 100.0 / (total_entries));
        if (cur_percent - pre_percent > 0 && cur_percent % 2 == 0) {
            pre_percent = cur_percent;
            cout << "#" << flush;
        }
        t_pol_angle_tree->GetEntry(q);
        // cut list
        if (t_pol_angle.is_na22) continue;
        if (t_pol_angle.is_bad_calib) continue;
        if (t_pol_angle.with_badch) continue;
        if (t_pol_angle.is_adjacent) continue;
        if (t_pol_angle.weight <= 0) continue;
        // fill angle
        if (t_pol_angle.event_time > options_mgr.grb_start && t_pol_angle.event_time < options_mgr.grb_stop) {
            modcur_grb_with_bkg->Fill(t_pol_angle.rand_angle, t_pol_angle.weight);
            continue;
        }
        if (t_pol_angle.event_time > options_mgr.bkg_before_start && t_pol_angle.event_time < options_mgr.bkg_before_stop) {
            modcur_bkg->Fill(t_pol_angle.rand_angle, t_pol_angle.weight);
            continue;
        }
        if (t_pol_angle.event_time > options_mgr.bkg_after_start && t_pol_angle.event_time < options_mgr.bkg_after_stop) {
            modcur_bkg->Fill(t_pol_angle.rand_angle, t_pol_angle.weight);
            continue;
        }
    }
    cout << " DONE ]" << endl;

    angle_file->Close();
    delete angle_file;

    // scale background
    double bkg_time_duration = (options_mgr.bkg_before_stop - options_mgr.bkg_before_start) + (options_mgr.bkg_after_stop - options_mgr.bkg_after_start);
    double grb_time_duration = options_mgr.grb_stop - options_mgr.grb_start;
    if (bkg_time_duration < 10) {
        cout << "bkg_time_duration is too short." << endl;
        return 1;
    }
    double bkg_scale = grb_time_duration / bkg_time_duration;
    // modcur_bkg->Scale(bkg_scale);
    for (int i = 0; i < options_mgr.nbins; i++) {
        double cur_binc = modcur_bkg->GetBinContent(i + 1);
        double cur_bine = modcur_bkg->GetBinError(i + 1);
        modcur_bkg->SetBinContent(i + 1, cur_binc * bkg_scale);
        modcur_bkg->SetBinError(  i + 1, cur_bine * bkg_scale);
    }

    // subtract background
    for (int i = 0; i < options_mgr.nbins; i++) {
        double grb_binc = modcur_grb_with_bkg->GetBinContent(i + 1);
        double grb_bine = modcur_grb_with_bkg->GetBinError(i + 1);
        double bkg_binc = modcur_bkg->GetBinContent(i + 1);
        double bkg_bine = modcur_bkg->GetBinError(i + 1);
        modcur_grb_sub_bkg->SetBinContent(i + 1, grb_binc - bkg_binc);
        modcur_grb_sub_bkg->SetBinError(i + 1, TMath::Sqrt(grb_bine * grb_bine + bkg_bine * bkg_bine));
    }

    output_file->cd();
    modcur_grb_sub_bkg->Write();
    modcur_grb_with_bkg->Write();
    modcur_bkg->Write();
    TNamed("grb_range", grb_range.c_str()).Write();
    TNamed("bkg_range", bkg_range.c_str()).Write();

    output_file->Close();
    delete output_file;

    return 0;
}
