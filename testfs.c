#include <stdio.h>

void pp(char* para)
{
	char output[200];
	int i,a;
	strcpy(output, para);
	i = 1;
	a = 7;
	printf(output, i, a, &a);
}

int main()
{
	pp("So test it. %d %d %x\n");
	return 0;
}

