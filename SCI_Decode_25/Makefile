CC := g++
CC_FLAGS := -Wl,--no-as-needed -Wall -O2 -I./include
ROOT_FLAGS := `root-config --cflags --glibs`
CPP_FILES := $(wildcard src/*.cpp)
OBJ_FILES := $(addprefix obj/,$(notdir $(CPP_FILES:.cpp=.o)))

.PHONY : all clean
all: ../bin/SCI_Decode_25

../bin/SCI_Decode_25: $(OBJ_FILES)
	$(CC) -o $@ $^ $(ROOT_FLAGS)

obj/%.o: src/%.cpp
	$(CC) $(CC_FLAGS) -c -o $@ $< $(ROOT_FLAGS)

clean:
	@echo "Cleaning ..."
	@rm obj/*.o

