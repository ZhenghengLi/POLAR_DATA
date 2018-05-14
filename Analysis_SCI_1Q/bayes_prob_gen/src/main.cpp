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

bool read_modcur(const char* filename, TH1D* modcur_grb_sub_bkg, TH1D* modcur_bkg, int& grb_with_bkg_entries, int& bkg_at_grb_entries) {
    TFile* modcur_file = new TFile(filename, "read");
    if (modcur_file->IsZombie()) return false;

    TH1D*   tmp_hist = NULL;
    TNamed* tmp_tnamed = NULL;

    tmp_hist = static_cast<TH1D*>(modcur_file->Get("modcur_grb_sub_bkg"));
    if (tmp_hist == NULL) return false;
    modcur_grb_sub_bkg = static_cast<TH1D*>(tmp_hist->Clone());
    modcur_grb_sub_bkg->SetDirectory(NULL);

    tmp_hist = static_cast<TH1D*>(modcur_file->Get("modcur_bkg"));
    if (tmp_hist == NULL) return false;
    modcur_bkg = static_cast<TH1D*>(tmp_hist->Clone());
    modcur_bkg->SetDirectory(NULL);

    tmp_tnamed = static_cast<TNamed*>(modcur_file->Get("grb_with_bkg_entries"));
    if (tmp_tnamed == NULL) return false;
    grb_with_bkg_entries = TString(tmp_tnamed->GetTitle()).Atoi();

    tmp_tnamed = static_cast<TNamed*>(modcur_file->Get("bkg_at_grb_entries"));
    if (tmp_tnamed == NULL) return false;
    bkg_at_grb_entries = TString(tmp_tnamed->GetTitle()).Atoi();

    modcur_file->Close();
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


    TH1D* modcur_array[90][50];
    TH1D* modcur_grb_sub_bkg = NULL;
    TH1D* modcur_bkg = NULL;
    int grb_with_bkg_entries = 0;
    int bkg_at_grb_entries = 0;

    // read modcur
    if (read_modcur_array(options_mgr.modcur_array_filename.Data(), modcur_array)) {
        cout << "modcur_array read done." << endl;
    } else {
        cout << "modcur_array read failed: " << options_mgr.modcur_array_filename.Data() << endl;
        return 1;
    }
    if (read_modcur(options_mgr.modcur_filename.Data(), modcur_grb_sub_bkg, modcur_bkg, grb_with_bkg_entries, bkg_at_grb_entries)) {
        cout << "modcur_meas read done." << endl;
    } else {
        cout << "modcur_meas read failed: " << options_mgr.modcur_filename.Data() << endl;
        return 1;
    }
    int grb_entries = grb_with_bkg_entries - bkg_at_grb_entries;






    TFile* output_file = new TFile(options_mgr.output_filename.Data(), "recreate");
    if (output_file->IsZombie()) {
        cout << "output_file open failed: " << options_mgr.output_filename.Data() << endl;
        return 1;
    }





    output_file->Close();
    delete output_file;

    return 0;
}
