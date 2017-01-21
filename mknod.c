#define _XOPEN_SOURCE	500

#include <sys/stat.h>
#include <sys/sysmacros.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static mode_t get_mode(const char *str)
{
	mode_t mode;

	if (!*str) {
		return (mode_t)-1;
	}
	mode = 0;
	while (*str) {
		if ('0' < *str && *str < '7') {
			mode += *str - '0';
			mode *= 010;
		} else {
			return (mode_t)-1;
		}
	}
	return mode;
}

static unsigned int parse_int(const char *argv0, const char *str)
{
	char *ptr;
	long value;

	value = strtol(str, &ptr, 10);
	if (*ptr) {
		fprintf(stderr, "%s: not a number: %s\n",
			argv0, str);
		exit(1);
	} else if (value < 0) {
		fprintf(stderr, "%s: number is negative: %s\n",
			argv0, str);
		exit(1);
	}
	return value;
}

/* Usage: ./mknod [-m=MODE] name type [major minor] */
int main(int argc, const char *argv[])
{
	const char *path, *typestr;
	unsigned int major, minor;
	mode_t mode;
	int i;

	mode = 0666;
	for (i = 1; i < argc; i++) {
		if (argv[i][0] != '-') {
			break;
		} else if (!strncmp("-m=", argv[i], 3)) {
			const char *modestr;

			modestr = argv[i] + 3;
			mode = get_mode(modestr);
			if (mode == (mode_t)-1) {
				fprintf(stderr, "%s: invalid mode: %s\n",
					argv[0], modestr);
				return 1;
			}
		} else {
			fprintf(stderr, "%s: invalid argument: %s\n",
				argv[0], argv[i]);
			return 1;
		}
	}

	if (argc != i + 4) {
		fprintf(stderr, "usage: %s [option]... name type [major minor]\n",
			argv[0]);
		return 1;
	} else {
		path = argv[i];
		typestr = argv[i + 1];
		major = parse_int(argv[0], argv[i + 2]);
		minor = parse_int(argv[0], argv[i + 3]);
	}

	if (!strcmp("b", typestr)) {
		mode |= S_IFBLK;
	} else if (!strcmp("c", typestr) || !strcmp("u", typestr)) {
		mode |= S_IFCHR;
	} else {
		fprintf(stderr, "%s: unknown file type: %s\n",
			argv[0], argv[i]);
		return 1;
	}
	if (mknod(path, mode, makedev(major, minor))) {
		fprintf(stderr, "%s: %s: %s\n",
			argv[0], path, strerror(errno));
		return 1;
	}
	return 0;
}

