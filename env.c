#define _POSIX_C_SOURCE		200112L

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static void print_env(const char *environ[])
{
	const char **env;

	for (env = environ; *env; env++) {
		puts(*env);
	}
}

/* Usage: env [VARIABLE=value] program [ARGUMENTS] */
int main(int argc, char *argv[], const char *environ[])
{
	int i;

	if (argc == 1) {
		print_env(environ);
		return 0;
	} else for (i = 1; i < argc; i++) {
		char *val;

		val = strchr(argv[i], '=');
		if (!val) {
			break;
		}
		val[0] = '\0';
		val++;
		if (setenv(argv[i], val, 1)) {
			fprintf(stderr, "%s: %s\n",
				argv[0], strerror(errno));
			return 1;
		}
	}
	if (execvp(argv[i], argv + i)) {
		fprintf(stderr, "%s: %s\n",
			argv[0], strerror(errno));
		return 1;
	}
	return 0;
}
