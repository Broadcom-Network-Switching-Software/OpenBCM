/*! \file bcmlrd_resolve_enum.c
 *
 * Resolve enum by LRD map field data.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_libc.h>
#include <shr/shr_error.h>
#include <bcmlrd/bcmlrd_enum.h>
#include <bcmlrd/bcmlrd_map.h>

int
bcmlrd_resolve_enum(int unit,
                    uint32_t sid,
                    uint32_t fid,
                    const char *str,
                    uint64_t *value)
{
    int rv = SHR_E_NONE;
    bcmlrd_map_t const *map;
    uint32_t i, nfields;
    const bcmlrd_field_data_t *field;

    do {
        /* Check params. */
        if (str == NULL) {
            rv = SHR_E_PARAM;
            break;
        }

        if (value == NULL) {
            rv = SHR_E_PARAM;
            break;
        }

        /* Resolve by map. */
        rv = bcmlrd_map_get(unit, sid, &map);
        if (SHR_FAILURE(rv)) {
            break;
        }

        nfields = map->field_data->fields;
        if (fid >= nfields) {
            rv = SHR_E_UNAVAIL;
            break;
        }

        field = &map->field_data->field[fid];
        for (i = 0; i < field->edata->num_sym; i++) {
            if (!sal_strcmp(str, field->edata->sym[i].name)) {
                /* Resolved! */
                *value = field->edata->sym[i].val;
                break;
            }
        }

        /* Resolve by table. */
        if (i == field->edata->num_sym) {
            /* Symbol not found. */
            rv = bcmltd_resolve_enum(sid, fid, str, value);
        }
    } while (0);

    return rv;
}

