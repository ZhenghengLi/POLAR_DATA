crc_data_class: crc_data_class.o SciFrame.o
	g++ -Wall -O2 crc_data_class.o SciFrame.o -o crc_data_class

crc_data_class.o: crc_data_class.cpp SciFrame.hpp SciFrame.cpp
	g++ -Wall -O2 -c crc_data_class.cpp -o crc_data_class.o

SciFrame.o: SciFrame.hpp SciFrame.cpp
	g++ -Wall -O2 -c SciFrame.cpp -o SciFrame.o

clean:
	rm *.o
