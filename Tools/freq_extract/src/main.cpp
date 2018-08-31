#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <stdint.h>
#include "GPSTime.hpp"
#include "HkGPSIterator.hpp"
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
        return 1;
    }

    HkGPSIterator hkGPSIter;
    if (!hkGPSIter.open(options_mgr.hkfile.Data())) {
        cerr << "root file open falied: " << options_mgr.hkfile << endl;
        return 1;
    }
    hkGPSIter.initialize();

    struct {
        double met_time;
        double mean_temp;
        double freq;
    } t_freq;
    TFile* outfile = new TFile(options_mgr.outfile.Data(), "recreate");
    if (outfile->IsZombie()) {
        cout << "outfile open failed: " << options_mgr.outfile << endl;
        return 1;
    }
    TTree* t_freq_tree = new TTree("t_freq", "freq");
    t_freq_tree->Branch("met_time",   &t_freq.met_time  );
    t_freq_tree->Branch("mean_temp",  &t_freq.mean_temp );
    t_freq_tree->Branch("freq",       &t_freq.freq      );

    while (hkGPSIter.next_minute()) {
        if (hkGPSIter.cur_freq.found_init) continue;
        t_freq.met_time  = hkGPSIter.cur_freq.met_time;
        t_freq.mean_temp = hkGPSIter.cur_freq.mean_temp;
        t_freq.freq      = hkGPSIter.cur_freq.freq;
        t_freq_tree->Fill();
    }

    outfile->cd();
    t_freq_tree->Write();
    outfile->Close();
    delete outfile;

    return 0;
}
