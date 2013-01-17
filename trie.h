// Author Phinfinity

#ifndef PHINFINITY_TRIE_H
#define PHINFINITY_TRIE_H
#include <cstdio>
extern int convdict[256];
void ptrie_init();
template<typename value>
struct ptrie {
   ptrie<value> *root[36];
   value v;
   ptrie() {
      for(int i = 0 ; i < 36;i++)
	 root[i] = NULL;
      v=0;
   }
   ~ptrie() {
      for(int i = 0; i < 36; i++)
	 if(root[i])
	    delete root[i];
   }
   void add(const char* s,value val) {
      ptrie<value> *at =this;
      int c;
      while(*s) {
	 c = convdict[(int)*(s++)];
	 if(at->root[c] == NULL)at->root[c] = new ptrie<value>();
	 at = at->root[c];
      }
      at->v = val;
   }
   value& get(const char *s) {
      static int zer;
      zer = 0;
      ptrie *at =this;
      int c;
      while(*s) {
	 c = convdict[(int)*(s++)];
	 if(at->root[c] == NULL)return zer;
	 at = at->root[c];
      }
      return at->v;
   }
};
struct ptrie_set {
   ptrie<int> trie;
   int cnt;
   ptrie_set() {
      cnt = 0;
   }
   int el(const char* s) {
      ptrie<int> *at = &trie;
      int c;
      while(*s) {
	 c = convdict[(int)*(s++)];
	 if(at->root[c] == NULL)at->root[c] = new ptrie<int>();
	 at = at->root[c];
      }
      if(at->v==0)
	 at->v = ++cnt;
      return at->v;
   }
};
#endif
