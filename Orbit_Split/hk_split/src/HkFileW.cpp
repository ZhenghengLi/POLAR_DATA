#include "HkFileW.hpp"

using namespace std;

HkFileW::HkFileW() {
    t_file_out_ = NULL;
    t_hk_obox_tree_ = NULL;
    t_hk_ibox_tree_ = NULL;

    cur_hkfile_r = NULL;
}

HkFileW::~HkFileW() {
    close();
}

bool HkFileW::open(const char* filename) {
    if (t_file_out_ != NULL)
        return false;

    t_file_out_ = new TFile(filename, "RECREATE");
    if (t_file_out_->IsZombie())
        return false;

    // hk_obox
    t_hk_obox_tree_ = new TTree("t_hk_obox", "obox housekeeping packets");
    build_hk_obox_tree(t_hk_obox_tree_, t_hk_obox);

    // hk_ibox
    t_hk_ibox_tree_ = new TTree("t_hk_ibox", "ibox housekeeping info");
    build_hk_ibox_tree(t_hk_ibox_tree_, t_hk_ibox);

    cur_hkfile_r = NULL;

    hk_obox_cur_index_          = -1;
    hk_ibox_cur_index_          = -1;
    
    hk_obox_first_gps_found_    = false;
    hk_obox_first_gps_index_    = -1; 
    hk_obox_first_gps_week_     = -1; 
    hk_obox_first_gps_second_   = -1; 
    hk_obox_last_gps_index_     = -1; 
    hk_obox_last_gps_week_      = -1; 
    hk_obox_last_gps_second_    = -1; 
    hk_obox_total_gps_count_    = 0;
    hk_ibox_first_gps_found_    = false;
    hk_ibox_first_gps_index_    = -1; 
    hk_ibox_first_gps_week_     = -1; 
    hk_ibox_first_gps_second_   = -1; 
    hk_ibox_last_gps_index_     = -1; 
    hk_ibox_last_gps_week_      = -1; 
    hk_ibox_last_gps_second_    = -1; 
    hk_ibox_total_gps_count_    = 0;

    return true;
}

void HkFileW::close() {
    if (t_file_out_ == NULL)
        return;

    delete t_hk_obox_tree_;
    t_hk_obox_tree_ = NULL;

    delete t_hk_ibox_tree_;
    t_hk_ibox_tree_ = NULL;

    t_file_out_->Close();
    delete t_file_out_;
    t_file_out_ = NULL;
}

void HkFileW::set_hkfile_r(HkFileR* hkfile_r) {
    cur_hkfile_r = hkfile_r;
}

void HkFileW::write_hk_obox() {
    if (cur_hkfile_r == NULL)
        return;
    int pre_percent = 0;
    int cur_percent = 0;
    cout << "Copying obox housekeeping data from " << cur_hkfile_r->get_filename() << " ..." << endl;
    cout << "[ " << flush;
    cur_hkfile_r->hk_obox_set_start();
    while (cur_hkfile_r->hk_obox_next()) {
        cur_percent = static_cast<int>(100 * cur_hkfile_r->hk_obox_get_cur_entry() / cur_hkfile_r->hk_obox_get_tot_entries());
        if (cur_percent - pre_percent > 0 && cur_percent % 2 == 0) {
            pre_percent = cur_percent;
            cout << "#" << flush;
        }
        t_hk_obox = cur_hkfile_r->t_hk_obox;
        // fill data
        t_hk_obox_tree_->Fill();

        // record first and last GPS and entry
        hk_obox_cur_index_++;
        if (t_hk_obox.odd_is_bad == 0 || t_hk_obox.even_is_bad == 0) {
            hk_obox_total_gps_count_++;
            if (!hk_obox_first_gps_found_) {
                hk_obox_first_gps_found_  = true;
                hk_obox_first_gps_index_  = hk_obox_cur_index_;
                hk_obox_first_gps_week_   = t_hk_obox.abs_gps_week;
                hk_obox_first_gps_second_ = t_hk_obox.abs_gps_second;
            }
            hk_obox_last_gps_index_       = hk_obox_cur_index_;
            hk_obox_last_gps_week_        = t_hk_obox.abs_gps_week;
            hk_obox_last_gps_second_      = t_hk_obox.abs_gps_second;
        }
    }
    cout << " DONE ]" << endl;
}

void HkFileW::write_hk_ibox() {
    if (cur_hkfile_r == NULL)
        return;
    int pre_percent = 0;
    int cur_percent = 0;
    cout << "Copying ibox housekeeping data from " << cur_hkfile_r->get_filename() << " ..." << endl;
    cout << "[ " << flush;
    cur_hkfile_r->hk_ibox_set_start();
    while (cur_hkfile_r->hk_ibox_next()) {
        cur_percent = static_cast<int>(100 * cur_hkfile_r->hk_ibox_get_cur_entry() / cur_hkfile_r->hk_ibox_get_tot_entries());
        if (cur_percent - pre_percent > 0 && cur_percent % 2 == 0) {
            pre_percent = cur_percent;
            cout << "#" << flush;
        }
        t_hk_ibox = cur_hkfile_r->t_hk_ibox;
        // fill data
        t_hk_ibox_tree_->Fill();

        // record first and last GPS and entry
        hk_ibox_cur_index_++;
        if (t_hk_ibox.is_bad == 0) {
            hk_ibox_total_gps_count_++;
            if (!hk_ibox_first_gps_found_) {
                hk_ibox_first_gps_found_  = true;
                hk_ibox_first_gps_index_  = hk_ibox_cur_index_;
                hk_ibox_first_gps_week_   = t_hk_ibox.abs_gps_week;
                hk_ibox_first_gps_second_ = t_hk_ibox.abs_gps_second;
            }
            hk_ibox_last_gps_index_       = hk_ibox_cur_index_;
            hk_ibox_last_gps_week_        = t_hk_ibox.abs_gps_week;
            hk_ibox_last_gps_second_      = t_hk_ibox.abs_gps_second;
        }
    }
    cout << " DONE ]" << endl;
}

void HkFileW::write_before_close() {
    t_hk_obox_tree_->Write();
    t_hk_ibox_tree_->Write();
}

void HkFileW::write_meta(const char* key, const char* value) {
    if (t_file_out_ == NULL)
        return;
    TNamed* cur_meta = new TNamed(key, value);
    cur_meta->Write();
    delete cur_meta;
    cur_meta = NULL;
}

void HkFileW::gen_gps_result_str() {
    char str_buffer[200];
    // hk_obox
    sprintf(str_buffer, "%d:%d[%ld] => %d:%d[%ld]; %ld/%ld",
            static_cast<int>(hk_obox_first_gps_week_),
            static_cast<int>(hk_obox_first_gps_second_),
            static_cast<long int>(hk_obox_first_gps_index_),
            static_cast<int>(hk_obox_last_gps_week_),
            static_cast<int>(hk_obox_last_gps_second_),
            static_cast<long int>(hk_obox_last_gps_index_),
            static_cast<long int>(hk_obox_total_gps_count_),
            static_cast<long int>(t_hk_obox_tree_->GetEntries()));
    hk_obox_gps_result_str_.assign(str_buffer);
    // hk_ibox
    sprintf(str_buffer, "%d:%d[%ld] => %d:%d[%ld]; %ld/%ld",
            static_cast<int>(hk_ibox_first_gps_week_),
            static_cast<int>(hk_ibox_first_gps_second_),
            static_cast<long int>(hk_ibox_first_gps_index_),
            static_cast<int>(hk_ibox_last_gps_week_),
            static_cast<int>(hk_ibox_last_gps_second_),
            static_cast<long int>(hk_ibox_last_gps_index_),
            static_cast<long int>(hk_ibox_total_gps_count_),
            static_cast<long int>(t_hk_ibox_tree_->GetEntries()));
    hk_ibox_gps_result_str_.assign(str_buffer);
}

void HkFileW::write_gps_span() {
    write_meta("m_oboxgps", hk_obox_gps_result_str_.c_str());
    write_meta("m_iboxgps", hk_ibox_gps_result_str_.c_str());
}

void HkFileW::print_gps_span() {
    cout << "================================================================================" << endl;
    cout << "hk_obox_gps: { " << hk_obox_gps_result_str_ << " }" << endl;
    cout << "hk_ibox_gps: { " << hk_ibox_gps_result_str_ << " }" << endl;
    cout << "================================================================================" << endl;
}
