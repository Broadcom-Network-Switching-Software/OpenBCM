/*! \file bcmlrd_table_list_get.c
 *
 * Get a list of tables for the given unit.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_types.h>
#include <shr/shr_error.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmlrd/bcmlrd_internal.h>

int
bcmlrd_table_list_get(int unit, size_t list_max,
                      bcmlrd_sid_t *sid_list, size_t *actual)
{
    size_t i;
    size_t count;
    bcmlrd_sid_t *p;
    int rv = SHR_E_PARAM;
    const bcmlrd_map_conf_rep_t *conf;

    do {
        if (sid_list == NULL) {
            break;
        }

        conf = bcmlrd_unit_conf_get(unit);

        if (conf == NULL) {
            rv = SHR_E_UNAVAIL;
            break;
        }

        p = sid_list;
        count = 0;

        for (i=0; i<BCMLRD_TABLE_COUNT; i++) {
            if (count >= list_max) {
                break;
            }

            if (conf->map[i] != NULL) {
                /* add index to list if the table is valid for this conf */
                *p++ = i;
                count++;
            }
        }

        if (actual) {
            *actual = count;
        }

        rv = SHR_E_NONE;

    } while (0);

    return rv;
}
