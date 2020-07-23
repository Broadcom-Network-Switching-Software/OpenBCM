/*! \file bcmlrd_map_table_attr_get.c
 *
 * Get an attribute for a table.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_error.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmlrd/bcmlrd_map.h>

int
bcmlrd_map_table_attr_get(int unit,
                          bcmlrd_sid_t sid,
                          bcmlrd_map_table_attr_name_t name,
                          uint32_t *value)
{
    int rv = SHR_E_PARAM;
    uint32_t i;
    const bcmlrd_map_t *map;
    const bcmlrd_map_table_attr_t *attr;

    do {

        if (value == NULL) {
            /* value should be non-NULL */
            break;
        }

        if (sid >= BCMLRD_TABLE_COUNT) {
            rv = SHR_E_UNAVAIL;
            /* SID needs to be valid */
            break;
        }

        rv = bcmlrd_map_get(unit, sid, &map);
        if (SHR_FAILURE(rv)) {
            break;
        }

        rv = SHR_E_UNAVAIL;
        if (map->table_attr == NULL) {
            break;
        }

        for (i = 0; i < map->table_attr->attributes; ++i) {
            attr = &map->table_attr->attr[i];
            if (attr->key == name) {
                *value = attr->value;
                rv = SHR_E_NONE;
                break;
            }
        }

    } while (0);

    return rv;
}

