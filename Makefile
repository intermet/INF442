CXX = mpic++ -O3 -std=c++11

relation.o: relation.hpp relation.cpp
	$(CXX) -c relation.cpp -o relation.o

hash.o: hash.hpp hash.cpp
	$(CXX) -c hash.cpp -o hash.o

distribued.o: relation.o  hash.o distribued.hpp distribued.cpp
	$(CXX) -c relation.o hash.o distribued.cpp -o distribued.o

main: distribued.o
	$(CXX) distribued.o relation.o hash.o main.cpp -o main

run:
	salloc

%:
	@:
