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
    t_hk_packet_tree_->Branch("odd_index",            &t_hk_packet.odd_index,         "odd_index/I"     );
    t_hk_packet_tree_->Branch("even_index",           &t_hk_packet.even_index,        "even_index/I"    );
    t_hk_packet_tree_->Branch("odd_is_bad",           &t_hk_packet.odd_is_bad,        "odd_is_bad/I"    );
    t_hk_packet_tree_->Branch("even_is_bad",          &t_hk_packet.even_is_bad,       "even_is_bad/I"   );
    
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
    clear_all_branch_data_();
    t_hk_packet.even_index = -1;
    t_hk_packet.even_is_bad = 3;
    copy_odd_packet_(odd_pkt);
    t_hk_packet_tree_->Fill();
}

void HkDataFile::write_even_packet_alone(const HkEven& even_pkt) {
    if (t_out_file_ == NULL)
        return;
    clear_all_branch_data_();
    t_hk_packet.odd_index = -1;
    t_hk_packet.odd_is_bad = 3;
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
    t_hk_packet.odd_index       = static_cast<Int_t>(odd_pkt.frm_index);
    t_hk_packet.odd_is_bad      = static_cast<Int_t>(odd_pkt.is_bad);
}

void HkDataFile::copy_even_packet_(const HkEven& even_pkt) {
    t_hk_packet.even_index      = static_cast<Int_t>(even_pkt.frm_index);
    t_hk_packet.even_is_bad     = static_cast<Int_t>(even_pkt.is_bad);
    
}

void HkDataFile::clear_all_branch_data_() {
    t_hk_packet.odd_index = 0;
    t_hk_packet.even_index = 0;
    t_hk_packet.odd_is_bad = 0;
    t_hk_packet.even_is_bad = 0;
}
