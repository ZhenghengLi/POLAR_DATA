#include <iostream>
#include "RootInc.hpp"

using namespace std;

int main(int argc, char** argv) {
    if (argc < 5) {
        cout << "USAGE: " << argv[0] << " <event_data.root> <intensity.root> <rate.root> <output.root>" << endl;
        return 2;
    }
    string event_data_fn = argv[1];
    string intensity_fn = argv[2];
    string rate_fn = argv[3];
    string output_fn = argv[4];

    // open event_data
    TFile* t_event_file = new TFile(event_data_fn.c_str(), "read");
    if (t_event_file->IsZombie()) {
        cout << "event root file open failed." << endl;
        return 1;
    }
    TTree* t_event_tree = static_cast<TTree*>(t_event_file->Get("t_event"));
    if (t_event_tree == NULL) {
        cout << "cannot find TTree t_event" << endl;
        return 1;
    }
    // open intensity file
    TFile* t_beam_intensity_file = new TFile(intensity_fn.c_str(), "read");
    if (t_beam_intensity_file->IsZombie()) {
        cout << "intensity root file open failed." << endl;
        return 1;
    }
    TTree* t_beam_intensity_tree = static_cast<TTree*>(t_beam_intensity_file->Get("t_beam_intensity"));
    if (t_beam_intensity_tree == NULL) {
        cout << "cannot find TTree t_beam_intensity" << endl;
        return 1;
    }
    t_event_tree->AddFriend(t_beam_intensity_tree);
    struct {
        Double_t ct_time_second;
        Bool_t   time_aligned[25];
        Bool_t   trigger_bit[25][64];
        Double_t current;
    } t_event;
    t_event_tree->SetBranchAddress("ct_time_second",  &t_event.ct_time_second   );
    t_event_tree->SetBranchAddress("time_aligned",     t_event.time_aligned     );
    t_event_tree->SetBranchAddress("trigger_bit",      t_event.trigger_bit      );
    t_event_tree->SetBranchAddress("current",         &t_event.current          );
    t_event_tree->SetBranchStatus("*", false);
    t_event_tree->SetBranchStatus("ct_time_second", true);
    t_event_tree->SetBranchStatus("time_aligned", true);
    t_event_tree->SetBranchStatus("trigger_bit", true);
    t_event_tree->SetBranchStatus("current", true);



    return 0;
}
