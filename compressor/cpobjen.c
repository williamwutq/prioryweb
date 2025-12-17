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
    if (argc < 2) {
        printf("Usage: cpobjen <input_file> <output_file>\n");
        _cp_info("This program encode an compressed web object for compact transmission.");
        goto finish;
    }
    finish:
    _cp_finish();
}