/*! \file bcmltm_wb_apic_internal.h
 *
 * Logical Table Manager Working Buffer interfaces for API Cache.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLTM_WB_APIC_INTERNAL_H
#define BCMLTM_WB_APIC_INTERNAL_H

#include <shr/shr_types.h>

#include <bcmltm/bcmltm_types_internal.h>

/*!
 * \brief Add an API Cache block type to the working buffer handle.
 *
 * This function adds an API Cache working buffer block required
 * to copy fields between the input/output API field list and
 * the API field cache.
 *
 * \param [in] unit Unit number.
 * \param [in] num_fields Number of fields in list.
 * \param [in] list List of field index counts.
 * \param [in,out] handle Working buffer handle.
 * \param [out] block_id Returning ID for new working buffer block.
 * \param [out] block_ptr Optional return of pointer to working buffer block.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_wb_block_apic_add(int unit,
                         uint32_t num_fields,
                         const bcmltm_field_idx_count_t *list,
                         bcmltm_wb_handle_t *handle,
                         bcmltm_wb_block_id_t *block_id,
                         bcmltm_wb_block_t **block_ptr);

/*!
 * \brief Get working buffer offset for specified field element.
 *
 * This function gets the working buffer offset for the specified
 * field element (fid, idx) in the given API Cache working buffer block.
 *
 * \param [in] wb_block Working buffer block.
 * \param [in] field_id Field ID.
 * \param [in] field_idx Field index.
 * \param [out] offset Returning working buffer offset.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_wb_apic_field_offset_get(const bcmltm_wb_block_t *wb_block,
                                uint32_t field_id,
                                uint32_t field_idx,
                                uint32_t *offset);

#endif /* BCMLTM_WB_APIC_INTERNAL_H */
