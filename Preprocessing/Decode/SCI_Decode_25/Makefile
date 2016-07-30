CC := g++
CC_FLAGS := -Wall -O2 -I./include
LINK_FLAGS := -Wl,--no-as-needed
ROOT_FLAGS := $(shell root-config --cflags)
ROOT_LIBS := $(shell root-config --libs)
CPP_FILES := $(wildcard src/*.cpp)
OBJ_FILES := $(addprefix obj/,$(notdir $(CPP_FILES:.cpp=.o)))
DEST_BIN := ../../bin

.PHONY : all clean

all: $(DEST_BIN)/SCI_Decode_25

$(DEST_BIN)/SCI_Decode_25: $(OBJ_FILES)
	$(CC) $(LINK_FLAGS) -o $@ $^ $(ROOT_LIBS)

obj/%.o: src/%.cpp
	$(CC) $(CC_FLAGS) $(ROOT_FLAGS) -c -o $@ $<

clean:
	@echo "Cleaning objects ..."
	@rm -f obj/*.o

