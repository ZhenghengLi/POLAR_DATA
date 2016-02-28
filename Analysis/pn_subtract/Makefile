CC := g++
MY_LIB := ../lib
CC_FLAGS := -Wall -O2 -I ./include -I $(MY_LIB)/include
ROOT_FLAGS := $(shell root-config --cflags)
ROOT_LIBS := $(shell root-config --libs)

.PHONY : all clean

all: ../bin/pn_subtract ../bin/pn_fit

../bin/pn_subtract: obj/pn_subtract.o obj/PedTest.o $(MY_LIB)/obj/EventIterator.o
	$(CC) -o $@ $^ $(ROOT_LIBS)

../bin/pn_fit: obj/pn_fit.o obj/PedTest.o $(MY_LIB)/obj/ModuleIterator.o
	$(CC) -o $@ $^ $(ROOT_LIBS)

obj/%.o: src/%.cpp
	$(CC) $(CC_FLAGS) $(ROOT_FLAGS) -c -o $@ $<

$(MY_LIB)/obj/%.o: $(MY_LIB)/src/%.cpp
	$(CC) $(CC_FLAGS) $(ROOT_FLAGS) -c -o $@ $<

clean:
	@echo "Cleaning objects ..."
	@rm -f obj/*.o

