#include <stdio.h>

/* yes [TEXT...] */
int main(int argc, char *argv[])
{
	if (argc == 1) {
		while (1)
			puts("y");
	} else {
		while (1) {
			int i;

			for (i = 1; i < argc; i++) {
				fputs(argv[i], stdout);
				if (i < argc - 1)
					putchar(' ');
			}
			putchar('\n');
		}
	}
	return 0;
}
