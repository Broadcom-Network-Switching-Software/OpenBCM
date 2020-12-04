/*! \file bcmfp_pse_oper.c
 *
 * BCMFP presel enetry operational information get/set APIs.
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
#include <bcmfp/bcmfp_internal.h>
#include <bcmfp/bcmfp_ha_internal.h>
#include <bcmfp/bcmfp_types_internal.h>
#include <bcmfp/bcmfp_idp_internal.h>
#include <bcmfp/bcmfp_internal.h>
#include <bcmfp/generated/bcmfp_ha.h>

#define BSL_LOG_MODULE BSL_LS_BCMFP_DEV

/*!
 * \brief Get the pointer to memory segment, in HA area, that is
 *  holding the mapping between presel template ID and group ID(s)
 *  associated to it.
 *
 * \param [in] unit Logical device id
 * \param [in] stage_id stage Id(BCMFP_STAGE_ID_XXX).
 * \param [out] pse_group_map Pointer to memory segment.
 * \param [out] map_size Number of elements in the map array.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM pse_group_map is NULL
 * \retval SHR_E_INTERNAL Unsupported stage_id
 */
STATIC int
bcmfp_pse_group_map_get(int unit,
                        bcmfp_stage_id_t stage_id,
                        void **pse_group_map,
                        uint32_t *map_size)
{
    void *ha_mem = NULL;
    uint8_t blk_id = BCMFP_HA_BLK_ID_INVALID;
    uint32_t offset = 0;
    bcmfp_stage_oper_info_t *stage_oper_info = NULL;
    size_t size = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(pse_group_map, SHR_E_PARAM);

    BCMFP_HA_STAGE_ID_TO_BLK_ID(stage_id, blk_id);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_ha_mem_get(unit, blk_id, &ha_mem));
    size = sizeof(bcmfp_ha_blk_hdr_t);
    stage_oper_info = (bcmfp_stage_oper_info_t *)
                           ((uint8_t *)ha_mem + size);
    offset = stage_oper_info->pse_group_map_seg;
    *pse_group_map = (void *)((uint8_t *)ha_mem + offset);
    *map_size = stage_oper_info->pse_group_map_size;

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_pse_group_map_traverse(int unit,
                             bcmfp_stage_id_t stage_id,
                             uint32_t pse_id,
                             bcmfp_entry_id_traverse_cb cb,
                             void *user_data)
{
    void *pse_group_map = NULL;
    uint32_t map_size = 0;
    bcmfp_idp_map_type_t map_type = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(cb, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_pse_group_map_get(unit,
                                 stage_id,
                                 &pse_group_map,
                                 &map_size));
    map_type = BCMFP_IDP_MAP_TYPE_PSE_TO_GROUP;
    /* coverity[callee_ptr_arith] */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_idp_map_traverse(unit,
                                stage_id,
                                pse_group_map,
                                map_size,
                                map_type,
                                &pse_id,
                                cb,
                                user_data));
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_pse_group_map_cond_traverse(int unit,
                                  bcmfp_stage_id_t stage_id,
                                  uint32_t pse_id,
                                  bcmfp_entry_id_cond_traverse_cb cb,
                                  void *user_data,
                                  bool *cond_met)
{
    void *pse_group_map = NULL;
    uint32_t map_size = 0;
    bcmfp_idp_map_type_t map_type = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(cb, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_pse_group_map_get(unit,
                                 stage_id,
                                 &pse_group_map,
                                 &map_size));
    map_type = BCMFP_IDP_MAP_TYPE_PSE_TO_GROUP;
    /* coverity[callee_ptr_arith] */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_idp_map_cond_traverse(unit,
                                     stage_id,
                                     pse_group_map,
                                     map_size,
                                     map_type,
                                     &pse_id,
                                     cb,
                                     user_data,
                                     cond_met));
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_pse_group_map_check(int unit,
                          bcmfp_stage_id_t stage_id,
                          uint32_t pse_id,
                          bool *not_mapped)
{
    void *pse_group_map = NULL;
    uint32_t map_size = 0;
    bcmfp_idp_map_type_t map_type = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(not_mapped, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_pse_group_map_get(unit,
                                 stage_id,
                                 &pse_group_map,
                                 &map_size));

    map_type = BCMFP_IDP_MAP_TYPE_PSE_TO_GROUP;
    /* coverity[callee_ptr_arith] */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_idp_map_check(unit,
                             stage_id,
                             pse_group_map,
                             map_size,
                             map_type,
                             &pse_id,
                             not_mapped));
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_pse_group_map_add(int unit,
                        bcmfp_stage_id_t stage_id,
                        uint32_t pse_id,
                        uint32_t group_id)
{
    void *pse_group_map = NULL;
    uint32_t map_size = 0;
    bcmfp_idp_map_type_t map_type = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_pse_group_map_get(unit,
                                 stage_id,
                                 &pse_group_map,
                                 &map_size));

    map_type = BCMFP_IDP_MAP_TYPE_PSE_TO_GROUP;
    /* coverity[callee_ptr_arith] */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_idp_map_add(unit,
                           stage_id,
                           pse_group_map,
                           map_size,
                           map_type,
                           &pse_id,
                           group_id));
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_pse_group_map_delete(int unit,
                           bcmfp_stage_id_t stage_id,
                           uint32_t pse_id,
                           uint32_t group_id)
{
    void *pse_group_map = NULL;
    uint32_t map_size = 0;
    bcmfp_idp_map_type_t map_type = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_pse_group_map_get(unit,
                                 stage_id,
                                 &pse_group_map,
                                 &map_size));

    map_type = BCMFP_IDP_MAP_TYPE_PSE_TO_GROUP;
    /* coverity[callee_ptr_arith] */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_idp_map_delete(unit,
                              stage_id,
                              pse_group_map,
                              map_size,
                              map_type,
                              &pse_id,
                              group_id));
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_presel_same_prio_grp_cnt_get(int unit,
                                   bcmfp_stage_id_t stage_id,
                                   uint32_t presel_id,
                                   uint32_t group_priority,
                                   uint16_t *group_count)
{
    uint8_t blk_id = BCMFP_HA_BLK_ID_INVALID;
    uint32_t blk_offset = BCMFP_HA_BLK_OFFSET_INVALID;
    void *presel_group_map = NULL;
    bcmfp_group_id_t gid = 0;
    bcmfp_group_oper_info_t *group_oper_info = NULL;
    bcmfp_ha_blk_info_t *blk_info = NULL;
    bcmfp_generic_data_t *ha_element = NULL;
    uint32_t map_size = 0;

    SHR_FUNC_ENTER(unit);

    *group_count = 0;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_pse_group_map_get(unit,
                                 stage_id,
                                 &presel_group_map,
                                 &map_size));
    blk_info = (bcmfp_ha_blk_info_t *)presel_group_map;
    blk_id = blk_info[presel_id].blk_id;
    blk_offset = blk_info[presel_id].blk_offset;
    while (blk_id != BCMFP_HA_BLK_ID_INVALID &&
           blk_offset != BCMFP_HA_BLK_OFFSET_INVALID) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_ha_element_get(unit,
                                  blk_id,
                                  blk_offset,
                                  &ha_element));
        gid = ha_element->value;
        SHR_IF_ERR_EXIT_EXCEPT_IF
            (bcmfp_group_oper_info_get(unit,
                                       stage_id,
                                       gid,
                                       &group_oper_info),
                                       SHR_E_NOT_FOUND);
        if ((group_oper_info != NULL) &&
            (group_oper_info->valid == TRUE)) {
            if (group_priority == group_oper_info->group_prio) {
                *group_count = *group_count + 1;
            }
        }
        blk_id = ha_element->next_blk_id;
        blk_offset = ha_element->next_blk_offset;
    }

exit:
    SHR_FUNC_EXIT();
}
