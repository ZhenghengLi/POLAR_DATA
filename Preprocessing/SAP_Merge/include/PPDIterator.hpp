#ifndef PPDITERATOR_H
#define PPDITERATOR_H

#include <iostream>
#include <cmath>
#include <boost/regex.hpp>
#include "RootInc.hpp"
#include "Constants.hpp"

using namespace std;
using boost::regex;
using boost::cmatch;
using boost::regex_match;

class PPDIterator {
public:
    struct PPD_T {
        double       gps_time_sec;
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
    double det_z_dec_slope_;
    double det_x_dec_slope_;
    double earth_dec_slope_;
    double sun_dec_slope_;

private:
    TFile* t_file_in_;
    TTree* t_ppd_tree_;

    regex  re_gps_span_;
    double first_gps_time_;
    double last_gps_time_;

    Long64_t ppd_cur_entry_;
    bool     ppd_reach_end_;

    PPD_T cur_ppd_;

public:
    PPD_T ppd_before;
    PPD_T ppd_after;
    PPD_T ppd_interm;

private:
    double calc_ra_(
            double gps_time,
            double before_ra,
            double before_gps_time,
            double after_ra,
            double after_gps_time);
    double get_leap_seconds_(double utc_time_sec);
    double utc_to_gps_(double utc_time_sec);

public:
    PPDIterator();
    ~PPDIterator();

    bool open(const char* filename);
    void close();

    bool next_ppd();
    bool get_reach_end();
    void calc_ppd_interm(double gps_time);

    double get_first_gps_time();
    double get_last_gps_time();

};

#endif
