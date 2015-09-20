main: main.o SciFrame.o
	g++ -Wall -O2 main.o SciFrame.o -o main

main.o: main.cpp SciFrame.hpp SciFrame.cpp
	g++ -Wall -O2 -c main.cpp -o main.o

SciFrame.o: SciFrame.hpp SciFrame.cpp
	g++ -Wall -O2 -c SciFrame.cpp -o SciFrame.o

clean:
	rm *.o
