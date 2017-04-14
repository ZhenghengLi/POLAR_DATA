#include <iostream>
#include "RootInc.hpp"
#include "OptionsManager.hpp"
#include "CooConv.hpp"

#define SPEC_BINS 64
#define ADC_MIN   400
#define FUNC_MIN  800
#define FUNC_MAX  4096

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

    // open na22_data_file
    TFile* na22_data_file = new TFile(options_mgr.na22_data_filename.Data(), "read");
    if (na22_data_file->IsZombie()) {
        cout << "na22_data_file open failed." << endl;
        return 1;
    }
    TTree* t_na22_data_tree = static_cast<TTree*>(na22_data_file->Get("t_na22_data"));
    if (t_na22_data_tree == NULL) {
        cout << "cannot find TTree t_na22_data" << endl;
        return 1;
    }
    struct {
        Double_t event_time;
        Bool_t   time_aligned[25];
        Bool_t   trigger_bit[25][64];
        Int_t    trigger_n;
        Int_t    multiplicity[25];
        Float_t  energy_value[25][64];
        UShort_t channel_status[25][64];
        Int_t    first_ij[2];
        Int_t    second_ij[2];
        Float_t  fe_temp[25];
        Float_t  fe_hv[25];
        Float_t  aux_interval;
    } t_na22_data;
    t_na22_data_tree->SetBranchAddress("event_time",     &t_na22_data.event_time      );
    t_na22_data_tree->SetBranchAddress("time_aligned",    t_na22_data.time_aligned    );
    t_na22_data_tree->SetBranchAddress("trigger_bit",     t_na22_data.trigger_bit     );
    t_na22_data_tree->SetBranchAddress("trigger_n",      &t_na22_data.trigger_n       );
    t_na22_data_tree->SetBranchAddress("multiplicity",    t_na22_data.multiplicity    );
    t_na22_data_tree->SetBranchAddress("energy_value",    t_na22_data.energy_value    );
    t_na22_data_tree->SetBranchAddress("channel_status",  t_na22_data.channel_status  );
    t_na22_data_tree->SetBranchAddress("first_ij",        t_na22_data.first_ij        );
    t_na22_data_tree->SetBranchAddress("second_ij",       t_na22_data.second_ij       );
    t_na22_data_tree->SetBranchAddress("fe_temp",         t_na22_data.fe_temp         );
    t_na22_data_tree->SetBranchAddress("fe_hv",           t_na22_data.fe_hv           );
    t_na22_data_tree->SetBranchAddress("aux_interval",   &t_na22_data.aux_interval    );

    cout << options_mgr.na22_data_filename.Data() << " { " << "ct_num = " << options_mgr.ct_num << " }" << endl;

    // open output file
    TFile* ce_result_file = new TFile(options_mgr.ce_result_filename.Data(), "recreate");
    if (ce_result_file->IsZombie()) {
        cout << "ce_result_file open failed." << endl;
        return 1;
    }
    ce_result_file->cd();

    // prepare histogram
    TF1*  spec_func[64];
    TH1F* spec_hist[64];
    for (int j = 0; j < 64; j++) {
        spec_hist[j] = new TH1F(Form("spec_hist_%02d_%02d", options_mgr.ct_num, j),
                Form("Spectrum of CH %02d_%02d", options_mgr.ct_num, j), SPEC_BINS, 0, 4096);
        spec_func[j] = new TF1(Form("spec_func_%02d_%02d", options_mgr.ct_num, j),
                "[0] + [1] * TMath::Erfc((x-[2]) / TMath::Sqrt(2) / [3])", FUNC_MIN, FUNC_MAX);
        spec_func[j]->SetParName(2, "CE");
		spec_func[j]->SetParLimits(2, FUNC_MIN, FUNC_MAX);
        spec_func[j]->SetParName(3, "Sigma");
    }

    int ct_idx = options_mgr.ct_num - 1;

    // reading and selecting data
    int pre_percent = 0;
    int cur_percent = 0;
    cout << "Filling ADC histogram of CT_" << options_mgr.ct_num << " ..." << endl;
    cout << "[ " << flush;
    for (Long64_t q = 0; q < t_na22_data_tree->GetEntries(); q++) {
        cur_percent = static_cast<int>(q * 100.0 / t_na22_data_tree->GetEntries());
        if (cur_percent - pre_percent > 0 && cur_percent % 2 == 0) {
            pre_percent = cur_percent;
            cout << "#" << flush;
        }
        t_na22_data_tree->GetEntry(q);

        // filter start
        if (t_na22_data.aux_interval > 20) continue;
        if (t_na22_data.fe_temp[ct_idx] < options_mgr.low_temp) continue;
        if (t_na22_data.fe_temp[ct_idx] > options_mgr.high_temp) continue;
        if (t_na22_data.fe_hv[ct_idx]   < options_mgr.low_hv) continue;
        if (t_na22_data.fe_hv[ct_idx]   > options_mgr.high_hv) continue;
        // filter stop

        // fill ADC value
        if (t_na22_data.first_ij[0] == ct_idx) {
            int   cur_i   = t_na22_data.first_ij[0];
            int   cur_j   = t_na22_data.first_ij[1];
            float cur_adc = t_na22_data.energy_value[cur_i][cur_j];
            if (cur_adc > ADC_MIN && t_na22_data.channel_status[cur_i][cur_j] < 1) {
                spec_hist[cur_j]->Fill(cur_adc);
            }
        }
        if (t_na22_data.second_ij[0] == ct_idx) {
            int   cur_i   = t_na22_data.second_ij[0];
            int   cur_j   = t_na22_data.second_ij[1];
            float cur_adc = t_na22_data.energy_value[cur_i][cur_j];
            if (cur_adc > ADC_MIN && t_na22_data.channel_status[cur_i][cur_j] < 1) {
                spec_hist[cur_j]->Fill(cur_adc);
            }
        }

    }
    cout << " DONE ]" << endl;

    Double_t erfc_p[64][4];
    TVectorF adc_per_kev(64);
    TVectorF ce_adc_sigma(64);

    cout << "Fitting compton edge of CT_" << options_mgr.ct_num << " ..." << endl;
    // fit
    for (int j = 0; j < 64; j++) {
        // fit 1 => 1700 500
		spec_func[j]->SetParLimits(0, 0, spec_hist[j]->GetEntries());
        spec_func[j]->SetParameters(5, 50, 1700, 500);
        spec_hist[j]->Fit(spec_func[j], "RNQ");
        spec_func[j]->GetParameters(erfc_p[j]);
        if (erfc_p[j][2] > 200 && erfc_p[j][2] < 4095 && erfc_p[j][3] > 100 && erfc_p[j][3] < 2000)
            continue;
        // fit 2 => 1900 500
        spec_func[j]->SetParameters(5, 50, 1900, 500);
        spec_hist[j]->Fit(spec_func[j], "RNQ");
        spec_func[j]->GetParameters(erfc_p[j]);
        if (erfc_p[j][2] > 200 && erfc_p[j][2] < 4095 && erfc_p[j][3] > 100 && erfc_p[j][3] < 2000)
            continue;
        // fit 3 => 2100 500
        spec_func[j]->SetParameters(5, 50, 2100, 500);
        spec_hist[j]->Fit(spec_func[j], "RNQ");
        spec_func[j]->GetParameters(erfc_p[j]);
        if (erfc_p[j][2] > 200 && erfc_p[j][2] < 4095 && erfc_p[j][3] > 100 && erfc_p[j][3] < 2000)
            continue;
        // fit 4 => 2300 500
        spec_func[j]->SetParameters(5, 50, 2300, 500);
        spec_hist[j]->Fit(spec_func[j], "RNQ");
        spec_func[j]->GetParameters(erfc_p[j]);
        if (erfc_p[j][2] > 200 && erfc_p[j][2] < 4095 && erfc_p[j][3] > 100 && erfc_p[j][3] < 2000)
            continue;
        // fit 5 => 1700 600
        spec_func[j]->SetParameters(5, 50, 1700, 600);
        spec_hist[j]->Fit(spec_func[j], "RNQ");
        spec_func[j]->GetParameters(erfc_p[j]);
        if (erfc_p[j][2] > 200 && erfc_p[j][2] < 4095 && erfc_p[j][3] > 100 && erfc_p[j][3] < 2000)
            continue;
        // fit 6 => 1900 600
        spec_func[j]->SetParameters(5, 50, 1900, 600);
        spec_hist[j]->Fit(spec_func[j], "RNQ");
        spec_func[j]->GetParameters(erfc_p[j]);
        if (erfc_p[j][2] > 200 && erfc_p[j][2] < 4095 && erfc_p[j][3] > 100 && erfc_p[j][3] < 2000)
            continue;
        // fit 7 => 2100 600
        spec_func[j]->SetParameters(5, 50, 2100, 600);
        spec_hist[j]->Fit(spec_func[j], "RNQ");
        spec_func[j]->GetParameters(erfc_p[j]);
        if (erfc_p[j][2] > 200 && erfc_p[j][2] < 4095 && erfc_p[j][3] > 100 && erfc_p[j][3] < 2000)
            continue;
        // fit 8 => 2300 600
        spec_func[j]->SetParameters(5, 50, 2300, 600);
        spec_hist[j]->Fit(spec_func[j], "RNQ");
        spec_func[j]->GetParameters(erfc_p[j]);
        if (erfc_p[j][2] > 200 && erfc_p[j][2] < 4095 && erfc_p[j][3] > 100 && erfc_p[j][3] < 2000)
            continue;
        // fit 9 => 1700 700
        spec_func[j]->SetParameters(5, 50, 1700, 700);
        spec_hist[j]->Fit(spec_func[j], "RNQ");
        spec_func[j]->GetParameters(erfc_p[j]);
        if (erfc_p[j][2] > 200 && erfc_p[j][2] < 4095 && erfc_p[j][3] > 100 && erfc_p[j][3] < 2000)
            continue;
        // fit 10 => 1900 700
        spec_func[j]->SetParameters(5, 50, 1900, 700);
        spec_hist[j]->Fit(spec_func[j], "RNQ");
        spec_func[j]->GetParameters(erfc_p[j]);
        if (erfc_p[j][2] > 200 && erfc_p[j][2] < 4095 && erfc_p[j][3] > 100 && erfc_p[j][3] < 2000)
            continue;
        // fit 11 => 2100 700
        spec_func[j]->SetParameters(5, 50, 2100, 700);
        spec_hist[j]->Fit(spec_func[j], "RNQ");
        spec_func[j]->GetParameters(erfc_p[j]);
        if (erfc_p[j][2] > 200 && erfc_p[j][2] < 4095 && erfc_p[j][3] > 100 && erfc_p[j][3] < 2000)
            continue;
        // fit 12 => 2300 700
        spec_func[j]->SetParameters(5, 50, 2300, 700);
        spec_hist[j]->Fit(spec_func[j], "RNQ");
        spec_func[j]->GetParameters(erfc_p[j]);
        if (erfc_p[j][2] > 200 && erfc_p[j][2] < 4095 && erfc_p[j][3] > 100 && erfc_p[j][3] < 2000)
            continue;
    }
    const float CE_Na22  = 340.667;
    for (int j = 0; j < 64; j++) {
        if (erfc_p[j][2] > 200 && erfc_p[j][2] < 4095 && erfc_p[j][3] > 100 && erfc_p[j][3] < 2000) {
            adc_per_kev(j) = erfc_p[j][2] / CE_Na22;
            ce_adc_sigma(j) = erfc_p[j][3];
        } else {
            adc_per_kev(j) = 0;
            ce_adc_sigma(j) = 0;
        }
    }

    // save result
    gROOT->SetBatch(kTRUE);
    gErrorIgnoreLevel = kWarning;
    gStyle->SetOptStat(11);
    gStyle->SetOptFit(111);
    for (int j = 0; j < 64; j++) {
        spec_hist[j]->Fit(spec_func[j], "RQ");
    }
    TCanvas* canvas_spec_hist = new TCanvas("canvas_spec_hist", "canvas_spec_hist", 900, 900);
    canvas_spec_hist->SetFillColor(kYellow);
    canvas_spec_hist->Divide(8, 8);
    for (int j = 0; j < 64; j++) {
        canvas_spec_hist->cd(jtoc(j));
        canvas_spec_hist->GetPad(jtoc(j))->SetFillColor(kWhite);
        spec_hist[j]->Draw();
    }
    ce_result_file->cd();
    TNamed("ct_num", Form("%d", options_mgr.ct_num)).Write();
    TNamed("source_type", "Na22").Write();
    ce_result_file->mkdir("spec_hist")->cd();
    for (int j = 0; j < 64; j++) {
        spec_hist[j]->Write();
    }
    ce_result_file->cd();
    canvas_spec_hist->Write();
    adc_per_kev.Write("adc_per_kev");
    ce_adc_sigma.Write("ce_adc_sigma");
    TNamed("low_temp", Form("%f", options_mgr.low_temp)).Write();
    TNamed("high_temp", Form("%f", options_mgr.high_temp)).Write();
    TNamed("low_hv", Form("%f", options_mgr.low_hv)).Write();
    TNamed("high_hv", Form("%f", options_mgr.high_hv)).Write();

    ce_result_file->Close();

    return 0;
}
