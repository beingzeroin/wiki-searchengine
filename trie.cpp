// Author Phinfinity
#include "trie.h"
#include <cstdio>
int convdict[256];
void ptrie_init() {               
   int i;
   for(i='a';i<='z';i++)
      convdict[i] = i-'a'+10;
   for(i='0';i<='9';i++)
      convdict[i] = i-'0';

}

