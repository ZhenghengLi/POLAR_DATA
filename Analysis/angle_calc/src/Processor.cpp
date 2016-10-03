#include "Processor.hpp"

using namespace std;

Processor::Processor(OptionsManager* my_options_mgr, TApplication* my_rootapp) {
    cur_options_mgr_ = my_options_mgr;
    cur_rootapp_     = my_rootapp;
}

Processor::~Processor() {
    
}

int Processor::start_process() {
    switch (cur_options_mgr_->action) {
    case 1:
        return do_action_1_();
    case 2:
        return do_action_2_();
    default:
        return 1;
    }
}

int Processor::do_action_1_() {
    if (!rec_event_data_file_.open(cur_options_mgr_->rec_event_data_filename.Data(), 'r')) {
        cerr << "root file open failed: " << cur_options_mgr_->rec_event_data_filename.Data() << endl;
        return 1;
    }
    if (!angle_data_file_.open(cur_options_mgr_->angle_data_filename.Data(), 'w')) {
        cerr << "root file open failed: " << cur_options_mgr_->angle_data_filename.Data() << endl;
        return 1;
    }
    event_filter_.set_low_energy_thr(cur_options_mgr_->low_energy_thr);
    event_filter_.set_max_bars(cur_options_mgr_->max_bars);
    int pre_percent = 0;
    int cur_percent = 0;
    cout << "Calculating Scattering Angle ..." << endl;
    cout << "[ " << flush;
    rec_event_data_file_.event_set_start();
    while (rec_event_data_file_.event_next()) {
        cur_percent = static_cast<int>(100 * rec_event_data_file_.event_get_cur_entry() / rec_event_data_file_.event_get_tot_entries());
        if (cur_percent - pre_percent > 0 && cur_percent % 2 == 0) {
            pre_percent = cur_percent;
            cout << "#" << flush;
        }
        if (!event_filter_.find_first_two_bars(rec_event_data_file_.t_rec_event)) {
            continue;
        }
        if (event_filter_.check(rec_event_data_file_.t_rec_event)) {
            angle_data_file_.t_angle.abs_gps_week    = event_filter_.abs_gps_week;
            angle_data_file_.t_angle.abs_gps_second  = event_filter_.abs_gps_second;
            angle_data_file_.t_angle.abs_gps_valid   = event_filter_.abs_gps_valid;
            angle_data_file_.t_angle.abs_ship_second = event_filter_.abs_ship_second;
            angle_data_file_.t_angle.first_ij[0]     = event_filter_.first_pos.i;
            angle_data_file_.t_angle.first_ij[1]     = event_filter_.first_pos.j;
            angle_data_file_.t_angle.second_ij[0]    = event_filter_.second_pos.i;
            angle_data_file_.t_angle.second_ij[1]    = event_filter_.second_pos.j;
            angle_data_file_.t_angle.rand_angle      = event_filter_.first_pos.angle_to(event_filter_.second_pos);
            angle_data_file_.angle_fill();
        }
    }
    cout << " DONE ]" << endl;
    angle_data_file_.write_all_tree();
    angle_data_file_.write_meta("m_dattype", "POLAR SCATTERING ANGLE DATA", false);
    angle_data_file_.write_meta("m_version", (SW_NAME + " " + SW_VERSION).c_str(), false);
    angle_data_file_.write_fromfile(rec_event_data_file_.get_fromfile_str().c_str());
    angle_data_file_.write_gps_span(rec_event_data_file_.get_gps_span_str().c_str());
    angle_data_file_.write_lasttime();
    angle_data_file_.close();
    rec_event_data_file_.close();
    
    return 0;
}

int Processor::do_action_2_() {
    if (!angle_data_file_.open(cur_options_mgr_->angle_data_filename.Data(), 'r')) {
        cerr << "root file open failed: " << cur_options_mgr_->angle_data_filename.Data() << endl;
        return 1;
    }
    int hit_map[25][64];
    for (int i = 0; i < 25; i++) {
        for (int j = 0; j < 64; j++) {
            hit_map[i][j] = 0;
        }
    }
    TH1F* curve_hist = new TH1F("curve_hist", "Modulation Curve", 60, 0, 360);
    curve_hist->SetDirectory(NULL);
    curve_hist->SetMinimum(0);
    TH2F* hit_map_hist = new TH2F("hit_map_hist", "Hit Map", 40, 0, 40, 40, 0, 40);
    hit_map_hist->SetDirectory(NULL);
    hit_map_hist->GetXaxis()->SetNdivisions(40);
    hit_map_hist->GetYaxis()->SetNdivisions(40);
    for (int i = 0; i < 40; i++) {
        if (i % 8 == 0) {
            hit_map_hist->GetXaxis()->SetBinLabel(i + 1, Form("%02d", i));
            hit_map_hist->GetYaxis()->SetBinLabel(i + 1, Form("%02d", i));
        }
    }
    int pre_percent = 0;
    int cur_percent = 0;
    cout << "Filling Angle Data ..." << endl;
    cout << "[ " << flush;
    angle_data_file_.angle_set_start();
    while (angle_data_file_.angle_next()) {
        cur_percent = static_cast<int>(100 * angle_data_file_.angle_get_cur_entry() / angle_data_file_.angle_get_tot_entries());
        if (cur_percent - pre_percent > 0 && cur_percent % 2 == 0) {
            pre_percent = cur_percent;
            cout << "#" << flush;
        }
        hit_map[angle_data_file_.t_angle.first_ij[0]][angle_data_file_.t_angle.first_ij[1]]++;
        hit_map[angle_data_file_.t_angle.second_ij[0]][angle_data_file_.t_angle.second_ij[1]]++;
        curve_hist->Fill(angle_data_file_.t_angle.rand_angle);
    }
    cout << " DONE ]" << endl;
    angle_data_file_.close();
    for (int i = 0; i < 25; i++) {
        for (int j = 0; j < 64; j++) {
            hit_map_hist->SetBinContent(ijtox(i, j) + 1, ijtoy(i, j) + 1, hit_map[i][j]);
        }
    }
    curve_show_.set_curve_hist(curve_hist);
    curve_show_.set_hit_map_hist(hit_map_hist);
    curve_show_.show_curve();
    cur_rootapp_->Run();
    return 0;
}
