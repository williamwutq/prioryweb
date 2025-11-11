/**
 * Reads the character code at the specified index from an Int8Array treated as a string.
 * @param {Int8Array} str - The input string.
 * @param {number} index - The index of the character to read.
 * @returns {number} The character code at the specified index.
 */
function readCharCode(str, index) {
    return str[index] & 0xFF;
}

/**
 * Writes a character code at the specified index in an Int8Array treated as a string.
 * @param {Int8Array} str - The input string.
 * @param {number} index - The index where the character code will be written.
 * @param {number} charCode - The character code to write.
 */
function writeCharCode(str, index, charCode) {
    ensureCapacity(str, index + 1);
    str[index] = charCode & 0xFF;
}

/**
 * Ensures that the buffer has at least the required length, resizing if necessary.
 * @param {Uint8Array} buffer - The original buffer.
 * @param {number} requiredLength - The required minimum length.
 * @returns {Uint8Array} The original buffer if it meets the required length, otherwise a new resized buffer.
 */
function ensureCapacity(buffer, requiredLength) {
    if (buffer.length >= requiredLength) {
        return buffer;
    }
    let newLength = buffer.length;
    while (newLength < requiredLength) {
        newLength *= 2;
    }
    let newBuffer = new Uint8Array(newLength);
    newBuffer.set(buffer);
    return newBuffer;
}

/**
 * Shrinks the buffer to the specified length if it is larger.
 * @param {Uint8Array} buffer - The original buffer.
 * @param {number} requiredLength - The desired length.
 * @returns {Uint8Array} The original buffer if it is already the required length or smaller, otherwise a new shrunk buffer.
 */
function shrinkBuffer(buffer, requiredLength) {
    if (buffer.length <= requiredLength) {
        return buffer;
    }
    let newBuffer = new Uint8Array(requiredLength);
    newBuffer.set(buffer.subarray(0, requiredLength));
    return newBuffer;
}

/**
 * Slices a portion of the buffer from start to end.
 * @param {Uint8Array} buffer - The original buffer.
 * @param {number} start - The starting index.
 * @param {number} end - The ending index.
 * @returns {Uint8Array} The sliced portion of the buffer.
 */
function sliceBuffer(buffer, start, end) {
    return buffer.subarray(start, end);
}

/**
 * Concatenates multiple Uint8Array buffers into a single buffer.
 * @param {Uint8Array[]} buffers - The array of buffers to concatenate.
 * @returns {Uint8Array} The concatenated buffer.
 */
function concatBuffers(buffers) {
    let totalLength = buffers.reduce((sum, buf) => sum + buf.length, 0);
    let result = new Uint8Array(totalLength);
    let offset = 0;
    for (let buf of buffers) {
        result.set(buf, offset);
        offset += buf.length;
    }
    return result;
}

/**
 * Creates a new Uint8Array buffer of the specified length.
 * @param {number} length - The length of the buffer to create in bytes, default is 256.
 * @returns {Uint8Array} The newly created buffer.
 */
function createBuffer(length = 256) {
    return new Uint8Array(length);
}

/**
 * Returns the length of the given buffer treated as a string.
 * @param {Uint8Array} buffer - The input buffer.
 * @returns {number} The length of the buffer.
 */
function strlen(buffer) {
    // Find the first null terminator (0 byte)
    let length = 0;
    while (length < buffer.length && buffer[length] !== 0) {
        length++;
    }
    return length;
}

export { readCharCode, writeCharCode, ensureCapacity, shrinkBuffer, sliceBuffer, concatBuffers, createBuffer, strlen };