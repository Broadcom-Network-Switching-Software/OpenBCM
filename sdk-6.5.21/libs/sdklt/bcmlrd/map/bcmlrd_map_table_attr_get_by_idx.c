/*! \file bcmlrd_map_table_attr_get_by_idx.c
 *
 * Get value of given map table attribute value and index.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bcmlrd/bcmlrd_map.h>

/*******************************************************************************
 * Local definitions
 */

/* BSL Module */
#define BSL_LOG_MODULE BSL_LS_BCMLRD_TABLE

/*******************************************************************************
 * Public functions
 */

int
bcmlrd_map_table_attr_get_by_idx(int unit,
                                 bcmlrd_sid_t sid,
                                 bcmlrd_map_table_attr_name_t name,
                                 uint32_t idx,
                                 uint32_t *value)
{
    const bcmlrd_map_t *map;
    const bcmlrd_map_table_attr_t *attr;
    size_t i;
    size_t count;
    bool found = FALSE;

    SHR_FUNC_ENTER(unit);

    if (value == NULL) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_EXIT(bcmlrd_map_get(unit, sid, &map));

    if (map->table_attr == NULL) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    for (i = count = 0; i < map->table_attr->attributes; i++) {
        attr = &map->table_attr->attr[i];
        if (attr->key == name) {
            count++;
            if (count == (idx + 1)) {
                /* Attribute name and index match */
                *value = attr->value;
                found = TRUE;
                break;
            }
        }
    }

    if (!found) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

 exit:
    SHR_FUNC_EXIT();
}
