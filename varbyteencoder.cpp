//Author Phinfinity
#include "varbyteencoder.h"
#include <cstdlib>

unsigned char *buf = NULL;
int bufsz = 0;

void encode(FILE *f,const vector<pair<int,int> > v) {
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
      c = it.first;
      while(c) {
	 buf[s++] = ((c%128)<<1)|((c>>7)?1:0);
	 c>>=7;
      }
      c = it.second;
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
   fread(&c,1,sizeof(int),f);
   if(c >= bufsz) {
      while(c >= bufsz) bufsz*=2;
      buf = (unsigned char*)realloc(buf,bufsz);
   }
   fread(buf,c,1,f);
   int s = 0;
   int d;
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
      p.first = intval;

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
      p.second = intval;
      v.push_back(p);
   }
}
