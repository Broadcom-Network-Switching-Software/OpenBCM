/*! \file bcmltm_id_name.c
 *
 * Logical Table Manager Utility Interfaces.
 *
 * This file contains low level utility routines to translate
 * a table or field ID to its corresponding string name.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_types.h>

#include <bcmdrd/bcmdrd_types.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmlrd/bcmlrd_types.h>
#include <bcmlrd/bcmlrd_table.h>

#include <bcmltm/bcmltm_pt.h>
#include <bcmltm/bcmltm_util_internal.h>

const char *
bcmltm_pt_table_sid_to_name(int unit,
                            bcmdrd_sid_t sid)
{
    const char *name = NULL;

    name = bcmdrd_pt_sid_to_name(unit, sid);
    if (name == NULL) {
        name = "UNKNOWN";
    }

    return  name;
}

const char *
bcmltm_lt_table_sid_to_name(int unit,
                            bcmlrd_sid_t sid)
{
    const bcmlrd_table_rep_t *tbl;
    const char *name = NULL;

    COMPILER_REFERENCE(unit);

    tbl = bcmlrd_table_get(sid);
    if (tbl != NULL) {
        name = tbl->name;
    }

    if (name == NULL) {
        name = "UNKNOWN";
    }

    return name;
}

const char *
bcmltm_table_sid_to_name(int unit,
                         uint32_t sid,
                         bool logical)
{
    const char *name = NULL;

    if (logical) {
        name = bcmltm_lt_table_sid_to_name(unit, sid);
    } else {
        name = bcmltm_pt_table_sid_to_name(unit, sid);
    }

    return name;
}

const char *
bcmltm_pt_field_fid_to_name(int unit,
                            bcmdrd_sid_t sid,
                            bcmdrd_fid_t fid)
{
    int rv;
    bcmdrd_sym_field_info_t info;
    const char *name = NULL;

    if (fid == BCMLTM_PT_FIELD_KEY_INDEX) {
        name = BCMLTM_PT_FIELD_KEY_INDEX_NAME;
    } else if (fid == BCMLTM_PT_FIELD_KEY_PORT) {
        name = BCMLTM_PT_FIELD_KEY_PORT_NAME;
    } else if (fid == BCMLTM_PT_FIELD_KEY_INSTANCE) {
        name = BCMLTM_PT_FIELD_KEY_INSTANCE_NAME;
    } else {
        rv = bcmdrd_pt_field_info_get(unit, sid, fid, &info);
        if (SHR_SUCCESS(rv)) {
            name = info.name;
        }
    }

    if (name == NULL) {
        name = "UNKNOWN";
    }

    return name;
}

const char *
bcmltm_lt_field_fid_to_name(int unit,
                            bcmlrd_sid_t sid,
                            bcmlrd_fid_t fid)

{
    const bcmlrd_table_rep_t *tbl;
    const char *name = NULL;

    tbl = bcmlrd_table_get(sid);
    if ((tbl != NULL) && (fid < tbl->fields )) {
        name = tbl->field[fid].name;
    }

    if (name == NULL) {
        name = "UNKNOWN";
    }

    return name;
}

const char *
bcmltm_field_fid_to_name(int unit,
                         uint32_t sid,
                         uint32_t fid,
                         bool logical)
{
    const char *name = NULL;

    if (logical) {
        name = bcmltm_lt_field_fid_to_name(unit, sid, fid);
    } else {
        name = bcmltm_pt_field_fid_to_name(unit, sid, fid);
    }

    return name;
}
