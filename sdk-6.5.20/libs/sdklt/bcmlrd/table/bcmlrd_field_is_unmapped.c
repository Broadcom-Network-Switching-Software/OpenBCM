/*! \file bcmlrd_field_is_unmapped.c
 *
 * Check if the logical field is unmapped.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_libc.h>
#include <shr/shr_error.h>
#include <bcmltd/bcmltd_table.h>
#include <bcmlrd/bcmlrd_internal.h>
#include <bcmlrd/bcmlrd_client.h>

int
bcmlrd_field_is_unmapped(int unit,
                         bcmlrd_sid_t sid,
                         bcmlrd_fid_t fid,
                         bool *unmapped)
{
    int rv = SHR_E_PARAM;
    const bcmlrd_map_conf_rep_t *conf;
    const bcmlrd_table_rep_t *tbl;
    const bcmlrd_map_t *map;

    do {

        conf = bcmlrd_unit_conf_get(unit);
        if (conf == NULL) {
            /* Should have a configuration for this unit */
            rv = SHR_E_UNAVAIL;
            break;
        }

        if (unmapped == NULL) {
            break;
        }

        /* Get table */
        tbl = bcmltd_table_get(sid);
        if (tbl == NULL) {
            rv = SHR_E_UNAVAIL;
            break;
        }

        /* Get map */
        map = conf->map[sid];
        if (map == NULL) {
            break;
        }

        /* FID out of range */
        if (fid >= tbl->fields) {
            rv = SHR_E_UNAVAIL;
            break;
        }

        *unmapped = bcmlrd_field_is_unmapped_internal(map, fid);
        rv = SHR_E_NONE;

    } while (0);

    return rv;
}

