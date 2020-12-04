/*! \file bcmlrd_table_names_get.c
 *
 * Get the table names for the given unit.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_libc.h>
#include <shr/shr_error.h>
#include <bcmltd/bcmltd_table.h>
#include <bcmlrd/bcmlrd_client.h>
#include <bcmlrd/bcmlrd_internal.h>

int
bcmlrd_table_names_get(int unit, size_t num, const char *buf[], size_t *actual)
{
    size_t i;
    size_t j;
    int rv = SHR_E_PARAM;
    const bcmlrd_map_conf_rep_t *conf;
    do {

        conf = bcmlrd_unit_conf_get(unit);
        if (conf == NULL) {
            /* Should have a configuration for this unit */
            rv = SHR_E_UNIT;
            break;
        }

        if (buf == NULL) {
            break;
        }

        for (i=j=0; i<BCMLRD_TABLE_COUNT; i++) {
            if (j>=num) {
                break;
            }
            if (conf->map[i] != NULL) {
                const bcmltd_table_rep_t *tbl = bcmltd_table_get(i);
                if (tbl == NULL) {
                    break;
                }
                /* add name to list if the table is valid for this conf */
                buf[j++] = tbl->name;
            }
        }

        if (actual) {
            *actual = j;
        }

        rv = SHR_E_NONE;
    } while (0);

    return rv;
}
