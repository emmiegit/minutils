#define _POSIX_C_SOURCE		200809L

#include <unistd.h>

#include <errno.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static struct {
	const char *argv0;
	bool unique  : 1;
	bool reverse : 1;
} opt;

static struct {
	char **array;
	size_t len;
	size_t cap;
} lines;

static void lines_init(void)
{
	lines.len = 0;
	lines.cap = 64;
	lines.array = malloc(lines.cap * sizeof(char *));
	if (!lines.array) {
		fprintf(stderr, "%s: unable to allocate array: %s\n",
			opt.argv0, strerror(errno));
		exit(1);
	}
}

static void lines_resize(void)
{
	void *ptr;
	size_t new_cap;

	new_cap = lines.cap * 2;
	ptr = realloc(lines.array, new_cap * sizeof(char *));
	if (!ptr) {
		fprintf(stderr, "%s: unable to resize array: %s\n",
			opt.argv0, strerror(errno));
		exit(1);
	}

	lines.array = ptr;
}

static void lines_append(char *line)
{
	if (lines.len >= lines.cap)
		lines_resize();

	lines.array[lines.len] = line;
	lines.len++;
}

static int compare_normal(const void *_x, const void *_y)
{
	const char *const *const x = _x;
	const char *const *const y = _y;

	return strcmp(*x, *y);
}

static int compare_reverse(const void *_x, const void *_y)
{
	const char *const *const x = _x;
	const char *const *const y = _y;

	return -strcmp(*x, *y);
}

static void read_lines(FILE *fh)
{
	char *line;
	size_t n;

	do {
		line = NULL;
		n = 0;

		if (getline(&line, &n, fh) < 0) {
			free(line);
			return;
		}
		lines_append(line);
	} while (line);
}

int main(int argc, char *argv[])
{
	size_t i;
	int ch;
	struct {
		FILE **array;
		size_t len;
		bool allocd : 1;
	} files;

	opt.argv0 = argv[0];

	/* Parse command line options */
	while ((ch = getopt(argc, argv, "ur")) != -1) {
		switch (ch) {
		case 'u':
			puts("-u TODO");
			opt.unique = 1;
			break;
		case 'r':
			opt.reverse = 1;
			break;
		case '?':
			return 1;
		default:
			abort();
		}
	}

	/* Open files */
	if (optind == argc) {
		files.allocd = 0;
		files.array = &stdin;
		files.len = 1;
	} else {
		files.allocd = 1;
		files.len = argc - optind;
		files.array = malloc(files.len * sizeof(FILE *));
		if (!files.array) {
			fprintf(stderr, "%s: unable to allocate file array: %s\n",
				argv[0], strerror(errno));
			return 1;
		}

		for (i = 0; i < files.len; i++) {
			files.array[i] = fopen(argv[i + optind], "r");
			if (!files.array[i]) {
				fprintf(stderr, "%s: %s: unable to open\n",
					argv[0], argv[i]);
				return 1;
			}
		}
	}

	/* Ingest lines from files */
	lines_init();
	for (i = 0; i < files.len; i++) {
		read_lines(files.array[i]);
		fclose(files.array[i]);
	}
	if (files.allocd)
		free(files.array);

	/* Sort and print lines */
	qsort(lines.array,
	      lines.len,
	      sizeof(char *),
	      (opt.reverse)
		? compare_reverse
		: compare_normal);

	for (i = 0; i < lines.len; i++)
		fputs(lines.array[i], stdout);
	return 0;
}
