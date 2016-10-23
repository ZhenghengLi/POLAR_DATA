void gen_curve() {
    cout << " - starting ... " << endl;
    string angle_filename;
    cout << " - angle_filename = " << flush;
    cin >> angle_filename;
    TFile* angle_file = new TFile(angle_filename.c_str(), "read");
    if (angle_file->IsZombie()) {
        cout << "angle root file open failed." << endl;
        return;
    }
    TTree* t_angle_tree = static_cast<TTree*>(angle_file->Get("t_angle"));
    if (t_angle_tree == NULL) {
        cout << "t_angle not found." << endl;
        return;
    }
    struct {
        Int_t     first_ij[2];
        Int_t     second_ij[2];
        Float_t   rand_angle;
        Float_t   first_dep;
        Float_t   second_dep;
    } t_angle;
    t_angle_tree->SetBranchAddress("first_ij",      t_angle.first_ij      );
    t_angle_tree->SetBranchAddress("second_ij",     t_angle.second_ij     );
    t_angle_tree->SetBranchAddress("rand_angle",   &t_angle.rand_angle    );
    t_angle_tree->SetBranchAddress("first_dep",    &t_angle.first_dep     );
    t_angle_tree->SetBranchAddress("second_dep",   &t_angle.second_dep    );

    string cut_str = "second_dep > 0";
    t_angle_tree->Draw(">>elist", cut_str.c_str());
    TEventList* elist = static_cast<TEventList*>(gDirectory->Get("elist"));

    cout << " - total entries: " << elist->GetN() << endl;
    string select_sum_str;
    cout << " - select_sum = " << flush;
    cin >> select_sum_str;
    int select_sum = atoi(select_sum_str.c_str());
    if (select_sum < 0 || select_sum > elist->GetN()) return;
    double step = static_cast<double>(elist->GetN()) / static_cast<double>(select_sum);

    string curve_filename;
    cout << " - curve_filename = " << flush;
    cin >> curve_filename;
    TFile* curve_file = new TFile(curve_filename.c_str(), "recreate");
    if (curve_file->IsZombie()) {
        cout << "curve root file open failed." << endl;
        return;
    }
    TH1F* mod_curve = new TH1F("mod_curve", "modulation curve", 60, 0, 360);

    cout << " - reading angle data ..." << endl;
    for (int i = 0; i < select_sum; i++) {
        t_angle_tree->GetEntry(elist->GetEntry(static_cast<int>(i * step)));
        mod_curve->Fill(t_angle.rand_angle);
    }

    angle_file->Close();

    curve_file->cd();
    mod_curve->Write();
    curve_file->Close();

}
