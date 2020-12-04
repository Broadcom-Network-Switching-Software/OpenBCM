/*! \file bcmdrd_pt_dump.c
 *
 * Symbol dump functions.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_error.h>

#include <bcmdrd/bcmdrd_dev.h>
#include <bcmdrd/bcmdrd_pt_dump.h>

int
bcmdrd_pt_dump_fields(int unit, bcmdrd_sid_t sid, uint32_t *data,
                      uint32_t flags, shr_pb_t *pb)
{
    const bcmdrd_symbols_t *symbols;
    const bcmdrd_symbol_t *symbol;

    symbols = bcmdrd_dev_symbols_get(unit, 0);
    symbol = bcmdrd_sym_info_get(symbols, sid, NULL);

    if (symbol == NULL) {
        return SHR_E_PARAM;
    }

    bcmdrd_sym_dump_fields(pb, symbol, symbols->field_names,
                           data, flags,
                           bcmdrd_symbol_field_filter, data, NULL, NULL);

    return SHR_E_NONE;
}
