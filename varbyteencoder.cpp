//Author Phinfinity
#include "varbyteencoder.h"
#include <algorithm>
using std::sort;
#include <cstdlib>

namespace varbyteencoder{
   unsigned char *buf = NULL;
   int bufsz = 0;

   void encode(FILE *f,vector<pair<int,int> > v) {
      sort(v.begin(),v.end());
      int prev = 0;
      for(auto it = v.begin(); it != v.end(); it++) {
	 it->first-=prev;
	 prev += it->first;
      }
      if(buf==NULL) {
	 bufsz = 102400;
	 buf = new unsigned char[bufsz];
      }
      int s=0;
      for(auto it: v) {
	 if(s+18>=bufsz) {
	    bufsz*=2;
	    buf = (unsigned char*)realloc(buf,bufsz);
	 }
	 int c;
	 c = it.first+1;
	 while(c) {
	    buf[s++] = ((c%128)<<1)|((c>>7)?1:0);
	    c>>=7;
	 }
	 c = it.second+1;
	 while(c) {
	    buf[s++] = ((c%128)<<1)|((c>>7)?1:0);
	    c>>=7;
	 }
      }
      fwrite(&s,1,sizeof(int),f);
      fwrite(buf,s,1,f);
   }

   void decode(FILE *f, vector<pair<int,int>> &v) {
      int c;
      fread(&c,sizeof(int),1,f);
      if(c >= bufsz) {
	 while(c >= bufsz) bufsz*=2;
	 buf = (unsigned char*)realloc(buf,bufsz);
      }
      fread(buf,1,c,f);
      int s = 0;
      int d;
      int pval = 0;
      while(s<c) {
	 pair<int,int> p;
	 int intval;
	 intval=0;
	 d=1;
	 while(true) {
	    intval+=d*(buf[s]>>1);
	    if(buf[s++]&1)
	       d<<=7;
	    else break;
	 }
	 p.first = intval-1;

	 if(s>=c)
	    break;

	 intval=0;
	 d=1;
	 while(true) {
	    intval+=d*(buf[s]>>1);
	    if(buf[s++]&1)
	       d<<=7;
	    else break;
	 }
	 p.second = intval-1;
	 p.first += pval;
	 pval = p.first;
	 v.push_back(p);
      }
   }
}
