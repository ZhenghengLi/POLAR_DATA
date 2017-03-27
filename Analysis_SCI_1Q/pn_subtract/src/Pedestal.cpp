#include "Pedestal.hpp"

Pedestal::Pedestal() {
    for (int i = 0; i < 25; i++) {
        ped_mean_vec_CT_[i].ResizeTo(64);
        ped_vec_CT[i].ResizeTo(64);
    }
}

Pedestal::~Pedestal() {

}

bool Pedestal::read_pedestal(const char* ped_filename) {
    TFile* ped_file = new TFile(ped_filename, "read");
    if (ped_file->IsZombie()) {
        return false;
    }
    TVectorF* tmp_vec;
    for (int i = 0; i < 25; i++) {
        tmp_vec = static_cast<TVectorF*>(ped_file->Get(Form("ped_mean_vec_ct_%02d", i + 1)));
        if (tmp_vec == NULL) {
            cout << "cannot find TVectorF " << Form("ped_mean_vec_ct_%02d", i + 1) << endl;
            return false;
        } else {
            ped_mean_vec_CT_[i] = *tmp_vec;
        }
    }
    ped_file->Close();
    delete ped_file;
    ped_file = NULL;
    return true;
}

void Pedestal::gen_pedestal() {
    for (int i = 0; i < 25; i++) {
        ped_vec_CT[i] = ped_mean_vec_CT_[i];
    }
}

void Pedestal::gen_pedestal(int, double) {

}

