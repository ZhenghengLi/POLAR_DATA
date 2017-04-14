#ifndef DETINFO_H
#define DETINFO_H

const double BarD = 6.080;
const double BarW = 5.800;
const double BarH = 176.0;
const double ModD = 60.00;
const double CarG = 2.070;
const double SrcL = 1.500;
const double SrcP = 0.800;

const double SourcePos[4][2] = {
    { 3 * ModD + 7 * BarD + 1 * BarW + CarG + SrcP, 3 * ModD + 7 * BarD + 1 * BarW + CarG - SrcL },
    { 1 * ModD + 0 * BarD + 0 * BarW - CarG - SrcP, 3 * ModD + 7 * BarD + 1 * BarW + CarG + SrcP },
    { 1 * ModD + 0 * BarD + 0 * BarW - CarG - SrcP, 1 * ModD + 0 * BarD + 0 * BarW - CarG + SrcL },
    { 3 * ModD + 7 * BarD + 1 * BarW + CarG - SrcL, 1 * ModD + 0 * BarD + 0 * BarW - CarG - SrcP }
};

#endif
