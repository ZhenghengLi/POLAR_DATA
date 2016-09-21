#include <iostream>
#include <iomanip>
#include <cstring>
#include "OptionsManager.hpp"
#include "RootInc.hpp"
#include "HkIterator.hpp"

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

    HkIterator hkIter;
    if (!hkIter.open(options_mgr.decoded_data_filename.Data(), options_mgr.begin_gps.Data(), options_mgr.end_gps.Data())) {
        cout << "root file open failed: " << options_mgr.decoded_data_filename.Data() << endl;
        return 1;
    }

    hkIter.print_file_info();
    int    begin_gps_week   = hkIter.begin_hk_obox.abs_gps_week;
    double begin_gps_second = hkIter.begin_hk_obox.abs_gps_second;
    int    end_gps_week     = hkIter.end_hk_obox.abs_gps_week;
    double end_gps_second   = hkIter.end_hk_obox.abs_gps_second;

    char name[50];
    char title[100];
    sprintf(title, "%d:%d => %d:%d",
            static_cast<int>(begin_gps_week),
            static_cast<int>(begin_gps_second),
            static_cast<int>(end_gps_week),
            static_cast<int>(end_gps_second));
    string gps_time_span = title;

    TGraph* temp_graph[25];
    if (options_mgr.ct_num_vec.size() > 0) {
        for (size_t i = 0; i < options_mgr.ct_num_vec.size(); i++) {
            int idx = options_mgr.ct_num_vec[i] - 1;
            sprintf(name, "temp_graph_%d", idx + 1);
            sprintf(title, "temperature { %s }", gps_time_span.c_str());
            temp_graph[idx] = new TGraph(name, title);
            temp_graph[idx]->SetLineColor(ColorIndex[idx]);
        }
    } else {
        for (int i = 0; i < 25; i++) {
            sprintf(name, "temp_graph_%d", i + 1);
            sprintf(title, "temperature { %s } CT_%d", gps_time_span.c_str(), i + 1);
            temp_graph[i] = new TGraph(name, title);
            temp_graph[i]->SetLineColor(ColorIndex[i]);
        }
    }

    int point_num = -1;
    int pre_percent = 0;
    int cur_percent = 0;
    cout << "reading temperature data ... " << endl;
    cout << "[ " << flush;
    hkIter.hk_obox_set_start();
    while (hkIter.hk_obox_next()) {
        cur_percent = static_cast<int>(100 * hkIter.hk_obox_get_cur_entry() / hkIter.hk_obox_get_tot_entries());
        if (cur_percent - pre_percent > 0 && cur_percent % 2 == 0) {
            pre_percent = cur_percent;
            cout << "#" << flush;
        }
        if (hkIter.t_hk_obox.obox_is_bad > 0) {
            continue;
        }
        point_num += 1;
        if (options_mgr.ct_num_vec.size() > 0) {
            for (size_t i = 0; i < options_mgr.ct_num_vec.size(); i++) {
                int idx = options_mgr.ct_num_vec[i] - 1;
                temp_graph[idx]->SetPoint(point_num,
                                          (hkIter.t_hk_obox.abs_gps_week - begin_gps_week) * 604800 + (hkIter.t_hk_obox.abs_gps_second - begin_gps_second),
                                          hkIter.t_hk_obox.fe_temp[idx]);
            }
        } else {
            for (int i = 0; i < 25; i++) {
                temp_graph[i]->SetPoint(point_num,
                                        (hkIter.t_hk_obox.abs_gps_week - begin_gps_week) * 604800 + (hkIter.t_hk_obox.abs_gps_second - begin_gps_second),
                                        hkIter.t_hk_obox.fe_temp[i]);
            }
        }
    }
    
    return 0;
}
