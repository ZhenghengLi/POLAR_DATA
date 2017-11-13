#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <iostream>
#include <stdint.h>

using namespace std;

const string SW_NAME = "event_classify";

const string SW_VERSION = "v1.0.0";

const string RELEASE_DATE = "2017 Jun 23";

// const double mod_adc_cut[25] = {110, 200, 160, 200, 150,
//                                 120, 150, 145, 120, 140,
//                                 145, 260, 250, 245, 124,
//                                 250, 171, 140, 225, 220,
//                                 180, 150, 105, 200, 110};

const double mod_adc_cut[25] = {110, 130, 120, 120, 100,
                                150, 100, 100, 150, 140,
                                 70, 100, 100, 100, 100,
                                120, 150, 100, 140, 130,
                                120, 150,  70, 150, 150};

#endif
