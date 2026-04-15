#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "util.h"

int main(int argc, char** argv) {
  if (argc != 4) {
    fprintf(stderr, "Usage: %s N STRIDE OUTFILE\n", argv[0]);
    exit(1);
  }

  int n = atoi(argv[1]);
  double stride = strtod(argv[2], NULL);
  char* outfile = argv[3];

  double *points = calloc(n*2, sizeof(double));

  int edge = ceil(sqrt(n));

  int l = 0;
  for (int i = 0; i < edge; i++) {
    for (int j = 0; j < edge; j++) {
      if (l == n) {
        break;
      }
      points[l*2] = i*stride;
      points[l*2+1] = j*stride;
      l++;
    }
  }

  write_points(outfile, n, points);

  free(points);
}
