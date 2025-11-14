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
    const bitOffset = index * 5;
    const byteOffset = Math.floor(bitOffset / 8) + offset;
    const bitInByteOffset = bitOffset % 8;

    CodecBuffer.ensureCapacity(buffer, byteOffset + 2); // Ensure enough space

    let value = 0;

    // Read the first byte
    if (byteOffset < buffer.length) {
        value |= (buffer[byteOffset] >> bitInByteOffset) & 0x1F;
    }

    // If the 5 bits span across two bytes
    if (bitInByteOffset > 3 && (byteOffset + 1) < buffer.length) {
        value |= (buffer[byteOffset + 1] << (8 - bitInByteOffset)) & 0x1F;
    }

    return value & 0x1F; // Ensure we only return the lower 5 bits
}

/**
 * Writes a 5-bit value into the buffer starting at the specified offset.
 * @param {Uint8Array} buffer - The buffer to write to.
 * @param {number} offset - The offset in 5-bit units.
 * @param {number} value - The 5-bit value to write.
 */
function write5Bits(buffer, index, offset, value) {
    const bitOffset = index * 5;
    const byteOffset = Math.floor(bitOffset / 8) + offset;
    const bitInByteOffset = bitOffset % 8;

    CodecBuffer.ensureCapacity(buffer, byteOffset + 2); // Ensure enough space

    // Write to the first byte
    if (byteOffset < buffer.length) {
        buffer[byteOffset] &= ~(0x1F << bitInByteOffset); // Clear the bits
        buffer[byteOffset] |= (value & 0x1F) << bitInByteOffset; // Set the bits
    }

    // If the 5 bits span across two bytes
    if (bitInByteOffset > 3 && (byteOffset + 1) < buffer.length) {
        buffer[byteOffset + 1] &= ~(0x1F >> (8 - bitInByteOffset)); // Clear the bits
        buffer[byteOffset + 1] |= (value & 0x1F) >> (8 - bitInByteOffset); // Set the bits
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
    let outputIndex = 0;
    while (true) {
        let charCode = CodecBuffer.readCharCode(input, inputIndex);
        if (inputIndex >= input.length || charCode === 0) {
            write5Bits(output, outputIndex, offset, 0); // Write null terminator
            break;
        }
        // Maybe optimize this with a map
        // Support &nbsp; and &#160;
        if (readString(input, inputIndex, 6) === '&nbsp;' || readString(input, inputIndex, 7) === '&#160;') {
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
            // 'a' to 'z' -> 0b00101 to 0b11110
            case charCode >= 97 && charCode <= 122:
                write5Bits(output, outputIndex, offset, charCode - 92);
                break;
            // 'A' to 'Z' -> 0b00101 to 0b11110
            // The format does not allow uppercase letters, but we represent them with lowercase
            case charCode >= 65 && charCode <= 90:
                write5Bits(output, outputIndex, offset, charCode - 60);
                break;
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
            // Unsupported character: use null terminator but does not terminate
            default:
                write5Bits(output, outputIndex, offset, 0);
        }
        inputIndex++;
        outputIndex++;
    }
}