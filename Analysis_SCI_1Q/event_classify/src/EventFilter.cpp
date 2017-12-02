#include "EventFilter.hpp"
#include "Constants.hpp"

EventFilter::EventFilter() {

    // set default cut value
    too_low_cut_    = 70;
    too_many_cut_1_ = 15;
    too_many_cut_2_ = 35;
    too_short_cut_  = 1E-3;
    time_wait_cut_  = 100E-6;

    // init other values
    for (int i = 0; i < 25; i++) {
        for (int j = 0; j < 64; j++) {
            for (int k = 0; k < 4; k++) {
                bar_timewait_cache_[k][i][j] = 10.0;
            }
            bar_trigger_first_[i][j] = true;
            bar_pre_event_time_[i][j] = 0.0;
        }
    }
    for (int i = 0; i < 25; i++) {
        pre_is_cosmic_[i] = false;
        cosmic_event_time_[i] = 0;
    }

}

EventFilter::~EventFilter() {

}

void EventFilter::set_too_low_cut(double cut_value) {
    too_low_cut_ = cut_value;
}

UShort_t EventFilter::check_post_cosmic_(const POLEvent& pol_event) {
    bool is_post_cosmic = false;
    for (int i = 0; i < 25; i++) {
        if (pol_event.time_aligned[i]) {
            if (pre_is_cosmic_[i] && pol_event.event_time - cosmic_event_time_[i] < time_wait_cut_) {
                is_post_cosmic = true;
                cosmic_event_time_[i] = pol_event.event_time;
            } else {
                pre_is_cosmic_[i] = (pol_event.t_out_too_many[i] || pol_event.dy12_too_high[i]);
                cosmic_event_time_[i] = pol_event.event_time;
            }
        }
    }
    if (is_post_cosmic) {
        return POST_COSMIC;
    } else {
        return 0;
    }
}

UShort_t EventFilter::check_too_low_(const POLEvent& pol_event) {
    bool is_too_low = false;
    for (int i = 0; i < 25; i++) {
        if (pol_event.time_aligned[i]) {
            cur_time_aligned[i] = true;
        } else {
            cur_time_aligned[i] = false;
            cur_mod_maxadcdm[i] = 4096;
            continue;
        }

        double trig_max = -4096;
        int    trig_n  = 0;
        // int    max_j   = 0;

        for (int j = 0; j < 64; j++) {
            if (pol_event.trigger_bit[i][j]) {
                trig_n++;
                if (pol_event.energy_value[i][j] > trig_max) {
                    trig_max = pol_event.energy_value[i][j];
                    // max_j = j;
                }
            }
        }

        cur_mod_maxadcdm[i] = (trig_n > 0 ? trig_max / trig_n : 0);
        if (cur_mod_maxadcdm[i] < too_low_cut_) is_too_low = true;

        // if (pol_event.channel_status[i][max_j] & POLEvent::ADC_OVERFLOW) {
        //     cur_mod_maxadcdm[i] = 4096;
        // }


    }

    if (is_too_low) {
        return TOO_LOW;
    } else {
        return 0;
    }

}


UShort_t EventFilter::check_too_many_(const POLEvent& pol_event) {
    if (pol_event.trigger_n > too_many_cut_2_) {
        return TOO_MANY;
    }
    for (int i = 0; i < 25; i++) {
        if (pol_event.multiplicity[i] > too_many_cut_1_) {
            return TOO_MANY;
        }
    }
    return 0;
}

UShort_t EventFilter::check_too_short_(const POLEvent& pol_event) {
    for (int i = 0; i < 25; i++) {
        if (!pol_event.time_aligned[i]) continue;
        for (int j = 0; j < 64; j++) {
            if (pol_event.trigger_bit[i][j]) {
                if (bar_trigger_first_[i][j]) {
                    bar_trigger_first_[i][j] = false;
                    bar_pre_event_time_[i][j] = pol_event.event_time;
                    continue;
                }
                for (int k = 0; k < 3; k++) {
                    bar_timewait_cache_[k][i][j] = bar_timewait_cache_[k + 1][i][j];
                }
                bar_timewait_cache_[3][i][j] = pol_event.event_time - bar_pre_event_time_[i][j];
                bar_pre_event_time_[i][j] = pol_event.event_time;
                if (bar_timewait_cache_[0][i][j] < too_short_cut_ &&
                    bar_timewait_cache_[1][i][j] < too_short_cut_ &&
                    bar_timewait_cache_[2][i][j] < too_short_cut_ &&
                    bar_timewait_cache_[3][i][j] < too_short_cut_) {
                    return TOO_SHORT;
                }
            }
        }
    }

    return 0;
}

UShort_t EventFilter::classify(const POLEvent& pol_event) {
    if (pol_event.is_ped) return 0;

    UShort_t too_low_res = check_too_low_(pol_event);
    UShort_t post_cosmic_res = check_post_cosmic_(pol_event);
    UShort_t cosmic_res = (pol_event.type == 0xFF00 ? COSMIC : 0);

    UShort_t too_many_res = check_too_many_(pol_event);
    UShort_t too_short_res = check_too_short_(pol_event);

    UShort_t final_res = (too_low_res | post_cosmic_res | cosmic_res); // | too_many_res | too_short_res);

    return final_res;
}

bool EventFilter::is_bad_event(const POLEvent& pol_event) {
    return classify(pol_event);
}
