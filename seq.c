#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static struct {
	const char *argv0;
	const char *sep;

	double first;
	double last;
	double increment;
} opt;

static double parse_float(const char *str)
{
	char *ptr;
	double val;

	val = strtod(str, &ptr, 0);
	if (*ptr) {
		fprintf(stderr, "%s:
}

/* Usage: ./seq [-s=SEPARATOR] [FIRST] [INCREMENT] LAST */
int main(int argc, const char *argv[])
{
	int i;

	opt.argv0 = argv[0];
	opt.sep = "\n";
	for (i = 1; i < argc; i++) {
		if (argv[i][0] != '-') {
			break;
		} else if (!strncmp(argv[i], "-s=", 3)) {
			opt.sep = argv[i] + 3;
		} else {
			fprintf(stderr, "%s: invalid argument: %s\n",
				argv[0], strerror(errno));
			return 1;
		}
	}
	switch (argc - i) {
	case 1:
		;
	}
}
