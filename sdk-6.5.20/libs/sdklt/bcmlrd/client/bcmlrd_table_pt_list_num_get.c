/*! \file bcmlrd_table_pt_list_num_get.c
 *
 * Get the number of destination physical tables.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bcmlrd/bcmlrd_map.h>
#include <bcmlrd/bcmlrd_client.h>

/*******************************************************************************
 * Local definitions
 */

/* BSL Module */
#define BSL_LOG_MODULE BSL_LS_BCMLRD_CLIENT

/*******************************************************************************
 * Private functions
 */
static int
bcmlrd_table_pt_max_num_get(int unit,
                            const bcmlrd_map_t *sid_map,
                            uint32_t *max_count)
{
    const bcmlrd_map_group_t *group = NULL;
    const bcmlrd_map_entry_t *entry = NULL;
    const bcmlrd_field_xfrm_desc_t *xfrm = NULL;
    size_t i, j;
    size_t attr_count = 0;
    uint32_t num = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT(
        bcmlrd_map_table_attr_count_get(unit,
                                        sid_map->src_id,
                                        BCMLRD_MAP_TABLE_ATTRIBUTE_HARDWARE,
                                        &attr_count));
    num += (uint32_t) attr_count;

    for (i = 0; i < sid_map->groups; i++) {
        group = &sid_map->group[i];
        if (group->dest.kind != BCMLRD_MAP_PHYSICAL) {
            continue;
        }

        num++;
        for (j = 0; j < group->entries; j++) {
            entry = &group->entry[j];
            if (entry->entry_type == BCMLRD_MAP_ENTRY_FWD_KEY_FIELD_XFRM_HANDLER ||
                entry->entry_type == BCMLRD_MAP_ENTRY_FWD_VALUE_FIELD_XFRM_HANDLER ||
                entry->entry_type == BCMLRD_MAP_ENTRY_REV_KEY_FIELD_XFRM_HANDLER ||
                entry->entry_type == BCMLRD_MAP_ENTRY_REV_VALUE_FIELD_XFRM_HANDLER ||
                entry->entry_type == BCMLRD_MAP_ENTRY_ALWAYS_REV_VALUE_FIELD_XFRM_HANDLER) {
                xfrm = entry->u.xfrm.desc;
                if (xfrm->tables) {
                    num += xfrm->tables - 1;
                }
            }
        }
    }

    *max_count = num;

exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public functions
 */

int
bcmlrd_table_pt_list_num_get(int unit,
                             bcmlrd_sid_t sid,
                             uint32_t *num_pt)
{
    const bcmlrd_map_t *sid_map;
    uint32_t max_num = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT(
        bcmlrd_map_get(unit, sid, &sid_map));

    SHR_IF_ERR_EXIT(
        bcmlrd_table_pt_max_num_get(unit,
                                    sid_map,
                                    &max_num));
    *num_pt = max_num;
exit:
    SHR_FUNC_EXIT();
}

