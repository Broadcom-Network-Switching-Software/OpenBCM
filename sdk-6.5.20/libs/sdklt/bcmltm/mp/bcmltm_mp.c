/*! \file bcmltm_mp.c
 *
 * Logical Table Manager - Memory Pool.
 *
 * This adaptation of memory pool management is simple:
 * - A new handler is created per given object size.
 * - On an allocation request, if none of the existing pool blocks
 *   can accommodate the requested size, a new memory block
 *   is allocated from the heap and added to the handler block list.
 *
 * These set of interfaces do not support freeing of individual
 * objects within a handle.  The memory is only freed when
 * all the objects that belong to a memory handler are no longer used.
 * This is not useful for cases where objects need to be constantly
 * created and freed within the same handler.
 *
 * These interfaces can be used to save the memory overhead of pointers
 * to small size elements and also to prevent memory fragmentation.
 *
 * Considerations for using these interfaces:
 * - A handle is needed per data element size.
 * - Individual memory pointers cannot be freed.  All memory blocks
 *   and corresponding buffers within a handle are freed together.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_types.h>
#include <shr/shr_debug.h>
#include <sal/sal_libc.h>

#include <bsl/bsl.h>

#include <bcmltm/bcmltm_mp_internal.h>

/*******************************************************************************
 * Private functions
 */

/*!
 * \brief Find block with enough free memory.
 *
 * This routine finds a block in the provided memory pool handle
 * with enough free memory to accommodate a given number of elements.
 *
 * \param [in] handle Memory pool handle.
 * \param [in] elem_count Number of elements.
 *
 * \return Memory pool block with enough free elements,
 * or NULL if all blocks in handle are too small.
 */
static bcmltm_mp_block_t *
mp_block_find(bcmltm_mp_handle_t *handle,
              uint32_t elem_count)
{
    bcmltm_mp_block_t *block = NULL;
    uint32_t free_count;

    block = handle->blocks;

    while (block != NULL) {
        free_count = block->max_count - block->used_count;

        if (free_count >= elem_count) {
            return block;
        }
        block = block->next;
    }

    return block;
}

/*!
 * \brief Add a new block into given memory pool handle.
 *
 * This routine adds a new block into given memory pool handle
 * with a buffer allocation large enough to hold the
 * specified number of elements.
 *
 * \param [in] handle Memory pool handle.
 * \param [in] elem_count Number of elements to allocate in block.
 *
 * \return Memory pool block, or NULL on failure.
 */
static bcmltm_mp_block_t *
mp_block_add(bcmltm_mp_handle_t *handle,
             uint32_t elem_count)
{
    bcmltm_mp_block_t *block = NULL;
    size_t buffer_size;
    size_t size;

    /* Allocate memory for new pool block */
    buffer_size = handle->elem_size * elem_count;
    size = sizeof(bcmltm_mp_block_t) + buffer_size;
    block = sal_alloc(size, handle->str);
    if (block == NULL) {
        return NULL;
    }
    sal_memset(block, 0, size);

    block->max_count = elem_count;
    block->used_count = 0;
    block->buffer = (uint8_t *)&block[1];

    /* Add new block to handle and update handler information */
    block->next = handle->blocks;
    handle->blocks = block;
    handle->num_blocks++;

    return block;
}

/*******************************************************************************
 * Public functions
 */

int
bcmltm_mp_handle_init(bcmltm_mp_handle_t *handle,
                      size_t elem_size,
                      uint32_t max_count,
                      char *str)
{
    if (handle == NULL) {
        return SHR_E_PARAM;
    }

    if ((elem_size == 0) || (max_count == 0)) {
        return SHR_E_PARAM;
    }

    sal_memset(handle, 0, sizeof(*handle));

    handle->str = str;
    handle->elem_size = elem_size;
    handle->default_max_count = max_count;
    handle->num_blocks = 0;
    handle->blocks = NULL;

    return SHR_E_NONE;
}

void
bcmltm_mp_handle_cleanup(bcmltm_mp_handle_t *handle)
{
    bcmltm_mp_block_t *block;
    bcmltm_mp_block_t *next;

    if (handle == NULL) {
        return;
    }

    block = handle->blocks;
    while (block != NULL) {
        next = block->next;
        sal_free(block);
        block = next;
    }

    handle->str = "";
    handle->elem_size = 0;
    handle->default_max_count = 0;
    handle->num_blocks = 0;
    handle->blocks = NULL;

    return;
}

void *
bcmltm_mp_alloc(bcmltm_mp_handle_t *handle,
                uint32_t elem_count)
{
    bcmltm_mp_block_t *block = NULL;
    uint32_t block_max_count;
    uint32_t free_offset;
    void *ptr;

    if (elem_count == 0) {
        return NULL;
    }

    /*
     * Find block with enough memory.
     * If block is not found, add new block.
     */
    block = mp_block_find(handle, elem_count);
    if (block == NULL) {
        /*
         * Add new block.
         * Block allocation element count is the greater of:
         * - default element max count
         * - requested alloc element count
         */
        block_max_count = handle->default_max_count;
        if (elem_count > block_max_count) {
            block_max_count = elem_count;
        }

        block = mp_block_add(handle, block_max_count);
    }

    if (block == NULL) {
        return NULL;
    }

    /*
     * Since a handle deals with a specific data element size
     * (rather than different sizes), any padding is already
     * accounted for when allocating the memory for the buffer:
     *     sizeof(elem) x count
     *
     * There is no need to align the returning memory buffer pointer.
     */
    free_offset = handle->elem_size * block->used_count;
    ptr = &block->buffer[free_offset];

    /* Update block */
    block->used_count += elem_count;

    return (void *)ptr;
}
