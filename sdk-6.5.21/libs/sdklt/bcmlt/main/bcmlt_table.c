/*! \file bcmlt_table.c
 *
 * Handle table functionality
 *
 * This file contains the implementation of all the table level functionality.
 * In particular functions like subscribe to table events and table entries
 * enumeration.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <shr/shr_lmem_mgr.h>
#include <bcmlrd/bcmlrd_map.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmlrd/bcmlrd_client.h>
#include <bcmltd/bcmltd_table.h>
#include <bcmlt/bcmlt.h>
#include "bcmlt_internal.h"

#define BSL_LOG_MODULE BSL_LS_BCMLT_TABLE

int bcmlt_table_names_get_first(int unit,
                                uint32_t flags,
                                char **name)
{
    SHR_FUNC_ENTER(unit);
    UNIT_VALIDATION(unit);
    if (!name) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_db_table_name_get_first(unit, flags, name));
exit:
    SHR_FUNC_EXIT();
}

int bcmlt_table_names_get_next(int unit,
                               uint32_t flags,
                               char **name)
{
    SHR_FUNC_ENTER(unit);
    UNIT_VALIDATION(unit);
    if (!name) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_db_table_name_get_next(unit, flags, name));
exit:
    SHR_FUNC_EXIT();
}

int bcmlt_table_field_defs_get(int unit,
                               const char *table_name,
                               uint32_t    fields_array_size,
                               bcmlt_field_def_t *field_defs_array,
                               uint32_t *require_array_size)
{
    SHR_FUNC_ENTER(unit);
    UNIT_VALIDATION(unit);
    if (!table_name) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    SHR_IF_ERR_EXIT(bcmlt_db_table_field_defs_get(unit,
                                       table_name,
                                       fields_array_size,
                                       field_defs_array,
                                       require_array_size));
exit:
    SHR_FUNC_EXIT();
}

int bcmlt_table_subscribe(int unit,
                          const char *table_name,
                          bcmlt_table_sub_cb callback,
                          void *user_data)
{
    bcmlt_table_attrib_t *tbl_attr;
    void *tbl_hdl;

    SHR_FUNC_ENTER(unit);
    UNIT_VALIDATION(unit);
    if (!table_name || !callback) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    /* Obtain the table ID */
    SHR_IF_ERR_EXIT(bcmlt_db_table_info_get(unit,
                                            table_name,
                                            &tbl_attr,
                                            NULL,
                                            &tbl_hdl));

    if (tbl_attr->pt) { /* Not events for PT tables */
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_EXIT(bcmtrm_table_event_subscribe(unit,
                                        tbl_attr->table_id,
                                        callback,
                                        user_data));
exit:
    SHR_FUNC_EXIT();
}

int bcmlt_table_unsubscribe(int unit, const char *table_name)
{
    bcmlt_table_attrib_t *tbl_attr;
    void *tbl_hdl;

    SHR_FUNC_ENTER(unit);
    UNIT_VALIDATION(unit);
    if (!table_name) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    /* Obtain the table ID */
    SHR_IF_ERR_EXIT(bcmlt_db_table_info_get(unit,
                                            table_name,
                                            &tbl_attr,
                                            NULL,
                                            &tbl_hdl));

    if (tbl_attr->pt) { /* Not events for PT tables */
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* Register NULL callback */
    SHR_IF_ERR_EXIT(bcmtrm_table_event_subscribe(unit,
                                        tbl_attr->table_id,
                                        NULL,
                                        0));
exit:
    SHR_FUNC_EXIT();
}

int
bcmlt_table_pt_name_max_num_get(int unit,
                                const char *tbl_name,
                                uint32_t *pt_max_num)
{
    bcmlt_table_attrib_t *table_attr;
    void *hdl;
    shr_lmm_hdl_t fld_array_hdl;

    SHR_FUNC_ENTER(unit);
    UNIT_VALIDATION(unit);

    if (!tbl_name || !pt_max_num) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* Find the table ID associated with the name */
    SHR_IF_ERR_EXIT
        (bcmlt_db_table_info_get(unit, tbl_name, &table_attr,
                                 &fld_array_hdl, &hdl));

    SHR_IF_ERR_EXIT
        (bcmlrd_table_pt_list_num_get(unit, table_attr->table_id, pt_max_num));

exit:
    SHR_FUNC_EXIT();
}

int
bcmlt_table_pt_name_get(int unit,
                        const char *tbl_name,
                        uint32_t pt_count,
                        const char **pt_names,
                        uint32_t *actual_pt_count)
{
    bcmlt_table_attrib_t *table_attr;
    void *hdl;
    shr_lmm_hdl_t fld_array_hdl;
    uint32_t idx;
    const char **pt_name_list = pt_names;
    bcmdrd_sid_t *ptid_list = NULL;
    uint32_t size = sizeof(bcmdrd_sid_t) * pt_count;
    uint32_t actual_count;

    SHR_FUNC_ENTER(unit);
    UNIT_VALIDATION(unit);

    if (!tbl_name || !size || !pt_names) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* Find the table ID associated with the name */
    SHR_IF_ERR_EXIT
        (bcmlt_db_table_info_get(unit, tbl_name, &table_attr,
                                 &fld_array_hdl, &hdl));

    SHR_ALLOC(ptid_list, size, "bcmltPtidList");
    SHR_NULL_CHECK(ptid_list, SHR_E_MEMORY);
    sal_memset(ptid_list, 0x0, size);

    SHR_IF_ERR_EXIT
        (bcmlrd_table_pt_list_get(unit, table_attr->table_id,
                                  pt_count, ptid_list, &actual_count));
    for (idx = 0; idx < actual_count; idx++) {
        pt_name_list[idx] =
            (char *)bcmdrd_pt_sid_to_name(unit, ptid_list[idx]);
    }
    if (actual_pt_count) {
        *actual_pt_count = actual_count;
    }

exit:
    SHR_FREE(ptid_list);

    SHR_FUNC_EXIT();
}

int bcmlt_table_desc_get(int unit,
                         const char *tbl_name,
                         const char **tbl_desc)
{
    int rv;
    const bcmltd_table_rep_t *lt_info;
    void *hdl;
    bcmlt_table_attrib_t *table_attr;

    SHR_FUNC_ENTER(unit);

    /* Sanity checks. */
    UNIT_VALIDATION(unit);
    if (!tbl_name || !tbl_desc) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    rv = bcmlt_db_table_info_get(unit, tbl_name, &table_attr, NULL, &hdl);

    /* Return if table is not found. */
    if (rv != SHR_E_NONE) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "Table %s was not found\n"),
                     tbl_name));
        SHR_ERR_EXIT(rv);
    }

    lt_info = bcmltd_table_get(table_attr->table_id);
    if (lt_info == NULL) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    *tbl_desc = lt_info->desc;

exit:
    SHR_FUNC_EXIT();
}
