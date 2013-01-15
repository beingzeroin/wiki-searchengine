CCFLAGS = -g -Wall
index : index.o
	g++ $(CCFLAGS) index.o -o index

index.o : index.cpp
	g++ -c $(CCFLAGS) index.cpp
