/*! \file bcmbd_cmicx_pmac.h
 *
 * PMAC access driver for CMICx.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMBD_CMICX_PMAC_H
#define BCMBD_CMICX_PMAC_H

#include <bcmbd/bcmbd.h>

/*!
 * \brief Read Port/MAC register.
 *
 * \param [in] unit Unit number.
 * \param [in] blk_id S-channel block number of port macro.
 * \param [in] port The port index (lane) in the block.
 * \param [in] addr Register address in PMAC address format.
 * \param [in] idx Entry index
 * \param [in] size Data size in bytes
 * \param [out] data Data buffer
 */
extern int
bcmbd_cmicx_pmac_read(int unit, int blk_id, int port, uint32_t addr,
                      uint32_t idx, size_t size, uint32_t *data);

/*!
 * \brief Write Port/MAC register.
 *
 * \param [in] unit Unit number.
 * \param [in] blk_id S-channel block number of port macro.
 * \param [in] port The port index (lane) in the block.
 * \param [in] addr Register address in PMAC address format.
 * \param [in] idx Entry index
 * \param [in] size Data size in bytes
 * \param [in] data Data buffer
 */
extern int
bcmbd_cmicx_pmac_write(int unit, int blk_id, int port, uint32_t addr,
                       uint32_t idx, size_t size, uint32_t *data);

#endif /* BCMBD_CMICX_PMAC_H */
