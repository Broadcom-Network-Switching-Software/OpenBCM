/*! \file bcmltm_db_md_util.c
 *
 * Logical Table Manager - Utility functions.
 *
 * This file contains utility routines used to construct the LTM
 * database information.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_types.h>
#include <shr/shr_debug.h>
#include <bsl/bsl.h>

#include <bcmlrd/bcmlrd_types.h>

#include <bcmltm/bcmltm_util_internal.h>
#include <bcmltm/bcmltm_md_internal.h>
#include <bcmltm/bcmltm_wb_types_internal.h>
#include <bcmltm/bcmltm_wb_ptm_internal.h>
#include <bcmltm/bcmltm_db_core_internal.h>

#include "bcmltm_db_md_util.h"

/*******************************************************************************
 * Local definitions
 */

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMLTM_METADATA


/*******************************************************************************
 * Private functions
 */

/*******************************************************************************
 * Public functions
 */

void
bcmltm_db_field_maps_destroy(uint32_t num_fields,
                             bcmltm_field_map_t *field_map_list)
{
    uint32_t idx;
    bcmltm_field_map_t *field_map = field_map_list;
    bcmltm_field_map_t *next_map = NULL;

    if (field_map_list == NULL) {
        return;
    }

    for (idx = 0; idx < num_fields; idx++) {
        field_map = field_map_list[idx].next_map;
        while (field_map != NULL) {
            next_map = field_map->next_map;
            SHR_FREE(field_map);
            field_map = next_map;
        }
    }

    SHR_FREE(field_map_list);

    return;
}

void
bcmltm_db_field_map_list_destroy(bcmltm_field_map_list_t *field_list)
{
    if (field_list == NULL) {
        return;
    }

    bcmltm_db_field_maps_destroy(field_list->num_fields,
                                 field_list->field_maps);

    SHR_FREE(field_list);

    return;
}

void
bcmltm_db_field_mapping_destroy(bcmltm_field_select_mapping_t *field_mapping)
{
    uint32_t idx;

    if (field_mapping == NULL) {
        return;
    }

    for (idx = 0; idx < field_mapping->num_maps; idx++) {
        bcmltm_db_field_map_list_destroy(field_mapping->field_map_list[idx]);
    }

    SHR_FREE(field_mapping);

    return;
}

void
bcmltm_db_fixed_field_list_destroy(bcmltm_fixed_field_list_t *field_list)
{
    if (field_list == NULL) {
        return;
    }

    SHR_FREE(field_list->fixed_fields);
    SHR_FREE(field_list);

    return;
}

void
bcmltm_db_fixed_field_mapping_destroy(bcmltm_fixed_field_mapping_t *mapping)
{
    uint32_t idx;

    if (mapping == NULL) {
        return;
    }

    for (idx = 0; idx < mapping->num_maps; idx++) {
        bcmltm_db_fixed_field_list_destroy(mapping->fixed_field_list[idx]);
    }

    SHR_FREE(mapping);

    return;
}

void
bcmltm_db_lta_field_list_destroy(bcmltm_lta_field_list_t *list)
{
    if (list == NULL) {
        return;
    }

    SHR_FREE(list->field_maps);
    SHR_FREE(list);

    return;
}

void
bcmltm_db_lta_select_field_list_destroy(bcmltm_lta_select_field_list_t *list)
{
    if (list == NULL) {
        return;
    }

    SHR_FREE(list->lta_select_field_maps);
    SHR_FREE(list);

    return;
}

int
bcmltm_db_copy_lta_field_list_create(int unit,
                                     bcmlrd_sid_t sid,
                                     bcmltm_lta_field_list_t *from_list,
                                     bcmltm_lta_field_list_t *to_list,
                                     bcmltm_lta_field_list_t **copy_list_ptr)
{
    bcmltm_lta_field_list_t *copy_list = NULL;
    uint32_t copy_fields;
    unsigned int size;
    uint32_t from_ix, to_ix;
    bcmltm_lta_field_map_t *field_maps = NULL;
    bcmltm_lta_field_map_t *from_map, *to_map;
    bool is_read_only;

    SHR_FUNC_ENTER(unit);

    if (to_list->max_fields == 0) {
        /* Nothing to do */
        SHR_EXIT();
    }

    /* Allocate */
    SHR_ALLOC(copy_list, sizeof(*copy_list), "bcmltmCopyLtaFieldList");
    SHR_NULL_CHECK(copy_list, SHR_E_MEMORY);
    sal_memset(copy_list, 0, sizeof(*copy_list));

    /* Copying from one LTA field list to a different LTA field list. */

    /* Not applicable */
    copy_list->index_absent_offset = BCMLTM_WB_OFFSET_INVALID;
    copy_list->api_fields_parameter_offset = BCMLTM_WB_OFFSET_INVALID;

    /* Fill data */
    copy_list->fields_parameter_offset =
        from_list->fields_parameter_offset;
    copy_list->target_fields_parameter_offset =
        to_list->fields_parameter_offset;
    copy_list->max_fields = to_list->max_fields;

    /* Allocate field_map for all possible to fields */
    size = sizeof(*field_maps) * copy_list->max_fields;
    SHR_ALLOC(field_maps, size, "bcmltmCopyLtaFieldMaps");
    SHR_NULL_CHECK(field_maps, SHR_E_MEMORY);
    sal_memset(field_maps, 0, size);

    copy_fields = 0;

    for (from_ix = 0; from_ix < from_list->num_maps; from_ix++) {
        from_map = &(from_list->field_maps[from_ix]);
        for (to_ix = 0; to_ix < to_list->num_maps; to_ix++) {
            to_map = &(to_list->field_maps[to_ix]);
            if ((from_map->api_field_id == to_map->api_field_id)
                && (from_map->field_idx == to_map->field_idx)) {
                /* Check if room for the new field in copy list. */
                if (copy_fields == copy_list->max_fields) {
                    const char *table_name;

                    table_name = bcmltm_lt_table_sid_to_name(unit, sid);
                    LOG_ERROR(BSL_LOG_MODULE,
                              (BSL_META_U(unit,
                                          "LTM LTA Copy Map: current "
                                          "field count exceeds max count "
                                          "%s(ltid=%d) "
                                          "count=%d max_count=%d\n"),
                               table_name, sid,
                               copy_fields + 1, copy_list->max_fields));
                    SHR_ERR_EXIT(SHR_E_INTERNAL);
                }

                SHR_IF_ERR_EXIT
                    (bcmltm_db_field_is_read_only(unit, sid,
                                                  to_map->api_field_id,
                                                  &is_read_only));
                if (is_read_only) {
                    /* Do not copy Read Only fields. */
                    continue;
                }

                /* Matching field value, add to copy list */
                field_maps[copy_fields].api_field_id = to_map->api_field_id;
                field_maps[copy_fields].field_idx = to_map->field_idx;
                /* These are the only two structure members used by
                 * LTA to LTA field copy.
                 */

                copy_fields++;
                /* A given field should only appear once in the list. */
                break;
           }
        }
    }

    if (copy_fields > 0) {
        copy_list->field_maps = field_maps;
        copy_list->num_maps = copy_fields;

        *copy_list_ptr = copy_list;
    } else {
        SHR_FREE(field_maps);
        bcmltm_db_lta_field_list_destroy(copy_list);
        *copy_list_ptr = NULL;
    }

 exit:
    if (SHR_FUNC_ERR()) {
        SHR_FREE(field_maps);
        bcmltm_db_lta_field_list_destroy(copy_list);
        *copy_list_ptr = NULL;
    }

    SHR_FUNC_EXIT();
}

int
bcmltm_db_pt_param_offset_get(int unit,
                              const bcmltm_pt_list_t *pt_list,
                              bcmdrd_sid_t ptid,
                              uint32_t fid,
                              uint32_t *offset)
{
    uint32_t pt_view;
    uint32_t pt_op = 0;      /* For now, assume pt op is 0 */
    const bcmltm_pt_op_list_t *pt_op_list = NULL;
    bcmltm_wb_ptm_param_offsets_t param;

    SHR_FUNC_ENTER(unit);

    /* Find PT */
    for (pt_view = 0; pt_view < pt_list->num_pt_view; pt_view++) {
        if (pt_list->mem_args[pt_view]->pt == ptid) {
            pt_op_list = pt_list->memop_lists[pt_view];
            break;
        }
    }

    if (pt_op_list == NULL) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    /* Get PTM parameter offsets */
    (void)bcmltm_wb_ptm_param_offsets_get(pt_op_list->param_offsets[pt_op],
                                          &param);

    if (fid == BCMLRD_FIELD_INDEX) {
        *offset = param.index;
    } else if (fid == BCMLRD_FIELD_PORT) {
        *offset = param.port;
    } else if (fid == BCMLRD_FIELD_INSTANCE) {
        *offset = param.table_inst;
    } else if (fid == BCMLRD_FIELD_TABLE_SEL) {
        *offset = param.pt_sid_selector;
    } else if (fid == BCMLRD_FIELD_SELECTOR) {
        *offset = param.subentry;
    }

 exit:
    SHR_FUNC_EXIT();
}

int
bcmltm_db_field_properties_get(int unit,
                               bcmlrd_sid_t sid,
                               bcmlrd_fid_t fid,
                               uint32_t *flags)
{
    bool is_key = FALSE;
    bool is_read_only = FALSE;

    SHR_FUNC_ENTER(unit);

    *flags = 0x0;

    SHR_IF_ERR_EXIT
        (bcmltm_db_field_is_key(unit, sid, fid, &is_key));
    SHR_IF_ERR_EXIT
        (bcmltm_db_field_is_read_only(unit, sid, fid, &is_read_only));

    if (is_key) {
        *flags |= BCMLTM_FIELD_KEY;
    }

    if (is_read_only) {
        *flags |= BCMLTM_FIELD_READ_ONLY;
    }

 exit:
    SHR_FUNC_EXIT();
}
