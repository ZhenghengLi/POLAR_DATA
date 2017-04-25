#include "RateCanvas.hpp"
#include <cstdlib>
#include <cmath>
#include <ctime>
#include <cstdio>

using namespace std;

RateCanvas::RateCanvas(double met_time, double min_s) {
    start_met_time_ = met_time;
    min_signif_ = min_s;
    cur_trigger_hist_ = NULL;
    cur_trigger_hist_bkg_ = NULL;
    cur_trigger_hist_subbkg_ = NULL;
    select_count_ = 0;
    line_cnt_ = 0;
    for (int i = 0; i < 2; i++) {
        line_obj_[i] = NULL;
    }
    line_zero_ = NULL;
    line_t90_left_ = NULL;
    line_t90_right_ = NULL;
    line_peak_ = NULL;
    line_T0_ = NULL;
    pavetext_ = NULL;
    keypressed = false;
}

RateCanvas::~RateCanvas() {

}

void RateCanvas::cd_modules(int i) {
    canvas_modules_ = static_cast<TCanvas*>(gROOT->FindObject("canvas_modules"));
    if (canvas_modules_ == NULL) {
        canvas_modules_ = new TCanvas("canvas_modules", "rate of 25 modules", 1500, 1000);
        canvas_modules_->ToggleEventStatus();
        canvas_modules_->Divide(5, 5);
    }
    canvas_modules_->cd(itocb(i));
}

void RateCanvas::cd_modules_tout1(int i) {
    canvas_modules_tout1_ = static_cast<TCanvas*>(gROOT->FindObject("canvas_modules_tout1"));
    if (canvas_modules_tout1_ == NULL) {
        canvas_modules_tout1_ = new TCanvas("canvas_modules_tout1", "tout1 rate of 25 modules", 1500, 1000);
        canvas_modules_tout1_->ToggleEventStatus();
        canvas_modules_tout1_->Divide(5, 5);
    }
    canvas_modules_tout1_->cd(itocb(i));
}

void RateCanvas::cd_ch_map(int i) {
	canvas_ch_map_ = static_cast<TCanvas*>(gROOT->FindObject("canvas_ch_map"));
	if (canvas_ch_map_ == NULL) {
		canvas_ch_map_ = new TCanvas("canvas_ch_map", "Mean rate of 1600 channels", 1600, 800);
		canvas_ch_map_->Divide(2, 1);
		canvas_ch_map_->GetPad(1)->SetGrid();
		canvas_ch_map_->GetPad(2)->SetGrid();
	}
	canvas_ch_map_->cd(i);
}

void RateCanvas::draw_trigger_hist(TH1D* trigger_hist) {
    if (cur_trigger_hist_ != NULL)
        return;
    cur_trigger_hist_ = trigger_hist;
    canvas_trigger_ = static_cast<TCanvas*>(gROOT->FindObject("canvas_trigger"));
    if (canvas_trigger_ == NULL) {
        canvas_trigger_ = new TCanvas("canvas_trigger", "rate of event trigger", 1000, 600);
        canvas_trigger_->ToggleEventStatus();
        canvas_trigger_->SetCrosshair();
        canvas_trigger_->Connect("Closed()", "RateCanvas", this, "CloseWindow()");
        canvas_trigger_->Connect("ProcessedEvent(Int_t, Int_t, Int_t, TObject*)", "RateCanvas",
                                 this, "ProcessAction(Int_t, Int_t, Int_t, TObject*)");
    }
    canvas_trigger_->cd();
    cur_trigger_hist_->Draw("eh");
}
void RateCanvas::draw_trigger_hist_bkg(TH1D* trigger_hist_bkg) {
    if (cur_trigger_hist_ == NULL)
        return;
    if (cur_trigger_hist_bkg_ != NULL)
        return;
    cur_trigger_hist_bkg_ = trigger_hist_bkg;
    canvas_trigger_->cd();
    cur_trigger_hist_bkg_->Draw("SAME");
    canvas_trigger_->Update();
}

void RateCanvas::CloseWindow() {
    cout << "Quitting by user request." << endl;
    gApplication->Terminate(0);
}

void RateCanvas::CloseLC() {
    line_cnt_ = 0;
    for (int i = 0; i < 2; i++) {
        if (line_obj_[i] != NULL) {
            delete line_obj_[i];
            line_obj_[i] = NULL;
        }
    }
    if (line_zero_ != NULL) {
        delete line_zero_;
        line_zero_ = NULL;
    }
    if (cur_trigger_hist_subbkg_ != NULL) {
        delete cur_trigger_hist_subbkg_;
        cur_trigger_hist_subbkg_ = NULL;
    }
    if (line_t90_left_ != NULL) {
        delete line_t90_left_;
        line_t90_left_ = NULL;
    }
    if (line_t90_right_ != NULL) {
        delete line_t90_right_;
        line_t90_right_ = NULL;
    }
    if (line_peak_ != NULL) {
        delete line_peak_;
        line_peak_ = NULL;
    }
    if (line_T0_ != NULL) {
        delete line_T0_;
        line_T0_ = NULL;
    }
    if (pavetext_ != NULL) {
        delete pavetext_;
        pavetext_ = NULL;
    }
    keypressed = false;
    canvas_trigger_->Update();
}

void RateCanvas::ProcessAction(Int_t event, Int_t px, Int_t py, TObject* selected) {
    if (event != kButton1Double && event != kKeyPress && event != kButton1Down)
        return;

    canvas_trigger_ = static_cast<TCanvas*>(gROOT->FindObject("canvas_trigger"));
    if (canvas_trigger_ == NULL)
        return;
    double x_max = canvas_trigger_->GetUxmax();
    double x_min = canvas_trigger_->GetUxmin();
    double y_max = canvas_trigger_->GetUymax();

    if (event == kKeyPress) {  // fitting, calculate T90
        if (line_cnt_ < 2) return;
        if (keypressed) return;
        // start todo
        cout << "Subtracting background ..." << endl;
        if (select_x_[0] > select_x_[1]) {
            double tmp_x = select_x_[0];
            select_x_[0] = select_x_[1];
            select_x_[1] = tmp_x;
        }
        int bin_left = (select_x_[0] - cur_trigger_hist_->GetBinLowEdge(0)) / cur_trigger_hist_->GetBinWidth(0);
        int bin_right = (select_x_[1] - cur_trigger_hist_->GetBinLowEdge(0)) / cur_trigger_hist_->GetBinWidth(0);
        double left_edge = cur_trigger_hist_->GetBinLowEdge(bin_left);
        double right_edge = cur_trigger_hist_->GetBinLowEdge(bin_right) + cur_trigger_hist_->GetBinWidth(bin_right);
        // find T0
        double x_T0 = -1;
        for (int i = bin_left; i <= bin_right; i++) {
            double cur_sigbkg = cur_trigger_hist_->GetBinContent(i) * cur_trigger_hist_->GetBinWidth(i);
            double cur_bkg = cur_trigger_hist_bkg_->GetBinContent(i) * cur_trigger_hist_->GetBinWidth(i);
            double cur_signif = (cur_sigbkg - cur_bkg) / TMath::Sqrt(cur_bkg);
            if (cur_signif > min_signif_) {
                x_T0 = cur_trigger_hist_->GetBinLowEdge(i);
                break;
            }
        }
        if (x_T0 > 0) {
            left_edge -= x_T0;
            right_edge -= x_T0;
        }
        cur_trigger_hist_subbkg_ = new TH1D("cur_trigger_hist_subbkg_", "trigger rate after background subtracted",
                bin_right - bin_left + 1, left_edge, right_edge);
        cur_trigger_hist_subbkg_->SetDirectory(NULL);
        cur_trigger_hist_subbkg_->GetXaxis()->SetTitle("T-T0 (s)");
        cur_trigger_hist_subbkg_->GetYaxis()->SetTitle("Rate (trigger/s)");

        for (int i = bin_left; i <= bin_right; i++) {
            cur_trigger_hist_subbkg_->SetBinContent(i - bin_left + 1,
                    cur_trigger_hist_->GetBinContent(i) - cur_trigger_hist_bkg_->GetBinContent(i));
            cur_trigger_hist_subbkg_->SetBinError(i - bin_left + 1,
                    TMath::Sqrt(cur_trigger_hist_->GetBinContent(i) * cur_trigger_hist_->GetBinWidth(i) +
                        cur_trigger_hist_bkg_->GetBinContent(i) * cur_trigger_hist_bkg_->GetBinWidth(i)) /
                        cur_trigger_hist_->GetBinWidth(i));
        }
        canvas_trigger_subbkg_ = static_cast<TCanvas*>(gROOT->FindObject("canvas_trigger_sub_bkg_"));
        if (canvas_trigger_subbkg_ == NULL) {
            canvas_trigger_subbkg_ = new TCanvas("canvas_trigger_subbkg", "rate of event trigger", 700, 600);
            canvas_trigger_subbkg_->Divide(1, 2);
            canvas_trigger_subbkg_->ToggleEventStatus();
            canvas_trigger_subbkg_->SetCrosshair();
            canvas_trigger_subbkg_->Connect("Closed()", "RateCanvas", this, "CloseLC()");
        }
        canvas_trigger_subbkg_->cd(1);
        cur_trigger_hist_subbkg_->Draw("eh");
        line_zero_ = new TLine(left_edge, 0, right_edge, 0);
        line_zero_->SetLineColor(kRed);
        line_zero_->Draw();
        double y_min_subbkg = cur_trigger_hist_subbkg_->GetMinimum();
        double y_max_subbkg = cur_trigger_hist_subbkg_->GetMaximum() * 1.1;
        double total_counts = cur_trigger_hist_subbkg_->Integral();
        // find T90
        double x_t90_left = 0;
        int bin_t90_left = 0;
        double sum_counts = 0;
        for (int i = bin_left; i <= bin_right; i++) {
            sum_counts += cur_trigger_hist_subbkg_->GetBinContent(i - bin_left + 1);
            if (sum_counts > total_counts * 0.05) {
                x_t90_left = cur_trigger_hist_subbkg_->GetBinLowEdge(i - bin_left + 1) + cur_trigger_hist_subbkg_->GetBinWidth(i - bin_left + 1);
                bin_t90_left = i + 1;
                break;
            }
        }
        double x_t90_right = 0;
        int bin_t90_right = 0;
        sum_counts = 0;
        for (int i = bin_right; i >= bin_left; i--) {
            sum_counts += cur_trigger_hist_subbkg_->GetBinContent(i - bin_left + 1);
            if (sum_counts > total_counts * 0.05) {
                x_t90_right = cur_trigger_hist_subbkg_->GetBinLowEdge(i - bin_left + 1);
                bin_t90_right = i - 1;
                break;
            }
        }
        line_t90_left_ = new TLine(x_t90_left, y_min_subbkg, x_t90_left, y_max_subbkg);
        line_t90_left_->SetLineColor(kRed);
        line_t90_left_->Draw();
        line_t90_right_ = new TLine(x_t90_right, y_min_subbkg, x_t90_right, y_max_subbkg);
        line_t90_right_->SetLineColor(kRed);
        line_t90_right_->Draw();
        double sum_bkg = 0;
        double sum_sigbkg = 0;
        for (int i = bin_t90_left; i <= bin_t90_right; i++) {
            sum_sigbkg += cur_trigger_hist_->GetBinContent(i) * cur_trigger_hist_->GetBinWidth(i);
            sum_bkg += cur_trigger_hist_bkg_->GetBinContent(i) * cur_trigger_hist_->GetBinWidth(i);
        }
        double nsigma = (sum_sigbkg - sum_bkg) / TMath::Sqrt(sum_bkg);
        // find peak
        int bin_peak = cur_trigger_hist_subbkg_->GetMaximumBin();
        int rate_peak = cur_trigger_hist_subbkg_->GetBinContent(bin_peak);
        double x_peak = cur_trigger_hist_subbkg_->GetBinLowEdge(bin_peak);
        line_peak_ = new TLine(x_peak, y_min_subbkg, x_peak, y_max_subbkg);
        line_peak_->SetLineColor(kGreen);
        line_peak_->Draw();
        // find T0
        if (x_T0 > 0) {
            line_T0_ = new TLine(0, y_min_subbkg, 0, y_max_subbkg);
            line_T0_->SetLineColor(6);
            line_T0_->Draw();
        }
        double time_T0 = (x_T0 > 0 ? start_met_time_ + x_T0 : 0);
        double time_t90_start = (x_T0 > 0 ? start_met_time_ + x_t90_left + x_T0 : start_met_time_ + x_t90_left);
        double time_t90_stop  = (x_T0 > 0 ? start_met_time_ + x_t90_right + x_T0 : start_met_time_ + x_t90_right);
        double time_peak = (x_T0 > 0 ? start_met_time_ + x_peak + x_T0 : start_met_time_ + x_peak);
        canvas_trigger_subbkg_->cd(2);
        pavetext_ = new TPaveText(0.1, 0.1, 0.9, 1);
        pavetext_->AddText(0.0, 0.9, Form("GRB_T0GPS: %d:%.3f [%s]",
                    static_cast<int>(time_T0 / 604800),
                    fmod(time_T0, 604800),
                    met_to_utc_str_(time_T0).c_str()));
        pavetext_->AddText(0.0, 0.77 , Form("T90_START: %d:%.3f [%s]",
                    static_cast<int>(time_t90_start / 604800),
                    fmod(time_t90_start, 604800),
                    met_to_utc_str_(time_t90_start).c_str()));
        pavetext_->AddText(0.0, 0.64, Form("T90_STOP: %d:%.3f [%s]",
                    static_cast<int>(time_t90_stop / 604800),
                    fmod(time_t90_stop, 604800),
                    met_to_utc_str_(time_t90_stop).c_str()));
        pavetext_->AddText(0.0, 0.51, Form("PEAK_TIME: %d:%.3f [%s]",
                    static_cast<int>(time_peak / 604800),
                    fmod(time_peak, 604800),
                    met_to_utc_str_(time_peak).c_str()));
        pavetext_->AddText(0.0, 0.38, Form("PEAK_RATE: %d cnts/sec",
                    static_cast<int>(rate_peak)));
        pavetext_->AddText(0.0, 0.25, Form("GRB_INTEN: %d cnts/sec",
                static_cast<int>((sum_sigbkg - sum_bkg) / (x_t90_right - x_t90_left))));
        pavetext_->AddText(0.0, 0.12, Form("GRB_SIGNIF: %.2f sigma", nsigma));
        pavetext_->SetTextAlign(12);
        pavetext_->SetTextSize(0.06);
        pavetext_->Draw();

        // stop todo
        keypressed = true;
    } else if (event == kButton1Down) {  // select
        double x = static_cast<double>(canvas_trigger_->AbsPixeltoX(px));
        if (x < x_min || x > x_max) return;
        if (line_cnt_ >= 2) return;
        select_count_++;
        double tmp_met_time = start_met_time_ + x;
        cout << " - " << setw(2) << select_count_ << " -> "
             << tmp_met_time << endl;
        canvas_trigger_->cd();
        select_x_[line_cnt_] = x;
        line_obj_[line_cnt_] = new TLine(x, 0, x, y_max);
        line_obj_[line_cnt_]->SetLineColor(kGreen);
        line_obj_[line_cnt_]->Draw();
        line_cnt_++;
        canvas_trigger_->Update();
    } else if (event == kButton1Double) {  // clear
        if (keypressed) return;
        line_cnt_ = 0;
        for (int i = 0; i < 2; i++) {
            if (line_obj_[i] != NULL) {
                delete line_obj_[i];
                line_obj_[i] = NULL;
            }
        }
        canvas_trigger_->Update();
        cout << " -*-*-*-*-*-*-*-*-*-*-*-*- " << endl;
    }
}

string RateCanvas::met_to_utc_str_(double met_time) {
    double utc_time_second = 0;
    double gps_time = met_time + 1157984045;
    if (gps_time < 1119744016.0) {
        utc_time_second = gps_time - 16;
    } else if (gps_time < 1167264017.0) {
        utc_time_second = gps_time - 17;
    } else {
        utc_time_second = gps_time - 18;
    }
    double unixtime = utc_time_second + 315964800.0;
    double unixtime_second;
    int unixtime_millsecond;
    unixtime_millsecond = static_cast<int>(round(modf(unixtime, &unixtime_second) * 1000));
    char str_buffer[50];
    time_t cur_unixtime = unixtime_second;
    strftime(str_buffer, 20, "%Y-%m-%d %H:%M:%S", gmtime(&cur_unixtime));
    char str_result[50];
    if (unixtime_millsecond < 10) {
        sprintf(str_result, "%s.00%d UTC", str_buffer, unixtime_millsecond);
    } else if (unixtime_millsecond < 100) {
        sprintf(str_result, "%s.0%d UTC", str_buffer, unixtime_millsecond);
    } else {
        sprintf(str_result, "%s.%d UTC", str_buffer, unixtime_millsecond);
    }
    return string(str_result);
}
