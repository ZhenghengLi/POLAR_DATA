#include <iostream>
#include <cstdlib>
#include <iomanip>
#include <cstdio>
#include "PhyEventFile.hpp"
#include "CooConv.hpp"

using namespace std;

void print_help() {
	cout << "Help message" << endl;
}

int main(int argc, char** argv) {

	TApplication* rootapp = new TApplication("POLAR", &argc, argv);

	// == process command line parameters =================================
	TString infile_name;
	TString outfile_name;      // -o
	TString number_str;            // -n
	int number = 0;
	TString cur_par_str;
	int argv_idx = 0;
	while (argv_idx < rootapp->Argc() - 1) {
		cur_par_str = rootapp->Argv(++argv_idx);
		if (cur_par_str == "-o") {
			outfile_name = rootapp->Argv(++argv_idx);
		} if (cur_par_str == "-n") {
			number_str = rootapp->Argv(++argv_idx);
			number = number_str.Atoi();
		} else {
			infile_name = cur_par_str;
		}
	}
	if (infile_name.IsNull() || number == 0) {
		print_help();
		exit(1);
	}
	// ====================================================================
	
	PhyEventFile phyEventFile;
	phyEventFile.open(infile_name.Data(), 'r');

	TH2F* h_ch_map = new TH2F("h_ch_map", "Channel Map", 40, 0, 40, 40, 0, 40);
	h_ch_map->SetDirectory(NULL);
	h_ch_map->GetXaxis()->SetNdivisions(40);
	h_ch_map->GetYaxis()->SetNdivisions(40);
	char str_buffer[10];
	for (int i = 0; i < 40; i++) {
		if (i % 8 == 0) {
			sprintf(str_buffer, "%d", i);
			h_ch_map->GetXaxis()->SetBinLabel(i + 1, str_buffer);
			h_ch_map->GetYaxis()->SetBinLabel(i + 1, str_buffer);
		}
	}

	int order_num = 0;
	int idx = 0;
	while (phyEventFile.trigg_next()) {
		if (phyEventFile.trigg.pkt_count > 1 && phyEventFile.trigg.lost_count == 0)
			order_num++;
		if (order_num < number)
			continue;
		while (phyEventFile.event_next()) {
			cout << phyEventFile.event.trigg_index << endl;
			idx = phyEventFile.event.ct_num - 1;
			for (int j = 0; j < 64; j++) {
				h_ch_map->SetBinContent(ijtox(idx, j) + 1, ijtoy(idx, j) + 1, phyEventFile.event.energy_ch[j]);
			}
		}
		break;
	}

	phyEventFile.close();

	TCanvas* canvas_ch = new TCanvas("canvas_ch", "Channel Map", 800, 800);
	canvas_ch->SetGrid();
	canvas_ch->cd();
	gStyle->SetOptStat(0);
	h_ch_map->Draw("colz");

	cout << "All Finished." << endl;
	
	// ====================================================================	
	rootapp->Run();
	delete rootapp;
	return 0;
}
