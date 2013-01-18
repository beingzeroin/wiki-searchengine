//Author Phinfinity
#ifndef VAR_BYTE_ENCODER_H
#define VAR_BYTE_ENCODER_H
#include <vector>
#include <utility>
#include <cstdio>
using std::vector;
using std::pair;
namespace varbyteencoder{
   int encode(FILE *f,const vector<pair<int,int> > v);
   int decode(FILE *f,vector<pair<int,int> > &v) ;
}
#endif
