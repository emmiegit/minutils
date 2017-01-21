#define _XOPEN_SOURCE	500

#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#include <ctype.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static struct {
	char *size;
	int flags;
} opt;

static off_t parse_unit(char unit, off_t kilo)
{
	switch (unit) {
	case 'K':
	case 'k':
		return kilo;
		break;
	case 'M':
	case 'm':
		return kilo * kilo;
		break;
	case 'G':
	case 'g':
		return kilo * kilo * kilo;
		break;
	case 'T':
	case 't':
		return kilo * kilo * kilo * kilo;
		break;
	case 'P':
	case 'p':
		return kilo * kilo * kilo * kilo * kilo;
		break;
	case 'E':
	case 'e':
		return kilo * kilo * kilo * kilo * kilo * kilo;
		break;
	case 'Z':
	case 'z':
		return kilo * kilo * kilo * kilo * kilo * kilo * kilo;
		break;
	case 'Y':
	case 'y':
		return kilo * kilo * kilo * kilo * kilo * kilo * kilo * kilo;
		break;
	default:
		return 0;
	}
}

static off_t parse_size(const char *argv0)
{
	size_t len;
	off_t value;
	char unit;
	char *ptr;

	if (!opt.size) {
		return 0;
	}
	len = strlen(opt.size);
	switch (len) {
	case 0:
		fprintf(stderr, "%s: invalid size: %s\n",
			argv0, opt.size);
		exit(1);
	case 1:
		if (!isdigit(opt.size[0])) {
			fprintf(stderr, "%s: invalid size: %s\n",
				argv0, opt.size);
			exit(1);
		}
		return opt.size[0] - '0';
	}

	unit = opt.size[len - 1];
	value = parse_unit(unit, 1024);
	if (!value && (unit == 'B' || unit == 'b')) {
		unit = opt.size[len - 2];
		value = parse_unit(unit, 1000);
		if (!value) {
			fprintf(stderr, "%s: invalid number: %s\n",
				argv0, opt.size);
			exit(1);
		}
		opt.size[len - 2] = '\0';
	}
	opt.size[len - 1] = '\0';

	value *= strtol(opt.size, &ptr, 10);
	if (*ptr) {
		fprintf(stderr, "%s: invalid number: %s\n",
			argv0, opt.size);
		exit(1);
	}
	return value;
}

/* ./truncate [-c] [-s=SIZE] file... */
int main(int argc, char *argv[])
{
	off_t length;
	int i;

	opt.flags = O_CREAT;
	for (i = 1; i < argc; i++) {
		if (argv[i][0] != '-') {
			break;
		} else if (!strcmp(argv[i], "-c")) {
			opt.flags = 0;
		} else if (!strncmp(argv[i], "-s=", 3)) {
			opt.size = argv[i] + 3;
		} else {
			fprintf(stderr, "%s: invalid argument: %s\n",
				argv[0], argv[i]);
			return 1;
		}
	}
	if (i == argc) {
		fprintf(stderr, "%s: missing operands\n", argv[0]);
		return 1;
	}
	length = parse_size(argv[0]);

	for (; i < argc; i++) {
		int fd;

		fd = open(argv[i], O_WRONLY | opt.flags, 0666);
		if (fd < 0) {
			fprintf(stderr, "%s: %s: %s\n",
				argv[0], argv[i], strerror(errno));
			return 1;
		}
		if (ftruncate(fd, length)) {
			fprintf(stderr, "%s: %s: %s\n",
				argv[0], argv[i], strerror(errno));
			return 1;
		}
		if (close(fd)) {
			fprintf(stderr, "%s: %s: %s\n",
				argv[0], argv[i], strerror(errno));
			return 1;
		}
	}
	return 0;
}
