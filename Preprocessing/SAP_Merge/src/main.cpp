#include <iostream>
#include <iomanip>
#include <cmath>
#include <cstdio>
#include <stdint.h>
#include "Constants.hpp"
#include "OptionsManager.hpp"
#include "SCIIterator.hpp"
#include "AUXIterator.hpp"
#include "PPDIterator.hpp"
#include "SAPDataFile.hpp"

#define MAX_OFFSET 60
#define MAX_GAP 60

using namespace std;

double calc_ship_second(const uint64_t raw_ship_time) {
    double second = static_cast<double>(raw_ship_time >> 16);
    double millisecond = static_cast<double>(raw_ship_time & 0xFFFF) / 2000;
    return second + millisecond;
}

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

    // open files
    cout << "Opening files ..." << endl;
    SCIIterator sciIter;
    if (!sciIter.open(options_mgr.scifile.Data())) {
        cout << "SCI root file open failed: " << options_mgr.scifile.Data() << endl;
        return 1;
    }
    cout << " - " << options_mgr.scifile.Data() << " ["
         << static_cast<int>(sciIter.get_first_ship_second())
         << " => "
         << static_cast<int>(sciIter.get_last_ship_second())
         << "]" << endl;
    if (!sciIter.get_is_1p()) {
        cout << "WARNING: the opened SCI data file is not 1P level, use frame ship time instead." << endl;
    }

    AUXIterator auxIter;
    if (options_mgr.auxfile.IsNull()) {
        cout << "WARNING: no AUX data file input." << endl;
    } else {
        if (!auxIter.open(options_mgr.auxfile.Data())) {
            cout << "AUX root file open failed: " << options_mgr.auxfile.Data() << endl;
            return 1;
        }
        if (auxIter.get_first_ship_second() - sciIter.get_first_ship_second() > MAX_OFFSET
                || sciIter.get_last_ship_second() - auxIter.get_last_ship_second() > MAX_OFFSET) {
            cout << "AUX does not match to SCI by ship time." << endl;
            return 1;
        }
        cout << " - " << options_mgr.auxfile.Data() << " ["
             << static_cast<int>(auxIter.get_first_ship_second())
             << " => "
             << static_cast<int>(auxIter.get_last_ship_second())
             << "]" << endl;
    }

    PPDIterator ppdIter;
    if (options_mgr.ppdfile.IsNull()) {
        cout << "WARNING: no PPD data file input." << endl;
    } else {
        if (!ppdIter.open(options_mgr.ppdfile.Data())) {
            cout << "PPD root file open failed: " << options_mgr.ppdfile.Data() << endl;
            return 1;
        }
        if (ppdIter.get_first_ship_second() - sciIter.get_first_ship_second() > MAX_OFFSET
                || sciIter.get_last_ship_second() - ppdIter.get_last_ship_second() > MAX_OFFSET) {
            cout << "PPD does not match to SCI by ship time." << endl;
            return 1;
        }
        cout << " - " << options_mgr.ppdfile.Data() << " ["
             << static_cast<int>(ppdIter.get_first_ship_second())
             << " => "
             << static_cast<int>(ppdIter.get_last_ship_second())
             << "]" << endl;
    }

    SAPDataFile sapFile;
    if (!sapFile.open(options_mgr.outfile.Data())) {
        cout << "output root file open failed: " << options_mgr.outfile.Data() << endl;
        return 1;
    }

    // merge data
    bool     sap_start_flag  = true;
    int      sap_first_time  = 0;
    int      sap_last_time   = 0;
    long int sap_last_entry  = -1;
    int      pre_percent     = 0;
    int      cur_percent     = 0;
    double   cur_ship_second = 0;
    double   total_seconds   = sciIter.get_last_ship_second() - sciIter.get_first_ship_second();
    double   start_seconds   = sciIter.get_first_ship_second();
    long int aux_gap_count   = 0;
    long int ppd_gap_count   = 0;
    if (options_mgr.auxfile.IsNull() && options_mgr.ppdfile.IsNull()) {
        cout << "Merging SCI data ..." << endl;
    } else if (options_mgr.auxfile.IsNull()) {
        cout << "Merging SCI and PPD data ..." << endl;
    } else if (options_mgr.ppdfile.IsNull()) {
        cout << "Merging SCI and AUX data ..." << endl;
    } else {
        cout << "Merging SCI, AUX and PPD data ..." << endl;
    }
    cout << "[ " << flush;
    while (sciIter.next_event()) {
        if (sciIter.get_is_1p()) {
            cur_ship_second = sciIter.cur_trigger.abs_ship_second;
        } else {
            cur_ship_second = calc_ship_second(sciIter.cur_trigger.frm_ship_time);
        }
        cur_percent = static_cast<int>(100 * (cur_ship_second - start_seconds) / total_seconds);
        if (cur_percent - pre_percent > 0 && cur_percent % 2 == 0) {
            pre_percent = cur_percent;
            cout << "#" << flush;
        }
        while (!options_mgr.auxfile.IsNull() && !auxIter.get_reach_end() && cur_ship_second > auxIter.hk_obox_after.abs_ship_second) {
            auxIter.next_obox();
        }
        while (!options_mgr.ppdfile.IsNull() && !ppdIter.get_reach_end() && cur_ship_second > ppdIter.ppd_after.ship_time_sec) {
            ppdIter.next_ppd();
        }
        // merge trigger and modules packets
        sapFile.clear_data();
        sapFile.t_pol_event.event_id = sciIter.cur_trigger.trigg_num_g;
        if (sciIter.get_is_1p()) {
            sapFile.t_pol_event.event_time = (sciIter.cur_trigger.abs_gps_week - METStartGPSWeek) * 604800
                + (sciIter.cur_trigger.abs_gps_second - METStartGPSSecond);
        } else {
            sapFile.t_pol_event.event_time = cur_ship_second;
        }
        sapFile.t_pol_event.type = sciIter.cur_trigger.type;
        sapFile.t_pol_event.is_ped = (sciIter.cur_trigger.type == 0x00F0);
        sapFile.t_pol_event.packet_num = sciIter.cur_trigger.packet_num;
        sapFile.t_pol_event.ct_pre_is_bad = sciIter.cur_trigger.pre_is_bad;
        sapFile.t_pol_event.ct_time_second = sciIter.cur_trigger.time_second;
        sapFile.t_pol_event.ct_time_wait = sciIter.cur_trigger.time_wait;
        sapFile.t_pol_event.ct_dead_ratio = sciIter.cur_trigger.dead_ratio;
        sapFile.t_pol_event.pkt_count = sciIter.cur_trigger.pkt_count;
        sapFile.t_pol_event.lost_count = sciIter.cur_trigger.lost_count;
        sapFile.t_pol_event.trigger_n = sciIter.cur_trigger.trigger_n;
        for (int i = 0; i < 25; i++) {
            sapFile.t_pol_event.trig_accepted[i] = sciIter.cur_trigger.trig_accepted[i];
        }
        while (sciIter.next_packet()) {
            if (sciIter.cur_modules.trigg_num != sciIter.cur_trigger.trigg_num)
                cout << "WARNING: trigger and modules packets are not matched. " << endl;
            int idx = sciIter.cur_modules.ct_num - 1;
            sapFile.t_pol_event.time_aligned[idx] = true;
            sapFile.t_pol_event.fe_pre_is_bad[idx] = sciIter.cur_modules.pre_is_bad;
            sapFile.t_pol_event.fe_time_second[idx] = sciIter.cur_modules.time_second;
            sapFile.t_pol_event.fe_time_wait[idx] = sciIter.cur_modules.time_wait;
            sapFile.t_pol_event.fe_dead_ratio[idx] = sciIter.cur_modules.dead_ratio;
            sapFile.t_pol_event.raw_rate[idx] = sciIter.cur_modules.raw_rate;
            sapFile.t_pol_event.multiplicity[idx] = sciIter.cur_modules.multiplicity;
            sapFile.t_pol_event.compress[idx] = sciIter.cur_modules.compress;
            sapFile.t_pol_event.common_noise[idx] = (sciIter.cur_modules.compress == 3 ? sciIter.cur_modules.common_noise : -1);
            sapFile.t_pol_event.dy12_too_high[idx] = sciIter.cur_modules.status_bit.dy12_too_high;
            sapFile.t_pol_event.t_out_too_many[idx] = sciIter.cur_modules.status_bit.t_out_too_many;
            sapFile.t_pol_event.t_out_2[idx] = sciIter.cur_modules.status_bit.t_out_2;
            sapFile.t_pol_event.t_out_1[idx] = sciIter.cur_modules.status_bit.t_out_1;
            for (int j = 0; j < 64; j++) {
                sapFile.t_pol_event.trigger_bit[idx][j] = sciIter.cur_modules.trigger_bit[j];
                if (sciIter.cur_modules.energy_adc[j] > 4095) {
                    sapFile.t_pol_event.channel_status[idx][j] = 1;
                    sapFile.t_pol_event.energy_value[idx][j] = sapFile.t_pol_event.common_noise[idx];
                } else {
                    sapFile.t_pol_event.channel_status[idx][j] = 0;
                    sapFile.t_pol_event.energy_value[idx][j] = sciIter.cur_modules.energy_adc[j];
                }
            }
        }
        // merge AUX
        if (!options_mgr.auxfile.IsNull()) {
            sapFile.t_pol_event.aux_interval = auxIter.hk_obox_after.abs_ship_second - auxIter.hk_obox_before.abs_ship_second;
            if (fabs(cur_ship_second - auxIter.hk_obox_before.abs_ship_second)
                    < fabs(cur_ship_second - auxIter.hk_obox_after.abs_ship_second)) {
                sapFile.t_pol_event.obox_mode = auxIter.hk_obox_before.obox_mode;
                for (int i = 0; i < 25; i++) {
                    sapFile.t_pol_event.fe_hv[i] = auxIter.hk_obox_before.fe_hv[i];
                    sapFile.t_pol_event.fe_temp[i] = auxIter.hk_obox_before.fe_temp[i];
                }
            } else {
                sapFile.t_pol_event.obox_mode = auxIter.hk_obox_after.obox_mode;
                for (int i = 0; i < 25; i++) {
                    sapFile.t_pol_event.fe_hv[i] = auxIter.hk_obox_after.fe_hv[i];
                    sapFile.t_pol_event.fe_temp[i] = auxIter.hk_obox_after.fe_temp[i];
                }
            }
            if (fabs(cur_ship_second - auxIter.fe_thr_ship_second_current)
                    < fabs(cur_ship_second - auxIter.fe_thr_ship_second_next)) {
                for (int i = 0; i < 25; i++) {
                    sapFile.t_pol_event.fe_thr[i] = auxIter.fe_thr_current[i];
                }
            } else {
                for (int i = 0; i < 25; i++) {
                    sapFile.t_pol_event.fe_thr[i] = auxIter.fe_thr_next[i];
                }
            }
        }
        // merge PPD
        if (!options_mgr.ppdfile.IsNull()) {
            sapFile.t_pol_event.ppd_interval = ppdIter.ppd_after.ship_time_sec - ppdIter.ppd_before.ship_time_sec;
            ppdIter.calc_ppd_interm(cur_ship_second);
            sapFile.t_pol_event.wgs84_xyz[0] = ppdIter.ppd_interm.wgs84_x;
            sapFile.t_pol_event.wgs84_xyz[1] = ppdIter.ppd_interm.wgs84_y;
            sapFile.t_pol_event.wgs84_xyz[2] = ppdIter.ppd_interm.wgs84_z;
            sapFile.t_pol_event.det_z_radec[0] = ppdIter.ppd_interm.det_z_ra;
            sapFile.t_pol_event.det_z_radec[1] = ppdIter.ppd_interm.det_z_dec;
            sapFile.t_pol_event.det_x_radec[0] = ppdIter.ppd_interm.det_x_ra;
            sapFile.t_pol_event.det_x_radec[1] = ppdIter.ppd_interm.det_x_dec;
            sapFile.t_pol_event.earth_radec[0] = ppdIter.ppd_interm.earth_ra;
            sapFile.t_pol_event.earth_radec[1] = ppdIter.ppd_interm.earth_dec;
            sapFile.t_pol_event.sun_radec[0] = ppdIter.ppd_interm.sun_ra;
            sapFile.t_pol_event.sun_radec[1] = ppdIter.ppd_interm.sun_dec;
        }
        sapFile.fill_data();
        sap_last_entry++;
        if (sapFile.t_pol_event.aux_interval > MAX_GAP) {
            aux_gap_count++;
        }
        if (sapFile.t_pol_event.ppd_interval > MAX_GAP) {
            ppd_gap_count++;
        }
        if (sap_start_flag) {
            sap_start_flag = false;
            sap_first_time = sapFile.t_pol_event.event_time;
        }
        sap_last_time = sapFile.t_pol_event.event_time;
    }
    cout << " DONE ]" << endl;

    char time_span_str[100];
    sprintf(time_span_str, "%d[0] => %d[%ld]",
            sap_first_time, sap_last_time, sap_last_entry);

    // write root
    sapFile.write_tree();

    // write meta
    // m_dattype
    sapFile.write_meta("m_dattype", "POLAR EVENT DATA AFTER MERGED");
    // m_version
    sapFile.write_meta("m_version", (SW_NAME + " " + SW_VERSION).c_str());
    // m_gentime
    TTimeStamp* cur_time = new TTimeStamp();
    sapFile.write_meta("m_gentime", cur_time->AsString("lc"));
    delete cur_time;
    cur_time = NULL;
    // m_rawfile
    sapFile.write_meta("m_rawfile", sciIter.get_m_rawfile_str().c_str());
    // m_dcdinfo
    sapFile.write_meta("m_dcdinfo", sciIter.get_m_dcdinfo_str().c_str());
    // m_badinfo
    sapFile.write_meta("m_badinfo", sciIter.get_bad_ratio_str().c_str());
    // m_gapinfo
    char gapinfo_buffer[100];
    sprintf(gapinfo_buffer, "aux_gap: %ld/%ld, ppd_gap: %ld/%ld",
            aux_gap_count, sap_last_entry + 1, ppd_gap_count, sap_last_entry + 1);
    sapFile.write_meta("m_gapinfo", gapinfo_buffer);
    // m_merging
    string merging_str;
    if (sciIter.get_is_1p()) {
        merging_str = "SCI_1P";
    } else {
        merging_str = "SCI_1M";
    }
    if (!options_mgr.auxfile.IsNull()) {
        merging_str += ", AUX_1M";
    }
    if (!options_mgr.ppdfile.IsNull()) {
        merging_str += ", PPD_1N";
    }
    sapFile.write_meta("m_merging", merging_str.c_str());
    // m_eneunit
    sapFile.write_meta("m_energy_unit", "ADC");
    // m_levelnum
    sapFile.write_meta("m_level_num", "0");
    // m_timespan
    sapFile.write_meta("m_time_span", time_span_str);
    // m_utc_ref
    sapFile.write_meta("m_UTC_REF", MET_UTC_REF_str.c_str());
    // m_tt_ref
    sapFile.write_meta("m_TT_REF", MET_TT_REF_str.c_str());
    // m_mjd_ref
    sapFile.write_meta("m_MJD_REF", MET_MJD_REF_str.c_str());

    // close files
    sapFile.close();
    sciIter.close();
    auxIter.close();
    ppdIter.close();

    cout << "================================================================================" << endl;
    cout << "BAD_RATIO_INF: { " << sciIter.get_bad_ratio_str().c_str() << " } " << endl;
    cout << "GAP_RATIO_INF: { " << gapinfo_buffer << " } " << endl;
    cout << "MET_TIME_SPAN: { " << time_span_str << " } " << endl;
    cout << "================================================================================" << endl;

    return 0;
}
