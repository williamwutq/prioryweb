#ifndef _CP_BASE32_H_
#define _CP_BASE32_H_

#ifndef _CP_SYS_H_
#include "_cp_sys.h"
#endif // _CP_SYS_H_

#ifndef _STDDEF_H_
#include <stddef.h>
#endif // _STDDEF_H_

/**
 * Encode a single character into base32 format and write to buffer at given index.
 * @param buf The buffer to write to.
 * @param offset The byte offset in the buffer.
 * @param index The index of the 5-bit value to write.
 * @param c The character to encode.
 */
void _cp_base32_encode_char(_cp_Buffer* buf, const size_t offset, size_t* index, const char c);

/**
 * Encodes terminator character, which is the null character.
 * @param buf The buffer to write to.
 * @param offset The byte offset in the buffer.
 * @param index The index of the 5-bit value to write.
 */
void _cp_base32_encode_terminator(_cp_Buffer* buf, const size_t offset, size_t* index);

/**
 * Encodes start of sequence character.
 * @param buf The buffer to write to.
 * @param offset The byte offset in the buffer.
 * @param index The index of the 5-bit value to write.
 */
void _cp_base32_encode_starter(_cp_Buffer* buf, const size_t offset, size_t* index);

/**
 * Encodes a non-breaking space character.
 * @param buf The buffer to write to.
 * @param offset The byte offset in the buffer.
 * @param index The index of the 5-bit value to write.
 */
void _cp_base32_encode_nbsp(_cp_Buffer* buf, const size_t offset, size_t* index);

/**
 * Encodes an end-of-line character.
 * @param buf The buffer to write to.
 * @param offset The byte offset in the buffer.
 * @param index The index of the 5-bit value to write.
 */
void _cp_base32_encode_endl(_cp_Buffer* buf, const size_t offset, size_t* index);

/**
 * Writes 5 bits of value into the buffer at the specified index.
 * @param buf The buffer to write to.
 * @param offset The byte offset in the buffer.
 * @param index The index of the 5-bit value to write.
 * @param value The 5-bit value to write (0-31).
 */
void _cp_base32_write_5_bits(_cp_Buffer* buf, const size_t offset, size_t* index, byte value);

/**
 * Encodes a string into base32 format and writes to buffer.
 * @param buf The buffer to write to.
 * @param str Buffer containing. Null termination is followed, which means if str contains null character,
 *            encoding stops there, even if length indicates more characters.
 * @param offset The byte offset in the buffer.
 */
void _cp_base32_encode(_cp_Buffer* buf, const _cp_Buffer* str, const size_t offset);

/**
 * Reads 5 bits of value from the buffer at the specified index.
 * @param buf The buffer to read from.
 * @param offset The byte offset in the buffer.
 * @param index The index of the 5-bit value to read.
 * @return The 5-bit value read (0-31).
 */
char _cp_base32_read_5_bits(_cp_Buffer* buf, size_t offset, size_t index);

/**
 * Decodes another character in escape mode and appends the corresponding character(s) to output buffer.
 * @param buf The buffer containing base32 encoded data.
 * @param offset The byte offset in the buffer.
 * @param index The index of the 5-bit value to read.
 * @param out The output buffer to write decoded characters into.
 * @return true if an escape sequence was decoded, false otherwise.
 */
bool _cp_base32_decode_escape(_cp_Buffer* buf, size_t offset, size_t index, _cp_Buffer* out);

/**
 * Decodes another character in non-escape mode and appends the corresponding character(s) to output buffer.
 * @param buf The buffer containing base32 encoded data.
 * @param offset The byte offset in the buffer.
 * @param index The index of the 5-bit value to read.
 * @param out The output buffer to write decoded characters into.
 * @return true if an escape sequence was detected, false otherwise.
 */
bool _cp_base32_decode_nonescape(_cp_Buffer* buf, size_t offset, size_t index, _cp_Buffer* out);

/**
 * Decodes a base32 encoded buffer back into a string and append to output buffer.
 * @param buf The buffer containing base32 encoded data.
 * @param offset The byte offset in the buffer.
 * @param out The output buffer to write decoded characters into.
 */
void _cp_base32_decode(_cp_Buffer* buf, size_t offset, _cp_Buffer* out);

#endif // _CP_BASE32_H_;