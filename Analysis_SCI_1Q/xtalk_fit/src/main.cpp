#include <iostream>
#include "RootInc.hpp"
#include "OptionsManager.hpp"
#include "CooConv.hpp"
#include "CommonCanvas.hpp"

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

    // open xtalk_data_file
    TFile* xtalk_data_file = new TFile(options_mgr.xtalk_data_filename.Data(), "read");
    if (xtalk_data_file->IsZombie()) {
        cout << "xtalk_data_file open failed." << endl;
        return 1;
    }
    TTree* t_xtalk_data_tree = static_cast<TTree*>(xtalk_data_file->Get(Form("t_xtalk_data_ct_%02d", options_mgr.ct_num)));
    if (t_xtalk_data_tree == NULL) {
        cout << "cannot find TTree " << Form("t_xtalk_data_ct_%02d", options_mgr.ct_num) << endl;
        return 1;
    }
    struct {
        Double_t event_time;
        Int_t    jx;
        Int_t    jy;
        Float_t  x;
        Float_t  y;
        Float_t  fe_temp;
        Float_t  fe_hv;
        Float_t  aux_interval;
    } t_xtalk_data;
    t_xtalk_data_tree->SetBranchAddress("event_time",     &t_xtalk_data.event_time     );
    t_xtalk_data_tree->SetBranchAddress("jx",             &t_xtalk_data.jx             );
    t_xtalk_data_tree->SetBranchAddress("jy",             &t_xtalk_data.jy             );
    t_xtalk_data_tree->SetBranchAddress("x",              &t_xtalk_data.x              );
    t_xtalk_data_tree->SetBranchAddress("y",              &t_xtalk_data.y              );
    t_xtalk_data_tree->SetBranchAddress("fe_temp",        &t_xtalk_data.fe_temp        );
    t_xtalk_data_tree->SetBranchAddress("fe_hv",          &t_xtalk_data.fe_hv          );
    t_xtalk_data_tree->SetBranchAddress("aux_interval",   &t_xtalk_data.aux_interval   );
    cout << options_mgr.xtalk_data_filename.Data() << " { " << "ct_num = " << options_mgr.ct_num << " }" << endl;


    // prepare histogram
    TF1*   xtalk_line[64][64];
    TH2F*  xtalk_hist[64][64];
    for (int jx = 0; jx < 64; jx++) {
        for (int jy = 0; jy < 64; jy++) {
            if (jx == jy)
                continue;
             xtalk_hist[jx][jy] = new TH2F(Form("xtalk_hist%02d_%02d", jx + 1, jy + 1),
                                           Form("Crosstalk of %02d => %02d", jx + 1, jy + 1),
                                           256, 0, 4096, 128, -128, 1536);
            xtalk_hist[jx][jy]->SetDirectory(NULL);
            // xtalk_hist[jx][jy]->SetMarkerColor(9);
            // xtalk_hist[jx][jy]->SetMarkerStyle(31);
            xtalk_line[jx][jy] = new TF1(Form("xtalk_line%02d_%02d", jx + 1, jy + 1),
                                          "[0] * x", 0, 4096);
            xtalk_line[jx][jy]->SetParameter(0, 0.1);
        }
    }

    // reading and selecting data
    int pre_percent = 0;
    int cur_percent = 0;
    cout << "Filling crosstalk histogram of CT_" << options_mgr.ct_num << " ..." << endl;
    cout << "[ " << flush;
    for (Long64_t q = 0; q < t_xtalk_data_tree->GetEntries(); q++) {
        cur_percent = static_cast<int>(q * 100.0 / t_xtalk_data_tree->GetEntries());
        if (cur_percent - pre_percent > 0 && cur_percent % 2 == 0) {
            pre_percent = cur_percent;
            cout << "#" << flush;
        }
        t_xtalk_data_tree->GetEntry(q);

        // filter start
        if (t_xtalk_data.aux_interval > 20) continue;
        if (t_xtalk_data.fe_temp < options_mgr.low_temp) continue;
        if (t_xtalk_data.fe_temp > options_mgr.high_temp) continue;
        if (t_xtalk_data.fe_hv < options_mgr.low_hv) continue;
        if (t_xtalk_data.fe_hv > options_mgr.high_hv) continue;
        // filter stop
        xtalk_hist[t_xtalk_data.jx][t_xtalk_data.jy]->Fill(
                t_xtalk_data.x, t_xtalk_data.y);

    }
    cout << " DONE ]" << endl;

    // fit xtalk_hist
    cout << "Fitting crosstalk histogram ..." << endl;
    TMatrixF xtalk_matrix(64, 64);
    TMatrixF xtalk_matrix_err(64, 64);
    TMatrixF xtalk_matrix_inv(64, 64);
    for (int jx = 0; jx < 64; jx++) {
        for (int jy = 0; jy < 64; jy++) {
            if (jx == jy) {
                xtalk_matrix(jy, jx) = 1.0;
                xtalk_matrix_err(jy, jx) = 0;
                continue;
            }
            if (xtalk_hist[jx][jy]->GetEntries() < 5) {
                cerr << "CT_" << options_mgr.ct_num << " : " << jx + 1 << " => " << jy + 1
                     << "    " << "number of entries is too small" << endl;
                xtalk_matrix(jy, jx) = 0.0001 * gRandom->Rndm();
                xtalk_matrix_err(jy, jx) = 0;
            } else {
                xtalk_hist[jx][jy]->Fit(xtalk_line[jx][jy], "RQN");
                xtalk_matrix(jy, jx) = (xtalk_line[jx][jy]->GetParameter(0) > 0 ?
                        xtalk_line[jx][jy]->GetParameter(0) : 0.000001 * gRandom->Rndm());
                xtalk_matrix_err(jy, jx) = (xtalk_line[jx][jy]->GetParameter(0) > 0 ?
                        xtalk_line[jx][jy]->GetParError(0) : 0);
            }
        }
    }
    xtalk_matrix_inv = xtalk_matrix;
    // begin protection for bad channels
    int i = options_mgr.ct_num - 1;
    if (i == 1 || i == 4 || i == 5 || i == 7 || i == 21) {
        for (int j = 0; j < 64; j++) {
            if (j != 23 && j != 15 && j != 14 && j != 22 && j != 30 && j != 31) {
                xtalk_matrix(23, j) = 0.000001 * gRandom->Rndm();
                xtalk_matrix_err(23, j) = 0;
            }
        }
    }
    // end protection for bad channels
    xtalk_matrix_inv.Invert();

    // save result
    if (!options_mgr.xtalk_result_filename.IsNull()) {
        cout << "Saving crosstalk matrix of CT " << options_mgr.ct_num << " ..." << endl;
        TFile* xtalk_result_file = new TFile(options_mgr.xtalk_result_filename.Data(), "recreate");
        if (xtalk_result_file->IsZombie()) {
            cout << "xtalk_result_file open failed." << endl;
            return 1;
        }
        xtalk_result_file->cd();
        TNamed("ct_num", Form("%d", options_mgr.ct_num)).Write();
        xtalk_matrix.Write("xtalk_matrix");
        xtalk_matrix_err.Write("xtalk_matrix_err");
        xtalk_matrix_inv.Write("xtalk_matrix_inv");
        TNamed("low_temp", Form("%f", options_mgr.low_temp)).Write();
        TNamed("high_temp", Form("%f", options_mgr.high_temp)).Write();
        TNamed("low_hv", Form("%f", options_mgr.low_hv)).Write();
        TNamed("high_hv", Form("%f", options_mgr.high_hv)).Write();
        xtalk_result_file->Close();
        delete xtalk_result_file;
        xtalk_result_file = NULL;
    }

    if (options_mgr.show_flag) {
        cout << "Showing crosstalk matrix of CT " << options_mgr.ct_num << " ..." << endl;
        TApplication* rootapp = new TApplication("POLAR", NULL, NULL);
        CommonCanvas canvas_xtalk(options_mgr.ct_num);
        canvas_xtalk.init(xtalk_matrix, xtalk_hist, xtalk_line);
        canvas_xtalk.draw_xtalk_matrix();
        rootapp->Run();
    }

    return 0;
}
