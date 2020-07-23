/*! \file bcmltm_wb_fs_internal.h
 *
 * Logical Table Manager Working Buffer interfaces for Field Selection.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLTM_WB_FS_INTERNAL_H
#define BCMLTM_WB_FS_INTERNAL_H

#include <shr/shr_types.h>

#include <bcmltm/bcmltm_types_internal.h>

/*!
 * \brief Add a Field Selection block type to the working buffer handle.
 *
 * This function adds a Field Selection working buffer block required
 * for tables with field selections.
 *
 * \param [in] unit Unit number.
 * \param [in] num_sels Number of field selections in list.
 * \param [in] list List of field selection IDs.
 * \param [in,out] handle Working buffer handle.
 * \param [out] block_id Returning ID for new working buffer block.
 * \param [out] block_ptr Optional return of pointer to working buffer block.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_wb_block_fs_add(int unit,
                       uint32_t num_sels,
                       const bcmltm_field_selection_id_t *list,
                       bcmltm_wb_handle_t *handle,
                       bcmltm_wb_block_id_t *block_id,
                       bcmltm_wb_block_t **block_ptr);

/*!
 * \brief Get working buffer offset for specified field selection map index.
 *
 * This function gets the map index working buffer offset for
 * the given field selection in the specified Field Selection
 * working buffer block.
 *
 * \param [in] wb_block Working buffer block.
 * \param [in] sel_id Field selection ID.
 * \param [out] offset Returning map index offset.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_wb_fs_map_index_offset_get(const bcmltm_wb_block_t *wb_block,
                                  bcmltm_field_selection_id_t sel_id,
                                  uint32_t *offset);

#endif /* BCMLTM_WB_FS_INTERNAL_H */
