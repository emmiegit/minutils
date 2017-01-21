#define _POSIX_C_SOURCE		200809L

#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <libgen.h>
#include <unistd.h>

#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static int copy_file(const char *source, int destfd, const char *destfn)
{
	char buf[4096];
	ssize_t len;
	int ifd, ofd;

	ifd = open(source, O_RDONLY);
	if (ifd < 0) {
		return 1;
	}
	ofd = openat(destfd, destfn, O_CREAT | O_TRUNC | O_WRONLY, 0666);
	if (ofd < 0) {
		return 1;
	}

	do {
		len = read(ifd, buf, sizeof(buf));
		if (len < 0) {
			return 1;
		} else if (len == 0) {
			return 0;
		}
		if (write(ofd, buf, len) != len) {
			return 1;
		}
	} while (len == sizeof(buf));
	if (close(ifd) || close(ofd)) {
		return 1;
	}
	if (unlink(source)) {
		return 1;
	}
	return 0;
}

/* Usage: ./mv files... dest */
int main(int argc, char *argv[])
{
	if (argc == 1) {
		fprintf(stderr, "%s: missing destination\n", argv[0]);
		return 1;
	} else if (argc == 2) {
		if (rename(argv[1], argv[2])) {
			fprintf(stderr, "%s: %s\n",
				argv[0], strerror(errno));
			return 1;
		}
	} else {
		struct stat statbuf;
		const char *dest;
		int destfd, i;

		dest = argv[argc - 1];
		destfd = open(dest, O_RDONLY);
		if (destfd < 0) {
			fprintf(stderr, "%s: %s: %s\n",
				argv[0], dest, strerror(errno));
			return 1;
		}
		if (fstat(destfd, &statbuf) || !S_ISDIR(statbuf.st_mode)) {
			fprintf(stderr, "%s: %s: not a directory\n",
				argv[0], dest);
			return 1;
		}
		for (i = 1; i < argc - 1; i++) {
			char *copy, *destfn;

			copy = strdup(argv[i]);
			if (!copy) {
				fprintf(stderr, "%s: %s\n",
					argv[0], strerror(errno));
				return 1;
			}
			destfn = basename(copy);
			if (renameat(AT_FDCWD, argv[i], destfd, destfn)) {
				if (errno == EXDEV && !copy_file(argv[i], destfd, destfn)) {
					free(copy);
					continue;
				}
				fprintf(stderr, "%s: %s: %s\n",
					argv[0], argv[i], strerror(errno));
				return 1;
			}
			free(copy);
		}
		if (close(destfd)) {
			fprintf(stderr, "%s: %s: %s\n",
				argv[0], dest, strerror(errno));
			return 1;
		}
	}
	return 0;
}

