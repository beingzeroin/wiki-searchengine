// Author Phinfinity

#ifndef PHINFINITY_TRIE_H
#define PHINFINITY_TRIE_H
#include <cstdio>
#include <string>
#include <unordered_map>
using std::string;
using std::unordered_map;
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
   unordered_map<string,int> m;
   int el(const char* s) {
      int c = m[s];
      if(c==0) {
	 c = m.size();
	 m[s]=c;
      }
      return c;
   }
   bool has(const char* s) {
      return m.count(s)!=0;
   }
};
/*
struct ptrie_set {
   ptrie<int> trie;
   int cnt;
   size_t cap;
   ptrie_set() {
      cnt = 0;
      ptrie_init();
      cap = 0;
   }
   int el(const char* s) {
      ptrie<int> *at = &trie;
      int c;
      while(*s) {
	 c = convdict[(int)*(s++)];
	 if(at->root[c] == NULL){
	    at->root[c] = new ptrie<int>();
	    cap++;
	 }
	 at = at->root[c];
      }
      if(at->v==0)
	 at->v = ++cnt;
      return at->v;
   }
   bool has(const char*s) {
      ptrie<int> *at = &trie;
      int c;
      while(*s) {
	 c = convdict[(int)*(s++)];
	 if(at->root[c] == NULL)return false;
	 at = at->root[c];
      }
      return (at->v)!=0;
   }
};
*/
#endif
