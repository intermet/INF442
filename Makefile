CXX = mpic++ -O3 -std=c++11

relation.o: relation.hpp relation.cpp
	$(CXX) -c relation.cpp -o relation.o

hash.o: hash.hpp hash.cpp
	$(CXX) -c hash.cpp -o hash.o

distribued.o: relation.o  hash.o distribued.hpp distribued.cpp
	$(CXX) -c relation.o hash.o distribued.cpp -o distribued.o

hypercube.o: relation.o hypercube.hpp hypercube.cpp
	$(CXX) -c relation.o hypercube.cpp -o hypercube.o

main: distribued.o hypercube.o relation.o main.cpp
	$(CXX) distribued.o relation.o hash.o hypercube.o main.cpp -o main


