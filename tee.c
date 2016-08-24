#include <stdio.h>
#include <stdlib.h>

/* Usage: ./tee [file...] */
int main(int argc, char *argv[])
{
	FILE **fh;
	int ch, i;

	fh = malloc(sizeof(FILE *) * (argc - 1));
	if (!fh) {
		return -1;
	}

	for (i = 0; i < argc - 1; i++) {
		if ((fh[i] = fopen(argv[i + 1], "w+")) == NULL) {
			return 1;
		}
	}

	while ((ch = fgetc(stdin)) != EOF) {
		fputc(ch, stdout);
		for (i = 0; i < argc - 1; i++) {
			fputc(ch, fh[i]);
		}
	}

	for (i = 0; i < argc - 1; i++) {
		fclose(fh[i]);
	}

	return 0;
}

