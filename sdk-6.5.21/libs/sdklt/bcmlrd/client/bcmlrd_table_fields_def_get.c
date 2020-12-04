/*! \file bcmlrd_table_fields_def_get.c
 *
 * Get the field definitions for the given table name.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_libc.h>
#include <shr/shr_error.h>
#include <bcmltd/bcmltd_types.h>
#include <bcmltd/bcmltd_table.h>
#include <bcmlrd/bcmlrd_client.h>
#include <bcmlrd/bcmlrd_internal.h>

int
bcmlrd_table_fields_def_get(int unit,
                            const char *table_name,
                            size_t number_of_fields,
                            bcmlrd_client_field_info_t *fields_info,
                            size_t *actual)
{
    int rv = SHR_E_PARAM;
    const bcmlrd_map_conf_rep_t *conf;
    int idx;
    const bcmlrd_table_rep_t *tbl;
    const bcmlrd_map_t *map;
    size_t src;
    size_t dst;

    do {

        conf = bcmlrd_unit_conf_get(unit);
        if (conf == NULL) {
            /* Should have a configuration for this unit */
            rv = SHR_E_UNIT;
            break;
        }

        if (table_name == NULL) {
            break;
        }

        if (fields_info == NULL) {
            break;
        }

        idx = bcmlrd_table_name_to_idx(unit, table_name);
        if (SHR_FAILURE(idx)) {
            rv = idx;
            break;
        }

        tbl = bcmltd_table_get(idx);
        if (tbl == NULL) {
            break;
        }

        map = conf->map[idx];
        if (map == NULL) {
            break;
        }

        for (src = dst = 0; src < tbl->fields; src++) {
            if (dst >= number_of_fields) {
                break;
            }
            rv = bcmlrd_field_def_get(unit, src, tbl, map, &fields_info[dst]);
            if (SHR_FAILURE(rv)) {
                if (rv == SHR_E_UNAVAIL) {
                    /* This field is not mapped, so skip it. */
                    continue;
                } else {
                    break;
                }
            }
            dst++;
        }

        if (actual) {
            *actual = dst;
        }

        rv = SHR_E_NONE;

    } while (0);

    return rv;
}

