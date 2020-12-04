/*! \file bcmltm_wb_internal.h
 *
 * Logical Table Manager Working Buffer Definitions.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLTM_WB_INTERNAL_H
#define BCMLTM_WB_INTERNAL_H

#include <shr/shr_types.h>
#include <shr/shr_pb_local.h>

#include <bcmdrd/bcmdrd_pt.h>

#include <bcmltm/bcmltm_wb_types_internal.h>


/*!
 * \brief Initialize a working buffer handle.
 *
 * This function initializes a working buffer handle for the given
 * table ID.
 *
 * This handle is then used to request memory blocks in the working buffer
 * needed by the table operations.
 *
 * \param [in] sid Table ID.
 * \param [in] logical Indicates if given table is logical or physical.
 * \param [out] handle Working buffer handle to initialize.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_wb_handle_init(uint32_t sid,
                      bool logical,
                      bcmltm_wb_handle_t *handle);


/*!
 * \brief Cleanup a working buffer handle.
 *
 * This function frees any memory allocated during the construction
 * of the working buffer blocks.
 *
 * \param [in] handle Working buffer handle to cleanup.
 */
extern void
bcmltm_wb_handle_cleanup(bcmltm_wb_handle_t *handle);


/*!
 * \brief Validate given handle.
 *
 * This function checks that given handle is valid and has been
 * initialized.
 *
 * \param [in] handle Working buffer handle to check.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_wb_handle_validate(bcmltm_wb_handle_t *handle);

/*!
 * \brief Get working buffer size in words for given handle.
 *
 * The routine returns the working buffer size, in words, for
 * the given working buffer handle.
 *
 * \param [in] handle Working buffer handle to get size from.
 *
 * \retval Working buffer size in words.
 */
extern uint32_t
bcmltm_wb_wsize_get(const bcmltm_wb_handle_t *handle);


/*!
 * \brief Add a block into given working buffer handle.
 *
 * The routine adds a working buffer block of the specified size
 * and type into the given working buffer.
 *
 * Caller of this routine needs to fill out the specific
 * block information pertinent to that type.
 *
 * \param [in] wsize Block size, in words, to add.
 * \param [in] type Block type.
 * \param [in, out] handle Working buffer handle.
 *
 * \retval Working buffer block pointer.
 *         NULL on failure.
 */
extern bcmltm_wb_block_t *
bcmltm_wb_block_add(uint32_t wsize, bcmltm_wb_block_type_t type,
                    bcmltm_wb_handle_t *handle);


/*!
 * \brief Find and return pointer to working buffer block for given block ID.
 *
 * This routine finds the working buffer block for the given block ID
 * in the specified working buffer handle.
 *
 * \param [in] handle Working buffer handle.
 * \param [in] block_id Block ID to find.
 *
 * \retval Pointer to working buffer block.
 *         NULL if not found.
 */
extern const bcmltm_wb_block_t *
bcmltm_wb_block_find(const bcmltm_wb_handle_t *handle,
                     bcmltm_wb_block_id_t block_id);

/*!
 * \brief Return number of working buffer blocks for given block type.
 *
 * This routine returns the number of working buffer blocks
 * for the given block type in the specified working buffer handle.
 *
 * \param [in] handle Working buffer handle.
 * \param [in] block_type Working buffer type.
 *
 * \retval Number of working buffer blocks for specified block type.
 */
extern uint32_t
bcmltm_wb_block_type_count(const bcmltm_wb_handle_t *handle,
                           bcmltm_wb_block_type_t block_type);

/*!
 * \brief Get list of pointers to working buffer blocks for given block type.
 *
 * This routine gets the list of pointers to working buffer blocks that
 * matches the given block type for the specified working buffer handle.
 * The list must be large enough to return all matching blocks.
 *
 * \param [in] handle Working buffer handle.
 * \param [in] block_type Working buffer type to return list for.
 * \param [in] list_max Size of allocated list.
 * \param [out] block_list List of working buffer block pointers.
 * \param [out] num Actual number of working buffer blocks returned.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_wb_block_list_get(const bcmltm_wb_handle_t *handle,
                         bcmltm_wb_block_type_t block_type,
                         uint32_t list_max,
                         const bcmltm_wb_block_t **block_list,
                         uint32_t *num);
/*!
 * \brief Display information for given working buffer handle.
 *
 * This routine displays the working buffer information for given handle.
 *
 * \param [in] handle Working buffer handle.
 * \param [in,out] pb Print buffer to populate output data.
 */
extern void
bcmltm_wb_dump(const bcmltm_wb_handle_t *handle,
               shr_pb_t *pb);

#endif /* BCMLTM_WB_INTERNAL_H */
