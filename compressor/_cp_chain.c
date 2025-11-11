#include "_cp_sys.h"
#include "_cp_chain.h"
#include <stddef.h>
#include <stdlib.h>

_cp_Buffer_ChainNode* _cp_buffer_chain_node_create(_cp_Buffer* buffer) {
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

void _cp_buffer_chain_pusht(_cp_Buffer_Chain* chain, _cp_Buffer* buffer) {
    if (buffer == NULL) return;
    if (chain == NULL) {
        chain = _cp_buffer_chain_create();
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

void _cp_buffer_chain_pushf(_cp_Buffer_Chain* chain, _cp_Buffer* buffer) {
    if (buffer == NULL) return;
    if (chain == NULL) {
        chain = _cp_buffer_chain_create();
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

void _cp_buffer_chain_push(_cp_Buffer_Chain* chain, _cp_Buffer* buffer, const size_t count) {
    if (buffer == NULL) return;
    if (chain == NULL) {
        chain = _cp_buffer_chain_create();
    }
    if (count == 0) {
        _cp_buffer_chain_pushf(chain, buffer);
    }
    size_t _chain_count = _cp_buffer_chain_count(chain);
    if (count >= _chain_count) {
        _cp_buffer_chain_pusht(chain, buffer);
    } else {
        // Insert at the specified index
        _cp_Buffer_ChainNode* new_node = _cp_buffer_chain_node_create(buffer);
        _cp_Buffer_ChainNode* current = chain->head;
        for (size_t i = 0; i < count; i++) {
            current = current->next;
        }
        new_node->prev = current->prev;
        new_node->next = current;
        if (current->prev != NULL) {
            current->prev->next = new_node;
        } else {
            chain->head = new_node;
        }
        current->prev = new_node;
        chain->total_size += buffer->size;
    }
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

_cp_Buffer* _cp_buffer_chain_pop(_cp_Buffer_Chain* chain, const size_t count) {
    if (chain == NULL) {
        return NULL;
    }
    size_t _chain_count = _cp_buffer_chain_count(chain);
    if (count == 0) {
        return _cp_buffer_chain_popf(chain);
    }
    if (count >= _chain_count - 1) {
        return _cp_buffer_chain_popt(chain);
    }
    // Pop from the specified index
    _cp_Buffer_ChainNode* current = chain->head;
    for (size_t i = 0; i < count; i++) {
        current = current->next;
    }
    _cp_Buffer* buffer = current->buffer;

    if (current->prev != NULL) {
        current->prev->next = current->next;
    } else {
        chain->head = current->next;
    }
    if (current->next != NULL) {
        current->next->prev = current->prev;
    } else {
        chain->tail = current->prev;
    }
    chain->total_size -= buffer->size;

    free(current);
    return buffer;
}

void _cp_buffer_chain_iter(const _cp_Buffer_Chain* chain, const void (*func)(const _cp_Buffer* buffer, void* data), void* data) {
    if (chain == NULL || func == NULL) {
        return;
    }
    _cp_Buffer_ChainNode* current = chain->head;
    while (current != NULL) {
        func(current->buffer, data);
        current = current->next;
    }
}

_cp_Buffer_Chain* _cp_buffer_chain_concat(_cp_Buffer_Chain* c1, _cp_Buffer_Chain* c2) {
    if (c1 == NULL || c2 == NULL) {
        return NULL;
    }
    _cp_Buffer_Chain* combined_chain = _cp_buffer_chain_create();
    _cp_Buffer_ChainNode* h1 = c1->head;
    _cp_Buffer_ChainNode* h2 = c2->head;
    _cp_Buffer_ChainNode* t1 = c1->tail;
    _cp_Buffer_ChainNode* t2 = c2->tail;
    size_t total_size = c1->total_size + c2->total_size;
    // Free the original chains without freeing their buffers
    free(c1);
    free(c2);
    combined_chain->head = h1;
    combined_chain->tail = t2;
    combined_chain->total_size = total_size;
    if (t1 != NULL && h2 != NULL) {
        t1->next = h2;
        h2->prev = t1;
    }
    return combined_chain;
}

_cp_Buffer_Chain* _cp_buffer_chain_separate(_cp_Buffer_Chain* chain, size_t index) {
    if (chain == NULL) {
        return NULL; // No separation needed
    }
    if (index >= chain->total_size) {
        return _cp_buffer_chain_create(); // No separation needed
    }
    _cp_Buffer_Chain* new_chain = _cp_buffer_chain_create();
    size_t accumulated_size = 0;
    _cp_Buffer_ChainNode* current = chain->head;
    while (current != NULL) {
        if (accumulated_size + current->buffer->size > index) {
            break;
        }
        accumulated_size += current->buffer->size;
        current = current->next;
    }
    if (current == NULL) {
        return new_chain; // No separation needed
    }
    // Detach nodes from the original chain to the new chain
    new_chain->head = current;
    new_chain->tail = chain->tail;
    new_chain->total_size = chain->total_size - accumulated_size;

    if (current->prev != NULL) {
        current->prev->next = NULL;
        chain->tail = current->prev;
    } else {
        // The entire chain is moved
        chain->head = NULL;
        chain->tail = NULL;
    }
    current->prev = NULL;
    chain->total_size = accumulated_size;

    return new_chain;
}

_cp_Buffer_Chain* _cp_buffer_chain_split(_cp_Buffer_Chain* chain){
    if (chain == NULL) {
        return NULL; // No separation needed
    }
    _cp_Buffer_Chain* new_chain = _cp_buffer_chain_create();
    size_t accumulated_size = 0;
    _cp_Buffer_ChainNode* current = chain->head;
    while (current != NULL) {
        if (accumulated_size + current->buffer->size > chain->total_size / 2) {
            break;
        }
        accumulated_size += current->buffer->size;
        current = current->next;
    }
    if (current == NULL) {
        return new_chain; // No separation needed
    }
    // Detach nodes from the original chain to the new chain
    new_chain->head = current;
    new_chain->tail = chain->tail;
    new_chain->total_size = chain->total_size - accumulated_size;

    if (current->prev != NULL) {
        current->prev->next = NULL;
        chain->tail = current->prev;
    } else {
        // The entire chain is moved
        chain->head = NULL;
        chain->tail = NULL;
    }
    current->prev = NULL;
    chain->total_size = accumulated_size;

    return new_chain;
}

_cp_Buffer* _cp_buffer_chain_embuffer(_cp_Buffer_Chain* chain) {
    if (chain == NULL || chain->total_size == 0) {
        return _cp_buffer_create(); // Return an empty buffer
    }
    _cp_Buffer* combined_buffer = _cp_buffer_create_cap(chain->total_size);
    if (combined_buffer == NULL) {
        _cp_die("Failed to create combined buffer.");
    }
    _cp_Buffer_ChainNode* current = chain->head;
    while (current != NULL) {
        _cp_buffer_concat_inplace(combined_buffer, current->buffer);
        current = current->next;
    }
    _cp_buffer_chain_free(chain);
    return combined_buffer;
}

size_t _cp_buffer_chain_count(const _cp_Buffer_Chain* chain) {
    if (chain == NULL) {
        return 0;
    }
    size_t count = 0;
    _cp_Buffer_ChainNode* current = chain->head;
    while (current != NULL) {
        count++;
        current = current->next;
    }
    return count;
}