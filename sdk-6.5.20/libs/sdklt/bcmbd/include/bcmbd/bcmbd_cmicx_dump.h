/*! \file bcmbd_cmicx_dump.h
 *
 * Dump register/memory access data.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMBD_CMICX_DUMP_H
#define BCMBD_CMICX_DUMP_H

#include <sal/sal_libc.h>
#include <shr/shr_pb.h>

/*!
 * \brief Function to dump a register/memory address.
 *
 * A function of this type can be used to override the default
 * behavior of the register/memory address dump function used by debug
 * messages in low-level access functions.
 *
 * \param [in,out] pb Print buffer.
 * \param [in] unit Unit number.
 * \param [in] sym_flags Symbol flags from DRD.
 * \param [in] adext Address extension (bits [47:32]).
 * \param [in] addr Base address (bits [31:0]).
 *
 * \retval SHR_E_NONE Address successfuly decoded.
 * \retval SHR_E_FAIL Error decoding address.
 */
typedef int (*bcmbd_dump_addr_f)(shr_pb_t *pb, int unit, uint32_t sym_flags,
                                 uint32_t adext, uint32_t addr);

/*!
 * \brief Dump access data buffer.
 *
 * \param [in,out] pb Print buffer.
 * \param [in] data Buffer for data entry.
 * \param [in] wsize Size of buffer in 32-bit words.
 *
 * \return 0 if no errors, otherwise a negative value.
 */
extern void
bcmbd_cmicx_dump_data(shr_pb_t *pb,
                      uint32_t *data, size_t wsize);

/*!
 * \brief Dump register access data.
 *
 * \param [in,out] pb Print buffer.
 * \param [in] unit Unit number.
 * \param [in] adext Address extension (bits [47:32]).
 * \param [in] addr Base address (bits [31:0]).
 * \param [in] data Buffer for data entry.
 * \param [in] wsize Size of buffer in 32-bit words.
 *
 * \return 0 if no errors, otherwise a negative value.
 */
extern void
bcmbd_cmicx_dump_reg(shr_pb_t *pb, int unit,
                     uint32_t adext, uint32_t addr,
                     uint32_t *data, size_t wsize);

/*!
 * \brief Dump memory access data.
 *
 * \param [in,out] pb Print buffer.
 * \param [in] unit Unit number.
 * \param [in] adext Address extension (bits [47:32]).
 * \param [in] addr Base address (bits [31:0]).
 * \param [in] data Buffer for data entry.
 * \param [in] wsize Size of buffer in 32-bit words.
 *
 * \return 0 if no errors, otherwise a negative value.
 */
extern void
bcmbd_cmicx_dump_mem(shr_pb_t *pb, int unit,
                     uint32_t adext, uint32_t addr,
                     uint32_t *data, size_t wsize);

/*!
 * \brief Configure address dump function for debug messages.
 *
 * By default, register and memory addresses in verbose/debug messages
 * will be shown in raw hex format. This API can be used to install a
 * function that can perform a full symbolic decoding of a raw
 * address.
 *
 * The reason for making this configurable is to avoid that the
 * low-level register and memory access functions depend on access to
 * the full symbol table, e.g. if the SDK is used with limited
 * storage, such as a boot ROM.
 *
 * \param [in] unit Unit number.
 * \param [in] dump_addr_func Address decoding function to install.
 *
 * \return Nothing.
 */
extern int
bcmbd_cmicx_dump_addr_func_set(int unit, bcmbd_dump_addr_f dump_addr_func);

#endif /* BCMBD_CMICX_DUMP_H */
