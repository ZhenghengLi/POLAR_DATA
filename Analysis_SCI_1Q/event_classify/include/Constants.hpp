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

const double mod_adc_cut[25] = {130, 170, 120, 130, 120,
                                150, 130, 140, 150, 100,
                                 40, 130, 130, 110, 130,
                                100, 150, 100, 180, 110,
                                130, 140, 110, 100, 120};

#endif
