#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEFAULT_LINE_CAPACITY 32
#define DEFAULT_CHAR_CAPACITY 64

struct line_list {
    char **array;
    size_t length;
    size_t capacity;
};

struct char_list {
    char *array;
    size_t length;
    size_t capacity;
};

static void append_to_line_list(struct line_list *list, struct char_list *line);
static void append_to_char_list(struct char_list *list, char ch);

/* Usage: ./tac [file...] */
int main(int argc, char *argv[])
{
    struct line_list lines;
    lines.array = malloc(sizeof(char *) * DEFAULT_LINE_CAPACITY);
    lines.length = 0;
    lines.capacity = DEFAULT_LINE_CAPACITY;

    struct char_list line;
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
                append_to_char_list(&line, ch);
            } else {
                append_to_line_list(&lines, &line);
                line.length = 0;
            }
        }

        append_to_line_list(&lines, &line);
    } else {
        int i;
        for (i = 1; i < argc; i++) {
            FILE *fh;
            if (!strcmp(argv[i], "-")) {
                fh = stdin;
            } else {
                fh = fopen(argv[i], "r");
            }

            if (fh == NULL) {
                return 1;
            }

            int ch;
            while ((ch = fgetc(fh)) != EOF) {
                if (ch != '\n') {
                    append_to_char_list(&line, ch);
                } else {
                    append_to_line_list(&lines, &line);
                    line.length = 0;
                }
            }

            fclose(fh);
            append_to_line_list(&lines, &line);
        }
    }

    free(line.array);

    int i;
    for (i = lines.length - 1; i >= 0; i--) {
        printf("%s\n", lines.array[i]);
    }

    return 0;
}

static void append_to_line_list(struct line_list *list, struct char_list *line)
{
    if (list->length >= list->capacity) {
        size_t new_capacity = list->capacity * 2;
        char **new_array = malloc(sizeof(char *) * new_capacity);

        if (new_array == NULL) {
            exit(1);
        }

        memcpy(new_array, list->array, sizeof(char *) * list->length);
        free(list->array);
        list->array = new_array;
        list->capacity = new_capacity;
    }

    char *string = malloc(sizeof(char) * line->length + 1);
    memcpy(string, line->array, line->length);
    string[line->length] = '\0';

    list->array[list->length++] = string;
}

static void append_to_char_list(struct char_list *list, char ch)
{
    if (list->length >= list->capacity) {
        size_t new_capacity = list->capacity * 2;
        char *new_array = malloc(sizeof(char) * new_capacity);

        if (new_array == NULL) {
            exit(1);
        }

        memcpy(new_array, list->array, sizeof(char) * list->length);
        free(list->array);
        list->array = new_array;
        list->capacity = new_capacity;
    }

    list->array[list->length++] = ch;
}

