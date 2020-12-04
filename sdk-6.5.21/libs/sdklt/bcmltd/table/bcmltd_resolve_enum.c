/*! \file bcmltd_resolve_enum.c
 *
 * Resolve enum by table field data.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_libc.h>
#include <shr/shr_error.h>
#include <bcmltd/bcmltd_internal.h>
#include <bcmltd/bcmltd_table.h>

/*******************************************************************************
 * Public functions
 */

int
bcmltd_resolve_enum(const uint32_t sid,
                    const uint32_t fid,
                    const char *str,
                    uint64_t *value)
{
    int rv = SHR_E_NONE;
    const bcmltd_table_rep_t *tbl;
    uint32_t i;
    const bcmltd_field_rep_t *field = NULL;

    do {
        tbl = bcmltd_table_get(sid);
        if (tbl == NULL) {
            rv = SHR_E_UNAVAIL;
            break;
        }

        if (fid >= tbl->fields) {
            rv = SHR_E_UNAVAIL;
            break;
        }

        field = &tbl->field[fid];

        if (!field->edata) {
            /* Not an enum. */
            rv = SHR_E_UNAVAIL;
            break;
        }

        for (i = 0; i < field->edata->num_sym; i++) {
            if (!sal_strcmp(str, field->edata->sym[i].name)) {
                /* Resolved! */
                *value = field->edata->sym[i].val;
                break;
            }
        }

        if (i == field->edata->num_sym) {
            /* Symbol not found. */
            rv = SHR_E_UNAVAIL;
            break;
        }
    } while (0);

    return rv;
}

