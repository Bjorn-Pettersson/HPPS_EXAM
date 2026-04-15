#pragma once

#include <stdint.h>

// Return the number of seconds since some unspecified time. Timing
// can be done by calling this function multiple times and subtracting
// the return values.
double seconds(void);

// Read ints file from file of given name. Returns pointer to integer
// array and stores number of integers read in *n.
int32_t* read_ints(const char* fname, int* n);

// Write ints file containing given integers.
void write_ints(const char* fname, int n, int32_t* p);

// Read points file. Returns pointer to array and stores the number of
// points read in *n.
double* read_points(const char* fname, int* n);

// Writes n points to a points file by the given name.
void write_points(const char* fname, int n, double* p);

// Read the lines of the given file. Returns an array of lines and
// stores the number of lines in *n.
char** read_lines(const char* fname, int* n);
