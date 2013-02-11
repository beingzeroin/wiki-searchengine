//Author Phinfinity
#define _FILE_OFFSET_BITS 64
#include <cstdio>
#include <cmath>
#include <cstring>
#include <ctime>
#include <vector>
#include <string>
#include <algorithm>
#include <unordered_map>
#include "varbyteencoder.h"
#include "tokenize.h"
using varbyteencoder::decode;
using namespace std;

#define MAX_TERMS 15
#define INFOBOX(x) ((x&4)>0)
#define PLAIN(x) ((x&3)==0)
#define OUTLINK(x) ((x&3)==1)
#define TITLE(x) ((x&3)==2)
#define CATEG(x) ((x&3)==3)

#define MULTI_MATCH_BOOST 5
#define MIN_WORD_COUNT 0 // To prevent short document normalization issues

char buf[100];
vector<pair<string,off_t> > dict;
vector<pair<int,pair<int,string> > > document_list;
int doc_cnt;
// docid,(wc,title)

char progress[] = "|/-\\";
int pcnt = 0;
FILE *f;

int get_doc_wc(int doc_id) {
   pair<int,pair<int,string> > p;
   p.first = doc_id;
   auto it = lower_bound(document_list.begin(),document_list.end(), p);
   return max(MIN_WORD_COUNT,it->second.first);
}
string get_doc_title(int doc_id) {
   pair<int,pair<int,string> > p;
   p.first = doc_id;
   auto it = lower_bound(document_list.begin(),document_list.end(), p);
   return it->second.second;
}

// filter just checks and of last 3 bits matches or not
// if filter is -1 , then uses takes everything
vector<pair<int,double> > process_token(char *token,int filter) {
   vector<pair<int,double> > ret;
   char *red_token = reduce_token(token);
   //bool valid_word=false;
   if(red_token != 0) 
   {
      pair<string,off_t> p;
      p.first = red_token;
      p.second = 0;
      auto pt = lower_bound(dict.begin(),dict.end(),p);
      if(pt != dict.end() && p.first == pt->first) {
	 //valid_word = true;
	 p = *pt;
	 //printf("Found string : %s\n",p.first.c_str());
	 //printf("Offset : %ld\n",p.second);
	 fseeko(f,p.second,SEEK_SET);
	 vector<pair<int,int> > v;
	 fscanf(f,"%*s%*c");
	 decode(f,v);
	 vector<pair<int,int> > filtered_freq;
	 for(auto it = v.begin(); it != v.end(); it++) {
	    //it : docid,freq
	    int doc_id = it->first;
	    int freq = it->second;
	    if(filter!=-1 && (doc_id&7)!=filter)
	       continue;
	    doc_id>>=3;
	    if(filtered_freq.size()>0 && filtered_freq.back().first==doc_id) {
	       filtered_freq.back().second += freq;
	    } else {
	       filtered_freq.push_back(make_pair(doc_id,freq));
	    }
	 }
	 int elcnt = filtered_freq.size();
	 double idf = log(doc_cnt/double(elcnt));
	 for(auto it : filtered_freq) { 
	    pair<int,double> p;
	    p.first = it.first;
	    p.second = idf*double(it.second)/get_doc_wc(p.first);
	    ret.push_back(p);
	 }
      }
   }
   return ret;
}
vector<pair<string,int> > process_input() {
   char buf[10240];
   char buf2[10240];
   vector<pair<string,int> > ret;
   if(scanf(" %[^\n]",buf)==EOF) {
      ret.push_back(make_pair("EOF",-2));
   }
   int putp=0;
   char *p = buf;
   while(*p) {
      if(*p==':') {
	 buf2[putp++] = ' ';
	 buf2[putp++] = *p;
	 buf2[putp++] = ' ';
      } else {
	 buf2[putp++] = *p;
      }
      p++;
   }
   buf2[putp] = 0;
   vector<string> toks;
   for(char* tok = strtok(buf2," \t"); tok != NULL; tok = strtok(NULL," \t")) {
      toks.push_back(string(tok));
   }
   int mode = -1;
   for(size_t i = 0; i < toks.size(); i++) {
      bool modifier = false;
      if(i+1 < toks.size()) {
	 if(toks[i+1] == ":") {
	    modifier = true;
	   if(toks[i] == "T")
	      mode = 2;
	   else if(toks[i] == "B")
	      mode = 0;
	   else if(toks[i] == "I")
	      mode = 4;
	   else if(toks[i] == "C")
	      mode = 3;
	   else if(toks[i] == "O")
	      mode = 1;
	   else {
	      modifier = false;
	   }
	 }
      }
      if(modifier)
	 i++;
      else
	 ret.push_back(make_pair(toks[i],mode));
   }
   return ret;
}
double get_time(const timespec &a){
   return a.tv_sec+double(a.tv_nsec)/1e9;
}

int main(int argc, char**argv) {
   if(argc!=3) {
      fprintf(stderr, "Usage : %s index_file_name docindex.dat\n",argv[0]);
      return 2;
   }
   f = fopen(argv[1],"rb");

   off_t dict_seg_list_offset = 0,file_size;
   fseek(f,-sizeof(dict_seg_list_offset),SEEK_END);
   fread(&dict_seg_list_offset,sizeof(dict_seg_list_offset),1,f);
   file_size = ftello(f);
   printf("File Size : %.3f MB\n",float(file_size)/(1024*1024));
   fseeko(f,dict_seg_list_offset,SEEK_SET);
   vector<off_t> dict_seg_list;
   while(ftello(f)+8 < file_size) {
      off_t dic_seg_offset;
      fread(&dic_seg_offset,sizeof(off_t),1,f);
      dict_seg_list.push_back(dic_seg_offset);
   }
   int read_size = 0;
   fprintf(stderr,"  Loading TokenDictionary...");
   for(size_t d_seg = 0; d_seg < dict_seg_list.size(); d_seg++) {
      fseek(f,dict_seg_list[d_seg],SEEK_SET);
      fscanf(f,"%*s%*c");
      int c;
      fread(&c,sizeof(int),1,f);

      int l = 0;
      while(c--) {
	 fscanf(f,"%s%*c",buf);
	 off_t offset;
	 fread(&offset,sizeof(offset),1,f);
	 dict.push_back(make_pair(string(buf),offset));
	 l += 9+strlen(buf);

	 if(l >= 8*1024*1024) {
	    read_size+=l;
	    l = 0;
	    fprintf(stderr,"\rRead %d MB",read_size/(1024*1024));
	 }
      }
   }
   fprintf(stderr,"\r");
   printf("%ld Words in Dictionary!\n",dict.size());

   // Load documents
   FILE *doc_index = fopen(argv[2],"r");
   fprintf(stderr,"  Loading Document titles...");
   while(true) {
      int doc_id,wc;
      char buf[1000];
      if(fread(&doc_id, sizeof(int), 1, doc_index)==0)
	 break;
      fread(&wc, sizeof(int), 1, doc_index);
      int p = 0;
      while(true) {
	 buf[p] = fgetc(doc_index);
	 if(buf[p]==0)
	    break;
	 p++;
      }
      document_list.push_back(make_pair(doc_id,make_pair(wc,string(buf))));
      if(document_list.size()%1000000==0) {
	 pcnt = (pcnt+1)%4;
	 fprintf(stderr,"\r%c",progress[pcnt]);
      }
   }
   fclose(doc_index);
   sort(document_list.begin(),document_list.end());
   doc_cnt = document_list.size();
   fprintf(stderr,"\r");
   printf("%ld Documents in index\n",document_list.size());



   while(true) {
      printf("Enter query :\n");
      auto tokens = process_input();
      timespec start_time,end_time;
      clock_gettime(CLOCK_MONOTONIC,&start_time);
      if(tokens.size()>0 && tokens[0].second == -2)
	 break;
      unordered_map<int,double> ranklist;
      for(auto token : tokens) {
	 char buf3[100];
	 strcpy(buf3,token.first.c_str());
	 auto tfidf = process_token(buf3,token.second);
	 vector<pair<double,int> > v;
	 for(auto it: tfidf)
	    ranklist[it.first] += MULTI_MATCH_BOOST+it.second; // constant extra score of 1 for boosting multiple term match
      }
      vector<pair<double,int> > final_ranklist;
      for (auto it : ranklist)
	 final_ranklist.push_back(make_pair(it.second,it.first));
      sort(final_ranklist.begin(),final_ranklist.end());
      int c= 0;
      clock_gettime(CLOCK_MONOTONIC,&end_time);
      for(int i = final_ranklist.size() - 1; i >=0 && c < MAX_TERMS ; i--,c++) {
	 printf("%d. %s\nhttp://en.wikipedia.org/wiki?curid=%d\n",c+1,get_doc_title(final_ranklist[i].second).c_str(),final_ranklist[i].second);
	 //printf("%d. %.4lf  %s\nhttp://en.wikipedia.org/wiki?curid=%d\n",c+1,final_ranklist[i].first,get_doc_title(final_ranklist[i].second).c_str(),final_ranklist[i].second);
      }
      printf("Query took %lf seconds\n",get_time(end_time) - get_time(start_time));
   }
}
