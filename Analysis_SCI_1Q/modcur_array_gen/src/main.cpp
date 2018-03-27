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
// pol_direction: h1(45, 0, 180) => 2, 6, 10, ... , 174, 178
//
// pol_degree: h1(50, 0, 100) => 1, 3, 5, 7, 9, ... , 97, 99
//
/////////////////////////////////////////////////////////////

using namespace std;

// filename_XXX.mac.root
int extract_pol_direction(const char* filename) {
    string filename_str = filename;
    size_t start_pos = filename_str.length() - 12;
    string pol_direction_str = filename_str.substr(start_pos, 3);
    int pol_direction = atoi(pol_direction_str.c_str());
    if (pol_direction < 2 || pol_direction > 178) return -1;
    if ((pol_direction - 2) % 4 != 0) return -1;
    return pol_direction;
}

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

    TH1D pol_modcur[45];
    TH1D nopol_modcur;

    ///////////////////////////////////////////////////////
    // read pol files
    bool pol_modcur_readflag[45];
    for (int i = 0; i < 45; i++) {
        pol_modcur_readflag[i] = false;
    }
    for (size_t i = 0; i < options_mgr.pol_modcur_filelist.size(); i++) {
        int pol_direction = extract_pol_direction(options_mgr.pol_modcur_filelist[i].Data());
        if (pol_direction < 0) {
            return 1;
        } else {
            int idx = (pol_direction - 2) / 4;
            if (read_modcur(options_mgr.pol_modcur_filelist[i].Data(), pol_modcur[idx])) {
                pol_modcur_readflag[idx] = true;
            } else {
                cout << "read modcur failed: " << options_mgr.pol_modcur_filelist[i].Data() << endl;
                return 1;
            }
        }
    }
    bool all_read = true;
    for (int i = 0; i < 45; i++) {
        if (!pol_modcur_readflag[i]) {
            all_read = false;
            break;
        }
    }
    if (all_read) {
        cout << "all pol_modcur read done." << endl;
    } else {
        cout << "the files of pol_direction not read: " << flush;
        for (int i = 0; i < 45; i++) {
            if (!pol_modcur_readflag[i]) {
                cout << i * 4 + 2 << " ";
            }
        }
        cout << endl;
        return 1;
    }
    // read nopol file
    if (read_modcur(options_mgr.nopol_modcur_filename.Data(), nopol_modcur)) {
        cout << "nopol_modcur read done." << endl;
    } else {
        cout << "nopol_modcur read failed: " << options_mgr.nopol_modcur_filename.Data() << endl;
        return 1;
    }
    int nbins = nopol_modcur.GetNbinsX();
    ///////////////////////////////////////////////

    // generate modcur_array
    TFile* modcur_array_file = new TFile(options_mgr.modcur_array_filename.Data(), "recreate");
    modcur_array_file->cd();
    TH1D* modcur_array[45][50];
    for (int i = 0; i < 45; i++) {
        for (int j = 0; j < 50; j++) {
            modcur_array[i][j] = new TH1D(Form("modcur_%03d_%03d", i * 4 + 2, j * 2 + 1), Form("modcur_%03d_%03d", i * 4 + 2, j * 2 + 1), nbins, 0, 360);
            double pol_degree = (j * 2.0 + 1.0) / 100.0;
            double nopol_degree = 1 - pol_degree;
            for (int k = 0; k < nbins; k++) {
                double pol_binc = pol_modcur[i].GetBinContent(k + 1);
                double pol_bine = pol_modcur[i].GetBinError(k + 1);
                double nopol_binc = nopol_modcur.GetBinContent(k + 1);
                double nopol_bine = nopol_modcur.GetBinError(k + 1);
                modcur_array[i][j]->SetBinContent(k + 1, pol_degree * pol_binc + nopol_degree * nopol_binc);
                modcur_array[i][j]->SetBinError(k + 1, TMath::Sqrt(pol_degree * pol_degree * pol_bine * pol_bine + nopol_degree * nopol_degree * nopol_bine * nopol_bine));
            }
        }
    }
    // write modcur_array
    for (int i = 0; i < 45; i++) {
        modcur_array_file->cd();
        modcur_array_file->mkdir(Form("pol_direction_%03d", i * 4 + 2))->cd();
        for (int j = 0; j < 50; j++) {
            modcur_array[i][j]->Write();
        }
    }
    modcur_array_file->Close();
    delete modcur_array_file;

    return 0;
}
