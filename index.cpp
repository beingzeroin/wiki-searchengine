//Author Phinfinity
#include<iostream>
#include<cstdio>
#include "rapidxml/rapidxml.hpp"
using namespace std;

int main()
{
   char xml[] = "<test><sdfs>asdasd</sdfs><asf>afgrgs</asf></test>";
   rapidxml::xml_document<char> doc;
   doc.parse<0>(xml);
   return 0;
}
