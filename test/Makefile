CC := g++
CC_FLAGS := -Wl,--no-as-needed -Wall -O2 -I../SCI_Decode/include -I../Analysis/include
ROOT_FLAGS := `root-config --cflags --glibs`

test: test.cpp ../Analysis/src/EventIterator.cpp
	$(CC) $(CC_FLAGS) -o $@ $^ $(ROOT_FLAGS)
