#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "_cp_sys.h"

typedef unsigned char byte;
const uint16_t BLOCK_SIZE = 256;
const uint16_t DEFAULT_SIZE = 256 * 16;
const int DEBUG = 1;// Set to 1 to enable debug messages

// System
/**
 * Initializes the compression system.
 */
void _cp_init(){
    _cp_init_buffer_pool();
}

/**
 * Outputs a fatal error message to stderr and exits the program.
 */
void _cp_die(const char* msg) {
    fprintf(stderr, "Fatal error: %s\n", msg);
    exit(EXIT_FAILURE);
}

/**
 * Outputs a warning message to stderr.
 */
void _cp_warn(const char* msg) {
    fprintf(stderr, "Warning: %s\n", msg);
}

/**
 * Outputs an informational message to stdout.
 */
void _cp_info(const char* msg) {
    fprintf(stdout, "Info: %s\n", msg);
}

/**
 * Outputs a debug message to stdout.
 */
void _cp_debug(const char* msg) {
    // Uncomment the next line to enable debug messages
    if (DEBUG) {
        fprintf(stdout, "Debug: %s\n", msg);
    }
}

/**
 * Finalizes the compression process, frees resources, and exits successfully.
 */
void _cp_finish() {
    _cp_free_buffer_pool();
    fprintf(stdout, "Compression finished successfully.\n");
    exit(EXIT_SUCCESS);
}

// Memory buffer

_cp_Buffer** buffers = NULL;
size_t buffer_count = 0;
size_t POOL_SIZE = 1024;

/**
 * Initializes the buffer pool.
 */
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

/**
 * Pushes a buffer back into the pool for reuse.
 * If the pool is full, the buffer is freed.
 */
void _cp_push_buffer_to_pool(_cp_Buffer* buf) {
    if (buffer_count < POOL_SIZE) {
        buffers[buffer_count++] = buf;
    } else {
        // Pool full, free the buffer
        free(buf->data);
        free(buf);
    }
}

/**
 * Pops a buffer from the pool. If the pool is empty, returns NULL.
 */
void _cp_pop_buffer_from_pool(_cp_Buffer* buf) {
    if (buffer_count > 0) {
        buf = buffers[--buffer_count];
    } else {
        buf = NULL;
    }
}

/**
 * Creates a new buffer. It first attempts to retrieve a buffer from the pool.
 * If none are available, it allocates a new buffer with DEFAULT_SIZE capacity.
 */
_cp_Buffer* _cp_buffer_create() {
    // Try to get from pool first
    _cp_Buffer* buf = NULL;
    _cp_pop_buffer_from_pool(buf);
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

/**
 * Frees the buffer. If the buffer's capacity is equal to DEFAULT_SIZE,
 * it is returned to the pool for reuse. If larger, it is shrunk to
 * DEFAULT_SIZE before being returned to the pool. If smaller, it is
 * freed directly.
 */
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

/**
 * Ensures the buffer has enough capacity to accommodate additional data of size min_size.
 * If not, it expands the buffer's capacity.
 */
_cp_Buffer* _cp_buffer_expand(_cp_Buffer* buf, size_t min_size) {
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

_cp_Buffer* _cp_buffer_copy(_cp_Buffer* src) {
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