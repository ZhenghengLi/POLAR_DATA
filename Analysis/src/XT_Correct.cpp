#include <iostream>
#include <cstdlib>
#include <iomanip>
#include <cstdio>
#include "PhyEventFile.hpp"
#include "CrossTalkCalc.hpp"

using namespace std;

void print_help() {
    cout << endl;
    cout << "USAGE: " << "XT_Correct <infile_name.root> -F <xtalkfile_name.root>" << endl;
    cout << "       " << "XT_Correct -f <xtalkfile_name.root> -X <xmatfile_name.root>" << endl;
    cout << "       " << "XT_Correct <infile_name.root> -x <xmatfile_name.root> -o <outfile_name.root>" << endl;
    cout << "       " << "XT_Correct -x <xmatfile_name> -m" << endl;
    cout << endl;
}

int main(int argc, char** argv) {
    
    TApplication* rootapp = new TApplication("POLAR", &argc, argv);

    // == process command line parameters =================================

    TString infile_name;
    TString xtalkfile_name;        // -F | -f
    TString xmatfile_name;         // -X | -x
    TString outfile_name;          // -o
    bool show_map = false;         // -m
    char mode_1 = 'o';             // 'w' | 'r'
    char mode_2 = 'o';             // 'w' | 'r'

    TString cur_par_str;
    int argv_idx = 0;
    while (argv_idx < rootapp->Argc() - 1) {
        cur_par_str = rootapp->Argv(++argv_idx);
        if (cur_par_str == "-F") {
            xtalkfile_name = rootapp->Argv(++argv_idx);
            mode_1 = 'w';
        } else if (cur_par_str == "-f") {
            xtalkfile_name = rootapp->Argv(++argv_idx);
            mode_1 = 'r';
        } else if (cur_par_str == "-X") {
            xmatfile_name = rootapp->Argv(++argv_idx);
            mode_2 = 'w';
        } else if (cur_par_str == "-x") {
            xmatfile_name = rootapp->Argv(++argv_idx);
            mode_2 = 'r';
        } else if (cur_par_str == "-o") {
            outfile_name = rootapp->Argv(++argv_idx);
        } else if (cur_par_str == "-m") {
            show_map = true;
        } else {
            infile_name = cur_par_str;
        }
    }

    if (xtalkfile_name.IsNull() && xmatfile_name.IsNull()) {
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

    CrossTalkCalc crossTalkCalc;
    if (!xtalkfile_name.IsNull()) {
        if (!crossTalkCalc.open(xtalkfile_name.Data(), mode_1)) {
            cerr << "root file open failed: " << xtalkfile_name.Data() << endl;
            phyEventFile_R.close();
            exit(1);
        }
    }
    if (mode_2 == 'r' && !xmatfile_name.IsNull()) {
        cout << "Reading cross talk matrix from file: " << xmatfile_name.Data() << " ... " << endl;
        crossTalkCalc.read_xmat(xmatfile_name.Data());
    }
    
    int global_mode = 0;   // 1, 2, 3, 4

    if (mode_1 == 'w' && !infile_name.IsNull() && !xtalkfile_name.IsNull()) {
        global_mode = 1;
    } else if (mode_1 == 'r' && mode_2 == 'w') {
        global_mode = 2;
    } else if (xtalkfile_name.IsNull() && mode_2 == 'r' && !infile_name.IsNull() && !outfile_name.IsNull()) {
        global_mode = 3;
    } else if (xtalkfile_name.IsNull() && mode_2 == 'r' && show_map && infile_name.IsNull() && outfile_name.IsNull()) {
        global_mode = 4;
    } else {
        print_help();
        exit(1);
    }

    switch (global_mode) {
    case 1:
        cout << "Filling the data of Cross Talk ... " << endl;
        crossTalkCalc.do_fill(phyEventFile_R);
        crossTalkCalc.close();
        cout << "[ DONE ]" << endl;
        break;
    case 2:
        cout << "Do fitting ... " << endl;
        crossTalkCalc.do_fit();
        cout << "Writing cross talk matrix to file: " << xmatfile_name.Data() << " ... " << endl;
        crossTalkCalc.write_xmat(xmatfile_name.Data());
        cout << "[ DONE ]" << endl;
        break;
    case 3:
//        int pre_percent = 0;
//        int cur_percent = 0;
        cout << "writing corrected data to " << outfile_name.Data() << " ... " << endl;
        cout << "[ #" << flush;
        cout << " DONE ]" << endl;
        break;
    case 4:
        cout << "Showing cross talk map of all modules ... " << endl;
        crossTalkCalc.show_xtalk();
        cout << "[ DONE ]" << endl;
        break;
    }

    if (!infile_name.IsNull())
        phyEventFile_R.close();
    if (!xtalkfile_name.IsNull())
        crossTalkCalc.close();
    if (global_mode == 4)
        rootapp->Run();
    
    return 0;
}
