/*! \file bcmlrd_field_symbols_count_get.c
 *
 * Return the number of symbols for a given field.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_libc.h>
#include <shr/shr_error.h>
#include <bcmlrd/bcmlrd_client.h>

int
bcmlrd_field_symbols_count_get(int unit,
                               bcmlrd_sid_t sid,
                               bcmlrd_fid_t fid,
                               size_t *count)
{
    int rv = SHR_E_PARAM;
    const bcmlrd_field_data_t *field;

    do {


        if (!count) {
            break;
        }

        rv = bcmlrd_field_get(unit, sid, fid, &field);
        if (SHR_FAILURE(rv)) {
            break;
        }

        if (!(field->flags & BCMLRD_FIELD_F_ENUM)) {
            rv = SHR_E_UNAVAIL;
            /* This field is not an enum. */
            break;
        }

        if (!field->edata) {
            rv = SHR_E_INTERNAL;
            /* Says it's an enum but no symbol data. */
            break;
        }

        *count = field->edata->num_sym;

        rv = SHR_E_NONE;

    } while (0);

    return rv;
}
