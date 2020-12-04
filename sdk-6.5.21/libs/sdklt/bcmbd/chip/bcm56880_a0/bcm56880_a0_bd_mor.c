/*! \file bcm56880_a0_bd_mor.c
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
#include <bcmdrd/chip/bcm56880_a0_defs.h>

#include <bcmbd/bcmbd_cmicx.h>

#include "bcm56880_a0_drv.h"

/*******************************************************************************
* Local definitions
 */

#define ACCTYPE_SINGLE  20
#define ACCTYPE_UNIQUE  31

/*******************************************************************************
* Public functions
 */

uint32_t
bcm56880_a0_bd_mor_clks_read_get(int unit, bcmdrd_sid_t sid)
{
    uint32_t mor_clks, blktype, acctype;
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
    mor_clks = BCMBD_CMICX_BLKACC2MORCLKS(symbol->info);

    if (mor_clks == 0) {
        /* Default value depends on block type and access type */
        blktype = BCMDRD_SYM_INFO_BLKTYPE(symbol->info, 0);
        switch (blktype) {
        case BLKTYPE_IPIPE:
        case BLKTYPE_EPIPE:
            acctype = BCMDRD_SYM_INFO_ACCTYPE(symbol->info);
            switch (acctype) {
            case ACCTYPE_UNIQUE:
            case ACCTYPE_SINGLE:
                mor_clks = 8;
                break;
            default:
                break;
            }
            break;
        default:
            break;
        }
    }

    return mor_clks;
}
