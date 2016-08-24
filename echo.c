#include <stdio.h>

/* Usage: ./echo [text-to-print...] */
int main(int argc, char *argv[])
{
	int i;
	if (argc == 1) {
		putc('\n', stdout);
		return 0;
	}

	for (i = 1; i < argc; i++) {
		int j = 0;
		char ch;
		while ((ch = argv[i][j++]) != '\0') {
			if (putc(ch, stdout) == EOF) {
				return 1;
			}
		}

		if (i < argc - 1) {
			if (putc(' ', stdout) == EOF) {
				return 1;
			}
		}

		if (putc('\n', stdout) == EOF) {
			return 1;
		}
	}

	return 0;
}

