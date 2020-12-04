/*! \file bcmbd_cmicx_miim.h
 *
 * Default CMICx MIIM driver.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMBD_CMICX_MIIM_H
#define BCMBD_CMICX_MIIM_H

#include <bcmbd/bcmbd_miim_internal.h>

/*!
 * \brief Initialize MIIM driver with the default CMICx MIIM driver.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit number.
 */
extern int
bcmbd_cmicx_miim_drv_init(int unit);

#endif /* BCMBD_CMICX_MIIM_H */
