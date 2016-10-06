#include <RecEventType.hpp>

RecEventType::RecEventType() {

}

void RecEventType::bind_rec_event_tree(TTree* t_rec_event_tree, RecEvent_T& t_rec_event) {
    t_rec_event_tree->SetBranchAddress("abs_gps_week",       &t_rec_event.abs_gps_week     );
    t_rec_event_tree->SetBranchAddress("abs_gps_second",     &t_rec_event.abs_gps_second   );
    t_rec_event_tree->SetBranchAddress("abs_gps_valid",      &t_rec_event.abs_gps_valid    );
    t_rec_event_tree->SetBranchAddress("abs_ship_second",    &t_rec_event.abs_ship_second  );
    t_rec_event_tree->SetBranchAddress("type",               &t_rec_event.type             );
    t_rec_event_tree->SetBranchAddress("trig_accepted",       t_rec_event.trig_accepted    );
    t_rec_event_tree->SetBranchAddress("trigger_bit",         t_rec_event.trigger_bit      );
    t_rec_event_tree->SetBranchAddress("trigger_n",          &t_rec_event.trigger_n        );
    t_rec_event_tree->SetBranchAddress("multiplicity",        t_rec_event.multiplicity     );
    t_rec_event_tree->SetBranchAddress("energy_dep",          t_rec_event.energy_dep       );
}
