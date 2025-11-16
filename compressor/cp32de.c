#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "_cp_sys.h"
#include "_cp_io.h"
#include "_cp_chain.h"
#include "_cp_base32.h"

int main(int argc, char *argv[]) {
    _cp_init();
    _cp_info("This program directly process and decompress compressed tag files. \
            You may not process data files that include compressed tag portion using this program.");
    if (argc < 2) {
        printf("Usage: cp32en <input_file> <output_file>\n");
        _cp_info("This program deencodes a single input .tgvz (Compressed Tag Vector) \
             into a decoded output .tgss (Storage Tag Source) \
             file using the custom base32 encoding.");
        _cp_finish();
    }
    if (argc < 3) {
        _cp_die("No input files provided.");
    }
    if (argc != 3) {
        _cp_die("Invalid number of arguments. Please provide exactly one input file and one output file.\
            User single quotes ('') for paths with spaces.");
    }
    char* last = argv[2];
    // Check last is not empty
    if (strlen(last) == 0) {
        _cp_die("Output file path is empty.");
    }
    // Get input file path
    char* input_path = argv[1];
    // Check if input file exists
    FILE* file = fopen(input_path, "rb");
    if (file == NULL) {
        _cp_die("Could not open input file.");
    }
    fclose(file);
    // Check if input file ends in .tgvz
    if (strlen(input_path) > 5) {
        const char* ext = input_path + strlen(input_path) - 5;
        if (strcmp(ext, ".tgvz") != 0) {
            printf("Warning: Input file %s does not have .tgvz extension. Proceeding anyway.\n", input_path);
        }
    }
    bool allocated_last = false;
    // If output file does not end with .tgss, add the extension
    if (strlen(last) < 5 || strcmp(last + strlen(last) - 5, ".tgss") != 0) {
        char* new_last = (char*)malloc(strlen(last) + 6);
        _cp_assertmem(new_last);
        strcpy(new_last, last);
        strcat(new_last, ".tgss");
        last = new_last;
        allocated_last = true;
    }
    // Read input file
    _cp_Buffer* input_buf = _cp_buffer_create();
    _cp_Buffer* output_buf = _cp_buffer_create();
    _cp_assertmem(input_buf);
    _cp_assertmem(output_buf);
    _cp_read_file_to_buffer(input_path, input_buf);
    _cp_remove_eof_if_exists(input_buf);
    _cp_remove_null_terminator_if_exists(input_buf);
    // Decode base32
    _cp_base32_decode(input_buf, 0, output_buf);
    _cp_remove_null_terminator_if_exists(output_buf);
    _cp_info("Decoding completed.");
    _cp_buffer_free(input_buf);
    // Write output file
    _cp_write_buffer_to_file(last, output_buf);
    _cp_buffer_free(output_buf);
    if (allocated_last) {
        free(last);
    }
    _cp_info("Done.");
    _cp_finish();
}