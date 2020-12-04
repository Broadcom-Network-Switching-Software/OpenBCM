/*! \file bcma_bcmbd_sym_resetval.c
 *
 * Utility functions for comparing the symbol reset value.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_libc.h>

#include <bcmdrd/bcmdrd_dev.h>

#include <bcma/bcmbd/bcma_bcmbd.h>

int
bcma_bcmbd_resetval_cmp(int unit, const bcmdrd_symbol_t *symbol,
                        uint32_t data_wsz, uint32_t *data)
{
    const bcmdrd_symbols_t *symbols;
    const uint32_t *resetval = NULL;
    uint32_t bytes, words;

    bytes = BCMDRD_SYMBOL_INDEX_SIZE_GET(symbol->index);
    words = BCMDRD_BYTES2WORDS(bytes);
    if (data_wsz != words) {
        return -1;
    }

    symbols = bcmdrd_dev_symbols_get(unit, 0);
    if (symbol->flags & BCMDRD_SYMBOL_FLAG_MEMORY) {
        resetval = bcmdrd_sym_mem_null_ent_get(symbols, symbol);
    } else if (symbol->flags & BCMDRD_SYMBOL_FLAG_REGISTER) {
        resetval = bcmdrd_sym_reg_resetval_get(symbols, symbol);
    }

    if (!resetval) {
        return -1;
    }

    if (sal_memcmp(data, resetval, words * 4) != 0) {
        return -1;
    }

    return 0;
}

