/**
 * Prints the binary representation of a buffer with specified separator and line length.
 * @param {Uint8Array} buffer
 * @param {string} separator
 * @param {number} lineLength
 */
function printBufferBinary(buffer, separator = ' ', lineLength = 8) {
    let result = '';
    let cum = ''
    for (let i = 0; i < buffer.length; i++) {
        let byteStr = buffer[i].toString(2).padStart(8, '0');
        cum += byteStr;
    }
    result += cum.slice(0, lineLength);
    for (let i = lineLength; i < cum.length; i += lineLength) {
        result += separator + cum.slice(i, i + lineLength);
    }
    console.log(result.trim());
}

/**
 * Prints the hexadecimal representation of a buffer.
 * @param {Uint8Array} buffer
 */
function printBufferHex(buffer) {
    let result = '';
    for (let i = 0; i < buffer.length; i++) {
        let byteStr = buffer[i].toString(16).padStart(2, '0');
    }
    console.log(result.trim());
}

export { printBufferBinary, printBufferHex };