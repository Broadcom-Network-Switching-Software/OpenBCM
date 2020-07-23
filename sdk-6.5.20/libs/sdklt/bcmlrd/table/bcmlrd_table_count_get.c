/*! \file bcmlrd_table_count_get.c
 *
 * Get the number of valid tables for the given unit.
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
bcmlrd_table_count_get(int unit, size_t *num_sid)

{
    int rv = SHR_E_PARAM;
    const bcmlrd_map_conf_rep_t *conf;
    size_t i;
    size_t count;

    do {

        conf = bcmlrd_unit_conf_get(unit);

        if (num_sid == NULL) {
            break;
        }

        if (conf == NULL) {
            /* No configuration means no tables. */
            count = 0;
        } else {
            /* Count what is mapped. */
            for (count=i=0; i<BCMLRD_TABLE_COUNT; i++) {
                /* if the table is valid for this conf */
                if (conf->map[i] != NULL) {
                    count++;
                }
            }
        }

        *num_sid = count;
        rv = SHR_E_NONE;

    } while (0);

    return rv;
}
