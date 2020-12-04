/*! \file bcma_testutil_symbols.c
 *
 * Symbols operation utility.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <sal/sal_libc.h>
#include <bcmdrd/bcmdrd_symbols.h>
#include <bcma/test/util/bcma_testutil_symbols.h>

int
bcma_testutil_symbol_get(
    const bcmdrd_symbols_t *symbols,
    bcma_testutil_symbol_selector_t *ss,
    bcmdrd_symbol_t *s)
{
    int rc;

    for (; bcmdrd_symbols_get(symbols, ss->sindex, s) >= 0; ss->sindex++) {
        if (s->name == 0) {
            /* Last */
            continue;
        }

        /* Check flags which must be present */
        if (ss->pflags && ((s->flags & ss->pflags) != ss->pflags)) {
            /* Symbol does not match */
            continue;
        }

        /* Check flags which must be absent */
        if (ss->aflags && ((s->flags & ss->aflags) != 0)) {
            /* Symbol does not match */
            continue;
        }

        /* Check info which must be present */
        if (ss->pinfo && ((s->info & ss->pinfo) != ss->pinfo)) {
            /* Symbol does not match */
            continue;
        }

        /* Check info which must be absent */
        if (ss->ainfo && ((s->info & ss->ainfo) != 0)) {
            /* Symbol does not match */
            continue;
        }

        if (ss->filter_function && (rc = ss->filter_function(s)) < 0) {
            /* Symbol is filtered by function */
            continue;
        }

        /* Symbol found, move symbol index to next. */
        ss->sindex++;
        return SHR_E_NONE;
    }

    return SHR_E_NOT_FOUND;
}

int
bcma_testutil_symbols_iterate(const bcmdrd_symbols_t *symbols,
                              const char *match_str,
                              bcmdrd_symbols_iter_cb_f cb, void *cb_data)
{
    bcmdrd_symbols_iter_t iter;

    if (match_str == NULL || match_str[0] == '\0') {
        match_str = "*";
    }

    sal_memset(&iter, 0, sizeof(iter));
    iter.symbols = symbols;
    iter.function = cb;
    iter.vptr = cb_data;

    /*
     * By default we list all symbols with the input name as a
     * substring.
     */
    iter.matching_mode = BCMDRD_SYMBOLS_ITER_MODE_STRSTR;

    /*
     * The user can specify explicitly the type of matching with the
     * first character.
     */
    if (sal_strcmp(match_str, "*") != 0) {
        switch (match_str[0]) {
        case '^':
            iter.matching_mode = BCMDRD_SYMBOLS_ITER_MODE_START;
            match_str++;
            break;
        case '*':
            iter.matching_mode = BCMDRD_SYMBOLS_ITER_MODE_STRSTR;
            match_str++;
            break;
        case '@':
            iter.matching_mode = BCMDRD_SYMBOLS_ITER_MODE_EXACT;
            match_str++;
            break;
        default:
            break;
        }
    }
    iter.name = match_str;

    /* Iterate over all matching symbols */
    return bcmdrd_symbols_iter(&iter);
}
