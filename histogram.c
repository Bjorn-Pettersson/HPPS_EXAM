#include "histogram.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <omp.h>
#include <string.h>

static void histogram_sequential(int k, int n, int32_t *H, int32_t *samples) {

    for (int i = 0; i < n; i++) {
        int value = samples[i];
        if (value >= 0 && value < k) {
            H[value]++;
        }
    }
}

static void histogram_parallel_bins(int k, int n, int32_t *H, int32_t *samples) {

    #pragma omp parallel
    {
        int thread_id = omp_get_thread_num();
        int num_threads = omp_get_num_threads();
        
        int start_bin = thread_id * (k / num_threads);
        int end_bin = (thread_id + 1) * (k / num_threads);
        if (thread_id == num_threads - 1) {
            end_bin = k; 
        }

        int32_t *local_H = calloc(k, sizeof(int32_t));

        for (int i = 0; i < n; i++) {
            int value = samples[i];
            if (value >= start_bin && value < end_bin) {
                local_H[value]++;
            }
        }

        {
            for (int j = start_bin; j < end_bin; j++) {
                H[j] += local_H[j];
            }
        }
        
        free(local_H);
    }
}

static void histogram_parallel_samples(int k, int n, int32_t *H, int32_t *samples) {

    int num_threads = omp_get_max_threads(); 

    int32_t **local_histograms = malloc(num_threads * sizeof(int32_t *));
    for (int i = 0; i < num_threads; i++) {
        local_histograms[i] = calloc(k, sizeof(int32_t)); 
    }

    #pragma omp parallel
    {
        int thread_id = omp_get_thread_num();
        int chunk_size = (n + num_threads - 1) / num_threads; 
        int start = thread_id * chunk_size;
        int end = start + chunk_size;
        if (end > n) end = n;

        for (int i = start; i < end; i++) {
            int value = samples[i];
            if (value >= 0 && value < k) {
                local_histograms[thread_id][value]++;
            }
        }
    }

    for (int t = 0; t < num_threads; t++) {
        for (int j = 0; j < k; j++) {
            H[j] += local_histograms[t][j];
        }
        free(local_histograms[t]); 
    }
    free(local_histograms); 
}


struct algorithm algorithms[] = {
    { .name = "sequential", .f = &histogram_sequential },
    { .name = "parallel_bins", .f = &histogram_parallel_bins },
    { .name = "parallel_samples", .f = &histogram_parallel_samples }
};

const int num_algorithms = sizeof(algorithms) / sizeof(algorithms[0]);
