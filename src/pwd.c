#define _XOPEN_SOURCE	500

#include <limits.h>
#include <unistd.h>

#include <errno.h>
#include <stdio.h>
#include <string.h>

#define UNUSED(x)	((void)(x))

/* Usage: pwd */
int main(int argc, const char *argv[])
{
	char cwd[PATH_MAX];

	UNUSED(argc);

	if (getcwd(cwd, sizeof(cwd))) {
		printf("%s\n", cwd);
	} else {
		fprintf(stderr, "%s: %s\n",
			argv[0], strerror(errno));
		return 1;
	}
	return 0;
}
