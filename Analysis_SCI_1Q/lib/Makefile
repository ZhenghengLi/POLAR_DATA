CC := g++
CC_FLAGS := -Wall -O2 -I./include
ROOT_FLAGS := $(shell root-config --cflags)
ROOT_LIBS := $(shell root-config --libs)
CPP_FILES := $(wildcard src/*.cpp)
OBJ_FILES := $(addprefix obj/,$(notdir $(CPP_FILES:.cpp=.o)))

.PHONY : all clean

all: $(OBJ_FILES)

obj/%.o: src/%.cpp
	$(CC) $(CC_FLAGS) $(BOOST_INC) $(ROOT_FLAGS) -c -o $@ $<

clean:
	@echo "Cleaning objects ..."
	@rm -f obj/*.o

