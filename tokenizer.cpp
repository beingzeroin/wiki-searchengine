#include <cstdio>

int bufsz = 102400;
char *buf; 
void resize_buf(int s) {
   while(bufsz < s)
      bufsz*=2;
   delete [] buf;
   buf = new char[bufsz+5];
}
bool read_val() {
   int cnt;
   if(scanf("%d%*c",&cnt)==EOF)
      return false;
   if(cnt>bufsz) 
      resize_buf(cnt);
   int r = fread(buf,1,cnt,stdin);
   buf[r]=0;
   return true;
}
int main() {
   buf = new char[bufsz+5];
   while(true) {
      if(!read_val())
	 break;
      printf("Title: %s\n",buf);
      read_val();
      printf("Id: %s\n",buf);
      read_val();
      printf("Text: %s\n",buf);
   }
}
