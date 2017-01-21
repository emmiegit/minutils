#define _XOPEN_SOURCE	500

#include <fcntl.h>
#include <unistd.h>

/* Usage ./sync [path...] */
int main(int argc, const char *argv[])
{
	int i;

	if (argc == 1) {
		sync();
	} else for (i = 1; i < argc; i++) {
		int fd;

		fd = open(argv[i], O_WRONLY);
		if (fd < 0) {
			return 1;
		}
		if (fsync(fd)) {
			return 1;
		}
		if (close(fd)) {
			return 1;
		}
	}

	return 0;
}

