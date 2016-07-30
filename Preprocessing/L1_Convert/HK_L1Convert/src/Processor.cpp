#include "Processor.hpp"

using namespace std;

Processor::Processor(OptionsManager* my_options_mgr) {
    cur_options_mgr_ = my_options_mgr;
}

Processor::~Processor() {

}

void Processor::convert_data(HkFileL1Conv& hkfile_l1_conv) {
    int pre_percent = 0;
    int cur_percent = 0;
    cout << "Converting data file: " << cur_options_mgr_->in_file.Data() << " ... " << endl;
    cout << "[ " << flush;
    hkfile_l1_conv.hk_obox_set_start();
    while (hkfile_l1_conv.hk_obox_next()) {
        cur_percent = static_cast<int>(100 * hkfile_l1_conv.hk_obox_get_cur_entry() / hkfile_l1_conv.hk_obox_get_tot_entries());
        if (cur_percent - pre_percent > 0 && cur_percent % 2 == 0) {
            pre_percent = cur_percent;
            cout << "#" << flush;
        }

        hkfile_l1_conv.pol_hk_fill();
        
    }
    cout << " DONE ] " << endl;
}

void Processor::write_meta_info(HkFileL1Conv& hkfile_l1_conv) {
    // dattype
    hkfile_l1_conv.pol_hk_write_meta("m_dattype", "POLAR 1 LEVEL HK DECODED DATA");
    // version
    hkfile_l1_conv.pol_hk_write_meta("m_version", (SW_NAME + " " + SW_VERSION).c_str());
    // gentime
    TTimeStamp* cur_time = new TTimeStamp();
    hkfile_l1_conv.pol_hk_write_meta("m_gentime", cur_time->AsString("lc"));
    delete cur_time;
    cur_time = NULL;
    // dcdfile
    TSystem sys;
    hkfile_l1_conv.pol_hk_write_meta("m_dcdfile", sys.BaseName(cur_options_mgr_->in_file.Data()));
    // gps span
    hkfile_l1_conv.pol_hk_write_gps_span();
    // extra info
    hkfile_l1_conv.pol_hk_write_extra_info();
}
