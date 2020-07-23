/*! \file bcmlrd_enum_symbol_to_scalar_by_type.c
 *
 * Implementation of bcmlrd_enum_scalar_to_symbol_by_type().
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
bcmlrd_enum_symbol_to_scalar_by_type(int unit,
                                     const char *name,
                                     const char *symbol,
                                     uint64_t *value)
{
    int rv = SHR_E_UNAVAIL;
    const bcmltd_enum_type_t *enum_type;

    do {
        if (symbol == NULL) {
            rv = SHR_E_PARAM;
            break;
        }

        if (value == NULL) {
            rv = SHR_E_PARAM;
            break;
        }

        rv = bcmlrd_enum_find_symbol_type(unit, name, &enum_type);

        if (SHR_FAILURE(rv)) {
            break;
        }

        rv = bcmlrd_enum_symbol_lookup(enum_type, symbol, value);

    } while (0);

    return rv;
}
