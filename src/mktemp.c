#define _DEFAULT_SOURCE

#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <errno.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static struct {
	bool dir : 1;
} opt;

static int get_suffix(const char *str)
{
	size_t len;
	int i;

	len = strlen(str);
	for (i = len - 1; i >= 0; i--) {
		if (str[i] == 'X')
			return len - i - 1;
	}
	exit(-1);
}

static int replace_dir(const char *path)
{
	if (unlink(path))
		return 1;
	if (mkdir(path, 0777))
		return 1;
	return 0;
}

/* Usage: mktemp [-d] TEMPLATE */
int main(int argc, char *argv[])
{
	char *template;
	int i, fd;

	for (i = 1; i < argc; i++) {
		if (argv[i][0] != '-') {
			break;
		} else if (!strcmp("-d", argv[i])) {
			opt.dir = 1;
		} else {
			fprintf(stderr, "%s: invalid argument: %s\n",
				argv[0], argv[i]);
		}
	}
	if (i == argc) {
		fprintf(stderr, "%s: missing operand\n", argv[0]);
		return 1;
	} else if (i < argc - 1) {
		fprintf(stderr, "%s: extra operands\n", argv[0]);
		return 1;
	}
	template = argv[i];
	if (!strstr(template, "XXXXX")) {
		fprintf(stderr, "%s: too few X's in template: %s\n",
			argv[0], template);
		return 1;
	}
	fd = mkstemps(template, get_suffix(template));
	if (fd < 0) {
		fprintf(stderr, "%s: %s\n",
			argv[0], strerror(errno));
		return 1;
	}
	if (close(fd)) {
		fprintf(stderr, "%s: %s: %s\n",
			argv[0], template, strerror(errno));
		return 1;
	}
	if (opt.dir && replace_dir(template))
		return 1;
	puts(template);
	return 0;
}
