/*! \file bcmlrd_table_name_to_idx.c
 *
 * Return the table index for the given table name.
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
#include <bcmlrd/bcmlrd_conf.h>

/*******************************************************************************
 * Public functions
 */

int
bcmlrd_table_name_to_idx(int unit, const char *table_name)
{
    bcmlrd_variant_t base;
    bcmlrd_variant_t variant;
    int idx = SHR_E_UNAVAIL;

    if (table_name == NULL) {
        return SHR_E_PARAM;
    }

    base = bcmlrd_base_get(unit);
    variant = bcmlrd_variant_get(unit);
    if (base == variant) {
        /* For base tables, continue to use the BCMLTD converter. This
           will be combined with the variant search once the compact
           map representation is in place. */
        idx = bcmltd_table_name_to_idx(table_name);
    } else {
        const bcmlrd_map_conf_rep_t *conf;
        const bcmltd_table_rep_t *tbl;
        int rv;

        rv = bcmlrd_conf_get(unit, &conf);
        if (SHR_SUCCESS(rv)) {
            int idx_var;
            /* Painful linear search for now until the compact map
               representation. */
            for (idx_var = 0; idx_var < BCMLRD_TABLE_COUNT; idx_var++) {
                if (conf->map[idx_var] == NULL) {
                    continue;
                }
                tbl = bcmltd_table_get(conf->map[idx_var]->src_id);
                if (tbl == NULL) {
                    continue;
                }
                if (!sal_strcmp(tbl->name, table_name)) {
                    idx = idx_var;
                    break;
                }
            }
        }
    }

    return idx;
}
