#ifndef EventFilter_HH
#define EventFilter_HH

#include <iostream>
#include "RootInc.hpp"
#include "POLEvent.hpp"

using namespace std;

class EventFilter {
public:
    static const UShort_t TOO_LOW   = 0x1;
    static const UShort_t TOO_MANY  = 0x2;
    static const UShort_t TOO_SHORT = 0x4;

private:
    static const double too_low_cut_;
    static const int    too_many_cut_1_;
    static const int    too_many_cut_2_;
    static const double too_short_cut_;

private:
    double bar_timewait_cache_[4][25][64];
    bool   bar_trigger_first_[25][64];
    double bar_pre_event_time_[25][64];

private:
    UShort_t check_too_low_(const POLEvent& pol_event);
    UShort_t check_too_many_(const POLEvent& pol_event);
    UShort_t check_too_short_(const POLEvent& pol_event);

public:
    EventFilter();
    ~EventFilter();

    UShort_t classify(const POLEvent& pol_event);

    bool is_bad_event(const POLEvent& pol_event);

};

#endif
