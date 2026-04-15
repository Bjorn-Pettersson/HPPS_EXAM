#pragma once

#include <stdint.h>

// A pointer to a function that can compute histograms with 'k'
// buckets on 'n' samples. It may be assumed that the array H is
// already zero-initialised.
typedef void (*histogram_fn)(int k, int n, int32_t *H, int32_t *samples);

struct algorithm {
  const char* name;
  histogram_fn f;
};

extern struct algorithm algorithms[];

extern const int num_algorithms;
