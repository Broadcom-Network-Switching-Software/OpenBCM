/*! \file bcmlrd_enum_scalar_lookup.c
 *
 * Implementation of bcmlrd_enum_scalar_lookup().
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
bcmlrd_enum_scalar_lookup(const bcmltd_enum_type_t *etype,
                          uint64_t value,
                          const char **symbol)
{
    size_t i;
    int rv = SHR_E_UNAVAIL;

    if (etype == NULL) {
        return SHR_E_PARAM;
    }

    if (symbol == NULL) {
        return SHR_E_PARAM;
    }

    for (i = 0; i < etype->num_sym; i++) {
        if (((uint64_t)etype->sym[i].val) == value) {
            *symbol = etype->sym[i].name;
            rv = SHR_E_NONE;
            break;
        }
    }

    return rv;
}
