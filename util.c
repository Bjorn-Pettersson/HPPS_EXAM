#include "util.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <sys/time.h>
#include <errno.h>

double seconds(void) {
  struct timeval tv;
  gettimeofday(&tv, NULL); 
  return tv.tv_sec + tv.tv_usec/1000000.0;
}

int32_t* read_ints(const char* fname, int32_t* n) {
  FILE *f = fopen(fname, "r");
  assert(f != NULL);

  assert(fread(n, sizeof(int32_t), 1, f) == 1);

  int* p = calloc(*n, sizeof(int32_t));

  assert((int)fread(p, sizeof(int32_t), *n, f) == *n);

  fclose(f);

  return p;
}

void write_ints(const char* fname, int32_t n, int32_t* p) {
  FILE *f = fopen(fname, "w");
  assert(f != NULL);

  assert(fwrite(&n, sizeof(int32_t), 1, f) == 1);
  assert((int)fwrite(p, sizeof(int32_t), n, f) == n);

  fclose(f);
}

double* read_points(const char* fname, int* n) {
    FILE* f = fopen(fname, "rb");
    if (f == NULL) {
        fprintf(stderr, "Error: Could not open file %s for reading.\n", fname);
        return NULL;
    }

    if (fread(n, sizeof(int32_t), 1, f) != 1) {
        fprintf(stderr, "Error: Failed to read the number of points from file %s.\n", fname);
        fclose(f);
        return NULL;
    }

    double* points = (double*)malloc(2 * (*n) * sizeof(double));
    if (points == NULL) {
        fprintf(stderr, "Error: Memory allocation failed for points array.\n");
        fclose(f);
        return NULL;
    }

    if (fread(points, sizeof(double), 2 * (size_t)(*n), f) != 2 * (size_t)(*n)) {
        fprintf(stderr, "Error: Failed to read points data from file %s.\n", fname);
        free(points);
        fclose(f);
        return NULL;
    }

    fclose(f);
    return points;
}

void write_points(const char* fname, int n, double* p) {
    FILE* f = fopen(fname, "wb");
    if (f == NULL) {
        fprintf(stderr, "Error: Could not open file %s for writing.\n", fname);
        return;
    }

    if (fwrite(&n, sizeof(int32_t), 1, f) != 1) {
        fprintf(stderr, "Error: Failed to write the number of points to file %s.\n", fname);
        fclose(f);
        return;
    }

    if (fwrite(p, sizeof(double), 2 * (size_t)n, f) != 2 * (size_t)n) {
        fprintf(stderr, "Error: Failed to write points data to file %s.\n", fname);
        fclose(f);
        return;
    }

    fclose(f);
}

char** read_lines(const char* fname, int* n) {
    FILE* file = fopen(fname, "r");
    if (file == NULL) {
        fprintf(stderr, "Error: Unable to open file '%s' (%s)\n", fname, strerror(errno));
        return NULL;
    }

    size_t line_capacity = 256; // Initial buffer size for each line
    size_t lines_capacity = 10; // Initial number of lines to allocate
    char** lines = malloc(lines_capacity * sizeof(char*));
    if (lines == NULL) {
        fprintf(stderr, "Error: Memory allocation failed for lines array\n");
        fclose(file);
        return NULL;
    }

    *n = 0; // Number of lines read
    char* line = malloc(line_capacity * sizeof(char));
    if (line == NULL) {
        fprintf(stderr, "Error: Memory allocation failed for line buffer\n");
        free(lines);
        fclose(file);
        return NULL;
    }

    while (1) {
        size_t current_length = 0;

        while (1) {
            if (fgets(line + current_length, line_capacity - current_length, file) == NULL) {
                if (current_length == 0) {
                    free(line); // Free buffer if no data was read
                    fclose(file);
                    return *n > 0 ? lines : NULL; // Return NULL if no lines were read
                }
                break; // Partial line read at EOF
            }

            current_length = strlen(line);

            // Check if the line ends with a newline character
            if (current_length > 0 && line[current_length - 1] == '\n') {
                line[current_length - 1] = '\0'; // Remove newline
                break;
            }

            // Expand the line buffer if necessary
            line_capacity *= 2;
            char* new_line = realloc(line, line_capacity * sizeof(char));
            if (new_line == NULL) {
                fprintf(stderr, "Error: Memory reallocation failed for line buffer\n");
                free(line);
                for (int i = 0; i < *n; i++) {
                    free(lines[i]);
                }
                free(lines);
                fclose(file);
                return NULL;
            }
            line = new_line;
        }

        if (*n >= (int)lines_capacity) {
            lines_capacity *= 2;
            char** new_lines = realloc(lines, lines_capacity * sizeof(char*));
            if (new_lines == NULL) {
                fprintf(stderr, "Error: Memory reallocation failed for lines array\n");
                free(line);
                for (int i = 0; i < *n; i++) {
                    free(lines[i]);
                }
                free(lines);
                fclose(file);
                return NULL;
            }
            lines = new_lines;
        }

        lines[*n] = strdup(line);
        if (lines[*n] == NULL) {
            fprintf(stderr, "Error: Memory allocation failed for line duplication\n");
            free(line);
            for (int i = 0; i < *n; i++) {
                free(lines[i]);
            }
            free(lines);
            fclose(file);
            return NULL;
        }

        (*n)++;
    }

    free(line);
    fclose(file);
    return lines;
}

