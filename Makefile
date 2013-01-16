CCFLAGS = -g -Wall
all : xmlparse2 tokenizer

xmlparse2 : xmlparse2.o
	g++ $(CCFLAGS) xmlparse2.o -o xmlparse2
xmlparse2.o : xmlparse2.cpp
	g++ -c $(CCFLAGS) xmlparse2.cpp

tokenizer : tokenizer.o
	g++ $(CCFLAGS) tokenizer.o -o tokenizer
tokenizer.o : tokenizer.cpp
	g++ -c $(CCFLAGS) tokenizer.cpp



clean:
	rm -rf xmlparse2 tokenizer *.o
