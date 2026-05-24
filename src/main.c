#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "archive.h" 
#include "extract.h" 
void print_usage() {
    printf("Usage:\n");
    printf("  Archive: tarsau -b file1 file2 ... [-o output.sau]\n");
    printf("  Extract: tarsau -a archive.sau [target_directory]\n");
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        print_usage();
        return 1;
    }
    // ARCHIVE MODE (-b)
    // ------------------------------
    if (strcmp(argv[1], "-b") == 0) {
        char *output_filename = "a.sau"; // Default archive name
        int input_files_count = 0;
        char *input_files[32]; // Maximum of 32 input files
        for (int i = 2; i < argc; i++) {
            if (strcmp(argv[i], "-o") == 0) {
                // If -o is found, the next argument is the output filename
                if (i + 1 < argc) {
                    output_filename = argv[i + 1];
                    break; // Stop collecting input files
                } else {
                    printf("Error: -o parameter must be followed by an archive filename.\n");
                    return 1;
                }
            } else {
                if (input_files_count < 32) {
                    input_files[input_files_count++] = argv[i];
                } else {
                    printf("Error: A maximum of 32 input files is allowed.\n");
                    return 1;
                }
            }
        }

        if (input_files_count == 0) {
            printf("Error: No input files specified for archiving.\n");
            return 1;
        }

        printf("Archive Mode: %d file(s) will be merged into '%s'.\n", input_files_count, output_filename);
        
        // Call the archive logic
        archive_files(input_files, input_files_count, output_filename);

    // -------------------------------------------------------------------
    // --------------------------------------------------------
    // -----------------------------------------
    // EXTRACT MODE (-a)
    // -----------------
    } else if (strcmp(argv[1], "-a") == 0) {
        // -a parameter can take a maximum of 2 additional parameters
        if (argc > 4) { 
            printf("Error: -a parameter takes a maximum of 2 additional arguments.\n");
            return 1;
        }

        char *archive_filename = argv[2]; // First parameter must be the archive file
        
        // Validation: Check if the file has a .sau extension
        char *ext = strrchr(archive_filename, '.');
        if (!ext || strcmp(ext, ".sau") != 0) {
            printf("Arşiv dosyası uygunsuz veya bozuk! File contains invalid extension!\n");
            return 1;
        }
        char *target_directory = NULL;
        if (argc == 4) {
            target_directory = argv[3]; 
        }

        printf("Extract Mode: Archive '%s' ", archive_filename);
        if (target_directory) {
            printf("will be extracted to directory '%s'.\n", target_directory);
        } else {
            printf("will be extracted to the current directory.\n");
        }

        extract_archive(archive_filename, target_directory);

    } else {
        printf("Error: Invalid operation flag. Please use -b or -a.\n");
        print_usage();
        return 1;
    }

    return 0;
}