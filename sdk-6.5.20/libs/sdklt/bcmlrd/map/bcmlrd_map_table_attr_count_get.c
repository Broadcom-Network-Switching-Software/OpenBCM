/*! \file bcmlrd_map_table_attr_count_get.c
 *
 * Get count for a map table attribute.
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
bcmlrd_map_table_attr_count_get(int unit,
                                bcmlrd_sid_t sid,
                                bcmlrd_map_table_attr_name_t name,
                                size_t *count)
{
    const bcmlrd_map_t *map;
    const bcmlrd_map_table_attr_t *attr;
    size_t i;

    SHR_FUNC_ENTER(unit);

    if (count == NULL) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_EXIT(bcmlrd_map_get(unit, sid, &map));

    if (map->table_attr == NULL) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    *count = 0;
    for (i = 0; i < map->table_attr->attributes; i++) {
        attr = &map->table_attr->attr[i];
        if (attr->key == name) {
            (*count)++;
        }
    }

 exit:
    SHR_FUNC_EXIT();
}
