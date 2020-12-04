/*! \file bcmdrd_sym_profile.c
 *
 * BCMDRD routines related to register and memory symbol profile.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>

#include <sal/sal_assert.h>

#include <bcmdrd/bcmdrd_symbols.h>

#if BCMDRD_CONFIG_INCLUDE_CHIP_SYMBOLS == 1

/* Common NULL entry array of memory symbols. */
const uint32_t bcmdrd_sym_mem_entry_null_zeroes[BCMDRD_MAX_PT_WSIZE] = {0};

static int
bcmdrd_sym_mem_cmp_by_mask(void *ent_a, void *ent_b, void *mask, bool invert, int size)
{
    int idx, bytes;

    bytes = (size & 3);

    for (idx = 1; idx <= bytes; idx++) {
        uint8_t val_a, val_b, m;
        uint8_t *a = (uint8_t *)ent_a;
        uint8_t *b = (uint8_t *)ent_b;
        uint8_t *pm = (uint8_t *)mask;

        m = (invert) ? ~pm[size - idx]: pm[size - idx];
        val_a = a[size - idx] & m;
        val_b = b[size - idx] & m;

        BCMDRD_SYM_MEM_VAL_CMP_RETURN(val_a, val_b);
    }

    for (idx = (size >> 2) - 1; idx >= 0; idx--) {
        uint32_t val_a, val_b, m;
        uint32_t *a = (uint32_t *)ent_a;
        uint32_t *b = (uint32_t *)ent_b;
        uint32_t *pm = (uint32_t *)mask;

        m = (invert) ? ~pm[idx]: pm[idx];
        val_a = a[idx] & m;
        val_b = b[idx] & m;

        BCMDRD_SYM_MEM_VAL_CMP_RETURN(val_a, val_b);
    }

    return 0;
}

int
bcmdrd_sym_mem_cmp_undef(void *ent_a, void *ent_b)
{
    assert(0 && "Cannot compare entries of this type");

    return 0;
}

int
bcmdrd_sym_mem_cmp_imask(void *ent_a, void *ent_b, void *imask, int size)
{
    return bcmdrd_sym_mem_cmp_by_mask(ent_a, ent_b, imask, TRUE, size);
}

int
bcmdrd_sym_mem_cmp_mask(void *ent_a, void *ent_b, void *mask, int size)
{
    return bcmdrd_sym_mem_cmp_by_mask(ent_a, ent_b, mask, FALSE, size);
}

bcmdrd_sym_mem_cmp_f
bcmdrd_sym_mem_cmp_fun_get(const bcmdrd_symbols_t *symbols,
                           const bcmdrd_symbol_t *symbol)
{
    const bcmdrd_sym_mem_profile_t *prfs, *spf;
    int offset;

    if (symbols == NULL || symbol == NULL) {
        return NULL;
    }

    if (!(symbol->flags & BCMDRD_SYMBOL_FLAG_MEMORY)) {
        return NULL;
    }

    /* Get the memory profile array for all the symbols. */
    prfs = symbols->mem_profiles;
    if (prfs == NULL) {
        return NULL;
    }

    offset = BCMDRD_SYM_MP_OFFSET_GET(symbol->pfinfo);
    spf = prfs + offset;
    return spf->cmp_fn;
}

const void *
bcmdrd_sym_mem_null_ent_get(const bcmdrd_symbols_t *symbols,
                            const bcmdrd_symbol_t *symbol)
{
    const bcmdrd_sym_mem_profile_t *prfs, *spf;
    int offset;

    if (symbols == NULL || symbol == NULL) {
        return NULL;
    }

    if (!(symbol->flags & BCMDRD_SYMBOL_FLAG_MEMORY)) {
        return NULL;
    }

    /* Get the memory profile array for all the symbols. */
    prfs = symbols->mem_profiles;
    if (prfs == NULL) {
        return NULL;
    }

    offset = BCMDRD_SYM_MP_OFFSET_GET(symbol->pfinfo);
    spf =  prfs + offset;
    return spf->null_ent;
}

const uint32_t *
bcmdrd_sym_reg_resetval_get(const bcmdrd_symbols_t *symbols,
                            const bcmdrd_symbol_t *symbol)
{
    const uint32_t *prfs;
    int offset;

    if (symbols == NULL || symbol == NULL) {
        return NULL;
    }

    if (!(symbol->flags & BCMDRD_SYMBOL_FLAG_REGISTER)) {
        return NULL;
    }

    /* Get the register resetvals array of the whole symbols. */
    prfs = symbols->reg_profiles;
    if (prfs == NULL) {
        return NULL;
    }

    offset = BCMDRD_SYM_RP_OFFSET_GET(symbol->pfinfo);

    return prfs + offset;
}

bool
bcmdrd_sym_default_value_is_nonzero(const bcmdrd_symbols_t *symbols,
                                    const bcmdrd_symbol_t *symbol)
{
    const uint32_t *resetval;

    if (symbol == NULL) {
        return false;
    }
    if (symbol->flags & BCMDRD_SYMBOL_FLAG_MEMORY) {
        resetval = bcmdrd_sym_mem_null_ent_get(symbols, symbol);
        return (resetval != bcmdrd_sym_mem_entry_null_zeroes);
    }
    if (symbol->flags & BCMDRD_SYMBOL_FLAG_REGISTER) {
        int wsize, idx;

        resetval = bcmdrd_sym_reg_resetval_get(symbols, symbol);
        wsize = BCMDRD_BYTES2WORDS(BCMDRD_SYMBOL_INDEX_SIZE_GET(symbol->index));
        for (idx = 0; idx < wsize; idx++) {
            if (resetval[idx] != 0) {
                return true;
            }
        }
        return false;
    }
    return false;
}

#endif /* BCMDRD_CONFIG_INCLUDE_CHIP_SYMBOLS */
