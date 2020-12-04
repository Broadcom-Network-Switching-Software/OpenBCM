/*! \file bcmltd_field_depth_get.c
 *
 * Return the depth of the field.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

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
bcmltd_field_depth_get(bcmltd_sid_t sid,
                       bcmltd_fid_t fid,
                       uint32_t *depth)
{
    const bcmltd_table_rep_t *tbl;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    if (depth == NULL) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    tbl = bcmltd_table_get(sid);
    if (tbl == NULL) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    if (fid >= tbl->fields) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    *depth = (tbl->field[fid].depth) ? tbl->field[fid].depth : 1;

exit:
    SHR_FUNC_EXIT();
}
