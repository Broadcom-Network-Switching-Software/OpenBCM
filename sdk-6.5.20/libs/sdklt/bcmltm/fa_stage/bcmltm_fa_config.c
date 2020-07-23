/*! \file bcmltm_fa_config.c
 *
 * Logical Table Manager Field Adaptation.
 *
 * Configuration and stats.
 *
 * This file contains the LTM FA stage functions for LT table
 * statistics, information, and control parameters. LT statistics are
 * counters of table operations that are not preserved over HA events.
 * LT controls are parameters which may be modified at runtime
 * (though often only on an empty table). Table info includes statistics
 * which _are_ preserved over HA events.
 *
 * These values reach into the core of the LTM data structure, and so are
 * not suitable for LTA dispatch.  Access to these elements are protected
 * from components outside of the LTM.  Registered callbacks permit
 * the PTM to track individual PT ops within LTA implementations.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <bcmltm/bcmltm_tree.h>
#include <bcmltm/bcmltm_internal.h>
#include <bcmltm/bcmltm_wb_types_internal.h>
#include <bcmltm/bcmltm_db_core_internal.h>
#include <bcmltm/bcmltm_db_table_internal.h>
#include <bcmltm/bcmltm_fa_util_internal.h>
#include <bcmltm/bcmltm_fa_tree_nodes_internal.h>


/*******************************************************************************
 * Local definitions
 */

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMLTM_FIELDADAPTATION

/*******************************************************************************
 * Private functions
 */


/*******************************************************************************
 * Public functions
 */

int
bcmltm_fa_node_traverse_ltid(int unit,
                             bcmltm_lt_md_t *lt_md,
                             bcmltm_lt_state_t *lt_state,
                             bcmltm_entry_t *lt_entry,
                             uint32_t *ltm_buffer,
                             void *node_cookie)
{
    int rv;
    uint32_t target_ltid, offset_ltid;
    uint32_t target_fid, offset_gfid, target_gfid;
    uint32_t target_group, offset_group;
    uint32_t target_resource, offset_resource;
    bcmltm_lt_state_t *target_lt_state;
    bcmltm_fa_ltm_t *ltm_offsets;
    uint32_t max_fid;
    bcmltm_table_field_info_t field_info;
    bcmltm_table_field_select_t field_select;
    bcmltm_table_info_t table_info;
    bool found = FALSE;

    SHR_FUNC_ENTER(unit);

    ltm_offsets = BCMLTM_FA_LTM(node_cookie);

    if (ltm_offsets == NULL) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    offset_ltid = ltm_offsets->key_offset[BCMLTM_TABLE_KEYS_LTID];
    offset_gfid = ltm_offsets->key_offset[BCMLTM_TABLE_KEYS_LT_FIELD_ID];
    offset_group =
        ltm_offsets->key_offset[BCMLTM_TABLE_KEYS_SELECT_GROUP];
    offset_resource =
        ltm_offsets->key_offset[BCMLTM_TABLE_KEYS_RESOURCE_INFO];

    if (offset_ltid == BCMLTM_WB_OFFSET_INVALID) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    target_ltid = 0;
    target_fid = 0;
    target_group = 0;
    target_resource = 0;

    if (lt_entry->in_fields != NULL) { /* TRAVERSE next */
        /* Previous key values were copied to WB */
        target_ltid = ltm_buffer[offset_ltid];
        if (offset_gfid != BCMLTM_WB_OFFSET_INVALID) {
            target_gfid = ltm_buffer[offset_gfid];
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmltm_db_gfid_to_fid(unit,
                                       target_ltid, target_gfid, &target_fid));
            if (offset_group != BCMLTM_WB_OFFSET_INVALID) {
                /* Retrieve and increment field select group */
                target_group = ltm_buffer[offset_group] + 1;
            } else {
                /* Increment FID */
                target_fid++;
            }
        } else if (offset_resource != BCMLTM_WB_OFFSET_INVALID) {
            target_resource = ltm_buffer[offset_resource] + 1;
        } else {
            /* Increment LTID */
            target_ltid++;
        }
    }

    do {
        rv = bcmltm_state_lt_get(unit, target_ltid, &target_lt_state);

        if (rv == SHR_E_NOT_FOUND) {
            /* Not a valid LTID, skip */
            target_ltid++;
            continue;
        } else if (SHR_FAILURE(rv)) {
            /* Eventually the state get function will return SHR_E_PARAM
             * when the ltid exceeds the maximum defined value.
             */
            break;
        }

        /* Next level if needed */
        if (offset_gfid != BCMLTM_WB_OFFSET_INVALID) {
            /* Field ID is a key for this table. */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmltm_db_fid_max_get(unit, target_ltid, &max_fid));
            for (; target_fid <= max_fid; target_fid++) {
                rv = bcmltm_db_table_field_info_get(unit, target_ltid,
                                                    target_fid, &field_info);
                if (SHR_FAILURE(rv)) {
                    /* Not a valid field ID for this LT */
                    continue;
                }

                if (offset_group != BCMLTM_WB_OFFSET_INVALID) {
                    for (; target_group < field_info.num_select_group;
                         target_group++) {
                        rv = bcmltm_db_table_field_select_get(unit,
                                                              target_ltid,
                                                              target_fid,
                                                              target_group,
                                                              &field_select);
                        if (SHR_SUCCESS(rv)) {
                            /* Valid field select group for this LT */
                            found = TRUE;
                            break;
                        }
                    }
                    if (target_group >= field_info.num_select_group) {
                        /* Reset for next LT */
                        target_group = 0;
                    }
                } else {
                    found = TRUE;
                }
                if (found) {
                    break;
                }
            }
            if (target_fid > max_fid) {
                /* Reset for next LT */
                target_fid = 0;
            }
        } else if (offset_resource != BCMLTM_WB_OFFSET_INVALID) {
            rv = bcmltm_db_table_info_get(unit, target_ltid, &table_info);
            if (SHR_SUCCESS(rv)) {
                if (target_resource < table_info.num_resource_info) {
                    found = TRUE;
                } else {
                    target_resource = 0;
                }
            } /* Else not a valid LTID */
        } else {
            found = TRUE;
        }
        if (found) {
            break;
        }
        target_ltid++;
    } while (1);

    if (SHR_SUCCESS(rv)) {
        /* Copy keys to WB */
        ltm_buffer[offset_ltid] = target_ltid;
        if (offset_gfid != BCMLTM_WB_OFFSET_INVALID) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmltm_db_fid_to_gfid(unit,
                                       target_ltid, target_fid, &target_gfid));
            ltm_buffer[offset_gfid] = target_gfid;
        }
        if (offset_group != BCMLTM_WB_OFFSET_INVALID) {
            ltm_buffer[offset_group] = target_group;
        }
        if (offset_resource != BCMLTM_WB_OFFSET_INVALID) {
            ltm_buffer[offset_resource] = target_resource;
        }
    } else if (rv == SHR_E_PARAM) {
        /* LTID out of valid range. */
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    } else {
        SHR_ERR_EXIT(rv);
    }

exit:
    SHR_FUNC_EXIT();
}

