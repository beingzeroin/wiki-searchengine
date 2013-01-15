#include <cstdio>
#include <cstdlib>
#include <cstring>

void error(const char* s) {
   fprintf(stderr,"Error! : %s\n",s);
   exit(2);
}

int main () {
   int bigbufsize = 102400;
   char *bigbuf = new char[bigbufsize];
   int s = fread(bigbuf,1,bigbufsize,stdin);
   int page_beg;
   int beg;
   beg=0;
   while(s) {
      if(beg!=0)
	 for(int i = beg; i < s; i++)
	    bigbuf[i-beg] = bigbuf[i];
      s-=beg;
      beg=0;
      for(beg=0;beg<s;beg++)
	 if(strncmp(bigbuf+beg,"<page>",6)==0)
	    break;
      page_beg=beg;
      // Now a <page> begins. find </page>
      for(;beg<s-6;beg++) {
	 if(bigbuf[beg]=='<' && strncmp(bigbuf+beg,"</page>",7)==0) {
	    beg+=7;
	    bigbuf[beg]=0;
	    printf("%s\n###############\n",bigbuf+page_beg);
	    beg++;
	    break;
	 }
	 if(beg==s-7) {
	    if(s*2>=bigbufsize) {
	       bigbufsize*=2;
	       bigbuf = (char*) realloc(bigbuf,bigbufsize);
	    }
	    int r = fread(bigbuf+s,1,bigbufsize-s,stdin);
	    if(r==0)
	       return 0;
	    s+=r;
	 }
      }
   }

   free(bigbuf);
   return 0;
}
