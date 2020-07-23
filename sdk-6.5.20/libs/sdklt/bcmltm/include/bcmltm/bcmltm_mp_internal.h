/*! \file bcmltm_mp_internal.h
 *
 * Logical Table Manager - Memory Pool Interfaces.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLTM_MP_INTERNAL_H
#define BCMLTM_MP_INTERNAL_H

#include <shr/shr_types.h>

/*!
 * Memory Pool Block.
 */
typedef struct bcmltm_mp_block_s {
    /*! Next block of memory. */
    struct bcmltm_mp_block_s *next;

    /*! Total elements allocated. */
    uint32_t max_count;

    /*! Current number of elements used. */
    uint32_t used_count;

    /*! Memory pool buffer. */
    uint8_t *buffer;
} bcmltm_mp_block_t;

/*!
 * Memory Pool Handler.
 *
 * This structure contains the memory pool handler information
 * for a given data size.
 */
typedef struct bcmltm_mp_handle_s {
    /*! Alloc text string for tracking purposes. */
    char *str;

    /*! Element size (bytes). */
    size_t elem_size;

    /*! Default number of elements to allocate in new block. */
    uint32_t default_max_count;

    /*! Number of blocks. */
    uint32_t num_blocks;

    /*! List of memory pool blocks. */
    bcmltm_mp_block_t *blocks;
} bcmltm_mp_handle_t;


/*!
 * \brief Initialize a memory pool handle.
 *
 * This routine initializes a memory pool handle for the given
 * data element size.
 *
 * \param [out] handle Memory pool handle to initialize.
 * \param [in] elem_size Data element size in bytes.
 * \param [in] max_count Default element count to allocate in new block.
 * \param [in] str Alloc text string for tracking purposes.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_mp_handle_init(bcmltm_mp_handle_t *handle,
                      size_t elem_size,
                      uint32_t max_count,
                      char *str);

/*!
 * \brief Cleanup a memory pool handle.
 *
 * This routine releases all memory pool blocks and frees
 * all the memory allocated by the handle.
 *
 * \param [in] handle Memory pool handle.
 */
extern void
bcmltm_mp_handle_cleanup(bcmltm_mp_handle_t *handle);

/*!
 * \brief Allocate requested number of elements from memory pool handle.
 *
 * This routine allocates memory for the requested number of elements
 * from the specified memory pool handle.  The elements are returned
 * in contiguous memory locations (array).
 *
 * \param [in] handle Memory pool handle.
 * \param [in] elem_count Number of elements to allocate.
 *
 * \return Pointer to allocated memory, or NULL on error.
 */
extern void *
bcmltm_mp_alloc(bcmltm_mp_handle_t *handle,
                uint32_t elem_count);

#endif /* BCMLTM_MP_INTERNAL_H */
