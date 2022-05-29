main: GTU.o main.o
	g++ -o main main.o GTU.o -pthread

GTU.o : GTU.cpp
	g++ -c -std=c++11 -Wall GTU.cpp

main.o : main.cpp
	g++ -c -std=c++11 -Wall main.cpp

clean:
	rm *.o main
