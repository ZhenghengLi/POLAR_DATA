CC := g++
CC_FLAGS := -Wall -O2 -I./include
ROOT_FLAGS := $(shell root-config --cflags)
ROOT_GLIBS := $(shell root-config --glibs)

.PHONY : all clean test
all: ../bin/analysis_test
test: ../bin/analysis_test

../bin/analysis_test: src/test.cpp src/EventIterator.cpp src/PhyEventFile.cpp
	$(CC) $(CC_FLAGS) $(ROOT_FLAGS) -o $@ $^ $(ROOT_GLIBS)

clean:
	@echo "Cleaning ..."
	@rm -f ../bin/analysis_test

