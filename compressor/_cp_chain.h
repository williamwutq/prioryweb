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
 * Although buffer is not modified, this function takes a non-const pointer to allow ownership transfer.
 * This means do not pass in buffers that cannot be freed or are constructed by const macros.
 * @param buffer The buffer to store in the node.
 * @return A pointer to the newly created _cp_Buffer_ChainNode.
 */
_cp_Buffer_ChainNode* _cp_buffer_chain_node_create(_cp_Buffer* buffer);
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
void _cp_buffer_chain_pusht(_cp_Buffer_Chain* chain, _cp_Buffer* buffer);
/**
 * Push a buffer to the front of the buffer chain.
 * A new node is created to hold the buffer and linked to the front of the chain.
 * The total size of the chain is updated accordingly.
 * @param chain The buffer chain to prepend to.
 * @param buffer The buffer to prepend.
 */
void _cp_buffer_chain_pushf(_cp_Buffer_Chain* chain, _cp_Buffer* buffer);
/**
 * Push a buffer to the specified index in the buffer chain.
 * If the index is 0, the buffer is pushed to the front.
 * If the index is greater than or equal to the current count, the buffer is pushed to the end.
 * Otherwise, the buffer is inserted at the specified index.
 * The total size of the chain is updated accordingly.
 * 
 * In most cases, use this function is not necessary. For binary split, use _cp_buffer_chain_split and _cp_buffer_chain_concat instead, 
 * as they are much more efficient.
 * @param chain The buffer chain to modify.
 * @param buffer The buffer to insert.
 * @param count The index at which to insert the buffer. THIS INDEX IS NOT CHARACTER OR BYTE BASED, BUT NODE BASED.
 */
void _cp_buffer_chain_push(_cp_Buffer_Chain* chain, _cp_Buffer* buffer, const size_t count);
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
 * Pop a buffer from the specified index in the buffer chain.
 * The node containing the buffer is removed from the chain and freed.
 * The total size of the chain is updated accordingly.
 * 
 * In most cases, use this function is not necessary. For binary split, use _cp_buffer_chain_split and _cp_buffer_chain_concat instead, 
 * as they are much more efficient.
 * @param chain The buffer chain to pop from.
 * @param count The index from which to pop the buffer. THIS INDEX IS NOT CHARACTER OR BYTE BASED, BUT NODE BASED.
 * @return A pointer to the buffer that was removed from the specified index of the chain.
 */
_cp_Buffer* _cp_buffer_chain_pop(_cp_Buffer_Chain* chain, const size_t count);
/**
 * Iterates over each buffer in the buffer chain and applies the specified function that reads the buffer as a constant.
 * The function is called with each buffer and the provided data pointer.
 * @param chain The buffer chain to iterate over.
 * @param func The function to apply to each buffer.
 * @param data A pointer to user-defined data to pass to the function. This can be anything of any type.
 */
void _cp_buffer_chain_iter(const _cp_Buffer_Chain* chain, const void (*func)(const _cp_Buffer* buffer, void* data), void* data);
/**
 * Concatenates two buffer chains into a new buffer chain.
 * The original chains are freed, but their buffers are preserved in the new chain.
 * @param c1 The first buffer chain.
 * @param c2 The second buffer chain.
 * @return A pointer to the newly created concatenated buffer chain.
 */
_cp_Buffer_Chain* _cp_buffer_chain_concat(_cp_Buffer_Chain* c1, _cp_Buffer_Chain* c2);
/**
 * Separates a buffer chain into two chains at the specified index.
 * The original chain is modified to contain only the buffers before the index.
 * A new chain is created containing the buffers from the index onward.
 * @param chain The original buffer chain to separate.
 * @param index The index at which to separate the chain.
 * @return A pointer to the newly created buffer chain containing the separated buffers.
 */
_cp_Buffer_Chain* _cp_buffer_chain_separate(_cp_Buffer_Chain* chain, size_t index);
/**
 * Splits the buffer chain into two halves.
 * The original chain is modified to contain the first half of the buffers.
 * A new chain is created containing the second half of the buffers.
 * If the chain has an odd number of buffers, the extra buffer goes to the first half.
 * @param chain The original buffer chain to split.
 * @return A pointer to the newly created buffer chain containing the second half of the buffers.
 */
_cp_Buffer_Chain* _cp_buffer_chain_split(_cp_Buffer_Chain* chain);
/**
 * Embuffers the entire buffer chain into a single contiguous _cp_Buffer.
 * All buffers in the chain are copied into a new buffer.
 * The original chain is freed after embuffering, so is the buffers within it, except in some cases
 * when some buffers are reused directly to avoid unnecessary copying.
 * @param chain The buffer chain to embuffer.
 * @return A pointer to the newly created _cp_Buffer containing the combined data.
 */
_cp_Buffer* _cp_buffer_chain_embuffer(_cp_Buffer_Chain* chain);
/**
 * Counts the number of buffers in the buffer chain. This number is not the total size in bytes,
 * but the count of individual _cp_Buffer objects contained within the chain.
 * @param chain The buffer chain to count.
 * @return The number of buffers in the chain.
 */
size_t _cp_buffer_chain_count(const _cp_Buffer_Chain* chain);

#endif // _CP_CHAIN_H