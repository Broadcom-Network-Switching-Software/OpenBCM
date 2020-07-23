/*! \file bcmlrd_table_match_id_db_get.c
 *
 * Get the match id information for given table and field.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_libc.h>
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bsl/bsl.h>
#include <bcmltd/bcmltd_table.h>
#include <bcmlrd/bcmlrd_client.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmlrd/bcmlrd_map.h>
#include <bcmlrd/bcmlrd_internal.h>

/*******************************************************************************
 * Local definitions
 */

/* BSL Module */
#define BSL_LOG_MODULE BSL_LS_BCMLRD_TABLE

/*******************************************************************************
 * Public functions
 */

int
bcmlrd_table_match_id_db_get(int unit,
                             const bcmlrd_sid_t sid,
                             const bcmlrd_fid_t fid,
                             const bcmlrd_match_id_db_t **info)
{
    bcmlrd_match_id_db_get_t func;
    int rv;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(info, SHR_E_PARAM);

    func = bcmlrd_unit_match_id_db_ptr_get(unit);
    if (func == NULL) {
        /* Should have a configuration for this unit */
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Unit PCM configuration not available")));
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    rv = func(unit, sid, fid, info);
    if (rv != SHR_E_NONE) {
        SHR_ERR_EXIT(rv);
    }

exit:
    SHR_FUNC_EXIT();
}

