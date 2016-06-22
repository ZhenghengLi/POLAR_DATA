CC := g++
MY_LIB := ../lib
CC_FLAGS := -Wall -O2 -I ./include -I $(MY_LIB)/include
LINK_FLAGS := -Wl,--no-as-needed -lboost_regex
ROOT_FLAGS := $(shell root-config --cflags)
ROOT_LIBS := $(shell root-config --libs)

.PHONY : all clean

.PRECIOUS: src/%_Dict.cpp

all: ../bin/pn_subtract ../bin/pn_fit

../bin/pn_subtract: obj/pn_subtract.o obj/PedTest.o $(MY_LIB)/obj/EventIterator.o $(MY_LIB)/obj/SciType.o
	$(CC) $(LINK_FLAGS) -o $@ $^ $(ROOT_LIBS)

../bin/pn_fit: obj/pn_fit.o obj/PedTest.o $(MY_LIB)/obj/ModuleIterator.o $(MY_LIB)/obj/SciType.o obj/GuiTest.o obj/GuiTest_Dict.o
	$(CC) $(LINK_FLAGS) -o $@ $^ $(ROOT_LIBS)

obj/%.o: src/%.cpp
	$(CC) $(CC_FLAGS) $(ROOT_FLAGS) -c -o $@ $<

src/%_Dict.cpp: include/%.hpp
	cd include && \
		rootcint -f $(@:src/%_Dict.cpp=%_Dict.cpp) -c -I../$(MY_LIB)/include $(<:include/%.hpp=%.hpp) && \
		mv $(@:src/%_Dict.cpp=%_Dict.cpp) ../src/

$(MY_LIB)/obj/%.o: $(MY_LIB)/src/%.cpp
	$(CC) $(CC_FLAGS) $(ROOT_FLAGS) -c -o $@ $<

clean:
	@echo "Cleaning objects ..."
	@rm -f obj/*.o
	@rm -f include/*_Dict.h
	@rm -f src/*_Dict.cpp

