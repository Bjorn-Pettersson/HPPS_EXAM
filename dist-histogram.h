#pragma once

#include <stdint.h>

// A pointer to a function that can compute histograms with 'k'
// buckets on 'n' points.
typedef void (*histogram_fn)(int k, int n, int32_t *H, double *points);

struct algorithm {
  const char* name;
  histogram_fn f;
};

extern struct algorithm algorithms[];

extern const int num_algorithms;
