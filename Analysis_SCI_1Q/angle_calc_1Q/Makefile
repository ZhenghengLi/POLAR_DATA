CC := g++
MY_LIB := ../lib
MY_LIB_INC := $(MY_LIB)/include
CC_FLAGS := -Wall -O2 -I ./include -I $(MY_LIB)/include
LINK_FLAGS := -Wl,--no-as-needed -lboost_regex
ROOT_FLAGS := $(shell root-config --cflags)
ROOT_LIBS := $(shell root-config --libs)
DEST_BIN := $(shell cd ../bin; pwd)

.PHONY : all clean

.PRECIOUS: src/%_Dict.cpp

all: $(DEST_BIN)/angle_calc_1Q

$(DEST_BIN)/angle_calc_1Q: obj/main.o
	$(CC) $(LINK_FLAGS) $(BOOST_LIB) -o $@ $^ $(ROOT_LIBS)

obj/%.o: src/%.cpp
	$(CC) $(CC_FLAGS) $(BOOST_INC) $(ROOT_FLAGS) -c -o $@ $<

src/%_Dict.cpp: include/%.hpp
	cd include && \
		rootcint -f $(@:src/%_Dict.cpp=%_Dict.cpp) -c $(<:include/%.hpp=%.hpp) && \
		mv $(@:src/%_Dict.cpp=%_Dict.cpp) ../src/
	cd include && \
		if [ -f $(@:src/%_Dict.cpp=%_Dict_rdict.pcm) ]; then cp $(@:src/%_Dict.cpp=%_Dict_rdict.pcm) $(DEST_BIN)/; fi

$(MY_LIB)/obj/%.o: $(MY_LIB)/src/%.cpp
	$(CC) $(CC_FLAGS) $(BOOST_INC) $(ROOT_FLAGS) -c -o $@ $<

clean:
	@echo "Cleaning objects ..."
	@rm -f obj/*.o
	@rm -f include/*_Dict.h
	@rm -f include/*_Dict_rdict.pcm
	@rm -f src/*_Dict.cpp

