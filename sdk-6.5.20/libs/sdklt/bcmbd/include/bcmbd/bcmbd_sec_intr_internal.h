/*! \file bcmbd_sec_intr_internal.h
 *
 * BD sec interrupt driver internal API and structures.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMBD_SEC_INTR_INTERNAL_H
#define BCMBD_SEC_INTR_INTERNAL_H

#include <bcmbd/bcmbd_sec_intr.h>

/*!
 * \brief Install device-specific sec interrupt driver.
 *
 * Install device-specific sec interrupt driver into top-level sec
 * interrupt API.
 *
 * Use \c intr_drv = NULL to uninstall a driver.
 *
 * \param [in] unit Unit number.
 * \param [in] intr_drv sec interrupt driver object.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Device \c unit does not exist.
 */
extern int
bcmbd_sec_intr_drv_init(int unit, bcmbd_intr_drv_t *intr_drv);

/*!
 * \brief Install device-specific sec instrument interrupt driver.
 *
 * Install device-specific sec instrument interrupt driver into top-level
 * sec instrument interrupt API.
 *
 * Use \c intr_drv = NULL to uninstall a driver.
 *
 * \param [in] unit Unit number.
 * \param [in] intr_drv sec instrument interrupt driver object.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Device \c unit does not exist.
 */
extern int
bcmbd_seci_intr_drv_init(int unit, bcmbd_intr_drv_t *intr_drv);

#endif /* BCMBD_SEC_INTR_INTERNAL_H */
