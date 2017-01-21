#include <unistd.h>

int main(int argc, const char *argv[])
{
	int i;

	if (argc == 1) {
		return 1;
	}
	for (i = 1; i < argc; i++) {
		if (rmdir(argv[i])) {
			return 1;
		}
	}
	return 0;
}

