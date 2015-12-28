struct Xtalk_Point_T {
    int i; 
    int jx; 
    int jy;
    Float_t x;
    Float_t y;
};

void xtalk() {
    TFile* f1 = new TFile("output/xtalk_test.root", "READ");
    TTree* t_xtalk = static_cast<TTree*>(f1->Get("t_xtalk"));
    Xtalk_Point_T xtalk_point;
    t_xtalk->SetBranchAddress("xtalk_point", &xtalk_point.i);
    Long64_t tot_entries = t_xtalk->GetEntries();
    cout << tot_entries << endl;
    int counter = 0;
    for (int i = 0; i < tot_entries; i++) {
        t_xtalk->GetEntry(i);
        if (xtalk_point.i == 5 && xtalk_point.jx == 9 && xtalk_point.jy == 40)
            counter++;
    }
    cout << "counter: " << counter << endl;
}
