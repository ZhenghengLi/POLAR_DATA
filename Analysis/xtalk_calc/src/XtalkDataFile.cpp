#include "XtalkDataFile.hpp"

using namespace std;

XtalkDataFile::XtalkDataFile() {

}

XtalkDataFile::~XtalkDataFile() {
    close();
}

bool XtalkDataFile::open(const char* filename, char m) {
    if (t_xtalk_file_ != NULL)
        return false;

    if (m == 'w' || m == 'W')
        mode_ = 'w';
    else if (m == 'r' || m == 'R')
        mode_ = 'r';
    else
        return false;

    if (mode_ == 'r')
        t_xtalk_file_ = new TFile(filename, "READ");
    else
        t_xtalk_file_ = new TFile(filename, "UPDATE");
    if (t_xtalk_file_->IsZombie())
        return false;
    
    for (int i = 0; i < 25; i++) {
        sprintf(name_, "t_xtalk_data_ct_%d", i + 1);
        sprintf(title_, "crosstalk data of ct %d", i + 1);
        t_xtalk_data_tree_[i] = static_cast<TTree*>(t_xtalk_file_->Get(name_));
        is_first_created_[i] = false;
        if (t_xtalk_data_tree_[i] == NULL) {
            if (mode_ == 'r') {
                return false;
            } else {
                t_xtalk_data_tree_[i] = new TTree(name_, title_);
                t_xtalk_data_tree_[i]->SetDirectory(t_xtalk_file_);
                is_first_created_[i] = true;
            }
        }
        if (mode_ == 'r') {
            m_fromfile_ = static_cast<TNamed*>(t_xtalk_file_->Get("m_fromfile"));
            m_gps_span_ = static_cast<TNamed*>(t_xtalk_file_->Get("m_gps_span"));
            if (m_fromfile_ == NULL || m_gps_span_ == NULL)
                return false;
        }
        if (is_first_created_[i]) {

        } else {

        }
        
    }

    return true;
}

void XtalkDataFile::close() {

}
