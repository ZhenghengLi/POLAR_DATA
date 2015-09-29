
main: main.cpp SciFrame.cpp SciFrame.hpp FileList.cpp FileList.hpp SciEvent.cpp SciEvent.hpp SciTrigger.cpp SciTrigger.hpp Processor.cpp Processor.hpp Decoder.hpp Counter.hpp
	g++ -Wall -O2 main.cpp SciFrame.cpp FileList.cpp SciEvent.cpp SciTrigger.cpp Processor.cpp -o main

clean:
	rm *.o
