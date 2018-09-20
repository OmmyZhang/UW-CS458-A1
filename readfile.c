#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

static int readFile(char* src)
{
  unsigned int i, len;
  char buffer[3000];
  FILE *source;
  int c;

  source = fopen(src, "r");
  if (source == NULL) {
    fprintf(stderr, "Failed to open source file\n");
    return -1;
  }
  
  i = 0;
  c = fgetc(source);
  while (c != EOF) {
    buffer[i]  = (unsigned char) c;
    c = fgetc(source);
    i++;
	if(i > 2900)
		printf("%d -> %c\n", i, c);
  }

  len = i;
  fclose(source); 

  printf("len: %d\n",len);

  return 0;
}

int main(int argc, char* argv[])
{
	readFile(argv[1]);
	return 0;
}
