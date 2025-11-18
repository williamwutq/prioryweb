// Implement Encoding for Base 64 Format as described in format.md

#ifndef _CP_BASE64_C_
#include "_cp_base64.h"
#endif // _CP_BASE64_C_
#ifndef _CP_SYS_H_
#include "_cp_sys.h"
#endif // _CP_SYS_H_
#ifndef _STDLIB_H_
#include <stdlib.h>
#endif // _STDLIB_H_
#ifndef _STRING_H_
#include <string.h>
#endif // _STRING_H_
#ifndef _STDIO_H_
#include <stdio.h>
#endif // _STDIO_H_

void _cp_base64_write_6_bits(_cp_Buffer* buf, const size_t offset, const size_t index, byte value) {
    size_t bit_pos = index * 6;
    size_t byte_pos = bit_pos / 8 + offset;
    size_t bit_offset = bit_pos % 8;
    size_t required_size = bit_offset > 2 ? byte_pos + 2 : byte_pos + 1;

    while (byte_pos + 1 >= buf->capacity) {
        _cp_buffer_double(buf);
    }
    if (required_size > buf->size) {
        buf->size = required_size;
    }

    if (bit_offset == 0) {
        buf->data[byte_pos] &= ~0xFC;
        buf->data[byte_pos] |= (value & 0x3F) << 2;
        return;
    }

    buf->data[byte_pos] &= ~(0x3F >> (bit_offset - 2));
    if (bit_offset > 2) {
        buf->data[byte_pos + 1] &= ~(0x3F << (10 - bit_offset));
    }

    buf->data[byte_pos] |= (value & 0x3F) >> (bit_offset - 2);
    if (bit_offset > 2) {
        buf->data[byte_pos + 1] |= (value & 0x3F) << (10 - bit_offset);
    }
}

byte _cp_base64_read_6_bits(const _cp_Buffer* buf, size_t offset, size_t index) {
    size_t bit_pos = index * 6;
    size_t byte_pos = bit_pos / 8 + offset;
    size_t bit_offset = bit_pos % 8;

    if (byte_pos >= buf->size) {
        return 0;
    }

    if (bit_offset == 0) {
        return buf->data[byte_pos] >> 2 & 0x3F;
    }

    byte value = (buf->data[byte_pos] << (bit_offset - 2));
    if (bit_offset > 2 && byte_pos < buf->size) {
        value |= (buf->data[byte_pos + 1] >> (10 - bit_offset));
    }

    return value & 0x3F;
}