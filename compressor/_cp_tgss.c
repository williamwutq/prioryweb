#ifndef _CP_SYS_H_
#include "_cp_sys.h"
#endif // _CP_SYS_H_
#ifndef _CP_CHAIN_H_
#include "_cp_chain.h"
#endif // _CP_CHAIN_H_
#ifndef _CP_TGSS_H_
#include "_cp_tgss.h"
#endif // _CP_TGSS_H_
#ifndef _STRING_H_
#include <string.h>
#endif // _STRING_H_

_cp_Buffer* _cp_tag_create() {
    return _cp_buffer_create_cap(20);
}

_cp_Buffer* _cp_tag_from_str(const char* str) {
    if (str == NULL) return NULL;
    size_t len = strlen(str);
    _cp_Buffer* buf = _cp_buffer_create_cap(20);
    if (buf == NULL) return NULL;
    if (_cp_buffer_expand(buf, len) == NULL) {
        _cp_buffer_free(buf);
        return NULL;
    }
    memcpy(buf->data, str, len);
    buf->size = len;
    return buf;
}

_cp_Buffer* _cp_tag_from_buff(const _cp_Buffer* buff) {
    if (buff == NULL) return NULL;
    _cp_Buffer* buf = _cp_buffer_create_cap(buff->size);
    if (buf == NULL) return NULL;
    memcpy(buf->data, buff->data, buff->size);
    buf->size = buff->size;
    return buf;
}

_cp_Buffer* _cp_tag_escape(const _cp_Buffer* tag) {
    if (tag == NULL) return NULL;
    // First passs: count ,
    size_t extra_commas = 0;
    for (size_t i = 0; i < tag->size; i++) {
        if (tag->data[i] == ',') {
            extra_commas++;
        }
    }
    _cp_Buffer* esc = _cp_buffer_create_cap(tag->size + extra_commas);
    if (esc == NULL) return NULL;
    for (size_t i = 0; i < tag->size; i++) {
        if (tag->data[i] == ',') {
            esc->data[esc->size++] = ',';
            esc->data[esc->size++] = ',';
        } else {
            esc->data[esc->size++] = tag->data[i];
        }
    }
    return esc;
}

_cp_Buffer* _cp_tag_unescape(const _cp_Buffer* tag) {
    if (tag == NULL) return NULL;
    _cp_Buffer* unesc = _cp_buffer_create_cap(tag->size);
    if (unesc == NULL) return NULL;
    for (size_t i = 0; i < tag->size; i++) {
        if (tag->data[i] == ',') {
            if (i + 1 < tag->size && tag->data[i + 1] == ',') {
                unesc->data[unesc->size++] = ',';
                i++; // Skip next comma
            }
            // Else skip single comma
        } else if (tag->data[i] != ' ' && tag->data[i] != '\n' && tag->data[i] != '\t' && tag->data[i] != '\r') {
            unesc->data[unesc->size++] = tag->data[i];
        }
    }
    return unesc;
}

bool _cp_tgss_parse(const _cp_Buffer* data, _cp_Buffer_Chain* out_tags) {
    if (data == NULL || out_tags == NULL) return false;
    size_t i = 0; bool success = true;
    while (i < data->size) {
        _cp_Buffer* tag = _cp_buffer_create_cap(20);
        if (tag == NULL) return false;
        while (i < data->size) {
            if (data->data[i] == ',') {
                if (i + 1 < data->size && data->data[i + 1] == ',') {
                    // Escaped comma
                    if (_cp_buffer_expand(tag, 1) == NULL) {
                        _cp_buffer_free(tag);
                        return false;
                    }
                    tag->data[tag->size++] = ',';
                    i += 2;
                } else {
                    // End of tag
                    i++;
                    break;
                }
            } else if (data->data[i] != ' ' && data->data[i] != '\n' && data->data[i] != '\t' && data->data[i] != '\r') {
                // Regular character
                if (_cp_buffer_expand(tag, 1) == NULL) {
                    _cp_buffer_free(tag);
                    return false;
                }
                tag->data[tag->size++] = data->data[i++];
            } else {
                // Skip whitespace
                i++;
            }
        }
        if (tag->size > 0) {
            _cp_buffer_chain_pusht(out_tags, tag);
        } else {
            success = false;
            _cp_buffer_free(tag);
        }
    }
    return success;
}

bool _cp_tgss_serialize(const _cp_Buffer_Chain* tags, _cp_Buffer* out, bool pretty) {
    if (tags == NULL || out == NULL) return false;
    _cp_Buffer_ChainNode* current = tags->head;
    while (current != NULL) {
        _cp_Buffer* esc_tag = _cp_tag_escape(current->buffer);
        if (esc_tag == NULL) return false;
        if (out->size > 0) {
            if (_cp_buffer_expand(out, 1) == NULL) {
                _cp_buffer_free(esc_tag);
                return false;
            }
            out->data[out->size++] = ',';
            if (pretty) {
                if (_cp_buffer_expand(out, 1) == NULL) {
                    _cp_buffer_free(esc_tag);
                    return false;
                }
                out->data[out->size++] = '\n';
            }
        }
        if (_cp_buffer_expand(out, esc_tag->size) == NULL) {
            _cp_buffer_free(esc_tag);
            return false;
        }
        memcpy(out->data + out->size, esc_tag->data, esc_tag->size);
        out->size += esc_tag->size;
        _cp_buffer_free(esc_tag);
        current = current->next;
    }
    return true;
}