#include <iostream>
#include "OptionsManager.hpp"
#include "RootInc.hpp"
#include "EventIterator.hpp"
#include "RateCanvas.hpp"

using namespace std;

int main(int argc, char** argv) {
    OptionsManager options_mgr;
    if (!options_mgr.parse(argc, argv)) {
        if (options_mgr.get_version_flag()) {
            options_mgr.print_version();
        } else {
            options_mgr.print_help();
        }
        return 1;
    }

    argc = 1;
    TApplication* rootapp = new TApplication("POLAR", &argc, argv);

    EventIterator eventIter;
    if (!eventIter.open(options_mgr.decoded_data_filename.Data(),
                      options_mgr.begin_gps.Data(), options_mgr.end_gps.Data())) {
        cerr << "root file open failed: " << options_mgr.decoded_data_filename.Data() << endl;
        return 1;
    }

    if (!eventIter.file_is_1P()) {
        cout << "error: The opened file may be not 1P/1R SCI data file." << endl;
        eventIter.close();
        return 1;
    }
    
    eventIter.print_file_info();

    Double_t gps_time_length = (eventIter.phy_end_trigger.abs_gps_week - eventIter.ped_begin_trigger.abs_gps_week) * 604800 +
        (eventIter.phy_end_trigger.abs_gps_second - eventIter.phy_begin_trigger.abs_gps_second);
    int nbins = gps_time_length / options_mgr.binwidth;
    cout << "----------------------------------------------------------" << endl;
    cout << " - time length:     " << gps_time_length << " seconds" << endl;
    cout << " - bin width:       " << options_mgr.binwidth << " seconds" << endl;
    cout << " - number of bins:  " << nbins << endl;
    cout << " - phase shift:     " << options_mgr.phase << "/4" << endl;
    cout << "----------------------------------------------------------" << endl;

    RateCanvas rate_canvas(eventIter.phy_begin_trigger.abs_gps_week, eventIter.phy_begin_trigger.abs_gps_second);
    
    // prepare histogram
    char name[50];
    char title[100];
    sprintf(title, "%d:%d => %d:%d @ %.3f, %d/4",
            static_cast<int>(eventIter.phy_begin_trigger.abs_gps_week),
            static_cast<int>(eventIter.phy_begin_trigger.abs_gps_second),
            static_cast<int>(eventIter.phy_end_trigger.abs_gps_week),
            static_cast<int>(eventIter.phy_end_trigger.abs_gps_second),
            static_cast<float>(options_mgr.binwidth),
            static_cast<int>(options_mgr.phase));
    string gps_time_span = title;
    TH1D* trigger_hist = new TH1D("trigger_hist", (string("trigger: { ") + gps_time_span + string(" }")).c_str(),
                                   nbins, 0 + options_mgr.phase * options_mgr.binwidth / 4, gps_time_length + options_mgr.phase * options_mgr.binwidth / 4);
    TH1D* trigger_hist_int = new TH1D("trigger_hist_int", (string("trigger: { ") + gps_time_span + string(" }")).c_str(),
                                      nbins, 0 + options_mgr.phase * options_mgr.binwidth / 4, gps_time_length + options_mgr.phase * options_mgr.binwidth / 4);
    trigger_hist->SetDirectory(NULL);
    trigger_hist->SetMinimum(0);
    trigger_hist_int->SetDirectory(NULL);
    trigger_hist_int->SetLineColor(6);
    TH1D* modules_hist[25];
    TH1D* modules_hist_tout1[25];
    for (int i = 0; i < 25; i++) {
        sprintf(name, "module_hist_%d", i + 1);
        sprintf(title, "module CT_%d: { %s }", i + 1, gps_time_span.c_str());
        modules_hist[i] = new TH1D(name, title,
                                   nbins, 0 + options_mgr.phase * options_mgr.binwidth / 4, gps_time_length + options_mgr.phase * options_mgr.binwidth / 4);
        modules_hist[i]->SetDirectory(NULL);
        modules_hist[i]->SetLineColor(kGreen);
        modules_hist[i]->SetMinimum(0);
        if (!options_mgr.tout1_flag) {
            continue;
        }
        sprintf(name, "module_hist_tout1_%d", i + 1);
        sprintf(title, "module CT_%d: { %s }, TOUT1", i + 1, gps_time_span.c_str());
        modules_hist_tout1[i] = new TH1D(name, title,
                                         nbins, 0 + options_mgr.phase * options_mgr.binwidth / 4, gps_time_length + options_mgr.phase * options_mgr.binwidth / 4);
        modules_hist_tout1[i]->SetDirectory(NULL);
        modules_hist_tout1[i]->SetLineColor(kRed);
        modules_hist_tout1[i]->SetMinimum(0);
    }
    
    int pre_percent = 0;
    int cur_percent = 0;
    cout << "reading trigger data ... " << endl;
    cout << "[ " << flush;
    eventIter.phy_trigger_set_start();
    while (eventIter.phy_trigger_next_event()) {
        cur_percent = static_cast<int>(100 * eventIter.phy_trigger_get_cur_entry() / eventIter.phy_trigger_get_tot_entries());
        if (cur_percent - pre_percent > 0 && cur_percent % 2 == 0) {
            pre_percent = cur_percent;
            cout << "#" << flush;
        }
        if (!eventIter.t_trigger.abs_gps_valid) {
            continue;
        }
        if (eventIter.t_trigger.trigger_n > 4) {
            continue;
        }
        trigger_hist->Fill((eventIter.t_trigger.abs_gps_week   - eventIter.phy_begin_trigger.abs_gps_week) * 604800 +
                           (eventIter.t_trigger.abs_gps_second - eventIter.phy_begin_trigger.abs_gps_second));
        for (int i = 0; i < 25; i++) {
            if (eventIter.t_trigger.trig_accepted[i]) {
                modules_hist[i]->Fill((eventIter.t_trigger.abs_gps_week   - eventIter.phy_begin_trigger.abs_gps_week) * 604800 +
                                      (eventIter.t_trigger.abs_gps_second - eventIter.phy_begin_trigger.abs_gps_second));
            }
        }
        if (!options_mgr.tout1_flag) {
            continue;
        }
        for (int i = 0; i < 25; i++) {
            if (eventIter.t_trigger.trig_accepted[i]) {
                modules_hist_tout1[i]->Fill((eventIter.t_trigger.abs_gps_week   - eventIter.phy_begin_trigger.abs_gps_week) * 604800 +
                                            (eventIter.t_trigger.abs_gps_second - eventIter.phy_begin_trigger.abs_gps_second));
            }
        }
        while (eventIter.phy_modules_next_packet()) {
            int idx = eventIter.t_modules.ct_num - 1;
            for (int i = 0; i < eventIter.t_modules.raw_rate; i++) {
                modules_hist_tout1[idx]->Fill((eventIter.t_trigger.abs_gps_week   - eventIter.phy_begin_trigger.abs_gps_week) * 604800 +
                                              (eventIter.t_trigger.abs_gps_second - eventIter.phy_begin_trigger.abs_gps_second));
            }
        }
    }
    cout << " DONE ]" << endl;
    eventIter.close();

    double sum = 0;
    for (int i = 0; i < nbins; i++) {
        sum += trigger_hist->GetBinContent(i + 1);
        trigger_hist_int->SetBinContent(i + 1, sum);
        trigger_hist->SetBinContent(i + 1, trigger_hist->GetBinContent(i + 1) / options_mgr.binwidth);
        trigger_hist->SetBinError(i + 1, trigger_hist->GetBinError(i + 1) / options_mgr.binwidth);
        for (int j = 0; j < 25; j++) {
            modules_hist[j]->SetBinContent(i + 1, modules_hist[j]->GetBinContent(i + 1) / options_mgr.binwidth);
            modules_hist[j]->SetBinError(i + 1, modules_hist[j]->GetBinError(i + 1) / options_mgr.binwidth);
        }
    }

    if (options_mgr.tout1_flag) {
        for (int i = 0; i < nbins; i++) {
            for (int j = 0; j < 25; j++) {
                modules_hist_tout1[j]->SetBinContent(i + 1, modules_hist_tout1[j]->GetBinContent(i + 1) / options_mgr.binwidth);
                modules_hist_tout1[j]->SetBinError(i + 1, modules_hist_tout1[j]->GetBinError(i + 1) / options_mgr.binwidth);
            }
        }
    }

    cout << " - drawing ... " << endl;
    gStyle->SetOptStat(0);
    double y_max = 0;
    for (int i = 0; i < 25; i++) {
        if (modules_hist[i]->GetMaximum() > y_max) {
            y_max = modules_hist[i]->GetMaximum();
        }
    }
    for (int i = 0; i < 25; i++) {
        modules_hist[i]->SetMaximum(y_max * 1.1);
    }
    for (int i = 0; i < 25; i++) {
        rate_canvas.cd_modules(i);
        modules_hist[i]->Draw("EH");
    }

    if (options_mgr.tout1_flag) {
        y_max = 0;
        for (int i = 0; i < 25; i++) {
            if (modules_hist_tout1[i]->GetMaximum() > y_max) {
                y_max = modules_hist_tout1[i]->GetMaximum();
            }
        }
        for (int i = 0; i < 25; i++) {
            modules_hist_tout1[i]->SetMaximum(y_max * 1.1);
        }
        for (int i = 0; i < 25; i++) {
            rate_canvas.cd_modules_tout1(i);
            modules_hist_tout1[i]->Draw("EH");
        }
    }

    rate_canvas.cd_trigger();
    trigger_hist->Draw("EH");

    rate_canvas.draw_hist_int(trigger_hist_int);

    rootapp->Run();
    
    return 0;
}
