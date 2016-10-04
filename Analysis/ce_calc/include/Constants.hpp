#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <stdint.h>

using namespace std;

const string SW_NAME = "ce_calc";

const string SW_VERSION = "v1.0.0";

const string RELEASE_DATE = "2016 Jul 13";

const double BarD = 6.080;
const double BarW = 5.850;
const double BarH = 176.0;
const double ModD = 60.00;
const double CarG = 2.045;
const double SrcL = 2.000;
const double SrcP = 0.800;

const double SourcePos[4][2] = {
    { 3 * ModD + 7 * BarD + 1 * BarW + CarG + SrcP, 3 * ModD + 7 * BarD + 1 * BarW + CarG - SrcL },
    { 1 * ModD + 0 * BarD + 0 * BarW - CarG - SrcP, 3 * ModD + 7 * BarD + 1 * BarW + CarG - SrcL },
    { 1 * ModD + 0 * BarD + 0 * BarW - CarG - SrcP, 1 * ModD + 0 * BarD + 0 * BarW - CarG + SrcL },
    { 3 * ModD + 7 * BarD + 1 * BarW + CarG - SrcL, 1 * ModD + 0 * BarD + 0 * BarW - CarG - SrcP }
};

const double AngleMin = (150.0 / 180.0 * 3.1415926);

const double DistanceMax = (1.3 * BarD);

const float CE_Na22  = 340.667;
const float CE_Cs137 = 477.650;

#endif
