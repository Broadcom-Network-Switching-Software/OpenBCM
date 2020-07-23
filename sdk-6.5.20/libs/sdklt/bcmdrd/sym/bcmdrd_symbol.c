/*! \file bcmdrd_symbol.c
 *
 * BCMDRD symbol routines.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>

#include <sal/sal_libc.h>
#include <sal/sal_assert.h>

#include <shr/shr_error.h>

#include <bcmdrd/bcmdrd_field.h>
#include <bcmdrd/bcmdrd_symbols.h>

#if BCMDRD_CONFIG_INCLUDE_CHIP_SYMBOLS == 1

/*
 * The characters in names are composed of alphabet letters, numeric numbers
 * and the underscore letter ('_'). The ASCII codes for these letters are:
 * ['0'-'9'] < ['A'-'Z'] < '_' < ['a'-'z']. When comparing to the underscore
 * letter in binary search, the result will be different for upper case or
 * lower case comparison.
 * The upper case comparison method should always be used here for the
 * binary search of symbols names and symbol alias names since these two
 * tables are sorted in upper case order.
 */
static int
str_upr_cmp(const char *dest, const char *src)
{
    char dc,sc;
    int rv = 0;

    while (1) {
        dc = sal_toupper(*dest++);
        sc = sal_toupper(*src++);
        if ((rv = dc - sc) != 0 || !dc) {
            break;
        }
    }
    return rv;
}

#if BCMDRD_CONFIG_INCLUDE_ALIAS_NAMES == 1
/*
 * String compare function of binary search for symbol alias names.
 */
static int
sym_alias_name_upr_cmp(const void *k, const void *s)
{
    const bcmdrd_enum_map_t *key = (const bcmdrd_enum_map_t *)k;
    const bcmdrd_enum_map_t *sym = (const bcmdrd_enum_map_t *)s;

    return str_upr_cmp(key->name, sym->name);
}
#endif

/*
 * String compare function of binary search for symbol names.
 */
static int
sym_name_upr_cmp(const void *k, const void *s)
{
    const bcmdrd_symbol_t *key = (const bcmdrd_symbol_t *)k;
    const bcmdrd_symbol_t *sym = (const bcmdrd_symbol_t *)s;

    return str_upr_cmp(key->name, sym->name);
}

/*
 * Binary search function for symbol alias names.
 */
static const bcmdrd_symbol_t *
sym_bsearch_alias_names(const char *name, const bcmdrd_symbols_t *symbols)
{
    const bcmdrd_symbol_t *sym = NULL;
#if BCMDRD_CONFIG_INCLUDE_ALIAS_NAMES == 1
    bcmdrd_enum_map_t key;
    const bcmdrd_enum_map_t *map;

    key.name = (char *)name;
    map = sal_bsearch(&key, symbols->alias_names, symbols->alias_names_size,
                      sizeof(key), sym_alias_name_upr_cmp);
    if (map) {
        sym = symbols->symbols + map->val;
    }
#endif
    return sym;
}

/*
 * Binary search function for symbol names.
 */
static const bcmdrd_symbol_t *
sym_bsearch_names(const char *name, const bcmdrd_symbols_t *symbols)
{
    const bcmdrd_symbol_t *sym;
    bcmdrd_symbol_t key;
    int sect;
    uint32_t sect_size;
    uint32_t sect_offset;

    key.name = name;
    sect_offset = 0;
    /* Search each section until we find a match */
    for (sect = 0; sect < COUNTOF(symbols->sect_size); sect++) {
        sect_size = symbols->sect_size[sect];
        if (sect_size == 0) {
            continue;
        }
        sym = sal_bsearch(&key, &symbols->symbols[sect_offset], sect_size,
                          sizeof(key), sym_name_upr_cmp);
        if (sym) {
            return sym;
        }
        sect_offset += sect_size;
    }
    /* Search entire table if no sections were defined */
    if (sect_offset == 0) {
        sym = sal_bsearch(&key, symbols->symbols, symbols->size,
                          sizeof(key), sym_name_upr_cmp);
    }
    return sym;
}

static const bcmdrd_symbol_t *
symbol_from_sid(const bcmdrd_symbols_t *symbols, bcmdrd_sid_t sid)
{
    if (symbols == NULL) {
        return NULL;
    }
    if (sid >= symbols->size) {
        return NULL;
    }
    return &symbols->symbols[sid];
}

const bcmdrd_symbol_t *
bcmdrd_sym_info_get(const bcmdrd_symbols_t *symbols, bcmdrd_sid_t sid,
                    bcmdrd_sym_info_t *sinfo)
{
    const bcmdrd_symbol_t *symbol;
    uint32_t size;

    symbol = symbol_from_sid(symbols, sid);
    if (symbol == NULL) {
        return NULL;
    }

    if (sinfo == NULL) {
        return symbol;
    }

    sal_memset(sinfo, 0, sizeof(*sinfo));
    sinfo->sid = sid;
    sinfo->name = symbol->name;
    sinfo->offset = symbol->addr;
    sinfo->flags = symbol->flags;
    sinfo->blktypes = symbol->info;
    sinfo->index_min = BCMDRD_SYMBOL_INDEX_MIN_GET(symbol->index);
    sinfo->index_max = BCMDRD_SYMBOL_INDEX_MAX_GET(symbol->index);
    size = BCMDRD_SYMBOL_INDEX_SIZE_GET(symbol->index);
    sinfo->entry_wsize = BCMDRD_BYTES2WORDS(size);
    sinfo->step_size = BCMDRD_SYMBOL_INDEX_STEP_GET(symbol->index);

    return symbol;
}

const bcmdrd_symbol_t *
bcmdrd_symbol_find(const char *name,
                   const bcmdrd_symbol_t *table, uint32_t size,
                   bcmdrd_sid_t *sid)
{
    uint32_t idx;
    const bcmdrd_symbol_t *sym = table;

    assert(table);

    for (idx = 0; idx < size; idx++) {
        if (sal_strcasecmp(sym->name, name) == 0) {
            break;
        }
#if BCMDRD_CONFIG_INCLUDE_ALIAS_NAMES == 1
        if (sym->ufname && sal_strcasecmp(sym->ufname, name) == 0) {
            break;
        }
        if (sym->alias && sal_strcasecmp(sym->alias, name) == 0) {
            break;
        }
#endif
        sym++;
    }
    if (idx >= size) {
        sym = NULL;
    } else if (sid) {
        *sid = idx;
    }
    return sym;
}

const bcmdrd_symbol_t *
bcmdrd_symbols_fast_find(const char *name, const bcmdrd_symbols_t *symbols,
                         bcmdrd_sid_t *sid)
{
    const bcmdrd_symbol_t *sym;

    if (symbols == NULL || name == NULL || name[0] == '\0') {
        return NULL;
    }

    if (!symbols->sorted) {
        LOG_WARN(BSL_LS_BCMDRD_DEV,
                 (BSL_META("The symbols table is not sorted for bsearch.\n")));
        return NULL;
    }

    /* Search for symbol names first. If not found, search for alias names. */
    sym = sym_bsearch_names(name, symbols);
    if (sym == NULL) {
        sym = sym_bsearch_alias_names(name, symbols);
    }

    if (sym && sid) {
        *sid = sym - symbols->symbols;
    }
    return sym;
}

const bcmdrd_symbol_t *
bcmdrd_symbols_find(const char *name, const bcmdrd_symbols_t *symbols,
                    bcmdrd_sid_t *sid)
{
    if (symbols == NULL || symbols->symbols == NULL) {
        return NULL;
    }

    if (symbols->sorted) {
        return bcmdrd_symbols_fast_find(name, symbols, sid);
    }
    return bcmdrd_symbol_find(name, symbols->symbols, symbols->size, sid);
}

int
bcmdrd_symbols_get(const bcmdrd_symbols_t *symbols, uint32_t sindex,
                   bcmdrd_symbol_t *rsym)
{
    if (symbols) {
        if (sindex < symbols->size) {
            /* Index is within the symbol table */
            *rsym = symbols->symbols[sindex];
            return 0;
        }
    }
    return -1;
}

int
bcmdrd_symbols_index(const bcmdrd_symbols_t *symbols,
                     const bcmdrd_symbol_t *symbol)
{
    int size, idx;

    if (symbols == NULL || symbols->symbols == NULL) {
        return -1;
    }

    size = symbols->size;
    for (idx = 0; idx < size; idx++) {
        /* Sufficient to compare just the pointers */
        if (symbol->name == symbols->symbols[idx].name) {
            return idx;
        }
    }
    return -1;
}

#endif
