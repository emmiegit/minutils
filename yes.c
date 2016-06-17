#include <stdio.h>

int main(int argc, char *argv[])
{
    if (argc < 2) {
        for (;;) {
            printf("yes\n");
        }
    } else {
        for (;;) {
            printf("%s\n", argv[1]);
        }
    }

    return 0;
}

