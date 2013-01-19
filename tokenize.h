// Author Phinfinity
#ifndef TOKENIZER_H
#define TOKENIZER_H
// Pass NULL to use previous string, str for new string
// similar usage as strtok
char *tokenize(char *str,int&);
void symbol_strip(char *s);
void enable_stop_words();
#endif
