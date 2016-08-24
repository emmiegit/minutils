#include <stdio.h>

/* Usage: ./cp input output */
int main(int argc, char *argv[])
{
	FILE *ifh, *ofh;
	int ch;

	if (argc < 3) {
		return 1;
	}

	ifh = fopen(argv[1], "r");
	ofh = fopen(argv[2], "w+");
	if (!ifh || !ofh) {
		return 1;
	}

	while ((ch = fgetc(ifh)) != EOF) {
		if (fputc(ch, ofh) == EOF) {
			fclose(ifh);
			fclose(ofh);
			return 1;
		}
	}

	fclose(ifh);
	fclose(ofh);
	return 0;
}

