#include "HkDataFile.hpp"

using namespace std;

HkDataFile::HkDataFile() {
    t_out_file_ = NULL;
    t_hk_obox_tree_ = NULL;
    t_hk_ibox_tree_ = NULL;
}

HkDataFile::~HkDataFile() {
    
}

bool HkDataFile::open(const char* filename) {
    if (t_out_file_ != NULL)
        return false;
    t_out_file_ = new TFile(filename, "RECREATE");
    if (t_out_file_->IsZombie())
        return false;

    t_hk_obox_tree_ = new TTree("t_hk_obox", "obox housekeeping packets");
    t_hk_obox_tree_->SetDirectory(t_out_file_);
    t_hk_obox_tree_->Branch("odd_index",            &t_hk_obox.odd_index,         "odd_index/I"     );
    t_hk_obox_tree_->Branch("even_index",           &t_hk_obox.even_index,        "even_index/I"    );
    t_hk_obox_tree_->Branch("odd_is_bad",           &t_hk_obox.odd_is_bad,        "odd_is_bad/I"    );
    t_hk_obox_tree_->Branch("even_is_bad",          &t_hk_obox.even_is_bad,       "even_is_bad/I"   );

    t_hk_ibox_tree_ = new TTree("t_hk_ibox", "ibox housekeeping info");
    t_hk_ibox_tree_->SetDirectory(t_out_file_);
    t_hk_ibox_tree_->Branch("frm_index",            &t_hk_ibox.frm_index,         "frm_index/I"     );
    t_hk_ibox_tree_->Branch("pkt_tag",              &t_hk_ibox.pkt_tag,           "pkt_tag/I"       );
    t_hk_ibox_tree_->Branch("is_bad",               &t_hk_ibox.is_bad,            "is_bad/I"        );
    
    return true;
}

void HkDataFile::write_after_decoding() {
    t_hk_obox_tree_->Write();
    t_hk_ibox_tree_->Write();
}

void HkDataFile::close() {
    delete t_hk_obox_tree_;
    t_hk_obox_tree_ = NULL;

    delete t_hk_ibox_tree_;
    t_hk_ibox_tree_ = NULL;

    t_out_file_->Close();
    delete t_out_file_;
    t_out_file_ = NULL;
}

void HkDataFile::write_ibox_info(const HkFrame& frame) {
    copy_ibox_info_(frame);
    t_hk_ibox_tree_->Fill();
}

void HkDataFile::write_two_packet(const HkOdd& odd_pkt, const HkEven even_pkt) {
    if (t_out_file_ == NULL)
        return;
    copy_odd_packet_(odd_pkt);
    copy_even_packet_(even_pkt);
    t_hk_obox_tree_->Fill();
}

void HkDataFile::write_odd_packet_alone(const HkOdd& odd_pkt) {
    if (t_out_file_ == NULL)
        return;
    clear_obox_branch_data_();
    t_hk_obox.even_index = -1;
    t_hk_obox.even_is_bad = 3;
    copy_odd_packet_(odd_pkt);
    t_hk_obox_tree_->Fill();
}

void HkDataFile::write_even_packet_alone(const HkEven& even_pkt) {
    if (t_out_file_ == NULL)
        return;
    clear_obox_branch_data_();
    t_hk_obox.odd_index = -1;
    t_hk_obox.odd_is_bad = 3;
    copy_even_packet_(even_pkt);
    t_hk_obox_tree_->Fill();
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
    t_hk_obox.odd_index       = static_cast<Int_t>(odd_pkt.frm_index);
    t_hk_obox.odd_is_bad      = static_cast<Int_t>(odd_pkt.is_bad);
}

void HkDataFile::copy_even_packet_(const HkEven& even_pkt) {
    t_hk_obox.even_index      = static_cast<Int_t>(even_pkt.frm_index);
    t_hk_obox.even_is_bad     = static_cast<Int_t>(even_pkt.is_bad);
    
}

void HkDataFile::copy_ibox_info_(const HkFrame& frame) {
    t_hk_ibox.frm_index       = static_cast<Int_t>(frame.get_index());
    t_hk_ibox.pkt_tag         = static_cast<Int_t>(frame.get_pkt_tag());
    t_hk_ibox.is_bad          = static_cast<Int_t>(frame.is_bad);
}

void HkDataFile::clear_obox_branch_data_() {
    t_hk_obox.odd_index = 0;
    t_hk_obox.even_index = 0;
    t_hk_obox.odd_is_bad = 0;
    t_hk_obox.even_is_bad = 0;
}
