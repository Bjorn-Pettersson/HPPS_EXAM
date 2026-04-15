#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "util.h"

int main(int argc, char** argv) {
  if (argc != 2) {
    fprintf(stderr, "Usage: %s POINTSFILE\n", argv[0]);
    exit(1);
  }

  int n;
  double *p = read_points(argv[1], &n);

  for (int i = 0; i < n; i++) {
    printf("(%f,%f)\n", p[i*2], p[i*2+1]);
  }

  free(p);
}
