/*! \file bcmltm_wb_lt_pvt_internal.h
 *
 * Logical Table Manager Working Buffer Definitions for LT Private
 * Internal Elements.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLTM_WB_LT_PVT_INTERNAL_H
#define BCMLTM_WB_LT_PVT_INTERNAL_H

#include <shr/shr_types.h>

#include <bcmltm/bcmltm_wb_types_internal.h>


/*!
 * \brief Add a LT Private working buffer block.
 *
 * This function adds a working buffer block required for
 * the internal elements of Logical Tables.
 *
 * \param [in] unit Unit number.
 * \param [in,out] handle Working buffer handle.
 * \param [out] block_id Returning ID for new working buffer block.
 * \param [out] block_ptr Optional return of pointer to working buffer block.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_wb_block_lt_pvt_add(int unit,
                           bcmltm_wb_handle_t *handle,
                           bcmltm_wb_block_id_t *block_id,
                           bcmltm_wb_block_t **block_ptr);


/*!
 * \brief Get the LT Private working buffer offsets.
 *
 * This function gets the LT internal element offsets
 * in the specified LT private working buffer block.
 *
 * \param [in] wb_block Working buffer block.
 * \param [out] offsets Returning LT private element offsets.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_wb_lt_pvt_offsets_get(const bcmltm_wb_block_t *wb_block,
                             bcmltm_wb_lt_pvt_offsets_t *offsets);

#endif /* BCMLTM_WB_LT_PVT_INTERNAL_H */
