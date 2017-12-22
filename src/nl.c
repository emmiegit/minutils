#define _POSIX_C_SOURCE		200809L

#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAX(x, y)	(((x) > (y)) ? (x) : (y))
#define MIN(x, y)	(((x) < (y)) ? (x) : (y))

/* Usage: nl [FILE...] */
int main(int argc, const char *argv[])
{
	struct {
		FILE **array;
		int len;
	} files;
	struct {
		char *buf;
		size_t len;
	} line;
	unsigned long lineno;
	int i;

	line.buf = NULL;
	line.len = 0;

	/* Allocate file array */
	files.len = MAX(1, argc - 1);
	files.array = malloc(files.len * sizeof(FILE *));
	if (!files.array) {
		fprintf(stderr, "%s: unable to allocate file array: %s\n",
			argv[0], strerror(errno));
		return 1;
	}

	/* Open files */
	if (argc < 2) {
		files.array[0] = stdin;
	} else {
		for (i = 0; i < files.len; i++) {
			if (!strcmp(argv[i + 1], "-")) {
				files.array[i] = stdin;
			} else {
				files.array[i] = fopen(argv[i + 1], "r");
				if (!files.array[i]) {
					fprintf(stderr, "%s: %s: unable to open\n",
						argv[0], argv[i + 1]);
					return 1;
				}
			}
		}
	}

	/* Read and number lines */
	lineno = 1;
	for (i = 0; i < files.len; i++) {
		for (;;) {
			if (getline(&line.buf, &line.len, files.array[i]) < 0)
				break;

			printf("%6lu  %s", lineno, line.buf);
			lineno++;
		}
		fclose(files.array[i]);
	}

	free(files.array);
	free(line.buf);
	return 0;
}
