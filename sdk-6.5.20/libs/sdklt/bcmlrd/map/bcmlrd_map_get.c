/*! \file bcmlrd_map_get.c
 *
 * Get the map for the given table SID.
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
bcmlrd_map_get(int unit,
               bcmlrd_sid_t sid,
               bcmlrd_map_t const **map)
{
    const bcmlrd_map_conf_rep_t *conf;
    int rv = SHR_E_PARAM;
    const bcmlrd_map_t *sid_map;

    do {
        if (map == NULL) {
            /* map should be non-NULL */
            break;
        }

        conf = bcmlrd_unit_conf_get(unit);
        if (conf == NULL) {
            /* Should have a configuration for this unit */
            rv = SHR_E_UNAVAIL;
            break;
        }

        if (sid >= BCMLRD_TABLE_COUNT) {
            rv = SHR_E_UNAVAIL;
            /* SID needs to be valid */
            break;
        }

        sid_map = conf->map[sid];
        if (sid_map == NULL) {
            /* Should have a valid map for this configuration */
            rv = SHR_E_UNAVAIL;
            break;
        }

        *map = sid_map;

        rv = SHR_E_NONE;

    } while (0);

    return rv;
}

