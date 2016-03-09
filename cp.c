#include <stdio.h>

/* Usage: ./cp input output */
int main(int argc, char* argv[])
{
    if (argc < 3) {
        return 1;
    }

    FILE* ifh;
    FILE* ofh;
    ifh = fopen(argv[1], "r");
    ofh = fopen(argv[2], "w+");
    if (!ifh || !ofh) {
        return 1;
    }

    int ch;
    while ((ch = fgetc(ifh)) != EOF) {
        if (fputc(ch, ofh) == EOF) {
            fclose(ifh);
            fclose(ofh);
            return 1;
        }
    }

    fclose(ifh);
    fclose(ofh);
    return 0;
}

