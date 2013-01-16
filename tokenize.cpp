#include "tokenize.h"
#include "stem.h"
#include <cstring>
#include <ctype.h>

char *symbol_strip(char *s) {
   char *l = s;
   while(*l != 0) {
      *l = tolower(*l);
      if(l==s && !isalnum(*l) && *l != '.' && *l!='$' && *l!='+' && *l!='-')
	 s++;
      l++;
   }
   l--;
   while(l>=s && !isalnum(*l) && *l != '+' && *l != '$' && *l != '%' && *l != '\'') {
      l--;
   }
   l++;
   *l=0;
   return s;
}
char *tokenize(char *str) {
   static char *saveptr;
   char *ret = strtok_r(str," \t\v\n:|",&saveptr);
   if(ret)
      ret[stem(ret,0,strlen(ret)-1)+1]=0;
   return ret;
}
