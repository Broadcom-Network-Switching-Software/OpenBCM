/*! \file bcmfp_group_oper.c
 *
 * BCMFP group operational information get/set APIs.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <shr/shr_ha.h>
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bcmfp/bcmfp_ha_internal.h>
#include <bcmfp/bcmfp_types_internal.h>
#include <bcmfp/bcmfp_idp_internal.h>
#include <bcmfp/bcmfp_group_internal.h>
#include <bcmfp/generated/bcmfp_ha.h>

#define BSL_LOG_MODULE BSL_LS_BCMFP_DEV

/*!
 * \brief Get the pointer to memory segment, in HA area, that is
 *  holding the mapping between group template id and entry id(s)
 *  associated to it.
 *
 * \param [in] unit Logical device id
 * \param [in] stage_id stage id(BCMFP_STAGE_ID_XXX).
 * \param [out] group_entry_map Pointer to memory segment.
 * \param [out] map_size Number of elements in the map array.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM group_entry_map is NULL
 * \retval SHR_E_INTERNAL Unsupported stage_id
 */
STATIC int
bcmfp_group_entry_map_get(int unit,
                          bcmfp_stage_id_t stage_id,
                          void **group_entry_map,
                          uint32_t *map_size)
{
    void *ha_mem = NULL;
    uint8_t blk_id = BCMFP_HA_BLK_ID_INVALID;
    uint32_t offset = 0;
    bcmfp_stage_oper_info_t *stage_oper_info = NULL;
    size_t size = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(group_entry_map, SHR_E_PARAM);
    SHR_NULL_CHECK(map_size, SHR_E_PARAM);

    BCMFP_HA_STAGE_ID_TO_BLK_ID(stage_id, blk_id);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_ha_mem_get(unit, blk_id, &ha_mem));
    size = sizeof(bcmfp_ha_blk_hdr_t);
    stage_oper_info = (bcmfp_stage_oper_info_t *)
                           ((uint8_t *)ha_mem + size);
    offset = stage_oper_info->group_entry_map_seg;
    *group_entry_map = (void *)((uint8_t *)ha_mem + offset);
    *map_size = stage_oper_info->group_entry_map_size;
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_group_entry_map_traverse(int unit,
                               bcmfp_stage_id_t stage_id,
                               uint32_t group_id,
                               bcmfp_entry_id_traverse_cb cb,
                               void *user_data)
{
    void *group_entry_map = NULL;
    uint32_t map_size = 0;
    bcmfp_idp_map_type_t map_type;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(cb, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_group_entry_map_get(unit,
                                   stage_id,
                                   &group_entry_map,
                                   &map_size));

    map_type = BCMFP_IDP_MAP_TYPE_GROUP_TO_ENTRY;
    /* coverity[callee_ptr_arith] */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_idp_map_traverse(unit,
                                stage_id,
                                group_entry_map,
                                map_size,
                                map_type,
                                &group_id,
                                cb,
                                user_data));
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_group_entry_map_cond_traverse(int unit,
                                     bcmfp_stage_id_t stage_id,
                                     uint32_t group_id,
                                     bcmfp_entry_id_cond_traverse_cb cb,
                                     void *user_data,
                                     bool *cond_met)
{
    void *group_entry_map = NULL;
    uint32_t map_size = 0;
    bcmfp_idp_map_type_t map_type;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(cb, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_group_entry_map_get(unit,
                                   stage_id,
                                   &group_entry_map,
                                   &map_size));

    map_type = BCMFP_IDP_MAP_TYPE_GROUP_TO_ENTRY;
    /* coverity[callee_ptr_arith] */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_idp_map_cond_traverse(unit,
                                     stage_id,
                                     group_entry_map,
                                     map_size,
                                     map_type,
                                     &group_id,
                                     cb,
                                     user_data,
                                     cond_met));
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_group_entry_map_check(int unit,
                            bcmfp_stage_id_t stage_id,
                            uint32_t group_id,
                            bool *not_mapped)
{
    void *group_entry_map = NULL;
    uint32_t map_size = 0;
    bcmfp_idp_map_type_t map_type;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(not_mapped, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_group_entry_map_get(unit,
                                   stage_id,
                                   &group_entry_map,
                                   &map_size));

    map_type = BCMFP_IDP_MAP_TYPE_GROUP_TO_ENTRY;
    /* coverity[callee_ptr_arith] */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_idp_map_check(unit,
                             stage_id,
                             group_entry_map,
                             map_size,
                             map_type,
                             &group_id,
                             not_mapped));
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_group_entry_map_add(int unit,
                          bcmfp_stage_id_t stage_id,
                          uint32_t group_id,
                          uint32_t entry_id)
{
    void *group_entry_map = NULL;
    uint32_t map_size = 0;
    bcmfp_idp_map_type_t map_type;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_group_entry_map_get(unit,
                                   stage_id,
                                   &group_entry_map,
                                   &map_size));

    map_type = BCMFP_IDP_MAP_TYPE_GROUP_TO_ENTRY;
    /* coverity[callee_ptr_arith] */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_idp_map_add(unit,
                           stage_id,
                           group_entry_map,
                           map_size,
                           map_type,
                           &group_id,
                           entry_id));
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_group_entry_map_delete(int unit,
                             bcmfp_stage_id_t stage_id,
                             uint32_t group_id,
                             uint32_t entry_id)
{
    void *group_entry_map = NULL;
    uint32_t map_size = 0;
    bcmfp_idp_map_type_t map_type;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_group_entry_map_get(unit,
                                   stage_id,
                                   &group_entry_map,
                                   &map_size));

    map_type = BCMFP_IDP_MAP_TYPE_GROUP_TO_ENTRY;
    /* coverity[callee_ptr_arith] */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_idp_map_delete(unit,
                              stage_id,
                              group_entry_map,
                              map_size,
                              map_type,
                              &group_id,
                              entry_id));
exit:
    SHR_FUNC_EXIT();
}
