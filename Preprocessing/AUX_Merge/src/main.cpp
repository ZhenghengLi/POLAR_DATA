#include <iostream>
#include <iomanip>
#include "RootInc.hpp"
#include "HkType1M.hpp"

using namespace std;

const int    METStartGPSWeek = 1914;
const double METStartGPSSecond = 396845;

double gps_to_met(int gps_week, double gps_second) {
    return ((gps_week - METStartGPSWeek) * 604800 + (gps_second - METStartGPSSecond));
}

int main(int argc, char** argv) {
    if (argc < 3) {
        cout << "USAGE: " << argv[0] << " <aux_merged.root> <aux1.root> <aux2.root> ..." << endl;
        return 2;
    }

    HkType1M::Hk_Obox_T t_hk_obox;
    HkType1M::Hk_Ibox_T t_hk_ibox;
    double met_obox = 0, met_obox_pre = 0;
    double met_ibox = 0, met_ibox_pre = 0;
    bool obox_is_first = true, ibox_is_first = true;

    string output_filename = argv[1];
    cout << "output_filename = " << output_filename << endl;

    cout << "Opening output file: " << output_filename << " ..." << endl;
    TFile* t_output_file = new TFile(output_filename.c_str(), "recreate");
    if (t_output_file->IsZombie()) {
        cout << "output file open failed: " << output_filename << endl;
        return 1;
    }
    t_output_file->cd();
    TTree* t_hk_obox_tree_out = new TTree("t_hk_obox", "obox housekeeping packets");
    t_hk_obox_tree_out->Branch("met_second", &met_obox, "met_second/D");
    HkType1M::build_hk_obox_tree(t_hk_obox_tree_out, t_hk_obox);
    TTree* t_hk_ibox_tree_out = new TTree("t_hk_ibox", "ibox housekeeping info");
    t_hk_ibox_tree_out->Branch("met_second", &met_ibox, "met_second/D");
    HkType1M::build_hk_ibox_tree(t_hk_ibox_tree_out, t_hk_ibox);

    cout << "Reading and merging input files: " << endl;
    for (int i = 2; i < argc; i++) {
        cout << argv[i] << " ..." << endl;
        // open input file
        TFile* t_input_file = new TFile(argv[i], "read");
        if (t_input_file->IsZombie()) {
            cout << "input file open failed: " << argv[i] << endl;
            return 1;
        }
        TTree* t_hk_obox_tree_in = static_cast<TTree*>(t_input_file->Get("t_hk_obox"));
        if (t_hk_obox_tree_in == NULL) {
            cout << "cannot find TTree t_hk_obox" << endl;
            return 1;
        } else {
            HkType1M::bind_hk_obox_tree(t_hk_obox_tree_in, t_hk_obox);
        }
        TTree* t_hk_ibox_tree_in = static_cast<TTree*>(t_input_file->Get("t_hk_ibox"));
        if (t_hk_ibox_tree_in == NULL) {
            cout << "cannot find TTree t_hk_ibox" << endl;
            return 1;
        } else {
            HkType1M::bind_hk_ibox_tree(t_hk_ibox_tree_in, t_hk_ibox);
        }

        // reading and writting obox data
        for (Long64_t i = 0; i < t_hk_obox_tree_in->GetEntries(); i++) {
            t_hk_obox_tree_in->GetEntry(i);
            if (t_hk_obox.obox_is_bad) continue;
            met_obox = gps_to_met(t_hk_obox.abs_gps_week, t_hk_obox.abs_gps_second);
            if (obox_is_first) {
                obox_is_first = false;
                met_obox_pre = met_obox;
                t_hk_obox_tree_out->Fill();
            } else {
                if (met_obox > met_obox_pre) {
                    met_obox_pre = met_obox;
                    t_hk_obox_tree_out->Fill();
                }
            }
        }

        // reading and writting ibox data
        for (Long64_t i = 0; i < t_hk_ibox_tree_in->GetEntries(); i++) {
            t_hk_ibox_tree_in->GetEntry(i);
            if (t_hk_ibox.is_bad) continue;
            met_ibox = gps_to_met(t_hk_ibox.abs_gps_week, t_hk_ibox.abs_gps_second);
            if (ibox_is_first) {
                ibox_is_first = false;
                met_ibox_pre = met_ibox;
                t_hk_ibox_tree_out->Fill();
            } else {
                if (met_ibox > met_ibox_pre) {
                    met_ibox_pre = met_ibox;
                    t_hk_ibox_tree_out->Fill();
                }
            }
        }

        // close input file
        t_input_file->Close();
        delete t_input_file;

    }

    t_output_file->cd();
    t_hk_obox_tree_out->Write();
    t_hk_ibox_tree_out->Write();

    t_output_file->Close();
    delete t_output_file;

    return 0;
}
