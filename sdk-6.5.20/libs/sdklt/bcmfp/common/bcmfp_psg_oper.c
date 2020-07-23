/*! \file bcmfp_psg_oper.c
 *
 * Presel group operational information
 * set/get/reset/update APIs.
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
#include <bcmfp/generated/bcmfp_ha.h>

#define BSL_LOG_MODULE BSL_LS_BCMFP_DEV

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
 * \param [in] group_id group id created in stage_id.
 * \param [in] part_id First/Second/Third slice of the group.
 * \param [in] qual_id Qualifier identifier.
 * \param [out] q_offset Qualifiers offset(s) and width(s).
 *
 * \retval SHR_E_NONE Success, otherwise failure
 */
STATIC int
bcmfp_presel_group_qual_offset_info_set(int unit,
                        bcmfp_stage_id_t stage_id,
                        bcmltd_sid_t sid,
                        bcmfp_group_id_t group_id,
                        bcmfp_part_id_t part_id,
                        bcmltd_fid_t fid,
                        uint16_t fidx,
                        bcmfp_keygen_qual_offset_t *q_offset)
{
    uint16_t idx = 0;
    uint8_t blk_id = 0;
    uint32_t blk_offset = 0;
    uint8_t fid_offset_info_blk_id = 0;
    uint32_t fid_offset_info_blk_offset = 0;
    uint8_t qid_offset_blk_id = 0;
    uint32_t qid_offset_blk_offset = 0;
    uint32_t value = 0;
    uint16_t num_ha_elements = 0;
    bcmfp_generic_data_t *ha_element = NULL;
    bcmfp_generic_data_t *qid_offset_ha_element = NULL;
    bcmfp_group_oper_info_t *group_oper_info = NULL;
    bcmfp_fid_offset_info_t *fid_offset_info = NULL;
    bool fid_offset_info_found = FALSE;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(q_offset, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_presel_group_oper_info_get(unit,
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
 * \brief When presel group is deleted, offset(s) and width(s) information
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
STATIC int
bcmfp_presel_group_qual_offset_info_reset(int unit,
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
        (bcmfp_presel_group_oper_info_get(unit,
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
bcmfp_presel_group_oper_info_get(int unit,
                                 bcmfp_stage_id_t stage_id,
                                 uint8_t presel_group_id,
                                 bcmfp_group_oper_info_t **oper_info)
{
    void *ha_mem = NULL;
    uint8_t blk_id = BCMFP_HA_BLK_ID_INVALID;
    uint32_t offset = 0;
    bcmfp_stage_oper_info_t *stage_oper_info = NULL;
    bcmfp_group_oper_info_t *presel_group_oper_info = NULL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(oper_info, SHR_E_PARAM);

    *oper_info = NULL;

    BCMFP_HA_STAGE_ID_TO_BLK_ID(stage_id, blk_id);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_ha_mem_get(unit, blk_id, &ha_mem));
    stage_oper_info = (bcmfp_stage_oper_info_t *)
                      ((uint8_t *)ha_mem +
                      sizeof(bcmfp_ha_blk_hdr_t));

    offset = stage_oper_info->presel_group_oper_info_seg;
    presel_group_oper_info =
                      (bcmfp_group_oper_info_t *)
                      ((uint8_t *)ha_mem + offset);

    *oper_info = &(presel_group_oper_info[presel_group_id]);

exit:
    SHR_FUNC_EXIT();
}

static int
bcmfp_presel_group_qual_fid_get(int unit,
                         bcmfp_psg_config_t *gc,
                         bcmfp_qualifier_t qual_id,
                         bcmltd_fid_t *fid,
                         uint16_t *fidx)
{
    bcmfp_qual_bitmap_t *bitmap = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(gc, SHR_E_PARAM);
    SHR_NULL_CHECK(fid, SHR_E_PARAM);
    SHR_NULL_CHECK(fidx, SHR_E_PARAM);

    bitmap =  gc->qual_bitmap;
    while (bitmap != NULL) {
        if (qual_id == bitmap->qual) {
            *fid = bitmap->fid;
            *fidx = bitmap->fidx;
            break;
        }
        bitmap = bitmap->next;
    }

    if (bitmap == NULL) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_presel_group_oper_info_set(int unit,
                                 bcmfp_stage_id_t stage_id,
                                 bcmfp_group_id_t group_id,
                                 bcmfp_psg_config_t *psg_config,
                                 bcmfp_keygen_oper_t *keygen_oper)
{
    uint8_t idx = 0;
    uint8_t parts_count = 0;
    uint8_t blk_id = BCMFP_HA_BLK_ID_INVALID;
    bcmfp_qualifier_t qual_id;
    bcmfp_part_id_t part_id = 0;
    bcmfp_ext_codes_t *ext_codes = NULL;
    uint16_t ext_num = 0;
    uint16_t ext_value = 0;
    uint32_t flags = BCMFP_GROUP_SPAN_SINGLE_SLICE;
    bcmfp_group_oper_info_t *presel_group_oper_info = NULL;
    bcmfp_keygen_qual_offset_info_t *qual_offset_info = NULL;
    bcmfp_keygen_qual_offset_t *qual_offset = NULL;
    bcmltd_fid_t fid = 0;
    uint16_t fidx = 0;
    bcmltd_sid_t sid = 0;
    bcmfp_stage_t *stage = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(keygen_oper, SHR_E_PARAM);
    SHR_NULL_CHECK(psg_config, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));
    sid = stage->tbls.psg_tbl->sid;

    BCMFP_HA_STAGE_ID_TO_BLK_ID(stage_id, blk_id);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_trans_state_set(unit, blk_id,
                               BCMFP_TRANS_STATE_UC_A));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_presel_group_oper_info_get(unit,
                                          stage_id,
                                          group_id,
                                          &presel_group_oper_info));
    if (presel_group_oper_info->valid == TRUE) {
        SHR_EXIT();
    }

    presel_group_oper_info->valid = TRUE;
    presel_group_oper_info->group_id = group_id;
    presel_group_oper_info->flags = flags;
    presel_group_oper_info->ref_count = 0;

    if (keygen_oper->qual_offset_info == NULL) {
        SHR_EXIT();
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_group_parts_count(unit, TRUE, flags, &parts_count));
    for (part_id = 0; part_id < parts_count; part_id++) {
        qual_offset_info = &(keygen_oper->qual_offset_info[part_id]);
        for (idx = 0; idx < qual_offset_info->size; idx++) {
            qual_id = qual_offset_info->qid_arr[idx];
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_presel_group_qual_fid_get(unit,
                                                 psg_config,
                                                 qual_id,
                                                 &fid,
                                                 &fidx));
            qual_offset = &(qual_offset_info->offset_arr[idx]);
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_presel_group_qual_offset_info_set(unit,
                                                         stage_id,
                                                         sid,
                                                         group_id,
                                                         part_id,
                                                         fid,
                                                         fidx,
                                                         qual_offset));
        }
    }

    ext_codes = &(presel_group_oper_info->ext_codes[0]);
    sal_memset(ext_codes, BCMFP_EXT_SELCODE_DONT_CARE, sizeof(bcmfp_ext_codes_t));
    for (idx = 0; idx <  keygen_oper->ext_ctrl_sel_info_count; idx++) {
        ext_num = keygen_oper->ext_ctrl_sel_info[idx].ext_num;
        ext_value = keygen_oper->ext_ctrl_sel_info[idx].ctrl_sel_val;
        if (keygen_oper->ext_ctrl_sel_info[idx].gran == 4) {
            ext_codes->l1_e4_sel[ext_num] = ext_value;
        } else if (keygen_oper->ext_ctrl_sel_info[idx].gran == 8) {
            ext_codes->l1_e8_sel[ext_num] = ext_value;
        } else {
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_presel_group_oper_info_delete(int unit,
                             bcmfp_stage_id_t stage_id,
                             bcmfp_group_id_t group_id)
{
    uint8_t blk_id = BCMFP_HA_BLK_ID_INVALID;
    bcmfp_group_oper_info_t *group_oper_info = NULL;

    SHR_FUNC_ENTER(unit);

    BCMFP_HA_STAGE_ID_TO_BLK_ID(stage_id, blk_id);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_trans_state_set(unit,
                               blk_id,
                               BCMFP_TRANS_STATE_UC_A));

    SHR_IF_ERR_EXIT_EXCEPT_IF
        (bcmfp_presel_group_qual_offset_info_reset(unit,
                                                   stage_id,
                                                   group_id),
                                                   SHR_E_NOT_FOUND);
    SHR_IF_ERR_EXIT_EXCEPT_IF
        (bcmfp_presel_group_oper_info_get(unit,
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
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the pointer to memory segment, in HA area, that is
 *  holding the mapping between psg template ID and group ID(s)
 *  associated to it.
 *
 * \param [in] unit Logical device id
 * \param [in] stage_id stage Id(BCMFP_STAGE_ID_XXX).
 * \param [out] psg_group_map Pointer to memory segment.
 * \param [out] map_size Number of elements in the map array.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM psg_group_map is NULL
 * \retval SHR_E_INTERNAL Unsupported stage_id
 */
STATIC int
bcmfp_psg_group_map_get(int unit,
                        bcmfp_stage_id_t stage_id,
                        void **psg_group_map,
                        uint32_t *map_size)
{
    void *ha_mem = NULL;
    uint8_t blk_id = BCMFP_HA_BLK_ID_INVALID;
    uint32_t offset = 0;
    bcmfp_stage_oper_info_t *stage_oper_info = NULL;
    size_t size = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(psg_group_map, SHR_E_PARAM);

    BCMFP_HA_STAGE_ID_TO_BLK_ID(stage_id, blk_id);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_ha_mem_get(unit, blk_id, &ha_mem));
    size = sizeof(bcmfp_ha_blk_hdr_t);
    stage_oper_info = (bcmfp_stage_oper_info_t *)
                           ((uint8_t *)ha_mem + size);
    offset = stage_oper_info->psg_group_map_seg;
    *psg_group_map = (void *)((uint8_t *)ha_mem + offset);
    *map_size = stage_oper_info->psg_group_map_size;

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_psg_group_map_traverse(int unit,
                             bcmfp_stage_id_t stage_id,
                             uint32_t psg_id,
                             bcmfp_entry_id_traverse_cb cb,
                             void *user_data)
{
    void *psg_group_map = NULL;
    uint32_t map_size = 0;
    bcmfp_idp_map_type_t map_type = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(cb, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_psg_group_map_get(unit,
                                 stage_id,
                                 &psg_group_map,
                                 &map_size));
    map_type = BCMFP_IDP_MAP_TYPE_PSG_TO_GROUP;
    /* coverity[callee_ptr_arith] */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_idp_map_traverse(unit,
                                stage_id,
                                psg_group_map,
                                map_size,
                                map_type,
                                &psg_id,
                                cb,
                                user_data));
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_psg_group_map_check(int unit,
                          bcmfp_stage_id_t stage_id,
                          uint32_t psg_id,
                          bool *not_mapped)
{
    void *psg_group_map = NULL;
    uint32_t map_size = 0;
    bcmfp_idp_map_type_t map_type = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(not_mapped, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_psg_group_map_get(unit,
                                 stage_id,
                                 &psg_group_map,
                                 &map_size));

    map_type = BCMFP_IDP_MAP_TYPE_PSG_TO_GROUP;
    /* coverity[callee_ptr_arith] */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_idp_map_check(unit,
                             stage_id,
                             psg_group_map,
                             map_size,
                             map_type,
                             &psg_id,
                             not_mapped));
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_psg_group_map_add(int unit,
                        bcmfp_stage_id_t stage_id,
                        uint32_t psg_id,
                        uint32_t group_id)
{
    void *psg_group_map = NULL;
    uint32_t map_size = 0;
    bcmfp_idp_map_type_t map_type = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_psg_group_map_get(unit,
                                 stage_id,
                                 &psg_group_map,
                                 &map_size));

    map_type = BCMFP_IDP_MAP_TYPE_PSG_TO_GROUP;
    /* coverity[callee_ptr_arith] */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_idp_map_add(unit,
                           stage_id,
                           psg_group_map,
                           map_size,
                           map_type,
                           &psg_id,
                           group_id));
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_psg_group_map_delete(int unit,
                           bcmfp_stage_id_t stage_id,
                           uint32_t psg_id,
                           uint32_t group_id)
{
    void *psg_group_map = NULL;
    uint32_t map_size = 0;
    bcmfp_idp_map_type_t map_type = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_psg_group_map_get(unit,
                                 stage_id,
                                 &psg_group_map,
                                 &map_size));

    map_type = BCMFP_IDP_MAP_TYPE_PSG_TO_GROUP;
    /* coverity[callee_ptr_arith] */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_idp_map_delete(unit,
                              stage_id,
                              psg_group_map,
                              map_size,
                              map_type,
                              &psg_id,
                              group_id));
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_psg_prof_ref_count_get(int unit,
                             bcmfp_stage_id_t stage_id,
                             int pipe_id,
                             bcmfp_ref_count_t **psg_prof_ref_count)
{
    void *ha_mem = NULL;
    uint8_t blk_id = BCMFP_HA_BLK_ID_INVALID;
    bcmfp_stage_oper_mode_t oper_mode = BCMFP_STAGE_OPER_MODE_GLOBAL;
    bcmfp_stage_oper_info_t *stage_oper_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(psg_prof_ref_count, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT(
        bcmfp_stage_oper_mode_get(unit, stage_id, &oper_mode));
    if (oper_mode != BCMFP_STAGE_OPER_MODE_PIPE_UNIQUE && pipe_id != -1) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    } else if (oper_mode == BCMFP_STAGE_OPER_MODE_PIPE_UNIQUE &&
               pipe_id == -1) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    BCMFP_HA_STAGE_ID_TO_BLK_ID(stage_id, blk_id);
    SHR_IF_ERR_VERBOSE_EXIT(bcmfp_ha_mem_get(unit, blk_id, &ha_mem));
    stage_oper_info = (bcmfp_stage_oper_info_t *)
                       ((uint8_t *)ha_mem + sizeof(bcmfp_ha_blk_hdr_t));
    if (oper_mode == BCMFP_STAGE_OPER_MODE_PIPE_UNIQUE) {
         *psg_prof_ref_count = (bcmfp_ref_count_t *)((uint8_t *)ha_mem +
                       stage_oper_info->psg_prof_ref_count_seg +
                       (sizeof(bcmfp_ref_count_t) *
                        stage_oper_info->num_psg_profiles *
                        pipe_id));
    } else {
         *psg_prof_ref_count = (bcmfp_ref_count_t *)((uint8_t *)ha_mem +
                               stage_oper_info->psg_prof_ref_count_seg);
    }

exit:
    SHR_FUNC_EXIT();
}

