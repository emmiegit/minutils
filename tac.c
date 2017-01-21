#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEFAULT_LINE_CAPACITY	32
#define DEFAULT_CHAR_CAPACITY	64

static struct {
	char **array;
	size_t length;
	size_t capacity;
} lines;

static struct {
	char *array;
	size_t length;
	size_t capacity;
} line;

static void append_to_line_list(const char *argv0)
{
	char *str;
	if (lines.length >= lines.capacity) {
		void *ptr;
		size_t new_capacity;

		new_capacity = lines.capacity * 2;
		ptr = realloc(lines.array, sizeof(char *) * new_capacity);
		if (!ptr) {
			fprintf(stderr, "%s: %s\n",
				argv0, strerror(errno));
			exit(-1);
		}
		lines.array = ptr;
		lines.capacity = new_capacity;
	}

	str = malloc(line.length + 1);
	if (!str) {
		fprintf(stderr, "%s: %s\n",
			argv0, strerror(errno));
		exit(1);
	}

	memcpy(str, line.array, line.length);
	str[line.length] = '\0';
	lines.array[lines.length++] = str;
}

static void append_to_char_list(const char *argv0, char ch)
{
	if (line.length >= line.capacity) {
		void *ptr;
		size_t new_capacity;

		new_capacity = lines.capacity * 2;
		ptr = realloc(line.array, sizeof(char) * new_capacity);
		if (!ptr) {
			fprintf(stderr, "%s: %s\n",
				argv0, strerror(errno));
			exit(-1);
		}
		line.array = ptr;
		line.capacity = new_capacity;
	}
	line.array[line.length++] = ch;
}

/* Usage: ./tac [file...] */
int main(int argc, char *argv[])
{
	int i;

	lines.array = malloc(sizeof(char *) * DEFAULT_LINE_CAPACITY);
	lines.length = 0;
	lines.capacity = DEFAULT_LINE_CAPACITY;

	line.array = malloc(sizeof(char) * DEFAULT_CHAR_CAPACITY);
	line.length = 0;
	line.capacity = DEFAULT_CHAR_CAPACITY;

	if (!lines.array) {
		fprintf(stderr, "%s: %s\n",
			argv[0], strerror(errno));
		return 1;
	}
	if (argc == 1) {
		int ch;

		while ((ch = fgetc(stdin)) != EOF) {
			if (ch != '\n') {
				append_to_char_list(argv[0], ch);
			} else {
				append_to_line_list(argv[0]);
				line.length = 0;
			}
		}
		append_to_line_list(argv[0]);
	} else {
		for (i = 1; i < argc; i++) {
			FILE *fh;
			int ch;

			if (!strcmp(argv[i], "-")) {
				fh = stdin;
			} else {
				fh = fopen(argv[i], "r");
			}

			if (!fh) {
				return 1;
			}
			while ((ch = fgetc(fh)) != EOF) {
				if (ch != '\n') {
					append_to_char_list(argv[0], ch);
				} else {
					append_to_line_list(argv[0]);
					line.length = 0;
				}
			}
			if (fclose(fh)) {
				fprintf(stderr, "%s: %s\n",
					argv[0], strerror(errno));
				return 1;
			}
			append_to_line_list(argv[0]);
		}
	}

	free(line.array);
	for (i = lines.length - 1; i >= 0; i--) {
		printf("%s\n", lines.array[i]);
	}
	return 0;
}

