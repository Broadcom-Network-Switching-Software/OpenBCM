/*! \file bcmbd_cmicd_mcs.h
 *
 * MCS Definitions for CMICd v2.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMBD_CMICD_MCS_H
#define BCMBD_CMICD_MCS_H

#include <bcmdrd/bcmdrd_types.h>

/*!
 * \brief Get the default CMICd MCS driver.
 *
 * \param [in] unit Unit number.
 *
 * \return Pointer to CMICd MCS driver.
 */
extern const mcs_drv_t *
bcmbd_cmicd_mcs_drv_get(int unit);

/*!
 * \brief Initialize CMICd MCS driver for a device.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmbd_cmicd_mcs_init(int unit);

#endif /* BCMBD_CMICD_MCS_H */
