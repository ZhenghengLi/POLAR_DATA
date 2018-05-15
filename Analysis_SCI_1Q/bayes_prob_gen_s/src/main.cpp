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

bool read_modcur(const char* filename, TH1D*& modcur_grb_sub_bkg, TH1D*& modcur_bkg, int& grb_with_bkg_entries, int& bkg_at_grb_entries) {
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

struct PA_PD {
    double pa;
    int pa_idx;
    double pd;
    int pd_idx;
    double min_chi2;
    PA_PD(double pa_, int pa_idx_, double pd_, int pd_idx_, double min_chi2_) {
        pa = pa_;
        pa_idx = pa_idx_;
        pd = pd_;
        pd_idx = pd_idx_;
        min_chi2 = min_chi2_;
    }
};

PA_PD find_best_pa_pd(TH1D* modcur_grb, TH1D* modcur_array[90][50]) {
    double min_chi2 = 1000000;
    double pa = 0;
    double pa_idx = 0;
    double pd = 0;
    double pd_idx = 0;
    for (int i = 0; i < 90; i++) {
        for (int j = 0; j < 50; j++) {
            double chi2 = modcur_grb->Chi2Test(modcur_array[i][j], "WW CHI2");
            if (chi2 < min_chi2) {
                min_chi2 = chi2;
                pa = i * 2 + 1;
                pa_idx = i;
                pd = j * 2 + 1;
                pd_idx = j;
            }
        }
    }
    return PA_PD(pa, pa_idx, pd, pd_idx, min_chi2);
}

void generate_rand_modcur(TH1D* modcur_array[90][50], int pa_idx, int pd_idx,
        TH1D* modcur_grb, int grb_entries, TH1D* modcur_bkg, int bkg_entries) {
    modcur_grb->Reset();
    int nbins = modcur_grb->GetNbinsX();
    TH1D* modcur_grb_sub_bkg_rand = new TH1D("modcur_grb_sub_bkg_rand", "modcur_grb_sub_bkg_rand", nbins, 0, 360);
    modcur_grb_sub_bkg_rand->FillRandom(modcur_array[pa_idx][pd_idx], grb_entries);
    TH1D* modcur_bkg_rand = new TH1D("modcur_bkg_rand", "modcur_bkg_rand", nbins, 0, 360);
    modcur_bkg_rand->FillRandom(modcur_bkg, bkg_entries);
    TH1D* modcur_grb_with_bkg = new TH1D("modcur_grb_with_bkg", "modcur_grb_with_bkg", nbins, 0, 360);
    // add bkg
    for (int b = 1; b <= nbins; b++) {
        double binc_grb = modcur_grb_sub_bkg_rand->GetBinContent(b);
        double binc_bkg = modcur_bkg_rand->GetBinContent(b);
        double binc_sum = binc_grb + binc_bkg;
        modcur_grb_with_bkg->SetBinContent(b, binc_sum);
        modcur_grb_with_bkg->SetBinError(b, TMath::Sqrt(binc_sum));
    }
    // sub bkg
    for (int b = 1; b <= nbins; b++) {
        double binc_sum = modcur_grb_with_bkg->GetBinContent(b);
        double bine_sum = modcur_grb_with_bkg->GetBinError(b);
        double binc_bkg = modcur_bkg->GetBinContent(b);
        double bine_bkg = modcur_bkg->GetBinError(b);
        double binc_grb = binc_sum - binc_bkg;
        double bine_grb = TMath::Sqrt(bine_sum * bine_sum + bine_bkg * bine_bkg);
        modcur_grb->SetBinContent(b, binc_grb);
        modcur_grb->SetBinError(b, bine_grb);
    }
    // clean
    delete modcur_grb_sub_bkg_rand;
    delete modcur_bkg_rand;
    delete modcur_grb_with_bkg;
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

    TFile* output_file = new TFile(options_mgr.output_filename.Data());
    if (output_file->IsZombie()) {
        cout << "output_file open failed: " << options_mgr.output_filename.Data() << endl;
        return 1;
    }

    // find measured best pa and pd
    PA_PD papd_meas = find_best_pa_pd(modcur_grb_sub_bkg, modcur_array);

    int nbins = modcur_grb_sub_bkg->GetNbinsX();
    TH1D* modcur_rand = new TH1D("modcur_rand", "modcur_rand", nbins, 0, 360);
    TH2D* prob_map = new TH2D("prob_map", "prob_map", 50, 0, 100, 90, 0, 180);

    int rand_N = 10000;

    cout << "Generating probability map for " << Form("PA_%03d_PD_%03d", options_mgr.pa_idx * 2 + 1, options_mgr.pd_idx * 2 + 1) << " ..." << endl;
    for (int k = 1; k <= rand_N; k++) {
        if (k % 1000 == 0) cout << k << " " << flush;
        generate_rand_modcur(modcur_array, options_mgr.pa_idx, options_mgr.pd_idx, modcur_rand, grb_entries, modcur_bkg, bkg_at_grb_entries);
        PA_PD papd_rand = find_best_pa_pd(modcur_rand, modcur_array);
        prob_map->Fill(papd_rand.pd, papd_rand.pa);
    }
    double prob_m = prob_map->GetBinContent(papd_meas.pd_idx + 1, papd_meas.pa_idx + 1) / rand_N;
    cout << "=> " << prob_m << endl;

    output_file->cd();
    prob_map->Scale(1.0 / rand_N);
    prob_map->Write();
    TNamed("pa_idx", Form("%d", options_mgr.pa_idx)).Write();
    TNamed("pd_idx", Form("%d", options_mgr.pd_idx)).Write();
    TNamed("prob_m", Form("%.6f", prob_m)).Write();
    TNamed("rand_N", Form("%d", rand_N)).Write();

    output_file->Close();

    return 0;
}
