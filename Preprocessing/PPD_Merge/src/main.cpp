#include <iostream>
#include <iomanip>
#include "RootInc.hpp"
#include "PPDType1M.hpp"

using namespace std;

const int    METStartGPSWeek = 1914;
const double METStartGPSSecond = 396845;

double utc_to_met(double utc_sec) {
    double leap_seconds = 0;
    if (utc_sec < 1119744000.0) {
        leap_seconds = 16;
    } else if (utc_sec < 1167264000.0) {
        leap_seconds = 17;
    } else {
        leap_seconds = 18;
    }
    double gps_sec = utc_sec + leap_seconds;
    double met_sec = gps_sec - (METStartGPSWeek * 604800 + METStartGPSSecond);
    return met_sec;
}


int main(int argc, char** argv) {
    if (argc < 3) {
        cout << "USAGE: " << argv[0] << " <ppd_merged.root> <ppd1.root> <ppd2.root> ..." << endl;
        return 2;
    }

    PPDType1M::PPD_T t_ppd;
    double met_ppd = 0, met_ppd_pre = 0;
    bool ppd_is_first = true;

    string output_filename = argv[1];
    cout << "output_filename = " << output_filename << endl;

    cout << "Opening output file: " << output_filename << " ..." << endl;
    TFile* t_output_file = new TFile(output_filename.c_str(), "recreate");
    if (t_output_file->IsZombie()) {
        cout << "output file open failed: " << output_filename << endl;
        return 1;
    }
    t_output_file->cd();
    TTree* t_ppd_tree_out = new TTree("t_ppd", "ppd data");
    t_ppd_tree_out->Branch("met_second", &met_ppd, "met_second/D");
    PPDType1M::build_ppd_tree(t_ppd_tree_out, t_ppd);

    cout << "Reading and merging input files: " << endl;
    for (int i = 2; i < argc; i++) {
        cout << argv[i] << " ..." << endl;
        // open input file
        TFile* t_input_file = new TFile(argv[i], "read");
        if (t_input_file->IsZombie()) {
            cout << "input file open failed: " << argv[i] << endl;
            return 1;
        }
        TTree* t_ppd_tree_in = static_cast<TTree*>(t_input_file->Get("t_ppd"));
        if (t_ppd_tree_in == NULL) {
            cout << "cannot find TTree t_ppd" << endl;
            return 1;
        } else {
            PPDType1M::bind_ppd_tree(t_ppd_tree_in, t_ppd);
        }

        // reading and writting ppd data
        for (Long64_t i = 0; i < t_ppd_tree_in->GetEntries(); i++) {
            t_ppd_tree_in->GetEntry(i);
            if (t_ppd.flag_of_pos != 0x55 || t_ppd.utc_time_sec < 1.0) continue;
            met_ppd = utc_to_met(t_ppd.utc_time_sec);
            if (ppd_is_first) {
                ppd_is_first = false;
                met_ppd_pre = met_ppd;
                t_ppd_tree_out->Fill();
            } else {
                if (met_ppd > met_ppd_pre) {
                    met_ppd_pre = met_ppd;
                    t_ppd_tree_out->Fill();
                }
            }
        }

        // close input file
        t_input_file->Close();
        delete t_input_file;

    }

    t_output_file->cd();
    t_ppd_tree_out->Write();

    t_output_file->Close();
    delete t_output_file;

    return 0;
}
