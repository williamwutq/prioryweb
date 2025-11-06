#include "_cp_sys.h"
#include "_cp_chain.h"
#include <stddef.h>

_cp_Buffer_ChainNode* _cp_buffer_chain_node_create(const _cp_Buffer* buffer) {
    _cp_Buffer_ChainNode* node = (_cp_Buffer_ChainNode*)malloc(sizeof(_cp_Buffer_ChainNode));
    if (node == NULL) {
        _cp_die("Failed to allocate memory for _cp_Buffer_ChainNode.");
    }
    node->buffer = buffer;
    node->next = NULL;
    node->prev = NULL;
    return node;
}

void _cp_buffer_chain_node_free(_cp_Buffer_ChainNode* node) {
    if (node == NULL) {
        return;
    }
    if (node->buffer != NULL) {
        _cp_buffer_free(node->buffer);
    }
    free(node);
}

_cp_Buffer_Chain* _cp_buffer_chain_create(void) {
    _cp_Buffer_Chain* chain = (_cp_Buffer_Chain*)malloc(sizeof(_cp_Buffer_Chain));
    if (chain == NULL) {
        _cp_die("Failed to allocate memory for _cp_Buffer_Chain.");
    }
    chain->head = NULL;
    chain->tail = NULL;
    chain->total_size = 0;
    return chain;
}

void _cp_buffer_chain_free(_cp_Buffer_Chain* chain) {
    if (chain == NULL) {
        return;
    }
    _cp_Buffer_ChainNode* current = chain->head;
    while (current != NULL) {
        _cp_Buffer_ChainNode* next = current->next;
        _cp_buffer_chain_node_free(current);
        current = next;
    }
    free(chain);
}

void _cp_buffer_chain_pusht(_cp_Buffer_Chain* chain, const _cp_Buffer* buffer) {
    if (chain == NULL || buffer == NULL) {
        _cp_die("Cannot push to a null buffer chain or push a null buffer.");
    }
    _cp_Buffer_ChainNode* new_node = _cp_buffer_chain_node_create(buffer);
    if (chain->tail == NULL) {
        // Chain is empty
        chain->head = new_node;
        chain->tail = new_node;
    } else {
        // Append to the end
        chain->tail->next = new_node;
        new_node->prev = chain->tail;
        chain->tail = new_node;
    }
    chain->total_size += buffer->size;
}

void _cp_buffer_chain_pushf(_cp_Buffer_Chain* chain, const _cp_Buffer* buffer) {
    if (chain == NULL || buffer == NULL) {
        _cp_die("Cannot push to a null buffer chain or push a null buffer.");
    }
    _cp_Buffer_ChainNode* new_node = _cp_buffer_chain_node_create(buffer);
    if (chain->head == NULL) {
        // Chain is empty
        chain->head = new_node;
        chain->tail = new_node;
    } else {
        // Prepend to the front
        new_node->next = chain->head;
        chain->head->prev = new_node;
        chain->head = new_node;
    }
    chain->total_size += buffer->size;
}

_cp_Buffer* _cp_buffer_chain_popt(_cp_Buffer_Chain* chain) {
    if (chain == NULL || chain->tail == NULL) {
        return NULL;
    }
    _cp_Buffer_ChainNode* node_to_pop = chain->tail;
    _cp_Buffer* buffer = node_to_pop->buffer;

    chain->tail = node_to_pop->prev;
    if (chain->tail != NULL) {
        chain->tail->next = NULL;
    } else {
        // Chain is now empty
        chain->head = NULL;
    }
    chain->total_size -= buffer->size;

    free(node_to_pop);
    return buffer;
}

_cp_Buffer* _cp_buffer_chain_popf(_cp_Buffer_Chain* chain) {
    if (chain == NULL || chain->head == NULL) {
        return NULL;
    }
    _cp_Buffer_ChainNode* node_to_pop = chain->head;
    _cp_Buffer* buffer = node_to_pop->buffer;

    chain->head = node_to_pop->next;
    if (chain->head != NULL) {
        chain->head->prev = NULL;
    } else {
        // Chain is now empty
        chain->tail = NULL;
    }
    chain->total_size -= buffer->size;

    free(node_to_pop);
    return buffer;
}

void _cp_buffer_chain_iter(const _cp_Buffer_Chain* chain, const void (*func)(const _cp_Buffer*, void* data), void* data) {
    if (chain == NULL || func == NULL) {
        return;
    }
    _cp_Buffer_ChainNode* current = chain->head;
    while (current != NULL) {
        func(current->buffer, data);
        current = current->next;
    }
}