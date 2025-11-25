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
const compare_t inf_larger = 2;
const compare_t larger = 1;
const compare_t equal = 0;
const compare_t smaller = -1;
const compare_t inf_smaller = -2;
const compare_t undefined = -128;

bool is_larger(const compare_t cmp) {
    return cmp == larger || cmp == inf_larger;
}
bool is_smaller(const compare_t cmp) {
    return cmp == smaller || cmp == inf_smaller;
}

// System
static void _cp_clean() {
    _cp_free_buffer_pool();
}

void _cp_init(){
    _cp_init_buffer_pool();
}

void _cp_die(const char* msg) {
    fprintf(stderr, "Fatal error: %s\n", msg);
    _cp_clean();
    exit(EXIT_FAILURE);
}

void _cp_warn(const char* msg) {
    fprintf(stderr, "Warning: %s\n", msg);
}

void _cp_info(const char* msg) {
    fprintf(stdout, "Info: %s\n", msg);
}

void _cp_debug(const char* msg) {
    if (DEBUG) {
        fprintf(stdout, "Debug: %s\n", msg);
    }
}

void _cp_finish() {
    _cp_clean();
    fprintf(stdout, "Compression finished successfully.\n");
    exit(EXIT_SUCCESS);
}

void _cp_assertmem(const void* ptr) {
    if (ptr == NULL) {
        _cp_die("Memory allocation failed.");
    }
}

void _cp_assert(const bool condition, const char* msg) {
    if (!condition) {
        _cp_die(msg);
    }
}

void _cp_expect(const void* ptr1, const void* ptr2, size_t size, const char* msg) {
    if (memcmp(ptr1, ptr2, size) != 0) {
        _cp_die(msg);
    }
}

// Memory buffer

_cp_Buffer** buffers = NULL;
size_t buffer_count = 0;
size_t POOL_SIZE = 1024;

void _cp_dieb(const _cp_Buffer* msg) {
    if (msg == NULL || msg->data == NULL) {
        _cp_die("Fatal error: (null)");
    } else {
        printf("Fatal error: ");
        _cp_buffer_print(msg);
        _cp_clean();
        exit(EXIT_FAILURE);
    }
}

void _cp_warnb(const _cp_Buffer* msg) {
    if (msg == NULL || msg->data == NULL) {
        fprintf(stderr, "Warning: (null)\n");
    } else {
        fprintf(stderr, "Warning: ");
        _cp_buffer_print(msg);
    }
}

void _cp_infob(const _cp_Buffer* msg) {
    if (msg == NULL || msg->data == NULL) {
        fprintf(stdout, "Info: (null)\n");
    } else {
        fprintf(stdout, "Info: ");
        _cp_buffer_print(msg);
    }
}

void _cp_debugb(const _cp_Buffer* msg) {
    if (DEBUG) {
        if (msg == NULL || msg->data == NULL) {
            fprintf(stdout, "Debug: (null)\n");
        } else {
            fprintf(stdout, "Debug: ");
            _cp_buffer_print(msg);
        }
    }
}

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

_cp_Buffer* _cp_buffer_create_cap(size_t capacity) {
    _cp_Buffer* buf = (_cp_Buffer*)malloc(sizeof(_cp_Buffer));
    if (buf == NULL) {
        return NULL; // Allocation failed
    }
    buf->data = (byte*)malloc(capacity);
    if (buf->data == NULL) {
        free(buf);
        return NULL; // Allocation failed
    }
    buf->size = 0;
    buf->capacity = capacity;
    return buf;
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

_cp_Buffer* _cp_buffer_copy_range(const _cp_Buffer* src, const size_t start, const size_t end) {
    if (start >= end || end > src->size) {
        return NULL; // Invalid range
    }
    _cp_Buffer* dest = _cp_buffer_create();
    if (dest == NULL) {
        return NULL; // Allocation failed
    }
    size_t range_size = end - start;
    if (_cp_buffer_expand(dest, range_size) == NULL) {
        _cp_buffer_free(dest);
        return NULL; // Allocation failed
    }
    memcpy(dest->data, src->data + start, range_size);
    dest->size = range_size;
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

size_t _cp_buffer_to_lowercase(_cp_Buffer* buf) {
    if (buf == NULL) return 0;
    size_t changed = 0;
    for (size_t i = 0; i < buf->size; i++) {
        if (buf->data[i] >= 'A' && buf->data[i] <= 'Z') {
            buf->data[i] += 32;
            changed++;
        }
    }
    return changed;
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

void _cp_buffer_print(const _cp_Buffer* buf) {
    if (buf == NULL) {
        return;
    }
    for (size_t i = 0; i < buf->size; i++) {
        putchar(buf->data[i]);
    }
    putchar('\n');
}

void _cp_buffer_print_detail(const _cp_Buffer* buf) {
    if (buf == NULL) {
        printf("Buffer is NULL\n");
        return;
    }
    printf("Buffer size: %zu, capacity: %zu\n", buf->size, buf->capacity);
    printf("Buffer data (hex): ");
    for (size_t i = 0; i < buf->size; i++) {
        printf("%02X ", buf->data[i]);
    }
    printf("\n");
}

void _cp_buffer_print_binary(const _cp_Buffer* buf, const unsigned char divide_each) {
    if (buf == NULL) {
        return;
    }
    int c = 0;
    for (size_t i = 0; i < buf->size+1; i++) {
        for (int bit = 7; bit >= 0; bit--) {
            putchar((buf->data[i] & (1 << bit)) ? '1' : '0'); c++;
            if (divide_each > 0 && c % divide_each == 0) { putchar(' ');  }
        }
    }
    putchar('\n');
}

compare_t _cp_buffer_bufcomp(const _cp_Buffer* buf1, const _cp_Buffer* buf2, size_t offset1, size_t offset2, size_t length) {
    if (buf1 == NULL || buf2 == NULL) return undefined;
    if (offset1 >= buf1->size || offset2 >= buf2->size) return undefined;

    size_t avail1 = buf1->size - offset1;
    size_t avail2 = buf2->size - offset2;

    size_t minavail = min(avail1, avail2);
    size_t cmp_len = length == 0 ? minavail : min(length, minavail);

    const byte* p1 = buf1->data + offset1;
    const byte* p2 = buf2->data + offset2;

    for (size_t i = 0; i < cmp_len; ++i) {
        if (p1[i] < p2[i]) return smaller;
        if (p1[i] > p2[i]) return larger;
    }

    // All compared bytes equal
    if (length > minavail) {
        if (avail1 < avail2) return inf_smaller;
        if (avail1 > avail2) return inf_larger;
    }
    return equal;
}

void _cp_buffer_expectequal(const _cp_Buffer* buf1, const _cp_Buffer* buf2, const char* msg) {
    size_t min_size = min(buf1->size, buf2->size);
    if (_cp_buffer_bufcomp(buf1, buf2, 0, 0, min_size) != equal) {
        _cp_die(msg);
    }
}

void _cp_buffer_expectsize(const _cp_Buffer* buf, const size_t size, const char* msg) {
    if (buf->size != size) {
        _cp_die(msg);
    }
}

void _cp_buffer_expectcap(const _cp_Buffer* buf, const size_t capacity, const char* msg) {
    if (buf->capacity != capacity) {
        _cp_die(msg);
    }
}