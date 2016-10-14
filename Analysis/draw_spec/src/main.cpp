#include <iostream>
#include "OptionsManager.hpp"
#include "RootInc.hpp"
#include "EventIterator.hpp"

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

    TApplication* rootapp = new TApplication("POLAR", NULL, NULL);

    EventIterator eventIter;
    if (!eventIter.open(options_mgr.decoded_data_filename.Data(),
                      options_mgr.begin_gps.Data(), options_mgr.end_gps.Data())) {
        cerr << "root file open failed: " << options_mgr.decoded_data_filename.Data() << endl;
        return 1;
    }

    if (!eventIter.file_is_1P()) {
        cout << "error: The opened file may be not 1P/1R SCI data file." << endl;
        eventIter.close();
        return 1;
    }
    
    eventIter.print_file_info();

    TH1F* hist_spec[25][64];
    TH1F* hist_spec_all[25][64];

    for (int i = 0; i < 25; i++) {
        for (int j = 0; j < 64; j++) {
            hist_spec[i][j] = new TH1F(Form("hist_spec_CT_%02d_%02d", i + 1, j + 1), Form("hist_spec_%02d_%02d", i + 1, j + 1), 128, 0, 4096);
            hist_spec_all[i][j] = new TH1F(Form("hist_spec_all_CT_%02d_%02d", i + 1, j + 1), Form("hist_spec_%02d_%02d", i + 1, j + 1), 128, 0, 4096);
        }   
    }   

    int pre_percent = 0;
    int cur_percent = 0;
    cout << "reading trigger data ... " << endl;
    cout << "[ " << flush;
    eventIter.phy_trigger_set_start();
    while (eventIter.phy_trigger_next_event()) {
        cur_percent = static_cast<int>(100 * eventIter.phy_trigger_get_cur_entry() / eventIter.phy_trigger_get_tot_entries());
        if (cur_percent - pre_percent > 0 && cur_percent % 2 == 0) {
            pre_percent = cur_percent;
            cout << "#" << flush;
        }
        if (eventIter.t_trigger.is_bad > 0) 
            continue;
//        if (eventIter.t_trigger.trigger_n > 4)
//            continue;
        while (eventIter.phy_modules_next_packet()) {
//            if (eventIter.t_modules. multiplicity > 4)
//                continue;
            int idx = eventIter.t_modules.ct_num - 1;
            for (int j = 0; j < 64; j++) {
                if (eventIter.t_modules.energy_adc[j] == 4095)
                    continue;
                hist_spec_all[idx][j]->Fill(eventIter.t_modules.energy_adc[j]);
                if (eventIter.t_modules.trigger_bit[j]) {
                    hist_spec[idx][j]->Fill(eventIter.t_modules.energy_adc[j]);
                }
            }
        }
    }
    cout << " DONE ]" << endl;

    TCanvas* canvas_spec[25]; 
    for (int i = 0; i < 25; i++) {
        canvas_spec[i] = new TCanvas(Form("canvas_spec_CT_%02d", i + 1), Form("Spectrum of CT_%02d", i + 1), 800, 600);
        canvas_spec[i]->SetFillColor(kYellow);
        canvas_spec[i]->Divide(8, 8);
        for (int j = 0; j < 64; j++) {
            int c = 8 * (7 - j / 8) + (7 - j % 8) + 1;
            canvas_spec[i]->cd(c);
            canvas_spec[i]->GetPad(c)->SetFillColor(kWhite);
            canvas_spec[i]->GetPad(c)->SetLogy();
            hist_spec_all[i][j]->SetLineColor(kBlue);
            hist_spec_all[i][j]->Draw();
            hist_spec[i][j]->SetLineColor(kRed);
            hist_spec[i][j]->Draw("SAME"); 
        } 
    }

    rootapp->Run();
    
    return 0;
}
