/*! \file bcmltd_field_name_to_idx.c
 *
 * Return the index of the given field name.
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
#include <bcmltd/bcmltd_internal.h>
#include <bcmltd/bcmltd_table.h>

/*******************************************************************************
 * Local definitions
 */

/* BSL Module */
#define BSL_LOG_MODULE BSL_LS_BCMLTD_TABLE

/*******************************************************************************
 * Public functions
 */

int
bcmltd_field_name_to_idx(bcmltd_sid_t sid,
                         const char *field_name,
                         bcmltd_fid_t *fid)
{
    bcmltd_fid_t fld_id;
    const bcmltd_table_rep_t *tbl;
    const bcmltd_field_rep_t *fld;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    /* Get table information from idx */
    tbl = bcmltd_table_get(sid);
    if (tbl == NULL) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    /* Get field ID from field name */
    for (fld_id = 0; fld_id < tbl->fields; fld_id++) {
        fld = &tbl->field[fld_id];
        if (!sal_strcmp(field_name, fld->name)) {
            /* Field found */
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
