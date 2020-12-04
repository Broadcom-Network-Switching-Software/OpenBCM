/*! \file bcmlrd_field_pt_map_info_num_get.c
 *
 * Get the number of destination mappings for
 * a given table ID and field ID.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_error.h>
#include <bcmlrd/bcmlrd_map.h>
#include <bcmlrd/bcmlrd_client.h>

int
bcmlrd_field_pt_map_info_num_get(int unit,
                                 bcmlrd_sid_t sid,
                                 bcmlrd_fid_t fid,
                                 uint32_t *num_entries)
{
    int rv = SHR_E_PARAM;
    const bcmlrd_map_t *sid_map;
    const bcmlrd_map_group_t *group = NULL;
    const bcmlrd_map_entry_t *entry = NULL;
    const bcmlrd_field_xfrm_desc_t *xfrm = NULL;
    size_t i, j, s, d;
    uint32_t num = 0;
    bool found = false;

    do {
        rv = bcmlrd_map_get(unit, sid, &sid_map);
        if (SHR_FAILURE(rv)) {
            break;
        }

        for (i = 0; i < sid_map->groups; i++) {
            group = &sid_map->group[i];
            if (group->dest.kind != BCMLRD_MAP_PHYSICAL) {
                continue;
            }

            for (j = 0; j < group->entries; j++) {
                entry = &group->entry[j];
                found = false;
                if (entry->entry_type == BCMLRD_MAP_ENTRY_MAPPED_KEY ||
                    entry->entry_type == BCMLRD_MAP_ENTRY_MAPPED_VALUE) {
                    if (fid == entry->u.mapped.src.field_id) {
                        num++;
                        continue;
                    }
                } else if (
                    entry->entry_type == BCMLRD_MAP_ENTRY_FWD_KEY_FIELD_XFRM_HANDLER ||
                    entry->entry_type == BCMLRD_MAP_ENTRY_FWD_VALUE_FIELD_XFRM_HANDLER) {
                    xfrm = entry->u.xfrm.desc;
                    for (s = 0; s < xfrm->src_fields; s++) {
                        const bcmltd_field_desc_t *src = &xfrm->src[s];

                        if (fid == src->field_id) {
                            found = true;
                            break;
                        }
                    }
                    if (found) {
                        num += xfrm->dst_fields;
                    }
                } else if (
                    entry->entry_type == BCMLRD_MAP_ENTRY_REV_KEY_FIELD_XFRM_HANDLER ||
                    entry->entry_type == BCMLRD_MAP_ENTRY_REV_VALUE_FIELD_XFRM_HANDLER) {
                    xfrm = entry->u.xfrm.desc;
                    for (d = 0; d < xfrm->dst_fields; d++) {
                        const bcmltd_field_desc_t *dst = &xfrm->dst[d];

                        if (fid == dst->field_id) {
                            found = true;
                            break;
                        }
                    }
                    if (found) {
                        num += xfrm->src_fields;
                    }
                }
            }
        }

        if (num) {
            *num_entries = num;
        }
    } while (0);

    return rv;
}

