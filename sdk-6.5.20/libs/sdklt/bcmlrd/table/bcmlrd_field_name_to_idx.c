/*! \file bcmlrd_field_name_to_idx.c
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
bcmlrd_field_name_to_idx(int unit,
                         bcmltd_sid_t sid,
                         const char *field_name,
                         bcmltd_fid_t *fid)
{
    const bcmltd_table_rep_t *tbl = NULL;
    bcmltd_fid_t fld_id;

    SHR_FUNC_ENTER(unit);

    if (fid == NULL) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (field_name == NULL) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    tbl = bcmltd_table_get(sid);
    if (tbl == NULL) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    for (fld_id = 0; fld_id < tbl->fields; fld_id++) {
        if (!sal_strcmp(field_name, tbl->field[fld_id].name)) {
            /* Field found. */
            break;
        }
    }

    if (fld_id == tbl->fields) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    *fid = fld_id;

 exit:
    SHR_FUNC_EXIT();
}
