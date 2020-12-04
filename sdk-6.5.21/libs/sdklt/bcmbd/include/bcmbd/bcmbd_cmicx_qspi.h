/*! \file bcmbd_cmicx_qspi.h
 *
 * BD CMICx QSPI internal API.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMBD_CMICX_QSPI_H
#define BCMBD_CMICX_QSPI_H

/*!
 * \brief Use default CMICx QSPI driver for the given device.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit number.
 */
extern int
bcmbd_cmicx_qspi_drv_init(int unit);

#endif /* BCMBD_CMICX_QSPI_H */
