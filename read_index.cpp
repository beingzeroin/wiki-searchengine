//Author Phinfinity
#define _FILE_OFFSET_BITS 64
#include <cstdio>
#include <cstring>
#include <vector>
#include <string>
#include <algorithm>
#include "varbyteencoder.h"
#include "tokenize.h"
using varbyteencoder::decode;
using namespace std;
char buf[100];
vector<pair<string,off_t> > dict;

int main(int argc, char**argv) {
   if(argc!=2) {
      fprintf(stderr, "Usage : %s index_file_name\n",argv[0]);
      return 2;
   }
   FILE *f = fopen(argv[1],"rb");

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



   printf("Enter words to query :\n");
   while(scanf("%s",buf)!=EOF) {
      char *red_token = reduce_token(buf);
      bool valid_word=false;
      if(red_token != 0) 
      {
	 pair<string,off_t> p;
	 p.first = red_token;
	 p.second = 0;
	 auto pt = lower_bound(dict.begin(),dict.end(),p);
	 if(pt != dict.end() && p.first == pt->first) {
	    valid_word = true;
	    p = *pt;
	    printf("Found string : %s\n",p.first.c_str());
	    printf("Offset : %ld\n",p.second);
	    fseeko(f,p.second,SEEK_SET);
	    vector<pair<int,int> > v;
	    fscanf(f,"%*s%*c");
	    decode(f,v);
	    printf("%lu Documents with word\n",v.size());
	    for(auto it = v.begin(); it != v.end(); it++)
	       swap(it->first, it->second);
	    sort(v.begin(), v.end());
	    int c= 0;
	    for(auto it = v.rbegin(); it != v.rend(); it++) {
	       c++;
	       printf("freq:%d doc: %d (%d)\n",it->first,it->second/8, (it->second)%8);
	       if(c>=15)break;
	    }
	 }
      }
      if(!valid_word) {
	 printf("Word discarded...\n");
      }

   }
}
