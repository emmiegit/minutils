#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEFAULT_LINE_CAPACITY 32
#define DEFAULT_CHAR_CAPACITY 64

struct {
	char **array;
	size_t length;
	size_t capacity;
} lines;

struct {
	char *array;
	size_t length;
	size_t capacity;
} line;

static void append_to_line_list();
static void append_to_char_list(char ch);

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

	if (lines.array == NULL) {
		return 1;
	}

	if (argc == 1) {
		int ch;
		while ((ch = fgetc(stdin)) != EOF) {
			if (ch != '\n') {
				append_to_char_list(ch);
			} else {
				append_to_line_list();
				line.length = 0;
			}
		}

		append_to_line_list(&lines, &line);
	} else {
		for (i = 1; i < argc; i++) {
			FILE *fh;
			int ch;
			if (!strcmp(argv[i], "-")) {
				fh = stdin;
			} else {
				fh = fopen(argv[i], "r");
			}

			if (fh == NULL) {
				return 1;
			}

			while ((ch = fgetc(fh)) != EOF) {
				if (ch != '\n') {
					append_to_char_list(ch);
				} else {
					append_to_line_list();
					line.length = 0;
				}
			}

			fclose(fh);
			append_to_line_list(&lines, &line);
		}
	}

	free(line.array);

	for (i = lines.length - 1; i >= 0; i--) {
		printf("%s\n", lines.array[i]);
	}

	return 0;
}

static void append_to_line_list()
{
	char *str;
	if (lines.length >= lines.capacity) {
		size_t new_capacity = lines.capacity * 2;
		char **new_array = malloc(sizeof(char *) * new_capacity);

		if (new_array == NULL) {
			exit(1);
		}

		memcpy(new_array, lines.array, sizeof(char *) * lines.length);
		free(lines.array);
		lines.array = new_array;
		lines.capacity = new_capacity;
	}

	str = malloc(line.length + 1);
	if (!str) {
		exit(1);
	}

	memcpy(str, line.array, line.length);
	str[line.length] = '\0';
	lines.array[lines.length++] = str;
}

static void append_to_char_list(char ch)
{
	if (line.length >= line.capacity) {
		size_t new_capacity = line.capacity * 2;
		char *new_array = malloc(sizeof(char) * new_capacity);

		if (new_array == NULL) {
			exit(-1);
		}

		memcpy(new_array, line.array, sizeof(char) * line.length);
		free(line.array);
		line.array = new_array;
		line.capacity = new_capacity;
	}

	line.array[line.length++] = ch;
}

