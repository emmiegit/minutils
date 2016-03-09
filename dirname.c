#include <libgen.h>
#include <stdio.h>

/* Usage: ./dirname path */
int main(int argc, char* argv[])
{
    if (argc < 2) {
        return 1;
    }

    printf("%s\n", dirname(argv[1]));
    return 0;
}

