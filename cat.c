#include <fcntl.h>
#include <unistd.h>

static int cat(int fd)
{
	char buffer[8192];
	long n;

	while ((n = read(fd, buffer, sizeof(buffer))) > 0) {
		if (write(1, buffer, n) != n) {
			return 1;
		}
	}

	return 0;
}

/* Usage: ./cat [file...] */
int main(int argc, char *argv[])
{
	int i;

	if (argc == 1) {
		return cat(STDIN_FILENO);
	} else for (i = 1; i < argc; i++) {
		int ret, fd;
		fd = open(argv[i], O_RDONLY);
		if (fd < 0) {
			return 1;
		}
		ret = cat(fd);
		close(fd);
		if (ret) {
			return 1;
		}
	}

	return 0;
}

