#include <iostream>
#include <iomanip>
#include "OptionsManager.hpp"
#include "RootInc.hpp"
#include "EventIterator.hpp"

#define MAX_LEN 1000

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

    if (options_mgr.binwidth_vec.size() > MAX_LEN) {
        cout << "ERROR: number of bin width should be smaller than " << MAX_LEN << "." << endl;
        return 1;
    }

//    TApplication* rootapp = new TApplication("POLAR", NULL, NULL);
    gROOT->SetBatch(kTRUE);
    gErrorIgnoreLevel = kWarning;

    EventIterator eventIter;
    if (!eventIter.open(options_mgr.decoded_data_filename.Data(),
                      options_mgr.begin_gps.Data(), options_mgr.end_gps.Data())) {
        cerr << "root file open failed: " << options_mgr.decoded_data_filename.Data() << endl;
        return 1;
    }

    if (!eventIter.file_is_1P()) {
        cout << "error: The opened file may be not 1P/1R SCI data file." << endl;
        eventIter.close();
        return 1;
    }
    
    eventIter.print_file_info();
    int    begin_gps_week   = eventIter.phy_begin_trigger.abs_gps_week;
    double begin_gps_second = eventIter.phy_begin_trigger.abs_gps_second;
    int    end_gps_week     = eventIter.phy_end_trigger.abs_gps_week;
    double end_gps_second   = eventIter.phy_end_trigger.abs_gps_second;
    Double_t gps_time_length = (end_gps_week - begin_gps_week) * 604800 + (end_gps_second - begin_gps_second);

    TH1D*    hist_array[MAX_LEN][4];
    // prepare histogram
    char name[50];
    char title[50];
    for (size_t i = 0; i < options_mgr.binwidth_vec.size(); i++) {
        int cur_nbins = static_cast<int>(gps_time_length / options_mgr.binwidth_vec[i]);
        for (int j = 0; j < 4; j++) {
            sprintf(name,   "hist_%d_%d", static_cast<int>(i), static_cast<int>(j));
            sprintf(title, "%d:%d => %d:%d @ %.3f, %d/4",
                    static_cast<int>(begin_gps_week),
                    static_cast<int>(begin_gps_second),
                    static_cast<int>(end_gps_week),
                    static_cast<int>(end_gps_second),
                    static_cast<float>(options_mgr.binwidth_vec[i]),
                    static_cast<int>(j));
            hist_array[i][j] = new TH1D(name, title, cur_nbins, 0 + j * options_mgr.binwidth_vec[i] / 4, gps_time_length + j * options_mgr.binwidth_vec[i] / 4);
//            hist_array[i][j]->SetLineColor(kRed);
            hist_array[i][j]->SetDirectory(NULL);
        }
    }

    // fill histogram
    int pre_percent = 0;
    int cur_percent = 0;
    cout << "reading trigger data ... " << endl;
    cout << "[ " << flush;
    eventIter.phy_trigger_set_start();
    while (eventIter.phy_trigger_next_event()) {
        cur_percent = static_cast<int>(100 * eventIter.phy_trigger_get_cur_entry() / eventIter.phy_trigger_get_tot_entries());
        if (cur_percent - pre_percent > 0 && cur_percent % 2 == 0) {
            pre_percent = cur_percent;
            cout << "#" << flush;
        }   
        if (!eventIter.t_trigger.abs_gps_valid) {
            continue;
        }   
        for (size_t i = 0; i < options_mgr.binwidth_vec.size(); i++) {
            for (int j = 0; j < 4; j++) {
                hist_array[i][j]->Fill((eventIter.t_trigger.abs_gps_week   - begin_gps_week) * 604800 +
                                       (eventIter.t_trigger.abs_gps_second - begin_gps_second));
            }   
        }   
    }   
    cout << " DONE ]" << endl;
    eventIter.close();

    // draw
    cout << "print rate hist to file: " << TSystem().BaseName(options_mgr.pdf_filename.Data()) << " ... " << endl;
    cout << endl;
    gStyle->SetOptStat(0);
    TCanvas* canvas_array[MAX_LEN];    
    for (size_t i = 0; i < options_mgr.binwidth_vec.size(); i++) {
        cout << " - page " << left << setw(3) << i + 1 << " - bin width: " << options_mgr.binwidth_vec[i] << endl;
        double y_max = 0;
        for (int j = 0; j < 4; j++) {
            for (int k = 1; k <= hist_array[i][j]->GetNbinsX(); k++) {
                hist_array[i][j]->SetBinContent(k, hist_array[i][j]->GetBinContent(k) / options_mgr.binwidth_vec[i]);
                hist_array[i][j]->SetBinError(k  , hist_array[i][j]->GetBinError(k)   / options_mgr.binwidth_vec[i]);
            }
            if (hist_array[i][j]->GetMaximum() > y_max) {
                y_max = hist_array[i][j]->GetMaximum();
            }
        }
        for (int j = 0; j < 4; j++) {
            hist_array[i][j]->SetMaximum(y_max * 1.1);
        }
        sprintf(name,  "canvas_%d", static_cast<int>(i));
        sprintf(title, "canvas_%d", static_cast<int>(i));
        canvas_array[i] = new TCanvas(name, title, 800, 1000);
        canvas_array[i]->Divide(1, 4);
        for (int j = 0; j < 4; j++) {
            canvas_array[i]->cd(j + 1);
            hist_array[i][j]->Draw("EH");
        }
        if (i == 0 && i == options_mgr.binwidth_vec.size() - 1) {
            canvas_array[i]->Print(options_mgr.pdf_filename, "pdf");
        } else if (i == 0) {
            canvas_array[i]->Print(options_mgr.pdf_filename + "(", "pdf");
        } else if (i == options_mgr.binwidth_vec.size() - 1) {
            canvas_array[i]->Print(options_mgr.pdf_filename + ")", "pdf");
        } else {
            canvas_array[i]->Print(options_mgr.pdf_filename, "pdf");
        }
    }
    cout << endl;

 //   rootapp->Run();
    
    return 0;
}
