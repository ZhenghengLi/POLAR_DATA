#ifndef SCIITERATOR_H
#define SCIITERATOR_H

#include "SciType.hpp"

class SciIterator: private SciType {
private:
    bool   cur_is_1P_;

public:
    SciIterator();
    ~SciIterator();

};

#endif
