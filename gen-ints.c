#include <stdio.h>
#include <stdlib.h>

#include "util.h"

int main(int argc, char** argv) {
  if (argc != 5) {
    fprintf(stderr, "Usage: %s N K STRIDE OUTFILE\n", argv[0]);
    exit(1);
  }

  int n = atoi(argv[1]);
  int k = atoi(argv[2]);
  int stride = atoi(argv[3]);
  char* outfile = argv[4];

  if (k < 1) {
    fprintf(stderr, "k must be a positive number, but received %d\n", k);
  }

  int *ints = calloc(n, sizeof(int));

  for (int i = 0; i < n; i++) {
    ints[i] = i*stride % k;
  }

  write_ints(outfile, n, ints);

  free(ints);
}
