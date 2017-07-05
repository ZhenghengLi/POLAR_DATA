#include <iostream>
#include "RootInc.hpp"
#include "CommonCanvas.hpp"
#include "CooConv.hpp"
#include "OptionsManager.hpp"

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

    // open angle data file
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
    struct {
        Double_t  event_time;
        Int_t     first_ij[2];
        Int_t     second_ij[2];
        Float_t   rand_angle;
        Float_t   rand_distance;
        Float_t   first_energy;
        Float_t   second_energy;
        Bool_t    is_valid;
        Bool_t    is_na22;
        Bool_t    is_bad_calib;
        Bool_t    with_badch;
        Float_t   deadtime_weight;
        Float_t   efficiency_weight;
    } t_pol_angle;
    t_pol_angle_tree->SetBranchAddress("event_time",        &t_pol_angle.event_time         );
    t_pol_angle_tree->SetBranchAddress("first_ij",           t_pol_angle.first_ij           );
    t_pol_angle_tree->SetBranchAddress("second_ij",          t_pol_angle.second_ij          );
    t_pol_angle_tree->SetBranchAddress("rand_angle",        &t_pol_angle.rand_angle         );
    t_pol_angle_tree->SetBranchAddress("rand_distance",     &t_pol_angle.rand_distance      );
    t_pol_angle_tree->SetBranchAddress("first_energy",      &t_pol_angle.first_energy       );
    t_pol_angle_tree->SetBranchAddress("second_energy",     &t_pol_angle.second_energy      );
    t_pol_angle_tree->SetBranchAddress("is_valid",          &t_pol_angle.is_valid           );
    t_pol_angle_tree->SetBranchAddress("is_na22",           &t_pol_angle.is_na22            );
    t_pol_angle_tree->SetBranchAddress("is_bad_calib",      &t_pol_angle.is_bad_calib       );
    t_pol_angle_tree->SetBranchAddress("with_badch",        &t_pol_angle.with_badch         );
    t_pol_angle_tree->SetBranchAddress("deadtime_weight",   &t_pol_angle.deadtime_weight    );
    t_pol_angle_tree->SetBranchAddress("efficiency_weight", &t_pol_angle.efficiency_weight  );

    TApplication* rootapp = new TApplication("POLAR", NULL, NULL);
    CommonCanvas canvas;
    gStyle->SetOptStat(0);

    // angle_hit_map
    TLine* line_h[4];
    TLine* line_v[4];
    for (int i = 0; i < 4; i++) {
        line_h[i] = new TLine(0, (i + 1) * 8, 40, (i + 1) * 8);
        line_h[i]->SetLineColor(kWhite);
        line_v[i] = new TLine((i + 1) * 8, 0, (i + 1) * 8, 40);
        line_v[i]->SetLineColor(kWhite);
    }
    TH2D* angle_hit_map = new TH2D("angle_hit_map", "Hit Map", 40, 0, 40, 40, 0, 40);
    angle_hit_map->SetDirectory(NULL);
    angle_hit_map->GetXaxis()->SetNdivisions(40);
    angle_hit_map->GetYaxis()->SetNdivisions(40);
    for (int i = 0; i < 40; i++) {
        if (i % 8 == 0) {
            angle_hit_map->GetXaxis()->SetBinLabel(i + 1, Form("%02d", i));
            angle_hit_map->GetYaxis()->SetBinLabel(i + 1, Form("%02d", i));
        }
    }

    // angle_hist
    TH1D* angle_hist = new TH1D("angle_hist", "Angle Distribution", options_mgr.number_of_bins, 0, 360);
    angle_hist->SetDirectory(NULL);

    // distance_hist
    TH1D* distance_hist = new TH1D("distance_hist", "Distance Distribution", 200, 0, 300);
    distance_hist->SetDirectory(NULL);

    // energy_ratio_hist
    TH1D* energy_ratio_hist = new TH1D("energy_ratio_hist", "Energy Ratio Distribution", 200, 0, 1);
    energy_ratio_hist->SetDirectory(NULL);

    bool is_first = true;
    double first_time = 0;
    double last_time = 0;

    // reading angle data
    int pre_percent = 0;
    int cur_percent = 0;
    cout << "reading angle data ..." << endl;
    cout << "[ " << flush;
    for (Long64_t q = 0; q < t_pol_angle_tree->GetEntries(); q++) {
        cur_percent = static_cast<int>(q * 100.0 / t_pol_angle_tree->GetEntries());
        if (cur_percent - pre_percent > 0 && cur_percent % 2 == 0) {
            pre_percent = cur_percent;
            cout << "#" << flush;
        }
        t_pol_angle_tree->GetEntry(q);

        if (is_first) {
            first_time = t_pol_angle.event_time;
            is_first = false;
        }
        last_time = t_pol_angle.event_time;

        if (t_pol_angle.is_na22) continue;
        if (!t_pol_angle.is_valid) continue;

        double cur_weight = t_pol_angle.efficiency_weight * t_pol_angle.deadtime_weight;
        angle_hist->Fill(t_pol_angle.rand_angle, cur_weight);
        distance_hist->Fill(t_pol_angle.rand_distance, cur_weight);
        energy_ratio_hist->Fill(t_pol_angle.second_energy / t_pol_angle.first_energy, cur_weight);

        int first_x = ijtox(t_pol_angle.first_ij[0], t_pol_angle.first_ij[1]) + 1;
        int first_y = ijtoy(t_pol_angle.first_ij[0], t_pol_angle.first_ij[1]) + 1;
        int second_x = ijtox(t_pol_angle.second_ij[0], t_pol_angle.second_ij[1]) + 1;
        int second_y = ijtoy(t_pol_angle.second_ij[0], t_pol_angle.second_ij[1]) + 1;

        double first_content = angle_hit_map->GetBinContent(first_x, first_y);
        angle_hit_map->SetBinContent(first_x, first_y, first_content + cur_weight);
        double second_content = angle_hit_map->GetBinContent(second_x, second_y);
        angle_hit_map->SetBinContent(second_x, second_y, second_content + cur_weight);

    }
    cout << " DONE ]" << endl;
    angle_file->Close();
    delete angle_file;
    angle_file = NULL;

    cout << endl;
    double duration_time = last_time - first_time;
    double integration = angle_hist->Integral();
    cout << "duration time   : " << static_cast<int>(duration_time) << endl;
    cout << "integration     : " << static_cast<int>(integration) << endl;
    cout << "rate (counts/s) : " << static_cast<int>(integration / duration_time) << endl;
    cout << endl;

    if (!options_mgr.output_filename.IsNull()) {
        cout << "saving modulation curve ..." << endl;
        TFile* output_file = new TFile(options_mgr.output_filename.Data(), "recreate");
        if (output_file->IsZombie()) {
            cout << "output_file open failed: " << options_mgr.output_filename.Data() << endl;
            return 1;
        }
        output_file->cd();
        TH1D* angle_hist_norm = static_cast<TH1D*>(angle_hist->Clone("angle_hist_norm"));
        double scale = 1.0 / angle_hist_norm->Integral();
        angle_hist_norm->Scale(scale);
        angle_hist_norm->Write();
        TNamed("nbins", Form("%d", options_mgr.number_of_bins)).Write();
        TNamed("total", Form("%d", static_cast<int>(1.0 / scale))).Write();
        output_file->Close();
        delete output_file;
    }

    // angle hist
    canvas.cd(1);
    canvas.get_canvas()->GetPad(1)->SetGrid();
    angle_hist->DrawCopy("eh");

    // angle hit map
    canvas.cd(2);
    canvas.get_canvas()->GetPad(2)->SetGrid();
    angle_hit_map->DrawCopy("COLZ");
    for (int i = 0; i < 4; i++) {
        line_h[i]->Draw();
        line_v[i]->Draw();
    }

    // distance hist
    canvas.cd(3);
    distance_hist->DrawCopy("eh");

    // energy ratio hist
    canvas.cd(4);
    energy_ratio_hist->DrawCopy("eh");

    rootapp->Run();
    return 0;
}
