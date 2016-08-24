#include <stdio.h>

int main(int argc, char *argv[])
{
	const char *str;

	if (argc < 2) {
		str = "yes";
	} else {
		str = argv[1];
	}

	while (1) {
		puts(str);
	}

	return 0;
}

