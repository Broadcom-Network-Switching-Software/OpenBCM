/*! \file bcmltm_md_logical_internal.h
 *
 * Logical Table Manager for Logical Table Metadata.
 *
 * These definitions are internal to the LTM component and is
 * intended, therefore, to be used only by LTM sub-components.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLTM_MD_LOGICAL_INTERNAL_H
#define BCMLTM_MD_LOGICAL_INTERNAL_H

#include <shr/shr_types.h>

#include <bcmltm/bcmltm_md_internal.h>
#include <bcmltm/bcmltm_internal.h>

/*!
 * \brief Initialize the LTM metadata for Logical Tables on a given unit.
 *
 * This function requires caller to synchronize access to this routine
 * by possible multiple threads.
 *
 * Assumes unit is valid.
 *
 * \param [in] unit Unit number.
 * \param [in] warm Indicates if this is cold or warm boot.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_md_logical_create(int unit, bool warm);


/*!
 * \brief Destroy the LTM metadata for Logical Tables on a given unit.
 *
 * Cleanup all data associated with the LTM Logical Table metadata for
 * the specified unit.
 *
 * This function requires caller to synchronize access to this routine
 * by possible multiple threads.
 *
 * Assumes unit is valid.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_md_logical_destroy(int unit);


/*!
 * \brief Retrieve the metadata for a given logical table.
 *
 * Get the pointer to the table metadata for a given table id
 * on the specified unit.
 *
 * Assumes unit is valid.
 *
 * \param [in] unit Unit number.
 * \param [in] ltid Table ID.
 * \param [out] ltm_md_ptr Returns pointer to the table metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_md_logical_lt_retrieve(int unit,
                              uint32_t ltid,
                              bcmltm_lt_md_t **ltm_md_ptr);


/*!
 * \brief Get the maximum Working Buffer size for the indicated LT type.
 *
 * Get the maximum Working Buffer size to support the configured Logical
 * Tables.  This maximum is for either modeled LTs or interactive LTs.
 *
 * \param [in] unit Unit number.
 * \param [in] mode Table operating mode.
 * \param [out] wb_max Returns Working Buffer maximum.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_md_logical_wb_max_get(int unit,
                             bcmltm_table_mode_t mode,
                             uint32_t *wb_max);

/*!
 * \brief Reset the state data for given logical table.
 *
 * This routine resets the state data for the specified
 * logical table to the initial state during cold boot.
 *
 * \param [in] unit Unit number.
 * \param [in] ltid Table ID.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_md_logical_state_data_reset(int unit, uint32_t ltid);

/*!
 * \brief Get the valid bitmap of track indexes for given logical table.
 *
 * This routine gets the valid bitmap of track indexes for
 * the given logical table.
 *
 * Caller should allocate enough memory for the returning
 * bitmap buffer.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [out] valid_bitmap Returning valid track indexes bitmap.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_md_logical_valid_track_index_bitmap_get(int unit,
                                               uint32_t sid,
                                               SHR_BITDCL *valid_bitmap);

#endif /* BCMLTM_MD_LOGICAL_INTERNAL_H */
