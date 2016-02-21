#include "HkDataFile.hpp"

using namespace std;

HkDataFile::HkDataFile() {
    t_out_file_ = NULL;
    t_hk_packet_tree_ = NULL;
}

HkDataFile::~HkDataFile() {
    
}

bool HkDataFile::open(const char* filename) {
    if (t_out_file_ != NULL)
        return false;
    t_out_file_ = new TFile(filename, "RECREATE");
    if (t_out_file_->IsZombie())
        return false;

    t_hk_packet_tree_ = new TTree("t_hk_packet", "housekeeping packets");
    t_hk_packet_tree_->SetDirectory(t_out_file_);
    
    return true;
}

void HkDataFile::write_after_decoding() {
    t_hk_packet_tree_->Write();
}

void HkDataFile::close() {
    delete t_hk_packet_tree_;
    t_hk_packet_tree_ = NULL;

    t_out_file_->Close();
    delete t_out_file_;
    t_out_file_ = NULL;
}

void HkDataFile::write_two_packet(const HkOdd& odd_pkt, const HkEven even_pkt) {
    if (t_out_file_ == NULL)
        return;
    copy_odd_packet_(odd_pkt);
    copy_even_packet_(even_pkt);
    t_hk_packet_tree_->Fill();
}

void HkDataFile::write_odd_packet_alone(const HkOdd& odd_pkt) {
    if (t_out_file_ == NULL)
        return;
    clear_branch_data_();
    copy_odd_packet_(odd_pkt);
    t_hk_packet_tree_->Fill();
}

void HkDataFile::write_even_packet_alone(const HkEven& even_pkt) {
    if (t_out_file_ == NULL)
        return;
    clear_branch_data_();
    copy_even_packet_(even_pkt);
    t_hk_packet_tree_->Fill();
}

void HkDataFile::write_meta(const char* key, const char* value) {
    if (t_out_file_ == NULL)
        return;
    TNamed* cur_meta = new TNamed(key, value);
    cur_meta->Write();
    delete cur_meta;
    cur_meta = NULL;
}

void HkDataFile::copy_odd_packet_(const HkOdd& odd_pkt) {
    
}

void HkDataFile::copy_even_packet_(const HkEven& even_pkt) {
    
}

void HkDataFile::clear_branch_data_() {
    
}
