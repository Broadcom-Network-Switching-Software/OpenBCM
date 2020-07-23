/*! \file bcmbd_cmicx_mcs.h
 *
 * MCS Definitions for CMICx.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMBD_CMICX_MCS_H
#define BCMBD_CMICX_MCS_H

#include <bcmdrd/bcmdrd_types.h>

/*!
 * \brief Get the default CMICx MCS driver.
 *
 * \param [in] unit Unit number.
 *
 * \return Pointer to CMICx MCS driver.
 */
extern const mcs_drv_t *
bcmbd_cmicx_mcs_drv_get(int unit);

/*!
 * \brief Initialize CMICx MCS driver for a device.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmbd_cmicx_mcs_init(int unit);

#endif /* BCMBD_CMICX_MCS_H */
