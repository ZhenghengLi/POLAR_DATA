#include "Crosstalk.hpp"

Crosstalk::Crosstalk() {
    for (int i = 0; i < 25; i++) {
        xtalk_mat_inv_CT_[i].ResizeTo(64, 64);
        xtalk_matrix_inv_CT[i].ResizeTo(64, 64);
    }
}

Crosstalk::~Crosstalk() {

}

bool Crosstalk::read_crosstalk(const char* xtalk_filename) {
    TFile* xtalk_file = new TFile(xtalk_filename, "read");
    if (xtalk_file->IsZombie()) {
        return false;
    }
    TMatrixF* tmp_mat;
    for (int i = 0; i < 25; i++) {
        tmp_mat = static_cast<TMatrixF*>(xtalk_file->Get(Form("xtalk_mat_inv_ct_%02d", i + 1)));
        if (tmp_mat == NULL) {
            cout << "cannot find TMatrixF " << Form("xtalk_mat_inv_ct_%02d", i + 1) << endl;
            return false;
        } else {
            xtalk_mat_inv_CT_[i] = *tmp_mat;
        }
    }
    xtalk_file->Close();
    delete xtalk_file;
    xtalk_file = NULL;
    return true;
}

void Crosstalk::gen_crosstalk() {
    for (int i = 0; i < 25; i++) {
        xtalk_matrix_inv_CT[i] = xtalk_mat_inv_CT_[i];
    }
}

