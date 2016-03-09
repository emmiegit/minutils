#include <unistd.h>

/* Usage: ./rm [file...] */
int main(int argc, char* argv[])
{
    if (argc < 2) {
        return 1;
    }

    int i;
    for (i = 1; i < argc; i++) {
        if (unlink(argv[i]) < 0) {
            return 1;
        }
    }

    return 0;
}

