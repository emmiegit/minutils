#include <stdio.h>
/*
 * NOTE:
 * This program can only move files that are on the same filesystem.
 */

/* Usage: ./mv original destination */
int main(int argc, char *argv[])
{
    if (argc < 3) {
        return 1;
    }

    if (rename(argv[1], argv[2]) < 0) {
        return 1;
    }

    return 0;
}

