#include <sys/stat.h>
#include <sys/types.h>

int main(int argc, const char *argv[])
{
	int i;

	if (argc == 1) {
		return 1;
	}
	for (i = 1; i < argc; i++) {
		if (mkdir(argv[i], 0777)) {
			return 1;
		}
	}
	return 0;
}

