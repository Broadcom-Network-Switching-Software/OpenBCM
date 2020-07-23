/*! \file bcmbd_db_intr_internal.h
 *
 * BD db interrupt driver internal API and structures.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMBD_DB_INTR_INTERNAL_H
#define BCMBD_DB_INTR_INTERNAL_H

#include <bcmbd/bcmbd_db_intr.h>

/*!
 * \brief Install device-specific db interrupt driver.
 *
 * Install device-specific db interrupt driver into top-level db
 * interrupt API.
 *
 * Use \c intr_drv = NULL to uninstall a driver.
 *
 * \param [in] unit Unit number.
 * \param [in] intr_drv db interrupt driver object.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Device \c unit does not exist.
 */
extern int
bcmbd_db_intr_drv_init(int unit, bcmbd_intr_drv_t *intr_drv);

#endif /* BCMBD_DB_INTR_INTERNAL_H */
