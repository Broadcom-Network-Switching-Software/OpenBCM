/*! \file bcmlrd_port_size_get.c
 *
 * Get the maximum number of ports for the given table SID.
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
bcmlrd_port_size_get(int unit, bcmlrd_sid_t sid, uint32_t *size)
{
    int rv;
    uint32_t min_index;
    uint32_t max_index;

    do {

        rv = bcmlrd_map_table_attr_get(unit, sid,
                   BCMLRD_MAP_TABLE_ATTRIBUTE_PORT_SIZE,
                   size);
        if (rv != SHR_E_UNAVAIL) {
            break;
        }

        rv = bcmlrd_port_max_index_get(unit, sid, &max_index);
        if (SHR_FAILURE(rv)) {
            break;
        }

        rv = bcmlrd_port_min_index_get(unit, sid, &min_index);
        if (SHR_FAILURE(rv)) {
            break;
        }

        *size = max_index - min_index + 1;

    } while(0);

    return rv;
}

