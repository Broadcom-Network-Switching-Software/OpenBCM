/*! \file bcmlrd_field_list_get.c
 *
 * Get a list of fields for the given table.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_types.h>
#include <shr/shr_error.h>
#include <bcmltd/bcmltd_table.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmlrd/bcmlrd_internal.h>

int
bcmlrd_field_list_get(int unit, bcmlrd_sid_t sid, size_t list_max,
                      bcmlrd_fid_t *fid_list, size_t *num_fid)
{
    size_t src;
    size_t dst;
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

        if (fid_list == NULL) {
            break;
        }

        /* Check SID */
        if (sid >= BCMLRD_TABLE_COUNT) {
            rv = SHR_E_UNAVAIL;
            break;
        }

        tbl = bcmltd_table_get(sid);
        if (tbl == NULL) {
            /* Table ID is valid, but not defined for some reason */
            rv = SHR_E_UNAVAIL;
            break;
        }

        map = conf->map[sid];
        if (map == NULL) {
            /* Table ID is valid, but not for this unit */
            rv = SHR_E_UNAVAIL;
            break;
        }

        for (src = dst = 0; src < tbl->fields; src++) {
            if (bcmlrd_field_is_unmapped_internal(map, src)) {
                /* This field is not mapped, so skip it. */
                continue;
            }
            if (dst >= list_max) {
                break;
            }
            fid_list[dst] = src;
            dst++;
        }

        if (num_fid) {
            *num_fid = dst;
        }

        rv = SHR_E_NONE;

    } while (0);

    return rv;
}
