#include <iostream>
#include <vector>
#include "OptionsManager.hpp"
#include "RootInc.hpp"
#include "EventIterator.hpp"
#include "Math/ProbFunc.h"

using namespace std;
using namespace ROOT::Math;

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

    // prepare bin width list
    double logstep = (TMath::Log10(options_mgr.bw_stop) - TMath::Log10(options_mgr.bw_start)) / options_mgr.bw_len;
    vector<double> vec_bwlist;
    for (int i = 0; i <= options_mgr.bw_len; i++) {
        double cur_bwlog = TMath::Log10(options_mgr.bw_start) + i * logstep;
        vec_bwlist.push_back(TMath::Power(10, cur_bwlog));
    }
    
    // open data file
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

    // prepare histogram
    vector<int> vec_nblist(vec_bwlist.size());
    char name[50];
    char title[100];
    TH1F* hist_array[vec_bwlist.size()][4];
    for (size_t i = 0; i < vec_bwlist.size(); i++) {
        int cur_nbins = static_cast<int>(gps_time_length / vec_bwlist[i]);
        vec_nblist[i] = cur_nbins;
        for (size_t j = 0; j < 4; j++) {
            sprintf(name,   "hist_%d_%d", static_cast<int>(i), static_cast<int>(j));
            sprintf(title, "title_%d_%d", static_cast<int>(i), static_cast<int>(j));
            hist_array[i][j] = new TH1F(name, title, cur_nbins, 0 + j * vec_bwlist[i] / 4, gps_time_length + j * vec_bwlist[i] / 4);
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
        for (size_t i = 0; i < vec_bwlist.size(); i++) {
            for (size_t j = 0; j < 4; j++) {
                hist_array[i][j]->Fill((eventIter.t_trigger.abs_gps_week   - begin_gps_week) * 604800 +
                                       (eventIter.t_trigger.abs_gps_second - begin_gps_second));
            }
        }
    }
    cout << " DONE ]" << endl;
    eventIter.close();

    cout << poisson_cdf(10, 5) << endl;
    cout << poisson_cdf_c(10, 5) << endl;

    // grb searching
    vector<int>   exceeding_bins[vec_bwlist.size()][4];
    vector<float> exceeding_prob[vec_bwlist.size()][4];
    for (size_t i = 0; i < vec_bwlist.size(); i++) {
        for (size_t j = 0; j < 4; j++) {
            cout << i << " => " << j << endl;
            for (int k = 1; k <= vec_nblist[i]; k++) {
                cout << hist_array[i][j]->GetBinContent(k) << endl;
            }
        }
    }

    
    return 0;
}
