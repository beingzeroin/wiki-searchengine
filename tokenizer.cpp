#include <cstdio>
#include <cstring>
#include <regex.h>
#include "tokenize.h"

#define INFOBOX(x) ((x)|4)
#define OUTLINK(x) ((x)|1)
#define TITLE(x) ((x)|(2))
#define CATEG(x) ((x)|(3))
char DOCTYPE[][20] ={
   "PLAIN",
   "OUTLINK",
   "TITLE",
   "CATEG",
   "INFOBOX",
   "INFOBOXLINK",
   "INVALIDINFTITLE",
   "INVALIDINFCATEG"};
// the types of documents
// infobox is 0
// rest 2:
// 0 - plain
// 1 - outlink
// 2 - title
// 3 - category

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
	 printf("%d:%s:%s\n",id,DOCTYPE[id&7],s);
      s = tokenize(NULL);
   }
}
regex_t infobox,outlink;
regmatch_t pmatch[4];
void handle_text(char* buf, int id) {
   // handle category and outlinks
   char* s = buf;
   int l = 0;
   int categ = 0;
   while(*s) {
      if(*s == '[') {
	 l++;
	 if(l==1 && buf<s) {
	    *s='\0';
	    add_tokens(buf,id);
	    while(buf<s)
	       *(buf++)=' ';
	    *buf=' ';
	    //now buf = s
	 }
	 if(strncasecmp(s+1,"[category:",10)==0) {
	    categ=1;
	    while(*s!=':')
	       *(s++)=' ';
	 }
      }
      else if(*s == ']') {
	 l--;
	 if(l==0) {
	    *s='\0';
	    if(categ) {
	       add_tokens(buf,CATEG(id));
	    categ=0;
	    } else {
	       add_tokens(buf,OUTLINK(id));
	    }
	    while(buf<s)
	       *(buf++)=' ';
	    *buf=' ';
	 }
      }
      s++;
   }
   add_tokens(buf,id);
   while(buf<s)
      *(buf++)=' ';
}
void handle_infobox(char* buf,int id) {
   size_t s = 0;
   while(true) {
      int status = regexec(&infobox,buf+s,1,pmatch,0);
      if(status)
	 break;
      s+=pmatch[0].rm_so;
      int c = 2;
      char *p = buf+s+2;
      while(c!=0) {
	 if(*p=='{')c++;
	 if(*p=='}')c--;
	 p++;
      }
      *(--p)=' ';
      *(--p)='\0';
      //hadndle text wipes it clean
      handle_text(buf+s,id);
      *p = ' ';
   }
}
void init() {
   enable_stop_words();
   regcomp(&infobox,"{{Infobox",0);
}
int main() {
   init();
   buf = new char[bufsz+5];
   while(true) {
      if(!read_val())
	 break;
      int id;
      sscanf(buf,"%d",&id);
      printf("Id: %d\n",id);

      read_val();
      printf("Title:\n");add_tokens(buf,TITLE(id*8));
      read_val();
      handle_infobox(buf,INFOBOX(id*8));
      printf("Text:\n");
      handle_text(buf,id*8);
//      printf("%s\n",buf);
      //add_tokens(buf,id);
   }
}
