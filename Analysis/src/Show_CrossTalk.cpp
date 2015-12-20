#include <iostream>
#include <cstdlib>
#include <iomanip>
#include <cstdio>
#include "PhyEventFile.hpp"
#include "CooConv.hpp"

#define BINS 100

using namespace std;

void print_help() {
	cout << "Help message" << endl;
}

int main(int argc, char** argv) {

	TApplication* rootapp = new TApplication("POLAR", &argc, argv);

	// == process command line parameters =================================
	TString infile_name;
	TString outfile_name;      // -o
	TString number_str;        // -n
	int number = 0;
	TString jx_str;            // -jx
	int jx = 0;
	TString jy_str;            // -jy
	int jy = 0;
	
	TString cur_par_str;
	int argv_idx = 0;
	while (argv_idx < rootapp->Argc() - 1) {
		cur_par_str = rootapp->Argv(++argv_idx);
		if (cur_par_str == "-o") {
			outfile_name = rootapp->Argv(++argv_idx);
		} else if (cur_par_str == "-n") {
			number_str = rootapp->Argv(++argv_idx);
			number = number_str.Atoi();
		} else if (cur_par_str == "-jx") {
			jx_str = rootapp->Argv(++argv_idx);
			jx = jx_str.Atoi();
		} else if (cur_par_str == "-jy") {
			jy_str = rootapp->Argv(++argv_idx);
			jy = jy_str.Atoi();
		} else {
			infile_name = cur_par_str;
		}
	}
	if (infile_name.IsNull() || number == 0 || jx == jy) {
		print_help();
		exit(1);
	}
	// ====================================================================

	PhyEventFile phyEventFile;
	phyEventFile.open(infile_name.Data(), 'r');

	TF1* f_xtalk = new TF1("f_xtalk", "[0]*x", 0, 4096);
	f_xtalk->SetParameter(0, 0.1);
	f_xtalk->SetParName(0, "k");
	TGraph* g_xtalk = new TGraph();
	g_xtalk->SetName("g_xtalk");
	g_xtalk->SetTitle("Cross Talk");
	g_xtalk->SetMarkerColor(9);
	g_xtalk->SetMaximum(2048);
	int g_n = 0;
	while (phyEventFile.trigg_next()) {
		while (phyEventFile.event_next()) {
			if (phyEventFile.event.ct_num != number)
				continue;
			if (!phyEventFile.event.trigger_bit[jx] ||
				phyEventFile.event.trigger_bit[jy])
				continue;
			if ((jy + 1 != jx && jy + 1 <= 63 && phyEventFile.event.trigger_bit[jy + 1]) ||
				(jy - 1 != jx && jy - 1 >= 0  && phyEventFile.event.trigger_bit[jy - 1]) ||
				(jy + 8 != jx && jy + 8 <= 63 && phyEventFile.event.trigger_bit[jy + 8]) ||
				(jy + 9 != jx && jy + 9 <= 63 && phyEventFile.event.trigger_bit[jy + 9]) ||
				(jy + 7 != jx && jy + 7 <= 63 && phyEventFile.event.trigger_bit[jy + 7]) ||
				(jy - 8 != jx && jy - 8 >= 0  && phyEventFile.event.trigger_bit[jy - 8]) ||
				(jy - 9 != jx && jy - 9 >= 0  && phyEventFile.event.trigger_bit[jy - 9]) ||
				(jy - 7 != jx && jy - 7 >= 0  && phyEventFile.event.trigger_bit[jy - 7]))
				continue;
			if (phyEventFile.event.energy_ch[jy] < 1)
				continue;
			if (phyEventFile.event.energy_ch[jy] / phyEventFile.event.energy_ch[jx] > 0.3)
				continue;
			g_xtalk->SetPoint(g_n++, phyEventFile.event.energy_ch[jx], phyEventFile.event.energy_ch[jy]);
		}
	}

	phyEventFile.close();

	TCanvas* canvas = new TCanvas("canvas", "Cross Talk", 1000, 600);
	canvas->cd();
	g_xtalk->Draw("ap*");
	g_xtalk->Fit(f_xtalk, "");
		
	cout << "All Finished." << endl;
	
	// ====================================================================	
	rootapp->Run();
	delete rootapp;
	return 0;
}
