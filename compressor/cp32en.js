// You must run this script with Node.js, not in a web browser.
// For performance reasons, you may want to use the C version of this tool instead.
// The C version is named cp32en and is available in the same repository. It does the exact same thing.

import { encodeBase32, checkFormatBase32, correctFormatBase32 } from './Base32Codec.js';
import { createBuffer, appendCharCode, removeNullTerminatorIfPresent, concatBuffersAsString } from './CodecBuffer.js';
import { existsFile, findPath, readFile, writeFile, createDirIfNotExists} from './CodecIo.js';

let die = function (message) {
    console.error("Fatal error: " + message);
    process.exit(1);
}

let main = function (argv) {
    console.log("Info: This program directly process and compress tag files. If you want to add them into data files, you must manually link them with the data files.")
    // Process command line arguments: the first argument is the running evironment, the second is the script
    // These are ignored
    if (argv.length < 3) {
        console.log("Usage: node cp32en.js <input_file1> <input_file2> ... <output_file>")
        console.log("Info: This program encodes multiple input .tgss (Storage Tag Source) into a single compressed output .tgvz (Compressed Tag Vector) file using the custom base32 encoding.");
        process.exit(0);
    }
    if (argv.length < 4) {
        die("No input files provided.");
    }
    let outputFilePstr = argv[argv.length - 1];
    // If output file does not end with .tgvz, add the extension
    if (!outputFilePstr.endsWith('.tgvz')) {
        outputFilePstr += '.tgvz';
    }
    const outputFilePath = findPath(outputFilePstr);
    // Create folder if not exists
    createDirIfNotExists(outputFilePath.substring(0, outputFilePath.lastIndexOf('/')));
    // Create long input buffer
    let inputArray = []; let c = 0;
    for (let i = 2; i < argv.length - 1; i++) {
        const inputFilePstr = argv[i];
        const inputFilePath = findPath(inputFilePstr);
        // Check if input file exists
        if (!existsFile(inputFilePath)) {
            console.log("Could not open file: " + inputFilePath);
        }
        // Check if file ends in .tgss
        if (!inputFilePstr.endsWith('.tgss')) {
            console.log("Warning: Input file does not end with .tgss: " + inputFilePstr + ". Proceeding anyway.");
        }
        // Read input file
        const fileContent = readFile(inputFilePath);
        let processedContent;
        // Check format
        if (!checkFormatBase32(fileContent)) {
            console.log("Info: File is not well-formatted, correcting by first converting to lowercase then convert invalid characters into spaces.");
            processedContent = correctFormatBase32(fileContent);
            console.log("Info: Corrected content of file " + inputFilePstr + ":");
            console.log(processedContent);
        } else {
            processedContent = fileContent;
        }
        removeNullTerminatorIfPresent(processedContent);
        inputArray.push(processedContent);
        c++;
    }
    if (c === 0) {
        die("No valid input files provided.");
    }
    console.log("Encoding " + c + " input files into " + outputFilePath);
    // Process input array
    let outputArray = [];
    for (let i = 0; i < inputArray.length; i++) {
        let output = createBuffer();
        encodeBase32(inputArray[i], output, 0);
        appendCharCode(output, 0xFF);
        appendCharCode(output, 0xFF);
        outputArray.push(output);
    }
    const output = concatBuffersAsString(outputArray);
    console.log("Finished encoding. Writing to output file " + outputFilePath + "...");
    writeFile(outputFilePath, output);
    console.log("Done.");
};

// Call main
main(process.argv);