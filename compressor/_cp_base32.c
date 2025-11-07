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
#ifndef _STDIO_H_
#include <stdio.h>
#endif // _STDIO_H_

// Encoding functions

void _cp_base32_encode_char(_cp_Buffer* buf, const size_t offset, size_t* index, const char c) {
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
            _cp_base32_write_5_bits(buf, offset, index, 1);
        case 'k':
            _cp_base32_write_5_bits(buf, offset, index, 15);
            break;
        case ',':
            _cp_base32_write_5_bits(buf, offset, index, 1);
        case 'l':
            _cp_base32_write_5_bits(buf, offset, index, 16);
            break;
        case '-':
            _cp_base32_write_5_bits(buf, offset, index, 1);
        case 'm':
            _cp_base32_write_5_bits(buf, offset, index, 17);
            break;
        case '_':
            _cp_base32_write_5_bits(buf, offset, index, 1);
        case 'n':
            _cp_base32_write_5_bits(buf, offset, index, 18);
            break;
        case '/':
            _cp_base32_write_5_bits(buf, offset, index, 1);
        case 'o':
            _cp_base32_write_5_bits(buf, offset, index, 19);
            break;
        case ':':
            _cp_base32_write_5_bits(buf, offset, index, 1);
        case 'p':
            _cp_base32_write_5_bits(buf, offset, index, 20);
            break;
        case ';':
            _cp_base32_write_5_bits(buf, offset, index, 1);
        case 'q':
            _cp_base32_write_5_bits(buf, offset, index, 21);
            break;
        case '?':
            _cp_base32_write_5_bits(buf, offset, index, 1);
        case 'r':
            _cp_base32_write_5_bits(buf, offset, index, 22);
            break;
        case '!':
            _cp_base32_write_5_bits(buf, offset, index, 1);
        case 's':
            _cp_base32_write_5_bits(buf, offset, index, 23);
            break;
        case '@':
            _cp_base32_write_5_bits(buf, offset, index, 1);
        case 't':
            _cp_base32_write_5_bits(buf, offset, index, 24);
            break;
        case '*':
            _cp_base32_write_5_bits(buf, offset, index, 1);
        case 'u':
            _cp_base32_write_5_bits(buf, offset, index, 25);
            break;
        case '$':
            _cp_base32_write_5_bits(buf, offset, index, 1);
        case 'v':
            _cp_base32_write_5_bits(buf, offset, index, 26);
            break;
        case '+':
            _cp_base32_write_5_bits(buf, offset, index, 1);
        case 'w':
            _cp_base32_write_5_bits(buf, offset, index, 27);
            break;
        case '=':
            _cp_base32_write_5_bits(buf, offset, index, 1);
        case 'x':
            _cp_base32_write_5_bits(buf, offset, index, 28);
            break;
        case '"':
            _cp_base32_write_5_bits(buf, offset, index, 1);
        case 'y':
            _cp_base32_write_5_bits(buf, offset, index, 29);
            break;
        case '\'':
            _cp_base32_write_5_bits(buf, offset, index, 1);
        case 'z':
            _cp_base32_write_5_bits(buf, offset, index, 30);
            break;
        default:
            // Unsupported character, encode as terminator
            _cp_base32_write_5_bits(buf, offset, index, 0);
            break;
    }
}

void _cp_base32_encode_terminator(_cp_Buffer* buf, const size_t offset, size_t* index) {
    _cp_base32_write_5_bits(buf, offset, index, 0);
}

void _cp_base32_encode_starter(_cp_Buffer* buf, const size_t offset, size_t* index) {
    _cp_base32_write_5_bits(buf, offset, index, 1);
    _cp_base32_write_5_bits(buf, offset, index, 0);
}

void _cp_base32_encode_nbsp(_cp_Buffer* buf, const size_t offset, size_t* index) {
    _cp_base32_write_5_bits(buf, offset, index, 1);
    _cp_base32_write_5_bits(buf, offset, index, 3);
}

void _cp_base32_encode_endl(_cp_Buffer* buf, const size_t offset, size_t* index) {
    _cp_base32_write_5_bits(buf, offset, index, 3);
}

void _cp_base32_write_5_bits(_cp_Buffer* buf, const size_t offset, size_t* index, byte value) {
    if (buf == NULL) return;
    if (value >= 32) value %= 32; // Ensure value is within 5 bits

    size_t bit_pos = *index * 5;
    size_t byte_pos = bit_pos / 8 + offset;
    size_t bit_offset = bit_pos % 8;

    // Ensure enough space: we may need up to 2 bytes (5 bits can cross byte boundary)
    size_t required = byte_pos + 2;
    if (required > buf->capacity) {
        buf = _cp_buffer_expand(buf, required - buf->capacity);
        if (buf == NULL) return; // Expansion failed
    }

    // Clear the 5-bit target area across 1 or 2 bytes
    // We'll write bit-by-bit to avoid assumptions about endianness or alignment

    for (int i = 0; i < 5; ++i) {
        size_t current_bit = bit_pos + i;
        size_t target_byte = current_bit / 8 + offset;
        size_t target_bit  = current_bit % 8;
        byte bit_value = (value >> (4 - i)) & 1; // MSB first

        // Clear the target bit
        buf->data[target_byte] &= ~(1 << (7 - target_bit));
        // Set the target bit
        buf->data[target_byte] |= (bit_value << (7 - target_bit));
    }

    // Update buffer size if needed
    if (required > buf->size) {
        buf->size = required;
    }

    // Advance index for next 5-bit write
    (*index)++;
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
    _cp_base32_encode_starter(buf, offset, &index);
    for (size_t i = 0; _cp_buffer_char_at(str, i) != '\0' && i < str_len; i++) {
        char c = _cp_buffer_char_at(str, i);
        char n = _cp_buffer_char_at(str, i + 1);
        // Case: \n\r or \r\n
        if ((c == '\n' && n == '\r') || (c == '\r' && n == '\n')) {
            _cp_base32_encode_endl(buf, offset, &index);
            i++; // Skip next character
            continue;
        }
        // Case: non-breaking space (U+00A0)
        if ((unsigned char)c == 0xC2 && (unsigned char)n == 0xA0) {
            _cp_base32_encode_nbsp(buf, offset, &index);
            i++; // Skip next byte
            continue;
        }
        // Pattern: &nbsp; or &#160; for web representation of non-breaking space
        if (_cp_buffer_bufcomp(str, _cp_buffer_from_cstr_const("&nbsp;"), i, 0, 6) == 0 || _cp_buffer_bufcomp(str, _cp_buffer_from_cstr_const("&#160;"), i, 0, 6) == 0) {
            _cp_base32_encode_nbsp(buf, offset, &index);
            i += 5;
            continue;
        }
        // Encode regular character
        _cp_base32_encode_char(buf, offset, &index, c);
    }
    // Encode terminator
    _cp_base32_encode_terminator(buf, offset, &index);
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
char _cp_base32_read_5_bits(_cp_Buffer* buf, size_t offset, size_t index)
{
    if (buf == NULL || buf->data == NULL) return 0;
    size_t bit_pos   = index * 5;
    size_t byte_pos  = bit_pos / 8 + offset;
    size_t bit_off   = bit_pos % 8;

    /* we never need more than 2 bytes for 5 bits */
    size_t required = byte_pos + 2;
    if (required > buf->size) return 0;

    char value = 0;

    for (int i = 0; i < 5; ++i) {
        size_t cur_bit   = bit_pos + i;
        size_t cur_byte  = cur_bit / 8 + offset;
        size_t cur_off   = cur_bit % 8;
        /* read the bit (bit 7 = MSB of the byte) */
        int bit = (buf->data[cur_byte] >> (7 - cur_off)) & 1;

        value = (value << 1) | bit;
    }
    return value;
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
            _cp_buffer_append_char(out, '*');
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
            _cp_buffer_append_char(out, '"');
            break;
        case 30:
            _cp_buffer_append_char(out, '\'');
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

bool _cp_base32_check_format(const _cp_Buffer* buf, const size_t start, const size_t size, const bool ignore_premature) {
    if (buf == NULL) return false;
    // Scan string character by character
    int idx = start; char c;
    while (idx < start + size) {
        c = _cp_buffer_char_at(buf, idx);
        // Normal character check
        switch (c) {
            case '\0':
                if (!ignore_premature) return false;
                // Otherwise it will go to idx++ break 4 lines below
            case '\t': case '\n': case '\r':
            case 32 ... 34: case 36: // ' ', '!', '"', '$'
                idx++; continue;
            case 38: // Only allowed in &nbsp; and &#160; sequences
                if (_cp_buffer_bufcomp(buf, _cp_buffer_from_cstr_const("&nbsp;"), idx, 0, 6) == 0
                 || _cp_buffer_bufcomp(buf, _cp_buffer_from_cstr_const("&#160;"), idx, 0, 6) == 0) {
                    idx += 6;
                    continue;
                } else {
                    return false;
                }
            case 39: case 42 ... 59: // ''', '*', '+', ',', '-', '.', '/', '0'-'9', ':', ';'
            case 61: case 63: case 64: case 92: case 95: // '=', '?', '@', '\', '_'
            case 97 ... 122: // 'a' - 'z'
            case 124: // '|'
                idx++; continue;
            // &nbsp; (U+00A0)
            case -62:
                if ((unsigned char)_cp_buffer_char_at(buf, idx + 1) == 0xA0) {
                    idx += 2; // Skip both bytes
                    continue;
                }
            default:
                return false;
        }
    }
    return true;
}

void _cp_base32_correct_format(const _cp_Buffer* buf, const size_t start, const size_t size, const bool ignore_premature) {
    if (buf == NULL) return;
    // &nbsp; (U+00A0)
    // Scan string character by character
    int idx = start; char c;
    while (idx < start + size) {
        c = _cp_buffer_char_at(buf, idx);
        // Normal character check
        switch (c) {
            case '\0':
                if (!ignore_premature) ((char*)buf->data)[idx] = ' ';
                // Otherwise it will go to idx++ break 4 lines below
            case '\t': case '\n': case '\r':
            case 32 ... 34: case 36: // ' ', '!', '"', '$'
                idx++; continue;
            case 38: // Only allowed in &nbsp; and &#160; sequences
                if (_cp_buffer_bufcomp(buf, _cp_buffer_from_cstr_const("&nbsp;"), idx, 0, 6) == 0
                 || _cp_buffer_bufcomp(buf, _cp_buffer_from_cstr_const("&#160;"), idx, 0, 6) == 0) {
                    idx += 6;
                    continue;
                } else {
                    ((char*)buf->data)[idx] = ' ';
                }
            case 39: case 42 ... 59: // ''', '*', '+', ',', '-', '.', '/', '0'-'9', ':', ';'
            case 61: case 63: case 64: case 92: case 95: // '=', '?', '@', '\', '_'
            case 97 ... 122: // 'a' - 'z'
            case 124: // '|'
                idx++; break;
            // &nbsp; (U+00A0)
            case -62:
                if ((unsigned char)_cp_buffer_char_at(buf, idx + 1) == 0xA0) {
                    idx += 2; // Skip both bytes
                    continue;
                }
            default:
                // Replace with space
                ((char*)buf->data)[idx] = ' ';
                idx++;
                break;
        }
    }
}