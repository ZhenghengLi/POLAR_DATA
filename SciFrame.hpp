#include <cstddef>
#include <cassert>
#include <boost/crc.hpp>

using boost::crc_optimal;

class SciFrame {
private:
	const char* frame_data_;	
	char* pre_half_packet_;
	bool has_pre_;
	crc_optimal<32, 0x04C11DB7> crc_32_;
public:
	SciFrame();
	SciFrame(const char*);
	~SciFrame();
	void setdata(const char*);
	void reset();
	bool check_valid() const;
	uint16_t get_bits() const;
	uint16_t get_index() const;
	bool check_crc();
	void write_root();
};
