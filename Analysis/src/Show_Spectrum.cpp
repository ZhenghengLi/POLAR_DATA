#include <iostream>
#include <cstdlib>
#include <iomanip>
#include <cstdio>
#include "PhyEventFile.hpp"
#include "CooConv.hpp"

#define BINS 100

using namespace std;

void print_help() {
    cout << "Help message" << endl;
}

int main(int argc, char** argv) {

    TApplication* rootapp = new TApplication("POLAR", &argc, argv);

    // == process command line parameters =================================
    TString infile_name;
    TString outfile_name;      // -o
    TString number_str;            // -n
    int number = 0;
    TString cur_par_str;
    int argv_idx = 0;
    while (argv_idx < rootapp->Argc() - 1) {
        cur_par_str = rootapp->Argv(++argv_idx);
        if (cur_par_str == "-o") {
            outfile_name = rootapp->Argv(++argv_idx);
        } if (cur_par_str == "-n") {
            number_str = rootapp->Argv(++argv_idx);
            number = number_str.Atoi();
        } else {
            infile_name = cur_par_str;
        }
    }
    if (infile_name.IsNull() || number == 0) {
        print_help();
        exit(1);
    }
    // ====================================================================
    
    PhyEventFile phyEventFile;
    phyEventFile.open(infile_name.Data(), 'r');

    TH1F* h_spec[64];
    char name[50];
    char title[50];
    for (int j = 0; j < 64; j++) {
        sprintf(name, "spec_%d_%d", number, j + 1);
        sprintf(title, "Spectrum of CH %d_%d", number, j + 1);
        h_spec[j] = new TH1F(name, title, BINS, 0, 4096);
        h_spec[j]->SetDirectory(NULL);
    }
    while (phyEventFile.trigg_next()) {
        while (phyEventFile.event_next()) {
            if (phyEventFile.event.ct_num != number)
                continue;
            for (int j = 0; j < 64; j++) {
                if (phyEventFile.event.trigger_bit[j])
                    h_spec[j]->Fill(phyEventFile.event.energy_ch[j]);
            }
        }
    }

    phyEventFile.close();
    TCanvas* canvas = new TCanvas("canvas", "Spectrum of ADC Channel", 800, 600);
    canvas->Divide(8, 8);
    for (int j = 0; j < 64; j++) {
        canvas->cd(jtoc(j));
        canvas->GetPad(j + 1)->SetLogy();
        h_spec[j]->Draw();
    }
        
    cout << "All Finished." << endl;
    
    // ==================================================================== 
    rootapp->Run();
    delete rootapp;
    return 0;
}
