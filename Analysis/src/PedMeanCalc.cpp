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
			h_ped_[i][j] = new TH1D(name_, title_, PED_BINS, 0, PED_MAX);
			h_ped_[i][j]->SetDirectory(NULL);
		}
	}
}

void PedMeanCalc::destroy_() {
	if (eventIter_ != NULL)
		eventIter_ = NULL;
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
				h_ped_[idx][j]->Fill(ch);
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
			int min_ch = mean_0[i][j] - 200 > 0 ? mean_0[i][j] - 200 : 0;
			int max_ch = mean_0[i][j] + 200 < PED_MAX ? mean_0[i][j] + 200 : PED_MAX;
			f_gaus_[i][j]->SetParameter(1, mean_0[i][j]);
			f_gaus_[i][j]->SetParameter(2, sigma_0[i][j]);
			f_gaus_[i][j]->SetRange(min_ch, max_ch);
			h_ped_[i][j]->Fit(f_gaus_[i][j], "RQN");
			mean[i][j] = f_gaus_[i][j]->GetParameter(1);
			sigma[i][j] = abs(f_gaus_[i][j]->GetParameter(2));
			if (sigma[i][j] / sigma_0[i][j] > 3.0) {
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
		h_ped_[idx][j]->Fit(f_gaus_[idx][j], "RQ");
	}
}
