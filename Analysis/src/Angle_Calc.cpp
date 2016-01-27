#include <iostream>
#include <cstdlib>
#include <queue>
#include "RootInc.hpp"
#include "PhyEventFile.hpp"
#include "CooConv.hpp"

using namespace std;

struct Bar {
    Float_t energy;
    int i;
    int j;
    
    bool operator<(const Bar& right) const {
        return (energy < right.energy);
    }
    bool operator<=(const Bar& right) const {
        return (energy <= right.energy);
    }
    bool operator>(const Bar& right) const {
        return (energy > right.energy);
    }
    bool operator>=(const Bar& right) const {
        return (energy >= right.energy);
    }
    bool operator==(const Bar& right) const {
        return (energy == right.energy);
    }
};

Bar make_Bar(Float_t energy_par, int i_par, int j_par) {
    Bar tmp_bar;
    tmp_bar.energy = energy_par;
    tmp_bar.i = i_par;
    tmp_bar.j = j_par;
    return tmp_bar;
}

bool is_adjacent(const Bar& bar1, const Bar& bar2) {
    if (bar1.i != bar2.i) {
        return false;
    } else {
        int j_diff = abs(bar1.j - bar2.j);
        if (j_diff == 1 || j_diff == 8 || j_diff == 7 || j_diff == 9)
            return true;
        else
            return false;
    }
}

Double_t angle_calc(const Bar& bar1, const Bar& bar2) {
    int x1 = ijtox(bar1.i, bar1.j);
    int x2 = ijtox(bar2.i, bar2.j);
    int y1 = ijtoy(bar1.i, bar1.j);
    int y2 = ijtoy(bar2.i, bar2.j);
    Double_t abs_x1 = x1 / 8 * 60 + x1 % 8 * 6.08 + 5.85 * gRandom->Uniform();
    Double_t abs_x2 = x2 / 8 * 60 + x2 % 8 * 6.08 + 5.85 * gRandom->Uniform();
    Double_t abs_y1 = y1 / 8 * 60 + y1 % 8 * 6.08 + 5.85 * gRandom->Uniform();
    Double_t abs_y2 = y2 / 8 * 60 + y2 % 8 * 6.08 + 5.85 * gRandom->Uniform();
    Double_t angle = TMath::ATan2(abs_y2 - abs_y1, abs_x2 - abs_x1);
    angle = (angle > 0 ? angle : angle + 2 * TMath::Pi());
    return angle / TMath::Pi() * 180;
}

int main(int argc, char** argv) {

    TString infile_name;
    TString outfile_name;

    TString cur_par_str;
    int argv_idx = 0;
    while (argv_idx < argc - 1) {
        cur_par_str = argv[++argv_idx];
        if (cur_par_str == "-o")
            outfile_name = argv[++argv_idx];
        else
            infile_name = cur_par_str;
    }

    if (infile_name.IsNull() || outfile_name.IsNull()) {
        cout << "USAGE: " << argv[0] << " <infile_name.root> -o <outfile_name.root>" << endl;
        exit(1);
    }

    priority_queue<Bar> bar_queue;
    Bar first_bar, second_bar;
    bool found_not_adjacent = false;
    int pre_percent = 0;
    int cur_percent = 0;

    PhyEventFile phyEventFile_R;
    if (!phyEventFile_R.open(infile_name.Data(), 'r')) {
        cerr << "root file open failed: " << infile_name.Data() << endl;
        exit(1);
    }

    TFile* t_outfile = new TFile(outfile_name.Data(), "RECREATE");
    TTree* t_angle_tree = new TTree("t_angle", "Azimuth angle of Compton Scattering");
    t_angle_tree->SetDirectory(t_outfile);
    Double_t b_angle;
    t_angle_tree->Branch("angle", &b_angle, "angle/D");

    cout << "Calculating Angle ... " << endl;
    cout << "[ #" << flush;
    phyEventFile_R.trigg_restart();
    while (phyEventFile_R.trigg_next()) {
        cur_percent = static_cast<int>(100 * phyEventFile_R.current_index() / phyEventFile_R.total_entries());
        if (cur_percent - pre_percent > 0 && cur_percent % 2 == 0) {
            pre_percent = cur_percent;
            cout << "#" << flush;
        }
        if (phyEventFile_R.trigg.lost_count > 0)
            continue;
        if (phyEventFile_R.trigg.mode != 0x00FF)
            continue;
        while (!bar_queue.empty()) {
            bar_queue.pop();
        }
        while (phyEventFile_R.event_next()) {
            int idx = phyEventFile_R.event.ct_num - 1;
            for (int j = 0; j < 64; j++) {
                if (phyEventFile_R.event.trigger_bit[j]) {
                    bar_queue.push(make_Bar(phyEventFile_R.event.energy_ch[j], idx, j));
                }
            }
        }
        if (bar_queue.empty()){
            continue;
        }
        first_bar = bar_queue.top();
        bar_queue.pop();
        found_not_adjacent = false;
        while (!bar_queue.empty()) {
            second_bar = bar_queue.top();
            bar_queue.pop();
            if (!is_adjacent(first_bar, second_bar)) {
                found_not_adjacent = true;
                break;
            }
        }
        if (found_not_adjacent && second_bar.energy > 60.0) {
            b_angle = angle_calc(first_bar, second_bar);
            t_angle_tree->Fill();
        }
    }

    t_angle_tree->Write();
    t_outfile->Close();
    cout << " DONE ]" << endl;
    
    phyEventFile_R.close();
    
    return 0;
}
