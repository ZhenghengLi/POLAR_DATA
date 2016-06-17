#ifndef EVENTITERATOR_H
#define EVENTITERATOR_H

#include "SciType.hpp"

class EventIterator: private SciType {
private:
    bool   cur_is_1P_;

public:
    EventIterator();
    ~EventIterator();

};

#endif
