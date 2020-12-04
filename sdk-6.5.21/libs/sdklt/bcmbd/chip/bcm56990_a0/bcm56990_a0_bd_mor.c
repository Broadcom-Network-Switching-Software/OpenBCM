/*! \file bcm56990_a0_bd_mor.c
 *
 * Device-specific functions for SBUS MOR (multiple outstanding
 * requests).
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>

#include <shr/shr_debug.h>

#include <bcmdrd/bcmdrd_symbols.h>

#include <bcmbd/bcmbd_cmicx.h>

#include "bcm56990_a0_drv.h"


/*******************************************************************************
* Public functions
 */

uint32_t
bcm56990_a0_bd_mor_clks_read_get(int unit, bcmdrd_sid_t sid)
{
    const bcmdrd_symbols_t *symbols;
    const bcmdrd_symbol_t *symbol;

    symbols = bcmdrd_dev_symbols_get(unit, 0);
    if (symbols == NULL) {
        return 0;
    }
    symbol = bcmdrd_sym_info_get(symbols, sid, NULL);
    if (symbol == NULL) {
        return 0;
    }

    /* Use symbol-specific value if defined */
    return BCMBD_CMICX_BLKACC2MORCLKS(symbol->info);
}
