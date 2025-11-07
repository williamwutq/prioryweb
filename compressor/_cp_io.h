#ifndef _CP_IO_H
/**
 * @file _cp_io.h
 * @brief  Input/output Utility Module.
 * 
 * This module provides functions to read from and write to files and standard input/output using the _cp_Buffer structure.
 * It includes functions to read entire files into buffers, write buffers to files, and handle null terminators and EOF characters.
 * The functions ensure that buffers are expanded as needed to accommodate the data being read or written.
 * 
 * Functions provided by this module include all necessary I/O operations for working with _cp_Buffer objects,
 * such as reading from files, writing to files, appending data, and managing special characters like null terminators and EOF markers.
 * These functions are generally preferable than using standard C I/O functions directly or use the print functions in the default system
 * module, as they are specifically designed to work with the _cp_Buffer structure for files.
 * 
 * @author William Wu
 */
#define _CP_IO_H
#ifndef _STDDEF_H_
#include <stddef.h>
#endif // _STDDEF_H_
#ifndef _CP_SYS_H_
#include "_cp_sys.h"
#endif // _CP_SYS_H_

/**
 * Asserts that a file exists at the specified path.
 * If the file does not exist, the program terminates with a fatal error.
 * It is ideally used before attempting to read or append to a file, because
 * all I/O functions in this library fail silently on file not found errors to avoid crashes.
 * @param filepath The path to the file to check.
 */
void _cp_assert_file_exists(const char* filepath);
/**
 * Reads the entire contents of a file into the provided buffer.
 * If the buffer does not have enough capacity, it is expanded as needed.
 * @param filepath The path to the file to read.
 * @param buf The buffer to read the file contents into.
 */
void _cp_read_file_to_buffer(const char* filepath, _cp_Buffer* buf);
/**
 * Writes the contents of the provided buffer to a file, overwriting any existing content.
 * @param filepath The path to the file to write to.
 * @param buf The buffer containing the data to write.
 */
void _cp_append_file_to_buffer(const char* filepath, _cp_Buffer* buf);
/**
 * Appends the contents of the provided buffer to a file.
 * @param filepath The path to the file to append to.
 * @param buf The buffer containing the data to append.
 */
void _cp_write_buffer_to_file(const char* filepath, const _cp_Buffer* buf);
/**
 * Appends the contents of the provided buffer to a file.
 * @param filepath The path to the file to append to.
 * @param buf The buffer containing the data to append.
 */
void _cp_append_buffer_to_file(const char* filepath, const _cp_Buffer* buf);
/**
 * Reads the entire contents of standard input into the provided buffer.
 * This function blocks until EOF or control-D is encountered.
 * If the buffer does not have enough capacity, it is expanded as needed.
 * @param buf The buffer to read the standard input contents into.
 */
void _cp_read_stdin_to_buffer(_cp_Buffer* buf);
/**
 * Writes the contents of the provided buffer to standard output.
 * This function does not respect null-termination.
 * To print buffer as a string, use _cp_buffer_print().
 * @param buf The buffer containing the data to write.
 */
void _cp_write_buffer_to_stdout(const _cp_Buffer* buf);
/**
 * Removes a null terminator from the end of the buffer if it exists.
 * If the buffer is empty or does not end with a null terminator, no action is taken.
 * @param buf The buffer to modify.
 */
void _cp_remove_null_terminator_if_exists(_cp_Buffer* buf);
/**
 * Adds a null terminator to the end of the buffer if it does not already exist.
 * Expands the buffer if necessary to accommodate the null terminator.
 * @param buf The buffer to modify.
 */
void _cp_add_null_terminator_if_not_exists(_cp_Buffer* buf);
/**
 * Removes an EOF character from the end of the buffer if it exists.
 * If the buffer is empty or does not end with an EOF character, no action is taken.
 * The EOF character is represented as '\x1A' (ASCII 26) for compatibility with old DOS systems.
 * @param buf The buffer to modify.
 */
void _cp_add_eof_if_exists(_cp_Buffer* buf);
/**
 * Adds an EOF character to the end of the buffer if it does not already exist.
 * Expands the buffer if necessary to accommodate the EOF character.
 * This is to offer compatibility with old DOS systems that use '\x1A' (ASCII 26) as EOF marker.
 * @param buf The buffer to modify.
 */
void _cp_remove_eof_if_not_exists(_cp_Buffer* buf);

#endif // _CP_IO_H