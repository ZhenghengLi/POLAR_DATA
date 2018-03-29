#include "EventCanvas.hpp"
#include <cstdlib>
#include <cmath>
#include <ctime>
#include <cstdio>
#include <fstream>
#include <queue>
#include "BarPos.hpp"

using namespace std;

EventCanvas::EventCanvas() {
    t_pol_event_file_ = NULL;
    t_pol_event_tree_ = NULL;
    trigger_map_ = NULL;
    energy_map_ = NULL;
    h_stack_ = NULL;
    queue_flag_ = false;
    angle_arrow_ = NULL;
}

EventCanvas::~EventCanvas() {

}

bool EventCanvas::open(const char* filename, int start, int step) {
    entry_step_ = step;
    entry_current_ = start - step;

    t_pol_event_file_ = new TFile(filename, "read");
    if (t_pol_event_file_->IsZombie()) {
        cout << "pol_event_file open failed: " << filename << endl;
        return false;
    }
    t_pol_event_tree_ = static_cast<TTree*>(t_pol_event_file_->Get("t_pol_event"));
    if (t_pol_event_tree_ == NULL) {
        cout << "cannot find TTree t_pol_event." << endl;
        return false;
    }

    t_pol_event_.bind_pol_event_tree(t_pol_event_tree_);

    return true;
}

void EventCanvas::close() {
    if (t_pol_event_file_ == NULL) return;

    t_pol_event_file_->Close();
    delete t_pol_event_file_;
    t_pol_event_file_ = NULL;
    t_pol_event_tree_ = NULL;

}

void EventCanvas::CloseWindow() {
    cout << "Quitting by user request." << endl;
    close();
    gApplication->Terminate(0);
}

void EventCanvas::ProcessAction(Int_t event, Int_t px, Int_t py, TObject* selected) {
    if (event != kButton1Double)
        return;
    draw_event();
}

bool EventCanvas::find_first_two(const POLEvent& pol_event,
        int first_ij[2], int second_ij[2],
        double& first_energy, double& second_energy) {

    priority_queue<Bar> bar_queue;
    Bar first_bar;
    Pos first_pos;
    Bar second_bar;
    Pos second_pos;
    // clear bar_queue
    while (!bar_queue.empty()) bar_queue.pop();
    // find the first two bars'
    bool is_bad_event = false;
    for (int i = 0; i < 25; i++) {
        if (!pol_event.time_aligned[i]) continue;
        for (int j = 0; j < 64; j++) {
            if (pol_event.trigger_bit[i][j] && pol_event.channel_status[i][j] > 0 && pol_event.channel_status[i][j] != 0x4) {
               is_bad_event = true;
               break;
            }
            if (pol_event.trigger_bit[i][j]) {
                bar_queue.push(Bar(pol_event.energy_value[i][j], i, j));
            }
        }
        if (is_bad_event) break;
    }
    if (is_bad_event) {
        return false;
    }
    // get first bar
    if (bar_queue.empty()) {
        return false;
    }
    first_bar = bar_queue.top();
    bar_queue.pop();
    first_pos.randomize(first_bar.i, first_bar.j);
    // get second bar
    if (bar_queue.empty()) {
        return false;
    }
    second_bar = bar_queue.top();
    bar_queue.pop();
    second_pos.randomize(second_bar.i, second_bar.j);
    if (first_pos.is_adjacent_to(second_pos)) {
        return false;
    } else {
        first_ij[0]     = first_pos.i;
        first_ij[1]     = first_pos.j;
        second_ij[0]    = second_pos.i;
        second_ij[1]    = second_pos.j;
        first_energy    = pol_event.energy_value[first_pos.i][first_pos.j];
        second_energy   = pol_event.energy_value[second_pos.i][second_pos.j];
        return true;
    }

}

void EventCanvas::draw_event() {
    if (t_pol_event_tree_ == NULL)
        return;
    canvas_event_ = static_cast<TCanvas*>(gROOT->FindObject("canvas_event"));
    if (canvas_event_ == NULL) {
        canvas_event_ = new TCanvas("canvas_event", "POLAR Event Viewer", 800, 800);
        canvas_event_->Connect("Closed()", "EventCanvas", this, "CloseWindow()");
        canvas_event_->Connect("ProcessedEvent(Int_t, Int_t, Int_t, TObject*)", "EventCanvas",
                                 this, "ProcessAction(Int_t, Int_t, Int_t, TObject*)");
    }
    canvas_angle_ = static_cast<TCanvas*>(gROOT->FindObject("canvas_angle"));
    if (canvas_angle_ == NULL) {
        canvas_angle_ = new TCanvas("canvas_angle", "POLAR Event Viewer", 800, 800);
        // canvas_angle_->Connect("Closed()", "EventCanvas", this, "CloseWindow()");
        canvas_angle_->Connect("ProcessedEvent(Int_t, Int_t, Int_t, TObject*)", "EventCanvas",
                                 this, "ProcessAction(Int_t, Int_t, Int_t, TObject*)");
    }
    // read event
    if (queue_flag_) {
        if (entry_queue_.empty()) return;
        entry_current_ = entry_queue_.front();
        entry_queue_.pop();
        if (entry_current_ >= t_pol_event_tree_->GetEntries()) return;
        t_pol_event_tree_->GetEntry(entry_current_);
    } else {
        do {
            entry_current_ += entry_step_;
            if (entry_current_ >= t_pol_event_tree_->GetEntries()) {
                return;
            }
            t_pol_event_tree_->GetEntry(entry_current_);
        } while (t_pol_event_.is_ped || t_pol_event_.lost_count > 0);
    }

    /////////////////////////////////////////////////

    cout << endl;
    cout << " ############# Event " << entry_current_ << " ###################" << endl;
    cout << endl;

    ////////////////////////////////////////////////

    // prepare histogram
    if (trigger_map_ != NULL) {
        delete trigger_map_;
        trigger_map_ = NULL;
    }
    if (energy_map_ != NULL) {
        delete energy_map_;
        energy_map_ = NULL;
    }
    if (h_stack_ != NULL) {
        delete h_stack_;
        h_stack_ = NULL;
    }
    trigger_map_ = new TH2F("trigger_map", "trigger_map", 44, 0, 44, 44, 0, 44);
    trigger_map_->SetDirectory(NULL);
    trigger_map_->SetFillColor(6);
    energy_map_ = new TH2F("energy_map", "energy_map", 44, 0, 44, 44, 0, 44);
    energy_map_->SetDirectory(NULL);
    energy_map_->SetFillColor(kGreen);

    //////////////////////////

    double trig_adc_tot_sum = 0;
    int    trig_tot_n = 0;
    double trig_tot_mean = 0;
    double nontrig_adc_tot_sum = 0;
    int    nontrig_tot_n = 0;
    double nontrig_tot_mean = 0;

    //////////////////////////

    // fill event
    double max_energy = 0;
    double min_energy = 100000;
    for (int i = 0; i < 25; i++) {
        if (!t_pol_event_.time_aligned[i]) continue;
        for (int j = 0; j < 64; j++) {
            if ((t_pol_event_.channel_status[i][j] & POLEvent::ADC_NOT_READOUT) == 0) {
                int cur_x = ijtox(i, j) + ijtox(i, j) / 8 + 1;
                int cur_y = ijtoy(i, j) + ijtoy(i, j) / 8 + 1;
                double cur_energy = t_pol_event_.energy_value[i][j];
                if (cur_energy > max_energy) max_energy = cur_energy;
                if (cur_energy < min_energy) min_energy = cur_energy;
                energy_map_->SetBinContent(cur_x, cur_y, cur_energy);
            }
        }

        /////////////////////////////////////////////

        double trig_adc_sum = 0;
        int    trig_n = 0;
        double trig_mean = 0;
        double nontrig_adc_sum = 0;
        int    nontrig_n = 0;
        double nontrig_mean = 0;
        for (int j = 0; j < 64; j++) {
            if (t_pol_event_.trigger_bit[i][j]) {
                trig_adc_sum += t_pol_event_.energy_value[i][j];
                trig_n++;
            } else {
                nontrig_adc_sum += t_pol_event_.energy_value[i][j];
                nontrig_n++;
            }
        }
        trig_mean = trig_adc_sum / trig_n;
        nontrig_mean = nontrig_adc_sum / nontrig_n;
        cout << "CT_" << i + 1 << ": " << trig_mean - nontrig_mean << "   " << t_pol_event_.multiplicity[i] << endl;

        trig_adc_tot_sum += trig_mean;
        trig_tot_n++;
        nontrig_adc_tot_sum += nontrig_mean;
        nontrig_tot_n++;

        //////////////////////////////////////////////

    }

    //////////////////////////////

    trig_tot_mean = trig_adc_tot_sum / trig_tot_n;
    nontrig_tot_mean = nontrig_adc_tot_sum / nontrig_tot_n;
    cout << "TOTAL: " << trig_tot_mean - nontrig_tot_mean << "   " << t_pol_event_.trigger_n << endl;

    //////////////////////////////

    for (int i = 0; i < 25; i++) {
        if (!t_pol_event_.time_aligned[i]) continue;
        for (int j = 0; j < 64; j++) {
            if (t_pol_event_.trigger_bit[i][j]) {
                int cur_x = ijtox(i, j) + ijtox(i, j) / 8 + 1;
                int cur_y = ijtoy(i, j) + ijtoy(i, j) / 8 + 1;
                double cur_energy = energy_map_->GetBinContent(cur_x, cur_y);
                trigger_map_->SetBinContent(cur_x, cur_y, cur_energy);
                energy_map_->SetBinContent(cur_x, cur_y, 0);
            }
        }
    }

    h_stack_ = new THStack();
    h_stack_->Add(energy_map_);
    h_stack_->Add(trigger_map_);
    h_stack_->SetTitle(Form("Event: %ld", static_cast<long int>(entry_current_)));

    gStyle->SetOptStat(0);

    canvas_event_->cd();
    h_stack_->Draw("lego1 0");
    canvas_event_->Update();

    canvas_angle_->cd();

    const double METStartGPSTime = 1157984045;
    double event_gps_time = METStartGPSTime + t_pol_event_.event_time;
    trigger_map_->SetTitle(Form("event_time: %.9f", event_gps_time));
    trigger_map_->Draw("colz");

    if (angle_arrow_ != NULL) {
        delete angle_arrow_;
        angle_arrow_ = NULL;
    }

    int first_ij[2], second_ij[2];
    double first_energy, second_energy;
    if (find_first_two(t_pol_event_, first_ij, second_ij, first_energy, second_energy)) {
        int i1 = first_ij[0];
        int j1 = first_ij[1];
        int i2 = second_ij[0];
        int j2 = second_ij[1];
        double point_x1 = ijtox(i1, j1) + ijtox(i1, j1) / 8 + 0.5;
        double point_y1 = ijtoy(i1, j1) + ijtoy(i1, j1) / 8 + 0.5;
        double point_x2 = ijtox(i2, j2) + ijtox(i2, j2) / 8 + 0.5;
        double point_y2 = ijtoy(i2, j2) + ijtoy(i2, j2) / 8 + 0.5;
        angle_arrow_ = new TArrow(point_x1, point_y1, point_x2, point_y2, 0.02, ">");
        angle_arrow_->SetLineColor(kRed);
        angle_arrow_->SetLineWidth(2);
        angle_arrow_->Draw();
        cout << Form("CT_%02d_%02d => CT_%02d_%02d; %f => %f",
                i1 + 1, j1,
                i2 + 1, j2,
                first_energy, second_energy) << endl;
    }

    canvas_angle_->Update();
}

bool EventCanvas::read_entry_queue(const char* filename) {
    ifstream infile;
    infile.open(filename);
    if (!infile.is_open()) return false;
    long int entry_idx;
    while(true) {
        infile >> entry_idx;
        if (infile.eof()) break;
        if (entry_idx < 0) continue;
        entry_queue_.push(entry_idx);
    }
    infile.close();
    if (entry_queue_.empty()) {
        return false;
    } else {
        queue_flag_ = true;
        return true;
    }
}

