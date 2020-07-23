/*! \file bcmfp_pdd_oper.c
 *
 * BCMFP PDD operational information init/get/set APIs.
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
#include <bcmfp/bcmfp_pdd_internal.h>
#include <shr/shr_util.h>
#include <bcmlrd/bcmlrd_map.h>
#include <bcmfp/generated/bcmfp_ha.h>
#include <bcmmgmt/bcmmgmt_sysm.h>

#define BSL_LOG_MODULE BSL_LS_BCMFP_DEV

STATIC int
bcmfp_pdd_actions_info_acquire(int unit,
                               bcmfp_pdd_actions_info_t **ainfo,
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
    bcmfp_pdd_actions_info_t *actions_info = NULL;
    uint32_t num_actions_info = 0;
    uint32_t actions_info_size = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(ainfo, SHR_E_PARAM);
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
         if (blk_hdr->blk_type != BCMFP_HA_BLK_TYPE_PDD_ACTIONS_INFO) {
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
        num_actions_info = BCMFP_HA_PDD_ACTION_INFO_BLK_ELEMENT_COUNT;
        ha_hdr_size = sizeof(bcmfp_ha_blk_hdr_t);
        actions_info_size = sizeof(bcmfp_pdd_actions_info_t);
        blk_size = (num_actions_info * actions_info_size);
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
        blk_hdr->free_blk_offset = ha_hdr_size;
        blk_hdr->free_ha_element_count = num_actions_info;
        blk_hdr->blk_size = blk_size;
        blk_hdr->backup_blk_id = BCMFP_HA_BLK_ID_INVALID;
        blk_hdr->trans_state = BCMFP_TRANS_STATE_IDLE;
        blk_hdr->signature = BCMFP_HA_STRUCT_SIGN + blk_id;
        blk_hdr->blk_type = BCMFP_HA_BLK_TYPE_PDD_ACTIONS_INFO;
        actions_info = (bcmfp_pdd_actions_info_t *)
                       ((uint8_t *)ha_ptr + ha_hdr_size);
        for (idx = 0; idx < num_actions_info; idx++) {
            actions_info[idx].bitmap_info.blk_id =
                              BCMFP_HA_BLK_ID_INVALID;
            actions_info[idx].bitmap_info.blk_offset =
                              BCMFP_HA_BLK_OFFSET_INVALID;
            actions_info[idx].offset_info.blk_id =
                              BCMFP_HA_BLK_ID_INVALID;
            actions_info[idx].offset_info.blk_offset =
                              BCMFP_HA_BLK_OFFSET_INVALID;
            actions_info[idx].section_offset.blk_id =
                              BCMFP_HA_BLK_ID_INVALID;
            actions_info[idx].section_offset.blk_offset =
                              BCMFP_HA_BLK_OFFSET_INVALID;
            actions_info[idx].num_parts = 0;
            actions_info[idx].policy_width = 0;
            actions_info[idx].pdd_mode = BCMFP_GROUP_MODE_NONE;
            actions_info[idx].pdd_type = BCMFP_PDD_OPER_TYPE_NONE;
            actions_info[idx].next_actions_info.blk_id = blk_id;
            actions_info[idx].next_actions_info.blk_offset =
                       (((idx + 1) * actions_info_size) + ha_hdr_size);
        }
        actions_info[num_actions_info - 1].next_actions_info.blk_id = blk_id;
        actions_info[num_actions_info - 1].next_actions_info.blk_offset =
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

    actions_info = (bcmfp_pdd_actions_info_t *)
                   ((uint8_t *)ha_ptr + blk_hdr->free_blk_offset);

    *first_blk_id = blk_id;
    *first_blk_offset = blk_hdr->free_blk_offset;
    *ainfo = actions_info;

    blk_hdr->free_blk_offset =
             actions_info->next_actions_info.blk_offset;
    actions_info->next_actions_info.blk_offset =
                                    BCMFP_HA_BLK_OFFSET_INVALID;
    blk_hdr->free_ha_element_count--;
    SHR_IF_ERR_EXIT
        (bcmfp_trans_state_set(unit, blk_id, BCMFP_TRANS_STATE_UC_A));
exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Get actions offset(s) and width(s) belongs
 *  to a partucular part. Caller will provide chain of HA
 *  elements(blk_id and blk_offset corresponds to first HA
 *  element in the chain) holding actions offset(s) and
 *  width(s) information in all parts.
 *
 * \param [in] unit Logical device id
 * \param [in] blk_id HA block id of first HA element in the chain.
 * \param [in] blk_offset HA block offset of first HA element in the chain.
 * \param [in] part_id First/Second/Third Slice.
 * \param [out] a_offset Action offset(s) and width(s) in part_id.
 *
 * \retval SHR_E_NONE Success, otherwise failure
 */
STATIC int
bcmfp_action_oper_info_get(int unit,
                           uint8_t blk_id,
                           uint32_t blk_offset,
                           bcmfp_part_id_t part_id,
                           bcmfp_action_oper_info_t *a_offset)
{
    bool hit_status = FALSE;
    uint16_t offset_id = 0;
    uint16_t max_offset_id = 0;
    bcmfp_part_id_t part = 0;
    bcmfp_generic_data_t *ha_element = NULL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(a_offset, SHR_E_PARAM);

    sal_memset(a_offset, 0, sizeof(bcmfp_action_oper_info_t));
    /* Loop through the chain. */
    while (blk_id != BCMFP_HA_BLK_ID_INVALID &&
           blk_offset != BCMFP_HA_BLK_OFFSET_INVALID) {
        ha_element = NULL;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_ha_element_get(unit,
                                  blk_id,
                                  blk_offset,
                                  &ha_element));

        /* HA element with type "ACTION_OFFSET_WIDTH_PART_ID" has
         * following 4 different fields embedded in its value.
         * 1. b0  - b1  --> part id
         * 2. b2  - b11 --> offset
         * 3. b12 - b21 --> width
         * 4. b22 - b31 --> offset id
         */
        part = ha_element->value & 0x3;
        if (part == part_id) {
            hit_status = TRUE;
            offset_id = (ha_element->value >> 22) & 0x3FF;
            if (max_offset_id < offset_id) {
                max_offset_id = offset_id;
            }
            a_offset->offset[offset_id] =
                 (ha_element->value >> 2) & 0x3FF;
            a_offset->width[offset_id] =
                 (ha_element->value >> 12) & 0x3FF;
            a_offset->num_offsets = max_offset_id + 1;
            if (a_offset->width[offset_id] != 0) {
                LOG_VERBOSE(BSL_LOG_MODULE,(BSL_META_U(unit,
                        "offset_id=%d, offset=%d, width=%u\n"),
                        offset_id,
                        a_offset->offset[offset_id],
                        a_offset->width[offset_id]));
            }
        }
        blk_id = ha_element->next_blk_id;
        blk_offset = ha_element->next_blk_offset;
    }
    if (hit_status == FALSE) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_pdd_action_offset_info_set(int unit,
                        bcmfp_stage_id_t stage_id,
                        bcmfp_pdd_id_t pdd_id,
                        bcmfp_part_id_t part_id,
                        bcmfp_pdd_oper_type_t type,
                        bcmfp_action_oper_info_t *a_offset)
{
    uint16_t idx = 0;
    uint8_t blk_id = 0;
    uint8_t fid_offset_info_blk_id = 0;
    uint8_t action_offset_blk_id = 0;
    uint32_t blk_offset = 0;
    uint32_t fid_offset_info_blk_offset = 0;
    uint32_t action_offset_blk_offset = 0;
    uint32_t value = 0;
    uint16_t num_ha_elements = 0;
    bcmfp_generic_data_t *ha_element = NULL;
    bcmfp_generic_data_t *action_offset_ha_element = NULL;
    bcmfp_pdd_actions_info_t *actions_info = NULL;
    bcmfp_ha_blk_info_t *ha_blk_info = NULL;
    bcmfp_fid_offset_info_t *fid_offset_info = NULL;
    bool fid_offset_info_found = FALSE;
    bcmfp_stage_t *stage = NULL;
    bcmltd_sid_t sid = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(a_offset, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));
    sid = stage->tbls.pdd_tbl->sid;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_pdd_actions_info_get(unit,
                                    stage_id,
                                    pdd_id,
                                    type,
                                    &actions_info));
    ha_blk_info = &(actions_info->offset_info);
    blk_id = ha_blk_info->blk_id;
    blk_offset = ha_blk_info->blk_offset;

    /*
     * Check if offset(s) and width(s) of action is already
     * present in pdd operational information. This function
     * being called multiple times if pdd is multi wide and
     * action is present in multiple slices.
     */
    while ((blk_id != BCMFP_HA_BLK_ID_INVALID) &&
           (blk_offset != BCMFP_HA_BLK_OFFSET_INVALID)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_fid_offset_info_get(unit,
                                       blk_id,
                                       blk_offset,
                                       &fid_offset_info));
        if (fid_offset_info->fid == a_offset->action) {
            fid_offset_info_found = TRUE;
            break;
        }
        blk_id = fid_offset_info->next_blk_id;
        blk_offset = fid_offset_info->next_blk_offset;
    }

    /*
     * Caliculate the number of HA elements required to save
     * actions offset(s) and width(s) information.
     */
    num_ha_elements = 0;
    for (idx = 0; idx < a_offset->num_offsets; idx++) {
        if (a_offset->width[idx] == 0) {
            continue;
        }
        num_ha_elements++;
    }

    /*
     * Allocate required number of HA elements from HA blocks
     * created in run time.
     */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_ha_elements_acquire(unit,
                                   num_ha_elements,
                                   BCMFP_HA_BLK_BUCKET_SIZE_ONE,
                                   &action_offset_ha_element,
                                   &action_offset_blk_id,
                                   &action_offset_blk_offset));

    /*
     * First time offset info of this action is pushed to
     * pdd oper info.
     */
    if (fid_offset_info_found == FALSE) {
        fid_offset_info = NULL;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_fid_offset_infos_acquire(unit,
                                            1,
                                            &fid_offset_info,
                                            &fid_offset_info_blk_id,
                                            &fid_offset_info_blk_offset));
        fid_offset_info->sid = sid;
        fid_offset_info->fid = a_offset->action;
        fid_offset_info->fidx = 0;
        fid_offset_info->offset_info.blk_id = action_offset_blk_id;
        fid_offset_info->offset_info.blk_offset = action_offset_blk_offset;

        fid_offset_info->next_blk_id =
                         actions_info->offset_info.blk_id;
        fid_offset_info->next_blk_offset =
                         actions_info->offset_info.blk_offset;
        actions_info->offset_info.blk_id = fid_offset_info_blk_id;
        actions_info->offset_info.blk_offset = fid_offset_info_blk_offset;
    } else {
        /*
         * Add the new offset(s) and width(s) chain to the end of
         * Type-2 HA element chain of the action.
         */
        blk_id = fid_offset_info->offset_info.blk_id;
        blk_offset = fid_offset_info->offset_info.blk_offset;
        while (blk_id != BCMFP_HA_BLK_ID_INVALID &&
               blk_offset != BCMFP_HA_BLK_OFFSET_INVALID) {
            ha_element = NULL;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_ha_element_get(unit,
                                      blk_id,
                                      blk_offset,
                                      &ha_element));
            blk_id = ha_element->next_blk_id;
            blk_offset = ha_element->next_blk_offset;
        }
        if (ha_element != NULL) {
            ha_element->next_blk_id = action_offset_blk_id;
            ha_element->next_blk_offset = action_offset_blk_offset;
        } else {
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
    }

    /*
     * All the HA element chains are linked properly. Now update the
     * Type-2 HA element values.
     */
    blk_id = action_offset_blk_id;
    blk_offset = action_offset_blk_offset;
    for (idx = 0; idx < a_offset->num_offsets; idx++) {
        if (a_offset->width[idx] == 0) {
            continue;
        }
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_ha_element_get(unit,
                                  blk_id,
                                  blk_offset,
                                  &action_offset_ha_element));
        value = part_id;
        value |= (a_offset->offset[idx] << 2);
        value |= (a_offset->width[idx] << 12);
        value |= (idx << 22);
        action_offset_ha_element->value = value;
        blk_id = action_offset_ha_element->next_blk_id;
        blk_offset = action_offset_ha_element->next_blk_offset;
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_pdd_action_offset_info_reset(int unit,
                                   bcmfp_stage_id_t stage_id,
                                   bcmfp_pdd_id_t pdd_id,
                                   bcmfp_pdd_oper_type_t type)
{
    uint8_t blk_id = BCMFP_HA_BLK_ID_INVALID;
    uint8_t a_offset_blk_id = BCMFP_HA_BLK_ID_INVALID;
    uint32_t blk_offset = BCMFP_HA_BLK_OFFSET_INVALID;
    uint32_t a_offset_blk_offset = BCMFP_HA_BLK_OFFSET_INVALID;
    bcmfp_pdd_actions_info_t *actions_info = NULL;
    bcmfp_ha_blk_info_t *ha_blk_info = NULL;
    bcmfp_fid_offset_info_t *fid_offset_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_pdd_actions_info_get(unit,
                                    stage_id,
                                    pdd_id,
                                    type,
                                    &actions_info));
    ha_blk_info = &(actions_info->offset_info);
    blk_id = ha_blk_info->blk_id;
    blk_offset = ha_blk_info->blk_offset;
    if (blk_id == BCMFP_HA_BLK_ID_INVALID &&
        blk_offset == BCMFP_HA_BLK_OFFSET_INVALID) {
        SHR_EXIT();
    }
    /* First release Type-2 HA element chains of the PDD. */
    while ((blk_id != BCMFP_HA_BLK_ID_INVALID) &&
           (blk_offset != BCMFP_HA_BLK_OFFSET_INVALID)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_fid_offset_info_get(unit,
                                       blk_id,
                                       blk_offset,
                                       &fid_offset_info));

        a_offset_blk_id = fid_offset_info->offset_info.blk_id;
        a_offset_blk_offset = fid_offset_info->offset_info.blk_offset;

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_ha_elements_release(unit,
                                       a_offset_blk_id,
                                       a_offset_blk_offset));
        blk_id = fid_offset_info->next_blk_id;
        blk_offset = fid_offset_info->next_blk_offset;

    }
    /* Then release Type-1 HA element chain of the PDD. */
    blk_id = ha_blk_info->blk_id;
    blk_offset = ha_blk_info->blk_offset;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_fid_offset_infos_release(unit, blk_id, blk_offset));
    ha_blk_info->blk_id = BCMFP_HA_BLK_ID_INVALID;
    ha_blk_info->blk_offset = BCMFP_HA_BLK_OFFSET_INVALID;
exit:
    SHR_FUNC_EXIT();
}

static int
bcmfp_pdd_data_width_set(int unit,
                     bcmfp_stage_id_t stage_id,
                     bcmfp_pdd_id_t pdd_id,
                     bcmfp_part_id_t part_id,
                     bcmfp_pdd_oper_type_t type,
                     uint32_t data_width)
{
    uint8_t num_ha_elements = 0;
    uint8_t blk_id = BCMFP_HA_BLK_ID_INVALID;
    uint8_t first_blk_id = BCMFP_HA_BLK_ID_INVALID;
    uint32_t blk_offset = BCMFP_HA_BLK_OFFSET_INVALID;
    uint32_t first_blk_offset = BCMFP_HA_BLK_OFFSET_INVALID;
    bcmfp_generic_data_t *ha_element = NULL;
    bcmfp_pdd_actions_info_t *actions_info = NULL;
    bcmfp_stage_t *stage = NULL;
    bcmfp_ha_blk_info_t *ha_blk_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_pdd_actions_info_get(unit,
                                    stage_id,
                                    pdd_id,
                                    type,
                                    &actions_info));

    ha_blk_info = &(actions_info->data_width_info);

    /* Number of uint32 words required to save data width. */
    num_ha_elements = 1;
    /* Add one to save the part. */
    num_ha_elements++;
    /* Get required number of HA elements. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_ha_elements_acquire(unit,
                                   num_ha_elements,
                                   BCMFP_HA_BLK_BUCKET_SIZE_ONE,
                                   &ha_element,
                                   &first_blk_id,
                                   &first_blk_offset));
    blk_id = first_blk_id;
    blk_offset = first_blk_offset;

    /* Save the part number in first HA block */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_ha_element_get(unit, blk_id, blk_offset, &ha_element));
    ha_element->value = (part_id & 0x7);

    /* Save the data size. */
    blk_id = ha_element->next_blk_id;
    blk_offset = ha_element->next_blk_offset;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_ha_element_get(unit, blk_id, blk_offset, &ha_element));
    ha_element->value = data_width;

    ha_element->next_blk_id = ha_blk_info->blk_id;
    ha_element->next_blk_offset = ha_blk_info->blk_offset;
    ha_blk_info->blk_id = first_blk_id;
    ha_blk_info->blk_offset = first_blk_offset;
exit:
    SHR_FUNC_EXIT();
}

static int
bcmfp_pdd_data_width_reset(int unit,
                       bcmfp_stage_id_t stage_id,
                       bcmfp_pdd_id_t pdd_id,
                       bcmfp_pdd_oper_type_t type)
{
    uint8_t blk_id = BCMFP_HA_BLK_ID_INVALID;
    uint32_t blk_offset = BCMFP_HA_BLK_OFFSET_INVALID;
    bcmfp_pdd_actions_info_t *actions_info = NULL;
    bcmfp_ha_blk_info_t *ha_blk_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_pdd_actions_info_get(unit,
                                    stage_id,
                                    pdd_id,
                                    type,
                                    &actions_info));
    ha_blk_info = &(actions_info->data_width_info);
    blk_id = ha_blk_info->blk_id;
    blk_offset = ha_blk_info->blk_offset;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_ha_elements_release(unit,
                                   blk_id,
                                   blk_offset));

    ha_blk_info->blk_id = BCMFP_HA_BLK_ID_INVALID;
    ha_blk_info->blk_offset = BCMFP_HA_BLK_OFFSET_INVALID;
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_pdd_bitmap_set(int unit,
                     bcmfp_stage_id_t stage_id,
                     bcmfp_pdd_id_t pdd_id,
                     bcmfp_part_id_t part_id,
                     bcmfp_pdd_oper_type_t type,
                     uint32_t *pdd_bitmap)
{
    uint8_t idx = 0;
    uint8_t num_ha_elements = 0;
    uint8_t blk_id = BCMFP_HA_BLK_ID_INVALID;
    uint8_t first_blk_id = BCMFP_HA_BLK_ID_INVALID;
    uint32_t blk_offset = BCMFP_HA_BLK_OFFSET_INVALID;
    uint32_t first_blk_offset = BCMFP_HA_BLK_OFFSET_INVALID;
    bcmfp_generic_data_t *ha_element = NULL;
    bcmfp_pdd_actions_info_t *actions_info = NULL;
    bcmfp_stage_t *stage = NULL;
    bcmfp_ha_blk_info_t *ha_blk_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(pdd_bitmap, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_pdd_actions_info_get(unit,
                                    stage_id,
                                    pdd_id,
                                    type,
                                    &actions_info));

    ha_blk_info = &(actions_info->bitmap_info);

    /* Number of uint32 words required to save PDD bitmap. */
    num_ha_elements =
        ((stage->pdd_hw_info->pdd_bitmap_size / 32) + 1);
    /* Add one to save the part. */
    num_ha_elements++;
    /* Get required number of HA elements. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_ha_elements_acquire(unit,
                                   num_ha_elements,
                                   BCMFP_HA_BLK_BUCKET_SIZE_ONE,
                                   &ha_element,
                                   &first_blk_id,
                                   &first_blk_offset));
    blk_id = first_blk_id;
    blk_offset = first_blk_offset;
    /* Save the each word of PDD bitmap in one HA element. */
    for (idx = 0; idx < num_ha_elements; idx ++) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_ha_element_get(unit,
                                  blk_id,
                                  blk_offset,
                                  &ha_element));
        if (idx) {
            ha_element->value = pdd_bitmap[idx - 1];
        } else {
            /*
             * First three bits for part and next 5 bits
             * for number of HA elements required for PDD
             * bitmap.
             */
            ha_element->value = (part_id & 0x7) |
                       (((num_ha_elements - 1) << 3) & 0xF8);
        }
        blk_id = ha_element->next_blk_id;
        blk_offset = ha_element->next_blk_offset;
    }
    ha_element->next_blk_id = ha_blk_info->blk_id;
    ha_element->next_blk_offset = ha_blk_info->blk_offset;
    ha_blk_info->blk_id = first_blk_id;
    ha_blk_info->blk_offset = first_blk_offset;
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_pdd_bitmap_reset(int unit,
                       bcmfp_stage_id_t stage_id,
                       bcmfp_pdd_id_t pdd_id,
                       bcmfp_pdd_oper_type_t type)
{
    uint8_t blk_id = BCMFP_HA_BLK_ID_INVALID;
    uint32_t blk_offset = BCMFP_HA_BLK_OFFSET_INVALID;
    bcmfp_pdd_actions_info_t *actions_info = NULL;
    bcmfp_ha_blk_info_t *ha_blk_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_pdd_actions_info_get(unit,
                                    stage_id,
                                    pdd_id,
                                    type,
                                    &actions_info));
    ha_blk_info = &(actions_info->bitmap_info);
    blk_id = ha_blk_info->blk_id;
    blk_offset = ha_blk_info->blk_offset;

    if (blk_id != BCMFP_HA_BLK_ID_INVALID &&
        blk_offset != BCMFP_HA_BLK_OFFSET_INVALID) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_ha_elements_release(unit,
                                       blk_id,
                                       blk_offset));
    }
    ha_blk_info->blk_id = BCMFP_HA_BLK_ID_INVALID;
    ha_blk_info->blk_offset = BCMFP_HA_BLK_OFFSET_INVALID;
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_pdd_section_info_set(int unit,
                           bcmfp_stage_id_t stage_id,
                           bcmfp_pdd_id_t pdd_id,
                           bcmfp_part_id_t part_id,
                           bcmfp_pdd_oper_type_t type,
                           bcmfp_pdd_section_info_t *sinfo)
{
    uint8_t idx = 0;
    uint8_t blk_id = BCMFP_HA_BLK_ID_INVALID;
    uint32_t blk_offset = BCMFP_HA_BLK_OFFSET_INVALID;
    bcmfp_pdd_actions_info_t *actions_info = NULL;
    bcmfp_ha_blk_info_t *ha_blk_info = NULL;
    bcmfp_generic_data_t *ha_element = NULL;
    uint8_t first_blk_id = BCMFP_HA_BLK_ID_INVALID;
    uint32_t first_blk_offset = BCMFP_HA_BLK_OFFSET_INVALID;
    uint16_t num_ha_elements = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(sinfo, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_pdd_actions_info_get(unit,
                                    stage_id,
                                    pdd_id,
                                    type,
                                    &actions_info));
    ha_blk_info = &(actions_info->section_offset);
    num_ha_elements =  sinfo->num_sections;

    if (num_ha_elements == 0) {
        ha_blk_info->blk_id = BCMFP_HA_BLK_ID_INVALID;
        ha_blk_info->blk_offset = BCMFP_HA_BLK_OFFSET_INVALID;
        SHR_EXIT();
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_ha_elements_acquire(unit,
                                   num_ha_elements,
                                   BCMFP_HA_BLK_BUCKET_SIZE_ONE,
                                   &ha_element,
                                   &first_blk_id,
                                   &first_blk_offset));
    blk_id = first_blk_id;
    blk_offset = first_blk_offset;
    for (idx = 0; idx < num_ha_elements; idx++) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_ha_element_get(unit,
                                  blk_id,
                                  blk_offset,
                                  &ha_element));
        ha_element->value = (part_id & 0xF);
        ha_element->value |= ((sinfo->section_id[idx]  & 0xFF) << 4);
        ha_element->value |= ((sinfo->section_offset[idx] & 0xFFF) << 12);
        blk_id = ha_element->next_blk_id;
        blk_offset = ha_element->next_blk_offset;
    }
    ha_blk_info->blk_id = first_blk_id;
    ha_blk_info->blk_offset = first_blk_offset;
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_pdd_section_info_reset(int unit,
                             bcmfp_stage_id_t stage_id,
                             bcmfp_pdd_id_t pdd_id,
                             bcmfp_pdd_oper_type_t type)
{
    uint8_t blk_id = BCMFP_HA_BLK_ID_INVALID;
    uint32_t blk_offset = BCMFP_HA_BLK_OFFSET_INVALID;
    bcmfp_pdd_actions_info_t *actions_info = NULL;
    bcmfp_ha_blk_info_t *ha_blk_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_pdd_actions_info_get(unit,
                                    stage_id,
                                    pdd_id,
                                    type,
                                    &actions_info));
    ha_blk_info = &(actions_info->section_offset);
    blk_id = ha_blk_info->blk_id;
    blk_offset = ha_blk_info->blk_offset;

    if (blk_id == BCMFP_HA_BLK_ID_INVALID &&
        blk_offset == BCMFP_HA_BLK_OFFSET_INVALID) {
        SHR_EXIT();
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_ha_elements_release(unit, blk_id, blk_offset));

    ha_blk_info->blk_id = BCMFP_HA_BLK_ID_INVALID;
    ha_blk_info->blk_offset = BCMFP_HA_BLK_OFFSET_INVALID;
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_pdd_group_mode_set(int unit,
                         bcmfp_stage_id_t stage_id,
                         bcmfp_pdd_id_t pdd_id,
                         bcmfp_pdd_oper_type_t type,
                         bcmfp_group_mode_t pdd_mode)
{
    bcmfp_pdd_actions_info_t *actions_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_pdd_actions_info_get(unit,
                                    stage_id,
                                    pdd_id,
                                    type,
                                    &actions_info));

    actions_info->pdd_mode = pdd_mode;
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_pdd_group_mode_get(int unit,
                         bcmfp_stage_id_t stage_id,
                         bcmfp_pdd_id_t pdd_id,
                         bcmfp_pdd_oper_type_t type,
                         bcmfp_group_mode_t *pdd_mode)
{
    bcmfp_pdd_actions_info_t *actions_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(pdd_mode, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_pdd_actions_info_get(unit,
                                    stage_id,
                                    pdd_id,
                                    type,
                                    &actions_info));
    *pdd_mode = actions_info->pdd_mode;
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_pdd_num_parts_set(int unit,
                        bcmfp_stage_id_t stage_id,
                        bcmfp_pdd_id_t pdd_id,
                        bcmfp_pdd_oper_type_t type,
                        uint8_t num_parts)
{
    bcmfp_pdd_actions_info_t *actions_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_pdd_actions_info_get(unit,
                                    stage_id,
                                    pdd_id,
                                    type,
                                    &actions_info));
    actions_info->num_parts = num_parts;
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_pdd_num_parts_reset(int unit,
                          bcmfp_stage_id_t stage_id,
                          bcmfp_pdd_id_t pdd_id,
                          bcmfp_pdd_oper_type_t type)
{
    bcmfp_pdd_actions_info_t *actions_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_pdd_actions_info_get(unit,
                                    stage_id,
                                    pdd_id,
                                    type,
                                    &actions_info));
    actions_info->num_parts = 0;
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_pdd_oper_info_set(int unit,
                        bcmfp_stage_id_t stage_id,
                        bcmfp_pdd_id_t pdd_id,
                        bcmfp_pdd_oper_type_t type,
                        uint8_t num_parts,
                        bcmfp_group_mode_t pdd_mode,
                        uint32_t policy_width,
                        bcmfp_pdd_oper_t *pdd_oper_info)
{
    bcmfp_stage_t *stage = NULL;
    bcmfp_action_oper_info_t *pdd_act_oper_info;
    uint8_t part = 0;
    uint32_t *default_cont_bitmap = NULL;
    uint8_t num_words = 0;
    bcmfp_pdd_oper_info_t *pdd_opinfo = NULL;
    uint8_t blk_id = BCMFP_HA_BLK_ID_INVALID;
    uint32_t blk_offset = BCMFP_HA_BLK_OFFSET_INVALID;
    bcmfp_pdd_actions_info_t *ainfo = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_pdd_oper_info_get(unit,
                                 stage_id,
                                 pdd_id,
                                 &pdd_opinfo));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_pdd_actions_info_acquire(unit,
                                        &ainfo,
                                        &blk_id,
                                        &blk_offset));
    ainfo->pdd_type = type;
    ainfo->policy_width = policy_width;
    ainfo->next_actions_info.blk_id =
                             pdd_opinfo->actions_info.blk_id;
    ainfo->next_actions_info.blk_offset =
                             pdd_opinfo->actions_info.blk_offset;
    pdd_opinfo->actions_info.blk_id = blk_id;
    pdd_opinfo->actions_info.blk_offset = blk_offset;

    SHR_IF_ERR_VERBOSE_EXIT
         (bcmfp_pdd_num_parts_set(unit,
                                  stage_id,
                                  pdd_id,
                                  type,
                                  num_parts));

    num_words = ((stage->pdd_hw_info->pdd_bitmap_size / 32) + 1);

    BCMFP_ALLOC(default_cont_bitmap,
                sizeof(uint32_t) * num_words,
                "bcmfpDefaultPddContBitmap");
    sal_memset(default_cont_bitmap, 0, num_words * sizeof(uint32_t));

    for (part = 0; part < num_parts; part++) {

        if (NULL != pdd_oper_info) {

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_pdd_bitmap_set(unit,
                                      stage_id,
                                      pdd_id,
                                      part,
                                      type,
                                      pdd_oper_info->container_bitmap));

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_pdd_section_info_set(unit,
                                        stage_id,
                                        pdd_id,
                                        part,
                                        type,
                                        &(pdd_oper_info->section_info)));
            pdd_act_oper_info = pdd_oper_info->action_oper_info;

            while (NULL != pdd_act_oper_info) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmfp_pdd_action_offset_info_set(unit,
                                                    stage_id,
                                                    pdd_id,
                                                    part,
                                                    type,
                                                    pdd_act_oper_info));
                pdd_act_oper_info = pdd_act_oper_info->next;
            }
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_pdd_data_width_set(unit,
                                          stage_id,
                                          pdd_id,
                                          part,
                                          type,
                                          pdd_oper_info->data_width));
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_pdd_group_mode_set(unit,
                                          stage_id,
                                          pdd_id,
                                          type,
                                          pdd_mode));
            pdd_oper_info = pdd_oper_info->next;
        }
    }

    BCMFP_HA_STAGE_ID_TO_BLK_ID(stage_id, blk_id);
    SHR_IF_ERR_EXIT
        (bcmfp_trans_state_set(unit, blk_id, BCMFP_TRANS_STATE_UC_A));
exit:
    SHR_FREE(default_cont_bitmap);
    SHR_FUNC_EXIT();
}

int
bcmfp_pdd_oper_info_reset(int unit,
                          bcmfp_stage_id_t stage_id,
                          bcmfp_pdd_id_t pdd_id)
{
    bcmfp_pdd_actions_info_t *actions_info = NULL;
    bcmfp_pdd_oper_info_t *pdd_opinfo = NULL;
    uint8_t blk_id = BCMFP_HA_BLK_ID_INVALID;
    uint32_t blk_offset = BCMFP_HA_BLK_OFFSET_INVALID;
    uint8_t next_blk_id = BCMFP_HA_BLK_ID_INVALID;
    uint32_t next_blk_offset = BCMFP_HA_BLK_OFFSET_INVALID;
    bcmfp_pdd_oper_type_t type = BCMFP_PDD_OPER_TYPE_NONE;
    void *ha_mem = NULL;
    bcmfp_ha_blk_hdr_t *blk_hdr = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_pdd_oper_info_get(unit,
                                 stage_id,
                                 pdd_id,
                                 &pdd_opinfo));

    blk_id = pdd_opinfo->actions_info.blk_id;
    blk_offset = pdd_opinfo->actions_info.blk_offset;
    while (blk_id != BCMFP_HA_BLK_ID_INVALID &&
           blk_offset != BCMFP_HA_BLK_OFFSET_INVALID) {

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_ha_mem_get(unit, blk_id, &ha_mem));
        blk_hdr = (bcmfp_ha_blk_hdr_t *)ha_mem;
        actions_info = (bcmfp_pdd_actions_info_t *)
                       ((uint8_t *)ha_mem + blk_offset);

        type = actions_info->pdd_type;

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_pdd_bitmap_reset(unit,
                                    stage_id,
                                    pdd_id,
                                    type));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_pdd_data_width_reset(unit,
                                        stage_id,
                                        pdd_id,
                                        type));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_pdd_section_info_reset(unit,
                                          stage_id,
                                          pdd_id,
                                          type));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_pdd_action_offset_info_reset(unit,
                                                stage_id,
                                                pdd_id,
                                                type));

        next_blk_id = actions_info->next_actions_info.blk_id;
        next_blk_offset = actions_info->next_actions_info.blk_offset;

        actions_info->next_actions_info.blk_id = blk_id;
        actions_info->next_actions_info.blk_offset =
                               blk_hdr->free_blk_offset;
        blk_hdr->free_blk_offset = blk_offset;
        blk_hdr->free_ha_element_count++;
        SHR_IF_ERR_EXIT
            (bcmfp_trans_state_set(unit, blk_id, BCMFP_TRANS_STATE_UC_A));

        blk_id = next_blk_id;
        blk_offset = next_blk_offset;
    }
    pdd_opinfo->actions_info.blk_id = BCMFP_HA_BLK_ID_INVALID;
    pdd_opinfo->actions_info.blk_offset =
                             BCMFP_HA_BLK_OFFSET_INVALID;
    BCMFP_HA_STAGE_ID_TO_BLK_ID(stage_id, blk_id);
    SHR_IF_ERR_EXIT
        (bcmfp_trans_state_set(unit, blk_id, BCMFP_TRANS_STATE_UC_A));
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_pdd_config_info_set(int unit,
                          const bcmltd_op_arg_t *op_arg,
                          bcmfp_stage_id_t stage_id,
                          bcmfp_pdd_id_t pdd_id,
                          bcmfp_group_mode_t group_mode,
                          bool is_default_policy,
                          bool is_pdd_per_group,
                          bool is_sbr_per_group,
                          bcmfp_pdd_config_t *pdd_config,
                          uint8_t *valid_pdd_data,
                          bcmfp_pdd_cfg_t **pdd_config_info)
{
    bcmfp_stage_t *stage = NULL;
    bcmfp_action_data_t *action_data = NULL;
    uint16_t num_actions = 0;
    bcmfp_action_t pdd_actions[BCMFP_ACTIONS_COUNT];
    bcmfp_pdd_cfg_t *pdd_cfg_info = NULL;

    SHR_FUNC_ENTER(unit);

    BCMFP_ALLOC(pdd_cfg_info, sizeof(bcmfp_pdd_cfg_t),
                    "bcmfpPddConfigInfoPddCfgGet");

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));

    *valid_pdd_data = FALSE;
    pdd_cfg_info->action_cfg_db = stage->action_cfg_db;
    pdd_cfg_info->pdd_hw_info = stage->pdd_hw_info;

    if (BCMFP_STAGE_FLAGS_TEST(stage, BCMFP_STAGE_ENTRY_TYPE_HASH)) {
        if (TRUE == is_default_policy) {
            pdd_cfg_info->pdd_hw_info->raw_policy_width =
               stage->misc_info->default_policy_data_width;
        } else {
            pdd_cfg_info->pdd_hw_info->raw_policy_width =
                stage->key_data_size_info->data_size[group_mode];
        }
    }

    if ((TRUE == is_pdd_per_group) && (TRUE == is_sbr_per_group)) {

        pdd_cfg_info->raw_data_offset =
            stage->misc_info->per_group_pdd_sbr_raw_data_offset;

    } else if ((TRUE == is_pdd_per_group) && (FALSE == is_sbr_per_group)) {

        pdd_cfg_info->raw_data_offset =
            stage->misc_info->per_entry_sbr_raw_data_offset;

    } else if ((FALSE == is_pdd_per_group) && (TRUE == is_sbr_per_group)) {

        pdd_cfg_info->raw_data_offset =
            stage->misc_info->per_entry_pdd_raw_data_offset;

    } else {

        pdd_cfg_info->raw_data_offset =
            stage->misc_info->per_entry_pdd_sbr_raw_data_offset;

    }


    action_data = pdd_config->action_data;

    while (action_data != NULL) {

        *valid_pdd_data = TRUE;
        pdd_actions[num_actions] = action_data->action;
        num_actions++;
        action_data = action_data->next;
    }

    if (*valid_pdd_data == TRUE) {

        BCMFP_ALLOC(pdd_cfg_info->pdd_action, sizeof(bcmfp_action_t) * num_actions,
                    "bcmfpPddConfigInfoActions");

        sal_memcpy(pdd_cfg_info->pdd_action, &pdd_actions,
                    sizeof(bcmfp_action_t) * num_actions);

        pdd_cfg_info->num_actions = num_actions;

        *pdd_config_info = pdd_cfg_info;
    }

exit:
    if (SHR_FUNC_ERR() || *valid_pdd_data != TRUE) {
        bcmfp_pdd_config_info_free(unit,pdd_cfg_info);
    }
    SHR_FUNC_EXIT();
}

int
bcmfp_pdd_config_info_free(int unit,
                           bcmfp_pdd_cfg_t *pdd_cfg_info)
{
    SHR_FUNC_ENTER(unit);

    if (NULL != pdd_cfg_info) {
        if (NULL != pdd_cfg_info->pdd_action) {
            SHR_FREE(pdd_cfg_info->pdd_action);
        }
        SHR_FREE(pdd_cfg_info);
    }

    SHR_EXIT();

    exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_pdd_oper_info_get(int unit,
                        bcmfp_stage_id_t stage_id,
                        bcmfp_pdd_id_t pdd_id,
                        bcmfp_pdd_oper_info_t **oinfo)
{
    void *ha_mem = NULL;
    uint8_t blk_id = BCMFP_HA_BLK_ID_INVALID;
    uint32_t offset = 0;
    bcmfp_stage_oper_info_t *stage_oper_info = NULL;
    bcmfp_pdd_oper_info_t *pdd_oper_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(oinfo, SHR_E_PARAM);

    *oinfo = NULL;
    BCMFP_HA_STAGE_ID_TO_BLK_ID(stage_id, blk_id);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_ha_mem_get(unit, blk_id, &ha_mem));
    stage_oper_info = (bcmfp_stage_oper_info_t *)
                       ((uint8_t *)ha_mem + sizeof(bcmfp_ha_blk_hdr_t));
    offset = stage_oper_info->pdd_oper_info_seg;
    pdd_oper_info =
         (bcmfp_pdd_oper_info_t *)((uint8_t *)ha_mem + offset);
    *oinfo = &(pdd_oper_info[pdd_id]);
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_pdd_actions_info_get(int unit,
                           bcmfp_stage_id_t stage_id,
                           bcmfp_pdd_id_t pdd_id,
                           bcmfp_pdd_oper_type_t type,
                           bcmfp_pdd_actions_info_t **ainfo)
{
    bcmfp_pdd_oper_info_t *pdd_oper_info = NULL;
    bcmfp_pdd_actions_info_t *actions_info = NULL;
    uint8_t blk_id = BCMFP_HA_BLK_ID_INVALID;
    uint32_t blk_offset = BCMFP_HA_BLK_OFFSET_INVALID;
    void *ha_mem = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(ainfo, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_pdd_oper_info_get(unit,
                                 stage_id,
                                 pdd_id,
                                 &pdd_oper_info));

    blk_id = pdd_oper_info->actions_info.blk_id;
    blk_offset = pdd_oper_info->actions_info.blk_offset;

    while (blk_id != BCMFP_HA_BLK_ID_INVALID &&
           blk_offset != BCMFP_HA_BLK_OFFSET_INVALID) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_ha_mem_get(unit, blk_id, &ha_mem));
        actions_info = (bcmfp_pdd_actions_info_t *)
                       ((uint8_t *)ha_mem + blk_offset);
        if (actions_info->pdd_type == type) {
            break;
        }
        blk_id = actions_info->next_actions_info.blk_id;
        blk_offset = actions_info->next_actions_info.blk_offset;
        actions_info = NULL;
    }

    if (actions_info == NULL) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    *ainfo = actions_info;
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_pdd_action_offset_info_get(int unit,
                           bcmfp_stage_id_t stage_id,
                           bcmfp_pdd_id_t pdd_id,
                           bcmfp_part_id_t part_id,
                           bcmfp_action_t action_id,
                           bcmfp_pdd_oper_type_t type,
                           bcmfp_action_oper_info_t *a_offset)
{
    bool hit_status = FALSE;
    uint8_t blk_id = 0;
    uint32_t blk_offset = 0;
    uint8_t aid_blk_id = 0;
    uint32_t aid_blk_offset = 0;
    bcmfp_pdd_actions_info_t *actions_info = NULL;
    bcmfp_ha_blk_info_t *ha_blk_info = NULL;
    bcmfp_fid_offset_info_t *fid_offset_info = NULL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(a_offset, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_pdd_actions_info_get(unit,
                                    stage_id,
                                    pdd_id,
                                    type,
                                    &actions_info));
    ha_blk_info = &(actions_info->offset_info);
    blk_id = ha_blk_info->blk_id;
    blk_offset = ha_blk_info->blk_offset;

    if (blk_id == BCMFP_HA_BLK_ID_INVALID &&
        blk_offset == BCMFP_HA_BLK_OFFSET_INVALID) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    while (blk_id != BCMFP_HA_BLK_ID_INVALID &&
           blk_offset != BCMFP_HA_BLK_OFFSET_INVALID) {
        fid_offset_info = NULL;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_fid_offset_info_get(unit,
                                       blk_id,
                                       blk_offset,
                                       &fid_offset_info));
        if (action_id == fid_offset_info->fid) {
            hit_status = TRUE;
            LOG_VERBOSE(BSL_LOG_MODULE,(BSL_META_U(unit,
                        "---Action(%d) offset(s) and"
                        " width(s) in part(%d)---\n"),
                        action_id, part_id));

            aid_blk_id = fid_offset_info->offset_info.blk_id;
            aid_blk_offset = fid_offset_info->offset_info.blk_offset;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_action_oper_info_get(unit,
                                            aid_blk_id,
                                            aid_blk_offset,
                                            part_id,
                                            a_offset));
            break;
        }
        blk_id = fid_offset_info->next_blk_id;
        blk_offset = fid_offset_info->next_blk_offset;
    }
    if (hit_status == FALSE) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_pdd_data_width_get(int unit,
                     bcmfp_stage_id_t stage_id,
                     bcmfp_pdd_id_t pdd_id,
                     bcmfp_part_id_t part_id,
                     bcmfp_pdd_oper_type_t type,
                     uint32_t *data_width)
{
    bool found_part = FALSE;
    uint8_t blk_id = BCMFP_HA_BLK_ID_INVALID;
    uint32_t blk_offset = BCMFP_HA_BLK_OFFSET_INVALID;
    bcmfp_generic_data_t *ha_element = NULL;
    bcmfp_pdd_actions_info_t *actions_info = NULL;
    bcmfp_ha_blk_info_t *ha_blk_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(data_width, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_pdd_actions_info_get(unit,
                                    stage_id,
                                    pdd_id,
                                    type,
                                    &actions_info));
    ha_blk_info = &(actions_info->data_width_info);
    blk_id = ha_blk_info->blk_id;
    blk_offset = ha_blk_info->blk_offset;
    while (blk_id != BCMFP_HA_BLK_ID_INVALID &&
           blk_offset != BCMFP_HA_BLK_OFFSET_INVALID) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_ha_element_get(unit,
                                  blk_id,
                                  blk_offset,
                                  &ha_element));
        if (part_id == (ha_element->value & 0x7)) {
            found_part = TRUE;
        }
        blk_id = ha_element->next_blk_id;
        blk_offset = ha_element->next_blk_offset;

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_ha_element_get(unit,
                                  blk_id,
                                  blk_offset,
                                  &ha_element));
        blk_id = ha_element->next_blk_id;
        blk_offset = ha_element->next_blk_offset;
        if (found_part == TRUE) {
            *data_width = ha_element->value;
            break;
        }
    }
    if (found_part == FALSE) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_pdd_bitmap_get(int unit,
                     bcmfp_stage_id_t stage_id,
                     bcmfp_pdd_id_t pdd_id,
                     bcmfp_part_id_t part_id,
                     bcmfp_pdd_oper_type_t type,
                     uint32_t *pdd_bitmap)
{
    uint8_t idx = 0;
    bool found_part = FALSE;
    uint8_t num_ha_elements = 0;
    uint8_t blk_id = BCMFP_HA_BLK_ID_INVALID;
    uint32_t blk_offset = BCMFP_HA_BLK_OFFSET_INVALID;
    bcmfp_generic_data_t *ha_element = NULL;
    bcmfp_pdd_actions_info_t *actions_info = NULL;
    bcmfp_ha_blk_info_t *ha_blk_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(pdd_bitmap, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_pdd_actions_info_get(unit,
                                    stage_id,
                                    pdd_id,
                                    type,
                                    &actions_info));
    ha_blk_info = &(actions_info->bitmap_info);
    blk_id = ha_blk_info->blk_id;
    blk_offset = ha_blk_info->blk_offset;
    while (blk_id != BCMFP_HA_BLK_ID_INVALID &&
           blk_offset != BCMFP_HA_BLK_OFFSET_INVALID) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_ha_element_get(unit,
                                  blk_id,
                                  blk_offset,
                                  &ha_element));
        if (part_id == (ha_element->value & 0x7)) {
            found_part = TRUE;
        }
        num_ha_elements = ((ha_element->value >> 3) & 0x1F);
        blk_id = ha_element->next_blk_id;
        blk_offset = ha_element->next_blk_offset;
        for (idx = 0; idx < num_ha_elements; idx++) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_ha_element_get(unit,
                                      blk_id,
                                      blk_offset,
                                      &ha_element));
            blk_id = ha_element->next_blk_id;
            blk_offset = ha_element->next_blk_offset;
            if (found_part == TRUE) {
                pdd_bitmap[idx] = ha_element->value;
            }
        }
        if (found_part == TRUE) {
            break;
        }
    }
    if (found_part == FALSE) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_pdd_section_info_get(int unit,
                           bcmfp_stage_id_t stage_id,
                           bcmfp_pdd_id_t pdd_id,
                           bcmfp_part_id_t part_id,
                           bcmfp_pdd_oper_type_t type,
                           bcmfp_pdd_section_info_t *sinfo)
{
    uint8_t idx = 0;
    uint8_t blk_id = BCMFP_HA_BLK_ID_INVALID;
    uint32_t blk_offset = BCMFP_HA_BLK_OFFSET_INVALID;
    bcmfp_pdd_actions_info_t *actions_info = NULL;
    bcmfp_ha_blk_info_t *ha_blk_info = NULL;
    bcmfp_generic_data_t *ha_element = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(sinfo, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_pdd_actions_info_get(unit,
                                    stage_id,
                                    pdd_id,
                                    type,
                                    &actions_info));
    ha_blk_info = &(actions_info->section_offset);
    blk_id = ha_blk_info->blk_id;
    blk_offset = ha_blk_info->blk_offset;

    sal_memset(sinfo, 0, sizeof(bcmfp_pdd_section_info_t));
    while (blk_id != BCMFP_HA_BLK_ID_INVALID &&
           blk_offset != BCMFP_HA_BLK_OFFSET_INVALID) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_ha_element_get(unit,
                                  blk_id,
                                  blk_offset,
                                  &ha_element));

        if ((ha_element->value & 0xF) == part_id) {
            idx = sinfo->num_sections;
            sinfo->section_id[idx] = ((ha_element->value >> 4) & 0xFF);
            sinfo->section_offset[idx] = ((ha_element->value >> 12) & 0xFFF);
            sinfo->num_sections++;
        }
        blk_id = ha_element->next_blk_id;
        blk_offset = ha_element->next_blk_offset;
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_pdd_oper_status_get(int unit,
                          bcmfp_stage_id_t stage_id,
                          bcmfp_pdd_id_t pdd_id,
                          bcmfp_pdd_oper_type_t type,
                          bool *oper_status)
{
    bcmfp_pdd_actions_info_t *actions_info = NULL;

    SHR_FUNC_ENTER(unit);

    *oper_status = FALSE;

    (void)bcmfp_pdd_actions_info_get(unit,
                                     stage_id,
                                     pdd_id,
                                     type,
                                     &actions_info);
    if (actions_info != NULL) {
        *oper_status = TRUE;
    }

    SHR_FUNC_EXIT();
}

int
bcmfp_pdd_num_parts_get(int unit,
                        bcmfp_stage_id_t stage_id,
                        bcmfp_pdd_id_t pdd_id,
                        bcmfp_pdd_oper_type_t type,
                        uint8_t *num_parts)
{
    bcmfp_pdd_actions_info_t *actions_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(num_parts, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_pdd_actions_info_get(unit,
                                    stage_id,
                                    pdd_id,
                                    type,
                                    &actions_info));
    *num_parts = actions_info->num_parts;
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_pdd_policy_width_get(int unit,
                           bcmfp_stage_id_t stage_id,
                           bcmfp_pdd_id_t pdd_id,
                           bcmfp_pdd_oper_type_t type,
                           uint32_t *policy_width)
{
    bcmfp_pdd_actions_info_t *actions_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(policy_width, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_pdd_actions_info_get(unit,
                                    stage_id,
                                    pdd_id,
                                    type,
                                    &actions_info));
    *policy_width = actions_info->policy_width;
exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Get the pointer to memory segment, in HA area, that is
 *  holding the mapping between pdd template ID and group ID(s)
 *  associated to it.
 *
 * \param [in] unit Logical device id
 * \param [in] stage_id stage Id(BCMFP_STAGE_ID_XXX).
 * \param [out] pdd_group_map Pointer to memory segment.
 * \param [out] map_size Number of elements in the map array.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM pdd_group_map is NULL
 * \retval SHR_E_INTERNAL Unsupported stage_id
 */
STATIC int
bcmfp_pdd_group_map_get(int unit,
                        bcmfp_stage_id_t stage_id,
                        void **pdd_group_map,
                        uint32_t *map_size)
{
    void *ha_mem = NULL;
    uint8_t blk_id = BCMFP_HA_BLK_ID_INVALID;
    uint32_t offset = 0;
    bcmfp_stage_oper_info_t *stage_oper_info = NULL;
    size_t size = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(pdd_group_map, SHR_E_PARAM);

    BCMFP_HA_STAGE_ID_TO_BLK_ID(stage_id, blk_id);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_ha_mem_get(unit, blk_id, &ha_mem));
    size = sizeof(bcmfp_ha_blk_hdr_t);
    stage_oper_info = (bcmfp_stage_oper_info_t *)
                           ((uint8_t *)ha_mem + size);
    offset = stage_oper_info->pdd_group_map_seg;
    *pdd_group_map = (void *)((uint8_t *)ha_mem + offset);
    *map_size = stage_oper_info->pdd_group_map_size;

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_pdd_group_map_traverse(int unit,
                             bcmfp_stage_id_t stage_id,
                             uint32_t pdd_id,
                             bcmfp_entry_id_traverse_cb cb,
                             void *user_data)
{
    void *pdd_group_map = NULL;
    uint32_t map_size = 0;
    bcmfp_idp_map_type_t map_type = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(cb, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_pdd_group_map_get(unit,
                                 stage_id,
                                 &pdd_group_map,
                                 &map_size));
    map_type = BCMFP_IDP_MAP_TYPE_PDD_TO_GROUP;
    /* coverity[callee_ptr_arith] */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_idp_map_traverse(unit,
                                stage_id,
                                pdd_group_map,
                                map_size,
                                map_type,
                                &pdd_id,
                                cb,
                                user_data));
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_pdd_group_map_check(int unit,
                          bcmfp_stage_id_t stage_id,
                          uint32_t pdd_id,
                          bool *not_mapped)
{
    void *pdd_group_map = NULL;
    uint32_t map_size = 0;
    bcmfp_idp_map_type_t map_type = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(not_mapped, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_pdd_group_map_get(unit,
                                 stage_id,
                                 &pdd_group_map,
                                 &map_size));

    map_type = BCMFP_IDP_MAP_TYPE_PDD_TO_GROUP;
    /* coverity[callee_ptr_arith] */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_idp_map_check(unit,
                             stage_id,
                             pdd_group_map,
                             map_size,
                             map_type,
                             &pdd_id,
                             not_mapped));
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_pdd_group_map_add(int unit,
                        bcmfp_stage_id_t stage_id,
                        uint32_t pdd_id,
                        uint32_t group_id)
{
    void *pdd_group_map = NULL;
    uint32_t map_size = 0;
    bcmfp_idp_map_type_t map_type = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_pdd_group_map_get(unit,
                                 stage_id,
                                 &pdd_group_map,
                                 &map_size));

    map_type = BCMFP_IDP_MAP_TYPE_PDD_TO_GROUP;
    /* coverity[callee_ptr_arith] */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_idp_map_add(unit,
                           stage_id,
                           pdd_group_map,
                           map_size,
                           map_type,
                           &pdd_id,
                           group_id));
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_pdd_group_map_delete(int unit,
                           bcmfp_stage_id_t stage_id,
                           uint32_t pdd_id,
                           uint32_t group_id)
{
    void *pdd_group_map = NULL;
    uint32_t map_size = 0;
    bcmfp_idp_map_type_t map_type = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_pdd_group_map_get(unit,
                                 stage_id,
                                 &pdd_group_map,
                                 &map_size));

    map_type = BCMFP_IDP_MAP_TYPE_PDD_TO_GROUP;
    /* coverity[callee_ptr_arith] */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_idp_map_delete(unit,
                              stage_id,
                              pdd_group_map,
                              map_size,
                              map_type,
                              &pdd_id,
                              group_id));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the pointer to memory segment, in HA area, that is
 *  holding the mapping between pdd template ID and entry ID(s)
 *  associated to it.
 *
 * \param [in] unit Logical device id
 * \param [in] stage_id stage Id(BCMFP_STAGE_ID_XXX).
 * \param [out] pdd_entry_map Pointer to memory segment.
 * \param [out] map_size Number of elements in the map array.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM pdd_group_map is NULL
 * \retval SHR_E_INTERNAL Unsupported stage_id
 */
STATIC int
bcmfp_pdd_entry_map_get(int unit,
                        bcmfp_stage_id_t stage_id,
                        void **pdd_entry_map,
                        uint32_t *map_size)
{
    void *ha_mem = NULL;
    uint8_t blk_id = BCMFP_HA_BLK_ID_INVALID;
    uint32_t offset = 0;
    bcmfp_stage_oper_info_t *stage_oper_info = NULL;
    size_t size = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(pdd_entry_map, SHR_E_PARAM);

    BCMFP_HA_STAGE_ID_TO_BLK_ID(stage_id, blk_id);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_ha_mem_get(unit, blk_id, &ha_mem));
    size = sizeof(bcmfp_ha_blk_hdr_t);
    stage_oper_info = (bcmfp_stage_oper_info_t *)
                           ((uint8_t *)ha_mem + size);
    offset = stage_oper_info->pdd_entry_map_seg;
    *pdd_entry_map = (void *)((uint8_t *)ha_mem + offset);
    *map_size = stage_oper_info->pdd_entry_map_size;

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_pdd_entry_map_traverse(int unit,
                             bcmfp_stage_id_t stage_id,
                             uint32_t pdd_id,
                             bcmfp_entry_id_traverse_cb cb,
                             void *user_data)
{
    void *pdd_entry_map = NULL;
    uint32_t map_size = 0;
    bcmfp_idp_map_type_t map_type = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(cb, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_pdd_entry_map_get(unit,
                                 stage_id,
                                 &pdd_entry_map,
                                 &map_size));
    map_type = BCMFP_IDP_MAP_TYPE_PDD_TO_ENTRY;
    /* coverity[callee_ptr_arith] */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_idp_map_traverse(unit,
                                stage_id,
                                pdd_entry_map,
                                map_size,
                                map_type,
                                &pdd_id,
                                cb,
                                user_data));
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_pdd_entry_map_check(int unit,
                          bcmfp_stage_id_t stage_id,
                          uint32_t pdd_id,
                          bool *not_mapped)
{
    void *pdd_entry_map = NULL;
    uint32_t map_size = 0;
    bcmfp_idp_map_type_t map_type = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(not_mapped, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_pdd_entry_map_get(unit,
                                 stage_id,
                                 &pdd_entry_map,
                                 &map_size));

    map_type = BCMFP_IDP_MAP_TYPE_PDD_TO_ENTRY;
    /* coverity[callee_ptr_arith] */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_idp_map_check(unit,
                             stage_id,
                             pdd_entry_map,
                             map_size,
                             map_type,
                             &pdd_id,
                             not_mapped));
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_pdd_entry_map_add(int unit,
                        bcmfp_stage_id_t stage_id,
                        uint32_t pdd_id,
                        uint32_t entry_id)
{
    void *pdd_entry_map = NULL;
    uint32_t map_size = 0;
    bcmfp_idp_map_type_t map_type = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_pdd_entry_map_get(unit,
                                 stage_id,
                                 &pdd_entry_map,
                                 &map_size));

    map_type = BCMFP_IDP_MAP_TYPE_PDD_TO_ENTRY;
    /* coverity[callee_ptr_arith] */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_idp_map_add(unit,
                           stage_id,
                           pdd_entry_map,
                           map_size,
                           map_type,
                           &pdd_id,
                           entry_id));
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_pdd_entry_map_delete(int unit,
                           bcmfp_stage_id_t stage_id,
                           uint32_t pdd_id,
                           uint32_t entry_id)
{
    void *pdd_entry_map = NULL;
    uint32_t map_size = 0;
    bcmfp_idp_map_type_t map_type = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_pdd_entry_map_get(unit,
                                 stage_id,
                                 &pdd_entry_map,
                                 &map_size));

    map_type = BCMFP_IDP_MAP_TYPE_PDD_TO_ENTRY;
    /* coverity[callee_ptr_arith] */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_idp_map_delete(unit,
                              stage_id,
                              pdd_entry_map,
                              map_size,
                              map_type,
                              &pdd_id,
                              entry_id));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the pointer to memory segment, in HA area, that is
 *  holding the mapping between pdd template ID and group ID(s)
 *  associated to it.
 *
 * \param [in] unit Logical device id
 * \param [in] stage_id stage Id(BCMFP_STAGE_ID_XXX).
 * \param [out] pdd_group_map Pointer to memory segment.
 * \param [out] map_size Number of elements in the map array.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM pdd_group_map is NULL
 * \retval SHR_E_INTERNAL Unsupported stage_id
 */
STATIC int
bcmfp_dpdd_group_map_get(int unit,
                         bcmfp_stage_id_t stage_id,
                         void **pdd_group_map,
                         uint32_t *map_size)
{
    void *ha_mem = NULL;
    uint8_t blk_id = BCMFP_HA_BLK_ID_INVALID;
    uint32_t offset = 0;
    bcmfp_stage_oper_info_t *stage_oper_info = NULL;
    size_t size = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(pdd_group_map, SHR_E_PARAM);

    BCMFP_HA_STAGE_ID_TO_BLK_ID(stage_id, blk_id);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_ha_mem_get(unit, blk_id, &ha_mem));
    size = sizeof(bcmfp_ha_blk_hdr_t);
    stage_oper_info = (bcmfp_stage_oper_info_t *)
                           ((uint8_t *)ha_mem + size);
    offset = stage_oper_info->dpdd_group_map_seg;
    *pdd_group_map = (void *)((uint8_t *)ha_mem + offset);
    *map_size = stage_oper_info->dpdd_group_map_size;

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_dpdd_group_map_traverse(int unit,
                              bcmfp_stage_id_t stage_id,
                              uint32_t pdd_id,
                              bcmfp_entry_id_traverse_cb cb,
                              void *user_data)
{
    void *pdd_group_map = NULL;
    uint32_t map_size = 0;
    bcmfp_idp_map_type_t map_type = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(cb, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_dpdd_group_map_get(unit,
                                  stage_id,
                                  &pdd_group_map,
                                  &map_size));
    map_type = BCMFP_IDP_MAP_TYPE_DPDD_TO_GROUP;
    /* coverity[callee_ptr_arith] */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_idp_map_traverse(unit,
                                stage_id,
                                pdd_group_map,
                                map_size,
                                map_type,
                                &pdd_id,
                                cb,
                                user_data));
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_dpdd_group_map_check(int unit,
                           bcmfp_stage_id_t stage_id,
                           uint32_t pdd_id,
                           bool *not_mapped)
{
    void *pdd_group_map = NULL;
    uint32_t map_size = 0;
    bcmfp_idp_map_type_t map_type = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(not_mapped, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_dpdd_group_map_get(unit,
                                  stage_id,
                                  &pdd_group_map,
                                  &map_size));

    map_type = BCMFP_IDP_MAP_TYPE_DPDD_TO_GROUP;
    /* coverity[callee_ptr_arith] */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_idp_map_check(unit,
                             stage_id,
                             pdd_group_map,
                             map_size,
                             map_type,
                             &pdd_id,
                             not_mapped));
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_dpdd_group_map_add(int unit,
                         bcmfp_stage_id_t stage_id,
                         uint32_t pdd_id,
                         uint32_t group_id)
{
    void *pdd_group_map = NULL;
    uint32_t map_size = 0;
    bcmfp_idp_map_type_t map_type = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_dpdd_group_map_get(unit,
                                  stage_id,
                                  &pdd_group_map,
                                  &map_size));

    map_type = BCMFP_IDP_MAP_TYPE_DPDD_TO_GROUP;
    /* coverity[callee_ptr_arith] */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_idp_map_add(unit,
                           stage_id,
                           pdd_group_map,
                           map_size,
                           map_type,
                           &pdd_id,
                           group_id));
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_dpdd_group_map_delete(int unit,
                            bcmfp_stage_id_t stage_id,
                            uint32_t pdd_id,
                            uint32_t group_id)
{
    void *pdd_group_map = NULL;
    uint32_t map_size = 0;
    bcmfp_idp_map_type_t map_type = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_dpdd_group_map_get(unit,
                                  stage_id,
                                  &pdd_group_map,
                                  &map_size));

    map_type = BCMFP_IDP_MAP_TYPE_DPDD_TO_GROUP;
    /* coverity[callee_ptr_arith] */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_idp_map_delete(unit,
                              stage_id,
                              pdd_group_map,
                              map_size,
                              map_type,
                              &pdd_id,
                              group_id));
exit:
    SHR_FUNC_EXIT();
}
