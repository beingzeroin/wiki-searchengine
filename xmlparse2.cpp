#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sys/types.h>
#include "rapidxml/rapidxml.hpp"
using rapidxml::xml_document;
using rapidxml::xml_node;
int main() {

   int bufsz = 102400;
   char *buf = new char[bufsz+5];
   int s = fread(buf,1,bufsz,stdin);
   buf[s]=0;
   int b = 0;
   int cnt = 0;
   while(true) {
      char* start_off,*close_off;
      start_off = buf+b;
      bool got = false;
      while(*start_off && strncmp(start_off,"<page>",6)!=0)
	 start_off++;
      if(*start_off && strncmp(start_off,"<page>",6)==0) {
	 close_off = start_off +6;
	 while(*close_off && strncmp(close_off,"</page>",7)!=0)
	    close_off++;
	 if(*close_off && strncmp(close_off,"</page>",7)==0) {
	    got = true;
	    close_off+=7;
	 }
      }
      if(got){
	 cnt++;
	 *close_off=0;
	 xml_document<char> doc;
	 doc.parse<0>(start_off);
	 xml_node<char> *root = doc.first_node();
	 xml_node<char> *title,*id,*text;
	 title = root->first_node("title");
	 id = root->first_node("id");
	 text = root->first_node("revision")->first_node("text");
	 printf("%lu %s\n",id->value_size(), id->value());
	 printf("%lu %s\n",title->value_size(), title->value());
	 printf("%lu %s\n",text->value_size(), text->value());
	 b = close_off+1 - buf;
      } else {
	 if(b*2 < bufsz) {
	    bufsz *= 2;
	    fprintf(stderr,"Resizing.. %d\n",bufsz);
	    buf = (char*) realloc(buf,bufsz+5);
	 } 

	 for(int i = b; i < s; i++)
	    buf[i-b] = buf[i];
	 s-=b;
	 b=0;

	 int r = fread(buf+s,1,bufsz-s,stdin);
	 if(r==0)
	    return 0;
	 s += r;
	 buf[s]=0;
      }
   }
}
