/*! \file bcmlrd_field_symbol_to_value.c
 *
 * Return the integer value for a given field symbol.
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
bcmlrd_field_symbol_to_value(int unit,
                             bcmlrd_sid_t sid,
                             bcmlrd_fid_t fid,
                             const char *sym,
                             uint32_t *val)
{
    int rv = SHR_E_PARAM;
    const bcmlrd_field_data_t *field;
    size_t i;

    do {

        if (sym == NULL) {
            break;
        }

        if (val == NULL) {
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

        for (i=0; i < field->edata->num_sym; i++) {
            if (!sal_strcmp(field->edata->sym[i].name, sym)) {
                *val = field->edata->sym[i].val;
                break;
            }
        }

        rv = (i < field->edata->num_sym) ? SHR_E_NONE : SHR_E_UNAVAIL;

    } while (0);

    return rv;
}
