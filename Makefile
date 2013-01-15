CCFLAGS = -g -Wall
all : index
index : index.o
	g++ $(CCFLAGS) index.o -o index

index.o : index.cpp
	g++ -c $(CCFLAGS) index.cpp

clean:
	rm -rf index *.o
