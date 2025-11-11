// CodecIo is Node Only and cannot run on the web. Do not include this file in web builds.

// Detect node
if (typeof process !== 'undefined' && process.versions != null && process.versions.node != null) {
    // Node.js environment
} else {
    throw new Error('This module can only be used in a Node.js environment.');
}
import fs from 'fs';

/**
 * Checks whether a file exists at the given path.
 * @param {string} path
 * @returns {boolean} Whether the file exists
 */
function existsFile(path) {
    try {
        fs.accessSync(path, fs.constants.F_OK);
        return true;
    } catch (e) {
        return false;
    }
}

/**
 * Reads the contents of a file at the given path.
 * @param {string} path The file path
 * @returns {Uint8Array} The contents of the file
 */
function readFile(path) {
    return fs.readFileSync(path);
}

/**
 * Writes data to a file at the given path.
 * @param {string} path The file path
 * @param {Uint8Array} data The data to write
 */
function writeFile(path, data) {
    fs.writeFileSync(path, data);
}

/**
 * Appends data to a file at the given path.
 * @param {string} path The file path
 * @param {Uint8Array} data The data to append
 */
function appendFile(path, data) {
    fs.appendFileSync(path, data);
}

export { existsFile, readFile, writeFile, appendFile };