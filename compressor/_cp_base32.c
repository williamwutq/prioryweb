// Implement Encoding for Base 32 Format for Lowercase Letters.

#ifndef _CP_BASE32_C_
#include "_cp_base32.h"
#endif // _CP_BASE32_C_
#ifndef _CP_SYS_H_
#include "_cp_sys.h"
#endif // _CP_SYS_H_
#ifndef _STDLIB_H_
#include <stdlib.h>
#endif // _STDLIB_H_
#ifndef _STRING_H_
#include <string.h>
#endif // _STRING_H_

// Encoding functions

void _cp_base32_encode_char(_cp_Buffer* buf, const size_t offset, const size_t index, const char c) {
    switch (c){
        case '\0':
            _cp_base32_write_5_bits(buf, offset, index, 0);
            break;
        case '\\':
            _cp_base32_write_5_bits(buf, offset, index, 1);
            _cp_base32_write_5_bits(buf, offset, index, 1);
            break;
        case '\t':
            _cp_base32_write_5_bits(buf, offset, index, 1);
        case ' ':
            _cp_base32_write_5_bits(buf, offset, index, 2);
            break;
        case '\n':
        case '\r':
            _cp_base32_write_5_bits(buf, offset, index, 3);
            break;
        case '|':
            _cp_base32_write_5_bits(buf, offset, index, 4);
            break;
        case '0':
            _cp_base32_write_5_bits(buf, offset, index, 1);
        case 'a':
            _cp_base32_write_5_bits(buf, offset, index, 5);
            break;
        case '1':
            _cp_base32_write_5_bits(buf, offset, index, 1);
        case 'b':
            _cp_base32_write_5_bits(buf, offset, index, 6);
            break;
        case '2':
            _cp_base32_write_5_bits(buf, offset, index, 1);
        case 'c':
            _cp_base32_write_5_bits(buf, offset, index, 7);
            break;
        case '3':
            _cp_base32_write_5_bits(buf, offset, index, 1);
        case 'd':
            _cp_base32_write_5_bits(buf, offset, index, 8);
            break;
        case '4':
            _cp_base32_write_5_bits(buf, offset, index, 1);
        case 'e':
            _cp_base32_write_5_bits(buf, offset, index, 9);
            break;
        case '5':
            _cp_base32_write_5_bits(buf, offset, index, 1);
        case 'f':
            _cp_base32_write_5_bits(buf, offset, index, 10);
            break;
        case '6':
            _cp_base32_write_5_bits(buf, offset, index, 1);
        case 'g':
            _cp_base32_write_5_bits(buf, offset, index, 11);
            break;
        case '7':
            _cp_base32_write_5_bits(buf, offset, index, 1);
        case 'h':
            _cp_base32_write_5_bits(buf, offset, index, 12);
            break;
        case '8':
            _cp_base32_write_5_bits(buf, offset, index, 1);
        case 'i':
            _cp_base32_write_5_bits(buf, offset, index, 13);
            break;
        case '9':
            _cp_base32_write_5_bits(buf, offset, index, 1);
        case 'j':
            _cp_base32_write_5_bits(buf, offset, index, 14);
            break;
        case '.':
            _cp_base32_write_5_bits(buf, offset, index, 2);
        case 'k':
            _cp_base32_write_5_bits(buf, offset, index, 15);
            break;
        case ',':
            _cp_base32_write_5_bits(buf, offset, index, 2);
        case 'l':
            _cp_base32_write_5_bits(buf, offset, index, 16);
            break;
        case '-':
            _cp_base32_write_5_bits(buf, offset, index, 2);
        case 'm':
            _cp_base32_write_5_bits(buf, offset, index, 17);
            break;
        case '_':
            _cp_base32_write_5_bits(buf, offset, index, 2);
        case 'n':
            _cp_base32_write_5_bits(buf, offset, index, 18);
            break;
        case '/':
            _cp_base32_write_5_bits(buf, offset, index, 2);
        case 'o':
            _cp_base32_write_5_bits(buf, offset, index, 19);
            break;
        case ':':
            _cp_base32_write_5_bits(buf, offset, index, 3);
        case 'p':
            _cp_base32_write_5_bits(buf, offset, index, 20);
            break;
        case ';':
            _cp_base32_write_5_bits(buf, offset, index, 3);
        case 'q':
            _cp_base32_write_5_bits(buf, offset, index, 21);
            break;
        case '?':
            _cp_base32_write_5_bits(buf, offset, index, 3);
        case 'r':
            _cp_base32_write_5_bits(buf, offset, index, 22);
            break;
        case '!':
            _cp_base32_write_5_bits(buf, offset, index, 3);
        case 's':
            _cp_base32_write_5_bits(buf, offset, index, 23);
            break;
        case '@':
            _cp_base32_write_5_bits(buf, offset, index, 3);
        case 't':
            _cp_base32_write_5_bits(buf, offset, index, 24);
            break;
        case '#':
            _cp_base32_write_5_bits(buf, offset, index, 3);
        case 'u':
            _cp_base32_write_5_bits(buf, offset, index, 25);
            break;
        case '$':
            _cp_base32_write_5_bits(buf, offset, index, 3);
        case 'v':
            _cp_base32_write_5_bits(buf, offset, index, 26);
            break;
        case '+':
            _cp_base32_write_5_bits(buf, offset, index, 4);
        case 'w':
            _cp_base32_write_5_bits(buf, offset, index, 27);
            break;
        case '=':
            _cp_base32_write_5_bits(buf, offset, index, 4);
        case 'x':
            _cp_base32_write_5_bits(buf, offset, index, 28);
            break;
        case '*':
            _cp_base32_write_5_bits(buf, offset, index, 4);
        case 'y':
            _cp_base32_write_5_bits(buf, offset, index, 29);
            break;
        case '&':
            _cp_base32_write_5_bits(buf, offset, index, 4);
        case 'z':
            _cp_base32_write_5_bits(buf, offset, index, 30);
            break;
        default:
            // Unsupported character, encode as terminator
            _cp_base32_write_5_bits(buf, offset, index, 0);
            break;
    }
}

void _cp_base32_encode_terminator(_cp_Buffer* buf, const size_t offset, const size_t index) {
    _cp_base32_write_5_bits(buf, offset, index, 0);
}

void _cp_base32_encode_starter(_cp_Buffer* buf, const size_t offset, const size_t index) {
    _cp_base32_write_5_bits(buf, offset, index, 1);
    _cp_base32_write_5_bits(buf, offset, index, 0);
}

void _cp_base32_encode_nbsp(_cp_Buffer* buf, const size_t offset, const size_t index) {
    _cp_base32_write_5_bits(buf, offset, index, 1);
    _cp_base32_write_5_bits(buf, offset, index, 3);
}

void _cp_base32_encode_endl(_cp_Buffer* buf, const size_t offset, const size_t index) {
    _cp_base32_write_5_bits(buf, offset, index, 3);
}

void _cp_base32_write_5_bits(_cp_Buffer* buf, const size_t offset, const size_t index, byte value) {
    if (buf == NULL) return;
    if (value >= 32) value %= 32; // Ensure value is within 5 bits

    size_t bit_pos = index * 5;
    size_t byte_pos = bit_pos / 8 + offset;
    size_t bit_offset = bit_pos % 8;

    // Number of bits going into current byte
    size_t bits_in_current = 8 - bit_offset;
    if (bits_in_current > 5) bits_in_current = 5;

    size_t bits_in_next = 5 - bits_in_current;

    // Determine required size
    size_t required = byte_pos + 1;
    if (bits_in_next > 0) required++;

    // Expand buffer if needed
    if (required > buf->capacity) {
        buf = _cp_buffer_expand(buf, required - buf->capacity);
    }

    // Clear and write current byte part
    byte current_mask = (1 << bits_in_current) - 1;
    buf->data[byte_pos] &= ~(current_mask << bit_offset);
    buf->data[byte_pos] |= ((value << (5 - bits_in_current)) & 0x1F) << bit_offset;

    // Write overflow into next byte if any
    if (bits_in_next > 0) {
        byte next_mask = (1 << bits_in_next) - 1;
        buf->data[byte_pos + 1] &= ~next_mask;
        buf->data[byte_pos + 1] |= (value >> bits_in_current) & next_mask;
    }
}

void _cp_base32_encode(_cp_Buffer* buf, const _cp_Buffer* str, const size_t offset) {
    if (buf == NULL || str == NULL) return;

    // Approximate bits needed. In reality, may need more for special chars.
    size_t str_len = str->size;
    size_t required_bits = str_len * 5 + 10;
    // Ensure buffer has enough capacity by doubling as needed
    while (_cp_buffer_remaining_capacity(buf) < required_bits) {
        _cp_buffer_double(buf);
    }

    size_t index = 0;
    _cp_base32_encode_starter(buf, offset, index++);
    for (size_t i = 0; _cp_buffer_char_at(str, i) != '\0' && i < str_len; i++) {
        char c = _cp_buffer_char_at(str, i);
        char n = _cp_buffer_char_at(str, i + 1);
        // Case: \n\r or \r\n
        if ((c == '\n' && n == '\r') || (c == '\r' && n == '\n')) {
            _cp_base32_encode_endl(buf, offset, index++);
            i++; // Skip next character
            continue;
        }
        // Case: non-breaking space (U+00A0)
        if ((unsigned char)c == 0xC2 && (unsigned char)n == 0xA0) {
            _cp_base32_encode_nbsp(buf, offset, index++);
            i++; // Skip next byte
            continue;
        }
        // Pattern: &nbsp; or &#160; for web representation of non-breaking space
        if (_cp_buffer_bufcomp(str, _cp_buffer_from_cstr_const("&nbsp;"), index, 0, 6) == 0 || _cp_buffer_bufcomp(str, _cp_buffer_from_cstr_const("&#160;"), index, 0, 6) == 0) {
            _cp_base32_encode_nbsp(buf, offset, index++);
            i += 5; // Skip next 5 characters
            continue;
        }
        // Encode regular character
        _cp_base32_encode_char(buf, offset, index++, c);
    }
    // Encode terminator
    _cp_base32_encode_terminator(buf, offset, index++);
    // Make sure the byte at index is padded with zeros
    size_t bit_pos = index * 5;
    size_t byte_pos = bit_pos / 8 + offset;
    if (bit_pos % 8 != 0) {
        if (byte_pos >= buf->capacity) {
            _cp_buffer_expand(buf, byte_pos + 1 - buf->capacity);
        }
        byte remaining_bits = 8 - (bit_pos % 8);
        byte mask = (1 << remaining_bits) - 1;
        buf->data[byte_pos] &= ~mask; // Clear remaining bits
    }
}

// Decoding functions
char _cp_base32_read_5_bits(_cp_Buffer* buf, size_t offset, size_t index) {
    if (buf == NULL) return 0;

    size_t bit_pos = index * 5;
    size_t byte_pos = bit_pos / 8 + offset;
    size_t bit_offset = bit_pos % 8;

    // Number of bits in current byte
    size_t bits_in_current = 8 - bit_offset;
    if (bits_in_current > 5) bits_in_current = 5;

    size_t bits_in_next = 5 - bits_in_current;

    byte value = 0;

    // Protection against out-of-bounds
    if (byte_pos >= buf->capacity) return 0;

    // Read current byte part
    byte current_mask = (1 << bits_in_current) - 1;
    value |= (buf->data[byte_pos] >> bit_offset) & current_mask;

    // Read overflow from next byte if any
    if (bits_in_next > 0) {
        byte next_mask = (1 << bits_in_next) - 1;
        value |= (buf->data[byte_pos + 1] & next_mask) << bits_in_current;
    }

    return value & 0x1F; // Return only 5 bits
}

bool _cp_base32_decode_nonescape(_cp_Buffer* buf, size_t offset, size_t index, _cp_Buffer* out) {
    if (buf == NULL || out == NULL) return false;

    byte value = _cp_base32_read_5_bits(buf, offset, index);
    switch (value) {
        case 0:
            _cp_buffer_append_char(out, '\0');
            return 8; // Indicate termination
        case 1:
            return true; // Indicate escape sequence
        case 2:
            _cp_buffer_append_char(out, ' ');
            break;
        case 3:
            _cp_buffer_append_char(out, '\n');
            _cp_buffer_append_char(out, '\r');
            break;
        case 4:
            _cp_buffer_append_char(out, '|');
            break;
        case 31:
            // Reserved character. Append nothing but log warning.
            _cp_warn("Encountered reserved base32 value during decoding. Ignoring. Data may be corrupted or header misplaced.");
            break;
        default:
            if (value >= 5 && value <= 30) {
                char decoded_char = (char)(value - 5 + 'a');
                _cp_buffer_append_char(out, decoded_char);
            } else {
                // Unsupported value, treat as terminator
                _cp_buffer_append_char(out, '\0');
                return 8; // Indicate termination
            }
            break;
    }
    return false; // No escape sequence
}

bool _cp_base32_decode_escape(_cp_Buffer* buf, size_t offset, size_t index, _cp_Buffer* out) {
    if (buf == NULL || out == NULL) return false;

    byte value = _cp_base32_read_5_bits(buf, offset, index);
    switch (value) {
        case 0:
            // The standard says that parsers can disregard start of sequence even if it is in the middle of data.
            break;
        case 1: 
            _cp_buffer_append_char(out, '\\');
            break;
        case 2:
            _cp_buffer_append_char(out, '\t');
            break;
        case 3:
            // Append nbsp character (U+00A0)
            _cp_buffer_append_char(out, (char)0xC2);
            _cp_buffer_append_char(out, (char)0xA0);
            break;
        case 4:
            // Read unicode characters. They are not supported in this implementation, but we can skip them.
            // Still, we send a signal to toggle the mode.
            return 2; // Indicate unicode escape sequence (this is valid "boolean")
        case 5 ... 14:
            {
                char decoded_char = (char)(value - 5 + '0');
                _cp_buffer_append_char(out, decoded_char);
            }
            break;
        case 15:
            _cp_buffer_append_char(out, '.');
            break;
        case 16:
            _cp_buffer_append_char(out, ',');
            break;
        case 17:
            _cp_buffer_append_char(out, '-');
            break;
        case 18:
            _cp_buffer_append_char(out, '_');
            break;
        case 19:
            _cp_buffer_append_char(out, '/');
            break;
        case 20:
            _cp_buffer_append_char(out, ':');
            break;
        case 21:
            _cp_buffer_append_char(out, ';');
            break;
        case 22:
            _cp_buffer_append_char(out, '?');
            break;
        case 23:
            _cp_buffer_append_char(out, '!');
            break;
        case 24:
            _cp_buffer_append_char(out, '@');
            break;
        case 25:
            _cp_buffer_append_char(out, '#');
            break;
        case 26:
            _cp_buffer_append_char(out, '$');
            break;
        case 27:
            _cp_buffer_append_char(out, '+');
            break;
        case 28:
            _cp_buffer_append_char(out, '=');
            break;
        case 29:
            _cp_buffer_append_char(out, '*');
            break;
        case 30:
            _cp_buffer_append_char(out, '&');
            break;
        case 31:
            // Reserved character. Append nothing but log warning.
            _cp_warn("Encountered reserved base32 value during decoding. Ignoring. Data may be corrupted or header misplaced.");
            break;
        default:
            // Unsupported value, treat as terminator
            _cp_buffer_append_char(out, '\0');
            return 8; // Indicate termination
    }
    return false; // No further escape sequence
}

void _cp_base32_decode(_cp_Buffer* buf, size_t offset, _cp_Buffer* out) {
    if (buf == NULL || out == NULL) return;
    bool in_escape = false;
    for (size_t index = 0; in_escape != 8; index ++) {
        if (in_escape == 2) {
            unsigned char length = _cp_base32_read_5_bits(buf, offset, index);
            // Skip unicode characters
            index += length;
        } else if (in_escape) {
            in_escape = _cp_base32_decode_escape(buf, offset, index, out);
        } else {
            in_escape = _cp_base32_decode_nonescape(buf, offset, index, out);
        }
    }
}

