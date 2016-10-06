#include <iostream>
#include "OptionsManager.hpp"
#include "RootInc.hpp"
#include "RecEventIterator.hpp"
#include "CommonCanvas.hpp"

#define MIN_ENERGY 0
#define MAX_ENERGY 1000
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
    TApplication* rootapp = new TApplication("POLAR", NULL, NULL);


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
		if (recEventIter.t_rec_event.type != 0x00FF)
			continue;
		double dep_erg_sum = 0;
		for (int i = 0; i < 25; i++) {
			if (!recEventIter.t_rec_event.trig_accepted[i]) {
				continue;
			}
			for (int j = 0; j < 64; j++) {
				if (recEventIter.t_rec_event.trigger_bit[i * 64 + j]) {
					double cur_erg = recEventIter.t_rec_event.energy_dep[i * 64 + j];
					dep_erg_sum += (cur_erg > 0 ? cur_erg : 0);
				}
			}
		}
	}
	cout << " DONE ]" << endl;
	recEventIter.close();

	// draw
	CommonCanvas commonCanvas;
	commonCanvas.cd_spec();

	rootapp->Run();
    return 0;
}
