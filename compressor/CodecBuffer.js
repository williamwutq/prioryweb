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
 * Reads a string from an Int8Array starting at the specified offset up to the specified length.
 * @param {Int8Array} str - The input string.
 * @param {number} offset - The starting index.
 * @param {number} length - The maximum length of the string to read.
 * @returns {string} The string read from the Int8Array.
 */
function readString(str, offset, length) {
    let result = '';
    for (let i = 0; i < length; i++) {
        const charCode = readCharCode(str, offset + i);
        if (charCode === 0) break; // Null terminator
        result += String.fromCharCode(charCode);
    }
    return result;
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
 * Appends a character code to the end of an Int8Array treated as a string.
 * @param {Int8Array} str - The input string.
 * @param {number} charCode - The character code to append.
 */
function appendCharCode(str, charCode) {
    const length = conservativeStrlen(str);
    writeCharCode(str, length, charCode);
}

/**
 * Writes a string into an Int8Array starting at the specified offset, the null terminator is not written.
 * @param {Int8Array} str - The input string.
 * @param {number} offset - The starting index.
 * @param {string} value - The string to write.
 */
function writeString(str, offset, value) {
    for (let i = 0; i < value.length; i++) {
        writeCharCode(str, offset + i, value.charCodeAt(i));
    }
}

/**
 * Removes the null terminator from the end of the Int8Array if present.
 * @param {Int8Array} str - The input string.
 * @returns {Int8Array} The Int8Array without the null terminator.
 */
function removeNullTerminatorIfPresent(str) {
    const length = str.length;
    if (length > 0 && str[length - 1] === 0) {
        return shrinkBuffer(str, length - 1);
    }
    return str;
}

/**
 * Adds a null terminator to the end of the Int8Array if not already present.
 * @param {Int8Array} str - The input string.
 * @returns {Int8Array} The Int8Array with a null terminator.
 */
function addNullTerminatorIfNotPresent(str) {
    const length = str.length;
    if (length === 0 || str[length - 1] !== 0) {
        const newBuffer = ensureCapacity(str, length + 1);
        newBuffer[length] = 0;
        return newBuffer;
    }
    return str;
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
 * Shrinks the buffer to fit the string length plus null terminator.
 * For shrinking without null terminator, use shrinkBufferToMin.
 * @param {Uint8Array} buffer - The original buffer.
 * @returns {Uint8Array} The shrunk buffer.
 */
function shrinkBufferToFit(buffer) {
    return shrinkBuffer(buffer, conservativeStrlen(buffer) + 1);
}

/**
 * Shrinks the buffer to fit the string length without null terminator.
 * To preserve null terminator, use shrinkBufferToFit.
 * @param {Uint8Array} buffer - The original buffer.
 * @returns {Uint8Array} The shrunk buffer.
 */
function shrinkBufferToMin(buffer) {
    return shrinkBuffer(buffer, conservativeStrlen(buffer));
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

/**
 * Returns the length of the given buffer treating trailing null bytes as non-existent.
 * If, for some reason, the buffer has internal null bytes, this will still count up to the last non-null byte.
 * For normal data, this behaves the same as strlen.
 * @param {Uint8Array} buffer - The input buffer.
 * @returns {number} The conservative length of the buffer.
 */
function conservativeStrlen(buffer) {
    let length = 0;
    let index = buffer.length - 1;
    while (index >= 0 && buffer[index] === 0) {
        index--;
    }
    return index + 1;
}

export {
    readCharCode, readString, writeCharCode, writeString, appendCharCode,
    removeNullTerminatorIfPresent, addNullTerminatorIfNotPresent,
    ensureCapacity, shrinkBuffer, shrinkBufferToFit, shrinkBufferToMin,
    sliceBuffer, concatBuffers, createBuffer,
    strlen, conservativeStrlen
};