// An extension of the _cp_Buffer to provide optimizations for chaining multiple buffers together.
#ifndef _CP_CHAIN_H
#define _CP_CHAIN_H
#ifndef _CP_SYS_H_
#include "_cp_sys.h"
#endif // _CP_SYS_H_

/**
 * A node in a linked list of _cp_Buffers.
 * Each node contains a pointer to a _cp_Buffer and pointers to the next and previous nodes in the chain.
 */
typedef struct _cp_Buffer_ChainNode {
    struct _cp_Buffer_ChainNode* next;
    struct _cp_Buffer_ChainNode* prev;
    _cp_Buffer* buffer;
} _cp_Buffer_ChainNode;

/**
 * A chain of _cp_Buffers implemented as a linked list.
 * This structure allows efficient appending and concatenation of multiple buffers.
 */
typedef struct {
    _cp_Buffer_ChainNode* head;
    _cp_Buffer_ChainNode* tail;
    size_t total_size;
} _cp_Buffer_Chain;

/**
 * Creates a new _cp_Buffer_ChainNode containing the specified buffer.
 * The node is not linked to any other nodes upon creation.
 * @param buffer The buffer to store in the node.
 * @return A pointer to the newly created _cp_Buffer_ChainNode.
 */
_cp_Buffer_ChainNode* _cp_buffer_chain_node_create(const _cp_Buffer* buffer);
/**
 * Frees the memory allocated for a _cp_Buffer_ChainNode.
 * The node itself is freed with its buffer.
 * Note: This function does not free the buffer contained within the node.
 * @param node The node to free.
 */
void _cp_buffer_chain_node_free(_cp_Buffer_ChainNode* node);
/**
 * Creates an empty _cp_Buffer_Chain.
 * The chain contains no nodes and has a total size of zero.
 * @return A pointer to the newly created _cp_Buffer_Chain.
 */
_cp_Buffer_Chain* _cp_buffer_chain_create(void);
/**
 * Frees the memory allocated for a _cp_Buffer_Chain and all its nodes.
 * All buffers contained within the nodes are also freed.
 * @param chain The buffer chain to free.
 */
void _cp_buffer_chain_free(_cp_Buffer_Chain* chain);
/**
 * Push a buffer to the end of the buffer chain.
 * A new node is created to hold the buffer and linked to the end of the chain.
 * The total size of the chain is updated accordingly.
 * @param chain The buffer chain to append to.
 * @param buffer The buffer to append.
 */
void _cp_buffer_chain_pusht(_cp_Buffer_Chain* chain, const _cp_Buffer* buffer);
/**
 * Push a buffer to the front of the buffer chain.
 * A new node is created to hold the buffer and linked to the front of the chain.
 * The total size of the chain is updated accordingly.
 * @param chain The buffer chain to prepend to.
 * @param buffer The buffer to prepend.
 */
void _cp_buffer_chain_pushf(_cp_Buffer_Chain* chain, const _cp_Buffer* buffer);
/**
 * Pop a buffer from the end of the buffer chain.
 * The node containing the buffer is removed from the chain and freed.
 * The total size of the chain is updated accordingly.
 * @param chain The buffer chain to pop from.
 * @return A pointer to the buffer that was removed from the end of the chain.
 */
_cp_Buffer* _cp_buffer_chain_popt(_cp_Buffer_Chain* chain);
/**
 * Pop a buffer from the front of the buffer chain.
 * The node containing the buffer is removed from the chain and freed.
 * The total size of the chain is updated accordingly.
 * @param chain The buffer chain to pop from.
 * @return A pointer to the buffer that was removed from the front of the chain.
 */
_cp_Buffer* _cp_buffer_chain_popf(_cp_Buffer_Chain* chain);
/**
 * Iterates over each buffer in the buffer chain and applies the specified function that reads the buffer as a constant.
 * The function is called with each buffer and the provided data pointer.
 * @param chain The buffer chain to iterate over.
 * @param func The function to apply to each buffer.
 * @param data A pointer to user-defined data to pass to the function. This can be anything of any type.
 */
void _cp_buffer_chain_iter(const _cp_Buffer_Chain* chain, const void (*func)(const _cp_Buffer* buffer, void* data), void* data);

#endif // _CP_CHAIN_H