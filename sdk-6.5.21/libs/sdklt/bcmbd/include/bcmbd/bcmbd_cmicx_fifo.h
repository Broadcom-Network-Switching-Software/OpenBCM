/*! \file bcmbd_cmicx_fifo.h
 *
 * FIFO operation for CMICx-based device.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMBD_CMICX_FIFO_H
#define BCMBD_CMICX_FIFO_H

#include <bcmbd/bcmbd_cmicx.h>

/*!
 * \brief CMICx FIFO pop operation.
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
bcmbd_cmicx_fifo_pop(int unit, uint32_t adext, uint32_t addr,
                     uint32_t *data, size_t wsize);

/*!
 * \brief CMICx FIFO push operation.
 *
 * \param [in] unit Unit number.
 * \param [in] adext Address extension (bits [47:32]).
 * \param [in] addr Base address (bits [31:0]).
 * \param [in] data Data entry to push.
 * \param [in] wsize Size of data entry.
 *
 * \return 0 if no errors, otherwise a negative value.
 */
extern int
bcmbd_cmicx_fifo_push(int unit, uint32_t adext, uint32_t addr,
                      uint32_t *data, size_t wsize);

#endif /* BCMBD_CMICX_FIFO_H */
