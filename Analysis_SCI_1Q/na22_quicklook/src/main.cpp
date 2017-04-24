#include <iostream>
#include "RootInc.hpp"
#include "OptionsManager.hpp"
#include "CooConv.hpp"
#include "CommonCanvas.hpp"

#define SPEC_BINS 64
#define ADC_MIN   300
#define FUNC_MIN  400
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

    cout << options_mgr.na22_data_filename.Data() << endl;

    // prepare histogram
    TF1*  spec_func[25][64];
    TH1F* spec_hist[25][64];
    TH2F* spec_count_map;
    for (int i = 0; i < 25; i++) {
        for (int j = 0; j < 64; j++) {
            spec_hist[i][j] = new TH1F(Form("spec_hist%02d_%02d", i + 1, j + 1),
                    Form("Spectrum of CH %02d_%02d", i + 1, j + 1), SPEC_BINS, 0, 4096);
            spec_hist[i][j]->SetDirectory(NULL);
            spec_func[i][j] = new TF1(Form("spec_func%02d_%02d", i + 1, j + 1),
                    "[0] + [1] * TMath::Erfc((x - [2] ) / TMath::Sqrt(2) / [3])", FUNC_MIN, FUNC_MAX);
            spec_func[i][j]->SetParName(2, "CE");
			spec_func[i][j]->SetParLimits(2, FUNC_MIN, FUNC_MAX);
            spec_func[i][j]->SetParName(3, "Sigma");
        }
    }
    spec_count_map = new TH2F("spec_count_map", "Source Event Count Map", 40, 0, 40, 40, 0, 40);
    spec_count_map->SetDirectory(NULL);
    spec_count_map->GetXaxis()->SetNdivisions(40);
    spec_count_map->GetYaxis()->SetNdivisions(40);
    for (int i = 0; i < 40; i++) {
        if (i % 8 == 0) {
            spec_count_map->GetXaxis()->SetBinLabel(i + 1, Form("%02d", i));
            spec_count_map->GetYaxis()->SetBinLabel(i + 1, Form("%02d", i));
        }
    }

    // reading and selecting data
    int pre_percent = 0;
    int cur_percent = 0;
    cout << "Filling ADC histogram of all modules ..." << endl;
    cout << "[ " << flush;
    for (Long64_t q = 0; q < t_na22_data_tree->GetEntries(); q++) {
        cur_percent = static_cast<int>(q * 100.0 / t_na22_data_tree->GetEntries());
        if (cur_percent - pre_percent > 0 && cur_percent % 2 == 0) {
            pre_percent = cur_percent;
            cout << "#" << flush;
        }
        t_na22_data_tree->GetEntry(q);

        int first_i = t_na22_data.first_ij[0];
        int first_j = t_na22_data.first_ij[1];
        float first_adc = t_na22_data.energy_value[first_i][first_j];
        if (first_adc > ADC_MIN && t_na22_data.channel_status[first_i][first_j] < 1) {
            spec_hist[first_i][first_j]->Fill(first_adc);
        }

        int second_i = t_na22_data.second_ij[0];
        int second_j = t_na22_data.second_ij[1];
        float second_adc = t_na22_data.energy_value[second_i][second_j];
        if (second_adc > ADC_MIN && t_na22_data.channel_status[second_i][second_j] < 1) {
            spec_hist[second_i][second_j]->Fill(second_adc);
        }

    }
    cout << " DONE ]" << endl;

    // fit
    Double_t erfc_p[25][64][4];
    TVectorF adc_per_kev[25];
    TVectorF gain_sigma[25];
    for (int i = 0; i < 25; i++) {
        adc_per_kev[i].ResizeTo(64);
        gain_sigma[i].ResizeTo(64);
    }
    cout << "Fitting compton edge ..." << endl;
    for (int i = 0; i < 25; i++) {
        for (int j = 0; j < 64; j++) {
            // fit 1 => 1700 500
			spec_func[i][j]->SetParLimits(0, 0, spec_hist[i][j]->GetEntries());
            spec_func[i][j]->SetParameters(5, 50, 1700, 500);
            spec_hist[i][j]->Fit(spec_func[i][j], "RNQ");
            spec_func[i][j]->GetParameters(erfc_p[i][j]);
            if (erfc_p[i][j][2] > 200 && erfc_p[i][j][2] < 4095 && erfc_p[i][j][3] > 100 && erfc_p[i][j][3] < 2000)
                continue;
            // fit 2 => 1900 500
            spec_func[i][j]->SetParameters(5, 50, 1900, 500);
            spec_hist[i][j]->Fit(spec_func[i][j], "RNQ");
            spec_func[i][j]->GetParameters(erfc_p[i][j]);
            if (erfc_p[i][j][2] > 200 && erfc_p[i][j][2] < 4095 && erfc_p[i][j][3] > 100 && erfc_p[i][j][3] < 2000)
                continue;
            // fit 3 => 2100 500
            spec_func[i][j]->SetParameters(5, 50, 2100, 500);
            spec_hist[i][j]->Fit(spec_func[i][j], "RNQ");
            spec_func[i][j]->GetParameters(erfc_p[i][j]);
            if (erfc_p[i][j][2] > 200 && erfc_p[i][j][2] < 4095 && erfc_p[i][j][3] > 100 && erfc_p[i][j][3] < 2000)
                continue;
            // fit 4 => 2300 500
            spec_func[i][j]->SetParameters(5, 50, 2300, 500);
            spec_hist[i][j]->Fit(spec_func[i][j], "RNQ");
            spec_func[i][j]->GetParameters(erfc_p[i][j]);
            if (erfc_p[i][j][2] > 200 && erfc_p[i][j][2] < 4095 && erfc_p[i][j][3] > 100 && erfc_p[i][j][3] < 2000)
                continue;
            // fit 5 => 1700 600
            spec_func[i][j]->SetParameters(5, 50, 1700, 600);
            spec_hist[i][j]->Fit(spec_func[i][j], "RNQ");
            spec_func[i][j]->GetParameters(erfc_p[i][j]);
            if (erfc_p[i][j][2] > 200 && erfc_p[i][j][2] < 4095 && erfc_p[i][j][3] > 100 && erfc_p[i][j][3] < 2000)
                continue;
            // fit 6 => 1900 600
            spec_func[i][j]->SetParameters(5, 50, 1900, 600);
            spec_hist[i][j]->Fit(spec_func[i][j], "RNQ");
            spec_func[i][j]->GetParameters(erfc_p[i][j]);
            if (erfc_p[i][j][2] > 200 && erfc_p[i][j][2] < 4095 && erfc_p[i][j][3] > 100 && erfc_p[i][j][3] < 2000)
                continue;
            // fit 7 => 2100 600
            spec_func[i][j]->SetParameters(5, 50, 2100, 600);
            spec_hist[i][j]->Fit(spec_func[i][j], "RNQ");
            spec_func[i][j]->GetParameters(erfc_p[i][j]);
            if (erfc_p[i][j][2] > 200 && erfc_p[i][j][2] < 4095 && erfc_p[i][j][3] > 100 && erfc_p[i][j][3] < 2000)
                continue;
            // fit 8 => 2300 600
            spec_func[i][j]->SetParameters(5, 50, 2300, 600);
            spec_hist[i][j]->Fit(spec_func[i][j], "RNQ");
            spec_func[i][j]->GetParameters(erfc_p[i][j]);
            if (erfc_p[i][j][2] > 200 && erfc_p[i][j][2] < 4095 && erfc_p[i][j][3] > 100 && erfc_p[i][j][3] < 2000)
                continue;
            // fit 9 => 1700 700
            spec_func[i][j]->SetParameters(5, 50, 1700, 700);
            spec_hist[i][j]->Fit(spec_func[i][j], "RNQ");
            spec_func[i][j]->GetParameters(erfc_p[i][j]);
            if (erfc_p[i][j][2] > 200 && erfc_p[i][j][2] < 4095 && erfc_p[i][j][3] > 100 && erfc_p[i][j][3] < 2000)
                continue;
            // fit 10 => 1900 700
            spec_func[i][j]->SetParameters(5, 50, 1900, 700);
            spec_hist[i][j]->Fit(spec_func[i][j], "RNQ");
            spec_func[i][j]->GetParameters(erfc_p[i][j]);
            if (erfc_p[i][j][2] > 200 && erfc_p[i][j][2] < 4095 && erfc_p[i][j][3] > 100 && erfc_p[i][j][3] < 2000)
                continue;
            // fit 11 => 2100 700
            spec_func[i][j]->SetParameters(5, 50, 2100, 700);
            spec_hist[i][j]->Fit(spec_func[i][j], "RNQ");
            spec_func[i][j]->GetParameters(erfc_p[i][j]);
            if (erfc_p[i][j][2] > 200 && erfc_p[i][j][2] < 4095 && erfc_p[i][j][3] > 100 && erfc_p[i][j][3] < 2000)
                continue;
            // fit 12 => 2300 700
            spec_func[i][j]->SetParameters(5, 50, 2300, 700);
            spec_hist[i][j]->Fit(spec_func[i][j], "RNQ");
            spec_func[i][j]->GetParameters(erfc_p[i][j]);
            if (erfc_p[i][j][2] > 200 && erfc_p[i][j][2] < 4095 && erfc_p[i][j][3] > 100 && erfc_p[i][j][3] < 2000)
                continue;
        }
    }
    const float CE_Na22  = 340.667;
    for (int i = 0; i < 25; i++) {
        for (int j = 0; j < 64; j++) {
            adc_per_kev[i](j) = erfc_p[i][j][2] / CE_Na22;
            gain_sigma[i](j) = erfc_p[i][j][3] / CE_Na22;
        }
    }

    if (!options_mgr.ce_result_filename.IsNull()) {
        TFile* ce_result_file = new TFile(options_mgr.ce_result_filename.Data(), "recreate");
        for (int i = 0; i < 25; i++) {
            adc_per_kev[i].Write(Form("adc_per_kev_ct_%02d", i + 1));
            gain_sigma[i].Write(Form("gain_sigma_ct_%02d", i + 1));
        }
        ce_result_file->Close();
        delete ce_result_file;
        ce_result_file = NULL;
    }
    for (int i = 0; i < 25; i++) {
        for (int j = 0; j < 64; j++) {
            spec_count_map->SetBinContent(ijtox(i, j) + 1, ijtoy(i, j) + 1, spec_hist[i][j]->GetEntries());
        }
    }
    if (options_mgr.show_flag) {
        cout << "Showing fitting result..." << endl;
        TApplication* rootapp = new TApplication("POLAR", NULL, NULL);
        CommonCanvas canvas_fitting;
        canvas_fitting.init(spec_count_map, spec_hist, spec_func);
        canvas_fitting.draw_na22_hitmap();
        rootapp->Run();

    }

    return 0;
}
