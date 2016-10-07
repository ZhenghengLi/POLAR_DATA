#include <iostream>
#include "OptionsManager.hpp"
#include "RootInc.hpp"
#include "RecEventIterator.hpp"
#include "CommonCanvas.hpp"

#define MIN_ENERGY 0
#define MAX_ENERGY 1000
#define CEN_ENERGY 200
#define NBINS 100

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

	RecEventIterator recEventIter;
	if (!recEventIter.open(options_mgr.event_data_filename.Data(), options_mgr.begin_gps.Data(), options_mgr.end_gps.Data())) {
		cout << "Event file open failed." << endl;
		return 1;
	}

	recEventIter.print_file_info();

	double begin_ship_time = recEventIter.begin_rec_event.abs_ship_second;
	double pre_ship_time = begin_ship_time;
	double cur_ship_time;

    TApplication* rootapp = new TApplication("POLAR", NULL, NULL);

	float binlist[3] = {MIN_ENERGY, CEN_ENERGY, MAX_ENERGY};
	TH1F* energy_hist = new TH1F("energy_hist", "energy_hist", 2, binlist);
	energy_hist->SetDirectory(NULL);
	energy_hist->SetMinimum(0);

	TGraphErrors* graph_rate  = new TGraphErrors();
	graph_rate->SetTitle("Rate");
	graph_rate->SetLineColor(kRed);
	TGraphErrors* graph_ratio = new TGraphErrors();
	graph_ratio->SetTitle("Ratio");
	graph_ratio->SetLineColor(kGreen);
	int point_num = -1;

	int pre_percent = 0;
	int cur_percent = 0;
	cout << "reading reconstructed data ... " << endl;
	cout << "[ " << flush;
	recEventIter.set_start();
	while (recEventIter.next_event()) {
		cur_percent = static_cast<int>(100 * recEventIter.get_cur_entry() / recEventIter.get_total_entries());
		if (cur_percent - pre_percent > 0 && cur_percent % 2 == 0) {
			pre_percent = cur_percent;
			cout << "#" << flush;
		}
		if (!recEventIter.t_rec_event.abs_gps_valid) 
			continue;
		if (recEventIter.t_rec_event.type != 0x00FF)
			continue;
		if (recEventIter.t_rec_event.trigger_n < options_mgr.min_bars || recEventIter.t_rec_event.trigger_n > options_mgr.max_bars)
			continue;
		if (recEventIter.find_first_two_bars() && recEventIter.cur_is_na22())
			continue;
		double dep_erg_sum = 0;
		for (int i = 0; i < 25; i++) {
			if (!recEventIter.t_rec_event.trig_accepted[i])
				continue;
			for (int j = 0; j < 64; j++) {
				if (recEventIter.t_rec_event.trigger_bit[i * 64 + j]) {
					double cur_erg = recEventIter.t_rec_event.energy_dep[i * 64 + j];
					dep_erg_sum += (cur_erg > 0 ? cur_erg : 0);
				}
			}
		}
		cur_ship_time = recEventIter.t_rec_event.abs_ship_second;
		if (cur_ship_time - pre_ship_time < options_mgr.time_bin_width) {
			energy_hist->Fill(dep_erg_sum);
		} else {
			point_num += 1;
			graph_rate->SetPoint(point_num, (cur_ship_time + pre_ship_time) / 2, energy_hist->GetEntries() / (cur_ship_time - pre_ship_time));
			graph_ratio->SetPoint(point_num, (cur_ship_time + pre_ship_time) / 2, energy_hist->GetBinContent(2) / energy_hist->GetBinContent(1));
			pre_ship_time = cur_ship_time;
			energy_hist->Reset();	
		}
	}
	cout << " DONE ]" << endl;
	recEventIter.close();

	// draw
	CommonCanvas commonCanvas;
	commonCanvas.cd_spec(1);
	graph_rate->Draw();
	commonCanvas.cd_spec(2);
	graph_ratio->Draw();

	rootapp->Run();
    return 0;
}
