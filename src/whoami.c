#include <sys/types.h>
#include <unistd.h>
#include <pwd.h>

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Usage: whoami */
int main(int argc, const char *argv[])
{
	struct passwd *pw;
	uid_t uid;

	(void)argc;

	uid = geteuid();
	pw = getpwuid(uid);
	if (!pw) {
		fprintf(stderr, "%s: %s\n",
			argv[0], strerror(errno));
		return 1;
	}

	printf("%s\n", pw->pw_name);
	return 0;
}
