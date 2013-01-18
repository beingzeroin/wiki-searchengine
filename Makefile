CCFLAGS = -O2 -std=c++11 -Wall
all : xmlparse2 tokenizer mergeindex

xmlparse2 : xmlparse2.o
	g++ $(CCFLAGS) xmlparse2.o -o xmlparse2
tokenizer : tokenizer.o stem.o tokenize.o stem.o trie.o varbyteencoder.o
	g++ $(CCFLAGS) tokenizer.o tokenize.o stem.o trie.o varbyteencoder.o -o tokenizer
mergeindex : mergeindex.cpp varbyteencoder.o
	g++ $(CCFLAGS) -lrt mergeindex.cpp -o mergeindex 
xmlparse2.o : xmlparse2.cpp
	g++ -c $(CCFLAGS) xmlparse2.cpp

tokenizer.o : tokenizer.cpp
	g++ -c $(CCFLAGS) tokenizer.cpp

stem.o : stem.c
	g++ -c $(CCFLAGS) -Wno-write-strings stem.c

tokenize.o : tokenize.cpp
	g++ -c $(CCFLAGS) tokenize.cpp

trie.o : trie.cpp
	g++ -c $(CCFLAGS) trie.cpp

varbyteencoder.o : varbyteencoder.cpp
	g++ -c $(CCFLAGS) varbyteencoder.cpp


clean:
	rm -rf xmlparse2 tokenizer mergeindex *.o
