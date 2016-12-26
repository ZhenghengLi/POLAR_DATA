#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <stdint.h>

using namespace std;

const int ModuleIndex[25] = {405, 639, 415, 522, 424,
                             640, 408, 638, 441, 631,
                             411, 505, 503, 509, 410,
                             507, 402, 602, 414, 524,
                             423, 601, 406, 520, 413
};

const int TriggerIndex[64] = {23, 15, 31,  7, 22, 14, 30,  6,
                              21, 13, 29,  5, 20, 12, 28,  4,
                              27,  3, 19, 11, 26,  2, 18, 10,
                              25,  1, 17,  9, 24,  0, 16,  8,
                              48, 40, 56, 32, 49, 41, 57, 33,
                              50, 42, 58, 34, 59, 35, 51, 43,
                              60, 36, 52, 44, 61, 37, 53, 45,
                              62, 38, 54, 46, 63, 39, 55, 47
};

//#define ESRF
#ifdef ESRF
const int CircleTime = 2097152;
const int PedSecond = 1;
const int PedCircle = 6104;
const int LSB_Value = 2048;
const int TriggerShiftRight = 11;
const uint32_t EventTimeMask = 0x1FFFFF;
#else
const int CircleTime = 8388608;
const int PedSecond = 1;
const int PedCircle = 24414;
const int LSB_Value = 512;
const int TriggerShiftRight = 9;
const uint32_t EventTimeMask = 0x7FFFFF;
#endif

const string SW_NAME = "SCI_Decode";

const string SW_VERSION = "v1.1.0";

const string RELEASE_DATE = "2016 Dec 26";

#endif
