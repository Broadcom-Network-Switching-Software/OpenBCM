/*! \file bcmdrd_symbols_iter.c
 *
 * Iteration function in DRD symbol table.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_libc.h>

#include <bcmdrd/bcmdrd_symbols.h>

static int
sym_match(bcmdrd_symbols_iter_t *iter, const char *sym_name)
{
    switch(iter->matching_mode) {
    case BCMDRD_SYMBOLS_ITER_MODE_EXACT:
        if (sal_strcasecmp(sym_name, iter->name) == 0) {
            /* Name matches */
            return 1;
        }
        break;
    case BCMDRD_SYMBOLS_ITER_MODE_START:
        if (sal_strncasecmp(sym_name, iter->name,
                            sal_strlen(iter->name)) == 0) {
            /* Name matches */
            return 1;
        }
        break;
    case BCMDRD_SYMBOLS_ITER_MODE_STRSTR:
        if (sal_strcasestr(sym_name, iter->name) != NULL) {
            /* Name matches */
            return 1;
        }
        break;
    default:
        break;
    }
    return 0;
}

static bool
sym_flag_info_match(bcmdrd_symbols_iter_t *iter, const bcmdrd_symbol_t *s)
{
    /* Check flags which must be present */
    if (iter->pflags && ((s->flags & iter->pflags) != iter->pflags)) {
        /* Symbol does not match */
        return false;
    }

    /* Check flags which must be absent */
    if (iter->aflags && ((s->flags & iter->aflags) != 0)) {
        /* Symbol does not match */
        return false;
    }

    return true;
}

static int
symbols_fast_iter(bcmdrd_symbols_iter_t *iter)
{
    int count = 0;
    int rc;
    bcmdrd_symbol_t s;
    const bcmdrd_symbol_t *sym;
    bcmdrd_sid_t sid;

    sym = bcmdrd_symbols_fast_find(iter->name, iter->symbols, &sid);
    if (sym && sym_flag_info_match(iter, sym)) {
        s = *sym;
        if ((rc = iter->function(&s, sid, iter->vptr)) < 0) {
            return rc;
        }
        count++;
    }
    return count;
}

int
bcmdrd_symbols_iter(bcmdrd_symbols_iter_t *iter)
{
    int count = 0;
    int rc;
    int match;
    uint32_t idx;
    bcmdrd_symbol_t s;
    bool is_wildcard;

    if (iter == NULL || iter->symbols == NULL) {
        return 0;
    }

    is_wildcard = (sal_strcmp("*", iter->name) == 0);
    if (!is_wildcard && iter->symbols->sorted &&
        iter->matching_mode == BCMDRD_SYMBOLS_ITER_MODE_EXACT) {
        return symbols_fast_iter(iter);
    }

    for (idx = 0; bcmdrd_symbols_get(iter->symbols, idx, &s) >= 0; idx++) {

        if (s.name == 0) {
            /* Last */
            continue;
        }

        if (!sym_flag_info_match(iter, &s)) {
            continue;
        }

        /* Check the name */
        match = 0;
        if (!is_wildcard) {
            /* Not wildcarded */
            if (sym_match(iter, s.name)) {
                match = 1;
            }
#if BCMDRD_CONFIG_INCLUDE_ALIAS_NAMES == 1
            else if (s.ufname && sym_match(iter, s.ufname)) {
                match = 1;
            }
            else if (s.alias && sym_match(iter, s.alias)) {
                match = 1;
            }
#endif
            if (!match) {
                continue;
            }
        }

        /* Whew, name is okay */
        count++;

        if ((rc = iter->function(&s, idx, iter->vptr)) < 0) {
            return rc;
        }
    }

    return count;
}
