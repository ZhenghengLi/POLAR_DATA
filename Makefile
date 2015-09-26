
main: main.cpp SciFrame.cpp SciFrame.hpp FileList.cpp FileList.hpp SciEvent.cpp SciEvent.hpp SciTrigger.cpp SciTrigger.hpp process_packet.hpp
	g++ -Wall -O2 main.cpp SciFrame.cpp FileList.cpp SciEvent.cpp SciTrigger.cpp -o main

clean:
	rm *.o
