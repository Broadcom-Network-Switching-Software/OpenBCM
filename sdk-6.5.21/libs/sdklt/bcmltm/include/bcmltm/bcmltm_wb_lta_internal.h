/*! \file bcmltm_wb_lta_internal.h
 *
 * Logical Table Manager Working Buffer Definitions for LTA interfaces.
 *
 * These definitions are used to support Logical Tables that
 * requires LTA interfaces used for the following scenarios:
 *     Custom Table Handler
 *     Field Transforms
 *     Field Validations
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLTM_WB_LTA_INTERNAL_H
#define BCMLTM_WB_LTA_INTERNAL_H

#include <shr/shr_types.h>

#include <bcmltm/bcmltm_wb_types_internal.h>



/*! LTA Input field list. */
#define BCMLTM_WB_LTA_INPUT_FIELDS        0

/*! LTA Input key field list. */
#define BCMLTM_WB_LTA_INPUT_KEY_FIELDS    1

/*! LTA Output field list. */
#define BCMLTM_WB_LTA_OUTPUT_FIELDS       2


/*!
 * \brief Add a LTA Custom Table Handler working buffer block.
 *
 * This function adds a working buffer block required for a
 * Custom Table Handler for the given number of logical fields
 * elements (fid, idx).
 *
 * \param [in] unit Unit number.
 * \param [in] num_fields Number of LT field elements (fid, idx).
 * \param [in,out] handle Working buffer handle.
 * \param [out] block_id Returning ID for new working buffer block.
 * \param [out] block_ptr Optional return of pointer to working buffer block.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_wb_block_lta_cth_add(int unit,
                            uint32_t num_fields,
                            bcmltm_wb_handle_t *handle,
                            bcmltm_wb_block_id_t *block_id,
                            bcmltm_wb_block_t **block_ptr);


/*!
 * \brief Add a LTA Field Transform working buffer block.
 *
 * This function adds a working buffer block required for a
 * LTA field transform for the given number of input and output
 * fields elements (fid, idx).
 *
 * \param [in] unit Unit number.
 * \param [in] in_num_fields Number of input fields elements (fid, idx).
 * \param [in] in_key_num_fields Number of input key fields elements (fid, idx).
 * \param [in] out_num_fields Number of output fields elements (fid, idx).
 * \param [in,out] handle Working buffer handle.
 * \param [out] block_id Returning ID for new working buffer block.
 * \param [out] block_ptr Optional return of pointer to working buffer block.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_wb_block_lta_xfrm_add(int unit,
                             uint32_t in_num_fields,
                             uint32_t in_key_num_fields,
                             uint32_t out_num_fields,
                             bcmltm_wb_handle_t *handle,
                             bcmltm_wb_block_id_t *block_id,
                             bcmltm_wb_block_t **block_ptr);


/*!
 * \brief Add a LTA Field Validation working buffer block.
 *
 * This function adds a working buffer block required for a
 * LTA field validation for the given number of input
 * fields elements (fid, idx).
 *
 * \param [in] unit Unit number.
 * \param [in] in_num_fields Number of input fields elements (fid, idx).
 * \param [in,out] handle Working buffer handle.
 * \param [out] block_id Returning ID for new working buffer block.
 * \param [out] block_ptr Optional return of pointer to working buffer block.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_wb_block_lta_val_add(int unit,
                            uint32_t in_num_fields,
                            bcmltm_wb_handle_t *handle,
                            bcmltm_wb_block_id_t *block_id,
                            bcmltm_wb_block_t **block_ptr);


/*!
 * \brief Get LTA fields working buffer offsets.
 *
 * This function gets the LTA fields offsets for the given field
 * type in the specified LTA working buffer block.
 *
 * The field_type is either:
 *   BCMLTM_WB_LTA_INPUT_FIELDS
 *   BCMLTM_WB_LTA_OUTPUT_FIELDS
 *
 * \param [in] wb_block Working buffer block.
 * \param [in] field_type Indicates input or output field list.
 * \param [out] offsets Returning LTA fields offsets.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_wb_lta_fields_offsets_get(const bcmltm_wb_block_t *wb_block,
                                 int field_type,
                                 bcmltm_wb_lta_fields_offsets_t *offsets);


/*!
 * \brief Get working buffer offset for a LTA field.
 *
 * This function gets the working buffer offset for a field structure
 * in the specified LTA working buffer block.
 *
 * The field index indicates the index into the array of field
 * structures allocated in the working buffer space.
 * Index range should be [0 .. num_fields-1].
 *
 * \param [in] wb_block Working buffer block.
 * \param [in] field_type Indicates input or output field list.
 * \param [in] field_idx Index of field array to return offset for.
 * \param [out] offset Returning field offset.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_wb_lta_field_map_offset_get(const bcmltm_wb_block_t *wb_block,
                                   int field_type,
                                   uint32_t field_idx,
                                   uint32_t *offset);

#endif /* BCMLTM_WB_LTA_INTERNAL_H */
