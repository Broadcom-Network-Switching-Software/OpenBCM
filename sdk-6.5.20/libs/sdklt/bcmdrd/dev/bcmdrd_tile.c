/*! \file bcmdrd_tile.c
 *
 * Functions to handle flex TILEs.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_error.h>

#include <bcmdrd/bcmdrd_dev.h>
#include <bcmdrd/bcmdrd_tile.h>

static const bcmdrd_tile_mux_info_db_t **mux_info_list[BCMDRD_CONFIG_MAX_UNITS];

void
bcmdrd_tile_info_update(int unit,
                        const bcmdrd_tile_mux_info_db_t **mux_info)
{
    mux_info_list[unit] = mux_info;
    return;
}

int
bcmdrd_tile_mux_info_db_get(int unit,
                            uint32_t tile_id,
                            const bcmdrd_tile_mux_info_db_t **mux_info_db)
{
    const bcmdrd_tile_mux_info_db_t **db_list = mux_info_list[unit];

    if ((db_list == NULL) || (tile_id == BCMDRD_INVALID_TILE)) {
        return SHR_E_UNAVAIL;
    }

    *mux_info_db = mux_info_list[unit][tile_id];
    return SHR_E_NONE;
}
