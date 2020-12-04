/*! \file bcmfp_conflict_qset_oper.c
 *
 * BCMFP conflict QSET LT related operational information APIs.
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
#include <shr/shr_util.h>
#include <bcmlrd/bcmlrd_map.h>
#include <bcmfp/generated/bcmfp_ha.h>
#include <bcmmgmt/bcmmgmt_sysm.h>
#include <bcmfp/bcmfp_cth_conflict_qset.h>

#define BSL_LOG_MODULE BSL_LS_BCMFP_DEV

/*!
 * \brief Get the pointer to memory segment, in HA area, that is
 *  holding the mapping between conflict_qset template ID and
 *  group ID(s) associated to it.
 *
 * \param [in] unit Logical device id
 * \param [in] stage_id stage Id(BCMFP_STAGE_ID_XXX).
 * \param [out] conflict_qset_group_map Pointer to memory segment.
 * \param [out] map_size Number of elements in the map array.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM conflict_qset_group_map is NULL
 * \retval SHR_E_INTERNAL Unsupported stage_id
 */
STATIC int
bcmfp_conflict_qset_group_map_get(int unit,
                        bcmfp_stage_id_t stage_id,
                        void **conflict_qset_group_map,
                        uint32_t *map_size)
{
    void *ha_mem = NULL;
    uint8_t blk_id = BCMFP_HA_BLK_ID_INVALID;
    uint32_t offset = 0;
    bcmfp_stage_oper_info_t *stage_oper_info = NULL;
    size_t size = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(conflict_qset_group_map, SHR_E_PARAM);

    BCMFP_HA_STAGE_ID_TO_BLK_ID(stage_id, blk_id);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_ha_mem_get(unit, blk_id, &ha_mem));
    size = sizeof(bcmfp_ha_blk_hdr_t);
    stage_oper_info = (bcmfp_stage_oper_info_t *)
                           ((uint8_t *)ha_mem + size);
    offset = stage_oper_info->conflict_qset_group_map_seg;
    *conflict_qset_group_map = (void *)((uint8_t *)ha_mem + offset);
    *map_size = stage_oper_info->conflict_qset_group_map_size;

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_conflict_qset_group_map_traverse(int unit,
                             bcmfp_stage_id_t stage_id,
                             uint32_t conflict_qset_id,
                             bcmfp_entry_id_traverse_cb cb,
                             void *user_data)
{
    void *conflict_qset_group_map = NULL;
    uint32_t map_size = 0;
    bcmfp_idp_map_type_t map_type = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(cb, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_conflict_qset_group_map_get(unit,
                                 stage_id,
                                 &conflict_qset_group_map,
                                 &map_size));
    map_type = BCMFP_IDP_MAP_TYPE_CONFLICT_QSET_TO_GROUP;
    /* coverity[callee_ptr_arith] */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_idp_map_traverse(unit,
                                stage_id,
                                conflict_qset_group_map,
                                map_size,
                                map_type,
                                &conflict_qset_id,
                                cb,
                                user_data));
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_conflict_qset_group_map_check(int unit,
                          bcmfp_stage_id_t stage_id,
                          uint32_t conflict_qset_id,
                          bool *not_mapped)
{
    void *conflict_qset_group_map = NULL;
    uint32_t map_size = 0;
    bcmfp_idp_map_type_t map_type = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(not_mapped, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_conflict_qset_group_map_get(unit,
                                 stage_id,
                                 &conflict_qset_group_map,
                                 &map_size));

    map_type = BCMFP_IDP_MAP_TYPE_CONFLICT_QSET_TO_GROUP;
    /* coverity[callee_ptr_arith] */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_idp_map_check(unit,
                             stage_id,
                             conflict_qset_group_map,
                             map_size,
                             map_type,
                             &conflict_qset_id,
                             not_mapped));
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_conflict_qset_group_map_add(int unit,
                        bcmfp_stage_id_t stage_id,
                        uint32_t conflict_qset_id,
                        uint32_t group_id)
{
    void *conflict_qset_group_map = NULL;
    uint32_t map_size = 0;
    bcmfp_idp_map_type_t map_type = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_conflict_qset_group_map_get(unit,
                                 stage_id,
                                 &conflict_qset_group_map,
                                 &map_size));

    map_type = BCMFP_IDP_MAP_TYPE_CONFLICT_QSET_TO_GROUP;
    /* coverity[callee_ptr_arith] */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_idp_map_add(unit,
                           stage_id,
                           conflict_qset_group_map,
                           map_size,
                           map_type,
                           &conflict_qset_id,
                           group_id));
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_conflict_qset_group_map_delete(int unit,
                           bcmfp_stage_id_t stage_id,
                           uint32_t conflict_qset_id,
                           uint32_t group_id)
{
    void *conflict_qset_group_map = NULL;
    uint32_t map_size = 0;
    bcmfp_idp_map_type_t map_type = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_conflict_qset_group_map_get(unit,
                                 stage_id,
                                 &conflict_qset_group_map,
                                 &map_size));

    map_type = BCMFP_IDP_MAP_TYPE_CONFLICT_QSET_TO_GROUP;
    /* coverity[callee_ptr_arith] */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_idp_map_delete(unit,
                              stage_id,
                              conflict_qset_group_map,
                              map_size,
                              map_type,
                              &conflict_qset_id,
                              group_id));
exit:
    SHR_FUNC_EXIT();
}

STATIC int
bcmfp_conflict_qset_oper_info_acquire(int unit,
                               bcmfp_conflict_qset_oper_info_t **cqinfo,
                               uint8_t *first_blk_id,
                               uint32_t *first_blk_offset)
{
    bool found_blk_id = FALSE;
    void *ha_ptr = NULL;
    void *backup_ha_ptr = NULL;
    uint8_t blk_id = BCMFP_HA_BLK_ID_INVALID;
    uint8_t backup_blk_id = BCMFP_HA_BLK_ID_INVALID;
    uint32_t idx = 0;
    uint8_t first_free_blk_id = BCMFP_HA_BLK_ID_INVALID;
    uint32_t blk_size = 0;
    uint32_t blk_size_required = 0;
    uint32_t ha_hdr_size = 0;
    bcmfp_ha_blk_hdr_t *blk_hdr = NULL;
    bcmfp_ha_blk_hdr_t *backup_blk_hdr = NULL;
    bcmfp_ha_root_blk_t *root = NULL;
    bcmfp_conflict_qset_oper_info_t *cqset_info = NULL;
    uint32_t num_cqset_info = 0;
    uint32_t cqset_info_size = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(cqinfo, SHR_E_PARAM);
    SHR_NULL_CHECK(first_blk_id, SHR_E_PARAM);
    SHR_NULL_CHECK(first_blk_offset, SHR_E_PARAM);

    for (blk_id = BCMFP_HA_BLK_ID_DYNAMIC_MAX;
         blk_id >= BCMFP_HA_BLK_ID_DYNAMIC_MIN;
         blk_id--) {
         ha_ptr = NULL;
         (void)bcmfp_ha_mem_get(unit, blk_id, &ha_ptr);
         if (ha_ptr == NULL) {
            first_free_blk_id = blk_id;
            found_blk_id = TRUE;
            break;
         }
         blk_hdr = (bcmfp_ha_blk_hdr_t *)ha_ptr;

         if (blk_hdr->backup_blk_id == BCMFP_HA_BLK_ID_INVALID) {
             continue;
         }

         /* Skip HA blocks of different type. */
         if (blk_hdr->blk_type != BCMFP_HA_BLK_TYPE_CONFLICT_QSET_INFO) {
             continue;
         }

         /*! No free HA elements in this HA block. */
         if (blk_hdr->free_blk_offset == BCMFP_HA_BLK_OFFSET_INVALID) {
             continue;
         }
         found_blk_id = TRUE;
         break;
    }

    /* There is no enough room and no block ids left over to use. */
    if ((found_blk_id == FALSE)) {
        
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }

    /* There is no enough room and free block ids are there then
     * create a new HA block.
     */
    SHR_IF_ERR_VERBOSE_EXIT(bcmfp_ha_root_blk_get(unit, &root));
    if (first_free_blk_id != BCMFP_HA_BLK_ID_INVALID) {
        num_cqset_info = BCMFP_HA_CONFLICT_QSET_INFO_BLK_ELEMENT_COUNT;
        ha_hdr_size = sizeof(bcmfp_ha_blk_hdr_t);
        cqset_info_size = sizeof(bcmfp_conflict_qset_oper_info_t);
        blk_size = (num_cqset_info * cqset_info_size);
        blk_size += ha_hdr_size;
        blk_id = first_free_blk_id;
        blk_size_required = blk_size;
        /* Create the HA block */
        ha_ptr = shr_ha_mem_alloc(unit,
                                  BCMMGMT_FP_COMP_ID,
                                  blk_id,
                                  "bcmfpPddActionsInfoBlock",
                                  &blk_size);
        SHR_NULL_CHECK(ha_ptr, SHR_E_MEMORY);
        if (blk_size < blk_size_required) {
            SHR_IF_ERR_VERBOSE_EXIT
                (shr_ha_mem_free(unit, ha_ptr));
            blk_size = blk_size_required;
            ha_ptr = shr_ha_mem_alloc(unit,
                                BCMMGMT_FP_COMP_ID,
                                blk_id,
                                "bcmfpPddActionsInfoBlock",
                                &blk_size);
            SHR_NULL_CHECK(ha_ptr, SHR_E_MEMORY);
        }
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_ha_mem_set(unit, blk_id, ha_ptr));
        sal_memset(ha_ptr, 0, blk_size);
        SHR_BITSET(root->in_use_ha_blk_id_bmp.w, blk_id);
        /* Initialize the block header. */
        blk_hdr = (bcmfp_ha_blk_hdr_t *)ha_ptr;
        blk_hdr->free_blk_offset = 0;
        blk_hdr->free_ha_element_count = num_cqset_info;
        blk_hdr->blk_size = blk_size;
        blk_hdr->backup_blk_id = BCMFP_HA_BLK_ID_INVALID;
        blk_hdr->trans_state = BCMFP_TRANS_STATE_IDLE;
        blk_hdr->signature = BCMFP_HA_STRUCT_SIGN + blk_id;
        blk_hdr->blk_type = BCMFP_HA_BLK_TYPE_CONFLICT_QSET_INFO;
        cqset_info = (bcmfp_conflict_qset_oper_info_t *)
                       ((uint8_t *)ha_ptr + ha_hdr_size);
        for (idx = 0; idx < num_cqset_info; idx++) {
            cqset_info[idx].next_conflict_qset_oper_info.blk_id = blk_id;
            cqset_info[idx].next_conflict_qset_oper_info.blk_offset = (idx + 1);
        }
        cqset_info[num_cqset_info - 1].next_conflict_qset_oper_info.blk_id = blk_id;
        cqset_info[num_cqset_info - 1].next_conflict_qset_oper_info.blk_offset =
                                                BCMFP_HA_BLK_OFFSET_INVALID;

        /* Create the back up block Id. */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_ha_backup_blk_id_get(unit, blk_id, &backup_blk_id));
        blk_hdr->backup_blk_id = backup_blk_id;
        backup_ha_ptr = shr_ha_mem_alloc(unit,
                                         BCMMGMT_FP_COMP_ID,
                                         backup_blk_id,
                                         "bcmfpPddActionsInfoBkupBlock",
                                         &blk_size);
        SHR_NULL_CHECK(backup_ha_ptr, SHR_E_MEMORY);
        if (blk_size < blk_size_required) {
            SHR_IF_ERR_VERBOSE_EXIT
                (shr_ha_mem_free(unit, backup_ha_ptr));
            blk_size = blk_size_required;
            backup_ha_ptr = shr_ha_mem_alloc(unit,
                                       BCMMGMT_FP_COMP_ID,
                                       backup_blk_id,
                                       "bcmfpPddActionsInfoBkupBlock",
                                       &blk_size);
            SHR_NULL_CHECK(backup_ha_ptr, SHR_E_MEMORY);
        }
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_ha_mem_set(unit, backup_blk_id, backup_ha_ptr));
        backup_blk_hdr =
            (bcmfp_ha_blk_hdr_t *)backup_ha_ptr;
        SHR_BITSET(root->in_use_ha_blk_id_bmp.w, backup_blk_id);
        /* Copy the Active block to backup block. */
        sal_memcpy(backup_ha_ptr, ha_ptr, blk_size);
        backup_blk_hdr->signature =
                        (BCMFP_HA_STRUCT_SIGN + backup_blk_id);
        backup_blk_hdr->backup_blk_id = BCMFP_HA_BLK_ID_INVALID;
    }

    cqset_info = (bcmfp_conflict_qset_oper_info_t *)
                 ((uint8_t *)ha_ptr +
                  sizeof(bcmfp_ha_blk_hdr_t) +
                  (sizeof(bcmfp_conflict_qset_oper_info_t) *
                  blk_hdr->free_blk_offset));

    *first_blk_id = blk_id;
    *first_blk_offset = blk_hdr->free_blk_offset;
    *cqinfo = cqset_info;

    blk_hdr->free_blk_offset =
             cqset_info->next_conflict_qset_oper_info.blk_offset;
    cqset_info->next_conflict_qset_oper_info.blk_offset =
                                    BCMFP_HA_BLK_OFFSET_INVALID;
    blk_hdr->free_ha_element_count--;
    SHR_IF_ERR_EXIT
        (bcmfp_trans_state_set(unit, blk_id, BCMFP_TRANS_STATE_UC_A));
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_conflict_qset_oper_info_set(int unit,
                                  bcmfp_stage_id_t stage_id,
                                  bcmfp_group_id_t group_id,
                                  bcmfp_conflict_qset_id_t cqset_id,
                                  bcmfp_ext_codes_t *ext_codes)
{
    uint8_t blk_id = BCMFP_HA_BLK_ID_INVALID;
    uint32_t blk_offset = BCMFP_HA_BLK_OFFSET_INVALID;
    bcmfp_conflict_qset_oper_info_t *cqset_opinfo = NULL;
    bcmfp_group_oper_info_t *gopinfo = NULL;
    size_t size = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmfp_group_oper_info_get(unit, stage_id, group_id, &gopinfo));

    SHR_IF_ERR_EXIT
        (bcmfp_conflict_qset_oper_info_acquire(unit,
                                       &cqset_opinfo,
                                       &blk_id,
                                       &blk_offset));
    cqset_opinfo->conflict_qset_id = cqset_id;
    size = (sizeof(bcmfp_ext_codes_t) * BCMFP_ENTRY_PARTS_MAX);
    sal_memcpy(cqset_opinfo->ext_codes, ext_codes, size);
    cqset_opinfo->next_conflict_qset_oper_info.blk_id =
                  gopinfo->conflict_qset_oper_info.blk_id;
    cqset_opinfo->next_conflict_qset_oper_info.blk_offset =
                  gopinfo->conflict_qset_oper_info.blk_offset;
    gopinfo->conflict_qset_oper_info.blk_id = blk_id;
    gopinfo->conflict_qset_oper_info.blk_offset = blk_offset;

    BCMFP_HA_STAGE_ID_TO_BLK_ID(stage_id, blk_id);
    SHR_IF_ERR_EXIT
        (bcmfp_trans_state_set(unit, blk_id, BCMFP_TRANS_STATE_UC_A));
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_conflict_qset_oper_info_get(int unit,
                                  uint8_t blk_id,
                                  uint32_t blk_offset,
                                  bcmfp_conflict_qset_oper_info_t **cqset_opinfo)
{
    void *ha_ptr = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(cqset_opinfo, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (bcmfp_ha_mem_get(unit, blk_id, &ha_ptr));

    *cqset_opinfo = (bcmfp_conflict_qset_oper_info_t *)((uint8_t *)ha_ptr +
                    (sizeof(bcmfp_ha_blk_hdr_t)) +
                    (sizeof(bcmfp_conflict_qset_oper_info_t) * blk_offset));
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_conflict_qset_ext_codes_get(int unit,
                                  bcmfp_stage_id_t stage_id,
                                  bcmfp_group_id_t group_id,
                                  bcmfp_conflict_qset_id_t cqset_id,
                                  bcmfp_ext_codes_t **ext_codes)
{
    uint8_t blk_id = BCMFP_HA_BLK_ID_INVALID;
    uint32_t blk_offset = BCMFP_HA_BLK_OFFSET_INVALID;
    bcmfp_conflict_qset_oper_info_t *cqset_opinfo = NULL;
    bcmfp_group_oper_info_t *gopinfo = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(ext_codes, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (bcmfp_group_oper_info_get(unit, stage_id, group_id, &gopinfo));

    *ext_codes = NULL;
    blk_id = gopinfo->conflict_qset_oper_info.blk_id;
    blk_offset = gopinfo->conflict_qset_oper_info.blk_offset;

    while (blk_id != BCMFP_HA_BLK_ID_INVALID &&
           blk_offset != BCMFP_HA_BLK_OFFSET_INVALID) {
        SHR_IF_ERR_EXIT
            (bcmfp_conflict_qset_oper_info_get(unit,
                                               blk_id,
                                               blk_offset,
                                               &cqset_opinfo));
        if (cqset_opinfo->conflict_qset_id == cqset_id) {
            *ext_codes = cqset_opinfo->ext_codes;
            break;
        }
        blk_id = cqset_opinfo->next_conflict_qset_oper_info.blk_id;
        blk_offset = cqset_opinfo->next_conflict_qset_oper_info.blk_offset;
    }

    if (*ext_codes == NULL) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_conflict_qset_oper_info_release(int unit,
                                      bcmfp_stage_id_t stage_id,
                                      uint8_t blk_id,
                                      uint32_t blk_offset)
{
    bcmfp_ha_blk_hdr_t *blk_hdr = NULL;
    bcmfp_conflict_qset_oper_info_t *cqset_opinfo = NULL;
    uint8_t part = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmfp_ha_blk_hdr_get(unit, blk_id, &blk_hdr));

    SHR_IF_ERR_EXIT
        (bcmfp_conflict_qset_oper_info_get(unit,
                                           blk_id,
                                           blk_offset,
                                           &cqset_opinfo));
    sal_memset(cqset_opinfo, 0, sizeof(bcmfp_conflict_qset_oper_info_t));
    for (part = 0; part < BCMFP_ENTRY_PARTS_MAX; part++) {
        bcmfp_ext_codes_init(unit, &(cqset_opinfo->ext_codes[part]));
    }

    cqset_opinfo->next_conflict_qset_oper_info.blk_id = blk_id;
    cqset_opinfo->next_conflict_qset_oper_info.blk_offset =
                  blk_hdr->free_blk_offset;
    blk_hdr->free_blk_offset = blk_offset;

    BCMFP_HA_STAGE_ID_TO_BLK_ID(stage_id, blk_id);
    SHR_IF_ERR_EXIT
        (bcmfp_trans_state_set(unit, blk_id, BCMFP_TRANS_STATE_UC_A));
exit:
    SHR_FUNC_EXIT();
}

