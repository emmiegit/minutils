#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <err.h>
#include <unistd.h>
#include <limits.h>

int main(int argc, char *argv[]) {
  char name[HOST_NAME_MAX + 1];

  if (1 == argc) {
    if (gethostname(name, sizeof(name))) {
      err(EXIT_FAILURE, "gethostname");
    }
    puts(name);
  } else {
    if (sethostname(argv[1], strlen(argv[1]))) {
      err(EXIT_FAILURE, "sethostname");
    }
  }

  return EXIT_SUCCESS;
}
