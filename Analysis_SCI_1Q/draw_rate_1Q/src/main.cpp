#include <iostream>
#include <fstream>
#include <sstream>
#include "OptionsManager.hpp"
#include "RootInc.hpp"
#include "POLEvent.hpp"
#include "RateCanvas.hpp"
#include "CooConv.hpp"
#include "PosConv.hpp"

using namespace std;

int main(int argc, char** argv) {
    OptionsManager options_mgr;
    if (!options_mgr.parse(argc, argv)) {
        if (options_mgr.get_version_flag()) {
            options_mgr.print_version();
        } else {
            options_mgr.print_help();
        }
        return 2;
    }

    bool bar_mask[25][64];
    for (int i = 0; i < 25; i++) {
        for (int j = 0; j < 64; j++) {
            bar_mask[i][j] = false;
        }
    }
    bool type_mask[3];
    for (int i = 0; i < 3; i++) {
        type_mask[i] = false;
    }

    // read bar_mask
    char line_buffer[100];
    ifstream infile;
    if (!options_mgr.bar_mask_filename.IsNull()) {
        infile.open(options_mgr.bar_mask_filename.Data());
        if (!infile.is_open()) {
            cout << "bar_mask_file open failed." << endl;
            return 1;
        }
    }
    stringstream ss;
    int ct_num;
    int ch_idx;
    while (!options_mgr.bar_mask_filename.IsNull()) {
        infile.getline(line_buffer, 100);
        if (infile.eof()) break;
        if (string(line_buffer).find("#") != string::npos) {
            continue;
        } else if (string(line_buffer).find("single") != string::npos) {
            type_mask[0] = true;
            cout << "kill all single event" << endl;
        } else if (string(line_buffer).find("normal") != string::npos) {
            type_mask[1] = true;
            cout << "kill all normal event" << endl;
        } else if (string(line_buffer).find("cosmic") != string::npos) {
            type_mask[2] = true;
            cout << "kill all cosmic event" << endl;
        } else {
            ss.clear();
            ss.str(line_buffer);
            ss >> ct_num >> ch_idx;
            if (ct_num < 1 || ct_num > 25) {
                cout << "ct_num out of range" << endl;
                return 1;
            }
            if (ch_idx < 0 || ch_idx > 63) {
                cout << "ch_idx out of range" << endl;
                return 1;
            }
            cout << "kill bar ct_" << ct_num << ", ch_" << ch_idx << endl;
            bar_mask[ct_num - 1][ch_idx] = true;
        }
    }

    TApplication* rootapp = new TApplication("POLAR", NULL, NULL);

    // open pol_event_file
    TFile* pol_event_file = new TFile(options_mgr.pol_event_filename.Data(), "read");
    if (pol_event_file->IsZombie()) {
        cout << "pol_event_file open failed: " << options_mgr.pol_event_filename.Data() << endl;
        return 1;
    }
    TNamed* m_level_num = static_cast<TNamed*>(pol_event_file->Get("m_level_num"));
    if (m_level_num == NULL) {
        cout << "cannot find TNamed m_level_num." << endl;
        return 1;
    }
    // if (TString(m_level_num->GetTitle()).Atoi() != 1) {
    //     cout << "m_level_num is not 1." << endl;
    //     return 1;
    // }
    TTree* t_pol_event_tree = static_cast<TTree*>(pol_event_file->Get("t_pol_event"));
    if (t_pol_event_tree == NULL) {
        cout << "cannot find TTree t_pol_event." << endl;
        return 1;
    }
    POLEvent t_pol_event;
    t_pol_event.bind_pol_event_tree(t_pol_event_tree);
    t_pol_event.deactive_all(t_pol_event_tree);
    Long64_t begin_entry = 0;
    if (options_mgr.begin_met_time > 0) {
        begin_entry = t_pol_event.find_entry(t_pol_event_tree, options_mgr.begin_met_time);
        if (begin_entry < 0) {
            cout << "cannot find begin_entry." << endl;
            return 1;
        }
    }
    Long64_t end_entry = t_pol_event_tree->GetEntries();
    if (options_mgr.end_met_time > 0) {
        end_entry = t_pol_event.find_entry(t_pol_event_tree, options_mgr.end_met_time);
        if (end_entry < 0) {
            cout << "cannot find end_entry." << endl;
            return 1;
        }
    }
    t_pol_event.active(t_pol_event_tree, "event_time");
    t_pol_event.active(t_pol_event_tree, "is_ped");
    t_pol_event.active(t_pol_event_tree, "trigger_bit");
    t_pol_event.active(t_pol_event_tree, "time_aligned");
    t_pol_event.active(t_pol_event_tree, "raw_rate");
    t_pol_event.active(t_pol_event_tree, "trigger_n");
    t_pol_event.active(t_pol_event_tree, "type");
    t_pol_event.active(t_pol_event_tree, "trig_accepted");
    t_pol_event.active(t_pol_event_tree, "ppd_interval");
    t_pol_event.active(t_pol_event_tree, "wgs84_xyz");
    t_pol_event.active(t_pol_event_tree, "det_z_radec");
    t_pol_event.active(t_pol_event_tree, "det_x_radec");
    t_pol_event.active(t_pol_event_tree, "earth_radec");
    t_pol_event.active(t_pol_event_tree, "sun_radec");
    t_pol_event_tree->GetEntry(begin_entry);
    int begin_time = static_cast<int>(t_pol_event.event_time);
    t_pol_event_tree->GetEntry(end_entry - 1);
    int end_time = static_cast<int>(t_pol_event.event_time);
    cout << options_mgr.pol_event_filename.Data()
         << " { " << begin_time << "[" << begin_entry << "] => "
         << end_time << "[" << end_entry - 1 << "] }" << endl;


    Double_t met_time_length = end_time - begin_time;
    int nbins = met_time_length / options_mgr.binwidth;
    cout << "----------------------------------------------------------" << endl;
    cout << " - time length:     " << met_time_length << " seconds" << endl;
    cout << " - bin width:       " << options_mgr.binwidth << " seconds" << endl;
    cout << " - number of bins:  " << nbins << endl;
    cout << " - phase shift:     " << options_mgr.phase << "/4" << endl;
    cout << "----------------------------------------------------------" << endl;

    RateCanvas rate_canvas(begin_time, options_mgr.min_signif);

    // open weight file
    TFile* pol_weight_file = NULL;
    TTree* t_pol_weight_tree = NULL;
    struct {
        Double_t event_time_2;
        Float_t  ch_weight[25][64];
        Float_t  mod_weight[25];
        Float_t  event_weight;
    } t_pol_weight;
    if (!options_mgr.weight_filename.IsNull()) {
        pol_weight_file = new TFile(options_mgr.weight_filename.Data(), "read");
        if (pol_weight_file->IsZombie()) {
            cout << "pol_weight_file open failed." << endl;
            return 1;
        }
        t_pol_weight_tree = static_cast<TTree*>(pol_weight_file->Get("t_pol_weight"));
        if (t_pol_weight_tree == NULL) {
            cout << "cannot find TTree t_pol_weight" << endl;
            return 1;
        }
        t_pol_weight_tree->SetBranchAddress("event_time_2",     &t_pol_weight.event_time_2     );
        t_pol_weight_tree->SetBranchAddress("ch_weight",         t_pol_weight.ch_weight        );
        t_pol_weight_tree->SetBranchAddress("mod_weight",        t_pol_weight.mod_weight       );
        t_pol_weight_tree->SetBranchAddress("event_weight",     &t_pol_weight.event_weight     );
        if (t_pol_weight_tree->GetEntries() != t_pol_event_tree->GetEntries()) {
            cout << "Entries is different between TTree t_pol_weight and t_pol_event" << endl;
            return 1;
        }
    }

    // prepare histogram
    char name[50];
    char title[100];
    sprintf(title, "%d => %d @ %.3f, %d/4",
            static_cast<int>(begin_time),
            static_cast<int>(end_time),
            static_cast<float>(options_mgr.binwidth),
            static_cast<int>(options_mgr.phase));
    string met_time_span = title;
    TH1D* trigger_hist = new TH1D("trigger_hist", (string("trigger: { ") + met_time_span + string(" }")).c_str(),
                                   nbins, 0 + options_mgr.phase * options_mgr.binwidth / 4, met_time_length + options_mgr.phase * options_mgr.binwidth / 4);
    trigger_hist->SetDirectory(NULL);
    trigger_hist->SetMinimum(0);
    trigger_hist->GetXaxis()->SetTitle("T-T0 (s)");
    trigger_hist->GetYaxis()->SetTitle("Rate (trigger/s)");

    // theta and pha of earth
    TH1D* earth_theta_hist = new TH1D("earth_theta", "Theta of Earth",
            nbins, 0 + options_mgr.phase * options_mgr.binwidth / 4, met_time_length + options_mgr.phase * options_mgr.binwidth / 4);
    earth_theta_hist->SetDirectory(NULL);
    earth_theta_hist->SetMinimum(0);
    earth_theta_hist->SetMaximum(180);
    TH1D* earth_pha_hist = new TH1D("earth_pha", "Pha of Earth",
            nbins, 0 + options_mgr.phase * options_mgr.binwidth / 4, met_time_length + options_mgr.phase * options_mgr.binwidth / 4);
    earth_pha_hist->SetDirectory(NULL);
    earth_pha_hist->SetMinimum(0);
    earth_pha_hist->SetMaximum(360);
    // theta and pha of sun
    TH1D* sun_theta_hist = new TH1D("sun_theta", "Theta of Sun",
            nbins, 0 + options_mgr.phase * options_mgr.binwidth / 4, met_time_length + options_mgr.phase * options_mgr.binwidth / 4);
    sun_theta_hist->SetDirectory(NULL);
    sun_theta_hist->SetMinimum(0);
    sun_theta_hist->SetMaximum(180);
    TH1D* sun_pha_hist = new TH1D("sun_pha", "Pha of Sun",
            nbins, 0 + options_mgr.phase * options_mgr.binwidth / 4, met_time_length + options_mgr.phase * options_mgr.binwidth / 4);
    sun_pha_hist->SetDirectory(NULL);
    sun_pha_hist->SetMinimum(0);
    sun_pha_hist->SetMaximum(360);
    // theta and pha of GRB
    TH1D* grb_theta_hist = new TH1D("grb_theta", "Theta of Object",
            nbins, 0 + options_mgr.phase * options_mgr.binwidth / 4, met_time_length + options_mgr.phase * options_mgr.binwidth / 4);
    grb_theta_hist->SetDirectory(NULL);
    grb_theta_hist->SetMinimum(0);
    grb_theta_hist->SetMaximum(180);
    TH1D* grb_pha_hist = new TH1D("grb_pha", "Pha of Object",
            nbins, 0 + options_mgr.phase * options_mgr.binwidth / 4, met_time_length + options_mgr.phase * options_mgr.binwidth / 4);
    grb_pha_hist->SetDirectory(NULL);
    grb_pha_hist->SetMinimum(0);
    grb_pha_hist->SetMaximum(360);
    TH1D* count_hist = new TH1D("count_hist", "count_hist",
                                   nbins, 0 + options_mgr.phase * options_mgr.binwidth / 4, met_time_length + options_mgr.phase * options_mgr.binwidth / 4);
    count_hist->SetDirectory(NULL);
    // latlon
    TH2D* latlon_map_hist = new TH2D("latlon_map", "Latitude and Longitude", 360, -180, 180, 180, -90, 90);
    latlon_map_hist->SetDirectory(NULL);
    TH2D* lltime_map_hist = new TH2D("lltime_map", "LatLon Time", 360, -180, 180, 180, -90, 90);
    lltime_map_hist->SetDirectory(NULL);

    TH1D* modules_hist[25];
    TH1D* modules_hist_tout1[25];
    for (int i = 0; i < 25; i++) {
        sprintf(name, "module_hist_%02d", i + 1);
        sprintf(title, "module CT_%02d: { %s }", i + 1, met_time_span.c_str());
        modules_hist[i] = new TH1D(name, title,
                                   nbins, 0 + options_mgr.phase * options_mgr.binwidth / 4, met_time_length + options_mgr.phase * options_mgr.binwidth / 4);
        modules_hist[i]->SetDirectory(NULL);
        modules_hist[i]->SetLineColor(kGreen);
        modules_hist[i]->SetMinimum(0);
        modules_hist[i]->GetXaxis()->SetTitle("T-T0 (s)");
        modules_hist[i]->GetYaxis()->SetTitle("Rate (trigger/s)");
        if (!options_mgr.tout1_flag) {
            continue;
        }
        sprintf(name, "module_hist_tout1_%02d", i + 1);
        sprintf(title, "module CT_%02d: { %s }, TOUT1", i + 1, met_time_span.c_str());
        modules_hist_tout1[i] = new TH1D(name, title,
                                         nbins, 0 + options_mgr.phase * options_mgr.binwidth / 4, met_time_length + options_mgr.phase * options_mgr.binwidth / 4);
        modules_hist_tout1[i]->SetDirectory(NULL);
        modules_hist_tout1[i]->SetLineColor(kRed);
        modules_hist_tout1[i]->SetMinimum(0);
        modules_hist_tout1[i]->GetXaxis()->SetTitle("T-T0 (s)");
        modules_hist_tout1[i]->GetYaxis()->SetTitle("Rate (trigger/s)");
    }
    TH2D* ch_rate_map = new TH2D("ch_rate_map", "Mean rate of 1600 channels (view from back)", 40, 0, 40, 40, 0, 40);
    ch_rate_map->SetDirectory(NULL);
    ch_rate_map->SetNdivisions(40);
    ch_rate_map->GetYaxis()->SetNdivisions(40);
    for (int i = 0; i < 40; i++) {
        if (i % 8 == 0) {
            ch_rate_map->GetXaxis()->SetBinLabel(i + 1, Form("%02d", i));
            ch_rate_map->GetYaxis()->SetBinLabel(i + 1, Form("%02d", i));
        }
    }

    double pre_second = 0;
    double cur_second = 0;
    double total_valid_second = 0;

    bool is_first = true;
    double pre_time = 0;
    double cur_time = 0;

    int pre_percent = 0;
    int cur_percent = 0;
    cout << "reading trigger data ... " << endl;
    cout << "[ " << flush;
    for (Long64_t q = begin_entry; q < end_entry; q++) {
        cur_percent = static_cast<int>(100.0 * q / (end_entry - begin_entry));
        if (cur_percent - pre_percent > 0 && cur_percent % 2 == 0) {
            pre_percent = cur_percent;
            cout << "#" << flush;
        }
        t_pol_event_tree->GetEntry(q);
        if (t_pol_event.is_ped) continue;
        if (!options_mgr.weight_filename.IsNull()) {
            t_pol_weight_tree->GetEntry(q);
        }

        if (t_pol_event.trigger_n < options_mgr.min_bars || t_pol_event.trigger_n > options_mgr.max_bars) {
            continue;
        }
        cur_second = t_pol_event.event_time - begin_time;
        bool is_bad_event = false;
        if (type_mask[0] && t_pol_event.type == 0xF000) {
            is_bad_event = true;
        } else if (type_mask[1] && t_pol_event.type == 0x00FF) {
            is_bad_event = true;
        } else if (type_mask[2] && t_pol_event.type == 0xFF00) {
            is_bad_event = true;
        }
        if (!options_mgr.bar_mask_filename.IsNull()) {
            for (int i = 0; i < 25; i++) {
                if (!t_pol_event.time_aligned[i]) continue;
                for (int j = 0; j < 64; j++) {
                    if (t_pol_event.trigger_bit[i][j] && bar_mask[i][j]) {
                        is_bad_event = true;
                        break;
                    }
                }
            }
        }
        if (is_bad_event) {
            continue;
        }

        double cur_weight = 1.0;
        if (!options_mgr.weight_filename.IsNull()) cur_weight = t_pol_weight.event_weight;
        trigger_hist->Fill(cur_second, cur_weight);

        // fill angle and position
        earth_theta_hist->Fill(cur_second, radec_to_theta(
                    t_pol_event.det_z_radec[0], t_pol_event.det_z_radec[1],
                    t_pol_event.det_x_radec[0], t_pol_event.det_x_radec[1],
                    t_pol_event.earth_radec[0], t_pol_event.earth_radec[1]));
        earth_pha_hist->Fill(cur_second, radec_to_pha(
                    t_pol_event.det_z_radec[0], t_pol_event.det_z_radec[1],
                    t_pol_event.det_x_radec[0], t_pol_event.det_x_radec[1],
                    t_pol_event.earth_radec[0], t_pol_event.earth_radec[1]));
        sun_theta_hist->Fill(cur_second, radec_to_theta(
                    t_pol_event.det_z_radec[0], t_pol_event.det_z_radec[1],
                    t_pol_event.det_x_radec[0], t_pol_event.det_x_radec[1],
                    t_pol_event.sun_radec[0], t_pol_event.sun_radec[1]));
        sun_pha_hist->Fill(cur_second, radec_to_pha(
                    t_pol_event.det_z_radec[0], t_pol_event.det_z_radec[1],
                    t_pol_event.det_x_radec[0], t_pol_event.det_x_radec[1],
                    t_pol_event.sun_radec[0], t_pol_event.sun_radec[1]));
        grb_theta_hist->Fill(cur_second, radec_to_theta(
                    t_pol_event.det_z_radec[0], t_pol_event.det_z_radec[1],
                    t_pol_event.det_x_radec[0], t_pol_event.det_x_radec[1],
                    options_mgr.grb_ra, options_mgr.grb_dec));
        grb_pha_hist->Fill(cur_second, radec_to_pha(
                    t_pol_event.det_z_radec[0], t_pol_event.det_z_radec[1],
                    t_pol_event.det_x_radec[0], t_pol_event.det_x_radec[1],
                    options_mgr.grb_ra, options_mgr.grb_dec));
        count_hist->Fill(cur_second);
        latlon_map_hist->Fill(
                wgs84_to_longitude(t_pol_event.wgs84_xyz[0], t_pol_event.wgs84_xyz[1], t_pol_event.wgs84_xyz[2]),
                wgs84_to_latitude(t_pol_event.wgs84_xyz[0], t_pol_event.wgs84_xyz[1], t_pol_event.wgs84_xyz[2]),
                cur_weight);
        if (is_first) {
            is_first = false;
            pre_time = cur_second;
        } else {
            cur_time = cur_second;
            lltime_map_hist->Fill(
                    wgs84_to_longitude(t_pol_event.wgs84_xyz[0], t_pol_event.wgs84_xyz[1], t_pol_event.wgs84_xyz[2]),
                    wgs84_to_latitude(t_pol_event.wgs84_xyz[0], t_pol_event.wgs84_xyz[1], t_pol_event.wgs84_xyz[2]),
                    cur_time - pre_time);
            pre_time = cur_time;
        }

        for (int i = 0; i < 25; i++) {
            if (t_pol_event.trig_accepted[i]) {
                if (options_mgr.weight_filename.IsNull()) {
                    modules_hist[i]->Fill(cur_second, 1.0);
                } else {
                    modules_hist[i]->Fill(cur_second, t_pol_weight.mod_weight[i]);
                }
            }
        }
        if (options_mgr.tout1_flag) {
            for (int i = 0; i < 25; i++) {
                if (!t_pol_event.time_aligned[i]) continue;
                modules_hist_tout1[i]->Fill(cur_second, t_pol_event.raw_rate[i]);
                for (int j = 0; j < 64; j++) {
                    if (t_pol_event.trigger_bit[i][j]) {
                        double cur_bin_content = ch_rate_map->GetBinContent(ijtoxb(i, j) + 1, ijtoyb(i, j) + 1);
                        if (options_mgr.weight_filename.IsNull()) {
                            ch_rate_map->SetBinContent(ijtoxb(i, j) + 1, ijtoyb(i, j) + 1, cur_bin_content + 1);
                        } else {
                            ch_rate_map->SetBinContent(ijtoxb(i, j) + 1, ijtoyb(i, j) + 1, cur_bin_content + t_pol_weight.ch_weight[i][j]);
                        }
                    }
                }
            }
            if (cur_second - pre_second > 1.0) {
                if (cur_second - pre_second < 3.0) {
                    total_valid_second += cur_second - pre_second;
                }
                pre_second = cur_second;
            }
        }
    }
    cout << " DONE ]" << endl;
    pol_event_file->Close();
    delete pol_event_file;
    pol_event_file = NULL;
    if (!options_mgr.weight_filename.IsNull()) {
        pol_weight_file->Close();
        delete pol_weight_file;
        pol_weight_file = NULL;
    }

    // calculate angle and position rate
    for (int i = 1; i <= count_hist->GetNbinsX(); i++) {
        double cur_count = count_hist->GetBinContent(i);
        if (cur_count > 0) {
            earth_theta_hist->SetBinContent(i, earth_theta_hist->GetBinContent(i) / cur_count);
            earth_pha_hist->SetBinContent(i, earth_pha_hist->GetBinContent(i) / cur_count);
            sun_theta_hist->SetBinContent(i, sun_theta_hist->GetBinContent(i) / cur_count);
            sun_pha_hist->SetBinContent(i, sun_pha_hist->GetBinContent(i) / cur_count);
            grb_theta_hist->SetBinContent(i, grb_theta_hist->GetBinContent(i) / cur_count);
            grb_pha_hist->SetBinContent(i, grb_pha_hist->GetBinContent(i) / cur_count);
        }
    }
    for (int x = 1; x <= lltime_map_hist->GetNbinsX(); x++) {
        for (int y = 1; y <= lltime_map_hist->GetNbinsY(); y++) {
            double cur_timeint = lltime_map_hist->GetBinContent(x, y);
            if (cur_timeint > 0) {
                latlon_map_hist->SetBinContent(x, y, latlon_map_hist->GetBinContent(x, y) / cur_timeint);
            }
        }
    }

    // calculate rate for event rate
    for (int b = 1; b <= trigger_hist->GetNbinsX(); b++) {
        trigger_hist->SetBinContent(b, trigger_hist->GetBinContent(b) / trigger_hist->GetBinWidth(b));
        trigger_hist->SetBinError(b,   trigger_hist->GetBinError(b)   / trigger_hist->GetBinWidth(b));
    }

    // calculate background
    TH1D* trigger_hist_fix = static_cast<TH1D*>(trigger_hist->Clone("trigger_hist_fix"));
    // fix begin and end
    trigger_hist_fix->SetBinContent(1, trigger_hist_fix->GetBinContent(2));
    int total_bins = trigger_hist_fix->GetNbinsX();
    trigger_hist_fix->SetBinContent(total_bins, trigger_hist_fix->GetBinContent(total_bins - 1));
    // fill saa gap
    int bin_idx = 0;
    while (bin_idx < trigger_hist_fix->GetNbinsX()) {
        bin_idx++;
        if (trigger_hist_fix->GetBinContent(bin_idx) > 1) continue;
        // find the first bin before saa
        int bin_idx_left = bin_idx;
        int pre_count = 0;
        double bin_center = trigger_hist_fix->GetBinCenter(bin_idx);
        while (bin_idx_left >= 1) {
            bin_idx_left--;
            if (trigger_hist_fix->GetBinWidth(1) < 1.0) {
                if (trigger_hist_fix->GetBinCenter(bin_idx_left) < bin_center - 4.0) {
                    break;
                }
            } else {
                if (trigger_hist_fix->GetBinContent(bin_idx_left) < pre_count) {
                    break;
                } else {
                    pre_count = trigger_hist_fix->GetBinContent(bin_idx_left);
                }
            }
        }
        // find the first bin after saa
        int bin_idx_right = bin_idx;
        pre_count = 0;
        while (bin_idx_right <= trigger_hist_fix->GetNbinsX()) {
            bin_idx_right++;
            if (trigger_hist_fix->GetBinContent(bin_idx_right) == 0) {
                bin_center = trigger_hist_fix->GetBinCenter(bin_idx_right);
                continue;
            }
            if (trigger_hist_fix->GetBinWidth(1) < 1.0) {
                if (trigger_hist_fix->GetBinWidth(bin_idx_right) < 1.0 && trigger_hist_fix->GetBinCenter(bin_idx_right) > bin_center + 4.0) {
                    break;
                }
            } else {
                if (trigger_hist_fix->GetBinContent(bin_idx_right) < pre_count) {
                    break;
                } else {
                    pre_count = trigger_hist_fix->GetBinContent(bin_idx_right);
                }
            }
        }
        // fill gap
        double ratio = 1.5;
        double slope = (trigger_hist_fix->GetBinContent(bin_idx_right) * ratio - trigger_hist_fix->GetBinContent(bin_idx_left) * ratio) / (bin_idx_right - bin_idx_left);
        slope *= 2.0;
        for (int i = 1; i < bin_idx_right - bin_idx_left; i++) {
            if (slope > 0) {
                trigger_hist_fix->SetBinContent(bin_idx_left + i, trigger_hist_fix->GetBinContent(bin_idx_left) * ratio + i * slope);
            } else {
                trigger_hist_fix->SetBinContent(bin_idx_right - i, trigger_hist_fix->GetBinContent(bin_idx_right) * ratio - i * slope);
            }
        }
        // jump the gap
        bin_idx = bin_idx_right;
    }
    TSpectrum t_spec;
    TH1D* trigger_hist_bkg = static_cast<TH1D*>(t_spec.Background(trigger_hist_fix, options_mgr.niter));
    // correct bkg
    for (int i = 1; i <= trigger_hist->GetNbinsX(); i++) {
        if (trigger_hist->GetBinContent(i) < 2) {
            trigger_hist_bkg->SetBinContent(i, 0);
        }
    }

    // calculate rate for modules
    for (int j = 0; j < 25; j++) {
        for (int b = 1; b < modules_hist[j]->GetNbinsX(); b++) {
            modules_hist[j]->SetBinContent(b, modules_hist[j]->GetBinContent(b) / modules_hist[j]->GetBinWidth(b));
            modules_hist[j]->SetBinError(  b, modules_hist[j]->GetBinError(b)   / modules_hist[j]->GetBinWidth(b));
        }
    }
    if (options_mgr.tout1_flag) {
        for (int j = 0; j < 25; j++) {
            for (int b = 1; b < modules_hist_tout1[j]->GetNbinsX(); b++) {
                modules_hist_tout1[j]->SetBinContent(b, modules_hist_tout1[j]->GetBinContent(b) / modules_hist_tout1[j]->GetBinWidth(b));
                modules_hist_tout1[j]->SetBinError(  b, modules_hist_tout1[j]->GetBinError(b)   / modules_hist_tout1[j]->GetBinWidth(b));
            }
        }
    }

    if (!options_mgr.output_filename.IsNull()) {
        cout << " - saving light curve ..." << endl;
        TFile* outfile = new TFile(options_mgr.output_filename.Data(), "recreate");
        if (outfile->IsZombie()) {
            cout << "ERROR: output file open failed: " << options_mgr.output_filename.Data() << endl;
        } else {
            trigger_hist->Write();
            for (int i = 0; i < 25; i++) {
                modules_hist[i]->Write();
            }
            if (options_mgr.tout1_flag) {
                for (int i = 0; i < 25; i++) {
                    modules_hist_tout1[i]->Write();
                }
            }
            TNamed("met_time_span", Form("%d => %d", static_cast<int>(begin_time), static_cast<int>(end_time))).Write();
            TNamed("time_length", Form("%f", met_time_length)).Write();
            TNamed("bin_width", Form("%f", options_mgr.binwidth)).Write();
            TNamed("nbins", Form("%d", nbins)).Write();
            outfile->Close();
        }
    }

    cout << " - drawing ... " << endl;
    gStyle->SetOptStat(0);
    double y_max = 0;
    for (int i = 0; i < 25; i++) {
        if (modules_hist[i]->GetMaximum() > y_max) {
            y_max = modules_hist[i]->GetMaximum();
        }
    }
    for (int i = 0; i < 25; i++) {
        modules_hist[i]->SetMaximum(y_max * 1.1);
    }
    for (int i = 0; i < 25; i++) {
        rate_canvas.cd_modules(i);
        modules_hist[i]->Draw("EH");
    }

    if (options_mgr.tout1_flag) {
        y_max = 0;
        for (int i = 0; i < 25; i++) {
            if (modules_hist_tout1[i]->GetMaximum() > y_max) {
                y_max = modules_hist_tout1[i]->GetMaximum();
            }
        }
        for (int i = 0; i < 25; i++) {
            modules_hist_tout1[i]->SetMaximum(y_max * 1.1);
        }
        for (int i = 0; i < 25; i++) {
            rate_canvas.cd_modules_tout1(i);
            modules_hist_tout1[i]->Draw("EH");
        }
        for (int i = 0; i < 25; i++) {
            for (int j = 0; j < 64; j++) {
                double cur_bin_content = ch_rate_map->GetBinContent(ijtoxb(i, j) + 1, ijtoyb(i, j) + 1);
                ch_rate_map->SetBinContent(ijtoxb(i, j) + 1, ijtoyb(i, j) + 1, cur_bin_content / total_valid_second);
            }
        }
        rate_canvas.cd_ch_map(1);
        ch_rate_map->Draw("COLZ");
		TLine* line_h_[4];
		TLine* line_v_[4];
		for (int i = 0; i < 4; i++) {
			line_h_[i] = new TLine(0, (i + 1) * 8, 40, (i + 1) * 8);
			line_h_[i]->SetLineColor(kWhite);
			line_h_[i]->Draw("SAME");
			line_v_[i] = new TLine((i + 1) * 8, 0, (i + 1) * 8, 40);
			line_v_[i]->SetLineColor(kWhite);
			line_v_[i]->Draw("SAME");
		}
        rate_canvas.cd_ch_map(2);
        ch_rate_map->Draw("LEGO2");
    }

    rate_canvas.draw_trigger_hist(trigger_hist);
    rate_canvas.draw_trigger_hist_bkg(trigger_hist_bkg);

    rate_canvas.cd_pos_map();
    latlon_map_hist->Draw("colz");

    rate_canvas.cd_theta_pha(1);
    earth_theta_hist->Draw("h");
    rate_canvas.cd_theta_pha(2);
    earth_pha_hist->Draw("h");
    rate_canvas.cd_theta_pha(3);
    sun_theta_hist->Draw("h");
    rate_canvas.cd_theta_pha(4);
    sun_pha_hist->Draw("h");
    rate_canvas.cd_theta_pha(5);
    grb_theta_hist->Draw("h");
    rate_canvas.cd_theta_pha(6);
    grb_pha_hist->Draw("h");

    rootapp->Run();

    return 0;
}
