#include <iostream>
#include <fstream>
#include <sstream>
#include "OptionsManager.hpp"
#include "RootInc.hpp"
#include "EventIterator.hpp"
#include "RateCanvas.hpp"
#include "CooConv.hpp"

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

    bool bar_mask[25][64];
    for (int i = 0; i < 25; i++) {
        for (int j = 0; j < 64; j++) {
            bar_mask[i][j] = false;
        }
    }
    bool type_mask[3];
    for (int i = 0; i < 3; i++) {
        type_mask[i] = false;
    }

    // read bar_mask
    char line_buffer[100];
    ifstream infile;
    if (!options_mgr.bar_mask_filename.IsNull()) {
        infile.open(options_mgr.bar_mask_filename.Data());
        if (!infile.is_open()) {
            cout << "bar_mask_file open failed." << endl;
            return 1;
        }
    }
    stringstream ss;
    int ct_num;
    int ch_idx;
    while (!options_mgr.bar_mask_filename.IsNull()) {
        infile.getline(line_buffer, 100);
        if (infile.eof()) break;
        if (string(line_buffer).find("#") != string::npos) {
            continue;
        } else if (string(line_buffer).find("single") != string::npos) {
            type_mask[0] = true;
            cout << "kill all single event" << endl;
        } else if (string(line_buffer).find("normal") != string::npos) {
            type_mask[1] = true;
            cout << "kill all normal event" << endl;
        } else if (string(line_buffer).find("cosmic") != string::npos) {
            type_mask[2] = true;
            cout << "kill all cosmic event" << endl;
        } else {
            ss.clear();
            ss.str(line_buffer);
            ss >> ct_num >> ch_idx;
            if (ct_num < 1 || ct_num > 25) {
                cout << "ct_num out of range" << endl;
                return 1;
            }
            if (ch_idx < 0 || ch_idx > 63) {
                cout << "ch_idx out of range" << endl;
                return 1;
            }
            cout << "kill bar ct_" << ct_num << ", ch_" << ch_idx << endl;
            bar_mask[ct_num - 1][ch_idx] = true;
        }
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

    Double_t gps_time_length = (eventIter.phy_end_trigger.abs_gps_week - eventIter.phy_begin_trigger.abs_gps_week) * 604800 +
        (eventIter.phy_end_trigger.abs_gps_second - eventIter.phy_begin_trigger.abs_gps_second);
    int nbins = gps_time_length / options_mgr.binwidth;
    cout << "----------------------------------------------------------" << endl;
    cout << " - time length:     " << gps_time_length << " seconds" << endl;
    cout << " - bin width:       " << options_mgr.binwidth << " seconds" << endl;
    cout << " - number of bins:  " << nbins << endl;
    cout << " - phase shift:     " << options_mgr.phase << "/4" << endl;
    cout << "----------------------------------------------------------" << endl;

    RateCanvas rate_canvas(eventIter.phy_begin_trigger.abs_gps_week, eventIter.phy_begin_trigger.abs_gps_second, options_mgr.min_signif);

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
    trigger_hist->SetDirectory(NULL);
    trigger_hist->SetMinimum(0);
    trigger_hist->GetXaxis()->SetTitle("T-T0 (s)");
    trigger_hist->GetYaxis()->SetTitle("Rate (trigger/s)");
    TH1D* modules_hist[25];
    TH1D* modules_hist_tout1[25];
    for (int i = 0; i < 25; i++) {
        sprintf(name, "module_hist_%02d", i + 1);
        sprintf(title, "module CT_%02d: { %s }", i + 1, gps_time_span.c_str());
        modules_hist[i] = new TH1D(name, title,
                                   nbins, 0 + options_mgr.phase * options_mgr.binwidth / 4, gps_time_length + options_mgr.phase * options_mgr.binwidth / 4);
        modules_hist[i]->SetDirectory(NULL);
        modules_hist[i]->SetLineColor(kGreen);
        modules_hist[i]->SetMinimum(0);
        modules_hist[i]->GetXaxis()->SetTitle("T-T0 (s)");
        modules_hist[i]->GetYaxis()->SetTitle("Rate (trigger/s)");
        if (!options_mgr.tout1_flag) {
            continue;
        }
        sprintf(name, "module_hist_tout1_%02d", i + 1);
        sprintf(title, "module CT_%02d: { %s }, TOUT1", i + 1, gps_time_span.c_str());
        modules_hist_tout1[i] = new TH1D(name, title,
                                         nbins, 0 + options_mgr.phase * options_mgr.binwidth / 4, gps_time_length + options_mgr.phase * options_mgr.binwidth / 4);
        modules_hist_tout1[i]->SetDirectory(NULL);
        modules_hist_tout1[i]->SetLineColor(kRed);
        modules_hist_tout1[i]->SetMinimum(0);
        modules_hist_tout1[i]->GetXaxis()->SetTitle("T-T0 (s)");
        modules_hist_tout1[i]->GetYaxis()->SetTitle("Rate (trigger/s)");
    }
    TH2D* ch_rate_map = new TH2D("ch_rate_map", "Mean rate of 1600 channels (view from back)", 40, 0, 40, 40, 0, 40);
    ch_rate_map->SetDirectory(NULL);
    ch_rate_map->GetXaxis()->SetNdivisions(40);
    ch_rate_map->GetYaxis()->SetNdivisions(40);
    for (int i = 0; i < 40; i++) {
        if (i % 8 == 0) {
            ch_rate_map->GetXaxis()->SetBinLabel(i + 1, Form("%02d", i));
            ch_rate_map->GetYaxis()->SetBinLabel(i + 1, Form("%02d", i));
        }
    }

    double pre_second = 0;
    double cur_second = 0;
    double total_valid_second = 0;

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
        if (options_mgr.min_bars <= 1600 && options_mgr.max_bars <= 1600) {
            if (eventIter.t_trigger.trigger_n < options_mgr.min_bars || eventIter.t_trigger.trigger_n > options_mgr.max_bars) {
                continue;
            }
        } else if (options_mgr.min_bars == 1601 && options_mgr.max_bars == 1601) { // only single
            if (eventIter.t_trigger.type != 0xF000) {
                continue;
            }
        } else if (options_mgr.min_bars == 1602 && options_mgr.max_bars == 1602) { // only normal
            if (eventIter.t_trigger.type != 0x00FF) {
                continue;
            }
        } else if (options_mgr.min_bars == 1603 && options_mgr.max_bars == 1603) { // only cosmic
            if (eventIter.t_trigger.type != 0xFF00) {
                continue;
            }
        } else {
            continue;
        }
        cur_second = (eventIter.t_trigger.abs_gps_week   - eventIter.phy_begin_trigger.abs_gps_week) * 604800 +
            (eventIter.t_trigger.abs_gps_second - eventIter.phy_begin_trigger.abs_gps_second);
        bool is_bad_event = false;
        if (type_mask[0] && eventIter.t_trigger.type == 0xF000) {
            is_bad_event = true;
        } else if (type_mask[1] && eventIter.t_trigger.type == 0x00FF) {
            is_bad_event = true;
        } else if (type_mask[2] && eventIter.t_trigger.type == 0xFF00) {
            is_bad_event = true;
        }
        eventIter.phy_modules_set_start();
        while (!options_mgr.bar_mask_filename.IsNull() && eventIter.phy_modules_next_packet()) {
            int idx = eventIter.t_modules.ct_num - 1;
            for (int j = 0; j < 64; j++) {
                if (eventIter.t_modules.trigger_bit[j] && bar_mask[idx][j]) {
                    is_bad_event = true;
                    break;
                }
            }
        }
        if (is_bad_event) {
            continue;
        }
        trigger_hist->Fill(cur_second);
        for (int i = 0; i < 25; i++) {
            if (eventIter.t_trigger.trig_accepted[i]) {
                modules_hist[i]->Fill(cur_second);
            }
        }
        if (!options_mgr.tout1_flag) {
            continue;
        }
        for (int i = 0; i < 25; i++) {
            if (eventIter.t_trigger.trig_accepted[i]) {
                modules_hist_tout1[i]->Fill(cur_second);
            }
        }
        eventIter.phy_modules_set_start();
        while (eventIter.phy_modules_next_packet()) {
            int idx = eventIter.t_modules.ct_num - 1;
            for (int i = 0; i < eventIter.t_modules.raw_rate; i++) {
                modules_hist_tout1[idx]->Fill(cur_second);
            }
            for (int j = 0; j < 64; j++) {
                if (eventIter.t_modules.trigger_bit[j]) {
                    double cur_bin_content = ch_rate_map->GetBinContent(ijtoxb(idx, j) + 1, ijtoyb(idx, j) + 1);
                    ch_rate_map->SetBinContent(ijtoxb(idx, j) + 1, ijtoyb(idx, j) + 1, cur_bin_content + 1);
                }
            }
        }
        if (cur_second - pre_second > 1.0) {
            if (cur_second - pre_second < 3.0) {
                total_valid_second += cur_second - pre_second;
            }
            pre_second = cur_second;
        }
    }
    cout << " DONE ]" << endl;
    eventIter.close();

    // calculate rate for event rate
    for (int b = 1; b <= trigger_hist->GetNbinsX(); b++) {
        trigger_hist->SetBinContent(b, trigger_hist->GetBinContent(b) / trigger_hist->GetBinWidth(b));
        trigger_hist->SetBinError(b,   trigger_hist->GetBinError(b)   / trigger_hist->GetBinWidth(b));
    }

    // calculate background
    TH1D* trigger_hist_fix = static_cast<TH1D*>(trigger_hist->Clone("trigger_hist_fix"));
    // fill saa gap
    int bin_idx = 0;
    while (bin_idx <= trigger_hist_fix->GetNbinsX()) {
        bin_idx++;
        if (trigger_hist_fix->GetBinContent(bin_idx) > 0) continue;
        // find the first bin before saa
        int bin_idx_left = bin_idx;
        int pre_count = 0;
        double bin_center = trigger_hist_fix->GetBinCenter(bin_idx);
        while (bin_idx_left >= 1) {
            bin_idx_left--;
            if (trigger_hist_fix->GetBinWidth(1) < 1.0) {
                if (trigger_hist_fix->GetBinCenter(bin_idx_left) < bin_center - 4.0) {
                    break;
                }
            } else {
                if (trigger_hist_fix->GetBinContent(bin_idx_left) < pre_count) {
                    break;
                } else {
                    pre_count = trigger_hist_fix->GetBinContent(bin_idx_left);
                }
            }
        }
        // find the first bin after saa
        int bin_idx_right = bin_idx;
        pre_count = 0;
        while (bin_idx_right <= trigger_hist_fix->GetNbinsX()) {
            bin_idx_right++;
            if (trigger_hist_fix->GetBinContent(bin_idx_right) == 0) {
                bin_center = trigger_hist_fix->GetBinCenter(bin_idx_right);
                continue;
            }
            if (trigger_hist_fix->GetBinWidth(1) < 1.0) {
                if (trigger_hist_fix->GetBinWidth(bin_idx_right) < 1.0 && trigger_hist_fix->GetBinCenter(bin_idx_right) > bin_center + 4.0) {
                    break;
                }
            } else {
                if (trigger_hist_fix->GetBinContent(bin_idx_right) < pre_count) {
                    break;
                } else {
                    pre_count = trigger_hist_fix->GetBinContent(bin_idx_right);
                }
            }
        }
        // fill gap
        double ratio = 1.5;
        double slope = (trigger_hist_fix->GetBinContent(bin_idx_right) * ratio - trigger_hist_fix->GetBinContent(bin_idx_left) * ratio) / (bin_idx_right - bin_idx_left);
        slope *= 2.0;
        for (int i = 1; i < bin_idx_right - bin_idx_left; i++) {
            if (slope > 0) {
                trigger_hist_fix->SetBinContent(bin_idx_left + i, trigger_hist_fix->GetBinContent(bin_idx_left) * ratio + i * slope);
            } else {
                trigger_hist_fix->SetBinContent(bin_idx_right - i, trigger_hist_fix->GetBinContent(bin_idx_right) * ratio - i * slope);
            }
        }
        // jump the gap
        bin_idx = bin_idx_right;
    }
    TSpectrum t_spec;
    TH1D* trigger_hist_bkg = static_cast<TH1D*>(t_spec.Background(trigger_hist_fix, options_mgr.niter));
    // correct bkg
    for (int i = 1; i <= trigger_hist->GetNbinsX(); i++) {
        if (trigger_hist->GetBinContent(i) == 0) {
            trigger_hist_bkg->SetBinContent(i, 0);
        }
    }

    // calculate rate for modules
    for (int j = 0; j < 25; j++) {
        for (int b = 1; b < modules_hist[j]->GetNbinsX(); b++) {
            modules_hist[j]->SetBinContent(b, modules_hist[j]->GetBinContent(b) / modules_hist[j]->GetBinWidth(b));
            modules_hist[j]->SetBinError(  b, modules_hist[j]->GetBinError(b)   / modules_hist[j]->GetBinWidth(b));
        }
    }
    if (options_mgr.tout1_flag) {
        for (int j = 0; j < 25; j++) {
            for (int b = 1; b < modules_hist_tout1[j]->GetNbinsX(); b++) {
                modules_hist_tout1[j]->SetBinContent(b, modules_hist_tout1[j]->GetBinContent(b) / modules_hist_tout1[j]->GetBinWidth(b));
                modules_hist_tout1[j]->SetBinError(  b, modules_hist_tout1[j]->GetBinError(b)   / modules_hist_tout1[j]->GetBinWidth(b));
            }
        }
    }

    if (!options_mgr.output_filename.IsNull()) {
        cout << " - saving light curve ..." << endl;
        TFile* outfile = new TFile(options_mgr.output_filename.Data(), "recreate");
        if (outfile->IsZombie()) {
            cout << "ERROR: output file open failed: " << options_mgr.output_filename.Data() << endl;
        } else {
            trigger_hist->Write();
            for (int i = 0; i < 25; i++) {
                modules_hist[i]->Write();
            }
            if (options_mgr.tout1_flag) {
                for (int i = 0; i < 25; i++) {
                    modules_hist_tout1[i]->Write();
                }
            }
            TNamed("gps_time_span",
                    Form("%d:%d => %d:%d",
                        static_cast<int>(eventIter.phy_begin_trigger.abs_gps_week),
                        static_cast<int>(eventIter.phy_begin_trigger.abs_gps_second),
                        static_cast<int>(eventIter.phy_end_trigger.abs_gps_week),
                        static_cast<int>(eventIter.phy_end_trigger.abs_gps_second)
                        )).Write();
            TNamed("time_length", Form("%f", gps_time_length)).Write();
            TNamed("bin_width", Form("%f", options_mgr.binwidth)).Write();
            TNamed("nbins", Form("%d", nbins)).Write();
            outfile->Close();
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
        for (int i = 0; i < 25; i++) {
            for (int j = 0; j < 64; j++) {
                double cur_bin_content = ch_rate_map->GetBinContent(ijtoxb(i, j) + 1, ijtoyb(i, j) + 1);
                ch_rate_map->SetBinContent(ijtoxb(i, j) + 1, ijtoyb(i, j) + 1, cur_bin_content / total_valid_second);
            }
        }
        rate_canvas.cd_ch_map(1);
        ch_rate_map->Draw("COLZ");
		TLine* line_h_[4];
		TLine* line_v_[4];
		for (int i = 0; i < 4; i++) {
			line_h_[i] = new TLine(0, (i + 1) * 8, 40, (i + 1) * 8);
			line_h_[i]->SetLineColor(kWhite);
			line_h_[i]->Draw("SAME");
			line_v_[i] = new TLine((i + 1) * 8, 0, (i + 1) * 8, 40);
			line_v_[i]->SetLineColor(kWhite);
			line_v_[i]->Draw("SAME");
		}
        rate_canvas.cd_ch_map(2);
        ch_rate_map->Draw("LEGO2");
    }

    rate_canvas.draw_trigger_hist(trigger_hist);
    rate_canvas.draw_trigger_hist_bkg(trigger_hist_bkg);

    rootapp->Run();

    return 0;
}
