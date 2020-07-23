/*! \file bcmlrd_mapped_dest_get.c
 *
 * Get the map destinations for the given mapped table SID.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_error.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmlrd/bcmlrd_map.h>
#include <bcmlrd/bcmlrd_internal.h>

int
bcmlrd_mapped_dest_get(int unit,
                       bcmlrd_sid_t sid,
                       size_t count,
                       bcmlrd_table_desc_t *dst,
                       size_t *actual)
{
    int rv = SHR_E_PARAM;
    const bcmlrd_map_t *sid_map;
    size_t i;

    do {
        if (dst == NULL) {
            break;
        }

        rv = bcmlrd_map_get(unit, sid, &sid_map);

        if (!SHR_SUCCESS(rv)) {
            break;
        }

        for (i=0; i<sid_map->groups; i++) {
            if (i >= count) {
                break;
            }
            *dst++ = sid_map->group[i].dest;
        }

        if (actual) {
            *actual = i;
        }

    } while (0);

    return rv;
}

