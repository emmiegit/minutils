#include <fcntl.h>
#include <unistd.h>

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Usage: ./tee [FILE...] */
int main(int argc, char *argv[])
{
	char buf[4096];
	ssize_t n;
	int *fds;
	int i, ret;

	fds = malloc(sizeof(int) * argc);
	if (!fds) {
		fprintf(stderr, "%s: unable to allocate: %s\n",
			argv[0], strerror(errno));
		return -1;
	}
	fds[0] = STDOUT_FILENO;
	for (i = 1; i < argc; i++) {
		fds[i] = open(argv[i], O_CREAT | O_TRUNC | O_WRONLY, 0666);
		if (!fds[i]) {
			return 1;
		}
	}
	while ((n = read(STDIN_FILENO, buf, sizeof(buf))) > 0) {
		for (i = 0; i < argc; i++) {
			if (write(fds[i], buf, n) != n) {
				fprintf(stderr, "%s: %s: %s\n",
					argv[0], argv[i], strerror(errno));
				return 1;
			}
		}
	}

	ret = 0;
	for (i = 1; i < argc; i++) {
		if (close(fds[i])) {
			ret = 1;
		}
	}
	free(fds);
	return ret;
}
