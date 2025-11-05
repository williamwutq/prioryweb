#ifndef _CP_SYS_H
#define _CP_SYS_H
#include <stddef.h>
#include <stdint.h>

void _cp_init();
void _cp_die(const char* msg);
void _cp_warn(const char* msg);
void _cp_info(const char* msg);
void _cp_debug(const char* msg);
void _cp_finish();

typedef unsigned char byte;

/**
 * A simple buffer structure to hold data, its size, and capacity.
 */
typedef struct {
    byte* data;
    size_t size;
    size_t capacity;
} _cp_Buffer;

void _cp_init_buffer_pool();
void _cp_free_buffer_pool();
void _cp_push_buffer_to_pool(_cp_Buffer* buf);
void _cp_pop_buffer_from_pool(_cp_Buffer* buf);
_cp_Buffer* _cp_buffer_create();
void _cp_buffer_free(_cp_Buffer* buf);
_cp_Buffer* _cp_buffer_expand(_cp_Buffer* buf, size_t min_size);
_cp_Buffer* _cp_buffer_double(_cp_Buffer* buf);
_cp_Buffer* _cp_buffer_copy(_cp_Buffer* src);

#endif // _CP_SYS_H