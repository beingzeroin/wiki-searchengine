#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sys/types.h>
#include <regex.h>
int main() {

   regex_t page,pageclose;
   regmatch_t match1,match2;
   regcomp(&page,"<page>",0);
   regcomp(&pageclose,"</page>",0);

   int bufsz = 102400;
   char *buf = new char[bufsz];
   int s = fread(buf,1,bufsz,stdin);
   buf[s]=0;
   int b = 0;
   while(true) {
      if(!regexec(&pageclose,buf+b,1,&match1,0)) {
	 regexec(&page,buf+b,1,&match2,0);
	 buf[b+match1.rm_eo]=0;
	 printf("%s\n########\n",buf+b+match2.rm_so);
	 b += match1.rm_eo+1;
      } else {
	 if(b*2 < bufsz) {
	    bufsz *= 2;
	    buf = (char*) realloc(buf,bufsz);
	 } 

	 for(int i = b; i < s; i++)
	    buf[i-b] = buf[i];
	 s-=b;
	 b=0;

	 int r = fread(buf+s,1,bufsz-s,stdin);
	 if(r==0)
	    return 0;
	 s = r;
	 buf[s]=0;
      }
   }
}
