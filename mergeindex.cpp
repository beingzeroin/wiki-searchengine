//Author Phinfinity

#define _FILE_OFFSET_BITS 64
#include <glob.h>
#include <cstdio>
#include <cstring>
#include <cassert>
#include <ctime>
#include <string>
#include <vector>
#include <queue>
#include "varbyteencoder.h"
using varbyteencoder::encode;
using varbyteencoder::decode;
using namespace std;

vector<pair<string,off_t> > dictionary;
double get_time(const timespec &a) {
   return a.tv_sec+double(a.tv_nsec)/1e9;
}
FILE *f;
FILE **tempf;
vector<vector<pair<int,int> > > current_element;
long inread = 0;
//current_element[i][j] is the (docid,freq) pair for the jth document of the current word in file i
string read_element(int i) {
   current_element[i].clear();
   char buf[50];
   assert(strlen(buf)<=22);
   inread+=strlen(buf)+1;
   if(fscanf(tempf[i],"%s%*c",buf)==EOF)
      return "";
   inread += decode(tempf[i],current_element[i]);
   /* USING VARBYTE DECODING
   int c;
   assert(fread(&c,sizeof(int),1,tempf[i]));
   inread+=c*4*2+4;
   while(c--) {
      int doc,freq;
      assert(fread(&doc,sizeof(int),1,tempf[i]));
      assert(fread(&freq,sizeof(int),1,tempf[i]));
      current_element[i].push_back(make_pair(doc,freq));
   }
   */
   return string(buf);
}
vector<string> filenames;
int n;
void get_file_names() {
   glob_t globbuf;
   glob("tempfile-*",0,0,&globbuf);
   for(size_t i = 0; i < globbuf.gl_pathc; i++) {
      filenames.push_back(string(globbuf.gl_pathv[i]));
   }
   n = filenames.size();
   fprintf(stderr,"%lu Files for merging to Index\n",filenames.size());
}
int main() {
   get_file_names();
   f = fopen("searchindex.dat","wb");
   tempf = new FILE*[n];
   for(int i = 0; i < n ; i++)
      tempf[i] = fopen(filenames[i].c_str(),"r");
   current_element.resize(n);
   typedef pair<string,int> pq_t;
   priority_queue<pq_t,vector<pq_t>,greater<pq_t> > pq;
   for(int i = 0; i < n ; i++) {
      string s = read_element(i);
      pq.push(make_pair(s,i));
   }
   int wcnt = 0;
   long wlsum = 0;
   timespec ltime,start_time;
   clock_gettime(CLOCK_MONOTONIC,&ltime);
   clock_gettime(CLOCK_MONOTONIC,&start_time);
   long lread = 0;
   while(!pq.empty()) {
      string s = pq.top().first;
      dictionary.push_back(make_pair(s,ftello(f)));
      wcnt++;
      wlsum += s.size();
      if(wcnt%100000==0){
	 timespec etime;
	 clock_gettime(CLOCK_MONOTONIC,&etime);
	 float dur = get_time(etime) - get_time(ltime);
	 float rate = float(inread-lread)/(1024*1024);
	 rate/=dur;
	 dur = get_time(etime) - get_time(start_time);
	 ltime = etime;
	 lread = inread;
	 fprintf(stderr,"%d words output, %lu dict size\n",wcnt,wlsum);
	 fprintf(stderr,"%02d:%02d elapsed, %f read , [%.2f MB/s]\n",int(dur)/60,int(dur)%60,float(inread)/(1024*1024),rate);
      }
      vector<pair<int,int> > cur_index;
      while(!pq.empty() && pq.top().first==s) {
	 int id = pq.top().second;
	 cur_index.insert(cur_index.end(),current_element[id].begin(),current_element[id].end());
	 pq.pop();
	 string ns = read_element(id);
	 if(ns!="")
	    pq.push(make_pair(ns,id));
	 else {
	    fprintf(stderr,"tempfile-%d is done!\n",id);
	 }
      }
      fprintf(f,"%s ",s.c_str());
      encode(f,cur_index);
      /* USING VARBYTE ENCODER
      fwrite(&c, sizeof(int), 1, f);
      for(auto it : cur_index) {
	 fwrite(&it.first, sizeof(int), 1, f);
	 fwrite(&it.second, sizeof(int), 1, f);
      }
      */
   }
   for(int i = 0; i < n; i++)
      fclose(tempf[i]);
   off_t dict_offset = ftello(f);
   for(auto it : dictionary) {
      fprintf(f,"%s ",it.first.c_str());
      fwrite(&it.second,sizeof(off_t),1,f);
   }
   fwrite(&dict_offset,sizeof(off_t),1,f);
   fclose(f);
   return 0;
}
