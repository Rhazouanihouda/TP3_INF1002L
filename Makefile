all: ufs

ufs: disqueVirtuel.o block.o main.o
	g++ disqueVirtuel.o block.o main.o -o ufs

main.o: main.cpp
	g++ -c main.cpp -std=c++11 -g -Wall 

disqueVirtuel.o: disqueVirtuel.cpp
	g++ -c disqueVirtuel.cpp -std=c++11 -g -Wall 

block.o: block.cpp
	g++ -c block.cpp -std=c++11 -g -Wall 

clean: 
	rm main.o disqueVirtuel.o block.o ufs
