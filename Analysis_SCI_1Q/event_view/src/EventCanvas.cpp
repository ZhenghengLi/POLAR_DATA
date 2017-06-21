#include "EventCanvas.hpp"
#include <cstdlib>
#include <cmath>
#include <ctime>
#include <cstdio>

using namespace std;

EventCanvas::EventCanvas() {
    t_pol_event_file_ = NULL;
    t_pol_event_tree_ = NULL;
    trigger_map_ = new TH2F("trigger_map", "trigger_map", 44, 0, 44, 44, 0, 44);
    trigger_map_->SetDirectory(NULL);
    trigger_map_->SetFillColor(kRed);
    energy_map_ = new TH2F("energy_map", "energy_map", 44, 0, 44, 44, 0, 44);
    energy_map_->SetDirectory(NULL);
    energy_map_->SetFillColorAlpha(kBlue, 0.5);
    h_stack_ = new THStack();
    h_stack_->Add(energy_map_);
    h_stack_->Add(trigger_map_);
}

EventCanvas::~EventCanvas() {
    delete trigger_map_;
    delete energy_map_;
    delete h_stack_;
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
    t_pol_event_.deactive_all(t_pol_event_tree_);
    t_pol_event_.active(t_pol_event_tree_, "trigger_bit");
    t_pol_event_.active(t_pol_event_tree_, "energy_value");
    t_pol_event_.active(t_pol_event_tree_, "channel_status");
    t_pol_event_.active(t_pol_event_tree_, "time_aligned");
    t_pol_event_.active(t_pol_event_tree_, "lost_count");
    t_pol_event_.active(t_pol_event_tree_, "is_ped");

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
    if (event != kKeyPress)
        return;
    draw_event();
}

void EventCanvas::draw_event() {
    if (t_pol_event_tree_ == NULL)
        return;
    canvas_event_ = static_cast<TCanvas*>(gROOT->FindObject("canvas_event"));
    if (canvas_event_ == NULL) {
        canvas_event_ = new TCanvas("canvas_event", "POLAR Event Viewer", 1000, 600);
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

    // clear map
    for (int x = 1; x <= 44; x++) {
        for (int y = 1; y <= 44; y++) {
            trigger_map_->SetBinContent(x, y, 0);
            energy_map_->SetBinContent(x, y, 0);
        }
    }

    // fill event
    for (int i = 0; i < 25; i++) {
        if (!t_pol_event_.time_aligned[i]) continue;
        for (int j = 0; j < 64; j++) {
            if ((t_pol_event_.channel_status[i][j] & POLEvent::ADC_NOT_READOUT) == 0) {
                int cur_x = ijtox(i, j) + ijtox(i, j) / 8 + 1;
                int cur_y = ijtoy(i, j) + ijtoy(i, j) / 8 + 1;
                energy_map_->SetBinContent(cur_x, cur_y, t_pol_event_.energy_value[i][j]);
            }
        }
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

    h_stack_->Draw("lego1");
    canvas_event_->Modified();
    // canvas_event_->Update();
}

