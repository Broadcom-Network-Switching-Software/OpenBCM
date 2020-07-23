/*! \file bcmlrd_enum_scalar_to_symbol_by_type.c
 *
 * Implementation of bcmlrd_enum_scalar_to_symbol_by_type().
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_error.h>
#include <bcmlrd/bcmlrd_enum.h>

/*******************************************************************************
 * Public functions
 */

int
bcmlrd_enum_scalar_to_symbol_by_type(int unit,
                                     const char *name,
                                     uint64_t value,
                                     const char **symbol)
{
    int rv = SHR_E_UNAVAIL;
    const bcmltd_enum_type_t *enum_type;

    do {
        if (symbol == NULL) {
            rv = SHR_E_PARAM;
            break;
        }

        rv = bcmlrd_enum_find_symbol_type(unit, name, &enum_type);

        if (SHR_FAILURE(rv)) {
            break;
        }

        rv = bcmlrd_enum_scalar_lookup(enum_type, value, symbol);

    } while (0);

    return rv;
}

