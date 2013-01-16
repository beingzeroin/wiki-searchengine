#include <cstdio>
#include "tokenize.h"


int bufsz = 102400;
char *buf; 
void resize_buf(int s) {
   while(bufsz < s)
      bufsz*=2;
   delete [] buf;
   buf = new char[bufsz+5];
}
bool read_val() {
   int cnt;
   if(scanf("%d%*c",&cnt)==EOF)
      return false;
   if(cnt>bufsz) 
      resize_buf(cnt);
   int r = fread(buf,1,cnt,stdin);
   buf[r]=0;
   return true;
}
//id expected to have 3-bit data of title/category/outlink
void add_tokens(char *inp,int id) {
   char *s = tokenize(inp);
   while(s) {
      if(*s)
	 printf("%d:%s\n",id,s);
      s = tokenize(NULL);
   }
}

void init() {
}
int main() {
   buf = new char[bufsz+5];
   while(true) {
      if(!read_val())
	 break;
      int id;
      sscanf(buf,"%d",&id);
      printf("Id: %d\n",id);

      read_val();
      printf("Title:\n");add_tokens(buf,id);
      read_val();
      printf("Text:\n");add_tokens(buf,id);
   }
}
