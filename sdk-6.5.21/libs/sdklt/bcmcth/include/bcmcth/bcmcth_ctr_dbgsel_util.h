/*! \file bcmcth_ctr_dbgsel_util.h
 *
 * CTR_DBGSEL component utility function.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMCTH_CTR_DBGSEL_UTIL_H
#define BCMCTH_CTR_DBGSEL_UTIL_H

#include <bcmdrd/bcmdrd_types.h>
#include <bcmltd/bcmltd_types.h>

#include <bcmcth/bcmcth_ctr_dbgsel_drv.h>

/*!
 * \brief Generic interrupt read function for CTR_DBGSEL component.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_id Logical table identifier.
 * \param [in] pt_id Register/Memory identifier.
 * \param [in] index Logcial port identifier.
 * \param [in] entry_data Returned data value of entry.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmcth_ctr_dbgsel_ireq_read(int unit, bcmltd_sid_t lt_id,
                            bcmdrd_sid_t pt_id, int index, void *entry_data);

/*!
 * \brief Generic interrupt write function for CTR_DBGSEL component.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_id Logical table identifier.
 * \param [in] pt_id Register/Memory identifier.
 * \param [in] index Logcial port identifier.
 * \param [in] entry_data New data value to set to register.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmcth_ctr_dbgsel_ireq_write(int unit, bcmltd_sid_t lt_id,
                             bcmdrd_sid_t pt_id, int index, void *entry_data);

/*!
 * \brief Convert trigger to bitmap for CTR_DBGSEL component.
 *
 * \param [in] enum2bit Triggert to bit position mapping.
 * \param [in] trigger_array Selected trigger array.
 * \param [out] value Converted bitmap.
 *
 * \retval None.
 */
extern void
bcmcth_ctr_dbgsel_bitmap_convert(
    const int *enum2bit,
    const ctr_dbgsel_trigger_t *trigger_array,
    uint64_t *value);

/*!
 * \brief Update trigger in bitmap for CTR_DBGSEL component.
 *
 * \param [in] nshift32 Number of 32bit for shift.
 * \param [in] enum2bit Triggert to bit position mapping.
 * \param [in] trigger_array Selected trigger array.
 * \param [out] value Updated bitmap.
 *
 * \retval None.
 */
extern void
bcmcth_ctr_dbgsel_bitmap_update(
    int nshift32,
    const int *enum2bit,
    const ctr_dbgsel_trigger_t *trigger_array,
    uint32_t *value);

#endif /* BCMCTH_CTR_DBGSEL_UTIL_H */

