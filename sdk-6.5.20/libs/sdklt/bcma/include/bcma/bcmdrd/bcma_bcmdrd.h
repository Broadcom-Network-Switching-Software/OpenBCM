/*! \file bcma_bcmdrd.h
 *
 * Common functions for bcmdrd commands
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_BCMDRD_H
#define BCMA_BCMDRD_H

#include <bcmdrd/bcmdrd_types.h>

/*!
 * \brief Get block name from block type.
 *
 * Convert device-specified block type to device-specified block name.
 *
 * \param [in] unit Unit number
 * \param [in] blktype Block type to convert
 *
 * \retval Device-specified block name on success
 * \retval NULL on failure
 */
extern const char *
bcma_bcmdrd_block_type2name(int unit, int blktype);

/*!
 * \brief Get block type from block name.
 *
 * Convert device-specified block name to device-specified block type.
 *
 * \param [in] unit Unit number
 * \param [in] name Block name to convert
 *
 * \retval Device-specified block type on success
 * \retval -1 on failure
 */
extern int
bcma_bcmdrd_block_name2type(int unit, const char *name);

/*!
 * \brief Format block name string from block number.
 *
 * This function will ouput block name string in format of
 * \<block name\>\<block instance\> according to the
 * device-specified block number.
 *
 * \param [in] unit Unit number
 * \param [in] blknum Block number to convert
 * \param [out] dst Buffer to ouput the formatted string
 * \param [in] dstsz Maximum size of the output buffer
 *
 * \return \c dst on success, otherwise "*BADBLK*".
 */
extern char *
bcma_bcmdrd_block_name(int unit, int blknum, char *dst, int dstsz);

/*!
 * \brief Create bit range string.
 *
 * This function will create bit range string for a given port bitmap.
 *
 * \param [out] buf Output buffer for bit range string
 * \param [in] size Size of output buffer
 * \param [in] pbmp Port bitmap for output buffer
 * \param [in] mask Port bitmap that determines how many workds to print
 *
 * \retval Always 0
 */
extern int
bcma_bcmdrd_port_bitmap(char *buf, int size, const bcmdrd_pbmp_t *pbmp,
                       const bcmdrd_pbmp_t *mask);

#endif /* BCMA_BCMDRD_H */
