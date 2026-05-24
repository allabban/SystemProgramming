#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "archive.h"

#define MAX_TOTAL_SIZE (200 * 1024 * 1024) // 200 MB limit

// file contains only standard ASCII characters
int is_text_file(const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (!file) return 0; // Cannot open file
    int ch;
    while ((ch = fgetc(file)) != EOF) {
        if (ch > 127 || ch == 0) {
            fclose(file);
            return 0; 
        }
    }
    
    fclose(file);
    return 1;
}

int archive_files(char *input_files[], int file_count, const char *output_filename) {
    long total_size = 0;
    struct stat file_stats[32]; 
    // STEP 1: VALIDATION
    // -----------------
    for (int i = 0; i < file_count; i++) {
        if (stat(input_files[i], &file_stats[i]) != 0) {
            printf("Error: Could not read file '%s'. Does it exist?\n", input_files[i]);
            return 1;
        }

        if (!is_text_file(input_files[i])) {
            printf("%s input file has an incompatible format!\n", input_files[i]);
            return 1;
        }

        total_size += file_stats[i].st_size;
    }

    if (total_size > MAX_TOTAL_SIZE) {
        printf("Error: Total size of input files exceeds 200 MB.\n");
        return 1;
    }

    // STEP 2: METADATA
    // -----------------
    char metadata[8192] = ""; 
    char record[256];
    for (int i = 0; i < file_count; i++) {
        snprintf(record, sizeof(record), "|%s,%o,%ld|", 
                input_files[i], 
                file_stats[i].st_mode & 0777, 
                (long)file_stats[i].st_size);
        strcat(metadata, record);
    }
    int metadata_length = strlen(metadata);

    // -----------------------------------
    // STEP 3: WRITE TO ARCHIVE
    // ------------------------------------------
    FILE *out_file = fopen(output_filename, "w"); 
    if (!out_file) {
        printf("Error: Cannot create output archive '%s'.\n", output_filename);
        return 1;
    }

    // Write the 10-byte size header (left-aligned, padded with spaces)
    char header_size[11];
    snprintf(header_size, sizeof(header_size), "%-10d", metadata_length); 
    fwrite(header_size, 1, 10, out_file);
    fwrite(metadata, 1, metadata_length, out_file);
    // Append the contents of each file
    for (int i = 0; i < file_count; i++) {
        FILE *in_file = fopen(input_files[i], "r");
        if (!in_file) {
            printf("Error: Could not read '%s' during merging.\n", input_files[i]);
            fclose(out_file);
            return 1;
        }
        char buffer[4096];
        size_t bytes_read;
        while ((bytes_read = fread(buffer, 1, sizeof(buffer), in_file)) > 0) {
            fwrite(buffer, 1, bytes_read, out_file);
        }
        fclose(in_file);
    }
    fclose(out_file);
    
    printf("Files merged successfully.\n");
    
    return 0;
}