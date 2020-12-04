/*! \file bcmbd_hotswap_internal.h
 *
 * BD PCIE hotswap internal API and structures.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMBD_HOTSWAP_INTERNAL_H
#define BCMBD_HOTSWAP_INTERNAL_H

/*!
 * \brief Recover the PCIe hot swap.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_IO I/O error.
 * \retval SHR_E_TIMEOUT Timeout waiting for hotswap state to recover.
 */
typedef int (*bcmbd_hotswap_recover_f)(int unit);

/*!
 * \brief Set PCIe hot swap enable/disable.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_IO I/O error.
 */
typedef int (*bcmbd_hotswap_enable_set_f)(int unit, bool enable);

/*!
 * \brief Get PCIe hot swap enable status.
 *
 * \param [in] unit Unit number.
 * \param [out] Eanble/disable status.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_IO I/O error.
 */
typedef int (*bcmbd_hotswap_enable_get_f)(int unit, bool *enable);

/*!
 * \brief PCIE hot swap driver object.
 */
typedef struct bcmbd_hotswap_drv_s {

    /*! Recover hot swap */
    bcmbd_hotswap_recover_f hotswap_recover;

    /*! Enable/disable hot swap. */
    bcmbd_hotswap_enable_set_f hotswap_enable_set;

    /*! Get hot swap enable status */
    bcmbd_hotswap_enable_get_f hotswap_enable_get;
} bcmbd_hotswap_drv_t;

/*!
 * Initialize PCIE hot swap driver.
 *
 * \param [in] unit Unit number.
 * \param [in] drv Driver for the given device.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit number.
 */
extern int
bcmbd_hotswap_drv_init(int unit, bcmbd_hotswap_drv_t *drv);

#endif /* BCMBD_HOTSWAP_INTERNAL_H */
