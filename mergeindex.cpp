//Author Phinfinity

#include <glob.h>
#include <cstdio>
#include <cstring>
#include <cassert>
#include <ctime>
#include <string>
#include <vector>
#include <queue>
using namespace std;

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
   int c;
   assert(fread(&c,sizeof(int),1,tempf[i]));
   inread+=c*4*2+4;
   while(c--) {
      int doc,freq;
      assert(fread(&doc,sizeof(int),1,tempf[i]));
      assert(fread(&freq,sizeof(int),1,tempf[i]));
      current_element[i].push_back(make_pair(doc,freq));
   }
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
   clock_t ltime = clock();
   clock_t start_time = ltime;
   long lread = 0;
   while(!pq.empty()) {
      string s = pq.top().first;
      wcnt++;
      wlsum += s.size();
      if(wcnt%100000==0){
	 clock_t etime = clock();
	 float rate = float(inread-lread)/(1024*1024);
	 rate /= float(etime-ltime)/CLOCKS_PER_SEC;
	 ltime = etime;
	 lread = inread;
	 int dur = (etime-start_time)/CLOCKS_PER_SEC;
	 fprintf(stderr,"%d words output, %lu dict size\n",wcnt,wlsum);
	 fprintf(stderr,"%02d:%02d elapsed, %f read , [%.2f MB/s]\n",dur/60,dur%60,float(inread)/(1024*1024),rate);
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
      int c = cur_index.size();
      fwrite(&c, sizeof(int), 1, f);
      for(auto it : cur_index) {
	 fwrite(&it.first, sizeof(int), 1, f);
	 fwrite(&it.second, sizeof(int), 1, f);
      }
   }
   fclose(f);
   for(int i = 0; i < n; i++)
      fclose(tempf[i]);
   return 0;
}
