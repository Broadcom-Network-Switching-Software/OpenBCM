/*! \file bcmmgmt_issu.c
 *
 * SDK ISSU start API.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <bcmissu/issu_api.h>
#include <bcmmgmt/bcmmgmt.h>

#define BSL_LOG_MODULE BSL_LS_BCMMGMT_INIT

static bool issu_start = false;

int
bcmmgmt_issu_start(bool warm, const bcmmgmt_issu_info_t *issu_info)
{
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    if (issu_start) {
        SHR_IF_ERR_EXIT
            (bcmmgmt_issu_done());
    }

    if (warm && issu_info) {
        SHR_IF_ERR_EXIT
            (bcmissu_upgrade_start(issu_info->start_ver,
                                   issu_info->current_ver));
        issu_start = true;
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmmgmt_issu_done(void)
{
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    if (issu_start) {
        SHR_IF_ERR_EXIT
            (bcmissu_upgrade_done());
        issu_start = false;
    }

exit:
    SHR_FUNC_EXIT();
}
