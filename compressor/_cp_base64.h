#ifndef _CP_BASE64_H_
#define _CP_BASE64_H_

#ifndef _CP_SYS_H_
#include "_cp_sys.h"
#endif // _CP_SYS_H_

#ifndef _STDDEF_H_
#include <stddef.h>
#endif // _STDDEF_H_

/**
 * Writes 6 bits of value into the buffer at the specified index.
 * @param buf The buffer to write to.
 * @param offset The byte offset in the buffer.
 * @param index The index of the 6-bit value to write.
 * @param value The 6-bit value to write (0-63).
 */
void _cp_base64_write_6_bits(_cp_Buffer* buf, const size_t offset, const size_t index, byte value);

/**
 * Reads 6 bits of value from the buffer at the specified index.
 * @param buf The buffer to read from.
 * @param offset The byte offset in the buffer.
 * @param index The index of the 6-bit value to read.
 * @return The 6-bit value read (0-63).
 */
byte _cp_base64_read_6_bits(const _cp_Buffer* buf, size_t offset, size_t index);

#endif // _CP_BASE64_H_