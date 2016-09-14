#include "RateCanvas.hpp"

using namespace std;

RateCanvas::RateCanvas(int week, double second) {
    start_gps_week_   = week;
    start_gps_second_ = second;
    select_count_     = 0;
    for (int i = 0; i < 5; i++) {
        line_obj_[i] = NULL;
    }
    for (int i = 0; i < 2; i++) {
        line_t90_[i] = NULL;
    }
    line_cnt_ = 0;
    cur_hist_int_ = NULL;
    cur_scale_ = 1.0;
    fun_before_ = new TF1("fun_before", "[0] * x + [1]", 0, 1);
    fun_before_->SetParameters(1.0, 1.0);
    fun_after_  = new TF1("fun_after" , "[0] * x + [1]", 2, 3);
    fun_after_->SetParameters(1.0, 1.0);
    line_before_ = NULL;
    line_before_05_ = NULL;
    line_after_ = NULL;
    line_after_05_ = NULL;

    is_fitted_ = false;
}

RateCanvas::~RateCanvas() {

}

void RateCanvas::cd_trigger() {
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
}

void RateCanvas::cd_modules(int i) {
    canvas_modules_ = static_cast<TCanvas*>(gROOT->FindObject("canvas_modules"));
    if (canvas_modules_ == NULL) {
        canvas_modules_ = new TCanvas("canvas_modules", "rate of 25 modules", 1500, 1000);
        canvas_modules_->ToggleEventStatus();
        canvas_modules_->Divide(5, 5);
    }
    canvas_modules_->cd(itoc(i));
}

void RateCanvas::draw_hist_int(TH1D* hist_int) {
    if (cur_hist_int_ != NULL)
        return;
    cur_hist_int_ = hist_int;
    canvas_trigger_->Update();
    canvas_trigger_->cd();
    double rightmax = cur_hist_int_->GetMaximum();
    cur_scale_ = canvas_trigger_->GetUymax() / rightmax;
    cur_hist_int_->Scale(cur_scale_);
    cur_hist_int_->Draw("SAME");
    TGaxis *axis = new TGaxis(gPad->GetUxmax(),gPad->GetUymin(), gPad->GetUxmax(), gPad->GetUymax(), 0, rightmax, 510, "+L");
    axis->SetLineColor(cur_hist_int_->GetLineColor());
    axis->SetTextColor(cur_hist_int_->GetLineColor());
    axis->SetLabelSize(0.02);
    axis->Draw();
}

void RateCanvas::CloseWindow() {
    cout << "Quitting by user request." << endl;
    gApplication->Terminate(0);
}

void RateCanvas::ProcessAction(Int_t event, Int_t px, Int_t py, TObject* selected) {
    if (event != kArrowKeyPress && event != kKeyPress && event != kButton1Down)
        return;

    if (cur_hist_int_ == NULL)
        return;
    canvas_trigger_ = static_cast<TCanvas*>(gROOT->FindObject("canvas_trigger"));
    if (canvas_trigger_ == NULL)
        return;
    double x_max = canvas_trigger_->GetUxmax();
    double y_max = canvas_trigger_->GetUymax();

    if (event == kKeyPress) {  // fitting, calculate T90
        if (line_cnt_ < 5)
            return;
        if (is_fitted_)
            return;
        cout << "fitting ... " << endl;
        TMath::Sort(5, select_x_, select_i_, kFALSE);
        double x1 = select_x_[select_i_[0]];
        double x2 = select_x_[select_i_[1]];
        double x3 = select_x_[select_i_[3]];
        double x4 = select_x_[select_i_[4]];
        double xc = select_x_[select_i_[2]];
        line_obj_[select_i_[2]]->SetLineColor(kBlue);
        fun_before_->SetRange(x1, x2);
        fun_after_->SetRange( x3, x4);
        cur_hist_int_->Fit(fun_before_, "RQN");
        double a1 = fun_before_->GetParameter(0);
        double b1 = fun_before_->GetParameter(1);
        cur_hist_int_->Fit(fun_after_,  "RQN");
        double a2 = fun_after_->GetParameter(0);
        double b2 = fun_after_->GetParameter(1);

        double delta05 = ((a2 - a1) * xc + (b2 - b1)) * 0.05;
        double b05 = b1 + delta05;
        double b95 = b2 - delta05;

        line_before_ = new TLine(0, b1, x_max, a1 * x_max + b1);
        line_after_  = new TLine(0, b2, x_max, a2 * x_max + b2);
        line_before_05_ = new TLine(0, b05, x_max, a1 * x_max + b05);
        line_after_05_  = new TLine(0, b95, x_max, a2 * x_max + b95);

        line_before_->SetLineColor(kGreen);
        line_before_->SetLineStyle(2);
        line_before_->Draw();
        line_after_->SetLineColor(kGreen);
        line_after_->SetLineStyle(2);
        line_after_->Draw();
        line_before_05_->SetLineColor(kRed);
        line_before_05_->SetLineStyle(2);
        line_before_05_->Draw();
        line_after_05_->SetLineColor(kRed);
        line_after_05_->SetLineStyle(2);
        line_after_05_->Draw();

        is_fitted_ = true;

        vector<double> diff_vec_05;
        diff_vec_05.resize(cur_hist_int_->GetNbinsX());
        vector<double> diff_vec_95;
        diff_vec_95.resize(cur_hist_int_->GetNbinsX());
        for (int i = 1; i <= cur_hist_int_->GetNbinsX(); i++) {
            double cur_x = cur_hist_int_->GetBinCenter(i);
            double cur_y = cur_hist_int_->GetBinContent(i);
            diff_vec_05[i - 1] = TMath::Abs(a1 * cur_x + b05 - cur_y);
            diff_vec_95[i - 1] = TMath::Abs(a2 * cur_x + b95 - cur_y);
        }
        int min_bin_05 = 1;
        int min_bin_95 = 1;
        double min_diff_05 = 1.0E10;
        double min_diff_95 = 1.0E10;
        for (int i = 1; i < cur_hist_int_->GetNbinsX(); i++) {
            if (diff_vec_05[i - 1] < min_diff_05) {
                min_diff_05 = diff_vec_05[i - 1];
                min_bin_05  = i;
            }
            if (diff_vec_95[i - 1] < min_diff_95) {
                min_diff_95 = diff_vec_95[i - 1];
                min_bin_95 = i;
            }
        }
        double t90_begin = cur_hist_int_->GetBinCenter(min_bin_05);
        double t90_end   = cur_hist_int_->GetBinCenter(min_bin_95);
        double t90_total = cur_hist_int_->GetBinContent(min_bin_95) - cur_hist_int_->GetBinContent(min_bin_05);
        t90_total -= (a1 + a2) * (t90_end - t90_begin) / 2;  // subtract background
        line_t90_[0] = new TLine(t90_begin, 0, t90_begin, y_max);
        line_t90_[1] = new TLine(t90_end,   0, t90_end,   y_max);
        for (int i = 0; i < 2; i++) {
            line_t90_[i]->SetLineColor(kRed);
            line_t90_[i]->Draw();
        }

        // print T90
        double t90_begin_gps_second = start_gps_second_ + t90_begin;
        double t90_end_gps_second   = start_gps_second_ + t90_end;
        cout << " * T90 begin:      "
             << start_gps_week_ + static_cast<int>(t90_begin_gps_second / 604800)
             << ":"
             << fmod(t90_begin_gps_second, 604800)
             << endl;
        cout << " * T90 end:        "
             << start_gps_week_ + static_cast<int>(t90_end_gps_second / 604800)
             << ":"
             << fmod(t90_end_gps_second, 604800)
             << endl;
        cout << " * T90 mean rate:  "
             << t90_total / (t90_end - t90_begin)
             << endl;
        
        canvas_trigger_->Update();

    } else if (event == kArrowKeyPress) {  // clear 
        line_cnt_ = 0;
        for (int i = 0; i < 5; i++) {
            if (line_obj_[i] != NULL) {
                delete line_obj_[i];
                line_obj_[i] = NULL;
            }
        }
        if (line_before_ != NULL) {
            delete line_before_;
            line_before_ = NULL;
        }
        if (line_before_05_ != NULL) {
            delete line_before_05_;
            line_before_05_ = NULL;
        }
        if (line_after_ != NULL) {
            delete line_after_;
            line_after_ = NULL;
        }
        if (line_after_05_ != NULL) {
            delete line_after_05_;
            line_after_05_ = NULL;
        }
        for (int i = 0; i < 2; i++) {
            if (line_t90_[i] != NULL) {
                delete line_t90_[i];
                line_t90_[i] = NULL;
            }
        }
        is_fitted_ = false;
        canvas_trigger_->Update();
        cout << " -*-*-*-*-*-*-*-*-*-*-*-*- " << endl;
    } else if (event == kButton1Down) {  // select 
        if (line_cnt_ >= 5)
            return;
        int x = static_cast<int>(canvas_trigger_->AbsPixeltoX(px));
        select_count_ += 1;
        double tmp_gps_second = start_gps_second_ + x;
        cout << " - " << setw(2) << select_count_ << " -> " 
             << start_gps_week_ + static_cast<int>(tmp_gps_second / 604800) 
             << ":" 
             << fmod(tmp_gps_second, 604800) << endl;
        select_x_[line_cnt_] = x;
        line_obj_[line_cnt_] = new TLine(x, 0, x, y_max);
        line_obj_[line_cnt_]->SetLineColor(kGray);
        line_obj_[line_cnt_]->Draw();
        line_cnt_++;
    }
}
