#include "SciFrame.hpp"

using boost::crc_optimal;

SciFrame::SciFrame() {
	frame_data_ = NULL;
	pre_half_packet_ = new char[200];
	has_pre_ = false;
}

SciFrame::SciFrame(const char* data) {
	frame_data_ = data;
	pre_half_packet_ = new char[200];
	has_pre_ = false;
}

SciFrame::~SciFrame()
{
	delete [] pre_half_packet_;
}

void SciFrame::setdata(const char* data) {
	frame_data_ = data;
}

void SciFrame::reset()
{
	frame_data_ = NULL;
	has_pre_ = false;
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
	uint32_t expected, res;
	expected = 0;
	for (int i = 0; i < 4; i++) {
		expected <<= 8;
		expected += static_cast<uint8_t>(frame_data_[2048 + i]);
	}
	crc_32_.reset();
	crc_32_.process_bytes( frame_data_, 2048 );
	res = crc_32_.checksum();
	if (res == expected)
		return true;
	else
		return false;
}

void SciFrame::write_root()
{
	// todo : split and make correlation
}


