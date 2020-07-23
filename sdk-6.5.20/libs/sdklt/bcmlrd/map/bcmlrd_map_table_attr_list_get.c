/*! \file bcmlrd_map_table_attr_list_get.c
 *
 * Get list of values for a map table attribute.
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
bcmlrd_map_table_attr_list_get(int unit,
                               bcmlrd_sid_t sid,
                               bcmlrd_map_table_attr_name_t name,
                               size_t list_max,
                               uint32_t *value_list,
                               size_t *num_value)
{
    const bcmlrd_map_t *map;
    const bcmlrd_map_table_attr_t *attr;
    size_t i;
    size_t count;

    SHR_FUNC_ENTER(unit);

    if (value_list == NULL) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_EXIT(bcmlrd_map_get(unit, sid, &map));

    if (map->table_attr == NULL) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    for (i = count = 0; i < map->table_attr->attributes; i++) {
        if (count >= list_max) {
            break;
        }

        attr = &map->table_attr->attr[i];
        if (attr->key == name) {
            value_list[count] = attr->value;
            count++;
        }
    }

    if (num_value) {
        *num_value = count;
    }

 exit:
    SHR_FUNC_EXIT();
}



