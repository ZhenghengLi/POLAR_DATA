#ifndef PPDITERATOR_H
#define PPDITERATOR_H

#include <iostream>
#include <boost/regex.hpp>
#include "RootInc.hpp"

using namespace std;
using boost::regex;
using boost::cmatch;
using boost::regex_match;

class PPDIterator {
public:
    struct PPD_T {
        Double_t     ship_time_sec;
        Double_t     utc_time_sec;
        Int_t        flag_of_pos;
        Double_t     wgs84_x;
        Double_t     wgs84_y;
        Double_t     wgs84_z;
        Double_t     det_z_ra;
        Double_t     det_z_dec;
        Double_t     det_x_ra;
        Double_t     det_x_dec;
        Double_t     earth_ra;
        Double_t     earth_dec;
        Double_t     sun_ra;
        Double_t     sun_dec;
    };

private:
    double wgs84_x_slope_;
    double wgs84_y_slope_;
    double wgs84_z_slope_;
    double det_z_ra_slope_;
    double det_z_dec_slope_;
    double det_x_ra_slope_;
    double det_x_dec_slope_;
    double earth_ra_slope_;
    double earth_dec_slope_;
    double sun_ra_slope_;
    double sun_dec_slope_;

private:
    TFile* t_file_in_;
    TTree* t_ppd_tree_;

    regex  re_ship_span_;
    double first_ship_second_;
    double last_ship_second_;

    Long64_t ppd_cur_entry_;
    bool     ppd_reach_end_;

    PPD_T cur_ppd_;

public:
    PPD_T ppd_before;
    PPD_T ppd_after;
    PPD_T ppd_interm;

public:
    PPDIterator();
    ~PPDIterator();

    bool open(const char* filename);
    void close();

    bool next_ppd();
    bool get_reach_end();
    void calc_ppd_interm(double ship_time);

    double get_first_ship_second();
    double get_last_ship_second();

};

#endif
