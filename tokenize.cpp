#include "tokenize.h"
#include <cstdio>
#include "stem.h"
#include <cstring>
#include <ctype.h>
#include "fasttrie.h"

jdk_fasttree<int,128,char> trie;

void enable_stop_words() {
   char stop_words[][20] = {"a","about","above","after","again","against","all","am","an","and","any","are","aren't","as","at","be","because","been","before","being","below","between","both","but","by","can't","cannot","could","couldn't","did","didn't","do","does","doesn't","doing","don't","down","during","each","few","for","from","further","had","hadn't","has","hasn't","have","haven't","having","he","he'd","he'll","he's","her","here","here's","hers","herself","him","himself","his","how","how's","i","i'd","i'll","i'm","i've","if","in","into","is","isn't","it","it's","its","itself","let's","me","more","most","mustn't","my","myself","no","nor","not","of","off","on","once","only","or","other","ought","our","ours ","ourselves","out","over","own","same","shan't","she","she'd","she'll","she's","should","shouldn't","so","some","such","than","that","that's","the","their","theirs","them","themselves","then","there","there's","these","they","they'd","they'll","they're","they've","this","those","through","to","too","under","until","up","very","was","wasn't","we","we'd","we'll","we're","we've","were","weren't","what","what's","when","when's","where","where's","which","while","who","who's","whom","why","why's","with","won't","would","wouldn't","you","you'd","you'll","you're","you've","your","yours","yourself","yourselves"};
   for(int i=0;i<174;i++) {
      stop_words[i][stem(stop_words[i],0,strlen(stop_words[i])-1)+1]=0;
      trie.add(stop_words[i],strlen(stop_words[i]),1);
   }
}

char *symbol_strip(char *s) {
   char *start = s;
   char *putp = s;
   while(*s) {
      if(isalnum(*s))
	 *(putp++) = tolower(*s);
      s++;
   }
   *putp='\0';
   return start; 
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
char *tokenize(char *str) {
   static char *saveptr;
   char *ret = strtok_r(str," \t\v\n:|{}[]<>&@=;/\"",&saveptr);
   if(ret) {
      ret = symbol_strip(ret);
      ret[stem(ret,0,strlen(ret)-1)+1]=0;
      ret = symbol_strip(ret);
      if(trie.find(ret,strlen(ret)))
	 *ret=0;
   }
   return ret;
}
