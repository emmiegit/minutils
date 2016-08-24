#include <stdio.h>
#include <time.h>
#include <utime.h>
#include <sys/stat.h>

/* Usage: ./touch file */
int main(int argc, char *argv[])
{
	struct stat statbuf;
	struct utimbuf new_times;

	if (argc < 2) {
		return 1;
	}

	if (stat(argv[1], &statbuf) < 0) {
		FILE *fh;
		fh = fopen(argv[1], "w+");

		if (fh == NULL) {
			return 1;
		}

		if (fclose(fh) == EOF) {
			return 1;
		}
	}

	new_times.actime = statbuf.st_atime;
	new_times.modtime = time(NULL);

	if (utime(argv[1], &new_times) < 0) {
		return 1;
	}

	return 0;
}

