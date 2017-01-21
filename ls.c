#include <sys/types.h>
#include <dirent.h>

#include <errno.h>
#include <stdio.h>
#include <string.h>

static struct {
	int all;
} opt;

static int list(const char *argv0, const char *path)
{
	DIR *dh;
	struct dirent *ent;

	dh = opendir(path);
	if (!dh) {
		fprintf(stderr, "%s: %s: %s\n",
			argv0, path, strerror(errno));
		return 1;
	}
	while ((ent = readdir(dh))) {
		if (ent->d_name[0] == '.' && !opt.all) {
			continue;
		}
		printf("%s\n", ent->d_name);
	}
	if (closedir(dh)) {
		fprintf(stderr, "%s: %s: %s\n",
			argv0, path, strerror(errno));
		return 1;
	}
	return 0;
}

/* Usage: ls [-a] [path...] */
int main(int argc, const char *argv[])
{
	int i;

	for (i = 1; i < argc; i++) {
		if (argv[i][0] != '-') {
			break;
		} else if (!strcmp(argv[i], "-a")) {
			opt.all = 1;
		} else {
			fprintf(stderr, "%s: invalid argument: %s\n",
				argv[0], argv[i]);
			return 1;
		}
	}

	if (i == argc) {
		if (list(argv[0], ".")) {
			return 1;
		}
	} else if (i == argc - 1) {
		if (list(argv[0], argv[i])) {
			return 1;
		}
	} else for (; i < argc; i++) {
		printf("%s:\n\n", argv[i]);
		if (list(argv[0], argv[i])) {
			return 1;
		}
	}
	return 0;
}

