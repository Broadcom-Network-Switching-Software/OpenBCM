/*! \file bcmlrd_table_interactive_get.c
 *
 * Get the interactive attribute for the given table.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_error.h>
#include <bcmlrd/bcmlrd_types.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmlrd/bcmlrd_map.h>

/*******************************************************************************
 * Local definitions
 */

/*******************************************************************************
 * Private functions
 */

/*******************************************************************************
 * Public functions
 */

int
bcmlrd_table_interactive_get(int unit, uint32_t sid, bool *interactive)
{
    int rv;
    size_t i;
    bcmlrd_map_t const *map;
    const bcmlrd_map_table_attr_t *tbl_attr = NULL;
    uint32_t num_attrs;
    bool flag = false;

    do {

        if (interactive == NULL) {
            rv = SHR_E_PARAM;
            break;
        }

        rv = bcmlrd_map_get(unit, sid, &map);
        if (SHR_FAILURE(rv)) {
            break;
        }

        num_attrs = map->table_attr->attributes;
        for (i = 0; i < num_attrs; i++) {
            tbl_attr = &map->table_attr->attr[i];
            if (tbl_attr->key == BCMLRD_MAP_TABLE_ATTRIBUTE_INTERACTIVE) {
                if (tbl_attr->value == TRUE) {
                    flag = true;
                } else {
                    flag = false;
                }
                break;
            }
        }

        /* Set flag in caller. */
        *interactive = flag;

    } while (0);

    return rv;
}
