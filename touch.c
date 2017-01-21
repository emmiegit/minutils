#include <fcntl.h>
#include <time.h>
#include <unistd.h>
#include <utime.h>

#include <sys/stat.h>
#include <sys/types.h>

/* Usage: ./touch file */
int main(int argc, char *argv[])
{
	struct stat statbuf;
	struct utimbuf new_times;
	time_t new_time;
	int ret;

	if (argc == 1) {
		return 1;
	}

	ret = stat(argv[1], &statbuf);
	if (ret) {
		int fd = open(argv[1], O_CREAT | O_RDONLY, 0666);
		if (fd < 0) {
			return 1;
		}

		ret = close(fd);
		if (ret) {
			return 1;
		}
	}

	new_time = time(NULL);
	new_times.actime = new_time;
	new_times.modtime = new_time;

	ret = utime(argv[1], &new_times);
	if (ret < 0) {
		return 1;
	}

	return 0;
}

