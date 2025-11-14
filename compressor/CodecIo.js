// CodecIo is Node Only and cannot run on the web. Do not include this file in web builds.

// Detect node
if (typeof process !== 'undefined' && process.versions != null && process.versions.node != null) {
    // Node.js environment
} else {
    throw new Error('This module can only be used in a Node.js environment.');
}
import fs from 'fs';
import path from 'path';

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
 * Finds the absolute path for a given relative path.
 * @param {string} relativePath The relative path
 * @returns {string} The absolute path
 */
function findPath(relativePath) {
    return path.resolve(relativePath);
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

/**
 * Creates a directory at the given path if it does not already exist.
 * @param {string} path The directory path
 */
function createDirIfNotExists(path) {
    if (!fs.existsSync(path)) {
        fs.mkdirSync(path, { recursive: true });
    }
}

export { existsFile, findPath, readFile, writeFile, appendFile, createDirIfNotExists };