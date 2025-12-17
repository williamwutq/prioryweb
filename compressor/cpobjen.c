#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "_cp_sys.h"
#include "_cp_io.h"
#include "_cp_chain.h"
#include "_cp_base32.h"
#include "_cp_base64.h"

int main(int argc, char *argv[]) {
    _cp_init();
    // Processing Arguments
    if (argc < 2) {
        printf("Usage: cpobjen <input_file> <output_file>\n");
        _cp_info("This program encode an compressed web object for compact transmission.");
        goto finish;
    }
    if (argc < 3) _cp_die("No input files provided.");
    if (argc > 3) _cp_die("Invalid number of arguments. Please provide exactly one input file and one output file.\
        Use single quotes ('') for paths with spaces.");
    // Check path and file
    char* in_path = argv[1]; char* out_path = argv[2];
    if (strlen(out_path) == 0) _cp_die("Output file path is empty.");
    FILE* file = fopen(in_path, "rb");
    if (file == NULL) {
        fclose(file);
        _cp_die("Could not open input file.");
    }
    fclose(file);
    // Read input file
    _cp_Buffer* inputf_buf = _cp_buffer_create();
    if (inputf_buf == NULL) _cp_die("Allocation Failed");
    _cp_Buffer* outputf_buf = _cp_buffer_create();
    if (outputf_buf == NULL) {
        _cp_buffer_free(inputf_buf);
        _cp_die("Allocation Failed");
    }
    _cp_Buffer_Chain* major_chain = _cp_buffer_chain_create();
    if (major_chain == NULL) {
        _cp_buffer_free(inputf_buf);
        _cp_buffer_free(outputf_buf);
        _cp_die("Allocation Failed");
    }
    _cp_read_file_to_buffer(in_path, inputf_buf);
    _cp_remove_null_terminator_if_exists(inputf_buf);
    _cp_remove_eof_if_exists(in_path);
    _cp_remove_null_terminator_if_exists(inputf_buf);
    // Write to output file
    _cp_write_buffer_to_file(out_path, outputf_buf);
freeall:
    _cp_buffer_chain_free(major_chain);
    _cp_buffer_free(outputf_buf);
    _cp_buffer_free(inputf_buf);
finish:
    _cp_finish();
}