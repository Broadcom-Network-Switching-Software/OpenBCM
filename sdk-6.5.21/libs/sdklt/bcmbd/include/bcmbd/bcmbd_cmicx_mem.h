/*! \file bcmbd_cmicx_mem.h
 *
 * Programmed I/O access to SOC memory on CMICd-based (v2) device.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMBD_CMICX_MEM_H
#define BCMBD_CMICX_MEM_H

#include <sal/sal_types.h>

/*!
 * \brief Raw programmed I/O read from SOC memory.
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
bcmbd_cmicx_mem_read(int unit, uint32_t adext, uint32_t addr,
                     uint32_t *data, size_t wsize);

/*!
 * \brief Raw programmed I/O write to SOC memory.
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
bcmbd_cmicx_mem_write(int unit, uint32_t adext, uint32_t addr,
                      uint32_t *data, size_t wsize);

/*!
 * \brief Raw DMA operation on SOC memory.
 *
 * \param [in] unit Unit number.
 * \param [in] adext Address extension (bits [47:32]).
 * \param [in] addr Base address (bits [31:0]).
 * \param [in] wsize Size of entry in 32-bit words.
 * \param [in] shift Address increment shift in bits.
 * \param [in] count Count of entries to read.
 * \param [in] buf_paddr Data buffer physical address.
 * \param [in] flags Please refer to \ref BCMBD_ROF_xxx.
 * \param [in] mor_clks Multiple outstanding requests for SBUS.
 *
 * \return 0 if no errors, otherwise a negative value.
 */
extern int
bcmbd_cmicx_mem_range_op(int unit, uint32_t adext, uint32_t addr, size_t wsize,
                         uint32_t shift, uint32_t count, uint64_t buf_paddr,
                         uint32_t flags, uint32_t mor_clks);

/*!
 * \brief Raw DMA read from SOC memory.
 *
 * \param [in] unit Unit number.
 * \param [in] adext Address extension (bits [47:32]).
 * \param [in] addr Base address (bits [31:0]).
 * \param [in] wsize Size of entry in 32-bit words.
 * \param [in] shift Address increment shift in bits.
 * \param [in] count Count of entries to read.
 * \param [in] buf_paddr Data buffer physical address.
 * \param [in] flags Please refer to \ref BCMBD_ROF_xxx.
 *
 * \return 0 if no errors, otherwise a negative value.
 */
extern int
bcmbd_cmicx_mem_range_read(int unit, uint32_t adext, uint32_t addr,
                           size_t wsize, uint32_t shift, uint32_t count,
                           uint64_t buf_paddr, uint32_t flags);

/*!
 * \brief Raw DMA write to SOC memory.
 *
 * \param [in] unit Unit number.
 * \param [in] adext Address extension (bits [47:32]).
 * \param [in] addr Base address (bits [31:0]).
 * \param [in] wsize Size of entry in 32-bit words.
 * \param [in] shift Address increment shift in bits.
 * \param [in] count Count of entries to write.
 * \param [in] buf_paddr Data buffer physical address.
 * \param [in] flags Please refer to \ref BCMBD_ROF_xxx.
 *
 * \return 0 if no errors, otherwise a negative value.
 */
extern int
bcmbd_cmicx_mem_range_write(int unit, uint32_t adext, uint32_t addr,
                            size_t wsize, uint32_t shift, uint32_t count,
                            uint64_t buf_paddr, uint32_t flags);

#endif /* BCMBD_CMICX_MEM_H */
