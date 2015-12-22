#include "CrossTalkCalc.hpp"

CrossTalkCalc::CrossTalkCalc() {
	
}

CrossTalkCalc::~CrossTalkCalc() {

}

bool CrossTalkCalc::open(const char* filename, char m) {

	return true;
}

void CrossTalkCalc::close() {

}

void CrossTalkCalc::do_fill(PhyEventFile& phy_event_file) {

}

void CrossTalkCalc::do_fit() {

}

void CrossTalkCalc::do_move_trigg(PhyEventFile& phy_event_file_w,
								  const PhyEventFile& phy_event_file_r) const {

}

void CrossTalkCalc::do_correct(PhyEventFile& phy_event_file_w,
							   const PhyEventFile& phy_event_file_r) const {

}

void CrossTalkCalc::show(int ct_num) {

}

void CrossTalkCalc::show_xtalk() {

}
