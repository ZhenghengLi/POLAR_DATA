#include "PPDIterator.hpp"

using namespace std;

double PPDIterator::get_leap_seconds_(double utc_time_sec) {
    if (utc_time_sec < 1119744000.0) {
        return 16;
    } else if (utc_time_sec < 1167264000.0) {
        return 17;
    } else {
        return 18;
    }
}

double PPDIterator::utc_to_gps_(double utc_time_sec) {
    return (utc_time_sec + get_leap_seconds_(utc_time_sec));
}

PPDIterator::PPDIterator() {
    t_file_in_ = NULL;
    t_ppd_tree_ = NULL;

    re_gps_span_ = "^ *(\\d+):(\\d+)\\[\\d+\\] => (\\d+):(\\d+)\\[\\d+\\]; \\d+/\\d+ *$";
}

PPDIterator::~PPDIterator() {
    if (t_file_in_ != NULL)
        close();
}

bool PPDIterator::open(const char* filename) {
    if (t_file_in_ != NULL)
        return false;
    t_file_in_ = new TFile(filename, "READ");
    if (t_file_in_->IsZombie())
        return false;
    t_ppd_tree_ = static_cast<TTree*>(t_file_in_->Get("t_ppd"));
    if (t_ppd_tree_ == NULL)
        return false;

    // get first and last gps time
    TNamed* m_utc_span = static_cast<TNamed*>(t_file_in_->Get("m_utc_span"));
    if (m_utc_span == NULL)
        return false;
    cmatch cm;
    if (regex_match(m_utc_span->GetTitle(), cm, re_gps_span_)) {
        first_gps_time_ = utc_to_gps_(TString(cm[1]).Atof() * 604800 + TString(cm[2]).Atof());
        last_gps_time_  = utc_to_gps_(TString(cm[3]).Atof() * 604800 + TString(cm[4]).Atof());
    } else {
        cerr << "gps time span match failed: " << m_utc_span->GetTitle() << endl;
        return false;
    }

    t_ppd_tree_->SetBranchAddress("utc_time_sec",       &cur_ppd_.utc_time_sec   );
    t_ppd_tree_->SetBranchAddress("flag_of_pos",        &cur_ppd_.flag_of_pos    );
    t_ppd_tree_->SetBranchAddress("wgs84_x",            &cur_ppd_.wgs84_x        );
    t_ppd_tree_->SetBranchAddress("wgs84_y",            &cur_ppd_.wgs84_y        );
    t_ppd_tree_->SetBranchAddress("wgs84_z",            &cur_ppd_.wgs84_z        );
    t_ppd_tree_->SetBranchAddress("det_z_ra",           &cur_ppd_.det_z_ra       );
    t_ppd_tree_->SetBranchAddress("det_z_dec",          &cur_ppd_.det_z_dec      );
    t_ppd_tree_->SetBranchAddress("det_x_ra",           &cur_ppd_.det_x_ra       );
    t_ppd_tree_->SetBranchAddress("det_x_dec",          &cur_ppd_.det_x_dec      );
    t_ppd_tree_->SetBranchAddress("earth_ra",           &cur_ppd_.earth_ra       );
    t_ppd_tree_->SetBranchAddress("earth_dec",          &cur_ppd_.earth_dec      );
    t_ppd_tree_->SetBranchAddress("sun_ra",             &cur_ppd_.sun_ra         );
    t_ppd_tree_->SetBranchAddress("sun_dec",            &cur_ppd_.sun_dec        );

    // read the first ppd
    ppd_cur_entry_ = -1;
    do {
        ppd_cur_entry_++;
        if (ppd_cur_entry_ < t_ppd_tree_->GetEntries()) {
            t_ppd_tree_->GetEntry(ppd_cur_entry_);
            cur_ppd_.gps_time_sec = utc_to_gps_(cur_ppd_.utc_time_sec);
        } else {
            return false;
        }

    } while (cur_ppd_.flag_of_pos != 0x55 or cur_ppd_.utc_time_sec < 0);
    ppd_before = cur_ppd_;
    ppd_after  = cur_ppd_;

    ppd_reach_end_ = false;
    next_ppd();

    return true;
}

void PPDIterator::close() {
    if (t_file_in_ == NULL)
        return;
    t_file_in_->Close();
    t_file_in_ = NULL;
    t_ppd_tree_ = NULL;
}

bool PPDIterator::next_ppd() {
    if (t_file_in_ == NULL)
        return false;
    if (ppd_reach_end_)
        return false;
    do {
        ppd_cur_entry_++;
        if (ppd_cur_entry_ < t_ppd_tree_->GetEntries()) {
            t_ppd_tree_->GetEntry(ppd_cur_entry_);
            cur_ppd_.gps_time_sec = utc_to_gps_(cur_ppd_.utc_time_sec);
        } else {
            ppd_reach_end_ = true;
            return false;
        }
    } while (cur_ppd_.flag_of_pos != 0x55 or cur_ppd_.utc_time_sec < 0 or cur_ppd_.gps_time_sec - ppd_after.gps_time_sec < 0.5);
    ppd_before = ppd_after;
    ppd_after = cur_ppd_;

    // calculate slope
    double gps_time_diff = ppd_after.gps_time_sec - ppd_before.gps_time_sec;
    wgs84_x_slope_   = (ppd_after.wgs84_x   - ppd_before.wgs84_x  ) / gps_time_diff;
    wgs84_y_slope_   = (ppd_after.wgs84_y   - ppd_before.wgs84_y  ) / gps_time_diff;
    wgs84_z_slope_   = (ppd_after.wgs84_z   - ppd_before.wgs84_z  ) / gps_time_diff;
    det_z_dec_slope_ = (ppd_after.det_z_dec - ppd_before.det_z_dec) / gps_time_diff;
    det_x_dec_slope_ = (ppd_after.det_x_dec - ppd_before.det_x_dec) / gps_time_diff;
    earth_dec_slope_ = (ppd_after.earth_dec - ppd_before.earth_dec) / gps_time_diff;
    sun_dec_slope_   = (ppd_after.sun_dec   - ppd_before.sun_dec  ) / gps_time_diff;

    return true;
}

bool PPDIterator::get_reach_end() {
    return ppd_reach_end_;
}

double PPDIterator::calc_ra_(double gps_time,
        double before_ra, double before_gps_time,
        double after_ra,  double after_gps_time) {
    if (fabs(after_ra - before_ra) > 16 && fabs(after_gps_time - before_gps_time) < 300) {
        if (after_ra < before_ra) {
            after_ra += 24.0;
        } else {
            before_ra += 24.0;
        }
    }
    double ra_slope = (after_ra - before_ra) / (after_gps_time - before_gps_time);
    double result_ra = before_ra + ra_slope * (gps_time - before_gps_time);
    if (result_ra >= 24.0) {
        result_ra -= 24.0;
    } else if (result_ra < 0) {
        result_ra += 24.0;
    }
    return result_ra;
}

void PPDIterator::calc_ppd_interm(double gps_time) {
    double gps_time_diff = gps_time - ppd_before.gps_time_sec;

    ppd_interm.wgs84_x   = ppd_before.wgs84_x   + wgs84_x_slope_   * gps_time_diff;
    ppd_interm.wgs84_y   = ppd_before.wgs84_y   + wgs84_y_slope_   * gps_time_diff;
    ppd_interm.wgs84_z   = ppd_before.wgs84_z   + wgs84_z_slope_   * gps_time_diff;

    ppd_interm.det_z_ra  = calc_ra_(gps_time,
            ppd_before.det_z_ra, ppd_before.gps_time_sec,
            ppd_after.det_z_ra,  ppd_after.gps_time_sec);
    ppd_interm.det_z_dec = ppd_before.det_z_dec + det_z_dec_slope_ * gps_time_diff;

    ppd_interm.det_x_ra  = calc_ra_(gps_time,
            ppd_before.det_x_ra, ppd_before.gps_time_sec,
            ppd_after.det_x_ra,  ppd_after.gps_time_sec);
    ppd_interm.det_x_dec = ppd_before.det_x_dec + det_x_dec_slope_ * gps_time_diff;

    ppd_interm.earth_ra  = calc_ra_(gps_time,
            ppd_before.earth_ra, ppd_before.gps_time_sec,
            ppd_after.earth_ra,  ppd_after.gps_time_sec);
    ppd_interm.earth_dec = ppd_before.earth_dec + earth_dec_slope_ * gps_time_diff;

    ppd_interm.sun_ra    = calc_ra_(gps_time,
            ppd_before.sun_ra,   ppd_before.gps_time_sec,
            ppd_after.sun_ra,    ppd_after.gps_time_sec);
    ppd_interm.sun_dec   = ppd_before.sun_dec   + sun_dec_slope_   * gps_time_diff;
}

double PPDIterator::get_first_gps_time() {
    return first_gps_time_;
}

double PPDIterator::get_last_gps_time() {
    return last_gps_time_;
}

