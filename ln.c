#include <string.h>
#include <unistd.h>

/* Usage: ./ln [-s] source new-path */
int main(int argc, char *argv[])
{
	if (argc == 4) {
		if (!strcmp(argv[1], "-s")) {
			if (symlink(argv[2], argv[3]) < 0) {
				return 1;
			}
		} else {
			return 1;
		}
	} else if (argc == 3) {
		if (link(argv[1], argv[2]) < 0) {
			return 1;
		}
	} else {
		return 1;
	}

	return 0;
}

