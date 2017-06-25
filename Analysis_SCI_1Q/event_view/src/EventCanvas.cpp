#include "EventCanvas.hpp"
#include <cstdlib>
#include <cmath>
#include <ctime>
#include <cstdio>

using namespace std;

EventCanvas::EventCanvas() {
    t_pol_event_file_ = NULL;
    t_pol_event_tree_ = NULL;
    trigger_map_ = NULL;
    energy_map_ = NULL;
    h_stack_ = NULL;
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

void EventCanvas::draw_event() {
    if (t_pol_event_tree_ == NULL)
        return;
    canvas_event_ = static_cast<TCanvas*>(gROOT->FindObject("canvas_event"));
    if (canvas_event_ == NULL) {
        canvas_event_ = new TCanvas("canvas_event", "POLAR Event Viewer", 1000, 1000);
        canvas_event_->Connect("Closed()", "EventCanvas", this, "CloseWindow()");
        canvas_event_->Connect("ProcessedEvent(Int_t, Int_t, Int_t, TObject*)", "EventCanvas",
                                 this, "ProcessAction(Int_t, Int_t, Int_t, TObject*)");
    }
    canvas_event_->cd();
    do {
        entry_current_ += entry_step_;
        if (entry_current_ >= t_pol_event_tree_->GetEntries()) {
            return;
        }
        t_pol_event_tree_->GetEntry(entry_current_);
    } while (t_pol_event_.is_ped || t_pol_event_.lost_count > 0);

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
    h_stack_->Draw("lego1 0");
    canvas_event_->Update();
}

