#include <stdio.h>

/* Usage: ./cat [file...] */
int main(int argc, char *argv[])
{
    if (argc == 1) {
        int ch;
        while ((ch = fgetc(stdin)) != EOF) {
            putc(ch, stdout);
        }
    } else {
        int i;
        for (i = 1; i < argc; i++) {
            FILE *fh;
            if (argv[i][0] == '-' && argv[i][1] == '\0') {
                fh = stdin;
            } else {
                fh = fopen(argv[i], "r");
            }

            if (fh == NULL) {
                return 1;
            }

            int ch;
            while ((ch = fgetc(fh)) != EOF) {
                putc(ch, stdout);
            }
            fclose(fh);
        }
    }

    return 0;
}


