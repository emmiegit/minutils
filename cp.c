#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

static int mmap_copy(int ifd, int ofd)
{
	off_t len;
	void *ptr;

	len = lseek(ifd, 0, SEEK_END);
	if (len <= 0) {
		return 1;
	}
	ptr = mmap(NULL, len, PROT_READ, MAP_PRIVATE, ifd, 0);
	if (ptr == MAP_FAILED) {
		return 1;
	}
	if (write(ofd, ptr, len) != len) {
		return 1;
	}
	if (munmap(ptr, len)) {
		return 1;
	}
	return 0;
}

static int buffer_copy(int ifd, int ofd)
{
	char buf[4096];
	ssize_t len;

	do {
		len = read(ifd, buf, sizeof(buf));
		if (len < 0) {
			return 1;
		} else if (len == 0) {
			return 0;
		}
		if (write(ofd, buf, len) != len) {
			return 1;
		}
	} while (len == sizeof(buf));
	return 0;
}

/* Usage: ./cp input output */
int main(int argc, char *argv[])
{
	int ifd, ofd, ret;

	if (argc != 3) {
		fprintf(stderr, "%s: missing operand\n",
			argv[0]);
		return 1;
	}

	ifd = open(argv[1], O_RDONLY);
	if (ifd < 0) {
		return 1;
	}
	ofd = open(argv[2], O_CREAT | O_TRUNC | O_WRONLY, 0666);
	if (ofd < 0) {
		return 1;
	}

	ret = mmap_copy(ifd, ofd);
	if (ret) {
		ret = buffer_copy(ifd, ofd);
	}
	if (close(ifd) || close(ofd)) {
		return 1;
	}
	return ret;
}

