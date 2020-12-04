/*! \file bcmlrd_port_max_index_get.c
 *
 * Get the maximum port for the given table.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_types.h>
#include <shr/shr_error.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmlrd/bcmlrd_map.h>

int
bcmlrd_port_max_index_get(int unit, bcmlrd_sid_t sid, uint32_t *max_index)
{
    return bcmlrd_map_table_attr_get(unit, sid,
               BCMLRD_MAP_TABLE_ATTRIBUTE_PORT_MAX_INDEX,
               max_index);
}

