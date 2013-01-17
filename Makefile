CCFLAGS = -O2 -Wall
all : xmlparse2 tokenizer

xmlparse2 : xmlparse2.o
	g++ $(CCFLAGS) xmlparse2.o -o xmlparse2
xmlparse2.o : xmlparse2.cpp
	g++ -c $(CCFLAGS) xmlparse2.cpp

tokenizer : tokenizer.o stem.o tokenize.o stem.o trie.o
	g++ $(CCFLAGS) tokenizer.o tokenize.o stem.o trie.o -o tokenizer
tokenizer.o : tokenizer.cpp
	g++ -c $(CCFLAGS) tokenizer.cpp

stem.o : stem.c
	g++ -c $(CCFLAGS) -Wno-write-strings stem.c

tokenize.o : tokenize.cpp
	g++ -c $(CCFLAGS) tokenize.cpp

trie.o : trie.cpp
	g++ -c $(CCFLAGS) trie.cpp


clean:
	rm -rf xmlparse2 tokenizer *.o
