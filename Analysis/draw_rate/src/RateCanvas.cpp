#include "RateCanvas.hpp"

using namespace std;

RateCanvas::RateCanvas(int week, double second) {
    start_gps_time_ = week * 604800 + second;
    cur_trigger_hist_ = NULL;
    cur_trigger_hist_bkg_ = NULL;
    cur_trigger_hist_subbkg_ = NULL;
    select_count_ = 0;
    line_cnt_ = 0;
    for (int i = 0; i < 2; i++) {
        line_obj_[i] = NULL;
    }
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
    if (cur_trigger_hist_subbkg_ != NULL) {
        delete cur_trigger_hist_subbkg_;
        cur_trigger_hist_subbkg_ = NULL;
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
        cout << "calculating T90 ..." << endl;
        if (select_x_[0] > select_x_[1]) {
            double tmp_x = select_x_[0];
            select_x_[0] = select_x_[1];
            select_x_[1] = tmp_x;
        }
        int bin_left = (select_x_[0] - cur_trigger_hist_->GetBinLowEdge(0)) / cur_trigger_hist_->GetBinWidth(0);
        int bin_right = (select_x_[1] - cur_trigger_hist_->GetBinLowEdge(0)) / cur_trigger_hist_->GetBinWidth(0);
        cout << "bin_left = " << bin_left << endl;
        cout << "bin_right = " << bin_right << endl;
        double left_edge = cur_trigger_hist_->GetBinLowEdge(bin_left);
        double right_edge = cur_trigger_hist_->GetBinLowEdge(bin_right) + cur_trigger_hist_->GetBinWidth(bin_right);
        cout << "left_edge = " << left_edge << endl;
        cout << "right_edge = " << right_edge << endl;;
        cout << "right_edge - left_edge = " << right_edge - left_edge << endl;
        cur_trigger_hist_subbkg_ = new TH1D("cur_trigger_hist_subbkg_", "trigger rate",
                bin_right - bin_left + 1, left_edge, right_edge);
        cur_trigger_hist_subbkg_->SetDirectory(NULL);
        for (int i = bin_left; i <= bin_right; i++) {
            cur_trigger_hist_subbkg_->SetBinContent(i - bin_left + 1,
                    cur_trigger_hist_->GetBinContent(i) - cur_trigger_hist_bkg_->GetBinContent(i));
        }
        canvas_trigger_subbkg_ = static_cast<TCanvas*>(gROOT->FindObject("canvas_trigger_sub_bkg_"));
        if (canvas_trigger_subbkg_ == NULL) {
            canvas_trigger_subbkg_ = new TCanvas("canvas_trigger_subbkg", "rate of event trigger", 800, 500);
            canvas_trigger_subbkg_->ToggleEventStatus();
            canvas_trigger_subbkg_->SetCrosshair();
            canvas_trigger_subbkg_->Connect("Closed()", "RateCanvas", this, "CloseLC()");
        }
        canvas_trigger_subbkg_->cd();
        cur_trigger_hist_subbkg_->Draw("eh");
        // stop todo
        keypressed = true;
    } else if (event == kButton1Down) {  // select
        double x = static_cast<double>(canvas_trigger_->AbsPixeltoX(px));
        if (x < x_min || x > x_max) return;
        if (line_cnt_ >= 2) return;
        select_count_++;
        double tmp_gps_time = start_gps_time_ + x;
        cout << " - " << setw(2) << select_count_ << " -> "
             << static_cast<int>(tmp_gps_time / 604800)
             << ":"
             << fmod(tmp_gps_time, 604800) << endl;
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
