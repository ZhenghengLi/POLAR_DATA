#include "EventFilter.hpp"

// intialize cut constant
const double EventFilter::too_low_cut_    = 100.0;
const int    EventFilter::too_many_cut_1_ = 15;
const int    EventFilter::too_many_cut_2_ = 35;
const double EventFilter::too_short_cut_  = 0.01;

EventFilter::EventFilter() {
    for (int i = 0; i < 25; i++) {
        for (int j = 0; j < 64; j++) {
            for (int k = 0; k < 4; k++) {
                bar_timewait_cache_[k][i][j] = 10.0;
            }
            bar_trigger_first_[i][j] = true;
            bar_pre_event_time_[i][j] = 0.0;
        }
    }
}

EventFilter::~EventFilter() {

}

UShort_t EventFilter::check_too_low_(const POLEvent& pol_event) {
    for (int i = 0; i < 25; i++) {
        if (!pol_event.time_aligned[i]) continue;

        double trig_sum = 0;
        int    trig_n = 0;
        double trig_mean = 0;
        double nontrig_sum = 0;
        int    nontrig_n = 0;
        double nontrig_mean = 0;

        for (int j = 0; j < 64; j++) {
            if (pol_event.trigger_bit[i][j]) {
                trig_sum += pol_event.energy_value[i][j];
                trig_n++;
            } else {
                if (pol_event.channel_status[i][j] == 0 && pol_event.energy_value[i][j] > -30.0) {
                    nontrig_sum += pol_event.energy_value[i][j];
                    nontrig_n++;
                }
            }
        }

        trig_mean = (trig_n > 0 ? trig_sum / trig_n : 0);
        nontrig_mean = (nontrig_n > 0 ? nontrig_sum / nontrig_n : 0);

        if (trig_mean - nontrig_mean < too_low_cut_) {
            return TOO_LOW;
        }
    }

    return 0;
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
    UShort_t too_many_res = check_too_many_(pol_event);
    UShort_t too_short_res = check_too_short_(pol_event);

    UShort_t final_res = (too_low_res | too_many_res | too_short_res);

    return final_res;
}

bool EventFilter::is_bad_event(const POLEvent& pol_event) {
    return classify(pol_event);
}
