#include <libgen.h>

#include <stdio.h>

/* Usage: ./basename PATH */
int main(int argc, char *argv[])
{
	if (argc == 1) {
		fprintf(stderr, "%s: missing operand\n", argv[0]);
		return 1;
	}
	puts(basename(argv[1]));
	return 0;
}
