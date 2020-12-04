/*! \file bcmbd_port_intr_internal.h
 *
 * BD port interrupt driver internal API and structures.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMBD_PORT_INTR_INTERNAL_H
#define BCMBD_PORT_INTR_INTERNAL_H

#include <bcmbd/bcmbd_port_intr.h>

/*!
 * \brief Install device-specific port interrupt driver.
 *
 * Install device-specific port interrupt driver into top-level port
 * interrupt API.
 *
 * Use \c intr_drv = NULL to uninstall a driver.
 *
 * \param [in] unit Unit number.
 * \param [in] intr_drv Port interrupt driver object.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Device \c unit does not exist.
 */
extern int
bcmbd_port_intr_drv_init(int unit, bcmbd_intr_drv_t *intr_drv);

#endif /* BCMBD_PORT_INTR_INTERNAL_H */
