/*! \file bcmltm_wb_lt_internal.h
 *
 * Logical Table Manager - Logical Table Working Buffer Definitions.
 *
 * This file contains definitions for the management of
 * the working buffer of Logical Tables.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLTM_WB_LT_INTERNAL_H
#define BCMLTM_WB_LT_INTERNAL_H

#include <shr/shr_types.h>

#include <bcmltm/bcmltm_wb_types_internal.h>


/*!
 * \brief Initialize working buffer information for logical tables.
 *
 * This routine initializes the working buffer data structure
 * for the given number of logical tables.  It allocates memory, then
 * populates the data structures.
 *
 * \param [in] unit Unit number.
 * \param [in] sid_max_count Maximum number of table IDs on unit.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_wb_lt_info_init(int unit, uint32_t sid_max_count);


/*!
 * \brief Cleanup LT working buffer information.
 *
 * This routine frees any memory allocated during the
 * construction of the working buffer information for the given unit.
 *
 * \param [in] unit Unit number.
 */
extern void
bcmltm_wb_lt_info_cleanup(int unit);


/*!
 * \brief Return the working buffer handle for given table ID.
 *
 * This function returns the working buffer handle for the given table ID.
 *
 * The returned handle can be used to add new working buffer blocks
 * or to obtain information on the working buffer during the
 * table metadata construction.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID to return handle for.
 *
 * \retval Pointer to working buffer handle.
 *         NULL if not found.
 */
extern bcmltm_wb_handle_t *
bcmltm_wb_lt_handle_get(int unit, uint32_t sid);


/*!
 * \brief Return working buffer block for given table and block ID.
 *
 * This function returns the working buffer block for the given table
 * and working buffer block ID.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] block_id ID of working buffer block to return.
 *
 * \retval Pointer to working buffer block.
 *         NULL if not found.
 */
extern const bcmltm_wb_block_t *
bcmltm_wb_lt_block_get(int unit, uint32_t sid,
                       bcmltm_wb_block_id_t block_id);

/*!
 * \brief Return LT Private working buffer block for given table.
 *
 * This routine returns the LT Private (internal elements)
 * working buffer block for the given table.  Any logical table
 * cannot have more than one block of this type, so it is possible
 * to find the block given a unit and table id.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 *
 * \retval Pointer to working buffer block.
 *         NULL if not found or error.
 */
extern const bcmltm_wb_block_t *
bcmltm_wb_lt_pvt_block_get(int unit, uint32_t sid);

/*!
 * \brief Get the LT Private working buffer offsets by table ID.
 *
 * This function gets the LT internal element offsets for the given table.
 * It is possible to find the offset by table ID since
 * a logical table cannot have more than 1 LT Private block.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [out] offsets Returning LT internal element offsets.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_wb_lt_pvt_offsets_get_by_sid(int unit, uint32_t sid,
                                    bcmltm_wb_lt_pvt_offsets_t *offsets);

/*!
 * \brief Get the track index offset for given table.
 *
 * This function gets the track index offset for the specified table.
 * It is possible to find the offset by table ID since
 * a logical table cannot have more than 1 LT Private block.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [out] offset Returning working buffer offset for track index.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_wb_lt_track_index_offset_get_by_sid(int unit, uint32_t sid,
                                           uint32_t *offset);

/*!
 * \brief Get the index absent offset for given table.
 *
 * This function gets the index absent offset for the specified table.
 * It is possible to find the offset by table ID since
 * a logical table cannot have more than 1 LT Private block.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [out] offset Returning working buffer offset for index absent.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_wb_lt_index_absent_offset_get_by_sid(int unit, uint32_t sid,
                                            uint32_t *offset);

/*!
 * \brief Get the PT suppress status offset for given table.
 *
 * This function gets the PT suppress status offset for the specified table.
 * It is possible to find the offset by table ID since
 * a logical table cannot have more than 1 LT Private block.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [out] offset Returning working buffer offset for PT suppress status.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_wb_lt_pt_suppress_offset_get_by_sid(int unit, uint32_t sid,
                                           uint32_t *offset);

/*!
 * \brief Get the working buffer size for given table ID.
 *
 * Get the working buffer size in WORDS for given logical table ID.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [out] size Returning size in WORDS.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_wb_lt_wsize_get(int unit, uint32_t sid,
                       uint32_t *size);

/*!
 * \brief Return API Cache working buffer block for given table.
 *
 * This routine returns the API Cache working buffer block
 * for the given table.
 *
 * Any logical table cannot have more than one block of this type,
 * so it is possible to find the block given a unit and table id.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 *
 * \retval Pointer to working buffer block.
 *         NULL if not found or error.
 */
extern const bcmltm_wb_block_t *
bcmltm_wb_lt_apic_block_get(int unit, uint32_t sid);

/*!
 * \brief Get the offset to the given field element by table ID.
 *
 * This function gets the field element (fid, idx) working buffer offset
 * in the specified table ID.
 *
 * It is possible to find the offset by table ID since
 * a logical table cannot have more than 1 API Cache block.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] field_id Field ID.
 * \param [in] field_idx Field index.
 * \param [out] offset Returning working buffer offset for field (fid, idx).
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_wb_lt_apic_field_offset_get_by_sid(int unit, uint32_t sid,
                                          uint32_t field_id,
                                          uint32_t field_idx,
                                          uint32_t *offset);

/*!
 * \brief Return Field Selection working buffer block for given table.
 *
 * This routine returns the Field Selection working buffer block
 * for the given table.
 *
 * Any logical table cannot have more than one block of this type,
 * so it is possible to find the block given a unit and table id.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 *
 * \retval Pointer to working buffer block.
 *         NULL if not found or error.
 */
extern const bcmltm_wb_block_t *
bcmltm_wb_lt_fs_block_get(int unit, uint32_t sid);

/*!
 * \brief Get the offset to the field selection map index by table ID.
 *
 * This function gets the map index working buffer offset
 * for the given field selection ID in the specified table ID.
 *
 * It is possible to find the offset by table ID since
 * a logical table cannot have more than 1 Field Selection block.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] sel_id Field selection ID.
 * \param [out] offset Returning map index offset.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_wb_lt_fs_map_index_offset_get_by_sid(int unit, uint32_t sid,
                                            bcmltm_field_selection_id_t sel_id,
                                            uint32_t *offset);

#endif /* BCMLTM_WB_LT_INTERNAL_H */
