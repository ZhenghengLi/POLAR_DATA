#include "PPDIterator.hpp"

using namespace std;

PPDIterator::PPDIterator() {
    t_file_in_ = NULL;
    t_ppd_tree_ = NULL;

    re_ship_span_ = "^ *(\\d+)\\[\\d+\\] => (\\d+)\\[\\d+\\]; \\d+/\\d+ *$";
}

PPDIterator::~PPDIterator() {
    if (t_file_in_ != NULL)
        close();
}

bool PPDIterator::open(const char* filename) {
    if (t_file_in_ == NULL)
        return false;
    t_file_in_ = new TFile(filename, "READ");
    if (t_file_in_->IsZombie())
        return false;
    t_ppd_tree_ = static_cast<TTree*>(t_file_in_->Get("t_ppd"));
    if (t_ppd_tree_ == NULL)
        return false;

    // get first and last ship_second
    TNamed* m_shipspan = static_cast<TNamed*>(t_file_in_->Get("m_shipspan"));
    if (m_shipspan == NULL)
        return false;
    cmatch cm;
    if (regex_match(m_shipspan->GetTitle(), cm, re_ship_span_)) {
        first_ship_second_ = TString(cm[1]).Atof();
        last_ship_second_  = TString(cm[2]).Atof();
    } else {
        cerr << "ship time span match failed: " << m_shipspan->GetTitle() << endl;
        return false;
    }

    t_ppd_tree_->SetBranchAddress("ship_time_sec",      &cur_ppd_.ship_time_sec  );
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
        } else {
            return false;
        }

    } while (cur_ppd_.flag_of_pos != 0x55 or cur_ppd_.utc_time_sec < 0);
    ppd_before = cur_ppd_;
    ppd_after  = cur_ppd_;

    ppd_reach_end_ = false;

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
        } else {
            ppd_reach_end_ = true;
            return false;
        }
    } while (cur_ppd_.flag_of_pos != 0x55 or cur_ppd_.utc_time_sec < 0 or cur_ppd_.ship_time_sec - ppd_after.ship_time_sec < 0.5);
    ppd_before = ppd_after;
    ppd_after = cur_ppd_;

    // calculate slope
    double ship_time_diff = ppd_after.ship_time_sec - ppd_before.ship_time_sec;
    wgs84_x_slope_   = (ppd_after.wgs84_x   - ppd_before.wgs84_x  ) / ship_time_diff;
    wgs84_y_slope_   = (ppd_after.wgs84_y   - ppd_before.wgs84_y  ) / ship_time_diff;
    wgs84_z_slope_   = (ppd_after.wgs84_z   - ppd_before.wgs84_z  ) / ship_time_diff;
    det_z_ra_slope_  = (ppd_after.det_z_ra  - ppd_before.det_z_ra ) / ship_time_diff;
    det_z_dec_slope_ = (ppd_after.det_z_dec - ppd_before.det_z_dec) / ship_time_diff;
    det_x_ra_slope_  = (ppd_after.det_x_ra  - ppd_before.det_x_ra ) / ship_time_diff;
    det_x_dec_slope_ = (ppd_after.det_x_dec - ppd_before.det_x_dec) / ship_time_diff;
    earth_ra_slope_  = (ppd_after.earth_ra  - ppd_before.earth_ra ) / ship_time_diff;
    earth_dec_slope_ = (ppd_after.earth_dec - ppd_before.earth_dec) / ship_time_diff;
    sun_ra_slope_    = (ppd_after.sun_ra    - ppd_before.sun_ra   ) / ship_time_diff;
    sun_dec_slope_   = (ppd_after.sun_dec   - ppd_before.sun_dec  ) / ship_time_diff;

    return true;
}

bool PPDIterator::get_reach_end() {
    return ppd_reach_end_;
}

void PPDIterator::calc_ppd_interm(double ship_time) {
    double ship_time_diff = ship_time - ppd_before.ship_time_sec;
    ppd_interm.wgs84_x   = ppd_before.wgs84_x   + wgs84_x_slope_   * ship_time_diff;
    ppd_interm.wgs84_y   = ppd_before.wgs84_y   + wgs84_y_slope_   * ship_time_diff;
    ppd_interm.wgs84_z   = ppd_before.wgs84_z   + wgs84_z_slope_   * ship_time_diff;
    ppd_interm.det_z_ra  = ppd_before.det_z_ra  + det_z_ra_slope_  * ship_time_diff;
    ppd_interm.det_z_dec = ppd_before.det_z_dec + det_z_dec_slope_ * ship_time_diff;
    ppd_interm.det_x_ra  = ppd_before.det_x_ra  + det_x_ra_slope_  * ship_time_diff;
    ppd_interm.det_x_dec = ppd_before.det_x_dec + det_x_dec_slope_ * ship_time_diff;
    ppd_interm.earth_ra  = ppd_before.earth_ra  + earth_ra_slope_  * ship_time_diff;
    ppd_interm.earth_dec = ppd_before.earth_dec + earth_dec_slope_ * ship_time_diff;
    ppd_interm.sun_ra    = ppd_before.sun_ra    + sun_ra_slope_    * ship_time_diff;
    ppd_interm.sun_dec   = ppd_before.sun_dec   + sun_dec_slope_   * ship_time_diff;
}

double PPDIterator::get_first_ship_second() {
    return first_ship_second_;
}

double PPDIterator::get_last_ship_second() {
    return last_ship_second_;
}

