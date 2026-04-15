#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "util.h"

int main(int argc, char** argv) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s STRINGSFILE MAPPINGFILE\n", argv[0]);
        return 1;
    }

    const char* strings_file = argv[1];
    const char* mapping_file = argv[2];

    int num_lines = 0;
    char** lines = read_lines(strings_file, &num_lines);
    if (!lines || num_lines <= 0) {
        fprintf(stderr, "Failed to read lines from file: %s\n", strings_file);
        return 2;
    }

    char** unique_strings = malloc(num_lines * sizeof(char*));
    if (!unique_strings) {
        fprintf(stderr, "Failed to allocate memory for unique strings.\n");
        for (int i = 0; i < num_lines; i++) {
            free(lines[i]);
        }
        free(lines);
        return 3;
    }

    int num_unique = 0;

    for (int i = 0; i < num_lines; i++) {
        int is_unique = 1;
        for (int j = 0; j < num_unique; j++) {
            if (strcmp(lines[i], unique_strings[j]) == 0) {
                is_unique = 0;
                break;
            }
        }
        if (is_unique) {
            unique_strings[num_unique] = strdup(lines[i]);
            if (!unique_strings[num_unique]) {
                fprintf(stderr, "Failed to allocate memory for unique string.\n");
                for (int k = 0; k < num_unique; k++) {
                    free(unique_strings[k]);
                }
                free(unique_strings);
                for (int k = 0; k < num_lines; k++) {
                    free(lines[k]);
                }
                free(lines);
                return 4;
            }
            num_unique++;
        }
    }

    FILE* out_file = fopen(mapping_file, "wb");
    if (!out_file) {
        fprintf(stderr, "Failed to open file for writing: %s\n", mapping_file);
        for (int i = 0; i < num_unique; i++) {
            free(unique_strings[i]);
        }
        free(unique_strings);
        for (int i = 0; i < num_lines; i++) {
            free(lines[i]);
        }
        free(lines);
        return 5;
    }

    for (int i = 0; i < num_unique; i++) {
        int32_t string_length = (int32_t)strlen(unique_strings[i]);
        if (fwrite(&string_length, sizeof(int32_t), 1, out_file) != 1 ||
            fwrite(unique_strings[i], sizeof(char), (size_t)string_length, out_file) != (size_t)string_length ||
            fwrite(&i, sizeof(int32_t), 1, out_file) != 1) {
            fprintf(stderr, "Failed to write mapping to file.\n");
            fclose(out_file);
            for (int j = 0; j < num_unique; j++) {
                free(unique_strings[j]);
            }
            free(unique_strings);
            for (int j = 0; j < num_lines; j++) {
                free(lines[j]);
            }
            free(lines);
            return 6;
        }

    }

    free(unique_strings);
    for (int i = 0; i < num_lines; i++) {
        free(lines[i]);
    }
    free(lines);

    fclose(out_file);
    return 0;
}
