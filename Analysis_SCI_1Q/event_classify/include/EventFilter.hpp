#ifndef EventFilter_HH
#define EventFilter_HH

#include <iostream>
#include "RootInc.hpp"
#include "POLEvent.hpp"

using namespace std;

class EventFilter {
public:
    static const UShort_t Type_1 = 0x1;

public:
    EventFilter();
    ~EventFilter();

    UShort_t classify(const POLEvent& pol_event);

};

#endif
