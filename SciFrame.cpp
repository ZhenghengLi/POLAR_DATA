#include "SciFrame.hpp"
#include <cstring>

using namespace std;
using boost::crc_optimal;

SciFrame::SciFrame() {
	frame_data_ = NULL;
	pre_half_packet_ = new char[200];
	pre_half_packet_len_ = 0;
	pre_frame_index_ = 0;
	cur_packet_buffer_ = NULL;
	start_packet_pos_ = 22;
	cur_is_cross_ = true;
	reach_end_ = false;
}

SciFrame::SciFrame(const char* data) {
	frame_data_ = data;
	pre_half_packet_ = new char[200];
	pre_half_packet_len_ = 0;
	pre_frame_index_ = 0;
	cur_packet_buffer_ = NULL;
	start_packet_pos_ = 22;
	cur_is_cross_ = true;
	reach_end_ = false;
}

SciFrame::~SciFrame() {
	delete [] pre_half_packet_;
}

SciFrame::SciFrame(const SciFrame& other_frame) {
	pre_half_packet_ = new char[200];
	copy_frame_(other_frame);
}

const SciFrame& SciFrame::operator=(const SciFrame& other_frame) {
	if (this != &other_frame)
		copy_frame_(other_frame);
	return *this;
}

void SciFrame::copy_frame_(const SciFrame& other_frame) {
	frame_data_ = other_frame.frame_data_;
	pre_half_packet_len_ = other_frame.pre_half_packet_len_;
	pre_frame_index_ = other_frame.pre_frame_index_;
	start_packet_pos_ = other_frame.start_packet_pos_;
	cur_packet_pos_ = other_frame.cur_packet_pos_;
	cur_packet_len_ = other_frame.cur_packet_len_;
	cur_is_cross_ = other_frame.cur_is_cross_;
	reach_end_ = other_frame.reach_end_;
	for (int i = 0; i < pre_half_packet_len_; i++)
		pre_half_packet_[i] = other_frame.pre_half_packet_[i];
	if (other_frame.cur_packet_buffer_ == other_frame.pre_half_packet_)
		cur_packet_buffer_ = pre_half_packet_;
	else
		cur_packet_buffer_ = other_frame.cur_packet_buffer_;
}

void SciFrame::setdata(const char* data) {
	frame_data_ = data;
}

void SciFrame::reset() {
	frame_data_ = NULL;
	pre_half_packet_len_ = 0;
	pre_frame_index_ = 0;
	cur_packet_buffer_ = NULL;
	start_packet_pos_ = 22;
	cur_is_cross_ = true;
	reach_end_ = false;
}

bool SciFrame::check_valid() const {
	assert(frame_data_ != NULL);
	uint16_t frame_header = 0;
	for (int i = 0; i < 2; i++) {
		frame_header <<= 8;
		frame_header += static_cast<uint8_t>(frame_data_[0 + i]);
	}
	if (frame_header != 0x009F)
		return false;
	
	uint16_t frame_length = 0;
	for (int i = 0; i < 2; i++) {
		frame_length <<= 8;
		frame_length += static_cast<uint8_t>(frame_data_[4 + i]);
	}
	if (frame_length != 0x07F9)
		return false;

	uint32_t sci_stater = 0;
	for (int i = 0; i < 4; i++) {
		sci_stater <<= 8;
		sci_stater += static_cast<uint8_t>(frame_data_[6 + i]);
	}
	if (sci_stater != 0x706F6C61)
		return false;

	return true;
}

uint16_t SciFrame::get_bits() const {
	assert(frame_data_ != NULL);
	uint8_t tmp = 0;
	tmp += static_cast<uint8_t>(frame_data_[2]);
	return static_cast<uint16_t>(tmp >> 6);
}

uint16_t SciFrame::get_index() const {
	assert(frame_data_ != NULL);
	uint16_t sum = 0;
	sum += static_cast<uint8_t>(frame_data_[2] & 0x3F);
	sum <<= 8;
	sum += static_cast<uint8_t>(frame_data_[3]);
	return sum;
}

bool SciFrame::check_crc() {
	assert(frame_data_ != NULL);
	uint32_t expected, result;
	expected = 0;
	for (int i = 0; i < 4; i++) {
		expected <<= 8;
		expected += static_cast<uint8_t>(frame_data_[2048 + i]);
	}
	crc_32_.reset();
	crc_32_.process_bytes( frame_data_, 2048 );
	result = crc_32_.checksum();
	if (result == expected)
		return true;
	else
		return false;
}

void SciFrame::write_root() {
	// todo : split and make correlation
}

void SciFrame::updated() {
	reach_end_ = false;
}

bool SciFrame::next_packet() {
	assert(frame_data_ != NULL);
	if (reach_end_) {
		return false;
	} else if (pre_half_packet_len_ > 0) {
		memcpy(pre_half_packet_ + pre_half_packet_len_, frame_data_ + 22, cur_packet_len_
			   - pre_half_packet_len_);
		cur_packet_buffer_ = pre_half_packet_;		
		pre_half_packet_len_ = 0;
		return true;
	} else if (cur_is_cross_) {
		cur_is_cross_ = false;
		pre_frame_index_ = get_index();
		cur_packet_pos_ = start_packet_pos_;
		cur_packet_len_ = get_cur_packet_len_();		
		cur_packet_buffer_ = frame_data_ + cur_packet_pos_;
		return true;
	} else {
		cur_packet_pos_ += cur_packet_len_;
		cur_packet_len_ = get_cur_packet_len_();
		if (cur_packet_pos_ + cur_packet_len_ < 2048) {
			cur_packet_buffer_ = frame_data_ + cur_packet_pos_;
			return true;
		} else if (cur_packet_pos_ + cur_packet_len_ == 2048) {
			reach_end_ = true;
			cur_is_cross_ = true;
			pre_half_packet_len_ = 0;			
			start_packet_pos_ = 22;
			cur_packet_buffer_ = frame_data_ + cur_packet_pos_;			
			return true;
		} else {
			reach_end_ = true;
			cur_is_cross_ = true;
			pre_half_packet_len_ = 2048 - cur_packet_pos_;
			start_packet_pos_ = 22 + cur_packet_len_ - pre_half_packet_len_;
			memcpy(pre_half_packet_, frame_data_ + cur_packet_pos_, pre_half_packet_len_);
			return false;
		}
	}
}

bool SciFrame::cur_is_trigger() const {
	assert(cur_packet_buffer_ != NULL);
	uint16_t second = 0;
	for (int i = 0; i < 2; i++) {
		second <<= 8;
		second += static_cast<uint8_t>(cur_packet_buffer_[2 + i]);
	}
	if (second > 25)
		return true;
	else
		return false;
}

uint16_t SciFrame::cur_get_mode() const {
	assert(cur_packet_buffer_ != NULL);
	if (cur_is_trigger())
		return 0x0F00;
	uint16_t modeBit = 0;
	for (int i = 0; i < 2; i++) {
		modeBit <<= 8;
		modeBit += static_cast<uint8_t>(cur_packet_buffer_[6 + i]);
	}
	modeBit &= 0x180;
	modeBit >>= 7;
	return modeBit;
}

bool SciFrame::cur_check_crc() {
	assert(cur_packet_buffer_ != NULL);
	uint16_t expected, result;	
	if (cur_is_trigger()) {
		expected = 0;
		for (int i = 0; i < 2; i++) {
			expected <<= 8;
			expected += static_cast<uint8_t>(cur_packet_buffer_[cur_packet_len_ - 2 + i]);
		}
		crc_ccitt_.reset();
		crc_ccitt_.process_bytes(cur_packet_buffer_ + 4, cur_packet_len_ - 6);
		result = crc_ccitt_.checksum();
		if (result == expected)
			return true;
	} else {
		expected = 0;
		for (int i = 0; i < 2; i++) {
			expected <<= 8;
			expected += static_cast<uint8_t>(cur_packet_buffer_[cur_packet_len_ - 4 + i]);
		}
		crc_ccitt_.reset();
		crc_ccitt_.process_bytes(cur_packet_buffer_ + 4, cur_packet_len_ - 8);
		result = crc_ccitt_.checksum();
		if (result == expected)
			return true;
	}
	return false;
}

uint16_t SciFrame::cur_get_ctNum() const {
	assert(cur_packet_buffer_ != NULL);
	if (cur_is_trigger()) 
		return 0xFF00;
	uint16_t ctNum = 0;
	for (int i = 0; i < 2; i++) {
		ctNum <<= 8;
		ctNum += static_cast<uint8_t>(cur_packet_buffer_[2 + i]);
	}
	return ctNum;
}

bool SciFrame::cur_check_valid() const {
	assert(cur_packet_buffer_ != NULL);
	uint16_t tmp;
	if (cur_packet_len_ < 28)
		return false;
	if (cur_is_trigger()) {
		tmp = 0;
		for (int i = 0; i < 2; i++) {
			tmp <<= 8;
			tmp += static_cast<uint8_t>(cur_packet_buffer_[2 + i]);
		}
		if (!(tmp == 0x00F0 || tmp == 0x00FF || tmp == 0xFF00 || tmp == 0xF000)) {
			return false;
		}
		return true;
	} else {
		if (cur_get_ctNum() > 25) {
			return false;
		}
		tmp = 0;
		for (int i = 0; i < 2; i++) {
			tmp <<= 8;
			tmp += static_cast<uint8_t>(cur_packet_buffer_[cur_packet_len_ - 2 + i]);
		}
		if (tmp != 0xFFFF) {
			return false;
		}
		return true;
	}
}

bool SciFrame::find_start_pos() {
	assert(frame_data_ != NULL);
	start_packet_pos_ = 21;
	for (int i = 0; i < 200; i++) {
		start_packet_pos_ += 1;
		cur_packet_pos_ = start_packet_pos_;
		cur_packet_len_ = get_cur_packet_len_();
		cur_packet_buffer_ = frame_data_ + cur_packet_pos_;		
		if (cur_check_valid() && cur_check_crc()) {
			pre_half_packet_len_ = 0;
			cur_is_cross_ = true;
			reach_end_ = false;
			return true;
		}
	}
	return false;
}

const char* SciFrame::get_cur_pkt_buf() {
	return cur_packet_buffer_;
}

size_t SciFrame::get_cur_pkt_len() {
	return static_cast<size_t>(cur_packet_len_);
}

bool SciFrame::can_connect() {
	assert(frame_data_ != NULL);
	uint16_t cur_frm_index = get_index();
	assert(cur_is_cross_);
	if (pre_half_packet_len_ == 0) {
		if (cur_frm_index == pre_frame_index_ + 1
			|| ((pre_frame_index_ == 16383) && (cur_frm_index == 0)))
			return true;
		else
			return false;
	} else {
		char tmp_packet[200];
		const char* tmp_pointer = cur_packet_buffer_;
		memcpy(tmp_packet, pre_half_packet_, pre_half_packet_len_);
		memcpy(tmp_packet + pre_half_packet_len_, frame_data_ + 22, cur_packet_len_
			   - pre_half_packet_len_);
		cur_packet_buffer_ = tmp_packet;
		if (cur_check_valid() && cur_check_crc()) {
			cur_packet_buffer_ = tmp_pointer;
			return true;
		} else {
			cur_packet_buffer_ = tmp_pointer;
			return false;
		}
	}
}

void SciFrame::print_packet() {
	cout << "^^^^^^^^^^" << endl;
	cout << "cur_packet_len_: " << cur_packet_len_ << endl; //for debug
	for (int i = 0; i < cur_packet_len_; i++)
		cout << hex << uppercase << setfill('0') << setw(2)
			 << (int)(*((uint8_t*)(&cur_packet_buffer_[i]))) << " ";
	cout << dec << endl;
	cout << ".........." << endl;
}

void SciFrame::process(int* counts) {
	if (check_valid()) {
	} else {
		cout << "This frame is not valid." << endl;
		return;
	}
	if (check_crc()) {
	} else {
		cout << "frame CRC Error!" << endl;
		return;
	}
	int pkt_cnt = 0;
	while(next_packet()) {
		pkt_cnt++;
		if (cur_is_trigger()) {
			counts[0]++;
		} else {
			counts[1]++;
		}
		if (cur_check_valid()) {
			counts[2]++;
		} else {
			cout << endl;
			cout << "packet invalid" << endl;
			print_packet();
			counts[3]++;
		}
		if (cur_check_crc()) {
			counts[4]++;
		} else {
			cout << endl;
			cout << "packet crc error" << endl;
			print_packet();
			counts[5]++;
		}
		if (cur_packet_len_ < 28) {
			counts[6]++;
		}
	}
}

