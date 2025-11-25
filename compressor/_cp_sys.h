#ifndef _CP_SYS_H
/**
 * @file _cp_sys.h
 * @brief Core system functions and types for the compression library.
 * This header defines essential system functions, types, and macros used throughout the compression library.
 * It includes initialization, error handling, logging, and buffer management functionalities.
 * The types defined here include a byte type, a boolean type, and a comparison type used for various comparisons.
 * The buffer structure defined here is a simple dynamic array used for storing data during compression operations.
 * This file is intended to be included by other components of the compression library to provide consistent system-level functionality.
 * This file is part of the Compression Library.
 * 
 * Detail about buffer management:
 * The buffer management functions provide a simple interface for creating, expanding, copying, and manipulating dynamic
 * buffers. These buffers are used to hold data during compression, decompression, and general data processing tasks.
 * The buffer structure consists of a data pointer, size, and capacity. The functions allow for efficient memory management
 * by reusing buffers from a pool when possible, reducing the overhead of frequent allocations and deallocations.
 * A buffer pool is maintained to store reusable buffers, which helps improve performance in scenarios with frequent buffer usage.
 * By default, buffers are created with a predefined size of 4096 bytes, which is suitable for typical compression tasks.
 * When a buffer needs to grow, it is expanded to accommodate the required size, doubling its capacity as needed.
 * The buffer functions also provide utilities for converting between C strings and buffers, as well as for
 * appending characters and accessing individual bytes within the buffer. The system library does not provide bit-level
 * access for buffers, they can be found in each compression algorithm package.
 * 
 * Details about boolean and comparison types:
 * The boolean type is defined as an unsigned char, allowing for simple true/false values. The comparison type is defined as a char,
 * with specific values representing different comparison results, such as larger, smaller, equal, and undefined.
 * Utility functions, is_larger() and is_smaller(), are provided to facilitate checking comparison results.
 * 
 * @author William Wu
 */
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
/**
 * Asserts that a memory allocation was successful.
 * If the pointer is NULL, it outputs a fatal error message and exits the program.
 * @param ptr The pointer to check.
 */
void _cp_assertmem(const void* ptr);

// Math macros
#define min(a,b) ((a) < (b) ? (a) : (b))
#define max(a,b) ((a) > (b) ? (a) : (b))
#define min_three(a,b,c) ((a) < (b) ? ((a) < (c) ? (a) : (c)) : ((b) < (c) ? (b) : (c)))
#define max_three(a,b,c) ((a) > (b) ? ((a) > (c) ? (a) : (c)) : ((b) > (c) ? (b) : (c)))
/**
 * A byte type representing an 8-bit unsigned value.
 */
typedef unsigned char byte;
/**
 * A boolean type representing true or false values. Boolean values are values that are either true or false.
 */
typedef unsigned char bool;
/**
 * A comparison type used for comparisons between values, such as strings or integers.
 * It can represent various comparison results including larger, smaller, equal, and undefined.
 * 
 * Comparison operators can be used to compare two compare_t values, and it follows that infinitely larger is greater than larger,
 * which is greater than equal, which is greater than smaller, which is greater than infinitely smaller. All values are greater than undefined.
 * 
 * When checking for comparison results for the simple purpose of equality, it is possible to do '== equal'.
 * When checking for inequality, it is possible to do '!= equal'.
 * When checking for greater-than relationships, it is possible to do '> equal' to check for both larger and infinitely larger.
 * For simplicity, you may also use the is_larger() function to check for both larger and infinitely larger.
 * However, when checking for less-than relationships, in addition to '< larger', it is also necessary to check for != undefined to ensure the comparison is defined,
 * unless the context guarantees that the comparison is defined. For simplicity, you may also use the is_smaller() function to check for both smaller
 * and infinitely smaller while ensuring the comparison is defined.
 */
typedef char compare_t;
// Comment on why not use enum: enums are a bit wasteful and cannot achieve the fact to let undefined be less than all other values easily.
/**
 * False boolean value, evaluates to 0.
 */
extern const bool false;
/**
 * True boolean value, evaluates to 1.
 */
extern const bool true;
/**
 * Comparison result indicating the first value is infinitely larger than the second, which indicates that either
 * the first value is infinity or the second value is negative infinity.
 */
extern const compare_t inf_larger;
/**
 * Comparison result indicating the first value is larger than the second. This evaluates to 1.
 */
extern const compare_t larger;
/**
 * Comparison result indicating the first value is smaller than the second. This evaluates to -1.
 */
extern const compare_t smaller;
/**
 * Comparison result indicating the first value is infinitely smaller than the second, which indicates that either
 * the first value is negative infinity or the second value is infinity.
 */
extern const compare_t inf_smaller;
/**
 * Comparison result indicating the two values are equal. This evaluates to 0.
 */
extern const compare_t equal;
/**
 * Comparison result indicating the comparison is undefined, such as when comparing NaN values or for relationships that are not clearly defined.
 */
extern const compare_t undefined;

/**
 * Asserts that a condition is true.
 * If the condition is false, it outputs a fatal error message and exits the program.
 * @param condition The condition to check.
 * @param msg The error message to display if the assertion fails.
 */
void _cp_assert(const bool condition, const char* msg);
/**
 * Expects the contents of two pointers to be equal.
 * If they are not equal, it outputs a fatal error message and exits the program.
 * This compares the full contents of the memory locations pointed to by the pointers.
 * 
 * Note: This function may crush the program due to memory access violation if the pointers are invalid.
 * 
 * @param ptr1 The first pointer to compare.
 * @param ptr2 The second pointer to compare.
 * @param size The size of the memory to compare in bytes.
 * @param msg The error message to display if the expectation fails.
 */
void _cp_expect(const void* ptr1, const void* ptr2, size_t size, const char* msg);

/**
 * Checks if the comparison result indicates that the first value is larger than the second.
 * This includes both 'larger' and 'infinitely larger' results.
 * @param cmp The comparison result to check.
 * @return true if the first value is larger, false otherwise.
 */
bool is_larger(const compare_t cmp);
/**
 * Checks if the comparison result indicates that the first value is smaller than the second.
 * This includes both 'smaller' and 'infinitely smaller' results.
 * @param cmp The comparison result to check.
 * @return true if the first value is smaller, false otherwise.
 */
bool is_smaller(const compare_t cmp);

/**
 * A simple buffer structure to hold data, its size, and capacity.
 */
typedef struct {
    byte* data;
    size_t size;
    size_t capacity;
} _cp_Buffer;

/**
 * Creates a buffer from a null-terminated C string without copying the data.
 * The buffer's data pointer points directly to the original string.
 * The size and capacity of the buffer are set to the length of the string.
 * 
 * Unlike all the functions that create buffers, this macro does not perform any memory allocation,
 * so there is no need to free the buffer created by this macro.
 * However, the original string must remain valid for the lifetime of the buffer.
 * 
 * IMPORTANT: Since this macro does not perform memory allocation, the resulting buffer should not be freed.
 * Freeing such a buffer would lead to undefined behavior. Since most functions in this package can dymanically
 * allocate, reallocate, or free buffers, you may only pass the result of this macros to functions that
 * explicitly protect it with const qualifier and does not attempt to free or reallocate the buffer.
 * @param str The C string to use.
 * @return A new buffer referencing the string data, or NULL if str is NULL.
 */
#define _cp_buffer_from_cstr_const(strptr) \
    (&(_cp_Buffer){ \
        .data = (byte*)(strptr), \
        .size = strlen(strptr), \
        .capacity = strlen(strptr) + 1 \
    })
/**
 * Creates a buffer from raw data without copying the data.
 * The buffer's data pointer points directly to the provided data.
 * The size and capacity of the buffer are set to the provided data size.
 * The function assumes the correctness of the datasize, be aware of buffer overflows.
 * 
 * Unlike all the functions that create buffers, this macro does not perform any memory allocation,
 * so there is no need to free the buffer created by this macro.
 * However, the original data must remain valid for the lifetime of the buffer.
 * 
 * IMPORTANT: Since this macro does not perform memory allocation, the resulting buffer should not be freed.
 * Freeing such a buffer would lead to undefined behavior. Since most functions in this package can dymanically
 * allocate, reallocate, or free buffers, you may only pass the result of this macros to functions that
 * explicitly protect it with const qualifier and does not attempt to free or reallocate the buffer.
 * 
 * IMPORTANT: DO NOT store the result of this macro into a non-const _cp_Buffer pointer, as this object is compiled
 * statically on the stack and is not intended to be modified. For best practice, do not store the result of this macro
 * into any variable at all, just use it directly as a parameter to functions that accept const _cp_Buffer*.
 * @param dataptr Pointer to the raw data.
 * @param datasize Size of the raw data in bytes.
 * @return A new buffer referencing the raw data, or NULL if dataptr is NULL.
 */
#define _cp_buffer_from_data_const(dataptr, datasize) \
    (&(_cp_Buffer){ \
        .data = (byte*)(dataptr), \
        .size = (datasize), \
        .capacity = (datasize) \
    })
/**
 * Creates a constant view of the buffer starting from the specified offset.
 * The view shares the same data pointer as the original buffer, adjusted by the offset.
 * The size and capacity of the view are reduced by the offset.
 * 
 * The function assumes that the offset is within the bounds of the original buffer.
 * If this is not the case, the behavior is undefined.
 * 
 * This function does not perform any memory allocation, so there is no need to free the resulting buffer view.
 * However, the original buffer must remain valid for the lifetime of the buffer view.
 * 
 * IMPORTANT: Since this macro does not perform memory allocation, the resulting buffer should not be freed.
 * Freeing such a buffer would lead to undefined behavior. Since most functions in this package can dymanically
 * allocate, reallocate, or free buffers, you may only pass the result of this macros to functions that
 * explicitly protect it with const qualifier and does not attempt to free or reallocate the buffer.
 * 
 * IMPORTANT: DO NOT store the result of this macro into a non-const _cp_Buffer pointer, as this object is compiled
 * statically on the stack and is not intended to be modified. For best practice, do not store the result of this macro
 * into any variable at all, just use it directly as a parameter to functions that accept const _cp_Buffer*.
 * @param bufptr Pointer to the original buffer.
 * @param offset The offset from which the view starts.
 * @return A new constant buffer view starting from the specified offset.
 */
#define _cp_buffer_view_const(bufptr, offset) \
    (&(_cp_Buffer){ \
        .data = (byte*)((bufptr)->data + (offset)), \
        .size = (bufptr)->size - (offset), \
        .capacity = (bufptr)->capacity - (offset) \
    })
/**
 * Creates a constant window view of the buffer between the specified start and end offsets.
 * The view shares the same data pointer as the original buffer, adjusted by the start offset.
 * The size of the view is set to the difference between the end and start offsets.
 * The capacity of the view is reduced by the start offset.
 * 
 * The function assumes that the start and end offsets are within the bounds of the original buffer
 * and that start is less than or equal to end. If this is not the case, the behavior is undefined.
 * 
 * This function does not perform any memory allocation, so there is no need to free the resulting buffer view.
 * However, the original buffer must remain valid for the lifetime of the buffer view.
 * 
 * It is the best practice to avoid use this macro if a few calls to _cp_buffer_char_at()
 * or direct manipulation of the original buffer with offset calculations would suffice. This is because
 * although this macro may improve performance and code readability in certain scenarios,
 * it also increases the risk of creating dangling buffer views if the original buffer is modified or freed.
 * 
 * IMPORTANT: Since this macro does not perform memory allocation, the resulting buffer should not be freed.
 * Freeing such a buffer would lead to undefined behavior. Since most functions in this package can dymanically
 * allocate, reallocate, or free buffers, you may only pass the result of this macros to functions that
 * explicitly protect it with const qualifier and does not attempt to free or reallocate the buffer.
 * 
 * IMPORTANT: DO NOT store the result of this macro into a non-const _cp_Buffer pointer, as this object is compiled
 * statically on the stack and is not intended to be modified. For best practice, do not store the result of this macro
 * into any variable at all, just use it directly as a parameter to functions that accept const _cp_Buffer*.
 * @param bufptr Pointer to the original buffer.
 * @param start The starting offset of the window.
 * @param end The ending offset of the window.
 * @return A new constant window buffer view between the specified offsets.
 */
#define _cp_buffer_window_consts(bufptr, start, end) \
    (&(_cp_Buffer){ \
        .data = (byte*)((bufptr)->data + (start)), \
        .size = (end) - (start), \
        .capacity = (bufptr)->capacity - (start) \
    })
/**
 * Creates a constant trimmed view of the buffer with the specified new size.
 * The view shares the same data pointer as the original buffer.
 * The size of the view is set to the new size, while the capacity remains unchanged.
 * 
 * The function assumes that the new size is less than or equal to the original buffer's size.
 * If this is not the case, the behavior is undefined.
 * 
 * This function does not perform any memory allocation, so there is no need to free the resulting buffer view.
 * However, the original buffer must remain valid for the lifetime of the buffer view.
 * 
 * IMPORTANT: Since this macro does not perform memory allocation, the resulting buffer should not be freed.
 * Freeing such a buffer would lead to undefined behavior. Since most functions in this package can dymanically
 * allocate, reallocate, or free buffers, you may only pass the result of this macros to functions that
 * explicitly protect it with const qualifier and does not attempt to free or reallocate the buffer.
 * 
 * IMPORTANT: DO NOT store the result of this macro into a non-const _cp_Buffer pointer, as this object is compiled
 * statically on the stack and is not intended to be modified. For best practice, do not store the result of this macro
 * into any variable at all, just use it directly as a parameter to functions that accept const _cp_Buffer*.
 * @param bufptr Pointer to the original buffer.
 * @param newsize The new size for the trimmed view.
 * @return A new constant trimmed buffer view with the specified size.
 */ 
#define _cp_buffer_trim_const(bufptr, newsize) \
    (&(_cp_Buffer){ \
        .data = (byte*)((bufptr)->data), \
        .size = (newsize), \
        .capacity = (bufptr)->capacity \
    })

/**
 * Asserts that two buffers are equal in content.
 * If they are not equal, it outputs a fatal error message and exits the program.
 * This compares the full contents of the two buffers.
 * @param buf1 The first buffer to compare.
 * @param buf2 The second buffer to compare.
 * @param msg The error message to display if the expectation fails.
 */
void _cp_buffer_expectequal(const _cp_Buffer* buf1, const _cp_Buffer* buf2, const char* msg);
/**
 * Asserts that the buffer has the expected size.
 * If the buffer's size does not match the expected size, it outputs a fatal error message and exits the program.
 * 
 * Do not confuse this function with _cp_buffer_expectcap(), which checks for capacity instead of size.
 * @param buf The buffer to check.
 * @param size The expected size of the buffer.
 * @param msg The error message to display if the expectation fails.
 */
void _cp_buffer_expectsize(const _cp_Buffer* buf, const size_t size, const char* msg);
/**
 * Asserts that the buffer has the expected capacity.
 * If the buffer's capacity does not match the expected capacity, it outputs a fatal error message and exits the program.
 * 
 * Do not confuse this function with _cp_buffer_expectsize(), which checks for size instead of capacity.
 * @param buf The buffer to check.
 * @param capacity The expected capacity of the buffer.
 * @param msg The error message to display if the expectation fails.
 */
void _cp_buffer_expectcap(const _cp_Buffer* buf, const size_t capacity, const char* msg);
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
 * Creates a new buffer with the specified capacity.
 * It will not attempt to retrieve from the pool.
 * You still can free the buffer with _cp_buffer_free() and it will put it back to the pool if its capacity is more or equal to DEFAULT_SIZE.
 * For normal usage, prefer _cp_buffer_create() instead.
 * @param capacity The desired capacity of the buffer.
 * @return A pointer to the newly created buffer.
 */
_cp_Buffer* _cp_buffer_create_cap(size_t capacity);
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
 * Creates a copy of a range within the given buffer.
 * @param src The source buffer to copy from.
 * @param start The starting index of the range (inclusive).
 * @param end The ending index of the range (exclusive).
 * @return A new buffer containing the specified range, or NULL on allocation failure.
 */
_cp_Buffer* _cp_buffer_copy_range(const _cp_Buffer* src, const size_t start, const size_t end);
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
 * Converts all uppercase characters in the buffer to lowercase in place.
 * @param buf The buffer to convert.
 * @return The number of characters converted to lowercase.
 */
size_t _cp_buffer_to_lowercase(_cp_Buffer* buf);
/**
 * Concatenates two buffers into a new buffer. Note that this function
 * performs memory copying. The original buffers remain unaffected.
 * @param buf1 The first buffer.
 * @param buf2 The second buffer.
 * @return A new buffer containing the concatenated data, or NULL on allocation failure.
 */
_cp_Buffer* _cp_buffer_concat(const _cp_Buffer* buf1, const _cp_Buffer* buf2);
/**
 * Prints the contents of the buffer to stdout.
 * If the buffer is NULL, prints nothing.
 * @param buf The buffer to print.
 */
void _cp_buffer_print(const _cp_Buffer* buf);
/**
 * Prints detailed information about the buffer to stdout, including size, capacity, and hex representation of data.
 * If the buffer is NULL, indicates that the buffer is NULL.
 * @param buf The buffer to print details of.
 */
void _cp_buffer_print_detail(const _cp_Buffer* buf);
/**
 * Prints the binary representation of the buffer's data to stdout.
 * If the buffer is NULL, prints nothing.
 * @param buf The buffer to print in binary.
 * @param divide_each Number of bits after which to insert a space for readability. If 0, no spaces are added.
 */
void _cp_buffer_print_binary(const _cp_Buffer* buf, const unsigned char divide_each);
/**
 * Concatenates src buffer into dest buffer in place.
 * No new buffer is created. The dest buffer is expanded if necessary.
 * @param dest The destination buffer to append to.
 * @param src The source buffer to append from.
 */
void _cp_buffer_concat_inplace(_cp_Buffer* dest, const _cp_Buffer* src);
/**
 * Compares two buffers lexicographically from specified offsets for a given length.
 * @param buf1 The first buffer.
 * @param buf2 The second buffer.
 * @param offset1 The starting offset in the first buffer.
 * @param offset2 The starting offset in the second buffer.
 * @param length The number of bytes to compare
 * @return A compare_t value indicating the comparison result.
 */
compare_t _cp_buffer_bufcomp(const _cp_Buffer* buf1, const _cp_Buffer* buf2, size_t offset1, size_t offset2, size_t length);

#endif // _CP_SYS_H