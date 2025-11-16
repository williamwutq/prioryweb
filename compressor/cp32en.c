#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "_cp_sys.h"
#include "_cp_io.h"
#include "_cp_chain.h"
#include "_cp_base32.h"

int main(int argc, char *argv[]) {
    _cp_init();
    _cp_info("This program directly process and compress tag files. \
            If you want to add them into data files, you must manually link them with the data files.");
    if (argc < 2) {
        printf("Usage: cp32en <input_file1> <input_file2> ... <output_file>");
        _cp_info("This program encodes multiple input .tgss (Storage Tag Source)\
             into a single compressed output .tgvz (Compressed Tag Vector) \
             file using the custom base32 encoding.");
        _cp_finish();
    }
    if (argc < 3) {
        _cp_die("No input files provided.");
    }
    char* last = argv[argc - 1];
    // Check last is not empty
    if (strlen(last) == 0) {
        _cp_die("Output file path is empty.");
    }
    // If output file does not end with .tgvz, add the extension
    bool allocated_last = false;
    if (strlen(last) < 5 || strcmp(last + strlen(last) - 5, ".tgvz") != 0) {
        char* new_last = (char*)malloc(strlen(last) + 6);
        if (new_last == NULL) {
            _cp_die("Memory allocation failed for output file path.");
        }
        strcpy(new_last, last);
        strcat(new_last, ".tgvz");
        last = new_last;
        allocated_last = true;
    }
    _cp_Buffer_Chain* inputs = _cp_buffer_chain_create();
    _cp_assertmem(inputs);
    // Process for each file provided
    int c = 0;
    for (int i = 1; i < argc - 1; i++) {
        const char* file_path = argv[i];
        // Check if file exists
        FILE* file = fopen(file_path, "rb");
        if (file == NULL) {
            printf("Could not open file: %s\n", file_path);
            continue;
        }
        fclose(file);
        // Check if file ends in .tgss
        if (strlen(file_path) > 5) {
            const char* ext = file_path + strlen(file_path) - 5;
            if (strcmp(ext, ".tgss") != 0) {
                printf("Warning: File %s does not have .tgss extension. Proceeding anyway.\n", file_path);
            }
        }
        // Make new buffer
        _cp_Buffer* input_buf = _cp_buffer_create();
        _cp_assertmem(input_buf);
        _cp_read_file_to_buffer(file_path, input_buf);
        bool wrong_format = !_cp_base32_check_format(input_buf, 0, input_buf->size, true);
        if (wrong_format) _cp_info("File is not well-formatted, correcting by first converting to lowercase then convert invalid characters into spaces.");
        _cp_buffer_to_lowercase(input_buf);
        _cp_base32_correct_format(input_buf, 0, input_buf->size, true);
        if (wrong_format) {
            printf("Corrected content of file: %s\n", file_path);
            _cp_buffer_print(input_buf);
        }
        _cp_remove_eof_if_exists(input_buf);
        _cp_remove_null_terminator_if_exists(input_buf);
        // Append to chain
        _cp_buffer_chain_pusht(inputs, input_buf);
        c ++;
    }
    if (c == 0) {
        _cp_die("No valid input files provided.");
    }
    printf("Encoding %d files...\n", c);
    // Process all inputs
    for (_cp_Buffer_ChainNode* node = inputs->head; node != NULL; node = node->next) {
        _cp_Buffer* encoded_buf = _cp_buffer_create();
        _cp_assertmem(encoded_buf);
        _cp_base32_encode(encoded_buf, node->buffer, 0);
        // End of block marker. Since this is reserved character, it is safe to use.
        _cp_buffer_append_char(encoded_buf, (char)0xFF);
        _cp_buffer_append_char(encoded_buf, (char)0xFF);
        node->buffer = encoded_buf;
    }
    printf("Finished encoding. Writing to output file %s...\n", last);
    // Write to output file
    _cp_Buffer* result = _cp_buffer_chain_embuffer(inputs); // This frees all things
    _cp_write_buffer_to_file(last, result);
    if (allocated_last) {
        free(last);
    }
    _cp_buffer_free(result);
    printf("Done.\n");
    _cp_finish();
    return 0;
}