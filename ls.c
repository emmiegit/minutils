#include <sys/types.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>

static struct {
	int all;
} opt;

static int list(const char *path)
{
	DIR *dh;
	struct dirent *ent;

	dh = opendir(path);
	if (!dh) {
		return 1;
	}
	while ((ent = readdir(dh))) {
		if (ent->d_name[0] == '.' && !opt.all) {
			continue;
		}
		printf("%s\n", ent->d_name);
	}
	if (closedir(dh)) {
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
			return 1;
		}
	}

	if (i == argc) {
		list(".");
	} else if (i == argc - 1) {
		list(argv[i]);
	} else for (; i < argc; i++) {
		printf("%s:\n\n", argv[i]);
		list(argv[i]);
	}
	return 0;
}

