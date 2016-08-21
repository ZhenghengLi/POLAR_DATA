#include "Processor.hpp"

using namespace std;

Processor::Processor(OptionsManager* my_options_mgr) {
    cur_options_mgr_ = my_options_mgr;
}

Processor::~Processor() {

}

void Processor::convert_data(SciFile1RR& scifile_1r, SciFileL1W& scifile_l1) {
    int pre_percent = 0;
    int cur_percent = 0;
    cout << "Converting data file: " << cur_options_mgr_->in_file.Data() << " ... " << endl;
    cout << "[ " << flush;
    scifile_1r.trigger_set_start();
    while (scifile_1r.trigger_next_event()) {
        cur_percent = static_cast<int>(100 * scifile_1r.trigger_get_cur_entry() / scifile_1r.trigger_get_tot_entries());
        if (cur_percent - pre_percent > 0 && cur_percent % 2 == 0) {
            pre_percent = cur_percent;
            cout << "#" << flush;
        }
        
        scifile_l1.t_pol_event.abs_gps_week             = scifile_1r.t_triggerl1.abs_gps_week;
        scifile_l1.t_pol_event.abs_gps_second           = scifile_1r.t_triggerl1.abs_gps_second;
        scifile_l1.t_pol_event.abs_gps_valid            = scifile_1r.t_triggerl1.abs_gps_valid;
        scifile_l1.t_pol_event.abs_ship_second          = scifile_1r.t_triggerl1.abs_ship_second;
        scifile_l1.t_pol_event.type                     = scifile_1r.t_triggerl1.type;
        scifile_l1.t_pol_event.pkt_count                = scifile_1r.t_triggerl1.pkt_count;
        scifile_l1.t_pol_event.lost_count               = scifile_1r.t_triggerl1.lost_count;
        scifile_l1.t_pol_event.trigger_n                = scifile_1r.t_triggerl1.trigger_n;
        for (int i = 0; i < 25; i++) {
            scifile_l1.t_pol_event.trig_accepted[i]     = scifile_1r.t_triggerl1.trig_accepted[i];
            scifile_l1.t_pol_event.time_aligned[i]      = false;
            scifile_l1.t_pol_event.multiplicity[i]      = 0;
            scifile_l1.t_pol_event.compress[i]          = -1;
            scifile_l1.t_pol_event.common_noise[i]      = 8192;
        }
        for (int k = 0; k < 1600; k++) {
            scifile_l1.t_pol_event.trigger_bit[k]       = false;
            scifile_l1.t_pol_event.energy_adc[k]        = 0;
        }
        
        while (scifile_1r.modules_next_packet()) {
            if (!scifile_1r.modules_cur_matched()) {
                scifile_l1.t_pol_event.lost_count      += 1;
                scifile_l1.t_pol_event.pkt_count       -= 1;
                continue;
            }
            int idx                                     = scifile_1r.t_modulesl1.ct_num - 1;
            scifile_l1.t_pol_event.time_aligned[idx]    = true;
            scifile_l1.t_pol_event.multiplicity[idx]    = scifile_1r.t_modulesl1.multiplicity;
            scifile_l1.t_pol_event.compress[idx]        = scifile_1r.t_modulesl1.compress;
            scifile_l1.t_pol_event.common_noise[idx]    = scifile_1r.t_modulesl1.common_noise;
            for (int j = 0; j < 64; j++) {
                int k = idx * 64 + j;
                scifile_l1.t_pol_event.trigger_bit[k]   = scifile_1r.t_modulesl1.trigger_bit[j];
                scifile_l1.t_pol_event.energy_adc[k]    = scifile_1r.t_modulesl1.energy_adc[j];
            }
        }
        scifile_l1.fill_event();
    }
    cout << " DONE ] " << endl;
}

void Processor::write_meta_info(SciFile1RR& scifile_1r, SciFileL1W& scifile_l1) {
    // dattype
    scifile_l1.write_meta("m_dattype", "POLAR 1 LEVEL SCI DECODED DATA");
    // version
    scifile_l1.write_meta("m_version", (SW_NAME + " " + SW_VERSION).c_str());
    // gentime
    TTimeStamp* cur_time = new TTimeStamp();
    scifile_l1.write_meta("m_gentime", cur_time->AsString("lc"));
    delete cur_time;
    cur_time = NULL;
    // dcdfile
    TSystem sys;
    scifile_l1.write_meta("m_dcdfile", sys.BaseName(cur_options_mgr_->in_file.Data()));
    // gps span
    scifile_l1.write_gps_span();
    // align info
    scifile_l1.write_meta("m_algninf", scifile_1r.get_align_info_str().c_str());
}
