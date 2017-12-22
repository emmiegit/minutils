#define _XOPEN_SOURCE	500

#include <unistd.h>

#include <stdbool.h>
#include <stdio.h>

static char *set1, *set2;
static struct {
	bool complement : 1;
	bool delete     : 1;
	bool squeeze    : 1;
	bool truncate   : 1;
} opt;

static void interpret_sets(int arg, char *argv[])
{
	size_t i;

	set1 = argv[arg];
	set2 = argv[arg + 1];

	for (i = 0; set1[i]; i++) {
		if (set1[i] == '\\') {

		}
	}

}

static void truncate_sets(void)
{
}

int main(int argc, char *argv[])
{
	int i, ch;

	while ((ch = getopt(argc, argv, "
}
