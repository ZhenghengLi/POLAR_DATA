#include <iostream>
#include <cstdio>
#include <iomanip>
#include <cstdlib>
#include "RootInc.hpp"
#include "PhyEventFile.hpp"
#include "ComptonEdgeCalc.hpp"

using namespace std;

void print_help() {
    cout << endl;
    cout << "USAGE: " << "CE_Calibrate <infile_name.root> -F <specfile_name.root>" << endl;
    cout << "       " << "CE_Calibrate -f <specfile_name.root> -V <kvecfile_name.root>" << endl;
    cout << "       " << "CE_Calibrate -f <specfile_name.root> -m" << endl;
    cout << "       " << "CE_Calibrate -v <kvecfile_name.root> -m" << endl;
    cout << "       " << "CE_Calibrate <infile_name.root> -v <kvecfile_name.root> -o <outfile_name.root>" << endl;
    cout << endl;
}

int main(int argc, char** argv) {
    TApplication* rootapp = new TApplication("POLAR", &argc, argv);

    // == process command line parameters =================================
    TString infile_name;              
    TString specfile_name;            // -F | -f
    TString kvecfile_name;            // -V | -v
    TString outfile_name;             // -o
    bool show_map = false;            // -m
    char mode_1 = 'o';                // 'w' | 'r'
    char mode_2 = 'o';                // 'w' | 'r'

    TString cur_par_str;
    int argv_idx = 0;
    while (argv_idx < rootapp->Argc() - 1) {
        cur_par_str = rootapp->Argv(++argv_idx);
        if (cur_par_str == "-F") {
            specfile_name = rootapp->Argv(++argv_idx);
            mode_1 = 'w';
        } else if (cur_par_str == "-f") {
            specfile_name = rootapp->Argv(++argv_idx);
            mode_1 = 'r';
        } else if (cur_par_str == "-V") {
            kvecfile_name = rootapp->Argv(++argv_idx);
            mode_2 = 'w';
        } else if (cur_par_str == "-v") {
            kvecfile_name = rootapp->Argv(++argv_idx);
            mode_2 = 'r';
        } else if (cur_par_str == "-o") {
            outfile_name = rootapp->Argv(++argv_idx);
        } else if (cur_par_str == "-m") {
            show_map = true;
        } else {
            infile_name = cur_par_str;
        }
    }

    if (specfile_name.IsNull() && kvecfile_name.IsNull()) {
         print_help();
        exit(1);
    }

    // ====================================================================

    PhyEventFile phyEventFile_R;
    if (!infile_name.IsNull()) {
        if (!phyEventFile_R.open(infile_name.Data(), 'r')) {
            cerr << "root file open failed: " << infile_name.Data() << endl;
            exit(1);
        }
    }

    ComptonEdgeCalc comptonEdgeCalc;
    if (!specfile_name.IsNull()) {
        if (!comptonEdgeCalc.open(specfile_name.Data(), mode_1)) {
            cerr << "root file open failed: " << specfile_name.Data() << endl;
            phyEventFile_R.close();
            exit(1);
        }
        if (mode_1 == 'r') {
            cout << "Do fitting ... " << endl;
            comptonEdgeCalc.do_fit();
        }
    }

    if (mode_2 == 'r' && !kvecfile_name.IsNull()) {
        cout << "Reading ADC/keV vector from file: " << kvecfile_name.Data() << " ... " << endl;
        if (!comptonEdgeCalc.read_kvec(kvecfile_name.Data())) {
            cerr << "root file open failed: " << kvecfile_name.Data() << endl;
            phyEventFile_R.close();
            comptonEdgeCalc.close();
            exit(1);
        }
    }

    int global_mode = 0;

    if (mode_1 == 'w' && !infile_name.IsNull() && !specfile_name.IsNull()) {
        global_mode = 1;
    } else if (mode_1 == 'r' && mode_2 == 'w') {
        global_mode = 2;
    } else if (mode_1 == 'r' && mode_2 != 'r' && show_map) {
        global_mode = 3;
    } else if (mode_2 == 'r' && mode_1 != 'r' && show_map) {
        global_mode = 4;
    } else if (mode_2 == 'r' && !infile_name.IsNull() && !outfile_name.IsNull()) {
        global_mode = 5;
    } else {
        print_help();
        exit(1);
    }

    int pre_percent = 0;
    int cur_percent = 0;
    PhyEventFile phyEventFile_W;
    
    switch (global_mode) {
    case 1:
        cout << "Filling the data of Spectrum ... " << endl;
        comptonEdgeCalc.do_fill(phyEventFile_R);
        comptonEdgeCalc.close();
        cout << "[ DONE ]" << endl;
        break;
    case 2:
        cout << "Writing ADC/keV vector to file: " << kvecfile_name.Data() << " ... " << endl;
        comptonEdgeCalc.write_kvec(kvecfile_name.Data());
        cout << "[ DONE ]" << endl;
        break;
    case 3:
        cout << "Showing the counts map and spectrum of each channel ... " << endl;
        comptonEdgeCalc.show_counts();
        cout << "[ DONE ]" << endl;
        break;
    case 4:
        cout << "Showing the ADC/keV vector of each module ... " << endl;
        comptonEdgeCalc.show_adc_per_kev();
        cout << "[ DONE ]" << endl;
        break;
    case 5:
        cout << "Writing calibrated data to " << outfile_name.Data() << " ... " << endl;
        cout << "[ #" << flush;
        if (!phyEventFile_W.open(outfile_name.Data(), 'w')) {
            cerr << "root file open failed: " << outfile_name.Data() << endl;
            phyEventFile_R.close();
            exit(1);
        }
        phyEventFile_R.trigg_restart();
        while (phyEventFile_R.trigg_next()) {
            cur_percent = static_cast<int>(100 * phyEventFile_R.current_index() / phyEventFile_R.total_entries());
            if (cur_percent - pre_percent > 0 && cur_percent % 2 == 0) {
                pre_percent = cur_percent;
                cout << "#" << flush;
            }
            comptonEdgeCalc.do_move_trigg(phyEventFile_W, phyEventFile_R);
            phyEventFile_W.write_trigg();
            while (phyEventFile_R.event_next()) {
                comptonEdgeCalc.do_calibrate(phyEventFile_W, phyEventFile_R);
                phyEventFile_W.write_event();
            }
        }
        phyEventFile_R.trigg_restart();
        phyEventFile_W.close();
        cout << " DONE]" << endl;
        break;
    }

    if (!infile_name.IsNull())
        phyEventFile_R.close();
    if (global_mode == 3 || global_mode == 4)
        rootapp->Run(); 
    
    return 0;
}
