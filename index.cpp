//Author Phinfinity
#include<iostream>
#include<cstdio>
#include "rapidxml/rapidxml.hpp"
using namespace std;
using namespace rapidxml;
void pc(int c) {
   while(c--)
      printf("*");
}
void dfs(xml_node<char> *n,int c = 1) {
   pc(c);
   printf("%s\n",n->name());
   if(n->first_node()==0) {
      pc(c+1);
      printf("#%s\n",n->value());
   }
   n = n->first_node();
   while(n) {
      dfs(n,c+1);
      n=n->next_sibling();
   }
}
int main()
{
   char xml[] = "<test><sdfs>asdasd</sdfs><asf>afgrgs</asf></test>";
   xml_document<char> doc;
   doc.parse<0>(xml);
   dfs(doc.first_node());
   return 0;
}
