void geo_correct() {
    cout << " - reading mod_curve_h: " << endl;
    string mod_curve_h_fn;
    cout << " - mod_curve_h_fn = " << flush;
    cin >> mod_curve_h_fn;
    TFile* curve_file;
    curve_file = new TFile(mod_curve_h_fn.c_str(), "read");
    if (curve_file->IsZombie()) {
        cout << "root file open failed." << endl;
        return;
    }
    TH1F* curve_hist;
    curve_hist = static_cast<TH1F*>(curve_file->Get("mod_curve"));
    TH1F* mod_curve_h = curve_hist->Clone("mod_curve_h");
    mod_curve_h->SetDirectory(NULL);
    curve_file->Close();
    
    cout << " - reading mod_curve_v: " << endl;
    string mod_curve_v_fn;
    cout << " - mod_curve_v_fn = " << flush;
    cin >> mod_curve_v_fn;
    curve_file = new TFile(mod_curve_v_fn.c_str(), "read");
    if (curve_file->IsZombie()) {
        cout << "root file open failed." << endl;
        return;
    }
    curve_hist = static_cast<TH1F*>(curve_file->Get("mod_curve"));
    TH1F* mod_curve_v = curve_hist->Clone("mod_curve_v");
    mod_curve_v->SetDirectory(NULL);
    curve_file->Close();

    TCanvas* canvas_h = new TCanvas("canvas_h", "canvas_h", 800, 600);
    canvas_h->cd();
    mod_curve_h->Draw();

    TCanvas* canvas_v = new TCanvas("canvas_v", "canvas_v", 800, 600);
    canvas_v->cd();
    mod_curve_v->Draw();

    TH1F* mod_curve_hv = new TH1F("mod_curve_hv", "mod_curve_hv", 60, 0, 360);
    mod_curve_hv->Add(mod_curve_h);
    mod_curve_hv->Add(mod_curve_v);
    TCanvas* canvas_hv = new TCanvas("canvas_hv", "canvas_hv", 800, 600);
    canvas_hv->cd();
    mod_curve_hv->Draw();

    TH1F* mod_curve_h_geo = mod_curve_h->Clone("mod_curve_h_geo");
    TH1F* mod_curve_v_geo = mod_curve_v->Clone("mod_curve_v_geo");

    for (int i = 1; i <= 60; i++) {
        mod_curve_h_geo->SetBinContent(i, mod_curve_h_geo->GetBinContent(i) / mod_curve_hv->GetBinContent(i) * mod_curve_hv->GetEntries() / 60.0);
        mod_curve_v_geo->SetBinContent(i, mod_curve_v_geo->GetBinContent(i) / mod_curve_hv->GetBinContent(i) * mod_curve_hv->GetEntries() / 60.0);
    }

    gStyle->SetOptStat(11);
    gStyle->SetOptFit(111);

    TF1* curve_h_func = new TF1("curve_h_func", "[0] + [1] * TMath::Cos(2 * ((x - [2]) / 180 * TMath::Pi() + TMath::Pi() / 2))", 0, 360);
    curve_h_func->SetParameters(500, 800, 90);
    curve_h_func->SetParName(0, "B");
    curve_h_func->SetParName(1, "A");
    curve_h_func->SetParName(2, "F");
    TF1* curve_v_func = new TF1("curve_v_func", "[0] + [1] * TMath::Cos(2 * ((x - [2]) / 180 * TMath::Pi() + TMath::Pi() / 2))", 0, 360);
    curve_v_func->SetParameters(500, 800, 90);
    curve_v_func->SetParName(0, "B");
    curve_v_func->SetParName(1, "A");
    curve_v_func->SetParName(2, "F");

    TCanvas* canvas_h_geo = new TCanvas("canvas_h_geo", "canvas_h_geo", 800, 600);
    canvas_h_geo->cd();
    mod_curve_h_geo->Fit(curve_h_func, "RQ");
    double A_h = curve_h_func->GetParameter(1);
    double B_h = curve_h_func->GetParameter(0);
    double F_h = curve_h_func->GetParameter(2);
    cout << " -----------------------------" << endl;
    cout << " - F_h = " << F_h << endl;
    cout << " - A_h = " << A_h << endl;
    cout << " - B_h = " << B_h << endl;
    cout << " - m_h = " << A_h / B_h << endl;

    TCanvas* canvas_v_geo = new TCanvas("canvas_v_geo", "canvas_v_geo", 800, 600);
    canvas_v_geo->cd();
    mod_curve_v_geo->Fit(curve_v_func, "RQ");
    double A_v = curve_v_func->GetParameter(1);
    double B_v = curve_v_func->GetParameter(0);
    double F_v = curve_v_func->GetParameter(2);
    cout << " -----------------------------" << endl;
    cout << " - F_v = " << F_v << endl;
    cout << " - A_v = " << A_v << endl;
    cout << " - B_v = " << B_v << endl;
    cout << " - m_v = " << A_v / B_v << endl;

}
