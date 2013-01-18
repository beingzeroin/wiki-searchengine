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
struct ptrie_set_umap {
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
struct ptrie_set_trie {
   ptrie<int> trie;
   int cnt;
   size_t cap;
   ptrie_set_trie() {
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

struct hybrid_node {
   hybrid_node* root[36];
   int v;
   unordered_map<string,int> *m;
   hybrid_node() {
      v = 0;
      m = NULL;
      for(int i = 0; i < 36; i++)
	 root[i] = NULL;
   }
   ~hybrid_node() {
      for(int i = 0; i < 36; i++)
	 if(root[i])
	    delete root[i];
      if(m)
	 delete m;
   }
};
#define HYBRID_TRIE_LIM 5
struct ptrie_set_hybrid {
   hybrid_node trie;
   int cnt;
   size_t cap;
   ptrie_set_hybrid() {
      cnt = 0;
      ptrie_init();
      cap = 0;
   }
   int el(const char* s) {
      hybrid_node *at = &trie;
      int c;
      int d = 0;
      while(*s) {
	 c = convdict[(int)*(s++)];
	 if(at->root[c] == NULL){
	    at->root[c] = new hybrid_node();
	    cap++;
	 }
	 at = at->root[c];
	 d++;
	 if(d==HYBRID_TRIE_LIM && *s) {
	    if(at->m == NULL)
		  at->m = new unordered_map<string,int>();
	    int c = (*at->m)[s];
	    if(c==0)
	       c = (*at->m)[s] = ++cnt;
	    return c;
	 }
      }
      if(at->v==0)
	 at->v = ++cnt;
      return at->v;
   }
   bool has(const char*s) {
      hybrid_node *at = &trie;
      int c;
      int d = 0;
      while(*s) {
	 c = convdict[(int)*(s++)];
	 if(at->root[c] == NULL)return false;
	 at = at->root[c];
	 d++;
	 if(d==HYBRID_TRIE_LIM && *s) {
	    if(at->m)
	       return (*at->m).count(s)!=0;
	    else
	       return false;
	 }
      }
      return (at->v)!=0;
   }
};
typedef ptrie_set_hybrid ptrie_set;
#endif
