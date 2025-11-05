#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "_cp_sys.h"

typedef unsigned char byte;
const uint16_t BLOCK_SIZE = 256;
const uint16_t DEFAULT_SIZE = 256 * 16;
const int DEBUG = 1;// Set to 1 to enable debug messages
const bool false = 0;
const bool true = 1;

// System
void _cp_init(){
    _cp_init_buffer_pool();
}

void _cp_die(const char* msg) {
    fprintf(stderr, "Fatal error: %s\n", msg);
    exit(EXIT_FAILURE);
}

void _cp_warn(const char* msg) {
    fprintf(stderr, "Warning: %s\n", msg);
}

void _cp_info(const char* msg) {
    fprintf(stdout, "Info: %s\n", msg);
}

void _cp_debug(const char* msg) {
    // Uncomment the next line to enable debug messages
    if (DEBUG) {
        fprintf(stdout, "Debug: %s\n", msg);
    }
}

void _cp_finish() {
    _cp_free_buffer_pool();
    fprintf(stdout, "Compression finished successfully.\n");
    exit(EXIT_SUCCESS);
}

// Memory buffer

_cp_Buffer** buffers = NULL;
size_t buffer_count = 0;
size_t POOL_SIZE = 1024;

void _cp_init_buffer_pool() {
    buffers = (_cp_Buffer**)malloc(sizeof(_cp_Buffer*) * POOL_SIZE);
    if (buffers == NULL) {
        _cp_die("Failed to initialize buffer pool. Compression cannot proceed.");
    }
    buffer_count = 0;
}

void _cp_free_buffer_pool() {
    for (size_t i = 0; i < buffer_count; i++) {
        free(buffers[i]->data);
        free(buffers[i]);
    }
    free(buffers);
    buffers = NULL;
    buffer_count = 0;
}

void _cp_push_buffer_to_pool(_cp_Buffer* buf) {
    if (buffer_count < POOL_SIZE) {
        buffers[buffer_count++] = buf;
    } else {
        // Pool full, free the buffer
        free(buf->data);
        free(buf);
    }
}

_cp_Buffer* _cp_pop_buffer_from_pool() {
    if (buffer_count > 0) {
        return buffers[--buffer_count];
    } else {
        return NULL;
    }
}

_cp_Buffer* _cp_buffer_create() {
    // Try to get from pool first
    _cp_Buffer* buf = _cp_pop_buffer_from_pool();
    if (buf != NULL) {
        return buf;
    } else {
        buf = (_cp_Buffer*)malloc(sizeof(_cp_Buffer));
        if (buf == NULL) {
            return NULL; // Allocation failed
        }
        buf->data = (byte*)malloc(DEFAULT_SIZE);
        if (buf->data == NULL) {
            free(buf);
            return NULL; // Allocation failed
        }
        buf->size = 0;
        buf->capacity = DEFAULT_SIZE;
        return buf;
    }
}

void _cp_buffer_free(_cp_Buffer* buf) {
    if (buf == NULL) return;
    if (buf->capacity == DEFAULT_SIZE) {
        _cp_push_buffer_to_pool(buf);
    } else if (buf->capacity > DEFAULT_SIZE) {
        // Shrink to default size and push to pool
        byte* new_data = (byte*)realloc(buf->data, DEFAULT_SIZE);
        if (new_data == NULL) {
            // Failed to shrink — free entirely to avoid corruption
            free(buf->data);
            free(buf);
            return;
        }
        buf->data = new_data;
        buf->capacity = DEFAULT_SIZE;
        _cp_push_buffer_to_pool(buf);
    } else {
        // Smaller than default size, free directly
        free(buf->data);
        free(buf);
    }
}

void _cp_buffer_clear(_cp_Buffer* buf) {
    if (buf == NULL) return;
    buf->size = 0;
}

_cp_Buffer* _cp_buffer_expand(_cp_Buffer* buf, const size_t min_size) {
    if (buf->size + min_size > buf->capacity) {
        size_t new_capacity = buf->capacity * 2;
        while (buf->size + min_size > new_capacity) {
            new_capacity *= 2;
        }
        byte* new_data = (byte*)realloc(buf->data, new_capacity);
        if (new_data == NULL) {
            return NULL; // Allocation failed
        }
        buf->data = new_data;
        buf->capacity = new_capacity;
    }
    return buf;
}

_cp_Buffer* _cp_buffer_double(_cp_Buffer* buf) {
    size_t new_capacity = buf->capacity * 2;
    byte* new_data = (byte*)realloc(buf->data, new_capacity);
    if (new_data == NULL) {
        return NULL; // Allocation failed
    }
    buf->data = new_data;
    buf->capacity = new_capacity;
    return buf;
}

_cp_Buffer* _cp_buffer_copy(const _cp_Buffer* src) {
    _cp_Buffer* dest = _cp_buffer_create();
    if (dest == NULL) {
        return NULL; // Allocation failed
    }
    if (_cp_buffer_expand(dest, src->size) == NULL) {
        _cp_buffer_free(dest);
        return NULL; // Allocation failed
    }
    memcpy(dest->data, src->data, src->size);
    dest->size = src->size;
    return dest;
}

size_t _cp_buffer_remaining_capacity(const _cp_Buffer* buf) {
    if (buf == NULL) return 0;
    return buf->capacity - buf->size;
}

char _cp_buffer_char_at(const _cp_Buffer* buf, const size_t index) {
    if (buf == NULL || index >= buf->size) return '\0';
    return (char)buf->data[index];
}

void _cp_buffer_set_char_at(const _cp_Buffer* buf, const size_t index, const char c) {
    if (buf == NULL || index >= buf->size) return;
    buf->data[index] = (byte)c;
}

void _cp_buffer_append_char(_cp_Buffer* buf, const char c) {
    if (buf == NULL) return;
    if (buf->size + 1 > buf->capacity) {
        if (_cp_buffer_expand(buf, 1) == NULL) {
            return; // Allocation failed
        }
    }
    buf->data[buf->size++] = (byte)c;
}

char* _cp_buffer_cstr(_cp_Buffer* buf) {
    if (buf == NULL) return NULL;
    // Ensure null-termination
    if (buf->size == buf->capacity) {
        if (_cp_buffer_expand(buf, 1) == NULL) {
            return NULL; // Allocation failed
        }
    }
    buf->data[buf->size] = '\0';
    return (char*)buf->data;
}

_cp_Buffer* _cp_buffer_from_cstr(const char* str) {
    if (str == NULL) return NULL;
    size_t len = strlen(str);
    _cp_Buffer* buf = _cp_buffer_create();
    if (buf == NULL) return NULL;
    if (_cp_buffer_expand(buf, len) == NULL) {
        _cp_buffer_free(buf);
        return NULL;
    }
    memcpy(buf->data, str, len);
    buf->size = len;
    return buf;
}

_cp_Buffer* _cp_buffer_from_cstr_inplace(const char* str) {
    if (str == NULL) return NULL;
    size_t len = strlen(str);
    _cp_Buffer* buf = _cp_buffer_create();
    if (buf == NULL) return NULL;
    if (_cp_buffer_expand(buf, len) == NULL) {
        _cp_buffer_free(buf);
        return NULL;
    }
    buf->data = (byte*)str; // Directly assign pointer
    buf->size = len;
    buf->capacity = len; // Capacity equals size
    return buf;
}

_cp_Buffer* _cp_buffer_concat(const _cp_Buffer* buf1, const _cp_Buffer* buf2) {
    if (buf1 == NULL || buf2 == NULL) return NULL;
    _cp_Buffer* bufn = _cp_buffer_create();
    if (bufn == NULL) return NULL;
    size_t total_size = buf1->size + buf2->size;
    if (_cp_buffer_expand(bufn, total_size) == NULL) {
        _cp_buffer_free(bufn);
        return NULL;
    }
    memcpy(bufn->data, buf1->data, buf1->size);
    memcpy(bufn->data + buf1->size, buf2->data, buf2->size);
    bufn->size = total_size;
    return bufn;
}

void _cp_buffer_concat_inplace(_cp_Buffer* dest, const _cp_Buffer* src) {
    if (dest == NULL || src == NULL) return;
    if (_cp_buffer_expand(dest, src->size) == NULL) {
        return; // Allocation failed
    }
    memcpy(dest->data + dest->size, src->data, src->size);
    dest->size += src->size;
}