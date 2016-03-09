#include <unistd.h>
/*
 * NOTE:
 * This program does not take options, and only creates hard links.
 * For symbolic links, use ln_s instead.
 */

/* Usage: ./ln source new-path */
int main(int argc, char* argv[])
{
    if (argc < 3) {
        return 1;
    }

    if (link(argv[1], argv[2]) < 0) {
        return 1;
    }

    return 0;
}

