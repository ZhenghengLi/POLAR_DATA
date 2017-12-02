#ifndef EventFilter_HH
#define EventFilter_HH

#include <iostream>
#include "RootInc.hpp"
#include "POLEvent.hpp"

using namespace std;

class EventFilter {
public:
    static const UShort_t TOO_LOW     = 1;
    static const UShort_t POST_COSMIC = 2;
    static const UShort_t COSMIC      = 4;
    static const UShort_t TOO_MANY    = 8;
    static const UShort_t TOO_SHORT   = 16;

private:
    double too_low_cut_;
    int    too_many_cut_1_;
    int    too_many_cut_2_;
    double too_short_cut_;
    double time_wait_cut_;

private:
    bool   pre_is_cosmic_[25];
    double cosmic_event_time_[25];

private:
    double bar_timewait_cache_[4][25][64];
    bool   bar_trigger_first_[25][64];
    double bar_pre_event_time_[25][64];

private:
    UShort_t check_too_low_(const POLEvent& pol_event);
    UShort_t check_post_cosmic_(const POLEvent& pol_event);
    UShort_t check_too_many_(const POLEvent& pol_event);
    UShort_t check_too_short_(const POLEvent& pol_event);

public:
    bool  cur_time_aligned[25];
    float cur_mod_maxadcdm[25];

public:
    EventFilter();
    ~EventFilter();

    void set_too_low_cut(double cut_value);

    UShort_t classify(const POLEvent& pol_event);

    bool is_bad_event(const POLEvent& pol_event);

};

#endif
