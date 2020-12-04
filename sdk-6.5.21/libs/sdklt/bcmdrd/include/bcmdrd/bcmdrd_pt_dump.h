/*! \file bcmdrd_pt_dump.h
 *
 * Symbol dump functions.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMDRD_PT_DUMP_H
#define BCMDRD_PT_DUMP_H

#include <bcmdrd/bcmdrd_symbols.h>
#include <bcmdrd/bcmdrd_sym_dump.h>

/*!
 * \brief Dump field information into a print buffer.
 *
 * If \c data is NULL, then fields will be listed, but no field values
 * will be shown.
 *
 * The reset marker of print buffer may be modified by this function.
 *
 * \param [in] unit Unit nunmber.
 * \param [in] sid Symbol ID.
 * \param [in] data Table entry for symbol ID.
 * \param [in] flags Formatting flags (\ref BCMDRD_SYM_DUMP_xxx).
 * \param [in,out] pb Print buffer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid \c sid.
 */
extern int
bcmdrd_pt_dump_fields(int unit, bcmdrd_sid_t sid, uint32_t *data,
                      uint32_t flags, shr_pb_t *pb);

#endif /* BCMDRD_PT_DUMP_H */
