CC := g++
CC_FLAGS := -Wall -O2
ROOT_FLAGS := $(shell root-config --cflags)
ROOT_LIBS := $(shell root-config --libs)

.PHONY: clean

all: bin/data_split bin/extract_trigger

bin/data_split: data_split.cpp
	$(CC) $(CC_FLAGS) -o $@ $^

bin/extract_trigger: extract_trigger.cpp
	$(CC) $(CC_FLAGS) $(ROOT_FLAGS) -o $@ $^ $(ROOT_LIBS)

clean:
	rm -f bin/data_split
	rm -f bin/extract_trigger
