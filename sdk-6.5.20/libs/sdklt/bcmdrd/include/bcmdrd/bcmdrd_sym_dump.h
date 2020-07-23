/*! \file bcmdrd_sym_dump.h
 *
 * Symbol dump functions.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMDRD_SYM_DUMP_H
#define BCMDRD_SYM_DUMP_H

#include <shr/shr_pb.h>

#include <bcmdrd/bcmdrd_symbol_types.h>

/*!
 * \name Field formatting flags.
 * \anchor BCMDRD_SYM_DUMP_xxx
 */

/*! \{ */

/*! Skip fields with a value of zero. */
#define BCMDRD_SYM_DUMP_SKIP_ZEROS      (1 << 0)

/*! List fields one a single line (default is one field per line). */
#define BCMDRD_SYM_DUMP_SINGLE_LINE     (1 << 1)

/*! Sort fields in alphabetical order (default is bit order). */
#define BCMDRD_SYM_DUMP_ALPHA_SORT      (1 << 2)

/*! Show bit range information. */
#define BCMDRD_SYM_DUMP_BIT_RANGE       (1 << 3)

/*! Show single-word field values in both decimal and hexadecimal. */
#define BCMDRD_SYM_DUMP_EXT_HEX         (1 << 4)

/*! \} */

/*!
 * \brief Dump fields for a table entry into a print buffer.
 *
 * Decode a table entry according to field definitions in symbol
 * table. Mainly intended for CLI usage.
 *
 * The reset marker of print buffer may be modified by this function.
 *
 * \param [in,out] pb Print buffer.
 * \param [in] symbol Symbol structure (single symbol).
 * \param [in] fnames List of all field names for this unit.
 * \param [in] data Table entry data to process.
 * \param [in] flags Formatting flags (\ref BCMDRD_SYM_DUMP_xxx).
 * \param [in] fcb Filter call-back function for multi-view table entries.
 * \param [in] fcb_cookie Context for filter call-back function.
 * \param [in] ccb Customized call-back function to filter field names or
 *                 field flags.
 * \param [in] ccb_cookie Context for the customized call-back function.
 *
 * \retval 0 No errors.
 */
extern int
bcmdrd_sym_dump_fields(shr_pb_t *pb, const bcmdrd_symbol_t *symbol,
                       const char **fnames, uint32_t *data, uint32_t flags,
                       bcmdrd_symbol_filter_cb_f fcb, void *fcb_cookie,
                       bcmdrd_symbol_custom_filter_cb_f ccb, void *ccb_cookie);

#endif /* BCMDRD_SYM_DUMP_H */
