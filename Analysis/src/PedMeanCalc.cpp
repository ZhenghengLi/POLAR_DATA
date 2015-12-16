#include "PedMeanCalc.hpp"

PedMeanCalc::PedMeanCalc() {
	eventIter_ = NULL;
	done_flag_ = false;
	for (int i = 0; i < 25; i++) {
		canvas_[i] = NULL;
		for (int j = 0; j < 64; j++) {
			f_gaus_[i][j] = NULL;
			h_ped_[i][j] = NULL;
		}
	}
	canvas_res_ = NULL;
	g_ped_ = NULL;
}

PedMeanCalc::~PedMeanCalc() {
	destroy_();
}

void PedMeanCalc::create_() {
	if (eventIter_ == NULL)
		return;
	for (int i = 0; i < 25; i++) {
		for (int j = 0; j < 64; j++) {
			sprintf(name_, "f_gaus_%d_%d", i + 1, j + 1);
			f_gaus_[i][j] = new TF1(name_, "gaus(0)", 0, PED_MAX);
			f_gaus_[i][j]->SetParameters(1, 400, 25);
			f_gaus_[i][j]->SetParName(0, "Amplitude");
			f_gaus_[i][j]->SetParName(1, "Mean");
			f_gaus_[i][j]->SetParName(2, "Sigma");
			sprintf(name_, "h_ped_%d_%d", i + 1, j + 1);
			sprintf(title_, "Pedestal of CH %d_%d", i + 1, j + 1);
			h_ped_[i][j] = new TH1F(name_, title_, PED_BINS, 0, PED_MAX);
			h_ped_[i][j]->SetDirectory(NULL);
		}
	}
}

void PedMeanCalc::destroy_() {
	if (eventIter_ != NULL)
		eventIter_ = NULL;
	if (canvas_res_ != NULL) {
		delete canvas_res_;
		canvas_res_ = NULL;
	}
	if (g_ped_ != NULL) {
		delete g_ped_;
		g_ped_ = NULL;
	}
	for (int i = 0; i < 25; i++) {
		if (canvas_[i] != NULL) {
			delete canvas_[i];
			canvas_[i] = NULL;
		}
		for (int j = 0; j < 64; j++) {
			if (f_gaus_[i][j] != NULL) {
				delete f_gaus_[i][j];
				f_gaus_[i][j] = NULL;
			}
			if (h_ped_[i][j] != NULL) {
				delete h_ped_[i][j];
				h_ped_[i][j] = NULL;
			}
		}
	}
}

void PedMeanCalc::init(EventIterator* iter) {
	destroy_();
	eventIter_ = iter;
	done_flag_ = false;	
	create_();
}

void PedMeanCalc::do_calc() {
	if (eventIter_ == NULL)
		return;
	eventIter_->ped_trigg_restart();
	while (eventIter_->ped_trigg_next()) {
		while (eventIter_->ped_event_next()) {
			int idx = eventIter_->ped_event.ct_num - 1;
			for (int j = 0; j < 64; j++) {
				Int_t ch = eventIter_->ped_event.energy_ch[j];
				if (ch > PED_MAX)
					continue;
				h_ped_[idx][j]->Fill(static_cast<Float_t>(ch));
			}
		}
	}
	
	for (int i = 0; i < 25; i++) {
		for (int j = 0; j < 64; j++) {
			mean_0[i][j] = h_ped_[i][j]->GetMean();
			sigma_0[i][j] = h_ped_[i][j]->GetRMS();
			if (h_ped_[i][j]->GetEntries() < 20) {
				mean[i][j] = mean_0[i][j];
				sigma[i][j] = sigma_0[i][j];
				continue;
			}
			Float_t min_ch = mean_0[i][j] - 200 > 0 ? mean_0[i][j] - 200 : 0;
			Float_t max_ch = mean_0[i][j] + 200 < PED_MAX ? mean_0[i][j] + 200 : PED_MAX;
			f_gaus_[i][j]->SetParameter(1, mean_0[i][j]);
			f_gaus_[i][j]->SetParameter(2, sigma_0[i][j]);
			f_gaus_[i][j]->SetRange(min_ch, max_ch);
			h_ped_[i][j]->Fit(f_gaus_[i][j], "RQN");
			mean[i][j] = f_gaus_[i][j]->GetParameter(1);
			sigma[i][j] = abs(f_gaus_[i][j]->GetParameter(2));
			if (mean[i][j] < 0 || mean[i][j] > PED_MAX) {
				mean[i][j] = mean_0[i][j];
				sigma[i][j] = sigma_0[i][j];
			} else if (sigma[i][j] / sigma_0[i][j] > 3.0) {
				mean[i][j] = mean_0[i][j];
				sigma[i][j] = sigma_0[i][j];
			}
		}
	}
	eventIter_->ped_trigg_restart();
	done_flag_ = true;
}

void PedMeanCalc::show(int ct_num) {
	if (!done_flag_)
		return;
	int idx = ct_num - 1;
	if (canvas_[idx] != NULL) {
		delete canvas_[idx];
		canvas_[idx] = NULL;
	}
	gStyle->SetOptStat(11);
	gStyle->SetOptFit(111);
	sprintf(name_, "canvas_%d", idx);
	sprintf(title_, "Pedestal of CT %d", ct_num);
	canvas_[idx] = new TCanvas(name_, title_, 600, 400);
	canvas_[idx]->Divide(8, 8);
	for (int j = 0; j < 64; j++) {
		canvas_[idx]->cd(j + 1);
		if (h_ped_[idx][j]->GetEntries() < 20)
			continue;
		h_ped_[idx][j]->Fit(f_gaus_[idx][j], "RQ");
	}
}

void PedMeanCalc::do_move_trigg(PhyEventFile& phy_event_file, const EventIterator& event_iterator) const {
	if (!done_flag_)
		return;
	phy_event_file.trigg.trigg_index = event_iterator.trigg.trigg_index;
	phy_event_file.trigg.mode = event_iterator.trigg.mode;
	for (int i = 0; i < 25; i++)
		phy_event_file.trigg.trig_accepted[i] = event_iterator.trigg.trig_accepted[i];
	phy_event_file.trigg.start_entry = event_iterator.trigg.start_entry;
	phy_event_file.trigg.pkt_count = event_iterator.trigg.pkt_count;
	phy_event_file.trigg.lost_count = event_iterator.trigg.lost_count;
	phy_event_file.trigg.level_flag = 1;
}

void PedMeanCalc::do_subtruct(PhyEventFile& phy_event_file, const EventIterator& event_iterator) const {
	if (!done_flag_)
		return;
	phy_event_file.event.trigg_index = event_iterator.event.trigg_index;
	phy_event_file.event.ct_num = event_iterator.event.ct_num;
	for (int i = 0; i < 64; i++)
		phy_event_file.event.trigger_bit[i] = event_iterator.event.trigger_bit[i];
	if (event_iterator.event.mode == 3) {
		for (int i = 0; i < 64; i++)
			phy_event_file.event.energy_ch[i] = static_cast<Float_t>(event_iterator.event.energy_ch[i]);
	} else {
		int idx = event_iterator.event.ct_num - 1;
		Float_t tmp_energy_ch[64];
		for (int i = 0; i < 64; i++)
			tmp_energy_ch[i] = static_cast<Float_t>(event_iterator.event.energy_ch[i]) - mean[idx][i];
		Float_t common_noise = 0;
		Double_t common_sum = 0;
		int common_n = 0;
		for (int i = 0; i < 64; i++) {
			if (event_iterator.event.trigger_bit[i])
				continue;
			common_sum += tmp_energy_ch[i];
			common_n ++;
		}
		common_noise = (common_n > 0 ? common_sum / common_n : 0);
		for (int i = 0; i < 64; i++) {
			tmp_energy_ch[i] = tmp_energy_ch[i] - common_noise;
			phy_event_file.event.energy_ch[i] = (tmp_energy_ch[i] > 0 ? tmp_energy_ch[i] : 0);
		}
	}
}

void PedMeanCalc::print(bool sigma_flag) {
	if (!done_flag_)
		return;
	cout << endl;
	cout << setw(3) << " " << " | ";
	for (int i = 0; i < 25; i++)
		cout << setw(7) << i + 1;
	cout << endl;
	cout << endl;
	cout << fixed << setprecision(2);
	for (int j = 0; j < 64; j++) {
        cout << setw(3) << j + 1 << " | ";
        for (int i = 0; i < 25; i++) {
            cout << setw(7) << mean[i][j];
        }
        cout << endl;
		if (sigma_flag) {
			cout << setw(3) << " " << " | ";
			for (int i = 0; i < 25; i++) {
				cout << setw(7) << sigma[i][j];
			}
			cout << endl;
		}
        cout << endl;
	}
}

void PedMeanCalc::show_all() {
	if (!done_flag_)
		return;
	for (int i = 0; i < 25; i++)
		show(i + 1);
}

void PedMeanCalc::show_mean() {
	if (!done_flag_)
		return;
	if (canvas_res_ != NULL) {
		delete canvas_res_;
		canvas_res_ = NULL;
	}
	if (g_ped_ != NULL) {
		delete g_ped_;
		g_ped_ = NULL;
	}
	canvas_res_ = new TCanvas("canvas_res", "Pedestal Map of 1600 Channels", 800, 800);
	canvas_res_->SetFixedAspectRatio();
	g_ped_ = new TGraph2D(1600);
	g_ped_->SetName("g_ped");
	g_ped_->SetTitle("Pedestal Map of 1600 Channels");
	g_ped_->SetDirectory(NULL);
	int n, x1, x2, y1, y2, x, y;
	for (int i = 0; i < 25; i++) {
		for (int j = 0; j < 64; j++) {
			n = i * 64 + j;
			x1 = i / 5;
			y1 = 4 - i % 5;
			x2 = j % 8;
			y2 = j / 8;
			x = x1 * 8 + x2;
			y = y1 * 8 + y2;
			g_ped_->SetPoint(n, x, y, mean[i][j]);
		}
	}
	canvas_res_->cd();
	g_ped_->Draw("COLZ");
}

