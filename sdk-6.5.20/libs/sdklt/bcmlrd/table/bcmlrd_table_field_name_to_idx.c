/*! \file bcmlrd_table_field_name_to_idx.c
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
#include <bcmlrd/bcmlrd_table.h>
#include <bcmltd/bcmltd_internal.h>
#include <bcmltd/bcmltd_table.h>
#include <bcmltd/chip/bcmltd_limits.h>

/*******************************************************************************
 * Local definitions
 */

/* BSL Module */
#define BSL_LOG_MODULE BSL_LS_BCMLRD_TABLE

/*******************************************************************************
 * Public functions
 */

int
bcmlrd_table_field_name_to_idx(int unit,
                               const char *table_name,
                               const char *field_name,
                               bcmltd_sid_t *sid,
                               bcmltd_fid_t *fid)
{
    bcmltd_sid_t tbl_id;
    const bcmltd_table_rep_t *tbl;

    SHR_FUNC_ENTER(unit);

    if (sid == NULL) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (fid == NULL) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (table_name == NULL) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    tbl_id = bcmlrd_table_name_to_idx(unit, table_name);
    if (tbl_id >= BCMLTD_TABLE_COUNT) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    /* Get table information from idx. */
    tbl = bcmltd_table_get(tbl_id);
    if (tbl == NULL) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    /* Get field ID from field name. */
    SHR_IF_ERR_EXIT
        (bcmlrd_field_name_to_idx(unit, tbl_id, field_name, fid));

    *sid = tbl_id;

exit:
    SHR_FUNC_EXIT();
}

