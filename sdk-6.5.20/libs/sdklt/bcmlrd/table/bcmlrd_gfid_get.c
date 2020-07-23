/*! \file bcmlrd_gfid_get.c
 *
 * Return the global field ID given table ID and local field ID.
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
bcmlrd_gfid_get(int unit,
                bcmltd_sid_t sid,
                bcmltd_fid_t fid,
                bcmltd_gfid_t *gfid)
{
    const char *field_name;
    uint64_t gfid_64;

    SHR_FUNC_ENTER(unit);

    if (gfid == NULL) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_EXIT
        (bcmlrd_field_idx_to_name(unit, sid, fid, &field_name));

    SHR_IF_ERR_EXIT
        (bcmlrd_enum_symbol_to_scalar_by_type(unit,
                                              "LFID_T",
                                              field_name,
                                              &gfid_64));

    if (gfid_64 > 0xffffffff) {
        /* LTL enums are expected to be constrained to 32 bits. */
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    *gfid = (bcmltd_gfid_t)gfid_64;

 exit:
    SHR_FUNC_EXIT();
}
