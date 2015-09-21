
className = FileList

main: main.cpp $(className).cpp $(className).hpp
	g++ -Wall -O2 main.cpp $(className).cpp -o main

clean:
	rm *.o
