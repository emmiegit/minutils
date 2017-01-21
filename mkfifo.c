#include <sys/stat.h>
#include <sys/types.h>

int main(int argc, const char *argv[])
{
	int i;

	if (argc == 1) {
		return 1;
	}
	for (i = 1; i < argc; i++) {
		if (mkfifo(argv[i], 0666)) {
			return 1;
		}
	}
	return 0;
}

