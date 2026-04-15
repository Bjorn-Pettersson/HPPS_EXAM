#include "util.h"
#include "histogram.h"

#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

int main(int argc, char** argv) {
  if (argc != 6) {
    fprintf(stderr, "Usage: %s RUNS ALGORITHM K SAMPLESFILE HISTOGRAMFILE\n", argv[0]);
    exit(1);
  }

  int runs = atoi(argv[1]);
  const char *algorithm = argv[2];
  int k = atoi(argv[3]);
  const char *ints_fname = argv[4];
  const char *histogram_fname = argv[5];

  histogram_fn histogram_f = NULL;

  for (int i = 0; i < num_algorithms; i++) {
    if (strcmp(algorithms[i].name, algorithm) == 0) {
      histogram_f = algorithms[i].f;
    }
  }

  if (histogram_f == NULL) {
    fprintf(stderr, "Unknown algorithm: %s\n", algorithm);
    exit(1);
  }

  int n;
  int32_t *ints = read_ints(ints_fname, &n);
  int32_t *H = calloc(k, sizeof(int));

  printf("k=%d n=%d algorithm=%s\n", k, n, algorithm);
  printf("Mean runtime of %d runs: ", runs);
  fflush(stdout);

  double elapsed = 0;
  for (int r = 0; r < runs; r++) {
    memset(H, 0, k*sizeof(int32_t));
    double bef = seconds();
    histogram_f(k, n, H, ints);
    double aft = seconds();
    elapsed += aft-bef;
  }

  printf("%f\n", elapsed/runs);
  write_ints(histogram_fname, k, H);

  free(ints);
  free(H);
}
