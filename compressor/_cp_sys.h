#ifndef _CP_SYS_H
#define _CP_SYS_H
#ifndef _STDDEF_H_
#include <stddef.h>
#endif // _STDDEF_H_

/**
 * Initializes the compression system.
 */
void _cp_init(void);
/**
 * Outputs a fatal error message to stderr and exits the program.
 * @param msg The error message to display.
 * @return Never returns; exits the program.
 */
void _cp_die(const char* msg);
/**
 * Outputs a warning message to stderr.
 * @param msg The warning message to display.
 */
void _cp_warn(const char* msg);
/**
 * Outputs an informational message to stdout.
 * @param msg The informational message to display.
 */
void _cp_info(const char* msg);
/**
 * Outputs a debug message to stdout.
 * @param msg The debug message to display.
 */
void _cp_debug(const char* msg);
/**
 * Finalizes the compression process, frees resources, and exits successfully.
 * @return Never returns; exits the program.
 */
void _cp_finish(void);

typedef unsigned char byte;
typedef unsigned char bool;
extern const bool false;
extern const bool true;

/**
 * A simple buffer structure to hold data, its size, and capacity.
 */
typedef struct {
    byte* data;
    size_t size;
    size_t capacity;
} _cp_Buffer;

/**
 * Initializes the buffer pool.
 * This function allocates memory for a pool of reusable buffers.
 */
void _cp_init_buffer_pool(void);
/**
 * Frees all buffers in the pool and releases associated memory.
 */
void _cp_free_buffer_pool(void);
/**
 * Pushes a buffer back into the pool for reuse.
 * If the pool is full, the buffer is freed.
 * @param buf The buffer to push back into the pool.
 */
void _cp_push_buffer_to_pool(_cp_Buffer* buf);
/**
 * Pops a buffer from the pool. If the pool is empty, returns NULL.
 * @return A buffer from the pool or NULL if the pool is empty.
 */
_cp_Buffer* _cp_pop_buffer_from_pool(void);
/**
 * Creates a new buffer. It first attempts to retrieve a buffer from the pool.
 * If none are available, it allocates a new buffer with DEFAULT_SIZE capacity.
 * @return A pointer to the newly created buffer.
 */
_cp_Buffer* _cp_buffer_create(void);
/**
 * Frees the buffer. If the buffer's capacity is equal to DEFAULT_SIZE,
 * it is returned to the pool for reuse. If larger, it is shrunk to
 * DEFAULT_SIZE before being returned to the pool. If smaller, it is
 * freed directly.
 * @param buf The buffer to free.
 */
void _cp_buffer_free(_cp_Buffer* buf);
/**
 * Clears the buffer by setting its size to zero.
 * @param buf The buffer to clear.
 */
void _cp_buffer_clear(_cp_Buffer* buf);
/**
 * Ensures the buffer has enough capacity to accommodate additional data of size min_size.
 * If not, it expands the buffer's capacity.
 * @param buf The buffer to expand.
 * @param min_size The minimum additional size required.
 * @return The expanded buffer, or NULL on allocation failure.
 */
_cp_Buffer* _cp_buffer_expand(_cp_Buffer* buf, const size_t min_size);
/**
 * Doubles the capacity of the buffer.
 * @param buf The buffer to double.
 * @return The buffer with doubled capacity, or NULL on allocation failure.
 */
_cp_Buffer* _cp_buffer_double(_cp_Buffer* buf);
/**
 * Creates a copy of the given buffer.
 * @param src The source buffer to copy.
 * @return A new buffer that is a copy of the source, or NULL on allocation failure.
 */
_cp_Buffer* _cp_buffer_copy(const _cp_Buffer* src);
/**
 * Returns the remaining capacity of the buffer.
 * @param buf The buffer to check.
 * @return The remaining capacity in bytes.
 */
size_t _cp_buffer_remaining_capacity(const _cp_Buffer* buf);
/**
 * Returns the character at the specified index in the buffer.
 * @param buf The buffer to read from.
 * @param index The index of the character to retrieve.
 * @return The character at the specified index, or '\0' if out of bounds.
 */
char _cp_buffer_char_at(const _cp_Buffer* buf, const size_t index);
/**
 * Sets the character at the specified index in the buffer.
 * @param buf The buffer to modify.
 * @param index The index of the character to set.
 * @param c The character to set at the specified index.
 */
void _cp_buffer_set_char_at(const _cp_Buffer* buf, const size_t index, const char c);
/**
 * Appends a character to the end of the buffer.
 * Expands the buffer if necessary.
 * @param buf The buffer to append to.
 * @param c The character to append.
 */
void _cp_buffer_append_char(_cp_Buffer* buf, const char c);
/**
 * Converts the buffer data to a null-terminated C string.
 * Note: The buffer must have enough capacity to add the null terminator.
 * If not, the function will attempt to expand the buffer.
 * @param buf The buffer to convert.
 * @return A pointer to the C string, or NULL on failure.
 */
char* _cp_buffer_cstr(_cp_Buffer* buf);
/**
 * Creates a buffer from a null-terminated C string.
 * Note that this function performs memory copying. The original string
 * remains unaffected. For in place usage, see _cp_buffer_from_cstr_inplace.
 * @param str The C string to convert.
 * @return A new buffer containing the string data, or NULL on allocation failure.
 */
_cp_Buffer* _cp_buffer_from_cstr(const char* str);
/**
 * Creates a buffer that directly uses the provided C string's memory.
 * No copying is performed; the buffer's data pointer points to the original string.
 * The buffer's size and capacity are set to the length of the string.
 * @param str The C string to use.
 * @return A new buffer referencing the string data, or NULL if str is NULL or on allocation failure.
 */
_cp_Buffer* _cp_buffer_from_cstr_inplace(const char* str);
/**
 * Concatenates two buffers into a new buffer. Note that this function
 * performs memory copying. The original buffers remain unaffected.
 * @param buf1 The first buffer.
 * @param buf2 The second buffer.
 * @return A new buffer containing the concatenated data, or NULL on allocation failure.
 */
_cp_Buffer* _cp_buffer_concat(const _cp_Buffer* buf1, const _cp_Buffer* buf2);
/**
 * Concatenates src buffer into dest buffer in place.
 * No new buffer is created. The dest buffer is expanded if necessary.
 * @param dest The destination buffer to append to.
 * @param src The source buffer to append from.
 */
void _cp_buffer_concat_inplace(_cp_Buffer* dest, const _cp_Buffer* src);

#endif // _CP_SYS_H