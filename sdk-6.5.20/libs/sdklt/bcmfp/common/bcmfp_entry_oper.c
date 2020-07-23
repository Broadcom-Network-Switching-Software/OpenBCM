/*! \file bcmfp_entry_oper.c
 *
 * BCMFP entry operational information get/set APIs.
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
#include <bcmfp/bcmfp_internal.h>
#include <bcmfp/bcmfp_types_internal.h>
#include <bcmfp/bcmfp_idp_internal.h>
#include <bcmfp/bcmfp_group_internal.h>
#include <bcmfp/generated/bcmfp_ha.h>

#define BSL_LOG_MODULE BSL_LS_BCMFP_DEV

static int
bcmfp_entry_list_compress_parent_entry_lookup(int unit,
                                 bcmfp_stage_id_t stage_id,
                                 bcmfp_entry_id_t parent_entry_id,
                                 bcmfp_entry_id_t child_entry_id,
                                 void *user_data)
{
    bcmfp_idp_info_t *idp_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(user_data, SHR_E_PARAM);

    idp_info = user_data;

    if (idp_info->list_compress_parent_found == TRUE) {
        SHR_EXIT();
    }

    idp_info->list_compress_parent_entry_id = parent_entry_id;
    idp_info->list_compress_parent_found = TRUE;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the pointer to memory segment, in HA area, that is
 *  holding the mapping between entry id and its list compressed
 *  entry id(s).
 *
 * \param [in]  unit Logical device id
 * \param [in]  stage_id stage id(BCMFP_STAGE_ID_XXX).
 * \param [out] entry_list_compress_child_entry_map Pointer to memory segment.
 * \param [out] map_size Number of elements in the map array.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM entry_list_compress_child_entry_map is NULL
 * \retval SHR_E_INTERNAL Unsupported stage_id
 */
STATIC int
bcmfp_entry_list_compress_child_entry_map_get(int unit,
                                      bcmfp_stage_id_t stage_id,
                                      void **child_entry_map,
                                      uint32_t *map_size)
{
    void *ha_mem = NULL;
    uint8_t blk_id = BCMFP_HA_BLK_ID_INVALID;
    uint32_t offset = 0;
    bcmfp_stage_oper_info_t *stage_oper_info = NULL;
    size_t size = 0;
    bcmfp_compress_oper_info_t *compress_oper_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(child_entry_map, SHR_E_PARAM);
    SHR_NULL_CHECK(map_size, SHR_E_PARAM);

    BCMFP_HA_STAGE_ID_TO_BLK_ID(stage_id, blk_id);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_ha_mem_get(unit, blk_id, &ha_mem));
    size = sizeof(bcmfp_ha_blk_hdr_t);
    stage_oper_info =
        (bcmfp_stage_oper_info_t *)((uint8_t *)ha_mem + size);

    blk_id = stage_oper_info->compress_oper_info.blk_id;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_ha_mem_get(unit, blk_id, &ha_mem));
    compress_oper_info =
        (bcmfp_compress_oper_info_t *)((uint8_t *)ha_mem + size);
    offset = compress_oper_info->entry_child_entry_map_seg;
    *child_entry_map = (void *)((uint8_t *)ha_mem + offset);
    *map_size = compress_oper_info->entry_child_entry_map_size;
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the pointer to memory segment, in HA area, that is
 *  holding the mapping between entry id and its parent entry_id
 *  in list compression.
 *
 * \param [in]  unit Logical device id
 * \param [in]  stage_id stage id(BCMFP_STAGE_ID_XXX).
 * \param [out] entry_list_compress_parent_entry_map Pointer to memory segment.
 * \param [out] map_size Number of elements in the map array.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM entry_list_compress_child_entry_map is NULL
 * \retval SHR_E_INTERNAL Unsupported stage_id
 */
STATIC int
bcmfp_entry_list_compress_parent_entry_map_get(int unit,
                                       bcmfp_stage_id_t stage_id,
                                       void **parent_entry_map,
                                       uint32_t *map_size)
{
    void *ha_mem = NULL;
    uint8_t blk_id = BCMFP_HA_BLK_ID_INVALID;
    uint32_t offset = 0;
    bcmfp_stage_oper_info_t *stage_oper_info = NULL;
    size_t size = 0;
    bcmfp_compress_oper_info_t *compress_oper_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(parent_entry_map, SHR_E_PARAM);
    SHR_NULL_CHECK(map_size, SHR_E_PARAM);

    BCMFP_HA_STAGE_ID_TO_BLK_ID(stage_id, blk_id);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_ha_mem_get(unit, blk_id, &ha_mem));
    size = sizeof(bcmfp_ha_blk_hdr_t);
    stage_oper_info =
          (bcmfp_stage_oper_info_t *)((uint8_t *)ha_mem + size);

    blk_id = stage_oper_info->compress_oper_info.blk_id;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_ha_mem_get(unit, blk_id, &ha_mem));
    compress_oper_info =
        (bcmfp_compress_oper_info_t *)((uint8_t *)ha_mem + size);

    offset = compress_oper_info->entry_parent_entry_map_seg;
    *parent_entry_map = (void *)((uint8_t *)ha_mem + offset);
    *map_size = compress_oper_info->entry_parent_entry_map_size;
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_entry_list_compress_status_get(int unit,
                                 bcmfp_stage_id_t stage_id,
                                 bcmfp_entry_id_t entry_id,
                                 bool *list_compressed)
{
    bcmfp_entry_id_traverse_cb cb;
    bcmfp_idp_info_t idp_info;
    void *user_data = NULL;
    bcmfp_stage_oper_info_t *stage_opinfo = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(list_compressed, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_oper_info_get(unit, stage_id, &stage_opinfo));
    if (stage_opinfo->compress_oper_info.blk_id ==
                         BCMFP_HA_BLK_ID_INVALID) {
        *list_compressed = FALSE;
        SHR_EXIT();
    }

    sal_memset(&idp_info, 0, sizeof(bcmfp_idp_info_t));
    idp_info.list_compress_child_entry_id = entry_id;
    idp_info.list_compress_parent_found = FALSE;
    cb = bcmfp_entry_list_compress_parent_entry_lookup;
    user_data = &idp_info;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_entry_list_compress_parent_entry_map_traverse(unit,
                                                    stage_id,
                                                    entry_id,
                                                    cb,
                                                    user_data));
    if (idp_info.list_compress_parent_found == FALSE) {
        *list_compressed = FALSE;
    } else {
        *list_compressed = TRUE;
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_entry_list_compress_parent_entry_map_traverse(int unit,
                                  bcmfp_stage_id_t stage_id,
                                  uint32_t child_eid,
                                  bcmfp_entry_id_traverse_cb cb,
                                  void *user_data)
{
    void *entry_list_compress_parent_entry_map = NULL;
    uint32_t map_size = 0;
    bcmfp_idp_map_type_t map_type = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(cb, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_entry_list_compress_parent_entry_map_get(unit,
                                      stage_id,
                                      &entry_list_compress_parent_entry_map,
                                      &map_size));
    map_type = BCMFP_IDP_MAP_TYPE_ENTRY_TO_LC_ENTRY;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_idp_map_traverse(unit,
                                stage_id,
                                entry_list_compress_parent_entry_map,
                                map_size,
                                map_type,
                                &child_eid,
                                cb,
                                user_data));

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_entry_list_compress_parent_entry_map_check(int unit,
                           bcmfp_stage_id_t stage_id,
                           uint32_t child_eid,
                           bool *not_mapped)
{
    void *entry_list_compress_parent_entry_map = NULL;
    uint32_t map_size = 0;
    bcmfp_idp_map_type_t map_type = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(not_mapped, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_entry_list_compress_parent_entry_map_get(unit,
                                      stage_id,
                                      &entry_list_compress_parent_entry_map,
                                      &map_size));
    map_type = BCMFP_IDP_MAP_TYPE_ENTRY_TO_LC_ENTRY;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_idp_map_check(unit,
                             stage_id,
                             entry_list_compress_parent_entry_map,
                             map_size,
                             map_type,
                             &child_eid,
                             not_mapped));
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_entry_list_compress_parent_entry_map_add(int unit,
                             bcmfp_stage_id_t stage_id,
                             uint32_t child_eid,
                             uint32_t parent_eid)
{
    void *entry_list_compress_parent_entry_map = NULL;
    uint32_t map_size = 0;
    bcmfp_idp_map_type_t map_type = 0;
    bcmfp_stage_oper_info_t *stage_opinfo = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_oper_info_get(unit, stage_id, &stage_opinfo));
    if (stage_opinfo->compress_oper_info.blk_id ==
                         BCMFP_HA_BLK_ID_INVALID) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_compress_oper_info_create(unit, stage_id));
    }
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_entry_list_compress_parent_entry_map_get(unit,
                                      stage_id,
                                      &entry_list_compress_parent_entry_map,
                                      &map_size));

    map_type = BCMFP_IDP_MAP_TYPE_ENTRY_TO_LC_ENTRY;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_idp_map_add(unit,
                           stage_id,
                           entry_list_compress_parent_entry_map,
                           map_size,
                           map_type,
                           &child_eid,
                           parent_eid));
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_entry_list_compress_parent_entry_map_delete(int unit,
                                bcmfp_stage_id_t stage_id,
                                uint32_t child_eid,
                                uint32_t parent_eid)
{
    void *entry_list_compress_parent_entry_map = NULL;
    uint32_t map_size = 0;
    bcmfp_idp_map_type_t map_type = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_entry_list_compress_parent_entry_map_get(unit,
                                      stage_id,
                                      &entry_list_compress_parent_entry_map,
                                      &map_size));

    map_type = BCMFP_IDP_MAP_TYPE_ENTRY_TO_LC_ENTRY;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_idp_map_delete(unit,
                              stage_id,
                              entry_list_compress_parent_entry_map,
                              map_size,
                              map_type,
                              &child_eid,
                              parent_eid));
exit:
    SHR_FUNC_EXIT();
}
int
bcmfp_entry_list_compress_child_entry_map_traverse(int unit,
                                  bcmfp_stage_id_t stage_id,
                                  uint32_t parent_eid,
                                  bcmfp_entry_id_traverse_cb cb,
                                  void *user_data)
{
    void *entry_list_compress_child_entry_map = NULL;
    uint32_t map_size = 0;
    bcmfp_idp_map_type_t map_type = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(cb, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_entry_list_compress_child_entry_map_get(unit,
                                      stage_id,
                                      &entry_list_compress_child_entry_map,
                                      &map_size));
    map_type = BCMFP_IDP_MAP_TYPE_ENTRY_TO_LC_ENTRY;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_idp_map_traverse(unit,
                                stage_id,
                                entry_list_compress_child_entry_map,
                                map_size,
                                map_type,
                                &parent_eid,
                                cb,
                                user_data));

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_entry_list_compress_child_entry_map_check(int unit,
                           bcmfp_stage_id_t stage_id,
                           uint32_t parent_eid,
                           bool *not_mapped)
{
    void *entry_list_compress_child_entry_map = NULL;
    uint32_t map_size = 0;
    bcmfp_idp_map_type_t map_type = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(not_mapped, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_entry_list_compress_child_entry_map_get(unit,
                                      stage_id,
                                      &entry_list_compress_child_entry_map,
                                      &map_size));
    map_type = BCMFP_IDP_MAP_TYPE_ENTRY_TO_LC_ENTRY;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_idp_map_check(unit,
                             stage_id,
                             entry_list_compress_child_entry_map,
                             map_size,
                             map_type,
                             &parent_eid,
                             not_mapped));
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_entry_list_compress_child_entry_map_add(int unit,
                             bcmfp_stage_id_t stage_id,
                             uint32_t parent_eid,
                             uint32_t child_eid)
{
    void *entry_list_compress_child_entry_map = NULL;
    uint32_t map_size = 0;
    bcmfp_idp_map_type_t map_type = 0;
    bcmfp_stage_oper_info_t *stage_opinfo = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_oper_info_get(unit, stage_id, &stage_opinfo));
    if (stage_opinfo->compress_oper_info.blk_id ==
                         BCMFP_HA_BLK_ID_INVALID) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_compress_oper_info_create(unit, stage_id));
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_entry_list_compress_child_entry_map_get(unit,
                                      stage_id,
                                      &entry_list_compress_child_entry_map,
                                      &map_size));

    map_type = BCMFP_IDP_MAP_TYPE_ENTRY_TO_LC_ENTRY;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_idp_map_add(unit,
                           stage_id,
                           entry_list_compress_child_entry_map,
                           map_size,
                           map_type,
                           &parent_eid,
                           child_eid));
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_entry_list_compress_child_entry_map_delete(int unit,
                                bcmfp_stage_id_t stage_id,
                                uint32_t parent_eid,
                                uint32_t child_eid)
{
    void *entry_list_compress_child_entry_map = NULL;
    uint32_t map_size = 0;
    bcmfp_idp_map_type_t map_type = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_entry_list_compress_child_entry_map_get(unit,
                                      stage_id,
                                      &entry_list_compress_child_entry_map,
                                      &map_size));

    map_type = BCMFP_IDP_MAP_TYPE_ENTRY_TO_LC_ENTRY;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_idp_map_delete(unit,
                              stage_id,
                              entry_list_compress_child_entry_map,
                              map_size,
                              map_type,
                              &parent_eid,
                              child_eid));
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_entry_list_compress_parent_entry_get(int unit,
                                 bcmfp_stage_id_t stage_id,
                                 bcmfp_entry_id_t child_entry_id,
                                 bcmfp_entry_id_t *parent_entry_id)
{
    bcmfp_entry_id_traverse_cb cb;
    bcmfp_idp_info_t idp_info;
    void *user_data = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(parent_entry_id, SHR_E_PARAM);

    sal_memset(&idp_info, 0, sizeof(bcmfp_idp_info_t));
    idp_info.list_compress_child_entry_id = child_entry_id;
    idp_info.list_compress_parent_found = FALSE;
    cb = bcmfp_entry_list_compress_parent_entry_lookup;
    user_data = &idp_info;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_entry_list_compress_parent_entry_map_traverse(unit,
                                                    stage_id,
                                                    child_entry_id,
                                                    cb,
                                                    user_data));
    if (idp_info.list_compress_parent_found == FALSE) {
        SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
    } else {
        *parent_entry_id =
             idp_info.list_compress_parent_entry_id;
    }
exit:
    SHR_FUNC_EXIT();
}

STATIC int
bcmfp_entry_list_compress_child_entry_move(int unit,
                                bcmfp_stage_id_t stage_id,
                                bcmfp_entry_id_t child_entry_id,
                                bcmfp_entry_id_t parent_entry_id,
                                void *user_data)
{
    bcmfp_idp_info_t *idp_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(user_data, SHR_E_PARAM);

    idp_info = user_data;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_entry_list_compress_child_entry_map_delete(unit,
                                                   stage_id,
                                                   parent_entry_id,
                                                   child_entry_id));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_entry_list_compress_parent_entry_map_delete(unit,
                                                    stage_id,
                                                    child_entry_id,
                                                    parent_entry_id));
    if (idp_info->list_compress_child_entry_id != child_entry_id) {
        parent_entry_id = idp_info->list_compress_child_entry_id;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_entry_list_compress_child_entry_map_add(unit,
                                                       stage_id,
                                                       parent_entry_id,
                                                       child_entry_id));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_entry_list_compress_parent_entry_map_add(unit,
                                                        stage_id,
                                                        child_entry_id,
                                                        parent_entry_id));
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_entry_list_compress_child_entry_map_move(int unit,
                                bcmfp_stage_id_t stage_id,
                                bcmfp_entry_id_t parent_entry_id,
                                bcmfp_entry_id_t *child_entry_id)
{
    bcmfp_entry_id_traverse_cb cb;
    bcmfp_idp_info_t idp_info;
    void *user_data = NULL;

    SHR_FUNC_ENTER(unit);

    sal_memset(&idp_info, 0, sizeof(bcmfp_idp_info_t));
    idp_info.list_compress_child_entry_id = *child_entry_id;
    cb = bcmfp_entry_list_compress_child_entry_move;
    user_data = &idp_info;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_entry_list_compress_child_entry_map_traverse(unit,
                                               stage_id,
                                               parent_entry_id,
                                               cb,
                                               user_data));
exit:
    SHR_FUNC_EXIT();
}
