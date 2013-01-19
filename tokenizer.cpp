// Author Phinfinity
#include <cstdio>
#include <map>
#include <cstring>
#include <algorithm>
#include <vector>
#include <string>
using namespace std;
#include <regex.h>
#include "tokenize.h"
#include "trie.h"
#include "varbyteencoder.h"
using varbyteencoder::encode;

#define CHUNK_FILE_LIM 50000

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

typedef unsigned long long ull;
//map<string,ull> dict2;
ptrie_set *dict=NULL;
int no_words = 0;
vector<string> words;
vector<int> tok_freq;
vector<vector<pair<int,int> > > inv_index;
void dump_all(const char *fname) {
   vector<pair<string,int> > wlist;
   //fprintf(stderr,"Sorting tokens...\n");
   for(size_t i = 0; i < words.size(); i++)
      wlist.push_back(make_pair(string(words[i]),i));
   sort(wlist.begin(),wlist.end());
   FILE *f = fopen(fname,"wb");
   //fprintf(stderr,"Writing to file...\n");
   for(size_t i = 0; i < wlist.size(); i++) {
      fprintf(f,"%s ",wlist[i].first.c_str());
      
      encode(f,inv_index[wlist[i].second]);
      /* USING VARBYTE ENCODING INSTEAD
      int s = inv_index[wlist[i].second].size();
      fwrite(&s,sizeof(s),1,f);
      auto end_it = inv_index[wlist[i].second].end();
      for(auto it = inv_index[wlist[i].second].begin(); it!=end_it; it++) {
	 int doc_id = it->first;
	 int freq = it->second;
	 fwrite(&doc_id,sizeof(int),1,f);
	 fwrite(&freq,sizeof(int),1,f);
      }
      */
   }
   //fprintf(stderr,"Clearing Memory...\n");
   words.clear();
   tok_freq.clear();
   inv_index.clear();
   delete dict;
   dict = new ptrie_set();
   no_words = 0;
}

int bufsz = 102400;
char *buf; 
void resize_buf(int s) {
   while(bufsz < s)
      bufsz*=2;
   delete [] buf;
   buf = new char[bufsz+5];
   //fprintf(stderr,"NOTE: Resized to %d\n",bufsz);
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
   int sz;
   char *s = tokenize(inp,sz);
   while(s) {
      if(*s)
      {
	 //dict2[string(s)]++;
	 int tok_id = dict->el(s);
	 if(tok_id>no_words) {
	    no_words++;
	    words.push_back(string(s));
	    tok_freq.push_back(0);
	    inv_index.resize(no_words);
	 }
	 tok_id--;
	 tok_freq[tok_id]++;
	 int c = 6;
	 bool done = false;
	 for(auto it = inv_index[tok_id].rbegin(); c>0 && it != inv_index[tok_id].rend(); it++) {
	    c--;
	    if(it->first==id)
	    {
	       it->second++;
	       done = true;
	    }
	 }
	 if(!done) {
	    inv_index[tok_id].push_back(make_pair(id,1));
	 }
	 //ull *v = dict.find(s,sz);
	 /*
	 if(v==NULL) {
	    v = new ull(0);
	    dict.add(s,sz,v);
	    no_words++;
	    if(no_words%10000==0)
	       printf("%llu words so far\n",no_words);
	    words.push_back(string(s));
	 }
	 (*v)++;
	 */
	 //printf("%d:%s:%s\n",id,DOCTYPE[id&7],s);
      }
      s = tokenize(NULL,sz);
      
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
   int status = regexec(&infobox,buf+s,1,pmatch,0);
   if(!status){
      s+=pmatch[0].rm_so;
      int c = 2;
      char *p = buf+s+2;
      int nl = 0;
      while(c!=0 && *p) {
	 if(*p=='{')c++;
	 if(*p=='}')c--;
	 if(*p=='\n')nl=1;
	 if(nl && *p=='|')nl=0;
	 if(nl && !isspace(*p)) {
	    p--;
	    break; //break if infobox doesn't start with '|'
	 }

	 p++;
      }
      if(*p==0){
	 printf("Infobox for %d is screwed...\n",id);
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
   int cnt = 0;
   dict = new ptrie_set();
   int file_id = 0;
   while(true) {
      if(!read_val())
	 break;
      int id;
      sscanf(buf,"%d",&id);

      read_val();
      add_tokens(buf,TITLE(id*8));

      read_val();
      handle_infobox(buf,INFOBOX(id*8));
      handle_text(buf,id*8);
      cnt++;
      if(cnt%CHUNK_FILE_LIM==0) {
	 char fname[100];
	 sprintf(fname,"tempfile-%d",file_id++);
	 fprintf(stderr,"Dumping partial index to file %s\r",fname);
	 dump_all(fname);
      }
      /*
      if(cnt%10000==0)
	 fprintf(stderr,"%d Documents Parsed %d unique words\n",cnt,no_words);
	 */
   }
   if(cnt%CHUNK_FILE_LIM!=0) {
      char fname[100];
      sprintf(fname,"tempfile-%d",file_id++);
      fprintf(stderr,"Dumping partial index to file %s\r",fname);
      dump_all(fname);
   }
}
