/*! \file bcmbd_cmicx_reg.h
 *
 * Programmed I/O access to SOC register on CMICd-based (v2) device.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMBD_CMICX_REG_H
#define BCMBD_CMICX_REG_H

#include <bcmbd/bcmbd_cmicx.h>

/*!
 * \brief Raw programmed I/O read from SOC register.
 *
 * \param [in] unit Unit number.
 * \param [in] adext Address extension (bits [47:32]).
 * \param [in] addr Base address (bits [31:0]).
 * \param [out] data Buffer for data entry.
 * \param [in] wsize Size of buffer in 32-bit words.
 *
 * \return 0 if no errors, otherwise a negative value.
 */
extern int
bcmbd_cmicx_reg_read(int unit, uint32_t adext, uint32_t addr,
                     uint32_t *data, size_t wsize);

/*!
 * \brief Raw programmed I/O write to SOC register.
 *
 * \param [in] unit Unit number.
 * \param [in] adext Address extension (bits [47:32]).
 * \param [in] addr Base address (bits [31:0]).
 * \param [in] data Data entry to write.
 * \param [in] wsize Size of data entry.
 *
 * \return 0 if no errors, otherwise a negative value.
 */
extern int
bcmbd_cmicx_reg_write(int unit, uint32_t adext, uint32_t addr,
                      uint32_t *data, size_t wsize);

#endif /* BCMBD_CMICX_REG_H */
