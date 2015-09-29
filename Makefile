CC := g++
CC_FLAGS := -Wall -O2 -I./include
CPP_FILES := $(wildcard src/*.cpp)
OBJ_FILES := $(addprefix obj/,$(notdir $(CPP_FILES:.cpp=.o)))

bin/main: $(OBJ_FILES)
	$(CC) -o $@ $^

obj/%.o: src/%.cpp
	$(CC) $(CC_FLAGS) -c -o $@ $<

clean:
	rm obj/*.o

