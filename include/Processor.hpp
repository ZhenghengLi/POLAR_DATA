#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <iostream>
#include <cstdlib>
#include <iomanip>
#include "SciFrame.hpp"
#include "SciEvent.hpp"
#include "SciTrigger.hpp"
#include "Counter.hpp"

using namespace std;

class Processor {
public:
	Counter cnt;
	SciEvent sci_event;
	SciTrigger sci_trigger;
	int64_t ped_trig[25];
	int64_t ped_event[25];
	int64_t noped_trig[25];
	int64_t noped_event[25];
	
public:
	Processor();
	~Processor();
	void initialize();
	bool process_frame(SciFrame& frame);	
	void process_packet(SciFrame& frame);
};

#endif
