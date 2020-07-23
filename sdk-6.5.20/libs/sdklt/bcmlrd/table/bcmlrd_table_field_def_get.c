/*! \file bcmlrd_table_field_def_get.c
 *
 * Get the field definition for the given table and field.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_libc.h>
#include <shr/shr_error.h>
#include <bcmltd/bcmltd_table.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmlrd/bcmlrd_internal.h>

int
bcmlrd_table_field_def_get(int unit,
                           const bcmlrd_sid_t sid,
                           const bcmlrd_fid_t fid,
                           bcmlrd_field_def_t *field_def)
{
    int rv = SHR_E_PARAM;
    const bcmlrd_map_conf_rep_t *conf;
    const bcmlrd_table_rep_t *tbl;
    const bcmlrd_map_t *map;

    do {

        conf = bcmlrd_unit_conf_get(unit);

        if (conf == NULL) {
            rv = SHR_E_UNAVAIL;
            break;
        }

        if (field_def == NULL) {
            break;
        }

        /* Check SID */
        if (sid >= BCMLRD_TABLE_COUNT) {
            rv = SHR_E_UNAVAIL;
            break;
        }

        tbl = bcmltd_table_get(sid);
        if ((tbl == NULL) ||
            (fid >= tbl->fields)) {
            rv = SHR_E_UNAVAIL;
            break;
        }

        map = conf->map[sid];
        if ((map == NULL) ||
            (fid >= map->field_data->fields)) {
            rv = SHR_E_UNAVAIL;
            break;
        }

        rv = bcmlrd_field_def_get(unit, fid, tbl, map, field_def);
    } while (0);

    return rv;
}

