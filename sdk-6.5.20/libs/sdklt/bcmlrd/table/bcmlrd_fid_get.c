/*! \file bcmlrd_fid_get.c
 *
 * Return the local field ID given table ID and global field ID.
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
#include <bcmlrd/bcmlrd_enum.h>
#include <bcmlrd/bcmlrd_table.h>

/*******************************************************************************
 * Local definitions
 */

/* BSL Module */
#define BSL_LOG_MODULE BSL_LS_BCMLRD_TABLE


/*******************************************************************************
 * Public functions
 */

int
bcmlrd_fid_get(int unit,
               bcmltd_sid_t sid,
               bcmltd_gfid_t gfid,
               bcmltd_fid_t *fid)
{
    const char *field_name;

    SHR_FUNC_ENTER(unit);

    if (fid == NULL) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_EXIT
        (bcmlrd_enum_scalar_to_symbol_by_type(unit, "LFID_T", gfid, &field_name));

    SHR_IF_ERR_EXIT
        (bcmlrd_field_name_to_idx(unit, sid, field_name, fid));

 exit:
    SHR_FUNC_EXIT();
}
