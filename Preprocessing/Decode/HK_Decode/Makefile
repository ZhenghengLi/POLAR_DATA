CC := g++
CC_FLAGS := -Wall -O2 -I./include
LINK_FLAGS := -Wl,--no-as-needed
ROOT_FLAGS := $(shell root-config --cflags)
ROOT_LIBS := $(shell root-config --libs)
CPP_FILES := $(wildcard src/*.cpp)
OBJ_FILES := $(addprefix obj/,$(notdir $(CPP_FILES:.cpp=.o)))
DEST_BIN := ../../bin

.PHONY : all clean

all: $(DEST_BIN)/HK_Decode

$(DEST_BIN)/HK_Decode: $(OBJ_FILES)
	$(CC) $(LINK_FLAGS) -o $@ $^ $(ROOT_LIBS) $(BOOST_LIB)

obj/%.o: src/%.cpp
	$(CC) $(CC_FLAGS) $(ROOT_FLAGS) $(BOOST_INC) -c -o $@ $<

clean:
	@echo "Cleaning objects ..."
	@rm -f obj/*.o

