#include <sys/types.h>
#include <sys/stat.h>

#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static struct {
	unsigned all : 1;
} opt;

static struct {
	struct path {
		char *str;
		size_t len;
	} *array;
	size_t len;
} paths;

#define IS_EXECUTABLE(x)				\
	(((x) & S_IXUSR) | ((x) & S_IXGRP) | ((x) & S_IXOTH))

static void split_paths(const char *argv0, size_t extra)
{
	const char *path_env, *ptr;
	struct path *ent;
	size_t i, j;

	path_env = getenv("PATH");
	if (!path_env || !path_env[0]) {
		fprintf(stderr, "%s: no PATH variable found\n", argv0);
		exit(-1);
	}

	paths.len = 1;
	for (i = 0; path_env[i]; i++) {
		if (path_env[i] == ':') {
			paths.len++;
		}
	}

	paths.array = malloc(sizeof(struct path) * paths.len);
	if (!paths.array) {
		fprintf(stderr, "%s: unable to allocate: %s\n",
			argv0, strerror(errno));
		exit(-1);
	}

	j = 0;
	ptr = path_env;
	for (i = 0; path_env[i]; i++) {
		if (path_env[i] == ':') {
			ent = &paths.array[j++];
			ent->len = path_env + i - ptr;
			ent->str = malloc(ent->len + extra);
			if (!ent->str) {
				fprintf(stderr, "%s: unable to allocate: %s\n",
					argv0, strerror(errno));
				exit(-1);
			}
			memcpy(ent->str, ptr, ent->len);
			ptr = path_env + i + 1;


			ent->str[ent->len]='\0';
		}
	}
	ent = &paths.array[j];
	ent->len = path_env + i - ptr;
	ent->str = malloc(ent->len + extra);
	if (!ent->str) {
		fprintf(stderr, "%s: unable to allocate: %s\n",
			argv0, strerror(errno));
		exit(-1);
	}
	memcpy(ent->str, ptr, ent->len);
}

static void setup(int i, int argc, const char *argv[])
{
	size_t len, max;

	max = 0;
	for (; i < argc; i++) {
		len = strlen(argv[i]);
		if (len > max) {
			max = len;
		}
	}
	split_paths(argv[0], max + 2);
}

static void which(const char *program)
{
	struct stat stbuf;
	struct path *ent;
	size_t i;
	int found;

	found = 0;
	for (i = 0; i < paths.len; i++) {
		ent = &paths.array[i];
		ent->str[ent->len] = '/';
		strcpy(ent->str + ent->len + 1, program);

		if (stat(ent->str, &stbuf)) {
			continue;
		}
		if (IS_EXECUTABLE(stbuf.st_mode)) {
			puts(ent->str);
			if (!opt.all) {
				return;
			}
			found = 1;
		}
	}

	if (!found) {
		printf("%s not found\n", program);
	}
}

/* Usage: ./which [-a] program... */
int main(int argc, const char *argv[])
{
	int i;

	for (i = 1; i < argc; i++) {
		if (argv[i][0] != '-') {
			break;
		} else if (!strcmp(argv[i], "-a")) {
			opt.all = 1;
		}
	}
	setup(i, argc, argv);
	for (; i < argc; i++) {
		which(argv[i]);
	}
	return 0;
}
