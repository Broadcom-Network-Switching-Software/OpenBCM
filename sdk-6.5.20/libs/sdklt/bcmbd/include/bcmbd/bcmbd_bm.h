/*! \file bcmbd_bm.h
 *
 * Definitions for Behavioral Model (BM) interface.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMBD_BM_H
#define BCMBD_BM_H

#include <bcmdrd/bcmdrd_dev.h>
#include <bcmbd/bcmbd.h>

/*!
 * \brief Initialize DRD features for a device.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmbd_bm_feature_init(int unit);

/*!
 * \brief Initialize BM base driver for a device.
 *
 * This function will initialize default function vectors for
 * register/memory access, etc.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmbd_bm_drv_init(int unit);

/*!
 * \brief Clean up BM base driver for a device.
 *
 * This function will free any resources allocated by \ref
 * bcmbd_bm_drv_init.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmbd_bm_drv_cleanup(int unit);

/*!
 * \brief Initialize basic BM settings.
 *
 * Ensure that endian settings are correct and that all interrupts are
 * disabled.
 *
 * \param [in] unit Unit number.
 *
 * \return 0 if no errors, otherwise a negative value.
 */
extern int
bcmbd_bm_init(int unit);

/*!
 * \brief Read physical table entry.
 *
 * Read a register/memory entry on a BM-based device.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Symbol ID.
 * \param [in] dyn_info Dynamic access information (table index, etc.).
 * \param [in] ovrr_info Optional override control (for debug).
 * \param [out] data Buffer for data entry.
 * \param [in] wsize Size of data entry buffer in 32-bit words.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_TIMEOUT S-channel operation timed out.
 * \retval SHR_E_FAIL Invalid reponse from S-channel operation.
 */
extern int
bcmbd_bm_read(int unit, bcmdrd_sid_t sid,
              bcmbd_pt_dyn_info_t *dyn_info, void *ovrr_info,
              uint32_t *data, size_t wsize);

/*!
 * \brief Write physical table entry.
 *
 * Write a register/memory entry on a BM-based device.
 *
 * The size of the data entry is derived from the symbol information.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Symbol ID.
 * \param [in] dyn_info Dynamic access information (table index, etc.).
 * \param [in] ovrr_info Optional override control (for debug).
 * \param [in] data Data entry to write.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_TIMEOUT S-channel operation timed out.
 * \retval SHR_E_FAIL Invalid reponse from S-channel operation.
 */
extern int
bcmbd_bm_write(int unit, bcmdrd_sid_t sid,
               bcmbd_pt_dyn_info_t *dyn_info, void *ovrr_info,
               uint32_t *data);

#endif /* BCMBD_BM_H */
