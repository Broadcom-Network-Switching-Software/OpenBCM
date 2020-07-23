/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/** \file utilex_defragmented_chunk.h
 *
 * Definitions and prototypes for all common utilities related 
 * to defragmented chunk. this data type can support allocations 
 * of pieces in a chunk of memory. the way in which the pieces 
 * are allocated ensures defragmentation on the fly when needed. 
 * the allocated pieces size must be a power of 2. 
 */
#ifndef UTILEX_DEFRAGMENTED_CHUNK_H_INCLUDED
/*
 * { 
 */
#define UTILEX_DEFRAGMENTED_CHUNK_H_INCLUDED

#ifdef BCM_DNX_SUPPORT
/** { */
/*************
* INCLUDES  *
 */
/** { */
#include <soc/dnx/swstate/auto_generated/types/utilex_defragmented_chunk_types.h>
/** } */

/*************
 * DEFINES   *
 */
/** { */
/** } */

/*************
 * MACROS    *
 */
/** { */
/** } */

/*************
 * TYPE DEFS *
 */
/** { */
/** } */

/*************
* GLOBALS   *
 */
/*
 * { 
 */
/*
 * } 
 */

/*************
* FUNCTIONS *
 */
/*
 * { 
 */

/**
 * \brief
 *   Initialize control structure for ALL defragmented chunks
 *   instances expected.
 * \par DIRECT INPUT
 *   \param [in] unit -
 *     Identifier of the device to access.
 *   \param [in] max_nof_chunks -
 *     Maximal number of defragmented chunks instances which can
 *     be sustained simultaneously.
 * \par INDIRECT INPUT
 *   * SWSTATE system
 * \par DIRECT OUTPUT
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
shr_error_e utilex_defragmented_chunk_init(
    int unit,
    uint32 max_nof_chunks);

/**
 * \brief
 *   Free all occupied defragmented chunk instances.
 * \par DIRECT INPUT
 *   \param [in] unit -
 *     Identifier of the device to access.
 * \par DIRECT OUTPUT
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \see
 *   * utilex_hash_table_create()
 */
shr_error_e utilex_defragmented_chunk_destroy_all(
    int unit);

/**
 * \brief
 *   Creates a new defragmented chunk instance.
 * \par DIRECT INPUT
 *   \param [in] unit -
 *     Identifier of the device to access.
 *   \param [in] init_info -
 *     Pointed memory contains setup parameters required for for the
 *     creation of the chunk.
 *   \param [out] chunk_handle -
 *     Pointer to memory to load output into. should be used as
 *     handle to the defragmented chunk in other APIs
 * \par DIRECT OUTPUT
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
shr_error_e utilex_defragmented_chunk_create(
    int unit,
    UTILEX_DEFRAGMENTED_CHUNK_INIT_INFO * init_info,
    uint32 *chunk_handle);

/**
 * \brief
 *   Free the specified chunk instance.
 * \par DIRECT INPUT
 *   \param [in] unit -
 *     Identifier of the device to access.
 *   \param [in] chunk_handle -
 *     Handle of the chunk to destroy.
 * \par DIRECT OUTPUT
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
shr_error_e utilex_defragmented_chunk_destroy(
    int unit,
    uint32 chunk_handle);

/**
 * \brief - allocate a piece in a defragmented chunk. the piece 
 *        consists of slots_in_piece slots. the alloc function
 *        will perform defragmentation if needed.
 * 
 * \param [in] unit - unit number
 * \param [in] chunk_handle - handle of the chunk in which to 
 *        alloc
 * \param [in] slots_in_piece - number of slots in the piece to 
 *        allocate. this number must be a power of 2
 * \param [in] move_cb_additional_info - additional information 
 *        for move CB.
 * \param [out] piece_offset - returned offset of the allocated 
 *        piece - to be used as the handle when free function is
 *        used.
 *   
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e utilex_defragmented_chunk_piece_alloc(
    int unit,
    uint32 chunk_handle,
    uint32 slots_in_piece,
    void *move_cb_additional_info,
    uint32 *piece_offset);

/**
 * \brief - free a piece in a defragmented chunk.
 * 
 * \param [in] unit - unit number
 * \param [in] chunk_handle - handle of the chunk in which to 
 *        free
 * \param [in] piece_offset - hadle for the piece to free. this 
 *        handle must be a handle that was returned by the alloc
 *        function
 *   
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e utilex_defragmented_chunk_piece_free(
    int unit,
    uint32 chunk_handle,
    uint32 piece_offset);

#ifdef UTILEX_DEBUG
/*
 * {
 */

/*
 * }
 */
#endif /* UTILEX_DEBUG */
/** } */
/** } */
#else
/** { */
/**
 * \brief - empty dummy function that does nothing
 * 
 * \param [in] unit - unit number
 *   
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e utilex_defragmented_chunk_dummy_empty_function_to_make_compiler_happy(
    int unit);
/** } */
#endif /* BCM_DNX_SUPPORT */
/*
 * }
 */

/*
 * } UTILEX_DEFRAGMENTED_CHUNK_H_INCLUDED
 */
#endif
