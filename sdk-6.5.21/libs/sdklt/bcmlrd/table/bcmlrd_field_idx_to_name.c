/*! \file bcmlrd_field_idx_to_name.c
 *
 * Return the field name for a table and field ID.
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
bcmlrd_field_idx_to_name(int unit,
                         bcmltd_sid_t sid,
                         bcmltd_fid_t fid,
                         const char **field_name)
{
    const bcmltd_table_rep_t *tbl;

    SHR_FUNC_ENTER(unit);

    if (field_name == NULL) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    tbl = bcmltd_table_get(sid);
    if (tbl == NULL) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    if (fid > tbl->fields) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    *field_name = tbl->field[fid].name;

 exit:
    SHR_FUNC_EXIT();
}
