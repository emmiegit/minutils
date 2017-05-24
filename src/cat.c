#include <fcntl.h>
#include <unistd.h>

#include <errno.h>
#include <stdio.h>
#include <string.h>

static const char *argv0;

static int cat(int fd)
{
	char buf[4096];
	ssize_t n;

	while ((n = read(fd, buf, sizeof(buf))) > 0) {
		if (write(STDOUT_FILENO, buf, n) != n) {
			fprintf(stderr, "%s: %s\n",
				argv0, strerror(errno));
			return 1;
		}
	}
	return 0;
}

/* Usage: cat [FILE...] */
int main(int argc, const char *argv[])
{
	int i;

	argv0 = argv[0];
	if (argc == 1) {
		return cat(STDIN_FILENO);
	} else for (i = 1; i < argc; i++) {
		int fd;

		fd = open(argv[i], O_RDONLY);
		if (fd < 0) {
			fprintf(stderr, "%s: %s: %s\n",
				argv[0], argv[i], strerror(errno));
			return 1;
		}
		if (cat(fd))
			return 1;
		if (close(fd)) {
			fprintf(stderr, "%s: %s: %s\n",
				argv[0], argv[i], strerror(errno));
			return 1;
		}
	}
	return 0;
}
