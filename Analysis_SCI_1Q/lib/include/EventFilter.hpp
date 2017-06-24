#ifndef EventFilter_HH
#define EventFilter_HH

#include <iostream>
#include "RootInc.hpp"
#include "POLEvent.hpp"

using namespace std;

class EventFilter {
public:
    static const UShort_t TOO_LOW  = 0x1;
    static const UShort_t TOO_MANY = 0x2;

private:
    static const UShort_t too_low_cut_ = 100.0;
    static const UShort_t too_many_cut_1_ = 27;
    static const UShort_t too_many_cut_2_ = 54;

private:
    UShort_t check_too_low_(const POLEvent& pol_event);
    UShort_t check_too_many_(const POLEvent& pol_event);

public:
    EventFilter();
    ~EventFilter();

    UShort_t classify(const POLEvent& pol_event);

    bool is_bad_event(const POLEvent& pol_event);

};

#endif
