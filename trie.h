// Author Phinfinity

#ifndef PHINFINITY_TRIE_H
#define PHINFINITY_TRIE_H
inline int conv(char c) {
   if(c<'a')
      return c-'0';
   else
      return c-'a'+10;
}
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
	 c = conv(*(s++));
	 if(root[c] == NULL)root[c] = new ptrie<value>();
	 at = root[c];
      }
      at->v = val;
   }
   value& get(const char *s) {
      static int zer;
      zer = 0;
      ptrie *at =this;
      int c;
      while(*s) {
	 c = conv(*(s++));
	 if(root[c] == NULL)return zer;
	 at = root[c];
      }
      return at->v;
   }
};
#endif
