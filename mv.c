#define _POSIX_C_SOURCE		200809L

#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#include <errno.h>
#include <stdio.h>
#include <string.h>

/*
 * NOTE:
 * This program can only move files that are on the same filesystem.
 */

/* Usage: ./mv original... destination */
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
			if (renameat(AT_FDCWD, argv[i], destfd, argv[i])) {
				fprintf(stderr, "%s: %s: %s\n",
					argv[0], argv[i], strerror(errno));
				return 1;
			}
		}
		if (close(destfd)) {
			fprintf(stderr, "%s: %s: %s\n",
				argv[0], dest, strerror(errno));
			return 1;
		}
	}
	return 0;
}

