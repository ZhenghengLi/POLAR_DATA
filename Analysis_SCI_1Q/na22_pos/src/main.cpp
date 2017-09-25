#include <iostream>
#include <vector>
#include "RootInc.hpp"
#include "DetInfo.hpp"
#include "BarPos.hpp"
#include "CommonCanvas.hpp"

using namespace std;

struct Line {
    Line() {  }
    Line(double x1_par, double y1_par, double x2_par, double y2_par) {
        x1 = x1_par;
        y1 = y1_par;
        x2 = x2_par;
        y2 = y2_par;
    }
    double angle_to(const Line& otherLine) {
        TVectorD vec1(2);
        vec1(0) = x2 - x1;
        vec1(1) = y2 - y1;
        TVectorD vec2(2);
        vec2(0) = otherLine.x2 - otherLine.x1;
        vec2(1) = otherLine.y2 - otherLine.y1;
        double dotvalue = vec1(0) * vec2(0) + vec1(1) * vec2(1);
        double abs1 = TMath::Sqrt(vec1(0) * vec1(0) + vec1(1) * vec1(1));
        double abs2 = TMath::Sqrt(vec2(0) * vec2(0) + vec2(1) * vec2(1));
        double angle = TMath::ACos(dotvalue / (abs1 * abs2)) / TMath::Pi() * 180.0;
        return (angle > 90 ? 180 - angle : angle);
    }
    double x1;
    double y1;
    double x2;
    double y2;
};

struct CrossPoint {
    CrossPoint(double x_par, double y_par) {
        x = x_par;
        y = y_par;
    }
    double x;
    double y;
};

CrossPoint calc_crosspoint(Line line1, Line line2) {
    double A1 = line1.y2 - line1.y1;
    double B1 = line1.x1 - line1.x2;
    double C1 = line1.x1 * line1.y2 - line1.x2 * line1.y1;
    double A2 = line2.y2 - line2.y1;
    double B2 = line2.x1 - line2.x2;
    double C2 = line2.x1 * line2.y2 - line2.x2 * line2.y1;
    TMatrixD mat(2, 2);
    TVectorD vec(2);
    TVectorD vec_res(2);
    mat(0, 0) = A1;
    mat(0, 1) = B1;
    mat(1, 0) = A2;
    mat(1, 1) = B2;
    mat.Invert();
    vec(0) = C1;
    vec(1) = C2;
    vec_res = mat * vec;

    return CrossPoint(vec_res[0], vec_res[1]);
}

int main(int argc, char** argv) {
    if (argc < 3) {
        cout << "USAGE: " << argv[0] << " <source_id> <na22_tag_1.root> <na22_tag_2.root> ..." << endl;
        return 2;
    }

    TApplication* rootapp = new TApplication("POLAR", NULL, NULL);

    int target_source_id = TString(argv[1]).Atoi();
    if (target_source_id < 0 || target_source_id > 3) {
        cout << "wrong source id, it should be one of 0, 1, 2, 3." << endl;
        return 1;
    }

    // open na22_flag_file
    struct {
        Double_t event_time;
        Bool_t   is_valid;
        Bool_t   is_na22;
        Int_t    first_ij[2];
        Int_t    second_ij[2];
        Int_t    source_id;
        Float_t  angle;
        Float_t  distance;
    } t_na22_flag;
    TTree* t_na22_flag_tree = NULL;
    TFile* na22_flag_file = NULL;

    vector<Line> line_vec;
    Pos first_pos;
    Pos second_pos;

    for (int q = 2; q < argc; q++) {
        cout << argv[q] << " ..." << endl;
        na22_flag_file = new TFile(argv[q], "read");
        if (na22_flag_file->IsZombie()) {
            cout << "na22_flag_file open failed." << endl;
            return 1;
        }
        t_na22_flag_tree = static_cast<TTree*>(na22_flag_file->Get("t_na22_flag"));
        if (t_na22_flag_tree == NULL) {
            cout << "cannot find TTree t_na22_flag" << endl;
            return 1;
        }
        t_na22_flag_tree->SetBranchAddress("event_time",    &t_na22_flag.event_time   );
        t_na22_flag_tree->SetBranchAddress("is_valid",      &t_na22_flag.is_valid     );
        t_na22_flag_tree->SetBranchAddress("is_na22",       &t_na22_flag.is_na22      );
        t_na22_flag_tree->SetBranchAddress("first_ij",       t_na22_flag.first_ij     );
        t_na22_flag_tree->SetBranchAddress("second_ij",      t_na22_flag.second_ij    );
        t_na22_flag_tree->SetBranchAddress("source_id",     &t_na22_flag.source_id    );
        t_na22_flag_tree->SetBranchAddress("angle",         &t_na22_flag.angle        );
        t_na22_flag_tree->SetBranchAddress("distance",      &t_na22_flag.distance     );

        int pre_percent = 0;
        int cur_percent = 0;
        cout << "[ " << flush;
        for (Long64_t e = 0; e < t_na22_flag_tree->GetEntries(); e++) {
            cur_percent = static_cast<int>(e * 100.0 / t_na22_flag_tree->GetEntries());
            if (cur_percent - pre_percent > 0 && cur_percent % 2 == 0) {
                pre_percent = cur_percent;
                cout << "#" << flush;
            }
            t_na22_flag_tree->GetEntry(e);
            if (t_na22_flag.is_na22 && t_na22_flag.source_id == target_source_id) {
                first_pos.randomize(t_na22_flag.first_ij[0], t_na22_flag.first_ij[1]);
                second_pos.randomize(t_na22_flag.second_ij[0], t_na22_flag.second_ij[1]);
                line_vec.push_back(Line(first_pos.abs_x, first_pos.abs_y, second_pos.abs_x, second_pos.abs_y));
            }
        }
        cout << " DONE ]" << endl;

        delete na22_flag_file;
        na22_flag_file = NULL;
        t_na22_flag_tree = NULL;

    }

    cout << "total line numbers is " << line_vec.size() << endl;

    TRandom3 rand(0);

    TH2F* na22_pos_map = new TH2F("na22_pos_map", "na22_pos_map", 300, 0, 300, 300, 0, 300);
    na22_pos_map->SetDirectory(NULL);
    for (size_t i = 0; i < line_vec.size(); i++) {
        Line lineA = line_vec[i];
        Line lineB;
        do {
            int j = rand.Rndm() * (line_vec.size() - 1);
            lineB = line_vec[j];
        } while(lineA.angle_to(lineB) < 30);
        CrossPoint cross_point = calc_crosspoint(lineA, lineB);
        na22_pos_map->Fill(cross_point.x, cross_point.y);
    }

    cout << " ====== Position of Source " << target_source_id << " ===================" << endl;
    cout << " X = " << na22_pos_map->GetMean(1) << endl;
    cout << " Y = " << na22_pos_map->GetMean(2) << endl;

    CommonCanvas canvas;
    canvas.cd();
    na22_pos_map->Draw("colz");

    rootapp->Run();

    return 0;
}
