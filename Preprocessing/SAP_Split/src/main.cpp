#include <iostream>
#include <cstdio>
#include "OptionsManager.hpp"
#include "SAPFileR.hpp"
#include "SAPFileW.hpp"

// must lest than 343
#define TIME_GAP_MAX 300

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

    // open 1q files to read
    size_t    sapfile_r_len = options_mgr.raw_file_vector.size();
    SAPFileR* sapfile_r_arr = new SAPFileR[sapfile_r_len];
    if (sapfile_r_len == 1) {
        cout << "Opening file: " << options_mgr.raw_file_vector[0].Data() << " ... " << endl;
        if (sapfile_r_arr[0].open(options_mgr.raw_file_vector[0].Data(),
                                  options_mgr.met_begin.Data(),
                                  options_mgr.met_end.Data())) {
            sapfile_r_arr[0].print_file_info();
        } else {
            cout << "Error: file open failed." << endl;
            sapfile_r_arr[0].close();
            return 1;
        }
    } else {
        bool open_result;
        for (size_t i = 0; i < sapfile_r_len; i++) {
            cout << "Opening file: " << options_mgr.raw_file_vector[i].Data() << " ... " << endl;
            if (i == 0) {
                open_result = sapfile_r_arr[i].open(options_mgr.raw_file_vector[i].Data(),
                                                    options_mgr.met_begin.Data(),
                                                    "end");
            } else if (i == sapfile_r_len - 1) {
                open_result = sapfile_r_arr[i].open(options_mgr.raw_file_vector[i].Data(),
                                                    "begin",
                                                    options_mgr.met_end.Data());
            } else {
                open_result = sapfile_r_arr[i].open(options_mgr.raw_file_vector[i].Data(),
                                                    "begin",
                                                    "end");
            }
            if (open_result) {
                sapfile_r_arr[i].print_file_info();
                // check connection
                if (i > 0) {
                    if (sapfile_r_arr[i].get_time_first() - sapfile_r_arr[i - 1].get_time_last() > TIME_GAP_MAX) {
                        cout << "Error: two files cannot connect in MET time." << endl;
                        for (size_t j = 0; j <= i; j++) {
                            sapfile_r_arr[j].close();
                        }
                        return false;
                    } else if (sapfile_r_arr[i].get_time_first() - sapfile_r_arr[i - 1].get_time_last() < -1) {
                        cout << "Error: two files have overlap in MET time." << endl;
                        for (size_t j = 0; j <= i; j++) {
                            sapfile_r_arr[j].close();
                        }
                        return false;
                    }
                }
            } else {
                cout << "Error: file open failed." << endl;
                for (size_t j = 0; j <= i; j++) {
                    sapfile_r_arr[j].close();
                }
                return false;
            }
        }
    }

    // write 1q data file one by one
    SAPFileW sapfile_w;
    if (!sapfile_w.open(options_mgr.out_file.Data())) {
        cerr << "output file open failed: " << options_mgr.out_file.Data() << endl;
        return 1;
    }
    for (size_t i = 0; i < sapfile_r_len; i++) {
        cout << "-----------------------------------------------------------------------------" << endl;
        cout << "Processing file: " << options_mgr.raw_file_vector[i] << " ... " << endl;
        int pre_percent = 0;
        int cur_percent = 0;
        cout << "Copying data from " << sapfile_r_arr[i].get_filename() << " ..." << endl;
        cout << "[ " << flush;
        sapfile_r_arr[i].pol_event_set_start();
        while (sapfile_r_arr[i].pol_event_next()) {
            cur_percent = static_cast<int>(100 * sapfile_r_arr[i].pol_event_get_cur_entry() / sapfile_r_arr[i].pol_event_get_tot_entries());
            if (cur_percent - pre_percent > 0 && cur_percent % 2 == 0) {
                pre_percent = cur_percent;
                cout << "#" << flush;
            }
            sapfile_w.t_pol_event = sapfile_r_arr[i].t_pol_event;
            sapfile_w.fill_entry();
        }
        sapfile_r_arr[i].close();
        cout << " DONE ]" << endl;
    }
    sapfile_w.write_tree();

    // write meta information
    // m_dattype
    sapfile_w.write_meta("m_dattype", "POLAR EVENT DATA AFTER MERGED AND SPLITTED");
    // m_version
    sapfile_w.write_meta("m_version", (SW_NAME + " " + SW_VERSION).c_str());
    // m_gentime
    TTimeStamp* cur_time = new TTimeStamp();
    sapfile_w.write_meta("m_gentime", cur_time->AsString("lc"));
    delete cur_time;
    cur_time = NULL;
    // m_fromfile
    TSystem sys;
    string fromfile_list;
    fromfile_list += sys.BaseName(sapfile_r_arr[0].get_filename().c_str());
    fromfile_list += Form("[%ld=>%ld]",
            static_cast<long int>(sapfile_r_arr[0].pol_event_get_entry_begin()),
            static_cast<long int>(sapfile_r_arr[0].pol_event_get_entry_end()));
    for (size_t i = 1; i < sapfile_r_len; i++) {
        fromfile_list += "; ";
        fromfile_list += sys.BaseName(sapfile_r_arr[i].get_filename().c_str());
        fromfile_list += Form("[%ld=>%ld]",
                static_cast<long int>(sapfile_r_arr[i].pol_event_get_entry_begin()),
                static_cast<long int>(sapfile_r_arr[i].pol_event_get_entry_end()));

    }
    sapfile_w.write_meta("m_fromfile", fromfile_list.c_str());
    // m_eneunit
    sapfile_w.write_meta("m_energy_unit", "ADC");
    // m_levelnum
    sapfile_w.write_meta("m_level_num", "0");
    // m_timespan
    sapfile_w.write_meta("m_time_span", sapfile_w.get_time_span().c_str());
    // m_utc_ref
    sapfile_w.write_meta("m_UTC_REF", MET_UTC_REF_str.c_str());
    // m_tt_ref
    sapfile_w.write_meta("m_TT_REF", MET_TT_REF_str.c_str());
    // m_mjd_ref
    sapfile_w.write_meta("m_MJD_REF", MET_MJD_REF_str.c_str());

    cout << "================================================================================" << endl;
    cout << "MET TIME SPAN: { " << sapfile_w.get_time_span() << " }" << endl;
    cout << "================================================================================" << endl;

    sapfile_w.close();

    return 0;
}
