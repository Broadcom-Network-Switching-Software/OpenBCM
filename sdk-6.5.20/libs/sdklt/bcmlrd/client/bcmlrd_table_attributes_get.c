/*! \file bcmlrd_table_attributes_get.c
 *
 * Get the attributes for the given table.
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
#include <bcmlrd/bcmlrd_table.h>
#include <bcmlrd/bcmlrd_map.h>
#include <bcmlrd/bcmlrd_internal.h>

int
bcmlrd_table_attributes_get(int unit,
                            const char *table_name,
                            bcmlrd_table_attrib_t *attrib)
{
    int rv = SHR_E_PARAM;
    const bcmlrd_map_conf_rep_t *conf;
    const bcmlrd_table_rep_t *tbl;
    uint32_t size;
    uint32_t flags = 0;
    int idx;

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

        if (attrib == NULL) {
            break;
        }

        idx = bcmlrd_table_name_to_idx(unit, table_name);
        if (SHR_FAILURE(idx)) {
            rv = idx;
            break;
        }

        tbl = bcmltd_table_get(idx);

        if (tbl == NULL) {
            rv = SHR_E_INTERNAL;
            break;
        }

        if (tbl->flags == BCMLRD_TABLE_F_TYPE_MAPPED) {
            /* For tables of type mapped, get type from table attributes. */
            rv = bcmlrd_map_table_attr_get(unit, idx,
                       BCMLRD_MAP_TABLE_ATTRIBUTE_TABLE_TYPE,
                       &flags);
            if (SHR_FAILURE(rv)) {
                break;
            }
        } else {
            flags = tbl->flags;
        }

        attrib->id = idx;
        if (flags & BCMLRD_TABLE_F_TYPE_TCAM) {
            attrib->type = TCAM;
        } else if  (flags & BCMLRD_TABLE_F_TYPE_HASH) {
            attrib->type = HASH;
        } else {
            attrib->type = INDEX;
        }

        rv = bcmlrd_table_size_get(unit, idx, &size);
        if (SHR_FAILURE(rv)) {
            size = 0;
        }
        attrib->max_entries = size;

        attrib->access_type = (tbl->flags & BCMLRD_TABLE_F_READ_ONLY) ?
            READ_ONLY : READ_WRITE;

        rv = bcmlrd_table_interactive_get(unit,
                                          idx,
                                          &attrib->interactive);
        if (SHR_FAILURE(rv)) {
            break;
        }

        rv = bcmlrd_field_count_get(unit,idx,&attrib->number_of_fields);

    } while (0);

    return rv;
}

