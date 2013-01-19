//Author Phinfinity
#define _FILE_OFFSET_BITS 64
#include <cstdio>
#include <cstring>
#include <vector>
#include <string>
#include <algorithm>
#include "varbyteencoder.h"
using varbyteencoder::decode;
using namespace std;
int main(int argc, char**argv) {
   fprintf(stderr,"Warning this tool is part of the querying engine, This tool is under construction and has several bugs. Do not Evaluate\n");
   if(argc!=2) {
      fprintf(stderr, "Usage : %s index_file_name\n",argv[0]);
      return 2;
   }
   FILE *f = fopen(argv[1],"rb");

   off_t dict_offset = 0,file_size;
   fseek(f,-sizeof(dict_offset),SEEK_END);
   file_size = ftello(f);
   printf("File Size : %.3f MB\n",float(file_size)/(1024*1024));
   fread(&dict_offset,sizeof(dict_offset),1,f);
   printf("Dictionary Size : %.3f MB\n",float(file_size - dict_offset)/(1024*1024)); 
   fseeko(f,dict_offset,SEEK_SET);
   char buf[100];
   vector<pair<string,off_t> > dict;
   int read_size = 0;
   int l = 0;
   while(ftello(f)+8 < file_size) {
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
   fprintf(stderr,"\r");
   printf("%ld Words in Dictionary!",dict.size());
   printf("Enter words to query :\n");
   while(scanf("%s",buf)!=EOF) {
      pair<string,off_t> p;
      p.first = buf;
      p.second = 0;
      p = *lower_bound(dict.begin(),dict.end(),p);
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
