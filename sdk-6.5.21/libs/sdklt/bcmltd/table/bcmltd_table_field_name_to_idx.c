/*! \file bcmltd_table_field_name_to_idx.c
 *
 * Return the index of the given table name.
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
#include <bcmltd/chip/bcmltd_limits.h>

/*******************************************************************************
 * Local definitions
 */

/* BSL Module */
#define BSL_LOG_MODULE BSL_LS_BCMLTD_TABLE

/*******************************************************************************
 * Public functions
 */

/* Binary search table to return index */
int
bcmltd_table_field_name_to_idx(int unit,
                               const char *table_name,
                               const char *field_name,
                               bcmltd_sid_t *sid,
                               bcmltd_fid_t *fid)
{
    bcmltd_sid_t tbl_id;
    bcmltd_fid_t fld_id;
    const bcmltd_table_rep_t *tbl;
    const bcmltd_field_rep_t *fld;

    SHR_FUNC_ENTER(unit);

    tbl_id = bcmltd_table_name_to_idx(table_name);
    if (tbl_id >= BCMLTD_TABLE_COUNT) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    /* Get table information from idx. */
    tbl = bcmltd_table_get(tbl_id);
    if (tbl == NULL) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    /* Get field ID from field name. */
    for (fld_id = 0; fld_id < tbl->fields; fld_id++) {
        fld = &tbl->field[fld_id];
        if (!sal_strcmp(field_name, fld->name)) {
            /* Field found. */
            break;
        }
    }
    if (fld_id == tbl->fields) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    *sid = tbl_id;
    *fid = fld_id;

exit:
    SHR_FUNC_EXIT();
}

