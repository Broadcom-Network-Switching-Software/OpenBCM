/*! \file bcmfp_oper_set.c
 *
 * BCMFP operational information set APIs.
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
#include <bcmfp/bcmfp_oper_internal.h>
#include <bcmfp/bcmfp_pdd_internal.h>
#include <bcmlrd/bcmlrd_map.h>
#include <shr/shr_util.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmmgmt/bcmmgmt_sysm.h>
#include <bcmissu/issu_api.h>
#include <bcmfp/generated/bcmfp_ha.h>
#include <bcmfp/bcmfp_issu_internal.h>

#define BSL_LOG_MODULE BSL_LS_BCMFP_DEV

/*
 * \brief During FP component initialization time, every stage
 *  gets HA space to save group operational information of max
 *  number of groups that can be created in that stage. When
 *  some group is created successfully on run time, one of the
 *  free group operational information blocks will acquired and
 *  assigned to that group id. Note that these consecutive group
 *  operational information blocks in stages HA space are not
 *  indexed by group id.
 *
 * \param [in] unit Logical device id
 * \param [in] stage_id BCMFp stage id(INGRESS/EGRESS/VLAN/EM).
 * \param [out] oinfo Allocated group operational information.
 *
 * \retval SHR_E_NONE Success, otherwise failure
 */
static int
bcmfp_group_oper_info_acquire(int unit,
                      bcmfp_stage_id_t stage_id,
                      bcmfp_group_oper_info_t **oinfo)
{
    void *ha_mem = NULL;
    uint32_t idx = 0;
    uint8_t blk_id = BCMFP_HA_BLK_ID_INVALID;
    uint32_t offset = 0;
    bcmfp_stage_oper_info_t *stage_oper_info = NULL;
    bcmfp_group_oper_info_t *group_oper_info = NULL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(oinfo, SHR_E_PARAM);

    *oinfo = NULL;
    BCMFP_HA_STAGE_ID_TO_BLK_ID(stage_id, blk_id);
    SHR_IF_ERR_VERBOSE_EXIT(bcmfp_ha_mem_get(unit, blk_id, &ha_mem));
    stage_oper_info = (bcmfp_stage_oper_info_t *)
                       ((uint8_t *)ha_mem + sizeof(bcmfp_ha_blk_hdr_t));
    offset = stage_oper_info->group_oper_info_seg;
    group_oper_info =
         (bcmfp_group_oper_info_t *)((uint8_t *)ha_mem + offset);
    for (idx = 0; idx < stage_oper_info->num_groups; idx++)  {
        if (group_oper_info[idx].group_id == 0)  {
            break;
        }
    }
    if (idx == stage_oper_info->num_groups) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    *oinfo = &(group_oper_info[idx]);
exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Upon successfull creation of group, offset(s) and
 *  width(s) information of all qualifiers in all parts of
 *  the group will be saved in HA area. A single qualifier
 *  may be part of different slices of the group(if the group
 *  is multi wide). So this function may be called multiple
 *  times for a single qualifier. offset(s) and width(s) of a
 *  single qualifier in all parts are saved in single HA
 *  elements chain.
 *
 *  Groups Qualifiers offset(s) and width(s) information is
 *  stored in two types of HA element chains.
 *
 *  Type-1: Each Qualifier has 3 consecutive HA elements in this
 *  chain. First HA element element holds qualifier id. Second HA
 *  element holds head HA block id of another HA element chain
 *  holding offset(s) and width(s) of the qualifier in all parts.
 *  Third HA element holds head HA block offset of HA block id
 *  stored in second HA element.Only one HA element chain of this
 *  type is required per group. Head HA block id and offset comes
 *  from the group operational information.Number of HA elements
 *  in this chain is equal to (3 * number of qualifiers present in
 *  the group.
 *
 *  Type-2:Each qualifier has one HA element chain of this type.
 *  Head HA blok id and offsets to this chain comes from Type-1
 *  HA element chain. Number of HA elements in this chain depends
 *  on the number of unique offsets needs to be saved per qualifier.
 *  Number of HA element chains is equal to number of qualifiers
 *  present in the group.
 *
 * \param [in] unit Logical device id
 * \param [in] stage_id BCMFP stage id(INGRESS/EGRESS/VLAN/EM).
 * \param [in] LTID of group/presel group template LT.
 * \param [in] group_id group id created in stage_id.
 * \param [in] part_id First/Second/Third slice of the group.
 * \param [in] fid Qualifier field ID.
 * \param [in] fidx Qualifier field ID index if it is array.
 * \param [out] q_offset Qualifiers offset(s) and width(s).
 *
 * \retval SHR_E_NONE Success, otherwise failure
 */
static int
bcmfp_group_qual_offset_info_set(int unit,
                        bcmfp_stage_id_t stage_id,
                        bcmltd_sid_t sid,
                        bcmfp_group_id_t group_id,
                        bcmfp_part_id_t part_id,
                        bcmltd_fid_t fid,
                        uint16_t fidx,
                        bcmfp_qual_offset_info_t *q_offset)
{
    uint32_t idx = 0;
    uint8_t blk_id = 0;
    uint32_t blk_offset = 0;
    uint8_t fid_offset_info_blk_id = 0;
    uint32_t fid_offset_info_blk_offset = 0;
    uint8_t qid_offset_blk_id = 0;
    uint32_t qid_offset_blk_offset = 0;
    uint32_t value = 0;
    uint32_t num_ha_elements = 0;
    bcmfp_generic_data_t *ha_element = NULL;
    bcmfp_generic_data_t *qid_offset_ha_element = NULL;
    bcmfp_group_oper_info_t *group_oper_info = NULL;
    bcmfp_fid_offset_info_t *fid_offset_info = NULL;
    bool fid_offset_info_found = FALSE;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(q_offset, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_group_oper_info_get(unit,
                                   stage_id,
                                   group_id,
                                   &group_oper_info));
    blk_id = group_oper_info->qual_offset_info.blk_id;
    blk_offset = group_oper_info->qual_offset_info.blk_offset;

    /* Check if offset(s) and width(s) of qualifier is already
     * present in group operational information. This function
     * being called multiple times if group is multi wide and
     * qualifier is present in multiple slices.
     */
    while ((blk_id != BCMFP_HA_BLK_ID_INVALID) &&
           (blk_offset != BCMFP_HA_BLK_OFFSET_INVALID)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_fid_offset_info_get(unit,
                                       blk_id,
                                       blk_offset,
                                       &fid_offset_info));
        if (fid_offset_info->fid == fid &&
            fid_offset_info->fidx == fidx) {
            fid_offset_info_found = TRUE;
            break;
        }
        blk_id = fid_offset_info->next_blk_id;
        blk_offset = fid_offset_info->next_blk_offset;
    }

    /*
     * Caliculate the number of HA elements required to save
     * qualifiers offset(s) and width(s) information.
     */
    num_ha_elements = 0;
    for (idx = 0; idx < q_offset->num_offsets; idx++) {
        if (q_offset->width[idx] == 0) {
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
                                   &qid_offset_ha_element,
                                   &qid_offset_blk_id,
                                   &qid_offset_blk_offset));

    /* First time offset info of this qualifier is pushed to
     * group oper info.
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
        fid_offset_info->fid = fid;
        fid_offset_info->fidx = fidx;
        fid_offset_info->offset_info.blk_id = qid_offset_blk_id;
        fid_offset_info->offset_info.blk_offset = qid_offset_blk_offset;

        fid_offset_info->next_blk_id =
                         group_oper_info->qual_offset_info.blk_id;
        fid_offset_info->next_blk_offset =
                         group_oper_info->qual_offset_info.blk_offset;
        group_oper_info->qual_offset_info.blk_id = fid_offset_info_blk_id;
        group_oper_info->qual_offset_info.blk_offset =
                                          fid_offset_info_blk_offset;
    } else {
        /*
         * Add the new offset(s) and width(s) chain to the end of
         * Type-2 HA element chain of the qualifier.
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
            ha_element->next_blk_id = qid_offset_blk_id;
            ha_element->next_blk_offset = qid_offset_blk_offset;
        } else {
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
    }

    /*
     * All the HA element chains are linked properly. Now update the
     * Type-2 HA element values.
     */
    blk_id = qid_offset_blk_id;
    blk_offset = qid_offset_blk_offset;
    for (idx = 0; idx < q_offset->num_offsets; idx++) {
        if (q_offset->width[idx] == 0) {
            continue;
        }
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_ha_element_get(unit,
                                  blk_id,
                                  blk_offset,
                                  &qid_offset_ha_element));
        value = part_id;
        value |= (q_offset->offset[idx] << 2);
        value |= (q_offset->width[idx] << 12);
        value |= (idx << 22);
        qid_offset_ha_element->value = value;
        blk_id = qid_offset_ha_element->next_blk_id;
        blk_offset = qid_offset_ha_element->next_blk_offset;
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief When group is deleted, offset(s) and width(s) information
 *  of all its qualifiers needs to be deleted. This involves releasing
 *  the Type-1 and Type-2 HA element chains of Group. To know more
 *  about Type-1 and Type-2 HA element chains refer the function header
 *  of bcmfp_group_qual_offset_info_set.
 *
 * \param [in] unit Logical device id
 * \param [in] stage_id BCMFP stage id(INGRESS/EGRESS/VLAN/EM).
 * \param [in] group_id group id created in stage_id.
 *
 * \retval SHR_E_NONE Success, otherwise failure
 */
static int
bcmfp_group_qual_offset_info_reset(int unit,
                                   bcmfp_stage_id_t stage_id,
                                   bcmfp_group_id_t group_id)
{
    uint8_t blk_id = BCMFP_HA_BLK_ID_INVALID;
    uint8_t q_offset_blk_id = BCMFP_HA_BLK_ID_INVALID;
    uint32_t blk_offset = BCMFP_HA_BLK_OFFSET_INVALID;
    uint32_t q_offset_blk_offset = BCMFP_HA_BLK_OFFSET_INVALID;
    bcmfp_group_oper_info_t *group_oper_info = NULL;
    bcmfp_fid_offset_info_t *fid_offset_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_group_oper_info_get(unit,
                                   stage_id,
                                   group_id,
                                   &group_oper_info));

    blk_id = group_oper_info->qual_offset_info.blk_id;
    blk_offset = group_oper_info->qual_offset_info.blk_offset;

    /* First release Type-2 HA element chains of the group. */
    while ((blk_id != BCMFP_HA_BLK_ID_INVALID) &&
           (blk_offset != BCMFP_HA_BLK_OFFSET_INVALID)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_fid_offset_info_get(unit,
                                       blk_id,
                                       blk_offset,
                                       &fid_offset_info));

        q_offset_blk_id = fid_offset_info->offset_info.blk_id;
        q_offset_blk_offset = fid_offset_info->offset_info.blk_offset;

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_ha_elements_release(unit,
                                       q_offset_blk_id,
                                       q_offset_blk_offset));
        blk_id = fid_offset_info->next_blk_id;
        blk_offset = fid_offset_info->next_blk_offset;
    }

    /* Then release Type-1 HA element chain of the group. */
    blk_id = group_oper_info->qual_offset_info.blk_id;
    blk_offset = group_oper_info->qual_offset_info.blk_offset;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_fid_offset_infos_release(unit, blk_id, blk_offset));
    group_oper_info->qual_offset_info.blk_id = BCMFP_HA_BLK_ID_INVALID;
    group_oper_info->qual_offset_info.blk_offset = BCMFP_HA_BLK_OFFSET_INVALID;
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_stage_oper_mode_set(int unit,
                          bcmfp_stage_id_t stage_id,
                          bcmfp_stage_oper_mode_t oper_mode)
{
    void *ha_mem = NULL;
    uint8_t blk_id = BCMFP_HA_BLK_ID_INVALID;
    bcmfp_stage_oper_info_t *stage_oper_info = NULL;

    SHR_FUNC_ENTER(unit);

    BCMFP_HA_STAGE_ID_TO_BLK_ID(stage_id, blk_id);
    SHR_IF_ERR_VERBOSE_EXIT(bcmfp_ha_mem_get(unit, blk_id, &ha_mem));
    stage_oper_info = (bcmfp_stage_oper_info_t *)
                       ((uint8_t *)ha_mem + sizeof(bcmfp_ha_blk_hdr_t));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_trans_state_set(unit, blk_id,
                           BCMFP_TRANS_STATE_UC_A));
    stage_oper_info->oper_mode = oper_mode;
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_action_res_id_set(int unit,
                        bcmfp_stage_id_t stage_id,
                        bcmfp_group_id_t group_id,
                        uint16_t action_res_id)
{
    uint8_t blk_id = BCMFP_HA_BLK_ID_INVALID;
    bcmfp_group_oper_info_t *group_oper_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT(
        bcmfp_group_oper_info_get(unit, stage_id,
                              group_id, &group_oper_info));
    BCMFP_HA_STAGE_ID_TO_BLK_ID(stage_id, blk_id);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_trans_state_set(unit, blk_id,
                               BCMFP_TRANS_STATE_UC_A));
    group_oper_info->action_res_id = action_res_id;
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_group_oper_info_create(int unit,
                             bcmfp_stage_id_t stage_id,
                             bcmfp_group_id_t group_id,
                             bcmfp_group_oper_info_t **opinfo)
{
    size_t size = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(opinfo, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_group_oper_info_acquire(unit,
                                       stage_id,
                                       opinfo));
    size = sizeof(bcmfp_group_oper_info_t);
    sal_memset((*opinfo), 0, size);
    (*opinfo)->group_id = group_id;
    (*opinfo)->valid = TRUE;
    (*opinfo)->qual_offset_info.blk_id =
                           BCMFP_HA_BLK_ID_INVALID;
    (*opinfo)->qual_offset_info.blk_offset =
                           BCMFP_HA_BLK_OFFSET_INVALID;
    (*opinfo)->ext_ctrl_info.blk_id =
                           BCMFP_HA_BLK_ID_INVALID;
    (*opinfo)->ext_ctrl_info.blk_offset =
                           BCMFP_HA_BLK_OFFSET_INVALID;
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_group_oper_info_set(int unit, bcmfp_group_t *fg)
{
    uint8_t idx = 0;
    uint8_t parts_count = 0;
    uint8_t blk_id = BCMFP_HA_BLK_ID_INVALID;
    uint16_t fidx = 0;
    bcmltd_fid_t fid = 0;
    bcmfp_part_id_t part_id = 0;
    bcmfp_stage_t *stage = NULL;
    bcmfp_group_oper_info_t *group_oper_info = NULL;
    bcmfp_qual_offset_info_t  *q_offset = NULL;
    bcmltd_sid_t sid = 0;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(fg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, fg->stage_id, &stage));
    sid = stage->tbls.group_tbl->sid;

    BCMFP_HA_STAGE_ID_TO_BLK_ID(fg->stage_id, blk_id);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_trans_state_set(unit,
                               blk_id,
                               BCMFP_TRANS_STATE_UC_A));

    SHR_IF_ERR_EXIT_EXCEPT_IF
        (bcmfp_group_oper_info_get(unit,
                                   fg->stage_id,
                                   fg->group_id,
                                   &group_oper_info),
                                   SHR_E_NOT_FOUND);
    if (group_oper_info == NULL) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_group_oper_info_acquire(unit,
                                           fg->stage_id,
                                           &group_oper_info));
    }
    group_oper_info->group_id = fg->group_id;

    if (!(group_oper_info->qset_error)) {
        group_oper_info->valid = TRUE;
        group_oper_info->group_prio = fg->group_prio;
        group_oper_info->tbl_inst = fg->pipe_id;
        group_oper_info->slice_id= fg->lookup_id;
        group_oper_info->group_mode = fg->group_mode;
        group_oper_info->group_slice_mode = fg->group_slice_mode;
        group_oper_info->flags = fg->flags;
        group_oper_info->action_res_id = fg->action_res_id;
        group_oper_info->num_presel_ids = fg->num_presel_ids;
        group_oper_info->default_presel = FALSE;
        group_oper_info->presel_not_exists = FALSE;
        group_oper_info->compress = fg->compress;
        group_oper_info->sbr_id = fg->sbr_id;
        group_oper_info->sbr_template_id = fg->sbr_template_id;
        group_oper_info->group_slice_type = fg->group_slice_type;

        sal_memcpy(group_oper_info->compress_types,
                   fg->compress_types,
                   (sizeof(bool) * BCMFP_COMPRESS_TYPES_MAX));
        group_oper_info->ref_count = 0;

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_group_parts_count(unit, FALSE, fg->flags, &parts_count));

        group_oper_info->flex_mode = false;
        if (BCMFP_STAGE_FLAGS_TEST(stage, BCMFP_STAGE_HASH_FLEX_KEY_DATA)) {
            group_oper_info->flex_mode = true;
            group_oper_info->key_size = fg->key_size;
        }

        for (part_id = 0; part_id < parts_count; part_id++) {
            for (idx = 0; idx < fg->qual_arr[part_id].size; idx++) {
                fid = fg->qual_arr[part_id].fid_arr[idx];
                fidx = fg->qual_arr[part_id].fidx_arr[idx];
                q_offset = &(fg->qual_arr[part_id].offset_arr[idx]);
                SHR_IF_ERR_VERBOSE_EXIT(
                    bcmfp_group_qual_offset_info_set(unit,
                                                     fg->stage_id,
                                                     sid,
                                                     fg->group_id,
                                                     part_id,
                                                     fid,
                                                     fidx,
                                                     q_offset));
            }
            sal_memcpy(&(group_oper_info->ext_codes[part_id]),
                       &(fg->ext_codes[part_id]),
                       sizeof(bcmfp_ext_codes_t));
        }
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_group_oper_info_delete(int unit,
                             bcmfp_stage_id_t stage_id,
                             bcmfp_group_id_t group_id)
{
    uint8_t blk_id = BCMFP_HA_BLK_ID_INVALID;
    bcmfp_group_oper_info_t *group_oper_info = NULL;
    bcmfp_stage_oper_info_t *stage_oper_info = NULL;

    SHR_FUNC_ENTER(unit);

    BCMFP_HA_STAGE_ID_TO_BLK_ID(stage_id, blk_id);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_trans_state_set(unit, blk_id,
                           BCMFP_TRANS_STATE_UC_A));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_oper_info_get(unit,
                                   stage_id,
                                   &stage_oper_info));

    SHR_IF_ERR_EXIT_EXCEPT_IF
        (bcmfp_group_qual_offset_info_reset(unit,
                                            stage_id,
                                            group_id),
                                            SHR_E_NOT_FOUND);
    SHR_IF_ERR_EXIT_EXCEPT_IF
        (bcmfp_group_oper_info_get(unit,
                                   stage_id,
                                   group_id,
                                   &group_oper_info),
                                   SHR_E_NOT_FOUND);
    if (group_oper_info != NULL) {
        group_oper_info->valid = FALSE;
        group_oper_info->qual_offset_info.blk_id =
                                     BCMFP_HA_BLK_ID_INVALID;
        group_oper_info->qual_offset_info.blk_offset =
                                     BCMFP_HA_BLK_OFFSET_INVALID;
        group_oper_info->group_id = 0;
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_group_oper_info_update(int unit,
                             bcmfp_group_t *fg)
{
    uint8_t idx = 0;
    uint8_t parts_count = 0;
    uint8_t blk_id = BCMFP_HA_BLK_ID_INVALID;
    bcmltd_fid_t fid = 0;
    uint16_t fidx = 0;
    bcmfp_part_id_t part_id = 0;
    bcmfp_group_oper_info_t *group_oper_info = NULL;
    bcmfp_qual_offset_info_t  *q_offset = NULL;
    bcmltd_sid_t sid = 0;
    bcmfp_stage_t *stage = NULL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(fg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, fg->stage_id, &stage));
    sid = stage->tbls.group_tbl->sid;

    BCMFP_HA_STAGE_ID_TO_BLK_ID(fg->stage_id, blk_id);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_trans_state_set(unit, blk_id,
                               BCMFP_TRANS_STATE_UC_A));

    SHR_IF_ERR_EXIT(
        bcmfp_group_oper_info_get(unit, fg->stage_id,
                     fg->group_id, &group_oper_info));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_group_parts_count(unit, FALSE, fg->flags, &parts_count));
    for (part_id = 0; part_id < parts_count; part_id++) {
        for (idx = 0; idx < fg->qual_arr[part_id].size; idx++) {
            fid = fg->qual_arr[part_id].fid_arr[idx];
            fidx = fg->qual_arr[part_id].fidx_arr[idx];
            q_offset = &(fg->qual_arr[part_id].offset_arr[idx]);
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_group_qual_offset_info_set(unit,
                                                  fg->stage_id,
                                                  sid,
                                                  fg->group_id,
                                                  part_id,
                                                  fid,
                                                  fidx,
                                                  q_offset));
        }
        sal_memcpy(&(group_oper_info->ext_codes[part_id]),
                   &(fg->ext_codes[part_id]),
                   sizeof(bcmfp_ext_codes_t));
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_group_params_set(int unit,
                       bcmfp_stage_id_t stage_id,
                       int pipe_id,
                       bcmfp_group_id_t group_id,
                       uint32_t group_prio,
                       bcmfp_group_slice_mode_t group_slice_mode,
                       uint8_t port_pkt_type)
{
    uint8_t blk_id = BCMFP_HA_BLK_ID_INVALID;
    bcmfp_group_oper_info_t *group_oper_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT_EXCEPT_IF
        (bcmfp_group_oper_info_get(unit, stage_id,
            group_id, &group_oper_info), SHR_E_NOT_FOUND);

    BCMFP_HA_STAGE_ID_TO_BLK_ID(stage_id, blk_id);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_trans_state_set(unit, blk_id,
                           BCMFP_TRANS_STATE_UC_A));

    if (group_oper_info == NULL) {
        /*If the given group Id is not found */
        SHR_IF_ERR_VERBOSE_EXIT(
                bcmfp_group_oper_info_acquire(unit,
                                              stage_id,
                                              &group_oper_info));
    }
    group_oper_info->group_id = group_id;
    group_oper_info->tbl_inst = pipe_id;
    group_oper_info->group_prio = group_prio;
    group_oper_info->group_slice_mode = group_slice_mode;
    group_oper_info->port_pkt_type = port_pkt_type;

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_group_params_reset(int unit,
                         bcmfp_stage_id_t stage_id,
                         bcmfp_group_id_t group_id)
{
    uint8_t blk_id = BCMFP_HA_BLK_ID_INVALID;
    bcmfp_group_oper_info_t *group_oper_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT_EXCEPT_IF
        (bcmfp_group_oper_info_get(unit, stage_id,
            group_id, &group_oper_info), SHR_E_NOT_FOUND);

    if (group_oper_info == NULL) {
        SHR_EXIT();
    }

    BCMFP_HA_STAGE_ID_TO_BLK_ID(stage_id, blk_id);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_trans_state_set(unit, blk_id,
                           BCMFP_TRANS_STATE_UC_A));

    group_oper_info->group_id = 0;
    group_oper_info->group_prio = 0;
    group_oper_info->tbl_inst = 0;
    group_oper_info->group_slice_mode = 0;
    group_oper_info->port_pkt_type = 0;

exit:
    SHR_FUNC_EXIT();
}

/*
 * Initialize the different reference count segements and their offsets in
 * stage operation info of the HA block.
 */
static int
bcmfp_stage_oper_info_ref_counts_init_cb(int unit,
                                      bcmfp_stage_id_t stage_id,
                                      bcmfp_ha_blk_hdr_t * ha_mem,
                                      bcmfp_ha_struct_offset_t *issu_offset)
{
    bcmfp_ref_count_t *ref_counts = NULL;
    uint32_t size = 0;
    bcmfp_stage_oper_info_t *stage_oper_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_oper_info_get(unit, stage_id, &stage_oper_info));

    ref_counts = (bcmfp_ref_count_t * )
                 ((uint8_t *)ha_mem + issu_offset->ref_count_offset);
    size = (issu_offset->ref_count_array_size * sizeof(bcmfp_ref_count_t));
    sal_memset(ref_counts, 0, size);

    stage_oper_info->keygen_prof_ref_count_seg =
                     issu_offset->keygen_prof_ref_count_offset;
    stage_oper_info->action_prof_ref_count_seg =
                     issu_offset->action_prof_ref_count_offset;
    stage_oper_info->qos_prof_ref_count_seg =
                     issu_offset->qos_prof_ref_count_offset;
    stage_oper_info->em_key_attrib_prof_ref_count_seg =
                     issu_offset->em_key_attrib_prof_ref_count_offset;
    stage_oper_info->sbr_prof_ref_count_seg =
                     issu_offset->sbr_prof_ref_count_offset;
    stage_oper_info->psg_prof_ref_count_seg =
                     issu_offset->psg_prof_ref_count_offset;
    stage_oper_info->num_keygen_prof = issu_offset->num_keygen_prof;
    stage_oper_info->num_action_prof = issu_offset->num_pdd_profiles;
    stage_oper_info->num_qos_prof = issu_offset->num_qos_prof;
    stage_oper_info->num_sbr_profiles = issu_offset->num_sbr_profiles;
    stage_oper_info->num_psg_profiles = issu_offset->num_presel_groups;
    stage_oper_info->num_em_key_attrib_profile =
                     issu_offset->num_em_key_attrib_profile;
exit:
    SHR_FUNC_EXIT();
}

/*
 * Initialize the deifferent template IDs to entry ID map segements  and
 * their offset in stage operation info of the HA block.
 */
static int
bcmfp_stage_oper_info_entry_maps_init_cb(int unit,
                                      bcmfp_stage_id_t stage_id,
                                      bcmfp_ha_blk_hdr_t * ha_mem,
                                      bcmfp_ha_struct_offset_t *issu_offset)
{
    bcmfp_stage_oper_info_t *stage_oper_info = NULL;
    bcmfp_ha_blk_info_t *map_info = NULL;
    size_t size = 0;
    uint32_t idx = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(issu_offset, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_oper_info_get(unit, stage_id, &stage_oper_info));

    map_info = (bcmfp_ha_blk_info_t *)
               ((uint8_t *)ha_mem + issu_offset->ha_blk_info_offset);
    size = issu_offset->ha_blk_info_array_size;

    for (idx = 0; idx < size; idx++) {
        map_info[idx].blk_id = BCMFP_HA_BLK_ID_INVALID;
        map_info[idx].blk_offset = BCMFP_HA_BLK_OFFSET_INVALID;
    }
    stage_oper_info->group_entry_map_seg =
                     issu_offset->group_entry_map_offset;
    stage_oper_info->group_entry_map_size =
                     issu_offset->group_entry_map_size;

    stage_oper_info->rule_entry_map_seg =
                     issu_offset->rule_entry_map_offset;
    stage_oper_info->rule_entry_map_size =
                     issu_offset->rule_entry_map_size;


    stage_oper_info->policy_entry_map_seg =
                     issu_offset->policy_entry_map_offset;
    stage_oper_info->policy_entry_map_size =
                     issu_offset->policy_entry_map_size;


    stage_oper_info->meter_entry_map_seg =
                     issu_offset->meter_entry_map_offset;
    stage_oper_info->meter_entry_map_size =
                     issu_offset->meter_entry_map_size;

    stage_oper_info->ctr_entry_map_seg =
                     issu_offset->ctr_entry_map_offset;
    stage_oper_info->ctr_entry_map_size =
                     issu_offset->ctr_entry_map_size;

    stage_oper_info->pdd_entry_map_seg =
                     issu_offset->pdd_entry_map_offset;
    stage_oper_info->pdd_entry_map_size =
                     issu_offset->pdd_entry_map_size;

    stage_oper_info->sbr_entry_map_seg =
                     issu_offset->sbr_entry_map_offset;
    stage_oper_info->sbr_entry_map_size =
                     issu_offset->sbr_entry_map_size;
exit:
    SHR_FUNC_EXIT();
}

/*
 * Initialize the deifferent template IDs to group ID map segements and
 * their offset in stage operational info of the HA block.
 */
static int
bcmfp_stage_oper_info_group_maps_init_cb(int unit,
                                      bcmfp_stage_id_t stage_id,
                                      bcmfp_ha_blk_hdr_t * ha_mem,
                                      bcmfp_ha_struct_offset_t *issu_offset)
{
    bcmfp_stage_oper_info_t *stage_oper_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_oper_info_get(unit, stage_id, &stage_oper_info));

    stage_oper_info->pse_group_map_seg =
                     issu_offset->pse_group_map_offset;
    stage_oper_info->pse_group_map_size =
                     issu_offset->pse_group_map_size;

    stage_oper_info->psg_group_map_seg =
                     issu_offset->psg_group_map_offset;
    stage_oper_info->psg_group_map_size =
                     issu_offset->psg_group_map_size;

    stage_oper_info->sbr_group_map_seg =
                     issu_offset->sbr_group_map_offset;
    stage_oper_info->sbr_group_map_size =
                     issu_offset->sbr_group_map_size;

    stage_oper_info->pdd_group_map_seg =
                     issu_offset->pdd_group_map_offset;
    stage_oper_info->pdd_group_map_size =
                     issu_offset->pdd_group_map_size;

    stage_oper_info->dpdd_group_map_seg =
                     issu_offset->dpdd_group_map_offset;
    stage_oper_info->dpdd_group_map_size =
                     issu_offset->dpdd_group_map_size;

    stage_oper_info->dpolicy_group_map_seg =
                     issu_offset->dpolicy_group_map_offset;
    stage_oper_info->dpolicy_group_map_size =
                     issu_offset->dpolicy_group_map_size;

    stage_oper_info->dsbr_group_map_seg =
                     issu_offset->dsbr_group_map_offset;
    stage_oper_info->dsbr_group_map_size =
                     issu_offset->dsbr_group_map_size;
exit:
    SHR_FUNC_EXIT();
}

/*
 * Initialize the deifferent legacy counter state segments and their
 * offset in stage operational info of the HA block.
 */
static int
bcmfp_stage_oper_info_ctrs_state_init_cb(int unit,
                                      bcmfp_stage_id_t stage_id,
                                      bcmfp_ha_blk_hdr_t *ha_mem,
                                      bcmfp_ha_struct_offset_t *issu_offset)
{
    bcmfp_stage_oper_info_t *stage_oper_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(issu_offset, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_oper_info_get(unit, stage_id, &stage_oper_info));

    stage_oper_info->ctr_hw_index_bitmap_seg =
                     issu_offset->ctr_hw_index_bitmap_offset;
    stage_oper_info->ctr_id_to_hw_index_map_seg =
                     issu_offset->ctr_id_to_hw_index_map_offset;
    stage_oper_info->ctr_egr_stat_byte_count_map_seg =
                     issu_offset->ctr_egr_stat_byte_count_map_offset;
    stage_oper_info->ctr_egr_stat_pkt_count_map_seg =
                     issu_offset->ctr_egr_stat_pkt_count_map_offset;
    stage_oper_info->ctr_egr_stats_id_bmp_seg =
                     issu_offset->ctr_egr_stats_id_bmp_offset;

exit:
    SHR_FUNC_EXIT();
}

static int
bcmfp_stage_oper_info_misc_init_cb(int unit,
                                   bcmfp_stage_id_t stage_id,
                                   bcmfp_ha_blk_hdr_t *ha_mem)
{
    bcmfp_stage_oper_info_t *stage_oper_info = NULL;
    bcmfp_stage_t *stage = NULL;
    uint32_t idx = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_oper_info_get(unit, stage_id, &stage_oper_info));

    for (idx = 0; idx < stage->misc_info->max_em_banks; idx++) {
        stage_oper_info->em_bank_blk_info[idx].bank_id = 0;
        stage_oper_info->em_bank_blk_info[idx].blk_id = 0;
        stage_oper_info->em_bank_blk_info[idx].valid = false;
    }
exit:
    SHR_FUNC_EXIT();
}

/*
 * Initialize the main group operational info segment and its offset
 * in stage operational info of the HA block.
 */
static int
bcmfp_group_oper_info_init_cb(int unit,
                              bcmfp_stage_id_t stage_id,
                              bcmfp_ha_blk_hdr_t *ha_mem,
                              bcmfp_ha_struct_offset_t *issu_offset)
{
    uint32_t size = 0;
    bcmfp_group_oper_info_t *group_oper_info = NULL;
    uint32_t idx = 0;
    bcmfp_stage_oper_info_t *stage_oper_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(issu_offset, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_oper_info_get(unit, stage_id, &stage_oper_info));
    /*
     * Initialize the offset to group opertaional information of the
     * stage in the fixed HA block created for the stage.
     */
    stage_oper_info->group_oper_info_seg =
                     issu_offset->group_oper_info_offset;
    stage_oper_info->num_groups = issu_offset->num_groups;

    group_oper_info = (bcmfp_group_oper_info_t *)((uint8_t *)ha_mem +
                      stage_oper_info->group_oper_info_seg);

    size = stage_oper_info->num_groups * sizeof(bcmfp_group_oper_info_t);

    /*
     * Initialize the fields in group operational information for maximum
     * possible groups in the stage.
     */
    sal_memset(group_oper_info, 0, size);
    for (idx = 0; idx < stage_oper_info->num_groups; idx++) {
         group_oper_info[idx].valid = FALSE;
         group_oper_info[idx].qual_offset_info.blk_id =
                              BCMFP_HA_BLK_ID_INVALID;
         group_oper_info[idx].qual_offset_info.blk_offset =
                              BCMFP_HA_BLK_OFFSET_INVALID;
         group_oper_info[idx].ext_ctrl_info.blk_id =
                              BCMFP_HA_BLK_ID_INVALID;
         group_oper_info[idx].ext_ctrl_info.blk_offset =
                              BCMFP_HA_BLK_OFFSET_INVALID;
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * Initialize the presel group operational info segment and its
 * offset in stage operational info of the HA block.
 */
static int
bcmfp_presel_group_oper_info_init_cb(int unit,
                                  bcmfp_stage_id_t stage_id,
                                  bcmfp_ha_blk_hdr_t *ha_mem,
                                  bcmfp_ha_struct_offset_t *issu_offset)
{
    bcmfp_stage_oper_info_t *stage_oper_info = NULL;
    uint32_t max_psg_id =0;
    uint32_t size = 0;
    uint32_t idx = 0;
    bcmfp_group_oper_info_t *presel_group_oper_info = NULL;

    SHR_FUNC_ENTER(unit);

    if (issu_offset->num_presel_groups == 0) {
        SHR_EXIT();
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_oper_info_get(unit, stage_id, &stage_oper_info));

    stage_oper_info->presel_group_oper_info_seg =
                     issu_offset->presel_group_oper_info_offset;
    stage_oper_info->presel_group_oper_info_size =
                     issu_offset->num_presel_groups;

    presel_group_oper_info =
                 (bcmfp_group_oper_info_t *)((uint8_t *)ha_mem +
                  stage_oper_info->presel_group_oper_info_seg);

    size = (issu_offset->num_presel_groups *
            sizeof(bcmfp_group_oper_info_t));

    sal_memset(presel_group_oper_info, 0, size);

    for (idx = 0; idx < max_psg_id; idx++) {
        presel_group_oper_info[idx].valid = FALSE;
        presel_group_oper_info[idx].qual_offset_info.blk_id =
                                    BCMFP_HA_BLK_ID_INVALID;
        presel_group_oper_info[idx].qual_offset_info.blk_offset =
                                    BCMFP_HA_BLK_OFFSET_INVALID;
        presel_group_oper_info[idx].ext_ctrl_info.blk_id =
                                    BCMFP_HA_BLK_ID_INVALID;
        presel_group_oper_info[idx].ext_ctrl_info.blk_offset =
                                    BCMFP_HA_BLK_OFFSET_INVALID;
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * Initialize the PDD operational info segment and its offset in
 * stage operational info of the HA block.
 */
static int
bcmfp_pdd_oper_info_init_cb(int unit,
                            bcmfp_stage_id_t stage_id,
                            void *ha_mem,
                            bcmfp_ha_struct_offset_t *issu_offset)
{
    uint32_t idx = 0;
    bcmfp_stage_oper_info_t *stage_oper_info = NULL;
    bcmfp_pdd_oper_info_t *pdd_oper_info = NULL;
    uint32_t size = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(issu_offset, SHR_E_PARAM);
    SHR_NULL_CHECK(ha_mem, SHR_E_PARAM);

    if (issu_offset->num_pdd_profiles == 0) {
        SHR_EXIT();
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_oper_info_get(unit, stage_id, &stage_oper_info));

    pdd_oper_info = (bcmfp_pdd_oper_info_t *)
                    ((uint8_t *)ha_mem + issu_offset->pdd_oper_info_offset);

    stage_oper_info->pdd_oper_info_seg = issu_offset->pdd_oper_info_offset;
    stage_oper_info->pdd_oper_info_size = issu_offset->num_pdd_profiles;

    size = (sizeof(bcmfp_pdd_oper_info_t) * issu_offset->num_pdd_profiles);
    sal_memset(pdd_oper_info, 0, size);
    for (idx = 0; idx < issu_offset->num_pdd_profiles; idx++) {
        pdd_oper_info[idx].actions_info.blk_id =
                           BCMFP_HA_BLK_ID_INVALID;
        pdd_oper_info[idx].actions_info.blk_offset =
                           BCMFP_HA_BLK_OFFSET_INVALID;
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_stage_oper_info_init_cb(int unit,
                              bcmfp_stage_id_t stage_id,
                              uint32_t blk_size,
                              bcmfp_ha_struct_offset_t *issu_offset)
{
    void *ha_mem = NULL;
    uint8_t blk_id = 0;
    uint8_t backup_blk_id = 0;
    bcmfp_stage_t *stage = NULL;
    bcmfp_stage_oper_info_t *stage_oper_info = NULL;
    bcmfp_ha_blk_hdr_t *blk_hdr = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(issu_offset, SHR_E_PARAM);

    BCMFP_HA_STAGE_ID_TO_BLK_ID(stage_id, blk_id);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_ha_mem_get(unit, blk_id, &ha_mem));

    /* Initialize the block header. */
    blk_hdr = (bcmfp_ha_blk_hdr_t *)ha_mem;
    blk_hdr->blk_size = blk_size;
    blk_hdr->free_blk_offset = BCMFP_HA_BLK_OFFSET_INVALID;
    blk_hdr->free_ha_element_count = 0;
    blk_hdr->trans_state = BCMFP_TRANS_STATE_IDLE;
    blk_hdr->signature = (BCMFP_HA_STRUCT_SIGN + blk_id);
    blk_hdr->blk_type = BCMFP_HA_BLK_TYPE_STAGE_OPER;
    blk_hdr->stage_id = stage_id;
    SHR_IF_ERR_VERBOSE_EXIT(
        bcmfp_ha_backup_blk_id_get(unit, blk_id, &backup_blk_id));
    blk_hdr->backup_blk_id = backup_blk_id;

    /* Initialize different fields in stage operational information. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_oper_info_get(unit, stage_id, &stage_oper_info));
    stage_oper_info->trans_state = BCMFP_TRANS_STATE_IDLE;
    stage_oper_info->num_groups = issu_offset->num_groups;
    stage_oper_info->num_em_banks = stage->misc_info->max_em_banks;

    /*
     * initailize the flag which indicates the UFT MGR
     * allocated the sbr hw indices
     */
    stage_oper_info->sbr_uftmgr_allocated = false;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_oper_info_misc_init_cb(unit,
                                            stage_id,
                                            ha_mem));

    /*
     * Initialize different segements and their offsets in stage
     * operational info of the HA block.
     */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_oper_info_ref_counts_init_cb(unit,
                                                  stage_id,
                                                  ha_mem,
                                                  issu_offset));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_oper_info_entry_maps_init_cb(unit,
                                                  stage_id,
                                                  ha_mem,
                                                  issu_offset));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_oper_info_group_maps_init_cb(unit,
                                                  stage_id,
                                                  ha_mem,
                                                  issu_offset));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_oper_info_ctrs_state_init_cb(unit,
                                                  stage_id,
                                                  ha_mem,
                                                  issu_offset));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_group_oper_info_init_cb(unit,
                                       stage_id,
                                       ha_mem,
                                       issu_offset));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_presel_group_oper_info_init_cb(unit,
                                              stage_id,
                                              ha_mem,
                                              issu_offset));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_pdd_oper_info_init_cb(unit,
                                     stage_id,
                                     ha_mem,
                                     issu_offset));
exit:
    SHR_FUNC_EXIT();
}

/* Update the different reference count segement offsets in the HA block. */
static int
bcmfp_stage_oper_info_ref_counts_init_wb(int unit,
                                 bcmfp_stage_oper_info_t *stage_oper_info,
                                 bcmfp_ha_struct_offset_t *issu_offset)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(issu_offset, SHR_E_PARAM);
    SHR_NULL_CHECK(stage_oper_info, SHR_E_PARAM);

    stage_oper_info->keygen_prof_ref_count_seg =
                     issu_offset->keygen_prof_ref_count_offset;
    stage_oper_info->action_prof_ref_count_seg =
                     issu_offset->action_prof_ref_count_offset;
    stage_oper_info->qos_prof_ref_count_seg =
                     issu_offset->qos_prof_ref_count_offset;
    stage_oper_info->em_key_attrib_prof_ref_count_seg =
                     issu_offset->em_key_attrib_prof_ref_count_offset;
    stage_oper_info->sbr_prof_ref_count_seg =
                     issu_offset->sbr_prof_ref_count_offset;
    stage_oper_info->psg_prof_ref_count_seg =
                     issu_offset->psg_prof_ref_count_offset;
    stage_oper_info->num_keygen_prof = issu_offset->num_keygen_prof;
    stage_oper_info->num_action_prof = issu_offset->num_pdd_profiles;
    stage_oper_info->num_qos_prof = issu_offset->num_qos_prof;
    stage_oper_info->num_sbr_profiles = issu_offset->num_sbr_profiles;
    stage_oper_info->num_psg_profiles = issu_offset->num_presel_groups;
    stage_oper_info->num_em_key_attrib_profile =
                     issu_offset->num_em_key_attrib_profile;
exit:
    SHR_FUNC_EXIT();

}

/*
 * Update the deifferent template IDs to entry ID map segements offset in
 * the HA block.
 */
static int
bcmfp_stage_oper_info_entry_maps_init_wb(int unit,
                                 bcmfp_stage_oper_info_t *stage_oper_info,
                                 bcmfp_ha_struct_offset_t *issu_offset)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(issu_offset, SHR_E_PARAM);
    SHR_NULL_CHECK(stage_oper_info, SHR_E_PARAM);

    stage_oper_info->group_entry_map_seg =
                     issu_offset->group_entry_map_offset;
    stage_oper_info->group_entry_map_size =
                     issu_offset->group_entry_map_size;

    stage_oper_info->rule_entry_map_seg =
                     issu_offset->rule_entry_map_offset;
    stage_oper_info->rule_entry_map_size =
                     issu_offset->rule_entry_map_size;


    stage_oper_info->policy_entry_map_seg =
                     issu_offset->policy_entry_map_offset;
    stage_oper_info->policy_entry_map_size =
                     issu_offset->policy_entry_map_size;


    stage_oper_info->meter_entry_map_seg =
                     issu_offset->meter_entry_map_offset;
    stage_oper_info->meter_entry_map_size =
                     issu_offset->meter_entry_map_size;

    stage_oper_info->ctr_entry_map_seg =
                     issu_offset->ctr_entry_map_offset;
    stage_oper_info->ctr_entry_map_size =
                     issu_offset->ctr_entry_map_size;

    stage_oper_info->pdd_entry_map_seg =
                     issu_offset->pdd_entry_map_offset;
    stage_oper_info->pdd_entry_map_size =
                     issu_offset->pdd_entry_map_size;

    stage_oper_info->sbr_entry_map_seg =
                     issu_offset->sbr_entry_map_offset;
    stage_oper_info->sbr_entry_map_size =
                     issu_offset->sbr_entry_map_size;
exit:
    SHR_FUNC_EXIT();
}

/*
 * Update the deifferent template IDs to group ID map segements offset in
 * the HA block.
 */
static int
bcmfp_stage_oper_info_group_maps_init_wb(int unit,
                                 bcmfp_stage_oper_info_t *stage_oper_info,
                                 bcmfp_ha_struct_offset_t *issu_offset)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(issu_offset, SHR_E_PARAM);
    SHR_NULL_CHECK(stage_oper_info, SHR_E_PARAM);

    stage_oper_info->pse_group_map_seg =
                     issu_offset->pse_group_map_offset;
    stage_oper_info->pse_group_map_size =
                     issu_offset->pse_group_map_size;

    stage_oper_info->psg_group_map_seg =
                     issu_offset->psg_group_map_offset;
    stage_oper_info->psg_group_map_size =
                     issu_offset->psg_group_map_size;

    stage_oper_info->sbr_group_map_seg =
                     issu_offset->sbr_group_map_offset;
    stage_oper_info->sbr_group_map_size =
                     issu_offset->sbr_group_map_size;

    stage_oper_info->pdd_group_map_seg =
                     issu_offset->pdd_group_map_offset;
    stage_oper_info->pdd_group_map_size =
                     issu_offset->pdd_group_map_size;

    stage_oper_info->dpdd_group_map_seg =
                     issu_offset->dpdd_group_map_offset;
    stage_oper_info->dpdd_group_map_size =
                     issu_offset->dpdd_group_map_size;

    stage_oper_info->dpolicy_group_map_seg =
                     issu_offset->dpolicy_group_map_offset;
    stage_oper_info->dpolicy_group_map_size =
                     issu_offset->dpolicy_group_map_size;

    stage_oper_info->dsbr_group_map_seg =
                     issu_offset->dsbr_group_map_offset;
    stage_oper_info->dsbr_group_map_size =
                     issu_offset->dsbr_group_map_size;
exit:
    SHR_FUNC_EXIT();
}

/* Update the deifferent legacy counter state segments offset in HA block. */
static int
bcmfp_stage_oper_info_ctrs_state_init_wb(int unit,
                                 bcmfp_stage_oper_info_t *stage_oper_info,
                                 bcmfp_ha_struct_offset_t *issu_offset)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(issu_offset, SHR_E_PARAM);
    SHR_NULL_CHECK(stage_oper_info, SHR_E_PARAM);

    stage_oper_info->ctr_hw_index_bitmap_seg =
                     issu_offset->ctr_hw_index_bitmap_offset;
    stage_oper_info->ctr_id_to_hw_index_map_seg =
                     issu_offset->ctr_id_to_hw_index_map_offset;
    stage_oper_info->ctr_egr_stat_byte_count_map_seg =
                     issu_offset->ctr_egr_stat_byte_count_map_offset;
    stage_oper_info->ctr_egr_stat_pkt_count_map_seg =
                     issu_offset->ctr_egr_stat_pkt_count_map_offset;
    stage_oper_info->ctr_egr_stats_id_bmp_seg =
                     issu_offset->ctr_egr_stats_id_bmp_offset;
exit:
    SHR_FUNC_EXIT();
}

/* Update the main group operational info segment offset in HA block. */
static int
bcmfp_group_oper_info_init_wb(int unit,
                              bcmfp_stage_oper_info_t *stage_oper_info,
                              bcmfp_ha_struct_offset_t *issu_offset)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(issu_offset, SHR_E_PARAM);
    SHR_NULL_CHECK(stage_oper_info, SHR_E_PARAM);

    stage_oper_info->group_oper_info_seg =
                     issu_offset->group_oper_info_offset;

exit:
    SHR_FUNC_EXIT();
}

/* Update the presel group operational info segment offset in HA block. */
static int
bcmfp_presel_group_oper_info_init_wb(int unit,
                             bcmfp_stage_oper_info_t *stage_oper_info,
                             bcmfp_ha_struct_offset_t *issu_offset)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(issu_offset, SHR_E_PARAM);
    SHR_NULL_CHECK(stage_oper_info, SHR_E_PARAM);

    stage_oper_info->presel_group_oper_info_seg =
                     issu_offset->presel_group_oper_info_offset;
exit:
    SHR_FUNC_EXIT();
}

/* Update the PDD operational info segment offset in HA block. */
static int
bcmfp_pdd_oper_info_init_wb(int unit,
                            bcmfp_stage_oper_info_t *stage_oper_info,
                            bcmfp_ha_struct_offset_t *issu_offset)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(issu_offset, SHR_E_PARAM);
    SHR_NULL_CHECK(stage_oper_info, SHR_E_PARAM);

    stage_oper_info->pdd_oper_info_seg = issu_offset->pdd_oper_info_offset;

exit:
    SHR_FUNC_EXIT();
}

/*
 * Check if any of the the array sizes are modified in the stage
 * operational info HA block. If changed the resizing of the
 * existing block is required.
 */
static int
bcmfp_stage_oper_info_update_required(int unit,
                             bcmfp_stage_id_t stage_id,
                             uint16_t blk_id,
                             bcmfp_ha_struct_offset_t *issu_offset,
                             bool *update_required)
{
    bcmfp_stage_oper_info_t *stage_oper_info = NULL;
    uint32_t stage_oper_info_offset = 0;
    void *ha_mem = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(update_required, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_ha_mem_get(unit, blk_id, &ha_mem));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmissu_struct_info_get(unit,
                                 BCMFP_STAGE_OPER_INFO_T_ID,
                                 BCMMGMT_FP_COMP_ID,
                                 blk_id,
                                 &stage_oper_info_offset));
    stage_oper_info = (bcmfp_stage_oper_info_t *)
                      ((uint8_t *)ha_mem + stage_oper_info_offset);

    *update_required = FALSE;
    if (stage_oper_info->dpolicy_group_map_size !=
        issu_offset->dpolicy_group_map_size) {
        *update_required = TRUE;
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_stage_oper_info_init_wb(int unit,
                              bcmfp_stage_id_t stage_id,
                              uint16_t blk_id,
                              uint32_t blk_size)
{
    uint32_t new_blk_size = 0;
    bcmfp_ha_struct_offset_t issu_offset;
    bool update_required = FALSE;
    bcmfp_stage_oper_info_t *stage_oper_info = NULL;
    void *ha_mem = NULL;

    SHR_FUNC_ENTER(unit);

    /*
     * If it is just warm boot not issu(i.e no change in the reported
     * structures or number of array elements), there is nothing to do.
     */
    if (bcmissu_is_active() == FALSE) {
        SHR_EXIT();
    }

    sal_memset(&issu_offset, 0, sizeof(bcmfp_ha_struct_offset_t));

    /* Get the total size of the stage operational info HA block. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_ha_stage_oper_size_get(unit,
                                      stage_id,
                                      &new_blk_size,
                                      &issu_offset));


    /* Check if the resizing of the existing HA block is required. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_oper_info_update_required(unit,
                                               stage_id,
                                               blk_id,
                                               &issu_offset,
                                               &update_required));

    /* Resize and update the existing stage oper info HA block. */
    if (update_required == TRUE) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_stage_oper_info_update(unit,
                                          stage_id,
                                          blk_id,
                                          new_blk_size,
                                          &issu_offset));
    } else {
        /*
         * Even some elements are removed from the existing structures,
         * ISSU will not change the offsets of segements. But offsets
         * generated in bcmfp_ha_stage_oper_size_get are using the new
         * structures. So there will be difference in offsets generated
         * in the bcmfp_ha_stage_oper_size_get and actuall segment
         * offsets.
         */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_issu_offsets_rectify(unit,
                                        stage_id,
                                        blk_id,
                                        &issu_offset));
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_ha_mem_get(unit, blk_id, &ha_mem));

    stage_oper_info = (bcmfp_stage_oper_info_t *)
                      ((uint8_t *)ha_mem +
                       issu_offset.stage_oper_info_offset);


    /*
     * Update offset of different segements in stage operational info.
     * Offset of segements may change due to following reasons
     * 1. New element is added to the existing structure
     * 2. Existing element is deleted from the existing structure.
     * 3. New segment is added in between the existing segments.
     */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_oper_info_ref_counts_init_wb(unit,
                                                  stage_oper_info,
                                                  &issu_offset));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_oper_info_entry_maps_init_wb(unit,
                                                  stage_oper_info,
                                                  &issu_offset));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_oper_info_group_maps_init_wb(unit,
                                                  stage_oper_info,
                                                  &issu_offset));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_oper_info_ctrs_state_init_wb(unit,
                                                  stage_oper_info,
                                                  &issu_offset));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_group_oper_info_init_wb(unit,
                                       stage_oper_info,
                                       &issu_offset));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_presel_group_oper_info_init_wb(unit,
                                              stage_oper_info,
                                              &issu_offset));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_pdd_oper_info_init_wb(unit,
                                     stage_oper_info,
                                     &issu_offset));
exit:
    SHR_FUNC_EXIT();
}
