#include <unistd.h>

#include <stdlib.h>
#include <stdio.h>

/* Usage: sleep SECONDS */
int main(int argc, const char *argv[])
{
	char *ptr;
	long val;

	if (argc != 2) {
		fprintf(stderr, "usage: %s seconds\n", argv[0]);
		return 1;
	}
	val = strtol(argv[1], &ptr, 10);
	if (*ptr || val < 0) {
		fprintf(stderr, "%s: not a positive integer: %s\n",
			argv[0], argv[1]);
		exit(1);
	}
	if (sleep(val))
		return 1;
	return 0;
}
