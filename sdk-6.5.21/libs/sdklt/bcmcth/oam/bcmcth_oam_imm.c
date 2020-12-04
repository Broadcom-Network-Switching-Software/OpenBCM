/*! \file bcmcth_oam_imm.c
 *
 * BCMCTH OAM IMM handler.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <shr/shr_debug.h>
#include <sal/sal_libc.h>

#include <bcmlrd/bcmlrd_types.h>
#include <bcmlrd/bcmlrd_map.h>
#include <bcmcth/bcmcth_oam_imm.h>
#include <bcmcth/bcmcth_oam_bfd.h>

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMCTH_OAM

/*******************************************************************************
 * Private Functions
 */

/* Register callbacks for the OAM LTs. */
static int
imm_register(int unit, bool warm)
{
    SHR_FUNC_ENTER(unit);

    /* Register callbacks for BFD LTs. */
    SHR_IF_ERR_EXIT(bcmcth_oam_bfd_imm_register(unit, warm));

exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public Functions
 */

bool
bcmcth_oam_imm_mapped(int unit, const bcmlrd_sid_t sid)
{
    int rv;
    const bcmlrd_map_t *map = NULL;

    rv = bcmlrd_map_get(unit, sid, &map);
    if (SHR_SUCCESS(rv) &&
        map &&
        map->group &&
        map->group[0].dest.kind == BCMLRD_MAP_CUSTOM) {
        return TRUE;
    }
    return FALSE;
}

int
bcmcth_oam_imm_init(int unit, bool warm)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT(imm_register(unit, warm));

exit:
    SHR_FUNC_EXIT();
}

