#include "_cp_sys.h"
#include "_cp_io.h"
#include <stdio.h>
#include <stdlib.h>

void _cp_assert_file_exists(const char* filepath){
    FILE* file = fopen(filepath, "rb");
    if (file == NULL) {
        _cp_die("File does not exist.");
    }
    fclose(file);
}

void _cp_read_file_to_buffer(const char* filepath, _cp_Buffer* buf) {
    if (buf == NULL || filepath == NULL) {
        return;
    }
    FILE* file = fopen(filepath, "rb");
    if (file == NULL) {
        return;
    }
    fseek(file, 0, SEEK_END);
    long filesize = ftell(file);
    fseek(file, 0, SEEK_SET);
    if (_cp_buffer_expand(buf, (size_t)filesize) == NULL) {
        fclose(file);
        _cp_die("Failed to expand buffer for file reading.");
    }
    buf->size = 0; // Reset size before reading
    fread(buf->data, 1, (size_t)filesize, file);
    buf->size = (size_t)filesize;
    fclose(file);
}

void _cp_append_file_to_buffer(const char* filepath, _cp_Buffer* buf) {
    if (buf == NULL || filepath == NULL) {
        return;
    }
    FILE* file = fopen(filepath, "rb");
    if (file == NULL) {
        return;
    }
    fseek(file, 0, SEEK_END);
    long filesize = ftell(file);
    fseek(file, 0, SEEK_SET);
    if (_cp_buffer_expand(buf, (size_t)filesize + buf->size) == NULL) {
        fclose(file);
        _cp_die("Failed to expand buffer for file appending.");
    }
    fread(buf->data + buf->size, 1, (size_t)filesize, file);
    buf->size += (size_t)filesize;
    fclose(file);
}

void _cp_write_buffer_to_file(const char* filepath, const _cp_Buffer* buf) {
    if (buf == NULL || filepath == NULL) {
        return;
    }
    FILE* file = fopen(filepath, "wb");
    if (file == NULL) {
        return;
    }
    fwrite(buf->data, 1, buf->size, file);
    fclose(file);
}

void _cp_append_buffer_to_file(const char* filepath, const _cp_Buffer* buf) {
    if (buf == NULL || filepath == NULL) {
        return;
    }
    FILE* file = fopen(filepath, "ab");
    if (file == NULL) {
        return;
    }
    fwrite(buf->data, 1, buf->size, file);
    fclose(file);
}

void _cp_read_stdin_to_buffer(_cp_Buffer* buf) {
    if (buf == NULL) {
        return;
    }
    int ch;
    while ((ch = getchar()) != EOF && ch != '\x04') { // Ctrl-D is ASCII 4
        _cp_buffer_append_char(buf, (char)ch);
    }
}

void _cp_write_buffer_to_stdout(const _cp_Buffer* buf) {
    if (buf == NULL) {
        return; // Nothing to write
    }
    fwrite(buf->data, 1, buf->size, stdout);
}

void _cp_remove_null_terminator_if_exists(_cp_Buffer* buf) {
    if (buf == NULL || buf->size == 0) {
        return;
    }
    if (buf->data[buf->size - 1] == '\0') {
        buf->size--;
    }
}

void _cp_remove_null_terminators(_cp_Buffer* buf) {
    if (buf == NULL || buf->size == 0) {
        return;
    }
    while (buf->size > 0 && buf->data[buf->size - 1] == '\0') {
        buf->size--;
    }
}

void _cp_add_null_terminator_if_not_exists(_cp_Buffer* buf) {
    if (buf == NULL) {
        return;
    }
    if (buf->size == 0 || buf->data[buf->size - 1] != '\0') {
        if (_cp_buffer_expand(buf, buf->size + 1) == NULL) {
            _cp_die("Failed to expand buffer for null terminator.");
        }
        buf->data[buf->size] = '\0';
        buf->size++;
    }
}

void _cp_remove_eof_if_exists(_cp_Buffer* buf) {
    if (buf == NULL || buf->size == 0) {
        return;
    }
    if (buf->data[buf->size - 1] == '\x1A') { // EOF character in Windows
        buf->size--;
    }
}

void _cp_add_eof_if_not_exists(_cp_Buffer* buf) {
    if (buf == NULL) {
        return;
    }
    if (buf->size == 0 || buf->data[buf->size - 1] != '\x1A') { // EOF character in Windows
        if (_cp_buffer_expand(buf, buf->size + 1) == NULL) {
            _cp_die("Failed to expand buffer for EOF character.");
        }
        buf->data[buf->size] = '\x1A';
        buf->size++;
    }
}