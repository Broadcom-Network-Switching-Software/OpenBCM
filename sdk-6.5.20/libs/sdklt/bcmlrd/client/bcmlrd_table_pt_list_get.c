/*! \file bcmlrd_table_pt_list_get.c
 *
 * Get the destination PT list.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <sal/sal_libc.h>
#include <bcmlrd/bcmlrd_map.h>
#include <bcmlrd/bcmlrd_client.h>

/*******************************************************************************
 * Local definitions
 */

/* BSL Module */
#define BSL_LOG_MODULE BSL_LS_BCMLRD_CLIENT

/*******************************************************************************
 * Public functions
 */

int
bcmlrd_table_pt_list_get(int unit,
                         bcmlrd_sid_t sid,
                         uint32_t max_pt_count,
                         bcmdrd_sid_t *ptid_list,
                         uint32_t *actual_pt_count)
{
    const bcmlrd_map_t *sid_map;
    const bcmlrd_map_group_t *group = NULL;
    const bcmlrd_map_entry_t *entry = NULL;
    const bcmlrd_field_xfrm_desc_t *xfrm = NULL;
    size_t i, j, c, p;
    size_t max_count = 0;
    size_t actual_count = 0;
    uint32_t pt_idx = 0;
    bool found = false;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT(
        bcmlrd_map_get(unit, sid, &sid_map));

    SHR_IF_ERR_EXIT(
        bcmlrd_map_table_attr_count_get(unit,
                                        sid_map->src_id,
                                        BCMLRD_MAP_TABLE_ATTRIBUTE_HARDWARE,
                                        &max_count));

    if (max_count) {
        SHR_IF_ERR_EXIT(
            bcmlrd_map_table_attr_list_get(unit,
                                           sid_map->src_id,
                                           BCMLRD_MAP_TABLE_ATTRIBUTE_HARDWARE,
                                           max_count,
                                           &ptid_list[pt_idx],
                                           &actual_count));

        pt_idx += (uint32_t) actual_count;
    }

    for (i = 0; i < sid_map->groups; i++) {
        group = &sid_map->group[i];
        if (group->dest.kind != BCMLRD_MAP_PHYSICAL) {
            continue;
        }

        ptid_list[pt_idx] = group->dest.id;
        pt_idx++;

        for (j = 0; j < group->entries; j++) {
            entry = &group->entry[j];
            if (entry->entry_type == BCMLRD_MAP_ENTRY_FWD_KEY_FIELD_XFRM_HANDLER ||
                entry->entry_type == BCMLRD_MAP_ENTRY_FWD_VALUE_FIELD_XFRM_HANDLER ||
                entry->entry_type == BCMLRD_MAP_ENTRY_REV_KEY_FIELD_XFRM_HANDLER ||
                entry->entry_type == BCMLRD_MAP_ENTRY_REV_VALUE_FIELD_XFRM_HANDLER ||
                entry->entry_type == BCMLRD_MAP_ENTRY_ALWAYS_REV_VALUE_FIELD_XFRM_HANDLER) {
                xfrm = entry->u.xfrm.desc;
                if (!xfrm->tables) {
                    continue;
                }

                for (p = 0; p < xfrm->tables; p++) {
                    uint32_t tbl_id = xfrm->tbl[p];
                    found = false;

                    /* Search existing pt list. */
                    for (c = 0; c < pt_idx; c++) {
                        if (ptid_list[c] == tbl_id) {
                            found = true;
                            break;
                        }
                    }
                    if (found) {
                        continue;
                    }
                    if (pt_idx >= max_pt_count) {
                        break;
                    }
                    ptid_list[pt_idx] = tbl_id;
                    pt_idx++;
                }
            }
        }
        if (pt_idx >= max_pt_count) {
            break;
        }
    }
    *actual_pt_count = pt_idx;

exit:
    SHR_FUNC_EXIT();
}

