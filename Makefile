
main: main.cpp SciFrame.cpp SciFrame.hpp FileList.cpp FileList.hpp
	g++ -Wall -O2 main.cpp SciFrame.cpp FileList.cpp -o main

clean:
	rm *.o
