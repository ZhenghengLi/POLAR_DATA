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
			sprintf(name_, "f_gaus_%d_%d", i, j);
			f_gaus_[i][j] = new TF1(name_, "gaus(0)", 1, PED_MAX);
			f_gaus_[i][j]->SetParameters(1, 400, 30);
			sprintf(name_, "h_ped_%d_%d", i, j);
			sprintf(title_, "Pedestal of CH %d_%d", i + 1, j + 1);
			h_ped_[i][j] = new TH1F(name_, title_, PED_BINS, 1, PED_MAX);
			h_ped_[i][j]->SetDirectory(NULL);
			tot_ch_[i][j] = 0;
			num_ch_[i][j] = 0;
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
				tot_ch_[idx][j] += ch;
				num_ch_[idx][j]++;
				h_ped_[idx][j]->Fill(ch);
			}
		}
	}
	
	for (int i = 0; i < 25; i++) {
		for (int j = 0; j < 64; j++) {
			mean_0_[i][j] = tot_ch_[i][j] / num_ch_[i][j];
			int min_ch = mean_0_[i][j] - 150 > 1 ? mean_0_[i][j] - 150 : 1;
			int max_ch = mean_0_[i][j] + 150 < PED_MAX ? mean_0_[i][j] + 150 : PED_MAX;
			f_gaus_[i][j]->SetParameter(1, mean_0_[i][j]);
			f_gaus_[i][j]->SetRange(min_ch, max_ch);
			h_ped_[i][j]->Fit(f_gaus_[i][j], "RQ0");
			h_ped_[i][j]->Fit(f_gaus_[i][j], "RQ0");
			mean[i][j] = f_gaus_[i][j]->GetParameter(1);
			sigma[i][j] = abs(f_gaus_[i][j]->GetParameter(2));
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
	sprintf(name_, "canvas_%d", idx);
	sprintf(title_, "Pedestal of CT %d", ct_num);
	canvas_[idx] = new TCanvas(name_, title_, 600, 400);
	canvas_[idx]->Divide(8, 8);
	for (int j = 0; j < 64; j++) {
		canvas_[idx]->cd(j + 1);
		h_ped_[idx][j]->Draw();
		h_ped_[idx][j]->Fit(f_gaus_[idx][j], "RQ");
	}
}
