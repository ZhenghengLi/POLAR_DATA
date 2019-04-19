CC := g++
CC_FLAGS := -Wall -O2 -I./include
ifeq ($(shell uname), Darwin)
    LINK_FLAGS :=
else
    LINK_FLAGS := -Wl,--no-as-needed
endif
ROOT_FLAGS := $(shell root-config --cflags)
ROOT_LIBS := $(shell root-config --libs)
CPP_FILES := $(wildcard src/*.cpp)
OBJ_FILES := $(addprefix obj/,$(notdir $(CPP_FILES:.cpp=.o)))
DEST_BIN := ../../bin

.PHONY : all clean

all: $(DEST_BIN)/HK_RawTime

$(DEST_BIN)/HK_RawTime: $(OBJ_FILES)
	$(CC) $(LINK_FLAGS) $(BOOST_LIB) -o $@ $^ $(ROOT_LIBS)

obj/%.o: src/%.cpp
	$(CC) $(CC_FLAGS) $(BOOST_INC) $(ROOT_FLAGS) -c -o $@ $<

clean:
	@echo "Cleaning objects ..."
	@rm -f obj/*.o

