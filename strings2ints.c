#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "util.h"

typedef struct {
    char* string;
    int32_t code;
} Mapping;

int main(int argc, char** argv) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s MAPPINGFILE STRINGSFILE INTSFILE\n", argv[0]);
        exit(1);
    }

    const char* mapping_file = argv[1];
    const char* strings_file = argv[2];
    const char* ints_file = argv[3];

    FILE* mf = fopen(mapping_file, "rb");
    assert(mf != NULL);

    Mapping* mappings = NULL;
    size_t mapping_count = 0;
    while (1) {
        int32_t string_length;
        if (fread(&string_length, sizeof(int32_t), 1, mf) != 1) {
            break; 
        }

        char* string = malloc(string_length + 1);
        assert(string != NULL);
        assert(fread(string, sizeof(char), string_length, mf) == (size_t)string_length);
        string[string_length] = '\0';

        int32_t code;
        assert(fread(&code, sizeof(int32_t), 1, mf) == 1);

        mappings = realloc(mappings, (mapping_count + 1) * sizeof(Mapping));
        assert(mappings != NULL);

        mappings[mapping_count].string = string;
        mappings[mapping_count].code = code;
        mapping_count++;
    }
    fclose(mf);

    int num_lines;
    char** lines = read_lines(strings_file, &num_lines);

    FILE* out_file = fopen(ints_file, "wb");
    assert(out_file != NULL);

    assert(fwrite(&num_lines, sizeof(int32_t), 1, out_file) == 1);

    for (int i = 0; i < num_lines; i++) {
        int found = 0;

        for (size_t j = 0; j < mapping_count; j++) {
            if (strcmp(lines[i], mappings[j].string) == 0) {
                int32_t code = mappings[j].code;
                assert(fwrite(&code, sizeof(int32_t), 1, out_file) == 1);
                found = 1;
                break;
            }
        }

        if (!found) {
            fprintf(stderr, "Error: Line '%s' has no mapping.\n", lines[i]);
            exit(1);
        }
    }

    for (size_t j = 0; j < mapping_count; j++) {
        free(mappings[j].string);
    }
    free(mappings);

    for (int i = 0; i < num_lines; i++) {
        free(lines[i]);
    }
    free(lines);

    fclose(out_file);
    return 0;
}