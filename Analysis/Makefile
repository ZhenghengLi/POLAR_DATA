CC := g++
CC_FLAGS := -Wl,--no-as-needed -Wall -O2 -I./include
ROOT_FLAGS := `root-config --cflags --glibs`

.PHONY : all clean test
all: ../bin/analysis_test
test: ../bin/analysis_test

../bin/analysis_test: src/test.cpp src/EventIterator.cpp src/PhyEventFile.cpp
	$(CC) $(CC_FLAGS) -o $@ $^ $(ROOT_FLAGS)

clean:
	@echo "Cleaning ..."
	@rm ../bin/analysis_test

