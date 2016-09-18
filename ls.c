#include <sys/types.h>
#include <dirent.h>
#include <stdio.h>

/* Usage: ls [path] */
int main(int argc, const char *argv[])
{
	DIR *dirh;
	struct dirent *dirent;
	const char *path;
	int ret;

	if (argc == 1) {
		path = ".";
	} else if (argc == 2) {
		path = argv[1];
	} else {
		return 1;
	}

	dirh = opendir(path);
	if (!dirh) {
		return 1;
	}

	while ((dirent = readdir(dirh))) {
		printf("%s\n", dirent->d_name);
	}

	ret = closedir(dirh);
	if (ret) {
		return 1;
	}

	return 0;
}

