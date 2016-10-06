#include "RecEventIterator.hpp"

using namespace std;

RecEventIterator::RecEventIterator() {
    re_gps_      = "^ *(\\d+) *: *(\\d+) *$";
    re_gps_span_ = "^ *(\\d+:\\d+) => (\\d+:\\d+) *$";

    t_file_in_        = NULL;
    t_rec_event_tree_ = NULL;
    m_gps_span_       = NULL;

    is_first_two_ready = false;
}

RecEventIterator::~RecEventIterator() {
    close();
}

double RecEventIterator::value_of_gps_str_(const string gps_str) {
    cmatch cm;
    if (regex_match(gps_str.c_str(), cm, re_gps_)) {
        return TString(cm[1]).Atof() * 604800 + TString(cm[2]).Atof();
    } else {
        return -1;
    }
}

Long64_t RecEventIterator::find_entry_(TTree* t_tree, RecEvent_T& t_branch, double gps_value) {
    Long64_t head_entry       = -1;
    bool     head_entry_found = false;
    while (head_entry < t_tree->GetEntries()) {
        head_entry += 1;
        t_tree->GetEntry(head_entry);
        if (t_branch.abs_gps_valid) {
            head_entry_found = true;
            break;
        }
    }
    if (!head_entry_found) {
        return -1;
    }
    if (gps_value < t_branch.abs_gps_week * 604800 + t_branch.abs_gps_second) {
        return -1;
    }
    Long64_t tail_entry       = t_tree->GetEntries();
    bool     tail_entry_found = false;
    while (tail_entry >= 0) {
        tail_entry -= 1;
        t_tree->GetEntry(tail_entry);
        if (t_branch.abs_gps_valid) {
            tail_entry_found = true;
            break;
        }
    }
    if (!tail_entry_found) {
        return -1;
    }
    if (gps_value > t_branch.abs_gps_week * 604800 + t_branch.abs_gps_second) {
        return -1;
    }
    Long64_t center_entry       = 0;
    Long64_t tmp_center_entry   = 0;
    bool     found_valid_center = false;
    while (tail_entry - head_entry > 1) {
        center_entry = (head_entry + tail_entry) / 2;
        found_valid_center = false;
        t_tree->GetEntry(center_entry);
        if (t_branch.abs_gps_valid) {
            found_valid_center = true;
        }
        tmp_center_entry = center_entry;
        while (!found_valid_center && tail_entry - tmp_center_entry > 1) {
            tmp_center_entry += 1;
            t_tree->GetEntry(tmp_center_entry);
            if (t_branch.abs_gps_valid) {
                found_valid_center = true;
            }
        }
        if (!found_valid_center) {
            tmp_center_entry = center_entry;
        }
        while (!found_valid_center && tmp_center_entry - head_entry > 1) {
            tmp_center_entry -= 1;
            t_tree->GetEntry(tmp_center_entry);
            if (t_branch.abs_gps_valid) {
                found_valid_center = true;
            }
        }
        if (!found_valid_center) {
            break;
        }
        if (gps_value == t_branch.abs_gps_week * 604800 + t_branch.abs_gps_second) {
            return tmp_center_entry;
        } else if (gps_value > t_branch.abs_gps_week * 604800 + t_branch.abs_gps_second) {
            head_entry = tmp_center_entry;
        } else {
            tail_entry = tmp_center_entry;
        }
    }
    return tail_entry;
}

bool RecEventIterator::open(const char* filename) {
    return open(filename, "begin", "end");
}

bool RecEventIterator::open(const char* filename, const char* gps_begin, const char* gps_end) {
    if (t_file_in_ != NULL) {
        return false;
    }

    // record gps time
    gps_str_begin_.assign(gps_begin);
    if (gps_str_begin_ == "begin") {
        gps_value_begin_ = -2 * GPS_SPAN_MIN;
    } else {
        gps_value_begin_ = value_of_gps_str_(gps_str_begin_);
        if (gps_value_begin_ < 0) {
            cerr << "GPS string: " << gps_str_begin_ << " is invalid, it should be week:second." << endl;
            return false;
        }
    }
    gps_str_end_.assign(gps_end);
    if (gps_str_end_ == "end") {
        gps_value_end_ = numeric_limits<double>::max() - 3 * GPS_SPAN_MIN;
    } else {
        gps_value_end_ = value_of_gps_str_(gps_str_end_);
        if (gps_value_end_ < 0) {
            cerr << "GPS string: " << gps_str_end_ << " is invalid, it should be week:second." << endl;
            return false;
        }
    }
    if (gps_value_end_ - gps_value_begin_ < GPS_SPAN_MIN) {
        cerr << "the GPS span of input is too small: " << gps_value_end_ - gps_value_begin_ << endl;
        return false;
    }

    // open and check
    name_str_file_in_.assign(filename);
    t_file_in_ = new TFile(filename, "READ");
    if (t_file_in_->IsZombie())
        return false;
    t_rec_event_tree_ = static_cast<TTree*>(t_file_in_->Get("t_rec_event"));
    if (t_rec_event_tree_ == NULL)
        return false;
    bind_rec_event_tree(t_rec_event_tree_, t_rec_event);
    m_gps_span_ = static_cast<TNamed*>(t_file_in_->Get("m_gps_span"));
    if (m_gps_span_ == NULL)
        return false;
    cmatch cm;
    if (regex_match(m_gps_span_->GetTitle(), cm, re_gps_span_)) {
        gps_str_first_.assign(cm[1]);
        gps_str_last_.assign(cm[2]);
        gps_value_first_ = value_of_gps_str_(gps_str_first_);
        gps_value_last_  = value_of_gps_str_(gps_str_last_);
        if (gps_value_first_ < 0 || gps_value_last_ < 0) {
            return false;
        }
    } else {
        cerr << "GPS span regex match failed: " << m_gps_span_->GetTitle() << endl;
        return false;
    }
    
    // check gps span matching
    if (gps_str_begin_ != "begin") {
        if (gps_value_begin_ < gps_value_first_) {
            cerr << "GPS string of beginning is out of range: "
                 << gps_value_first_ - gps_value_begin_ << " seconds" << endl;
            return false;
        }
        if (gps_str_end_ == "end" && gps_value_last_ - gps_value_begin_ < GPS_DIFF_MIN) {
            cerr << "GPS string of beginning is too large: "
                 << gps_value_last_ - gps_value_begin_ << " seconds" << endl;
            return false;
        }
    }
    if (gps_str_end_ != "end") {
        if (gps_value_end_ > gps_value_last_) {
            cerr << "GPS string of ending is out fo range: "
                 << gps_value_end_ - gps_value_last_ << " seconds" << endl;
            return false;
        }
        if (gps_str_begin_ == "begin" && gps_value_end_ - gps_value_begin_ < GPS_DIFF_MIN) {
            cerr << "GPS string of ending is too small: "
                 << gps_value_end_ - gps_value_first_ << " seconds" << endl;
            return false;
        }
    }

    // find the first and last entry
    if (gps_str_begin_ == "begin") {
        first_entry_ = 0;
        if (gps_str_end_ == "end") {
            last_entry_ = t_rec_event_tree_->GetEntries();
        } else {
            last_entry_ = find_entry_(t_rec_event_tree_, t_rec_event, gps_value_end_);
            if (last_entry_ < 0) {
                cerr << "Cannot find the last valid entry of t_rec_event." << endl;
                return false;
            }
        }
    } else {
        first_entry_ = find_entry_(t_rec_event_tree_, t_rec_event, gps_value_begin_);
        if (first_entry_ < 0) {
            cerr << "Cannot find the first valid entry of t_rec_event." << endl;
            return false;
        }
        if (gps_str_end_ == "end") {
            last_entry_ = t_rec_event_tree_->GetEntries();
        } else {
            last_entry_ = find_entry_(t_rec_event_tree_, t_rec_event, gps_value_end_);
            if (last_entry_ < 0) {
                cerr << "Cannot find the last valid entry of t_rec_event." << endl;
                return false;
            }
        }
    }

	// find begin and end entry
	bool found_valid = false;
	for (Long64_t i = first_entry_; i < last_entry_; i++) {
		t_rec_event_tree_->GetEntry(i);
		if (t_rec_event.abs_gps_valid) {
			found_valid = true;
			begin_rec_event = t_rec_event;
		}
	}
	if (!found_valid) {
		cout << "Cannot find begin_rec_event." << endl;
		return false;
	}
	found_valid = false;
	for (Long64_t i = last_entry_ - 1; i >= first_entry_; i--) {
		t_rec_event_tree_->GetEntry(i);
		if (t_rec_event.abs_gps_valid) {
			found_valid = true;
			end_rec_event = t_rec_event;
		}
	}
	if (!found_valid) {
		cout << "Cannot find end_rec_event." << endl;
		return false;
	}

    return true;
}

void RecEventIterator::close() {
    if (t_file_in_ == NULL)
        return;
    t_file_in_->Close();
    delete t_file_in_;
    t_file_in_ = NULL;
    t_rec_event_tree_ = NULL;
    m_gps_span_ = NULL;
}

void RecEventIterator::print_file_info() {
    if (t_file_in_ == NULL)
        return;
    char str_buffer[200];
    string first_gps;
    string last_gps;
    string result_str;
    if (gps_str_begin_ == "begin") {
        first_gps = gps_str_first_;
    } else {
        t_rec_event_tree_->GetEntry(first_entry_);
        sprintf(str_buffer, "%d:%d",
                static_cast<int>(t_rec_event.abs_gps_week),
                static_cast<int>(t_rec_event.abs_gps_second)
        );
        first_gps.assign(str_buffer);
    }
    if (gps_str_end_ == "end") {
        last_gps = gps_str_last_;
    } else {
        t_rec_event_tree_->GetEntry(last_entry_);
        sprintf(str_buffer, "%d:%d",
                static_cast<int>(t_rec_event.abs_gps_week),
                static_cast<int>(t_rec_event.abs_gps_second)
        );
        last_gps.assign(str_buffer);
    }
    sprintf(str_buffer,
            "%s [%ld] => %s [%ld] / [%ld]",
            first_gps.c_str(),
            static_cast<long int>(first_entry_),
            last_gps.c_str(),
            static_cast<long int>(last_entry_),
            static_cast<long int>(t_rec_event_tree_->GetEntries())
    );
    result_str.assign(str_buffer);

    cout << name_str_file_in_ << endl;
    cout << " - GPS time span: { " << result_str << " } " << endl;
}

void RecEventIterator::set_start() {
    cur_entry_ = first_entry_ - 1;
    reach_end_ = false;
}
Long64_t RecEventIterator::get_total_entries() {
    return last_entry_ - first_entry_;
}

Long64_t RecEventIterator::get_cur_entry() {
    return cur_entry_ - first_entry_;
}

bool RecEventIterator::next_event() {
    if (reach_end_)
        return false;
    cur_entry_++;
    if (cur_entry_ < last_entry_) {
        t_rec_event_tree_->GetEntry(cur_entry_);
        return true;
    } else {
        reach_end_ = true;
        return false;
    }
}

double RecEventIterator::angle_of_3_points_(double x0, double y0, double x1, double y1, double x2, double y2) {
    double vec1[2] = {x1 - x0, y1 - y0};
    double vec2[2] = {x2 - x0, y2 - y0};
    double norm_vec1 = TMath::Sqrt(vec1[0] * vec1[0] + vec1[1] * vec1[1]);
    double norm_vec2 = TMath::Sqrt(vec2[0] * vec2[0] + vec2[1] * vec2[1]);
    double product12 = vec1[0] * vec2[0] + vec1[1] * vec2[1];
    return TMath::ACos(product12 / (norm_vec1 * norm_vec2));
}

double RecEventIterator::distance_of_3_points_(double x0, double y0, double x1, double y1, double x2, double y2) {
    double vec01[2] = {x1 - x0, y1 - y0};
    double vec12[2] = {x2 - x1, y2 - y1};
    double norm_vec01 = TMath::Sqrt(vec01[0] * vec01[0] + vec01[1] * vec01[1]);
    double norm_vec12 = TMath::Sqrt(vec12[0] * vec12[0] + vec12[1] * vec12[1]);
    double product_01_12 = vec01[0] * vec12[0] + vec01[1] * vec12[1];
    double angle_01_12 = TMath::ACos(product_01_12 / (norm_vec01 * norm_vec12));
    return (norm_vec01 * TMath::Sin(angle_01_12));
}

bool RecEventIterator::find_first_two_bars() {
    is_first_two_ready = false;
    priority_queue<Bar> bar_queue;
    for (int i = 0; i < 25; i++) {
        if (t_rec_event.trig_accepted[i]) {
            for (int j = 0; j < 64; j++) {
                if (t_rec_event.trigger_bit[i * 64 + j] && t_rec_event.energy_dep[i * 64 + j] > 0) {
                    bar_queue.push(Bar(t_rec_event.energy_dep[i * 64 + j], i, j));
                }
            }
        }
    }
    if (bar_queue.empty())
        return false;
    first_bar = bar_queue.top();
    bar_queue.pop();
    first_pos.randomize(first_bar.i, first_bar.j);
    bool found_not_adjacent = false;
    while (!bar_queue.empty()) {
        second_bar = bar_queue.top();
        bar_queue.pop();
        second_pos.randomize(second_bar.i, second_bar.j);
        if (!first_pos.is_adjacent_to(second_pos)) {
            found_not_adjacent = true;
            break;
        }
    }
    if (!found_not_adjacent) {
        return false;
    } else {
        is_first_two_ready = true;
        return true;
    }
}

bool RecEventIterator::cur_is_na22() {
    if (!is_first_two_ready) {
        return false;
    }   
    // calculate angle and distance to the 4 Na22 sources
    double src_angle[4];
    double src_distance[4];
    for (int i = 0; i < 4; i++) {
        src_angle[i] = angle_of_3_points_(SourcePos[i][0],  SourcePos[i][1],
                                          first_pos.abs_x,  first_pos.abs_y,
                                          second_pos.abs_x, second_pos.abs_y);
        src_distance[i] = distance_of_3_points_(SourcePos[i][0],  SourcePos[i][1],
                                                first_pos.abs_x,  first_pos.abs_y,
                                                second_pos.abs_x, second_pos.abs_y);
    }   
    // find the largest angle
    double largest_angle = 0;
    int    largest_index = 0;
    for (int i = 0; i < 4; i++) {
        if (src_angle[i] > largest_angle) {
            largest_angle = src_angle[i];
            largest_index = i;
        }   
    }   
    if (largest_angle < AngleMin) {
        return false;
    }   
    if (src_distance[largest_index] > DistanceMax) {
        return false;
    } else {
        return true;
    }   
}

