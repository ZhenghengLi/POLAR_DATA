#include <iostream>
#include "RootInc.hpp"
#include "CommonCanvas.hpp"
#include "CooConv.hpp"

using namespace std;

int main(int argc, char** argv) {
    if (argc < 2) {
        cout << "USAGE: " << argv[0] << " <xtalk_mat.root>" << endl;
        return 2;
    }

    TMatrixF xtalk_mat_ct[25];
    for (int i = 0; i < 25; i++) {
        xtalk_mat_ct[i].ResizeTo(64, 64);
    }

    // open xtalk_mat_file
    TFile* xtalk_mat_file = new TFile(argv[1], "read");
    if (xtalk_mat_file->IsZombie()) {
        cout << "xtalk matrix file open failed: " << argv[1] << endl;
        return 1;
    }
    TMatrixF* tmp_mat;
    // read xtalk matrix
    for (int i = 0; i < 25; i++) {
        tmp_mat = static_cast<TMatrixF*>(xtalk_mat_file->Get(Form("xtalk_mat_ct_%02d", i + 1)));
        if (tmp_mat == NULL) {
            cout << "cannot find TMatrixF " << Form("xtalk_mat_ct_%02d", i + 1) << endl;
            return 1;
        } else {
            xtalk_mat_ct[i] = *tmp_mat;
        }
    }
    xtalk_mat_file->Close();
    delete xtalk_mat_file;
    xtalk_mat_file = NULL;

    // show
    cout << "Showing crosstalk matrix map of all modules ..." << endl;
    TApplication* rootapp = new TApplication("POLAR", NULL, NULL);
    CommonCanvas canvas_xtalk_matrix;
    gStyle->SetOptStat(0);
    canvas_xtalk_matrix.init(xtalk_mat_ct);
    canvas_xtalk_matrix.draw_xtalk_map_all();
    rootapp->Run();
    return 0;
}
