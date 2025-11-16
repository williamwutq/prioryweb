#ifndef _CP_TGSS_H
/**
 * @file tgss.h
 * @brief Module dealing with the TGSS format. This does not do any compression.
 * 
 * This module provides functions to verify, escape, and unescape TGSS data.
 * It also provides buffer optimization for simple tags.
 * 
 * TGSS (Storage Tag Source) is a format used for storing tagged data.
 * It is designed to be simple and efficient for both storage and retrieval.
 * It is a comma separated format where a continous sequence of tags are stored
 * together. It is case insensitive and usually ignores whitespace. To escape
 * a comma, double it (,,). Empty elements are forbidden.
 * 
 * @author William Wu
 */
#define _CP_TGSS_H
#ifndef _CP_SYS_H_
#include "_cp_sys.h"
#endif // _CP_SYS_H_
#ifndef _CP_CHAIN_H_
#include "_cp_chain.h"
#endif // _CP_CHAIN_H_

/**
 * Creates a new empty buffer optimized for tags. This buffer has an initial size of 20 bytes.
 * @return A new empty buffer.
 */
_cp_Buffer* _cp_tag_create();
/**
 * Creates a new tag buffer from a C-string.
 * @param str The C-string to create the tag from.
 * @return A new buffer containing the tag.
 */
_cp_Buffer* _cp_tag_from_str(const char* str);
/**
 * Creates a new tag buffer from an existing buffer.
 * @param buff The buffer to create the tag from.
 * @return A new buffer containing the tag.
 */
_cp_Buffer* _cp_tag_from_buff(const _cp_Buffer* buff);
/**
 * Escapes a tag into TGSS format.
 * @param tag The tag to escape.
 * @return A new buffer containing the escaped tag.
 */
_cp_Buffer* _cp_tag_escape(const _cp_Buffer* tag);
/**
 * Unescapes a TGSS formatted tag and remove all whitespace.
 * @param tag The TGSS formatted tag to unescape.
 * @return A new buffer containing the unescaped tag.
 */
_cp_Buffer* _cp_tag_unescape(const _cp_Buffer* tag);
/**
 * Parses TGSS formatted data into a chain of tags.
 * @param data The TGSS formatted data to parse.
 * @param out_tags The output chain of tags.
 * @return True on success, false on failure.
 */
bool _cp_tgss_parse(const _cp_Buffer* data, _cp_Buffer_Chain* out_tags);
/**
 * Serializes a chain of tags into TGSS format.
 * @param tags The chain of tags to serialize.
 * @param out The output buffer to write the serialized data to.
 * @param pretty If true, adds whitespace and newlines for readability.
 * @return True on success, false on failure.
 */
bool _cp_tgss_serialize(const _cp_Buffer_Chain* tags, _cp_Buffer* out, bool pretty);

#endif // _CP_TGSS_H