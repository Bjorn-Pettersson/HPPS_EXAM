#include "dist-histogram.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <omp.h>

static double dist(double ax, double ay, double bx, double by) {
  double dx = ax - bx;
  double dy = ay - by;

  return sqrt(dx*dx + dy*dy);
}

static void histogram_sequential(int k, int n, int32_t *H, double *points) {

    for (int i = 0; i < n; i++) {
        // Get point a
        double ax = points[2 * i];
        double ay = points[2 * i + 1];

        for (int j = i + 1; j < n; j++) { 
            // Get point b
            double bx = points[2 * j];
            double by = points[2 * j + 1];

            double distance = dist(ax, ay, bx, by);
            int rounded_down_dist = (int)distance;

            if (rounded_down_dist >= 0 && rounded_down_dist < k) {
                H[rounded_down_dist] += 2;  
            }
        }
    }
}

static void histogram_parallel(int k, int n, int32_t *H, double *points) {

    int num_threads;
    int32_t **local_histograms;

    #pragma omp parallel
    {
        int thread_id = omp_get_thread_num();
        num_threads = omp_get_num_threads();

        #pragma omp single
        {
            local_histograms = (int32_t **)malloc(num_threads * sizeof(int32_t *));
            for (int i = 0; i < num_threads; i++) {
                local_histograms[i] = (int32_t *)calloc(k, sizeof(int32_t));
            }
        }

        #pragma omp for schedule(dynamic)
        for (int i = 0; i < n; i++) {
            double ax = points[2 * i];
            double ay = points[2 * i + 1];
            
            for (int j = i + 1; j < n; j++) {
                double bx = points[2 * j];
                double by = points[2 * j + 1];

                double distance = dist(ax, ay, bx, by);
                int bin = (int)distance;

                if (bin >= 0 && bin < k) {
                    local_histograms[thread_id][bin]++;
                }
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
  { .name = "parallel", .f = &histogram_parallel },
};

const int num_algorithms = sizeof(algorithms) / sizeof(algorithms[0]);
