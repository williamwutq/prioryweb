// Implement Encoding for Base 64 Format as described in format.md
// Supporting Unicode Standard: UTF-8

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

static const char decode_table_normal[64] = {
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
    'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
    'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
    'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
    'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
    'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
    'w', 'x', 'y', 'z', '0', '1', '2', '3',
    '4', '5', '6', '7', '8', '9', 0xF8, '\\' // Escape character
    // 0xF8 -> Assignment operator
};

static const char decode_table_escape[64] = {
    '.', ',', '-', '_', '/', ':', ';', '?',
    '!', '@', '#', '$', '+', '=', '*', '&',
    '%', '^', '(', ')', '[', ']', '{', '}',
    '<', '>', '\a','\b',0xA9, -15,'\e','\f',
    -16, 0xD7,0xF7,-10, -11, -12, -6,'\n',
    -13, 0xA0, 0xAE,'\r',' ', '\t',-5,'\v',
    -14, -2, -3, -4, '\\', '|', '~', '`',
    '"', '\'',-24, -25, -26, -27, -28, 0
    // 0 -> null terminator
    // A0 -> &nbsp;
    // -2 -> &thinsp;
    // -3 -> &ensp;
    // -4 -> &emsp;
    // A9 -> &copy;
    // AE -> &reg;
    // -5 -> &trade;
    // F7 -> &div;
    // -6 -> &#10003; (check mark)
    // -10 -> &laquo;
    // -11 -> &raquo;
    // -12 -> &bull;
    // -13 -> &hellip;
    // -14 -> &mdash;
    // -15 -> &deg;
    // -16 -> &plusmn;
    // -24 -> &larr;
    // -25 -> &uarr;
    // -26 -> &rarr;
    // -27 -> &darr;
    // -28 -> &harr;
    // D7 -> &times;
};

void _cp_base64_encode_unicode(_cp_Buffer* buf, const size_t offset, size_t* index, const unsigned int codepoint) {
    if (codepoint == 0xA0) {
        // &nbsp; -> escape + p
        _cp_base64_write_6_bits(buf, offset, (*index)++, 63);
        _cp_base64_write_6_bits(buf, offset, (*index)++, 41);
    } if (codepoint <= 0x7F) {
        // 1-byte UTF-8 -> 2 6-bit values
        _cp_base64_write_6_bits(buf, offset, (*index)++, 2);
        _cp_base64_write_6_bits(buf, offset, (*index)++, (byte)(0xC0 | ((codepoint >> 6) & 0x1F)));
        _cp_base64_write_6_bits(buf, offset, (*index)++, (byte)(0x80 | (codepoint & 0x3F)));
    } else if (codepoint <= 0x7FF) {
        // 2-byte UTF-8 -> 3 6-bit values
        _cp_base64_write_6_bits(buf, offset, (*index)++, 3);
        _cp_base64_write_6_bits(buf, offset, (*index)++, (byte)(0xE0 | ((codepoint >> 12) & 0x0F)));
        _cp_base64_write_6_bits(buf, offset, (*index)++, (byte)(0x80 | ((codepoint >> 6) & 0x3F)));
        _cp_base64_write_6_bits(buf, offset, (*index)++, (byte)(0x80 | (codepoint & 0x3F)));
    } else if (codepoint <= 0xFFFF) {
        // 3-byte UTF-8 -> 4 6-bit values
        _cp_base64_write_6_bits(buf, offset, (*index)++, 4);
        _cp_base64_write_6_bits(buf, offset, (*index)++, (byte)(0xF0 | ((codepoint >> 18) & 0x07)));
        _cp_base64_write_6_bits(buf, offset, (*index)++, (byte)(0x80 | ((codepoint >> 12) & 0x3F)));
        _cp_base64_write_6_bits(buf, offset, (*index)++, (byte)(0x80 | ((codepoint >> 6) & 0x3F)));
        _cp_base64_write_6_bits(buf, offset, (*index)++, (byte)(0x80 | (codepoint & 0x3F)));
    } else if (codepoint <= 0x10FFFF) {
        // 4-byte UTF-8 -> 5 6-bit values
        _cp_base64_write_6_bits(buf, offset, (*index)++, 5);
        _cp_base64_write_6_bits(buf, offset, (*index)++, (byte)(0xF8 | ((codepoint >> 24) & 0x03)));
        _cp_base64_write_6_bits(buf, offset, (*index)++, (byte)(0x80 | ((codepoint >> 18) & 0x3F)));
        _cp_base64_write_6_bits(buf, offset, (*index)++, (byte)(0x80 | ((codepoint >> 12) & 0x3F)));
        _cp_base64_write_6_bits(buf, offset, (*index)++, (byte)(0x80 | ((codepoint >> 6) & 0x3F)));
        _cp_base64_write_6_bits(buf, offset, (*index)++, (byte)(0x80 | (codepoint & 0x3F)));
    }
}

void _cp_base64_encode_ascii(_cp_Buffer* buf, const size_t offset, size_t* index, const char code) {
    if (code >= 'A' && code <= 'Z') {
        // A-Z -> 0-25
        _cp_base64_write_6_bits(buf, offset, (*index)++, (byte)(code - 65));
    } else if (code >= 'a' && code <= 'z') {
        // a-z -> 26-51
        _cp_base64_write_6_bits(buf, offset, (*index)++, (byte)(code - 71));
    } else if (code >= '0' && code <= '9') {
        // 0-9 -> 52-61
        _cp_base64_write_6_bits(buf, offset, (*index)++, (byte)(code + 4));
    } else if (code == ':') {
        // : -> 62
        _cp_base64_write_6_bits(buf, offset, (*index)++, 62);
    } else {
        byte c;
        switch (code) {
            case '\0':// NULL
                c = 63; break;
            case '\a':// 'a'
                c = 26; break;
            case '\b':// 'b'
                c = 27; break;
            case '\e':// 'e'
                c = 30; break;
            case '\f':// 'f'
                c = 31; break;
            case '\v':// 'v'
                c = 47; break;
            case ' ': // 's'
                c = 44; break;
            case '\t':// 't'
                c = 45; break;
            case '\n':// 'n'
                c = 39; break;
            case '\r':// 'r'
                c = 43; break;
            case '.': // 'A'
                c = 0; break;
            case ',': // 'B'
                c = 1; break;
            case '-': // 'C'
                c = 2; break;
            case '_': // 'D'
                c = 3; break;
            case '/': // 'E'
                c = 4; break;
            case ':': // 'F'
                c = 5; break;
            case ';': // 'G'
                c = 6; break;
            case '?': // 'H'
                c = 7; break;
            case '!': // 'I'
                c = 8; break;
            case '@': // 'J'
                c = 9; break;
            case '#': // 'K'
                c = 10; break;
            case '$': // 'L'
                c = 11; break;
            case '+': // 'M'
                c = 12; break;
            case '=': // 'N'
                c = 13; break;
            case '*': // 'O'
                c = 14; break;
            case '&': // 'P'
                c = 15; break;
            case '%': // 'Q'
                c = 16; break;
            case '^': // 'R'
                c = 17; break;
            case '(': // 'S'
                c = 18; break;
            case ')': // 'T'
                c = 19; break;
            case '[': // 'U'
                c = 20; break;
            case ']': // 'V'
                c = 21; break;
            case '{': // 'W'
                c = 22; break;
            case '}': // 'X'
                c = 23; break;
            case '<': // 'Y'
                c = 24; break;
            case '>': // 'Z'
                c = 25; break;
            case '\\':// '0'
                c = 52; break;
            case '|': // '1'
                c = 53; break;
            case '~': // '2'
                c = 54; break;
            case '`': // '3'
                c = 55; break;
            case '"': // '4'
                c = 56; break;
            case '\'':// '5'
                c = 57; break;
            default:
                return;     // Skip unsupported characters
        }
        _cp_base64_write_6_bits(buf, offset, (*index)++, 63); // Escape character + 
        _cp_base64_write_6_bits(buf, offset, (*index)++, c);
    }
}

void _cp_base64_encode(_cp_Buffer* buf, const _cp_Buffer* str, const size_t offset) {
    if (buf == NULL || str == NULL) return;

    // Approximate bits needed. In reality, may need more for special chars.
    size_t str_len = str->size;
    // Ensure buffer has enough capacity by doubling as needed
    while (_cp_buffer_remaining_capacity(buf) < str_len * 6) {
        _cp_buffer_double(buf);
    }
    size_t index = 0;
    for (size_t i = 0; _cp_buffer_char_at(str, i) != '\0' && i < str_len; i++) {
        char c1 = _cp_buffer_char_at(str, i);
        char c2 = _cp_buffer_char_at(str, i + 1);
        char c3 = _cp_buffer_char_at(str, i + 2);
        char c4 = _cp_buffer_char_at(str, i + 3);
        if ((unsigned char)c1 <= 127) {
            // If this is ASCII character
            _cp_base64_encode_ascii(buf, offset, &index, c1);
        } else if ((unsigned char)c1 >= 0xC2 && (unsigned char)c1 <= 0xDF) {
            // 2-byte UTF-8
            unsigned int codepoint = ((unsigned int)(c1 & 0x1F) << 6) | (unsigned int)(c2 & 0x3F);
            _cp_base64_encode_unicode(buf, offset, &index, codepoint);
            i += 1;
        } else if ((unsigned char)c1 >= 0xE0 && (unsigned char)c1 <= 0xEF) {
            // 3-byte UTF-8
            unsigned int codepoint = ((unsigned int)(c1 & 0x0F) << 12) | ((unsigned int)(c2 & 0x3F) << 6) | (unsigned int)(c3 & 0x3F);
            _cp_base64_encode_unicode(buf, offset, &index, codepoint);
            i += 2;
        } else if ((unsigned char)c1 >= 0xF0 && (unsigned char)c1 <= 0xF4) {
            // 4-byte UTF-8
            unsigned int codepoint = ((unsigned int)(c1 & 0x07) << 18) | ((unsigned int)(c2 & 0x3F) << 12) | ((unsigned int)(c3 & 0x3F) << 6) | (u_int32_t)(c4 & 0x3F);
            _cp_base64_encode_unicode(buf, offset, &index, codepoint);
            i += 3;
        } else {
            // Invalid UTF-8, skip
            continue;
        }
    }
    // Write the terminator if the last two characters are not 63, 63
    byte last1 = _cp_base64_read_6_bits(buf, offset, index - 1);
    byte last2 = _cp_base64_read_6_bits(buf, offset, index - 2);
    if (!(last1 == 63 && last2 == 63)) {
        _cp_base64_write_6_bits(buf, offset, index++, 63); // Escape character
        _cp_base64_write_6_bits(buf, offset, index++, 63);  // Null terminator
    }
}

void _cp_base64_decode(const _cp_Buffer* buf, const size_t offset, _cp_Buffer* out) {
    if (buf == NULL || out == NULL) return;

}