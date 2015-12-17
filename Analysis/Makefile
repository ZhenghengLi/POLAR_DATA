CC := g++
CC_FLAGS := -Wall -O2 -I./include
ROOT_FLAGS := $(shell root-config --cflags)
ROOT_GLIBS := $(shell root-config --glibs)

.PHONY : all clean test PN_Subtract
all: test PN_Subtract
test: ../bin/analysis_test
PN_Subtract: ../bin/PN_Subtract

../bin/analysis_test: src/test.cpp src/EventIterator.cpp src/PhyEventFile.cpp
	$(CC) $(CC_FLAGS) $(ROOT_FLAGS) -o $@ $^ $(ROOT_GLIBS)

../bin/PN_Subtract: src/PN_Subtract.cpp src/EventIterator.cpp src/PhyEventFile.cpp src/PedMeanCalc.cpp src/PedMeanCalc_Dict.cpp
	$(CC) $(CC_FLAGS) $(ROOT_FLAGS) -o $@ $^ $(ROOT_GLIBS)

src/PedMeanCalc_Dict.cpp: include/PedMeanCalc.hpp
	cd include; rootcint -f PedMeanCalc_Dict.cpp -c PedMeanCalc.hpp
	mv include/PedMeanCalc_Dict.cpp src/

clean:
	@echo "Cleaning ..."
	@rm -f ../bin/analysis_test
	@rm -f ../bin/PN_Subtract
	@rm -f include/PedMeanCalc_Dict.h src/PedMeanCalc_Dict.cpp

