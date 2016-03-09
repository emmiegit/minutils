#include <pwd.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

/* Usage: ./whoami */
int main(int argc, char* argv[])
{
    uid_t uid = geteuid();
    struct passwd* pw = getpwuid(uid);
    if (pw == NULL) {
        return 1;
    }

    printf("%s\n", pw->pw_name);
    return 0;
}

