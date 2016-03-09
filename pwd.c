#include <limits.h>
#include <unistd.h>
#include <stdio.h>

/* Usage: ./pwd */
int main(int argc, char* argv[])
{
    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("%s\n", cwd);
    } else {
        return 1;
    }

    return 0;
}

