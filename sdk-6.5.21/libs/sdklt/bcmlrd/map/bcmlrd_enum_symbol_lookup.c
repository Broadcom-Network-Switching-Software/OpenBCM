/*! \file bcmlrd_enum_symbol_lookup.c
 *
 * Implementation of bcmlrd_enum_symbol_lookup().
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_libc.h>
#include <shr/shr_error.h>
#include <bcmlrd/bcmlrd_enum.h>

/*******************************************************************************
 * Public functions
 */

int
bcmlrd_enum_symbol_lookup(const bcmltd_enum_type_t *etype,
                          const char *symbol,
                          uint64_t *value)
{
    size_t i;
    int rv = SHR_E_UNAVAIL;
    const shr_enum_map_t *enum_map;

    if (etype == NULL) {
        return SHR_E_PARAM;
    }

    if (symbol == NULL) {
        return SHR_E_PARAM;
    }

    if (value == NULL) {
        return SHR_E_PARAM;
    }

    enum_map = etype->sym;

    for (i = 0; i < etype->num_sym; i++) {
        if (!sal_strcmp(symbol, enum_map[i].name)) {
            *value = enum_map[i].val;
            rv = SHR_E_NONE;
            break;
        }
    }

    return rv;
}
