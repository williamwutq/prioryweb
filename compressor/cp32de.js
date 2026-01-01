// You must run this script with Node.js, not in a web browser.
// For performance reasons, you may want to use the C version of this tool instead.
// The C version is named cp32de and is available in the same repository. It does the exact same thing.

import { decodeBase32 } from './Base32Codec.js';
import { createBuffer, removeNullTerminatorIfPresent, shrinkBufferToMin } from './CodecBuffer.js';
import { existsFile, findPath, readFile, writeFile, createDirIfNotExists} from './CodecIo.js';

let die = function (message) {
    console.error("Fatal error: " + message);
    process.exit(1);
}

let main = function (argv) {
    console.log("Info: This program directly process and decompress tag files. If you want to extract them from data files, you must manually link them with the data files.")
    // Process command line arguments: the first argument is the running evironment, the second is the script
    // These are ignored
    if (argv.length < 3) {
        console.log("Usage: node cp32de.js <input_file> <output_file>")
        console.log("This program decodes a single input .tgvz (Compressed Tag Vector) into a decoded output .tgss (Storage Tag Source) file using the custom base32 encoding.");
        process.exit(0);
    }
    if (argv.length < 4) {
        die("No input files provided.");
    }
    if (argv.length != 4) {
        die("Invalid number of arguments. Provide exactly one input file and one output file.");
    }
    const inputFilePstr = argv[2];
    const outputFilePstr = argv[3];
    // Check if input file exists
    const inputFilePath = findPath(inputFilePstr);
    if (!existsFile(inputFilePath)) {
        die("Could not open file: " + inputFilePath);
    }
    // Check if input file ends with .tgvz
    if (!inputFilePstr.endsWith('.tgvz')) {
        console.log("Warning: Input file does not end with .tgvz: " + inputFilePstr + ". Proceeding anyway.");
    }
    // Check if output file ends with .tgss, if not, add the extension
    let outputFilePstrFinal = outputFilePstr;
    if (!outputFilePstrFinal.endsWith('.tgss')) {
        outputFilePstrFinal += '.tgss';
    }
    // Read input file
    let fileContent = readFile(inputFilePath);
    removeNullTerminatorIfPresent(fileContent);
    let processedContent = createBuffer();
    // Decode content
    decodeBase32(fileContent, processedContent, 0);
    processedContent = shrinkBufferToMin(processedContent);
    console.log("Info: Decoding completed.");
    // Prepare output path
    const outputFilePath = findPath(outputFilePstrFinal);
    // Create folder if not exists
    createDirIfNotExists(outputFilePath.substring(0, outputFilePath.lastIndexOf('/')));
    // Write output file
    writeFile(outputFilePath, processedContent);
    console.log("Done.");
}

// Call main
main(process.argv);