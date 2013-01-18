#include "varbyteencoder.h"
#include <cstdio>
using namespace std;

int main() {
   vector<pair<int,int> > v;
   for(int i = 0; i < 28; i++)
      v.push_back(make_pair((4<<i),(5<<i)));
   for(int i = 0; i < 100; i++)
      v.push_back(make_pair(i,2*i));
   FILE *f = fopen("/tmp/in","wb");
   encode(f,v);
   fclose(f);
   v.clear();
   f = fopen("/tmp/in","rb");
   decode(f,v);
   for(auto it : v)
      printf("%d %d\n",it.first,it.second);
}
