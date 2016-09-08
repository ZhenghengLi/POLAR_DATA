#include <iostream>
#include "TFile.h"
#include "TTree.h"
#include "TNamed.h"
#include "TSystem.h"
#include "TTimeStamp.h"

using namespace std;

int main(int argc, char** argv) {
    if (argc < 3) {
        cout << "USAGE: " << argv[0] << " <sci_data_1P/1R.root> <trigger_time.root>" << endl;
        return 1;
    }
    string infilename  = argv[1];
    string outfilename = argv[2];
    TFile* t_file_in = new TFile(infilename.c_str(), "READ");
    if (t_file_in->IsZombie()) {
        cout << "root file open failed: " << infilename << endl;
        return 1;
    }
    TTree* t_trigger_tree = static_cast<TTree*>(t_file_in->Get("t_trigger"));
    if (t_trigger_tree == NULL) {
        cout << "cannot find TTree t_trigger." << endl;
        t_file_in->Close();
        return 1;
    }
    if (t_trigger_tree->FindBranch("abs_gps_week") == NULL ||
        t_trigger_tree->FindBranch("abs_gps_second") == NULL ||
        t_trigger_tree->FindBranch("abs_gps_valid") == NULL) {
        cout << "The opened root file may be not a 1P/1R SCI data file." << endl;
        t_file_in->Close();
        return 1;
    }
    TNamed* m_phy_gps = static_cast<TTree*>(t_file_in->Get("m_phy_gps"));
    if (m_phy_gps == NULL) {
        cout << "cannot find TNamed m_phy_gps." << endl;
        t_file_in->Close();
    }

    struct {
        Int_t    type;
        Int_t    abs_gps_week;
        Double_t abs_gps_second;
        Bool_t   abs_gps_valid;
        Bool_t   trig_accepted[25];
    } t_trigger_time;

    t_trigger_tree->SetBranchAddress("type",           &t_trigger_time.type           );
    t_trigger_tree->SetBranchAddress("abs_gps_week",   &t_trigger_time.abs_gps_week   );
    t_trigger_tree->SetBranchAddress("abs_gps_second", &t_trigger_time.abs_gps_second );
    t_trigger_tree->SetBranchAddress("abs_gps_valid",  &t_trigger_time.abs_gps_valid  );
    t_trigger_tree->SetBranchAddress("trig_accepted",   t_trigger_time.trig_accepted  );

    TFile* t_file_out = new TFile(outfilename.c_str(), "RECREATE", "trigger_time_file", 0);
    if (t_file_out->IsZombie()) {
        cout << "root file open failed: " << outfilename << endl;
        t_file_in->Close();
        return 1;
    }
    t_file_out->cd();
    TTree* t_trigger_time_tree = new TTree("t_trigger", "trigger time");
    t_trigger_time_tree->Branch("type",            &t_trigger_time.type,            "type/I"              );
    t_trigger_time_tree->Branch("abs_gps_week",    &t_trigger_time.abs_gps_week,    "abs_gps_week/I"      );
    t_trigger_time_tree->Branch("abs_gps_second",  &t_trigger_time.abs_gps_second,  "abs_gps_second/D"    );
    t_trigger_time_tree->Branch("abs_gps_valid",   &t_trigger_time.abs_gps_valid,   "abs_gps_valie/O"     );
    t_trigger_time_tree->Branch("trig_accepted",    t_trigger_time.trig_accepted,   "trig_accepted[25]/O" );
    
    Long64_t tot_entries = t_trigger_tree->GetEntries();
    int pre_percent = 0;
    int cur_percent = 0;
    cout << "Extracting trigger time data from file: " << TSystem().BaseName(infilename.c_str()) << " ..." << endl;
    cout << "[ " << flush;
    for (Long64_t cur_entry = 0; cur_entry < tot_entries; cur_entry++) {
        cur_percent = static_cast<int>(100 * cur_entry / tot_entries);
        if (cur_percent - pre_percent > 0 && cur_percent % 2 == 0) {
            pre_percent = cur_percent;
            cout << "#" << flush;
        }
        t_trigger_tree->GetEntry(cur_entry);
        t_trigger_time_tree->Fill();
    }
    cout << "# DONE ]" << endl;
    
    t_file_out->cd();
    t_trigger_time_tree->Write();

    TNamed* cur_meta;
    cur_meta = new TNamed("m_dattype", "TRIGGER TIME DATA");
    cur_meta->Write();
    delete cur_meta;
    cur_meta = new TNamed("m_version", "extract_trigger v1.0.0");
    cur_meta->Write();
    delete cur_meta;
    TTimeStamp* cur_time = new TTimeStamp();
    cur_meta = new TNamed("m_gentime", cur_time->AsString("lc"));
    cur_meta->Write();
    delete cur_time;
    cur_time = NULL;
    delete cur_meta;
    cur_meta = new TNamed("m_dcdfile", TSystem().BaseName(infilename.c_str()));
    cur_meta = NULL;
    m_phy_gps->Write();

    t_file_in->Close();
    t_file_out->Close();
                          
    return 0;
}
