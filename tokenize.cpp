// Author Phinfinity
#include "tokenize.h"
#include <cstdio>
#include "stem.h"
#include <cstring>
#include <ctype.h>
#include "trie.h"

#define MAX_TOKEN_SIZE 20
//truncate tokens to first 20 characters

ptrie_set trie;


void symbol_strip(char *s) {
   char *putp = s;
   int c = 0;
   while(*s) {
      if(isalnum(*s)) {
	 *(putp++) = tolower(*s);
	 if(++c>=MAX_TOKEN_SIZE) {
	    *putp = '\0';
	    return;
	 }
      }
      s++;
   }
   *putp='\0';
   /*
   char *l = s;
   while(*l != 0) {
      *l = tolower(*l);
      if(l==s && !isalnum(*l))
	 s++;
     l++;
   }
   l--;
   while(l>=s && !isalnum(*l)) {
      l--;
   }
   l++;
   *l=0;
   return s;
   */
}

void enable_stop_words() {
   char stop_words[][20] = {"a","about","above","after","again","against","all","am","an","and","any","are","aren't","as","at","be","because","been","before","being","below","between","both","but","by","can't","cannot","could","couldn't","did","didn't","do","does","doesn't","doing","don't","down","during","each","few","for","from","further","had","hadn't","has","hasn't","have","haven't","having","he","he'd","he'll","he's","her","here","here's","hers","herself","him","himself","his","how","how's","i","i'd","i'll","i'm","i've","if","in","into","is","isn't","it","it's","its","itself","let's","me","more","most","mustn't","my","myself","no","nor","not","of","off","on","once","only","or","other","ought","our","ours ","ourselves","out","over","own","same","shan't","she","she'd","she'll","she's","should","shouldn't","so","some","such","than","that","that's","the","their","theirs","them","themselves","then","there","there's","these","they","they'd","they'll","they're","they've","this","those","through","to","too","under","until","up","very","was","wasn't","we","we'd","we'll","we're","we've","were","weren't","what","what's","when","when's","where","where's","which","while","who","who's","whom","why","why's","with","won't","would","wouldn't","you","you'd","you'll","you're","you've","your","yours","yourself","yourselves"};
   for(int i=0;i<174;i++) {
      stop_words[i][stem(stop_words[i],0,strlen(stop_words[i])-1)+1]=0;
      symbol_strip(stop_words[i]);
      trie.el(stop_words[i]);
   }
}

char* reduce_token(char *s) {
   s[stem(s,0,strlen(s)-1)+1]=0;
   symbol_strip(s);
   if(trie.has(s))
      return 0;
   return s;
}

char *tokenize(char *str,int &sz) {
   static char *saveptr;
   char *ret = strtok_r(str," \t\v\n:;|{}[]()<>`~!@#$%^&*-+=\\/\"_",&saveptr);
   if(ret) {
      symbol_strip(ret);
      ret[stem(ret,0,strlen(ret)-1)+1]=0;
      //symbol_strip(ret);
      if(trie.has(ret))
	 *ret=0;
   }
   return ret;
}
