#include <libgen.h>
#include <stdio.h>

/* Usage: ./basename path */
int main(int argc, char *argv[])
{
	if (argc == 1) {
		return 1;
	}

	printf("%s\n", basename(argv[1]));
	return 0;
}

