CC := g++
CC_FLAGS := -Wall -O2 -I./include
ROOT_FLAGS := $(shell root-config --cflags)
ROOT_GLIBS := $(shell root-config --glibs)

.PHONY : all clean test PN_Subtract XT_Correct CE_Calibrate
all: test PN_Subtract XT_Correct CE_Calibrate
test: ../bin/analysis_test
PN_Subtract: ../bin/PN_Subtract
XT_Correct: ../bin/XT_Correct
CE_Calibrate: ../bin/CE_Calibrate

../bin/analysis_test: src/test.cpp src/EventIterator.cpp src/PhyEventFile.cpp
	$(CC) $(CC_FLAGS) $(ROOT_FLAGS) -o $@ $^ $(ROOT_GLIBS)

../bin/PN_Subtract: src/PN_Subtract.cpp src/EventIterator.cpp src/PhyEventFile.cpp src/PedMeanCalc.cpp src/PedMeanCalc_Dict.cpp
	$(CC) $(CC_FLAGS) $(ROOT_FLAGS) -o $@ $^ $(ROOT_GLIBS)

src/PedMeanCalc_Dict.cpp: include/PedMeanCalc.hpp
	cd include; rootcint -f PedMeanCalc_Dict.cpp -c PedMeanCalc.hpp
	mv include/PedMeanCalc_Dict.cpp src/

../bin/XT_Correct: src/XT_Correct.cpp src/PhyEventFile.cpp src/CrossTalkCalc.cpp src/CrossTalkCalc_Dict.cpp
	$(CC) $(CC_FLAGS) $(ROOT_FLAGS) -o $@ $^ $(ROOT_GLIBS)

src/CrossTalkCalc_Dict.cpp: include/CrossTalkCalc.hpp
	cd include; rootcint -f CrossTalkCalc_Dict.cpp -c CrossTalkCalc.hpp
	mv include/CrossTalkCalc_Dict.cpp src/

../bin/CE_Calibrate: src/CE_Calibrate.cpp src/PhyEventFile.cpp src/ComptonEdgeCalc.cpp src/ComptonEdgeCalc_Dict.cpp
	$(CC) $(CC_FLAGS) $(ROOT_FLAGS) -o $@ $^ $(ROOT_GLIBS)

src/ComptonEdgeCalc_Dict.cpp: include/ComptonEdgeCalc.hpp
	cd include; rootcint -f ComptonEdgeCalc_Dict.cpp -c ComptonEdgeCalc.hpp
	mv include/ComptonEdgeCalc_Dict.cpp src/

clean:
	@echo "Cleaning ..."
	@rm -f ../bin/analysis_test
	@rm -f ../bin/PN_Subtract
	@rm -f include/PedMeanCalc_Dict.h src/PedMeanCalc_Dict.cpp
	@rm -f ../bin/XT_Correct
	@rm -f include/CrossTalkCalc_Dict.h src/CrossTalkCalc_Dict.cpp
	@rm -f ../bin/CE_Calibrate
	@rm -f include/ComptonEdgeCalc_Dict.h src/ComptonEdgeCalc_Dict.cpp

