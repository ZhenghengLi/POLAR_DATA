#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <iostream>
#include <stdint.h>

using namespace std;

const string SW_NAME = "nonlin_calc";

const string SW_VERSION = "v1.0.0";

const string RELEASE_DATE = "2017 Dec 05";

// mean Compton Edge of bank20_n7 in module level
const double norm_pos[25] = {2570, 2278, 2248, 2099, 2446,
                             2355, 2392, 2053, 2640, 2322,
                             2634, 1901, 2115, 2039, 2461,
                             1953, 2663, 2303, 2432, 2002,
                             2386, 2648, 2831, 1983, 2503
                            };

#endif
