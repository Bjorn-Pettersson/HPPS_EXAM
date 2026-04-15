#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "util.h"

int main(int argc, char** argv) {
  if (argc != 2) {
    fprintf(stderr, "Usage: %s INTSFILE\n", argv[0]);
    exit(1);
  }

  int n;
  int32_t *p = read_ints(argv[1], &n);

  for (int i = 0; i < n; i++) {
    printf("%d\n", p[i]);
  }

  free(p);
}
