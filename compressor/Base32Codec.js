import * as CodecBuffer from './CodecBuffer.js';

// Format in format.md

/**
 * Reads 5 bits from the buffer starting at the specified offset.
 * @param {Uint8Array} buffer - The buffer to read from.
 * @param {number} index - The index of the 5-bit unit to read.
 * @param {number} offset - The offset in bytes.
 * @returns {number} The 5-bit value read from the buffer.
 */
function read5Bits(buffer, index, offset) {
    const bit_pos   = index * 5;
    const byte_pos  = Math.floor(bit_pos / 8) + offset;
    const required  = byte_pos + 2;

    // Bounds check: we need up to 2 bytes
    if (required > buffer.size) {
        return 0;
    }
    let value = 0;
    // Read 5 bits one by one, MSB first
    for (let i = 0; i < 5; ++i) {
        const cur_bit   = bit_pos + i;
        const cur_byte  = Math.floor(cur_bit / 8) + offset;
        const cur_off   = cur_bit % 8;
        // Extract bit: shift right by (7 - bit_position_in_byte), mask with 1
        const bit = (buffer[cur_byte] >> (7 - cur_off)) & 1;
        // Accumulate into value (shift left and OR)
        value = (value << 1) | bit;
    }
    return value;
}

/**
 * Writes a 5-bit value into the buffer starting at the specified offset.
 * @param {Uint8Array} buffer - The buffer to write to.
 * @param {number} index - The index of the 5-bit unit to write.
 * @param {number} offset - The offset in 5-bit units.
 * @param {number} value - The 5-bit value to write.
 */
function write5Bits(buffer, index, offset, value) {
    // Ensure value is within 5 bits
    if (value >= 32) value %= 32;

    const bit_pos = index * 5;
    const byte_pos = Math.floor(bit_pos / 8) + offset;

    CodecBuffer.ensureCapacity(buffer, byte_pos + 2); // Ensure enough space

    // Write 5 bits one by one, MSB first
    for (let i = 0; i < 5; ++i) {
        const current_bit = bit_pos + i;
        const target_byte = Math.floor(current_bit / 8) + offset;
        const target_bit = current_bit % 8;
        const bit_value = (value >> (4 - i)) & 1; // Extract bit (MSB first)

        // Clear the target bit: mask with ~(1 << (7 - target_bit))
        const mask = ~(1 << (7 - target_bit)) & 0xFF;
        buffer[target_byte] &= mask;

        // Set the target bit
        buffer[target_byte] |= (bit_value << (7 - target_bit));
    }
}

/**
 * Encodes a string into the custom Base32 format and writes it into the output buffer.
 * @param {Int8Array} input - The input string as an Int8Array.
 * @param {Uint8Array} output - The output buffer to write the Base32 encoded data.
 * @param {number} offset - The offset in bytes to start writing in the output buffer.
 */
// TODO: Handle &nbsp; (&nbsp;, &#160;, 0xC2A0), possibly even unicode (C version will not support unicode but it is in the specs)
function encodeBase32(input, output, offset) {
    // Read char with readCharCode until null terminator or end of input
    let inputIndex = 0;
    let outputIndex = 2;
    // Always write the start symbol for compatibility
    write5Bits(output, 0, offset, 1);
    write5Bits(output, 1, offset, 0);
    while (true) {
        let charCode = CodecBuffer.readCharCode(input, inputIndex);
        if (inputIndex >= input.length || charCode === 0) {
            write5Bits(output, outputIndex, offset, 0); // Write null terminator
            break;
        }
        // Maybe optimize this with a map
        // Support &nbsp; and &#160;
        if (CodecBuffer.readString(input, inputIndex, 6) === '&nbsp;' || CodecBuffer.readString(input, inputIndex, 7) === '&#160;') {
            // Write escape + 0b00011
            write5Bits(output, outputIndex, offset, 1);
            outputIndex++;
            write5Bits(output, outputIndex, offset, 3);
            inputIndex += 5;
        }
        // Support 0xC2A0
        else if (charCode === 0xC2) {
            let nextCharCode = CodecBuffer.readCharCode(input, inputIndex + 1);
            if (nextCharCode === 0xA0) {
                // Write escape + 0b00011
                write5Bits(output, outputIndex, offset, 1);
                outputIndex++;
                write5Bits(output, outputIndex, offset, 3);
                inputIndex++; // Skip next byte
            } else {
                // Unsupported character: use null terminator but does not terminate
                write5Bits(output, outputIndex, offset, 0);
            }
        } 
        else
        switch (charCode) {
            // '\' -> escape 0b00001
            case 92:
                write5Bits(output, outputIndex, offset, 1); outputIndex++;
                write5Bits(output, outputIndex, offset, 1);
                break;
            // ' ' -> 0b00010; '\t' -> escape 0b00010
            case 32:
                write5Bits(output, outputIndex, offset, 1); outputIndex++;
            case 9:
                write5Bits(output, outputIndex, offset, 2);
                break;
            // '\n' or '\r' -> 0b00011
            case 10:
            case 13:
                write5Bits(output, outputIndex, offset, 3);
                break;
            // '|' -> 0b00100
            case 124:
                write5Bits(output, outputIndex, offset, 4);
                break;
            // '0' to '9' -> escape 0b00101 to escape 0b01110
            case charCode >= 48 && charCode <= 57:
                write5Bits(output, outputIndex, offset, 5); outputIndex++;
                write5Bits(output, outputIndex, offset, charCode - 43);
                break;
            // '.' -> escape 0b01111
            case 46:
                write5Bits(output, outputIndex, offset, 5); outputIndex++;
                write5Bits(output, outputIndex, offset, 15);
                break;
            // ',' -> escape 0b10000
            case 44:
                write5Bits(output, outputIndex, offset, 5); outputIndex++;
                write5Bits(output, outputIndex, offset, 16);
                break;
            // '-' -> escape 0b10001
            case 45:
                write5Bits(output, outputIndex, offset, 5); outputIndex++;
                write5Bits(output, outputIndex, offset, 17);
                break;
            // '_' -> escape 0b10010
            case 95:
                write5Bits(output, outputIndex, offset, 5); outputIndex++;
                write5Bits(output, outputIndex, offset, 18);
                break;
            // '/' -> escape 0b10011
            case 47:
                write5Bits(output, outputIndex, offset, 5); outputIndex++;
                write5Bits(output, outputIndex, offset, 19);
                break;
            // ':' -> escape 0b10100
            case 58:
                write5Bits(output, outputIndex, offset, 5); outputIndex++;
                write5Bits(output, outputIndex, offset, 20);
                break;
            // ';' -> escape 0b10101
            case 59:
                write5Bits(output, outputIndex, offset, 5); outputIndex++;
                write5Bits(output, outputIndex, offset, 21);
                break;
            // '?' -> escape 0b10110
            case 63:
                write5Bits(output, outputIndex, offset, 5); outputIndex++;
                write5Bits(output, outputIndex, offset, 22);
                break;
            // '!' -> escape 0b10111
            case 33:
                write5Bits(output, outputIndex, offset, 5); outputIndex++;
                write5Bits(output, outputIndex, offset, 23);
                break;
            // '@' -> escape 0b11000
            case 64:
                write5Bits(output, outputIndex, offset, 5); outputIndex++;
                write5Bits(output, outputIndex, offset, 24);
                break;
            // '*' -> escape 0b11001
            case 42:
                write5Bits(output, outputIndex, offset, 5); outputIndex++;
                write5Bits(output, outputIndex, offset, 25);
                break;
            // '$' -> escape 0b11010
            case 36:
                write5Bits(output, outputIndex, offset, 5); outputIndex++;
                write5Bits(output, outputIndex, offset, 26);
                break;
            // '+' -> escape 0b11011
            case 43:
                write5Bits(output, outputIndex, offset, 5); outputIndex++;
                write5Bits(output, outputIndex, offset, 27);
                break;
            // '=' -> escape 0b11100
            case 61:
                write5Bits(output, outputIndex, offset, 5); outputIndex++;
                write5Bits(output, outputIndex, offset, 28);
                break;
            // '"' -> escape 0b11101
            case 34:
                write5Bits(output, outputIndex, offset, 5); outputIndex++;
                write5Bits(output, outputIndex, offset, 29);
                break;
            // ''' -> escape 0b11110
            case 39:
                write5Bits(output, outputIndex, offset, 5); outputIndex++;
                write5Bits(output, outputIndex, offset, 30);
                break;
            default:
                if (charCode >= 97 && charCode <= 122){
                    // 'a' to 'z' -> 0b00101 to 0b11110
                    write5Bits(output, outputIndex, offset, charCode - 92);
                    break;
                } else if (charCode >= 65 && charCode <= 90) {
                    // 'A' to 'Z' -> 0b00101 to 0b11110
                    // The format does not allow uppercase letters, but we represent them with lowercase
                    write5Bits(output, outputIndex, offset, charCode - 60);
                    break;
                } else {
                    // Unsupported character: use null terminator but does not terminate
                    write5Bits(output, outputIndex, offset, 0);
                }
        }
        inputIndex++;
        outputIndex++;
    }
}

/**
 * Decodes a Base32 encoded buffer into a string and writes it into the output buffer.
 * @param {Uint8Array} input - The input buffer containing Base32 encoded data.
 * @param {Int8Array} output - The output buffer to write the decoded string.
 * @param {number} offset - The offset in bytes to start reading from the input buffer.
 */
function decodeBase32(input, output, offset) {
    // Setup
    let inputIndex = 0;
    let outputIndex = 0;
    let mode = 0; // 0 = normal, 1 = escape

    while (true) {
        let value = read5Bits(input, inputIndex, offset);
        if (mode === 1) {
            // Escape mode
            switch (value) {
                case 0:
                    // Start symbol: do nothing
                    break;
                case 1:
                    CodecBuffer.writeCharCode(output, outputIndex, 92); // '\'
                    outputIndex++;
                    break;
                case 2:
                    CodecBuffer.writeCharCode(output, outputIndex, 9); // Tab
                    outputIndex++;
                    break;
                case 3:
                    CodecBuffer.writeCharCode(output, outputIndex, 194); // 0xC2
                    outputIndex++;
                    CodecBuffer.writeCharCode(output, outputIndex, 160); // 0xA0
                    outputIndex++;
                    break;
                case 4:
                    // Switch mode to unicode
                    mode = 4;
                    outputIndex++;
                    continue;
                case 5:
                case 6:
                case 7:
                case 8:
                case 9:
                case 10:
                case 11:
                case 12:
                case 13:
                case 14:
                    CodecBuffer.writeCharCode(output, outputIndex, value + 43); // '0' to '9'
                    outputIndex++;
                    break;
                case 15:
                    CodecBuffer.writeCharCode(output, outputIndex, 46); // '.'
                    outputIndex++;
                    break;
                case 16:
                    CodecBuffer.writeCharCode(output, outputIndex, 44); // ','
                    outputIndex++;
                    break;
                case 17:
                    CodecBuffer.writeCharCode(output, outputIndex, 45); // '-'
                    outputIndex++;
                    break;
                case 18:
                    CodecBuffer.writeCharCode(output, outputIndex, 95); // '_'
                    outputIndex++;
                    break;
                case 19:
                    CodecBuffer.writeCharCode(output, outputIndex, 47); // '/'
                    outputIndex++;
                    break;
                case 20:
                    CodecBuffer.writeCharCode(output, outputIndex, 58); // ':'
                    outputIndex++;
                    break;
                case 21:
                    CodecBuffer.writeCharCode(output, outputIndex, 59); // ';'
                    outputIndex++;
                    break;
                case 22:
                    CodecBuffer.writeCharCode(output, outputIndex, 63); // '?'
                    outputIndex++;
                    break;
                case 23:
                    CodecBuffer.writeCharCode(output, outputIndex, 33); // '!'
                    outputIndex++;
                    break;
                case 24:
                    CodecBuffer.writeCharCode(output, outputIndex, 64); // '@'
                    outputIndex++;
                    break;
                case 25:
                    CodecBuffer.writeCharCode(output, outputIndex, 42); // '*'
                    outputIndex++;
                    break;
                case 26:
                    CodecBuffer.writeCharCode(output, outputIndex, 36); // '$'
                    outputIndex++;
                    break;
                case 27:
                    CodecBuffer.writeCharCode(output, outputIndex, 43); // '+'
                    outputIndex++;
                    break;
                case 28:
                    CodecBuffer.writeCharCode(output, outputIndex, 61); // '='
                    outputIndex++;
                    break;
                case 29:
                    CodecBuffer.writeCharCode(output, outputIndex, 34); // '"'
                    outputIndex++;
                    break;
                case 30:
                    CodecBuffer.writeCharCode(output, outputIndex, 39); // '''
                    outputIndex++;
                    break;
                default:
                    // Reserved: do nothing
                    // This is allowed because the current standard leave the behavior undefined
                    break;
            }
            mode = 0;
        } else if (mode === 4) {
            // Unicode mode (values ignored)
            // Use value as length of how many 5-bit words to skip
            inputIndex += value;
            // Maybe implement unicode later (not in C version)
            mode = 0;
        } else {
            // Normal mode
            switch (value) {
                case 0:
                    CodecBuffer.writeCharCode(output, outputIndex, 0); // Null terminator
                    return;
                case 1:
                    mode = 1; // Escape
                    break;
                case 2:
                    CodecBuffer.writeCharCode(output, outputIndex, 92); // \
                    outputIndex++;
                    break;
                case 3:
                    CodecBuffer.writeCharCode(output, outputIndex, 10); // Newline
                    outputIndex++;
                    CodecBuffer.writeCharCode(output, outputIndex, 13); // Write both newline and carriage return
                    outputIndex++;
                    break;
                case 4:
                    CodecBuffer.writeCharCode(output, outputIndex, 124); // '|'
                    outputIndex++;
                    break;
                default:
                    if (value >= 5 && value <= 30) {
                        CodecBuffer.writeCharCode(output, outputIndex, value + 92); // 'a' to 'z'
                        outputIndex++;
                    } else {
                        // Reserved: do nothing
                        // This is allowed because the current standard leave the behavior undefined
                    }
                    break;
            }
        }
        inputIndex++;
    }
}

export { encodeBase32, decodeBase32, read5Bits, write5Bits };