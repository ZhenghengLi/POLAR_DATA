#include <iostream>
#include <fstream>
#include <sstream>
#include "RootInc.hpp"
#include "OptionsManager.hpp"
#include "POLEvent.hpp"
#include "BarPos.hpp"
#include "PosConv.hpp"

/////////////////////////////////////////////////////////////
// MODULATION CURVE ARRAY
/////////////////////////////////////////////////////////////
//
// pol_direction: h1(180, 0, 180) => 1, 3, 5, ... , 179
//
// pol_degree: h1(100, 0, 100) => 1, 3, 5, ... , 99
//
/////////////////////////////////////////////////////////////

using namespace std;

bool read_modcur(const char* filename, TH1D& modcur) {
    TFile* modcur_file = new TFile(filename, "read");
    if (modcur_file->IsZombie()) return false;
    TH1D* modcur_hist = static_cast<TH1D*>(modcur_file->Get("modcur_grb_sub_bkg"));
    if (modcur_hist == NULL) return false;
    modcur = *modcur_hist;
    modcur.SetDirectory(NULL);
    modcur_file->Close();
    delete modcur_file;
    return true;
}

bool read_modcur_array(const char* filename, TH1D* modcur_array[90][50]) {
    TFile* modcur_array_file = new TFile(filename, "read");
    if (modcur_array_file->IsZombie()) return false;
    TH1D* modcur_hist = NULL;
    for (int i = 0; i < 90; i++) {
        for (int j = 0; j < 50; j++) {
            modcur_hist = static_cast<TH1D*>(modcur_array_file->Get(Form("pol_direction_%03d/modcur_%03d_%03d", i * 2 + 1, i * 2 + 1, j * 2 + 1)));
            if (modcur_hist == NULL) {
                cout << "cannot find TH1D " << Form("pol_direction_%03d/modcur_%03d_%03d", i * 2 + 1, i * 2 + 1, j * 2 + 1) << endl;
                return false;
            }
            modcur_array[i][j] = static_cast<TH1D*>(modcur_hist->Clone());
            modcur_array[i][j]->SetDirectory(NULL);
        }
    }
    modcur_array_file->Close();
    delete modcur_array_file;
    return true;
}


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

    double modcur_array_chi2[90][50];

    TH1D* modcur_array[90][50];
    TH1D modcur_meas;

    // read modcur
    if (read_modcur_array(options_mgr.modcur_array_filename.Data(), modcur_array)) {
        cout << "modcur_array read done." << endl;
    } else {
        cout << "modcur_array read failed: " << options_mgr.modcur_array_filename.Data() << endl;
        return 1;
    }
    if (read_modcur(options_mgr.modcur_filename.Data(), modcur_meas)) {
        cout << "modcur_meas read done." << endl;
    } else {
        cout << "modcur_meas read failed: " << options_mgr.modcur_filename.Data() << endl;
        return 1;
    }

    // calculate chi2 array
    for (int i = 0; i < 90; i++) {
        for (int j = 0; j < 50; j++) {
            modcur_array_chi2[i][j] = modcur_meas.Chi2Test(modcur_array[i][j], "WW CHI2");
        }
    }

    // generate contour map
    TFile* output_file = new TFile(options_mgr.output_filename.Data(), "recreate");
    if (output_file->IsZombie()) {
        cout << "output_file open failed: " << options_mgr.output_filename.Data() << endl;
        return 1;
    }
    TH2D* chi2_map = new TH2D("chi2_map", "chi2_map", 50, 0, 100, 90, 0, 180);
    for (int i = 0; i < 90; i++) {
        for (int j = 0; j < 50; j++) {
            chi2_map->SetBinContent(j + 1, i + 1, modcur_array_chi2[i][j]);
        }
    }
    TH2D* chi2_map_inter = new TH2D("chi2_map_inter", "chi2_map_inter", 100, 0, 100, 180, 0, 180);
    for (int i = 0; i < 180; i++) {
        for (int j = 0; j < 100; j++) {
            double binc = chi2_map->Interpolate(j + 0.5, i + 0.5);
            chi2_map_inter->SetBinContent(j + 1, i + 1, binc);
        }
    }
    double min_chi2 = 10000000000;
    double min_pol_degree = 0;
    double min_pol_direction = 0;
    for (int i = 0; i < 100; i++) {
        for (int j = 0; j < 180; j++) {
            double binc = chi2_map_inter->GetBinContent(i + 1, j + 1);
            if (binc < min_chi2) {
                min_chi2 = binc;
                min_pol_degree = i + 0.5;
                min_pol_direction = j + 0.5;
            }
        }
    }

    cout << "========================================" << endl;
    cout << "min_chi2 = " << min_chi2 << endl;
    cout << "min_pol_degree = " << min_pol_degree << endl;
    cout << "min_pol_direction = " << min_pol_direction << endl;
    cout << "========================================" << endl;

    TH2D* delta_chi2_map_inter = new TH2D("delta_chi2_map_inter", "delta_chi2_map_inter", 100, 0, 100, 180, 0, 180);

    double non_zero = 0.00001;

    for (int i = 0; i < 100; i++) {
        for (int j = 0; j < 180; j++) {
            double binc = chi2_map_inter->GetBinContent(i + 1, j + 1) - min_chi2 + non_zero;
            delta_chi2_map_inter->SetBinContent(i + 1, j + 1, binc);
        }
    }

    double level_list[3] = {1, 4, 9};

    double levels[3];
    for (int i = 0; i < 3; i++) {
        levels[i] = min_chi2 + level_list[i];
    }
    TH1D* chi2_map_inter_cont3 = static_cast<TH1D*>(chi2_map_inter->Clone("chi2_map_inter_cont3"));
    chi2_map_inter_cont3->SetContour(3, levels);
    chi2_map_inter_cont3->SetLineColor(kRed);
    chi2_map_inter_cont3->SetLineWidth(2);

    double delta_levels[3];
    for (int i = 0; i < 3; i++) {
        delta_levels[i] = non_zero + level_list[i];
    }
    TH1D* delta_chi2_map_inter_cont3 = static_cast<TH1D*>(chi2_map_inter->Clone("delta_chi2_map_inter_cont3"));
    delta_chi2_map_inter_cont3->SetContour(3, delta_levels);
    delta_chi2_map_inter_cont3->SetLineColor(kRed);
    delta_chi2_map_inter_cont3->SetLineWidth(2);

    output_file->cd();
    chi2_map->Write();
    chi2_map_inter->Write();
    chi2_map_inter_cont3->Write();
    delta_chi2_map_inter->Write();
    delta_chi2_map_inter_cont3->Write();
    TNamed("min_chi2", Form("%f", min_chi2)).Write();
    TNamed("min_pol_degree", Form("%f", min_pol_degree)).Write();
    TNamed("min_pol_direction", Form("%f", min_pol_direction)).Write();

    output_file->Close();
    delete output_file;

    return 0;
}
