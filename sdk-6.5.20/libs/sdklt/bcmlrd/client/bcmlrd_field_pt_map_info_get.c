/*! \file bcmlrd_field_pt_map_info_get.c
 *
 * Get the destination mappings for
 * a given table ID and field ID.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_error.h>
#include <sal/sal_libc.h>
#include <bcmlrd/bcmlrd_map.h>
#include <bcmlrd/bcmlrd_client.h>

int
bcmlrd_field_pt_map_info_get(int unit,
                             bcmlrd_sid_t sid,
                             bcmlrd_fid_t fid,
                             uint32_t num_entries,
                             uint32_t *ret_entries,
                             bcmlrd_pt_map_info_t *pt_map_infos)
{
    int rv = SHR_E_PARAM;
    const bcmlrd_map_t *sid_map;
    const bcmlrd_map_group_t *group = NULL;
    const bcmlrd_map_entry_t *entry = NULL;
    const bcmlrd_field_xfrm_desc_t *xfrm = NULL;
    bcmlrd_pt_map_info_t *pt_map = NULL;
    size_t i, j, s, d, f;
    uint32_t index = 0;
    bool found = false;
    uint32_t pt_id;

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

            pt_id = group->dest.id;
            for (j = 0; j < group->entries; j++) {
                if (index >= num_entries) {
                    /* No more space in return array */
                    break;
                }
                entry = &group->entry[j];
                found = false;
                if (entry->entry_type == BCMLRD_MAP_ENTRY_MAPPED_KEY ||
                    entry->entry_type == BCMLRD_MAP_ENTRY_MAPPED_VALUE) {
                    if (fid == entry->u.mapped.src.field_id) {
                        pt_map = &pt_map_infos[index];
                        pt_map->pt_id = pt_id;
                        pt_map->map_type_name = "Direct";
                        sal_memcpy(&pt_map->field, &entry->desc, sizeof(bcmlrd_field_desc_t));
                        index++;
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
                        for (f = 0; f < xfrm->dst_fields; f++) {
                            pt_map = &pt_map_infos[index];
                            pt_map->pt_id = pt_id;
                            if (xfrm->tables) {
                                pt_map->map_type_name = "Table Transform";
                            } else {
                                pt_map->map_type_name = "Transform";
                            }
                            pt_map->int_field_type = "Forward";
                            sal_memcpy(&pt_map->field, &xfrm->dst[f], sizeof(bcmlrd_field_desc_t));
                            index++;
                        }
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
                        for (f = 0; f < xfrm->src_fields; f++) {
                            pt_map = &pt_map_infos[index];
                            pt_map->pt_id = pt_id;
                            if (xfrm->tables) {
                                pt_map->map_type_name = "Table Transform";
                            } else {
                                pt_map->map_type_name = "Transform";
                            }
                            pt_map->int_field_type = "Reverse";
                            sal_memcpy(&pt_map->field, &xfrm->src[f], sizeof(bcmlrd_field_desc_t));
                            index++;
                        }
                    }
                }
            }
        }

    } while (0);

    *ret_entries = index;

    return rv;
}

