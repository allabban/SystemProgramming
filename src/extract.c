#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "extract.h"

int extract_archive(const char *archive_filename, const char *target_directory) {
    FILE *in_file = fopen(archive_filename, "r");
    if (!in_file) {
        printf("Error: Cannot open archive '%s'.\n", archive_filename);
        return 1;
    }
    // STEP 1: Read the 10-byte header
    // ---------------------------------------------------------
    char header_size_str[11] = {0};
    if (fread(header_size_str, 1, 10, in_file) != 10) {
        printf("Archive file is corrupted!\n");
        fclose(in_file);
        return 1;
    }

    int metadata_length = atoi(header_size_str);
    if (metadata_length <= 0) {
        printf("Archive file is corrupted!\n");
        fclose(in_file);
        return 1;
    }

    // ---------------------------------------------------------
    // STEP 2: Read the Metadata section
    // ---------------------------------------------------------
    char *metadata = malloc(metadata_length + 1);
    if (fread(metadata, 1, metadata_length, in_file) != (size_t)metadata_length) {
        printf("Archive file is corrupted!\n");
        free(metadata);
        fclose(in_file);
        return 1;
    }
    metadata[metadata_length] = '\0'; // Null-terminate the string

    // ---------------------------------------------------------
    // STEP 3: Handle the Target Directory
    // -----------------------------------
    char base_path[512] = "";
    if (target_directory != NULL) {
        struct stat st = {0};
        // Check if directory exists; if not, create it with standard 0777 permissions
        if (stat(target_directory, &st) == -1) {
            if (mkdir(target_directory, 0777) != 0) {
                printf("Error: Could not create directory '%s'.\n", target_directory);
                free(metadata);
                fclose(in_file);
                return 1;
            }
        }
        // Format the base path to ensure it ends with a slash ("d1/")
        snprintf(base_path, sizeof(base_path), "%s/", target_directory);
    }
    // ---------------------------------------------------------
    // STEP 4: Parse Metadata & Extract Files
    // --------------------------------------
    char *ptr = metadata;
    // Loop through the metadata string looking for our '|' delimiters
    while (*ptr == '|') {
        ptr++; // Skip the initial '|'
        
        char filename[256] = {0};
        int permissions;
        long size;
        //Extract Filename
        char *comma1 = strchr(ptr, ',');
        if (!comma1) break;
        strncpy(filename, ptr, comma1 - ptr);
        
        //Extract Permissions
        ptr = comma1 + 1;
        char *comma2 = strchr(ptr, ',');
        if (!comma2) break;
        sscanf(ptr, "%o", &permissions); 

        // 4c. Extract Size
        ptr = comma2 + 1;
        char *pipe = strchr(ptr, '|');
        if (!pipe) break;
        sscanf(ptr, "%ld", &size);

        // Construct the final output path (e.g., "d1/t1.txt" or just "t1.txt")
        char full_out_path[1024];
        snprintf(full_out_path, sizeof(full_out_path), "%s%s", base_path, filename);

        // 4d. Write Data
        FILE *out_file = fopen(full_out_path, "w");
        if (!out_file) {
            printf("Error: Cannot create extracted file '%s'.\n", full_out_path);
            break;
        }

        // Copy exactly 'size' bytes from the archive to the new file
        char buffer[4096];
        long bytes_left = size;
        while (bytes_left > 0) {
            size_t to_read = (bytes_left < (long)sizeof(buffer)) ? (size_t)bytes_left : sizeof(buffer);
            size_t bytes_read = fread(buffer, 1, to_read, in_file);
            if (bytes_read == 0) break; // Unexpected end of file
            fwrite(buffer, 1, bytes_read, out_file);
            bytes_left -= bytes_read;
        }
        fclose(out_file);

        // 4e. Restore Original Permissions
        chmod(full_out_path, permissions);
        ptr = pipe + 1; // Move the pointer to the start of the next file record
    }

    free(metadata);
    fclose(in_file);

    if (target_directory) {
        printf("%s files extracted.\n", target_directory);
    } else {
        printf("Files extracted to current directory.\n");
    }

    return 0;
}