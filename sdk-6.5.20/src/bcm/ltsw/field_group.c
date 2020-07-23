/*! \file field_group.c
 *
 * Field Module Group Implementation above SDKLT.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <bcm/types.h>
#include <bsl/bsl.h>
#include <bcm/field.h>
#include <bcm/port.h>

#include <bcm_int/ltsw_dispatch.h>
#include <bcm_int/ltsw/field.h>
#include <bcm_int/ltsw/field_int.h>
#include <bcm_int/ltsw/mbcm/field.h>

#include <bcm_int/ltsw/dev.h>
#include <bcm_int/ltsw/mbcm/field.h>
#include <bcm_int/ltsw/util.h>
#include <bcm_int/ltsw/lt_intf.h>
#include <bcm_int/ltsw/policer.h>

#include <sal/sal_types.h>
#include <shr/shr_debug.h>
#include <bcmlt/bcmlt.h>
/******************************************************************************
* Local definitions
 */
#define BSL_LOG_MODULE BSL_LS_BCM_FP

/* Default Group Strength Index */
#define BCMINT_FP_GRP_DEFAULT_STRENGTH_IDX 0xFF

static int
bcmint_field_convert_fields_to_bmp(int unit,
                                   bcmlt_entry_handle_t entry_hdl,
                                   char *tbl_name,
                                   uint32_t *entry_bmp)
{
    int dunit = 0, rv = 0;
    uint32_t s_bit = 0, width = 0;
    uint32_t nflds = 0, iter = 0, data[2] = {0};
    uint64_t data64 = 0;
    bcmlt_field_def_t *fld_arr = NULL;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_table_field_defs_get(dunit,
                                    tbl_name,
                                    0,
                                    NULL,
                                    &nflds));
    BCMINT_FIELD_MEM_ALLOC(fld_arr,
                           (sizeof(bcmlt_field_def_t) * nflds),
                           "Field array");

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_table_field_defs_get(dunit,
                                    tbl_name,
                                    nflds,
                                    fld_arr,
                                    &nflds));
    for (iter = 0; iter < nflds; iter++) {
        data64 = 0;
        rv = bcmlt_entry_field_get(entry_hdl,
                                   fld_arr[iter].name,
                                   &data64);
        if ((rv == SHR_E_NOT_FOUND) || (fld_arr[iter].key == true)) {
            data64 = 0;
        }
        sal_memcpy(data, &data64, sizeof(data64));
        width = fld_arr[iter].width;
        SHR_BITCOPY_RANGE(entry_bmp, s_bit, data, 0, width);
        s_bit += width;
    }

exit:
    if (fld_arr != NULL) {
        BCMINT_FIELD_MEM_FREE(fld_arr);
    }
    SHR_FUNC_EXIT();
}

int
bcmint_field_color_profile_id_alloc(
    int unit,
    char *tbl_name,
    char *key_name,
    char *pipe_name,
    bcmlt_entry_handle_t entry_hdl,
    int pipe,
    int pool,
    int max_pools,
    bcmi_field_ha_profile_info_t *profile_ptr,
    int curr_id,
    int *alloc_id,
    bool *insert)
{
    int idx = 0;
    int cmp = 0;
    int dunit = 0;
    int first_free_idx = -1;
    uint8_t  pipe_val = 0;
    uint32_t iter = 0, max_entries = 0;
    uint32_t entry_bmp[10], iter_bmp[10];
    bcmlt_entry_handle_t iter_hdl = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_NULL_CHECK(profile_ptr, SHR_E_INTERNAL);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_capacity_get(unit, tbl_name, &max_entries));

    sal_memset(entry_bmp, 0, sizeof(entry_bmp));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_convert_fields_to_bmp(unit,
                                            entry_hdl,
                                            tbl_name,
                                            entry_bmp));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit,
                              tbl_name,
                              &iter_hdl));

    if (pipe != -1) {
        pipe_val = pipe;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(iter_hdl,
                                   pipe_name,
                                   pipe_val));
    }

    for (iter = 1; iter < (max_entries - 1); iter++) {
        sal_memset(iter_bmp, 0, sizeof(iter_bmp));
        if (pool == -1) {
            idx = ((pipe_val * max_entries) + iter);
        } else {
            idx = ((pipe_val * max_pools * max_entries) +
                   ((pool * max_entries) + iter));
        }

        if (profile_ptr[idx].ref_count != 0) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(iter_hdl,
                                       key_name,
                                       iter));

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_lt_entry_commit(unit,
                                      iter_hdl,
                                      BCMLT_OPCODE_LOOKUP,
                                      BCMLT_PRIORITY_NORMAL));
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmint_field_convert_fields_to_bmp(unit,
                                                    iter_hdl,
                                                    tbl_name,
                                                    iter_bmp));
            cmp = sal_memcmp(entry_bmp, iter_bmp, sizeof(iter_bmp));
            if (cmp == 0) {
                *alloc_id = iter;
                if (*alloc_id != curr_id) {
                    profile_ptr[idx].ref_count++;
                }
                *insert = false;
                break;
            }
        } else {
            if (first_free_idx == -1) {
                first_free_idx = iter;
            }
        }
    }

    if (*alloc_id == -1) {
        if (first_free_idx == -1) {
            SHR_ERR_EXIT(SHR_E_RESOURCE);
        } else {
            *alloc_id = first_free_idx;
            *insert = true;

            if (pool == -1) {
                idx = ((pipe_val * max_entries) + first_free_idx);
            } else {
                idx = ((pipe_val * max_pools * max_entries) +
                       ((pool * max_entries) + first_free_idx));
            }
            profile_ptr[idx].ref_count = 1;
        }
    }

exit:
    if (BCMLT_INVALID_HDL != iter_hdl) {
        (void) bcmlt_entry_free(iter_hdl);
    }
    SHR_FUNC_EXIT();
}

int
bcmint_field_color_profile_id_free(
    int unit,
    char *tbl_name,
    int pipe,
    bcmi_field_ha_profile_info_t *profile_ptr,
    int free_id,
    int pool,
    int max_pools,
    bool *delete)
{
    int idx = 0;
    uint8_t pipe_val = 0;
    uint32_t max_entries = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(profile_ptr, SHR_E_INTERNAL);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_capacity_get(unit, tbl_name, &max_entries));

    if (pipe != -1) {
        pipe_val = pipe;
    }

    if (pool == -1) {
        idx = ((pipe_val * max_entries) + free_id);
    } else {
        idx = ((pipe_val * max_pools * max_entries) +
               ((pool * max_entries) + free_id));
    }

    profile_ptr[idx].ref_count--;
    if (profile_ptr[idx].ref_count == 0) {
        *delete = true;
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcmint_field_color_sbr_id_alloc(int unit,
                                bcmint_field_stage_info_t *stage_info,
                                int pipe,
                                int *sbr_idx)
{
    uint8_t ha_blk_id;
    uint32_t iter = 0, max_entries = 0;
    uint32_t idx = 0;
    uint32_t pipe_val = 0;
    void *ha_ptr = NULL;
    bcmi_field_ha_profile_info_t *profile_ptr = NULL;
    bcmint_field_color_action_tbls_info_t *color_tbl_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage_info, SHR_E_INTERNAL);

    color_tbl_info = stage_info->color_action_tbls_info;
    SHR_NULL_CHECK(color_tbl_info, SHR_E_INTERNAL);

    FP_COLOR_TBL_SBR_INFO_BLK_ID(unit, stage_info->stage_id, ha_blk_id);
    ha_ptr = FP_HA_BLK_HDR_PTR(unit, ha_blk_id);
    profile_ptr = (bcmi_field_ha_profile_info_t *)
        (ha_ptr + sizeof(bcmi_field_ha_blk_hdr_t));
    SHR_NULL_CHECK(profile_ptr, SHR_E_INTERNAL);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_capacity_get(unit,
                              color_tbl_info->sbr_sid,
                              &max_entries));

    if (pipe != -1) {
        pipe_val = pipe;
    }

    for (iter = 1; iter < (max_entries - 1); iter++) {
        idx = ((max_entries * pipe_val) + iter);
        if (profile_ptr[idx].ref_count == 0) {
            profile_ptr[idx].ref_count = 1;
            *sbr_idx = iter;
            break;
        }
    }

    if (*sbr_idx == -1) {
        SHR_ERR_EXIT(SHR_E_RESOURCE);
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcmint_field_color_sbr_id_free(int unit,
                               bcmint_field_stage_info_t *stage_info,
                               int sbr_idx,
                               int pipe,
                               bool *delete)
{
    uint8_t ha_blk_id;
    void *ha_ptr = NULL;
    uint32_t pipe_val = 0, idx = 0;
    uint32_t max_entries = 0;
    bcmi_field_ha_profile_info_t *profile_ptr = NULL;
    bcmint_field_color_action_tbls_info_t *color_tbl_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage_info, SHR_E_INTERNAL);

    color_tbl_info = stage_info->color_action_tbls_info;
    SHR_NULL_CHECK(color_tbl_info, SHR_E_INTERNAL);

    FP_COLOR_TBL_SBR_INFO_BLK_ID(unit, stage_info->stage_id, ha_blk_id);
    ha_ptr = FP_HA_BLK_HDR_PTR(unit, ha_blk_id);
    profile_ptr = (bcmi_field_ha_profile_info_t *)
        (ha_ptr + sizeof(bcmi_field_ha_blk_hdr_t));
    SHR_NULL_CHECK(profile_ptr, SHR_E_INTERNAL);

    if (pipe != -1) {
        pipe_val = pipe;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_capacity_get(unit, color_tbl_info->sbr_sid, &max_entries));

    idx = ((pipe_val * max_entries) + sbr_idx);

    profile_ptr[idx].ref_count--;
    if (profile_ptr[idx].ref_count == 0) {
        *delete = true;
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcmint_field_group_policer_pool_get(int unit,
                                    bcm_field_group_t group,
                                    bcm_field_hintid_t hintid,
                                    int *pool)
{
    int rv = 0;
    bcm_field_hint_t hint;

    SHR_FUNC_ENTER(unit);

    hint.hint_type = bcmFieldHintTypePolicerPoolSel;
    rv = bcmint_field_hints_get(unit, hintid, &hint);
    if (SHR_SUCCESS(rv)) {
        *pool = hint.value;
    } else {
        *pool = -1;
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief bcmint_field_stage_from_group_get
 *
 * Depending on the range of group ID and validity of group ID
 * determine the stage_id and return stage_info.
 *
 * \param [in] unit       - Unit Number.
 * \param [in] group      - Field Group Identifier.
 * \param [in] stage_info - Pipeline stage info.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmint_field_stage_from_group_get(int unit,
                                  bcm_field_group_t group,
                                  bcmint_field_stage_info_t **stage_info)
{
    int group_id = 0;
    int stage_base_id;
    bcmi_ltsw_field_stage_t stage_id;

    SHR_FUNC_ENTER(unit);

    stage_base_id = group & ~BCM_FIELD_ID_MASK;
    switch (stage_base_id) {
        case BCM_FIELD_IFP_ID_BASE:
             stage_id = bcmiFieldStageIngress;
             break;
        case BCM_FIELD_VFP_ID_BASE:
             stage_id = bcmiFieldStageVlan;
             break;
        case BCM_FIELD_EFP_ID_BASE:
             stage_id = bcmiFieldStageEgress;
             break;
        case BCM_FIELD_EM_ID_BASE:
             stage_id = bcmiFieldStageExactMatch;
             break;
        case BCM_FIELD_FT_ID_BASE:
             stage_id = bcmiFieldStageFlowTracker;
             break;
        default:
             SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
             break;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_stage_info_get(unit, stage_id, stage_info));

    /* Mask the stage bits from given group ID */
    group_id = group & BCM_FIELD_ID_MASK;

    /* Check whether the corresponding group ID exists. */
    if (SHR_BITGET((*stage_info)->group_info->grpid_bmp.w, group_id) == 0) {
        LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META_U(unit,
            "Error: Group ID not found\n")));
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief bcmint_field_group_stage_get
 *
 * Extract group pipeline stage from qualifiers set.
 *
 * \param [in]     unit    - Unit Number.
 * \param [in]     qset_p  - Group qualifiers set.
 * \param [in]     stage   - Pipeline stage id.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmint_field_group_stage_get(int unit,
                             bcm_field_qset_t *qset_p,
                             bcmi_ltsw_field_stage_t *stage)
{
    int stage_count = 0;

    SHR_FUNC_ENTER(unit);

    /* Input parameters check */
    SHR_NULL_CHECK(qset_p, SHR_E_PARAM);
    SHR_NULL_CHECK(stage, SHR_E_PARAM);

    /* Set stage based on qualifiers set. */
    if(BCM_FIELD_QSET_TEST(*qset_p, bcmFieldQualifyStageIngress)) {
        *stage  = bcmiFieldStageIngress;
        stage_count++;
    }
    if(BCM_FIELD_QSET_TEST(*qset_p, bcmFieldQualifyStageEgress)) {
        *stage  = bcmiFieldStageEgress;
        stage_count++;
    }

    if(BCM_FIELD_QSET_TEST(*qset_p, bcmFieldQualifyStageLookup)) {
        *stage  = bcmiFieldStageVlan;
        stage_count++;
    }
    if (BCM_FIELD_QSET_TEST(*qset_p, bcmFieldQualifyStageIngressExactMatch)) {
        *stage  = bcmiFieldStageExactMatch;
        stage_count++;
    }
    if (BCM_FIELD_QSET_TEST(*qset_p, bcmFieldQualifyStageIngressFlowtracker)) {
        *stage  = bcmiFieldStageFlowTracker;
        stage_count++;
    }

    /* Default to ingress */
    if (stage_count == 0) {
        *stage = bcmiFieldStageIngress;
        stage_count++;
    }

    /* Check that only 1 stage specified in qset */
    if (stage_count > 1) {
        LOG_DEBUG(BSL_LS_BCM_FP,
           (BSL_META_U(unit,
               "Error: More than one pipeline"
               " stage was specified.\n")));
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief bcmint_field_qset_is_subset
 *
 * Determine if qset one is a subset of stage supported qset.
 *
 * \param [in] unit     - Unit Number.
 * \param [in] qset     - Qualifier set.
 * \param [in] stage    - Stage ID.
 *
 * \retval TRUE Supported Qset.
 * \retval FALSE Failure.
 */
int
bcmint_field_qset_is_subset(int unit,
                            const bcm_field_qset_t *qset_1,
                            bcmint_field_stage_info_t *stage_info,
                            bool presel,
                            uint16_t *qual_count)
{
    bcmi_field_qualify_t idx = 0;
    int map_count = 0;
    for (idx = 0; idx < BCM_FIELD_QUALIFY_MAX; idx++) {
        if (SHR_BITGET(qset_1->w, idx)) {
            if (qual_count) {
               (*qual_count)++;
            }

            if (BCMINT_FIELD_IS_PSEUDO_QUAL((int)idx)) {
                continue;
            }

            if (bcmint_field_qual_map_find(unit, idx, stage_info,
                                           presel, 0, &map_count, NULL)
                         != SHR_E_NONE) {
                return (FALSE);
            }
        }
    }

    return (TRUE);
}

void
bcmint_group_field_qual_buffer_set(int unit,
                                   const bcm_field_qual_map_info_t *map,
                                   bcmlt_field_def_t *fld_defs,
                                   bcm_field_qualify_t qual,
                                   uint32_t *src_ofst_current,
                                   int qual_hint_extracted,
                                   uint32_t *qual_value_inp_buf,
                                   uint32_t *qual_value)
{
    int idx = 0;
    uint32_t width = 0;
    uint32_t offset = 0, part_width = 0;
    uint64_t mask = 0;
    uint64_t mask64 = 0xFFFFFFFFFFFFFFFFULL;
    uint64_t *dst_ptr = NULL;

    if (map->width[0] == 0) {
        /* Qualifier takes full field width */
        width = fld_defs->width;
        dst_ptr = (uint64_t *)qual_value;
        if ((fld_defs->dtag == BCMLT_FIELD_DATA_TAG_BOOL)
            && (fld_defs->width == 1)
            && (fld_defs->elements > 1)) {
            for (idx = 0; idx < fld_defs->elements; idx++) {
                offset = (idx * 64);
                dst_ptr = ((uint64_t *)qual_value + (offset / 64));
                mask = (mask64 >> (64 - width));
                *dst_ptr |= (mask << (offset % 64));
                *src_ofst_current += width;
            }
        } else {
            if (width > 64) {
                part_width = (width - 64);
                mask = (mask64 >> (64 - part_width));
                dst_ptr[1] |= (mask);
                width -= part_width;
                *src_ofst_current += part_width;
            }
            mask = (mask64 >> (64 - width));
            dst_ptr[0] |= (mask);
            *src_ofst_current += width;
        }
    } else {
        /* Set qualifier in o/p as per offset/widths specified in map */
        for (idx = 0; idx < map->num_offsets; idx++) {
            width = map->width[idx];
            offset = map->offset[idx];
            if ((offset < 64) && ((offset + width) > 64)) {
                dst_ptr = (uint64_t *)qual_value;
                part_width = ((offset + width) - 64);
                mask = (mask64 >> (64 - part_width));
                dst_ptr[1] |= (mask);
                width -= part_width;
                *src_ofst_current += part_width;

                mask = (mask64 >> (64 - width));
                dst_ptr[0] |= (mask << offset);
                *src_ofst_current += width;
            } else {
                dst_ptr = ((uint64_t *)qual_value + (offset / 64));
                mask = (mask64 >> (64 - width));
                *dst_ptr |= (mask << (offset % 64));
                *src_ofst_current += width;
            }
        }
    }
    /* If the map has extractor bits specified in the hint, then the
     * output should be AND with hint mask
     */
    if (qual_hint_extracted) {
        SHR_BITAND_RANGE(qual_value,
                    qual_value_inp_buf, 0, *src_ofst_current, qual_value);
    }
}


/*!
 * \brief bcmint_field_process_group_qset
 *
 * Process Qset and form Bitmap fields and values.
 *
 * \param [in] unit               - Unit Number.
 * \param [in] qset               - Qualifier set.
 * \param [in] hintid             - Hintid associated to group.
 * \param [in] stage_info         - Stage info.
 * \param [in] set_qual_bitmap    - set or clear bitmap values.
 * \param [in] is_presel_grp      - 1 if group is presel group
 * \param [out] group_template    - Group template
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmint_field_process_group_qset(int unit,
                                bcm_field_qset_t *qset,
                                bcm_field_hintid_t hintid,
                                bcmint_field_stage_info_t *stage_info,
                                uint8 set_qual_bitmap,
                                bool is_presel_grp,
                                bcmlt_entry_handle_t *group_template)
{
    bool group_map;
    int iter = 0;
    int ofst_idx = 0;
    uint8_t qual_hint_extracted = 0;
    uint32_t num_element = 0;
    uint16_t ip_op_buf_size = 0;
    uint32_t src_ofset_current = 0;
    uint16_t lt_map_field_width = 0;
    char     *lt_field_name = NULL;
    bcm_field_qualify_t qual;
    bcmi_field_qualify_t qual_idx;
    uint64_t *qual_value_inp_buf = NULL;
    uint64_t *qual_value_op_buf = NULL;
    bcmlt_field_def_t *fld_defs_ref = NULL;
    bcm_field_qual_map_t *lt_map_ref = NULL;
    char* group_sid_name = NULL;
    int map_count = 0;
    int rv = SHR_E_NONE;
    uint32_t map_src_offset = 0;
    bcm_field_qset_t udf_qset;
    bcmi_field_udf_qset_bitmap_t *udf_qset_bitmap = NULL;

    SHR_FUNC_ENTER(unit);
    BCM_FIELD_QSET_INIT(udf_qset);
    if (is_presel_grp) {
        group_sid_name = stage_info->tbls_info->presel_group_sid;
    } else {
        group_sid_name = stage_info->tbls_info->group_sid;
        group_map = 1;
    }

    /* update qset with qset with hints for udf */
    rv = bcmint_field_group_udf_qset_bitmap_alloc(unit, &udf_qset_bitmap);
    if (rv != BCM_E_NONE) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_MEMORY);
    }
    rv = bcmint_field_group_udf_qset_update_with_hints(
                unit, hintid, qset, &udf_qset, udf_qset_bitmap);
    if (SHR_FAILURE(rv) && (rv != SHR_E_NOT_FOUND)) {
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }
    if (rv == BCM_E_NONE) {
        /*update the grp qset passed with udf_qset.
         * internal qual and udf_map are updated
         * as per udf_id assed to grop through grp hint
         */
        SHR_BITAND_RANGE(udf_qset.w,
                    qset->w,
                    bcmFieldQualifyCount,
                    bcmiFieldQualifyLastCount-bcmFieldQualifyCount,
                    qset->w);
    }

    BCMINT_FIELD_MEM_ALLOC
        (fld_defs_ref, sizeof(bcmlt_field_def_t), "field definitions");

    for (qual_idx = 0; qual_idx < BCM_FIELD_QUALIFY_MAX; qual_idx++) {
        map_src_offset = 0;
        if (SHR_BITGET(qset->w, qual_idx)) {
            qual = qual_idx;
            if (BCMINT_FIELD_IS_PSEUDO_QUAL((int)qual)) {
                continue;
            }

            /* Find lt map from db for given qualifier in group qset */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmint_field_qual_map_find(unit, qual_idx,
                                            stage_info, is_presel_grp, group_map,
                                            &map_count,
                                            &lt_map_ref));
            if (map_count > 1) {
                for (iter = 0; iter < lt_map_ref->num_maps; iter++) {
                    if (FALSE == bcmint_field_map_supported(unit,
                                 stage_info,
                                 (lt_map_ref)->map[iter])) {
                        continue;
                    }
                    for(ofst_idx = 0;
                        ofst_idx < (lt_map_ref->map[iter]).num_offsets;
                        ofst_idx++) {
                        if ((lt_map_ref->map[iter]).width[ofst_idx] == 0) {
                            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
                        }
                    }
                    break;
                }
            }

            /* Loop through all LT fields associated to a qualifier */
            for (iter = 0; iter < lt_map_ref->num_maps; iter++) {
                if (FALSE == bcmint_field_map_supported(unit, stage_info,
                              (lt_map_ref)->map[iter])) {
                    continue;
                }

                sal_memset(fld_defs_ref, 0, sizeof(bcmlt_field_def_t));
                lt_map_field_width = 0;
                src_ofset_current = 0;

                lt_field_name = NULL;
                /* Get Field Qualifier name for group template */
                SHR_IF_ERR_VERBOSE_EXIT
                    (mbcm_ltsw_field_lt_qual_name_update(unit, 1,
                       stage_info, qset, &(lt_map_ref->map[iter]),
                       &lt_field_name));
                /* Retreive field definition for LT field */
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmi_lt_field_def_get(unit,
                        group_sid_name, lt_field_name, fld_defs_ref));

                /* Validate field definitions for valid values */
                if (!(fld_defs_ref->elements > 0)) {
                    SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
                }

                /* Check individual offset/width specified does not exceed
                 * size of field in any case */
                for(ofst_idx = 0;
                        ofst_idx < (lt_map_ref->map[iter]).num_offsets;
                        ofst_idx++) {
                    
                    if ((sal_strncmp(fld_defs_ref->name, "CONTAINER_2_BYTE", 16) == 0) ||
                        (sal_strncmp(fld_defs_ref->name, "CONTAINER_1_BYTE", 16) == 0) ||
                        (sal_strcmp(fld_defs_ref->name, "KEY_TYPE") == 0)) {
                        lt_map_field_width += 1;
                        continue;
                    }
                    if ((sal_strcmp(fld_defs_ref->name, "QUAL_UDF_CHUNKS") == 0) ||
                        (sal_strcmp(fld_defs_ref->name, "QUAL_UDF_CHUNKS_BITMAP") == 0)) {
                        lt_map_field_width += 16;
                        continue;
                    }
                    if ((((lt_map_ref->map[iter]).offset[ofst_idx]) +
                         ((lt_map_ref->map[iter]).width[ofst_idx])) >
                          (fld_defs_ref->width)) {
                        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
                    }
                    lt_map_field_width +=
                                    (lt_map_ref->map[iter]).width[ofst_idx];
                }

                /* Check total width requested does not exceed
                 * field width */
                if (lt_map_field_width > fld_defs_ref->width) {
                    SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
                }

                if (lt_map_field_width == 0) {
                    lt_map_field_width = fld_defs_ref->width;
                }

                /* Allocate memory for ip/op buffer */
                num_element = fld_defs_ref->elements;
                ip_op_buf_size = sizeof(uint64_t) * num_element;
                BCMINT_FIELD_MEM_ALLOC
                    (qual_value_inp_buf, ip_op_buf_size, "qual ip buf");
                BCMINT_FIELD_MEM_ALLOC
                    (qual_value_op_buf, ip_op_buf_size, "qual op buf");

                /* Set relevant bits of interest in input buffer if set_qual_bitmap is
                 * set it means user want to qualify on provided qset. If not set
                 * fill input buffer with 0, which means user wants to clear it */
                qual_hint_extracted = 0;
                if (set_qual_bitmap) {
                    if (hintid != 0) {
                        SHR_IF_ERR_VERBOSE_EXIT(
                                bcmint_field_hints_group_qual_value_update
                                    (unit, hintid, &qual_hint_extracted,
                                       qual_idx, lt_map_field_width,
                                       map_src_offset, qual_value_inp_buf));
                        if (!qual_hint_extracted) {
                            sal_memset(qual_value_inp_buf, 0xff, ip_op_buf_size);
                        }
                    } else {
                        sal_memset(qual_value_inp_buf, 0xff, ip_op_buf_size);
                    }
                } else {
                    sal_memset(qual_value_inp_buf, 0, ip_op_buf_size);
                }
                map_src_offset += lt_map_field_width;

                /* Set fields at provided offset/width */
                if (fld_defs_ref->elements > 1) {
                    if (fld_defs_ref->symbol) {
                        /* Bitmap fields are not of symbol array type */
                        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
                    } else {
                        
                        if ((sal_strncmp(fld_defs_ref->name, "CONTAINER_2_BYTE", 16) == 0) ||
                            (sal_strncmp(fld_defs_ref->name, "CONTAINER_1_BYTE", 16) == 0)) {
                            uint64_t val = 0;
                            uint32_t cont = 0;

                            if (sal_strncmp(fld_defs_ref->name, "CONTAINER_2_BYTE", 16) == 0) {
                                cont = qual_idx - bcmiFieldQualifyB2Chunk0;
                                val = udf_qset_bitmap->chunk_b2_bitmap[cont];
                            } else {
                                cont = qual_idx - bcmiFieldQualifyB1Chunk0;
                                val = udf_qset_bitmap->chunk_b1_bitmap[cont];
                            }
                            SHR_IF_ERR_VERBOSE_EXIT
                                (bcmlt_entry_field_array_add(
                                      *group_template,
                                      lt_field_name,
                                      cont, &val,
                                      1));
                        } else if ((sal_strcmp(fld_defs_ref->name, "QUAL_UDF_CHUNKS") == 0) ||
                                   (sal_strcmp(fld_defs_ref->name, "QUAL_UDF_CHUNKS_BITMAP") == 0)) {
                            uint64_t val = 0;
                            uint32_t cont = 0;

                            cont = qual_idx - bcmiFieldQualifyB2Chunk0;
                            val = udf_qset_bitmap->chunk_b2_bitmap[cont];

                            SHR_IF_ERR_VERBOSE_EXIT
                                (bcmlt_entry_field_array_add(
                                      *group_template,
                                      lt_field_name,
                                      cont, &val,
                                      1));
                        } else {
                            rv = bcmlt_entry_field_array_get(
                                           *group_template,
                                            lt_field_name,
                                            0, qual_value_op_buf,
                                            num_element, &num_element);
                            if ( SHR_FAILURE(rv) &&
                                (rv != SHR_E_NOT_FOUND)) {
                                SHR_IF_ERR_VERBOSE_EXIT(rv);
                            }

                            bcmint_group_field_qual_buffer_set(unit,
                                        &(lt_map_ref->map[iter]),
                                        fld_defs_ref,
                                        qual_idx,
                                        &src_ofset_current,
                                        qual_hint_extracted,
                                        (uint32_t *)qual_value_inp_buf,
                                        (uint32_t *)qual_value_op_buf);

                            SHR_IF_ERR_VERBOSE_EXIT
                                (bcmlt_entry_field_array_add(
                                                 *group_template,
                                                  lt_field_name,
                                                  0, qual_value_op_buf,
                                                  num_element));
                        }
                    }
                } else {
                    if (fld_defs_ref->symbol) {
                        /* _BITMAP field are not of symbol type */
                        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
                    } else {
                        if ((sal_strcmp(fld_defs_ref->name, "KEY_TYPE") == 0)) {
                            SHR_IF_ERR_VERBOSE_EXIT
                                (bcmlt_entry_field_add(*group_template,
                                                   lt_field_name,
                                                   1));
                        } else {
                            rv = bcmlt_entry_field_get(*group_template,
                                                       lt_field_name,
                                                       qual_value_op_buf);
                            if ( SHR_FAILURE(rv) && (rv != SHR_E_NOT_FOUND)) {
                                SHR_IF_ERR_VERBOSE_EXIT(rv);
                            }

                            bcmint_group_field_qual_buffer_set(unit,
                                               &(lt_map_ref->map[iter]),
                                                fld_defs_ref,
                                                qual_idx,
                                                &src_ofset_current,
                                                qual_hint_extracted,
                                                (uint32_t *)qual_value_inp_buf,
                                                (uint32_t *)qual_value_op_buf);

                            SHR_IF_ERR_VERBOSE_EXIT
                                (bcmlt_entry_field_add(*group_template,
                                                       lt_field_name,
                                                       *qual_value_op_buf));
                        }
                    }
                }

                /* Set internal maps associated if any. */
                if ((lt_map_ref->map[iter]).num_internal_maps > 0)
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmint_field_qual_internal_map_set(unit,
                                                            1,
                                                            1,
                                                            qual,
                                                            *group_template,
                                                            *group_template,
                                                            stage_info,
                                                            &(lt_map_ref->map[iter]),
                                                            0, 0));

                BCMINT_FIELD_MEM_FREE(lt_field_name);
                BCMINT_FIELD_MEM_FREE(qual_value_inp_buf);
                BCMINT_FIELD_MEM_FREE(qual_value_op_buf);
            }
        }
    }

exit:
    bcmint_field_group_udf_qset_bitmap_free(unit, &udf_qset_bitmap);
    BCMINT_FIELD_MEM_FREE(lt_field_name);
    BCMINT_FIELD_MEM_FREE(qual_value_inp_buf);
    BCMINT_FIELD_MEM_FREE(qual_value_op_buf);
    BCMINT_FIELD_MEM_FREE(fld_defs_ref);
    SHR_FUNC_EXIT();
}

/*!
 * \brief bcmint_field_process_group_aset
 *
 * Process Aset and update pdd template LT fields.
 *
 * \param [in] unit               - Unit Number.
 * \param [in] stage_info         - Stage Info.
 * \param [in] set_action_bitmap  - Set or Clear action bitmap.
 * \param [in] aset               - Action set..
 * \param [out] pdd_template      - PDD template
 * \param [out] color_pdd_idx     - Color PDD table index.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmint_field_process_group_aset(int unit,
                                bcmint_field_stage_info_t *stage_info,
                                uint8 set_action_bitmap,
                                int pipe,
                                const bcm_field_aset_t *aset,
                                bcm_field_hintid_t hintid,
                                bcmlt_entry_handle_t *pdd_template,
                                int policer_pool_id,
                                int *color_pdd_idx)
{
    uint8_t ha_blk_id;
    int dunit = 0;
    int pdd_idx = -1;
    bool policer_check_fail = false;
    bool insert = false, delete = false;
    uint64_t pdd_value = 0;
    uint16_t idx = 0, iter = 0, int_iter = 0, num_color_actions = 0;
    const bcm_field_action_map_t *lt_map = NULL;
    void *ha_ptr = NULL;
    bcmi_field_ha_profile_info_t *profile_ptr = NULL;
    bcmint_field_color_action_tbls_info_t *color_tbl_info = NULL;
    bcmlt_entry_handle_t color_pdd_template = BCMLT_INVALID_HDL;
    bool is_valid = true;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);

    /* if set action is true set PDD lt field to 1 */
    if(set_action_bitmap) {
        pdd_value = 1;
    }

    color_tbl_info = stage_info->color_action_tbls_info;
    if (color_tbl_info != NULL) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_allocate(dunit,
                                  color_tbl_info->pdd_tbl_sid,
                                  &color_pdd_template));
        if (pipe != -1) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(color_pdd_template,
                                       color_tbl_info->pipe_fid,
                                       pipe));
        }
    }

    if (stage_info->flags & BCMINT_FIELD_STAGE_POLICER_ACTION_SUPPORT) {
        if (!BCM_FIELD_ASET_TEST(*aset, bcmFieldActionPolicerGroup)) {
            policer_check_fail = true;
        }
    }

    /* Loop trough all actions. */
    for (idx = 0; idx < bcmFieldActionCount; idx++) {
        if (SHR_BITGET(aset->w, idx)) {
            if (bcmint_field_action_map_find
                  (unit, idx, stage_info, &lt_map) != SHR_E_NONE) {
                SHR_ERR_EXIT(SHR_E_UNAVAIL);
            }

            for (iter = 0; iter < lt_map->num_maps; iter++) {
                if ((bcmiFieldStageExactMatch == stage_info->stage_id) &&
                    (lt_map->map[iter].em & BCMI_FIELD_STAGE_LTMAP_EM_WITHOUT_PDD)) {
                    continue;
                }
                if ((lt_map->map[iter]).policer == TRUE) {
                    if (policer_check_fail == true) {
                        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_UNAVAIL);
                    }
                    if ((lt_map->map[iter]).policer_pdd == TRUE) {
                        num_color_actions++;
                        SHR_IF_ERR_VERBOSE_EXIT
                            (bcmlt_entry_field_add(
                                     color_pdd_template,
                                     (lt_map->map[iter]).lt_field_name,
                                     pdd_value));
                    }
                    continue;
                }
                if ((((lt_map->map[iter]).color_index != -1) &&
                    ((lt_map->map[iter]).color_index != policer_pool_id)) ||
                    (((lt_map->map[iter]).pdd_sbr_index != -1) &&
                    ((lt_map->map[iter]).pdd_sbr_index != policer_pool_id))) {
                    continue;
                }
                if ((idx == bcmFieldActionStatGroup) && (hintid > 0)) {
                    /* include the field based on opaque used used by group */
                    SHR_IF_ERR_VERBOSE_EXIT
                        (mbcm_ltsw_field_action_stat_group_lt_field_validate(
                            unit, stage_info, hintid,
                            (lt_map->map[iter]).lt_field_name,
                            &is_valid));
                    if (!is_valid) {
                        continue;
                    }
                }

                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_add(*pdd_template,
                       (lt_map->map[iter]).lt_field_name, pdd_value));
                if ((lt_map->map[iter]).num_internal_maps > 0) {
                    for (int_iter = 0;
                         int_iter < ((lt_map->map[iter]).num_internal_maps);
                         int_iter++) {
                          (bcmlt_entry_field_add(*pdd_template,
                           ((lt_map->map[iter]).internal_map[int_iter]).lt_field_name,
                             pdd_value));
                    }
                }
            }
        }
    }

    if ((stage_info->flags & BCMINT_FIELD_STAGE_POLICER_ACTION_SUPPORT)
        && (NULL != color_tbl_info)
        && (0 != num_color_actions)) {
        FP_COLOR_TBL_PDD_INFO_BLK_ID(unit, stage_info->stage_id, ha_blk_id);
        ha_ptr = FP_HA_BLK_HDR_PTR(unit, ha_blk_id);
        profile_ptr = (bcmi_field_ha_profile_info_t *)
            (ha_ptr + sizeof(bcmi_field_ha_blk_hdr_t));

        if (set_action_bitmap) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmint_field_color_profile_id_alloc(
                        unit,
                        color_tbl_info->pdd_tbl_sid,
                        color_tbl_info->pdd_tbl_key_fid,
                        color_tbl_info->pipe_fid,
                        color_pdd_template,
                        pipe,
                        -1,
                        -1,
                        profile_ptr,
                        -1,
                        &pdd_idx,
                        &insert));
            if (insert == true) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_add(color_pdd_template,
                                           color_tbl_info->pdd_tbl_key_fid,
                                           pdd_idx));

                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmi_lt_entry_commit(unit,
                                          color_pdd_template,
                                          BCMLT_OPCODE_INSERT,
                                          BCMLT_PRIORITY_NORMAL));
            }

            *color_pdd_idx = pdd_idx;
        } else {
            if (*color_pdd_idx != -1) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmint_field_color_profile_id_free(
                            unit,
                            color_tbl_info->pdd_tbl_sid,
                            pipe,
                            profile_ptr,
                            *color_pdd_idx,
                            -1,
                            -1,
                            &delete));
                if (delete == true) {
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmlt_entry_field_add(color_pdd_template,
                                               color_tbl_info->pdd_tbl_key_fid,
                                               *color_pdd_idx));

                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmi_lt_entry_commit(unit,
                                              color_pdd_template,
                                              BCMLT_OPCODE_DELETE,
                                              BCMLT_PRIORITY_NORMAL));
                }
            }
        }

    }

exit:
    if (BCMLT_INVALID_HDL != color_pdd_template) {
        (void) bcmlt_entry_free(color_pdd_template);
    }
    SHR_FUNC_EXIT();
}

/*
 * Do search for given group id and return success or Failure.
 *
 * Algorithm:
 *  1. Application provides required information about the group
 *     to identify the group location in group hash.
 *  2. Calculate the hash for the given group Id.
 *  3. Check group id is present in linked list of group ids
 *     having the same hash value.
 *  4. If group_id exists, return corresponding group hash pointer.
 *  5. If group_id does not exist, return error code "Not Found".
 */
int
bcmint_field_group_oper_lookup(
    int unit,
    int group_id,
    bcmint_field_stage_info_t *stage_info,
    bcmi_field_ha_group_oper_t **group_oper)
{
    uint8_t ha_blk_id;
    uint32_t ha_blk_offset;
    uint32_t hash_offset = 0;
    void *hash_blk = NULL;
    bcmi_field_ha_group_oper_t *grp_oper = NULL;
    bcmi_field_ha_group_oper_t *grp_oper_head = NULL;
    bcmi_field_ha_blk_info_t *group_hash = NULL;
    SHR_FUNC_ENTER(unit);

    /* Mask the stage bits from given group ID */
    group_id = group_id & BCM_FIELD_ID_MASK;

    /* Verify whether the given ID is in group ID's range */
    if (!((group_id > 0) && (group_id <= stage_info->group_info->gid_max))) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    FP_GRP_HASH_BLK_ID(unit, stage_info->stage_id, ha_blk_id);
    hash_blk = FP_HA_BLK_HDR_PTR(unit, ha_blk_id);
    if (hash_blk == NULL) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    /* Calculate the hash */
    hash_offset = group_id % stage_info->group_info->hash_size;

    group_hash = (bcmi_field_ha_blk_info_t *)
                  (hash_blk + sizeof(bcmi_field_ha_blk_hdr_t)) + hash_offset;
    ha_blk_id = group_hash->blk_id;
    ha_blk_offset = group_hash->blk_offset;

    grp_oper_head = (bcmi_field_ha_group_oper_t *)
                      (FP_HA_BLK_HDR_PTR(unit, ha_blk_id) +
                       ha_blk_offset);
    grp_oper = grp_oper_head;
    while (grp_oper != NULL) {
        if (grp_oper->group_id == group_id) {
            break;
        }
        FP_HA_GRP_OPER_NEXT(unit, grp_oper, grp_oper);
    }

    /* Return not found if hash ptr is null */
    if (!grp_oper) {
        SHR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }

    if (group_oper != NULL) {
        *group_oper = grp_oper;
    }
exit:
    SHR_FUNC_EXIT();
}

/*
 * Insert the given group ID at correct hash index
 *
 * Algorithm:
 *  1. Application provides required information about the group
 *     to identify the group location in group hash.
 *  2. Calculate the hash for the given group Id.
 *  3. Check group id is present in linked list of group ids
 *     having the same hash value.
 *  4. If group_id exists, return error. code has some bug.
 *  5. If group_id does not exist, create allocate a group hash node and
 *     insert at proper location.
 */
static int
ltsw_field_group_oper_insert(
    int unit,
    bcmint_field_stage_info_t *stage_info,
    int group_id,
    char *group_mode,
    bcm_field_group_config_t *group_config,
    bcmi_field_ha_group_oper_t **group_oper)
{
    uint8_t ha_blk_id;
    uint32_t ha_blk_offset;
    uint32_t hash_offset = 0;
    void *hash_blk = NULL;
    bcmi_field_ha_blk_type_t blk_type;
    bcmi_field_ha_group_oper_t *oper = NULL;
    bcmi_field_ha_blk_info_t *group_hash = NULL;
    SHR_FUNC_ENTER(unit);

    /* Mask the stage bits from given group ID */
    group_id = group_id & BCM_FIELD_ID_MASK;

    FP_GRP_HASH_BLK_ID(unit, stage_info->stage_id, ha_blk_id);
    hash_blk = FP_HA_BLK_HDR_PTR(unit, ha_blk_id);
    if (hash_blk == NULL) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    /* Calculate the hash */
    hash_offset = group_id % stage_info->group_info->hash_size;

    group_hash = (bcmi_field_ha_blk_info_t *)
                  (hash_blk + sizeof(bcmi_field_ha_blk_hdr_t)) + hash_offset;
    ha_blk_id = group_hash->blk_id;
    ha_blk_offset = group_hash->blk_offset;
    FP_STAGE_GRP_OPER_BLK_TYPE(unit, stage_info->stage_id, blk_type);
    SHR_IF_ERR_VERBOSE_EXIT
       (bcmint_field_ha_blk_element_acquire(unit, blk_type,
                                            (void *)&oper,
                                            &ha_blk_id,
                                            &ha_blk_offset));

    if (group_hash->blk_id == BCM_FIELD_HA_BLK_ID_INVALID) {
        group_hash->blk_id = ha_blk_id;
        group_hash->blk_offset = ha_blk_offset;
        oper->next.blk_id = BCM_FIELD_HA_BLK_ID_INVALID;;
        oper->next.blk_offset = BCM_FIELD_HA_BLK_OFFSET_INVALID;
    } else {
        oper->next.blk_id = group_hash->blk_id;
        oper->next.blk_offset = group_hash->blk_offset;
        group_hash->blk_id = ha_blk_id;
        group_hash->blk_offset = ha_blk_offset;
    }

    /* Update group operation structure */
    oper->group_id = group_id;
    oper->hintid = group_config->hintid;
    oper->group_flags = 0x0;
    oper->priority = group_config->priority;
    sal_strncpy(oper->group_mode, group_mode,
                sizeof(oper->group_mode));

    FP_HA_GROUP_OPER_QSET_ARR_SET(unit, oper, group_config->qset);
    FP_HA_GROUP_OPER_ASET_ARR_SET(unit, oper, group_config->aset);

    if (group_config->flags & BCM_FIELD_GROUP_CREATE_WITH_COMPRESSION) {
        oper->group_flags |= BCMINT_FIELD_GROUP_COMPRESSION_ENABLED;
        if (group_config->flags &
            BCM_FIELD_GROUP_CREATE_WITH_SRC_IP_COMPRESSION) {
            oper->group_flags |= BCMINT_FIELD_GROUP_COMPR_SRC_IP_ENABLED;
        }

        if (group_config->flags &
            BCM_FIELD_GROUP_CREATE_WITH_SRC_IP6_COMPRESSION) {
            oper->group_flags |= BCMINT_FIELD_GROUP_COMPR_SRC_IP6_ENABLED;
        }

        if (group_config->flags &
            BCM_FIELD_GROUP_CREATE_WITH_DST_IP_COMPRESSION) {
            oper->group_flags |= BCMINT_FIELD_GROUP_COMPR_DST_IP_ENABLED;
        }

        if (group_config->flags &
            BCM_FIELD_GROUP_CREATE_WITH_DST_IP6_COMPRESSION) {
            oper->group_flags |= BCMINT_FIELD_GROUP_COMPR_DST_IP6_ENABLED;
        }
    }

    *group_oper = oper;
exit:
    SHR_FUNC_EXIT();
}


/* Remove group ID operational database */
int
ltsw_field_group_oper_remove(
    int unit,
    bcmint_field_stage_info_t *stage_info,
    int group_id)
{
    uint8_t ha_blk_id;
    uint32_t ha_blk_offset;
    uint32_t hash_offset = 0;
    void *hash_blk = NULL;
    bcmi_field_ha_blk_type_t blk_type;
    bcmi_field_ha_group_oper_t *grp_oper = NULL;
    bcmi_field_ha_group_oper_t *grp_oper_head = NULL;
    bcmi_field_ha_group_oper_t *grp_oper_temp = NULL;
    bcmi_field_ha_group_info_t *group_info = NULL;
    bcmi_field_ha_blk_info_t *group_hash = NULL;
    SHR_FUNC_ENTER(unit);


    /* Mask the stage bits from given group ID */
    group_id = group_id & BCM_FIELD_ID_MASK;

    group_info = stage_info->group_info;
    SHR_NULL_CHECK(group_info, SHR_E_INTERNAL);

    FP_GRP_HASH_BLK_ID(unit, stage_info->stage_id, ha_blk_id);
    hash_blk = FP_HA_BLK_HDR_PTR(unit, ha_blk_id);
    if (hash_blk == NULL) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    /* Calculate the hash */
    hash_offset = group_id % group_info->hash_size;

    group_hash = (bcmi_field_ha_blk_info_t *)
                  (hash_blk + sizeof(bcmi_field_ha_blk_hdr_t)) + hash_offset;
    ha_blk_id = group_hash->blk_id;
    ha_blk_offset = group_hash->blk_offset;
    FP_STAGE_GRP_OPER_BLK_TYPE(unit, stage_info->stage_id, blk_type);

    grp_oper_head = (bcmi_field_ha_group_oper_t *)
                      (FP_HA_BLK_HDR_PTR(unit, ha_blk_id) +
                       ha_blk_offset);
    grp_oper = grp_oper_head;
    do {
        if (grp_oper->group_id == group_id) {
            if (grp_oper_temp == NULL) {
                ha_blk_id = group_hash->blk_id;
                ha_blk_offset = group_hash->blk_offset;
                group_hash->blk_id = grp_oper->next.blk_id;
                group_hash->blk_offset = grp_oper->next.blk_offset;
            } else {
                ha_blk_id = grp_oper_temp->next.blk_id;
                ha_blk_offset = grp_oper_temp->next.blk_offset;
                grp_oper_temp->next.blk_id = grp_oper->next.blk_id;
                grp_oper_temp->next.blk_offset = grp_oper->next.blk_offset;
            }
            break;
        }
        grp_oper_temp = grp_oper;
        FP_HA_GRP_OPER_NEXT(unit, grp_oper, grp_oper);
    } while (grp_oper != NULL);

    /* Return not found if hash ptr is null */
    if (!grp_oper) {
        SHR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_ha_blk_element_release(unit, ha_blk_id,
                                             ha_blk_offset,
                                             blk_type));

    /* Clear the group id from the bmp */
    SHR_BITCLR(group_info->grpid_bmp.w, group_id);

exit:
    SHR_FUNC_EXIT();
}

/*
 * Remove All group IDs Operational database
 */
int
bcmint_field_group_oper_remove_all(
    int unit,
    bcmint_field_stage_info_t *stage_info)
{
    int group_id;
    uint32_t idx;
    uint8_t blk_id, ha_blk_id;
    uint32_t blk_offset, ha_blk_offset;
    void *hash_blk = NULL;
    bcmi_field_ha_blk_type_t blk_type;
    bcmi_field_ha_group_oper_t *grp_oper = NULL;
    bcmi_field_ha_group_info_t *group_info = NULL;
    bcmi_field_ha_blk_info_t *group_hash = NULL;
    SHR_FUNC_ENTER(unit);

    group_info = stage_info->group_info;
    if (group_info == NULL) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    FP_GRP_HASH_BLK_ID(unit, stage_info->stage_id, ha_blk_id);
    hash_blk = FP_HA_BLK_HDR_PTR(unit, ha_blk_id);

    for (idx = 0; idx < group_info->hash_size; idx++) {
         group_hash = (bcmi_field_ha_blk_info_t *)
                      (hash_blk + sizeof(bcmi_field_ha_blk_hdr_t)) + idx;

         ha_blk_id = group_hash->blk_id;
         if (ha_blk_id == BCM_FIELD_HA_BLK_ID_INVALID) {
             break;
         }
         ha_blk_offset = group_hash->blk_offset;
         FP_STAGE_GRP_HASH_BLK_TYPE(unit, stage_info->stage_id, blk_type);
         grp_oper = (bcmi_field_ha_group_oper_t *)
                    (FP_HA_BLK_HDR_PTR(unit, ha_blk_id) +
                     ha_blk_offset);
         do {
            blk_id = ha_blk_id;
            blk_offset = ha_blk_offset;
            group_id = grp_oper->group_id;

            if (grp_oper->next.blk_id != BCM_FIELD_HA_BLK_ID_INVALID) {
                grp_oper = (bcmi_field_ha_group_oper_t *)
                            (FP_HA_BLK_HDR_PTR(unit, grp_oper->next.blk_id) +
                             grp_oper->next.blk_offset);
                ha_blk_id = grp_oper->next.blk_id;
                ha_blk_offset = grp_oper->next.blk_offset;
            } else {
                grp_oper = NULL;
            }

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmint_field_ha_blk_element_release(unit, blk_id,
                                                     blk_offset,
                                                     blk_type));
            /* Clear the group id from the bmp */
            SHR_BITCLR(group_info->grpid_bmp.w, group_id);
        } while (grp_oper != NULL);
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * Function to retrieve the Operational Mode Status.
 */
int
bcmint_field_oper_mode_status_get(int unit,
                                  bcmint_field_stage_info_t *stage_info,
                                  int *mode)
{
    int dunit = 0;
    uint64_t oper_mode = 0x0;
    bcmlt_entry_handle_t oper_mode_template = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(stage_info, SHR_E_INTERNAL);
    SHR_NULL_CHECK(mode, SHR_E_INTERNAL);
    dunit = bcmi_ltsw_dev_dunit(unit);

    if (stage_info->flags & BCMINT_FIELD_STAGE_PIPE_MODE_NOT_SUPPORTED) {
        *mode = bcmiFieldOperModeGlobal;
        SHR_EXIT();
    }

    /* Entry handle allocate for operational mode. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit,
                              stage_info->tbls_info->oper_mode_sid,
                              &oper_mode_template));

    /* Lookup mode template using handle. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, oper_mode_template,
                              BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));

    /* Retreive operation mode field from fetched entry. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(oper_mode_template,
                               stage_info->tbls_info->oper_mode_fid,
                               &oper_mode));

    *mode = oper_mode;

exit:
    if (BCMLT_INVALID_HDL != oper_mode_template) {
        (void) bcmlt_entry_free(oper_mode_template);
    }
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_field_oper_mode_status_get(int unit,
                                     bcmi_ltsw_field_stage_t stage,
                                     int *mode)
{
    bcmint_field_stage_info_t *stage_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_stage_info_get(unit, stage,
                                    &stage_info));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_oper_mode_status_get(unit,
                                           stage_info,
                                           mode));
exit:
    SHR_FUNC_EXIT();
}

int
bcmint_field_group_policer_pool_alloc(int unit,
                                      bcmint_field_stage_info_t *stage_info,
                                      int group_pipe,
                                      int group_priority,
                                      int *pool_id)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_policer_pool_alloc(unit,
                                      stage_info->stage_id,
                                      group_pipe,
                                      group_priority,
                                      pool_id));
exit:
    SHR_FUNC_EXIT();
}

int
bcmint_field_group_policer_pool_free(int unit,
                                     bcmint_field_stage_info_t *stage_info,
                                     int pipe,
                                     int pool_id)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_policer_pool_free(unit,
                                     stage_info->stage_id,
                                     pipe,
                                     pool_id));

exit:
    SHR_FUNC_EXIT();
}

int
bcmint_field_group_policer_prio_update(int unit,
                                       bcmint_field_stage_info_t *stage_info,
                                       int pipe,
                                       int pool_id,
                                       int group_priority)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_policer_pool_prio_update(unit,
                                            stage_info->stage_id,
                                            pipe,
                                            pool_id,
                                            group_priority));

exit:
    SHR_FUNC_EXIT();
}


/*
 * Function to validate group mode.
 */
int
bcmint_field_group_mode_validate(int unit,
                                 bcmlt_entry_handle_t grp_template,
                                 bcm_field_group_mode_t mode,
                                 char **group_mode)
{
    SHR_FUNC_ENTER(unit);

    if (mode == bcmFieldGroupModeSingle) {
        *group_mode = "SINGLE";
    } else if (mode == bcmFieldGroupModeIntraSliceDouble) {
        *group_mode = "DBLINTRA";
    } else if (mode == bcmFieldGroupModeDouble) {
        *group_mode = "DBLINTER";
    } else if (mode == bcmFieldGroupModeTriple) {
        *group_mode = "TRIPLE";
    } else if (mode == bcmFieldGroupModeQuad) {
        *group_mode = "QUAD";
    } else {
         LOG_DEBUG(BSL_LS_BCM_FP,
             (BSL_META_U(unit, "Error: Not a valid"
                               " FP group mode. \n")));
         SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
       (bcmlt_entry_field_symbol_add(grp_template,
                                     "MODE",
                                     *group_mode));

exit:
    SHR_FUNC_EXIT();
}

static int
ltsw_field_group_compression_add(int unit,
                                 bcmint_field_stage_info_t *stage_info,
                                 uint32_t grp_flags,
                                 bcm_field_qset_t *qset,
                                 bcmlt_entry_handle_t grp_template)
{
    SHR_FUNC_ENTER(unit);

    if (!(stage_info->flags & BCMINT_FIELD_STAGE_COMPRESSION_SUPPORTED)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_field_compression_set(unit, stage_info, NULL,
                                         qset, grp_flags, grp_template));
exit:
    SHR_FUNC_EXIT();
}

static int
ltsw_field_group_strength_insert(
    int unit,
    bcmint_field_stage_info_t *stage_info,
    int pipe,
    int group_prio,
    bcmint_field_action_strength_prof_t *str_prof)
{
    bcmint_field_action_strength_prof_t *base_ptr = NULL;

    SHR_FUNC_ENTER(unit);
    base_ptr = stage_info->strength_base_prof[pipe].strength_prof;
    if (base_ptr == NULL) {
        stage_info->strength_base_prof[pipe].strength_prof = str_prof;
    }

    /* Insert the strength profile in the priority order */
    for (; base_ptr != NULL; base_ptr = base_ptr->next) {
        if (base_ptr->group_priority == group_prio) {
            if (base_ptr->next != NULL) {
                base_ptr->next->prev = str_prof;
            }
            str_prof->next = base_ptr->next;
            str_prof->prev = base_ptr;
            base_ptr->next = str_prof;
            break;
        } else if (base_ptr->group_priority < group_prio) {
            str_prof->next = base_ptr;
            str_prof->prev = base_ptr->prev;
            base_ptr->prev = str_prof;
            if (str_prof->prev == NULL) {
               stage_info->strength_base_prof[pipe].strength_prof = str_prof;
            } else {
               str_prof->prev->next = str_prof;
            }
            break;
        } else if (base_ptr->next == NULL) {
            str_prof->prev = base_ptr;
            base_ptr->next = str_prof;
            break;
        }
    }

    SHR_FUNC_EXIT();
}

/* Remove strength profile from stage linked list */
static int
ltsw_fp_group_strength_prof_dealloc(
    int unit,
    bcmint_field_stage_info_t *stage_info,
    int pipe,
    bool color,
    uint32_t strength_prof_id)
{
    bool found = FALSE;
    bcmint_field_action_strength_prof_t *strength_prof = NULL;

    SHR_FUNC_ENTER(unit);

    strength_prof = stage_info->strength_base_prof[pipe].strength_prof;
    for (; strength_prof != NULL; strength_prof = strength_prof->next) {
        if ((strength_prof->color == color) &&
            (strength_prof->strength_prof_idx == strength_prof_id)) {
            if (strength_prof->next) {
                strength_prof->next->prev = strength_prof->prev;
            }

            if (strength_prof->prev) {
                strength_prof->prev->next = strength_prof->next;
            }

            if (strength_prof == stage_info->strength_base_prof[pipe].strength_prof) {
                stage_info->strength_base_prof[pipe].strength_prof = strength_prof->next;
            }
            found = TRUE;
            break;
        }
    }

    if (found == FALSE) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }

    BCMINT_FIELD_MEM_FREE(strength_prof->action_strength);
    BCMINT_FIELD_MEM_FREE(strength_prof);

exit:
    SHR_FUNC_EXIT();
}

static int
ltsw_fp_group_default_strength_add(int unit,
                bcmint_field_stage_info_t *stage_info,
                uint64_t *strength_idx)
{
    int dunit = 0;
    bcmlt_entry_handle_t sbr_template = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    /* Entry handle allocate for Presel Group Template */
    SHR_IF_ERR_VERBOSE_EXIT
       (bcmlt_entry_allocate(dunit,
                             stage_info->tbls_info->sbr_sid,
                             &sbr_template));

    /* Add sbr_id field to the sbr template. */
    SHR_IF_ERR_VERBOSE_EXIT
       (bcmlt_entry_field_add(sbr_template,
                              stage_info->tbls_info->sbr_key_fid,
                              stage_info->sbr_max_limit));

    /* Insert/update sbr template */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_set_commit(unit, sbr_template,
                                  BCMLT_PRIORITY_NORMAL));

    *strength_idx = stage_info->sbr_max_limit;

exit:
    if (sbr_template != BCMLT_INVALID_HDL) {
        (void)(bcmlt_entry_free(sbr_template));
    }
    SHR_FUNC_EXIT();
}

static int
ltsw_fp_group_color_pdd_template_delete(
    int unit,
    bcmint_field_stage_info_t *stage_info,
    int pipe,
    int color_pdd_idx)
{
    uint8_t ha_blk_id;
    bool delete_prof = false;
    void *ha_ptr = NULL;
    bcmi_field_ha_profile_info_t *profile_ptr = NULL;
    bcmlt_entry_handle_t color_pdd_template = BCMLT_INVALID_HDL;
    bcmint_field_color_action_tbls_info_t *color_tbl_info = NULL;

    SHR_FUNC_ENTER(unit);

    if (NULL == stage_info) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }
    color_tbl_info = stage_info->color_action_tbls_info;
    SHR_NULL_CHECK(color_tbl_info, SHR_E_INTERNAL);

    if (color_pdd_idx != -1) {
        FP_COLOR_TBL_PDD_INFO_BLK_ID(unit, stage_info->stage_id, ha_blk_id);
        ha_ptr = FP_HA_BLK_HDR_PTR(unit, ha_blk_id);
        profile_ptr = (bcmi_field_ha_profile_info_t *)
            (ha_ptr + sizeof(bcmi_field_ha_blk_hdr_t));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_field_color_profile_id_free(
                    unit,
                    color_tbl_info->pdd_tbl_sid,
                    pipe,
                    profile_ptr,
                    color_pdd_idx,
                    -1,
                    -1,
                    &delete_prof));
        if (delete_prof == true) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmint_field_lt_entry_commit(
                        unit, &color_pdd_template,
                        BCMLT_OPCODE_DELETE,
                        BCMLT_PRIORITY_NORMAL,
                        color_tbl_info->pdd_tbl_sid,
                        color_tbl_info->pdd_tbl_key_fid,
                        0, color_pdd_idx, NULL,
                        (pipe != -1) ? color_tbl_info->pipe_fid: NULL,
                        pipe,
                        0, 0,
                        1));
        }
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmint_field_find_is_subset_qset(
        const bcm_field_qset_t *qset_1,
        const bcm_field_qset_t *qset_2)
{
    int                 idx;

    for (idx = 0; idx < _SHR_BITDCLSIZE(BCM_FIELD_QUALIFY_MAX); idx++) {
        if ((qset_1->w[idx] | qset_2->w[idx]) & ~qset_2->w[idx]) {
            return (FALSE);
        }
    }
    return (TRUE);
}

static int
ltsw_field_group_create_validate(int unit,
                                 bcmint_field_stage_info_t *stage_info,
                                 bcm_field_group_config_t *gc,
                                 bool clear)
{
    int rv;
    int entry;
    bcm_field_entry_t entry_id;
    bcm_field_entry_config_t entry_config;
    bcmi_field_ha_entry_oper_t *entry_oper = NULL;

    SHR_FUNC_ENTER(unit);

    entry = stage_info->entry_info->eid_max;
    for (; entry > 0; entry--) {
        if (SHR_BITGET(stage_info->entry_info->eid_bmp.w, entry)) {
            continue;
        }
        break;
    }

    if (entry == 0) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_RESOURCE);
    }
    entry_id = entry;
    entry = stage_info->stage_base_id | entry;

    bcm_field_entry_config_t_init(&entry_config);
    entry_config.group_id = gc->group;
    entry_config.entry_id = entry;
    entry_config.flags |= BCM_FIELD_ENTRY_CREATE_WITH_ID;

    rv = bcmint_field_entry_config_create(unit,
                                     &entry_config);

    if (SHR_FAILURE(rv)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
    }
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_entry_oper_lookup(unit,
                                        entry_id,
                                        stage_info,
                                        &entry_oper));
    entry_oper->entry_flags |= BCMINT_FIELD_ENTRY_TEMP_FOR_SLICE_INFO;

    rv = (bcmint_field_entry_install(unit, entry));
    if (SHR_FAILURE(rv)) {
        if ((rv == _SHR_E_EXISTS) && (clear == TRUE)) {
            rv = _SHR_E_NONE;
        } else {
            (void) bcmint_field_entry_destroy(unit, entry, NULL);
            SHR_IF_ERR_VERBOSE_EXIT(rv);
        }
    }

    if (clear == TRUE) {
        rv = bcmint_field_entry_destroy(unit, entry, NULL);
        if (SHR_FAILURE(rv)) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
        }
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_field_entry_enable_set(unit, entry, 0));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief bcmint_field_group_config_create
 *
 * Create a group with a mode (single, double, etc.), a port bitmap,
 * group size and a Group ID.
 * \param [in] unit         - BCM device number.
 * \param [in] group_config - Group create attributes namely:
 * \param [in] flags        - Bits indicate which parameters have been
 *                             passed to API and should be used during group
 *                             creation.
 * \param [in] qset        - Field qualifier set
 * \param [in] priority    - Priority within allowable range,
 *                             or BCM_FIELD_GROUP_PRIO_ANY to automatically
 *                             assign a priority; each priority value may be
 *                             used only once
 * \param [in] mode        - Group mode (single, double, triple or Auto-wide)
 * \param [in] ports       - Ports where group is defined
 * \param [out] group      - Requested Group ID. If Group ID is not set,
 *                              then API allocates and returns the created
 *                              Group ID.
 * Returns:
 *     BCM_E_INIT      - BCM unit not initialized
 *     BCM_E_RESOURCE  - no select codes will satisfy qualifier set
 *     BCM_E_NONE      - Success
 */

int
bcmint_field_group_config_create(int unit,
                                 bcm_field_group_config_t *group_config,
                                 uint32 flags)
{
    bool enable = FALSE;
    bool clear = FALSE;
    int dunit = 0, i = 0, index = 0, rv = 0;
    int grp_pipe = -1;
    int act_pipe = -1;
    uint8_t grp_based_sbr = 1;
    int color_pdd_idx = -1;
    int color_sbr_idx = -1;
    int policer_pool_id = -1;
    uint16_t qual_count = 0;
    uint32_t presel_count = 0;
    uint64_t presel_id[32] = {0};
    char *group_mode = NULL;
    uint64_t strength_idx = 0x0;
    bcm_field_group_t group_id = 0;
    bool group_created = FALSE;
    bool group_template_created = FALSE;
    bool mode_auto = 0, group_id_found = FALSE;
    int presel_group = 0, stage_oper_mode = 0;
    int color_action_mode = 0;
    bcmint_field_tbls_info_t *tbls_info = NULL;
    bcmint_field_stage_info_t *stage_info = NULL;
    bcmi_ltsw_field_stage_t stage = bcmiFieldStageCount;
    bcmlt_entry_handle_t grp_template = BCMLT_INVALID_HDL;
    bcmlt_entry_handle_t pdd_template = BCMLT_INVALID_HDL;
    bcmi_field_ha_group_oper_t *group_oper = NULL;
    bcmi_field_ha_presel_oper_t *presel_oper = NULL;
    bool group_pdd_template_created = FALSE;
    bool group_hint_incr = FALSE;
    bool reserved_group = FALSE;
    bcmi_field_ha_group_oper_t *oper_temp = NULL;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);

    /* Retreive stage_id to which group belongs from qset. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_group_stage_get(unit,
                                      &(group_config->qset),
                                      &stage));

    /* Retreive stage info from stage id. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_stage_info_get(unit, stage,
                                    &stage_info));
    tbls_info = stage_info->tbls_info;

    if ((stage != bcmiFieldStageEgress) &&
        ((group_config->mode_type != bcmFieldGroupModeTypeAuto) ||
        (group_config->packet_type != bcmFieldGroupPacketTypeDefault))) {

            LOG_DEBUG(BSL_LS_BCM_FP,
               (BSL_META_U(unit, "Error: Mode_type and Packet_type"
                             " are valid only for EFP groups.\n")));
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    /* Retreive operational mode of stage first. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_oper_mode_status_get(unit,
                                stage_info, &stage_oper_mode));
    /* InPorts cannot be added as qset in global mode. */
    if (stage_oper_mode == bcmiFieldOperModeGlobal) {
        if (BCM_FIELD_QSET_TEST(group_config->qset, bcmFieldQualifyInPorts)) {
            LOG_DEBUG(BSL_LS_BCM_FP,
               (BSL_META_U(unit, "Error: Inports qualifier is not"
                             " not supported in global mode.\n")));
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }
    }

    /* Entry handle allocate for Group Template */
    SHR_IF_ERR_VERBOSE_EXIT
       (bcmlt_entry_allocate(dunit,
                             tbls_info->group_sid,
                             &grp_template));

    /* Check for group ID. if exists check if it is in range. */
    if (group_config->flags & BCM_FIELD_GROUP_CREATE_WITH_ID) {
        /* Reserved Ids are allocated as higher group IDs.
         * if flag passed is BCMINT_FIELD_GROUP_CREATE_WITH_RESERVE_ID,
         * then validate the reserve Id passed.
         */
        if (flags & BCMINT_FIELD_GROUP_CREATE_WITH_RESERVE_ID) {
            if (((group_config->group) > (stage_info->stage_base_id |
                stage_info->group_info->gid_max)) ||
                ((group_config->group) <= ((stage_info->stage_base_id |
                stage_info->group_info->gid_max) -
                BCMINT_FIELD_GROUP_RESERVE_GID_COUNT))) {
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
            }
            reserved_group = TRUE;
        } else {
            /* Verify whether the given ID is in group ID's range */
            if (!((group_config->group > stage_info->stage_base_id) &&
                  (group_config->group <=
                  (((stage_info->stage_base_id) |
                  (stage_info->group_info->gid_max))-
                  BCMINT_FIELD_GROUP_RESERVE_GID_COUNT)))) {
                LOG_DEBUG(BSL_LS_BCM_FP,
                    (BSL_META_U(unit,
                    "Error: This Group ID is not in valid range\n")));
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
            }
        }

        /* Mask the stage bits from given group ID */
        group_id = group_config->group & BCM_FIELD_ID_MASK;

        /* Check whether it is already in use. */
        if (SHR_BITGET(stage_info->group_info->grpid_bmp.w, group_id)) {
            LOG_DEBUG(BSL_LS_BCM_FP,
                (BSL_META_U(unit,
                "Error: This Group ID is already in use\n")));
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_EXISTS);
        }
    } else {
        /* Generate group ID */
        group_id = stage_info->group_info->last_allocated_gid + 1;
        for (index = 1;
             index <= ((stage_info->group_info->gid_max)-
             BCMINT_FIELD_GROUP_RESERVE_GID_COUNT); index++) {
            if (group_id > ((stage_info->group_info->gid_max)-
                BCMINT_FIELD_GROUP_RESERVE_GID_COUNT)) {
                group_id = 1;
            }
            if (!SHR_BITGET(stage_info->group_info->grpid_bmp.w, group_id)) {
                group_id_found = TRUE;
                break;
            }
            group_id++;
        }

        if (group_id_found == FALSE) {
            LOG_DEBUG(BSL_LS_BCM_FP,
                (BSL_META_U(unit,
                "Error: All Group ID's are in use\n")));
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_RESOURCE);
        }

        group_config->group = (stage_info->stage_base_id | group_id);
        stage_info->group_info->last_allocated_gid = group_id;
    }

    /* Add group_template_id field to Group LT. */
    SHR_IF_ERR_VERBOSE_EXIT
       (bcmlt_entry_field_add(grp_template,
                              tbls_info->group_key_fid,
                              group_id));

    /*
     * If group priority is BCM_FIELD_GROUP_PRIO_ANY,
     * generate a new unique priority.
     */
    if (group_config->priority == BCM_FIELD_GROUP_PRIO_ANY) {

        stage_info->group_info->last_allocated_prio += 1;
        group_config->priority = stage_info->group_info->last_allocated_prio;
    }

    /* Get priority and update the group LT priority field. */
    if (group_config->priority >= 0) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(grp_template,
                              "ENTRY_PRIORITY",
                              group_config->priority));
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_field_group_mode_set(unit, group_config,
                                        stage_info, grp_template,
                                        &mode_auto, &group_mode));
    /* Group Mode shouldn't be NULL. As it needs to be stored in group oper */
    SHR_NULL_CHECK(group_mode, SHR_E_INTERNAL);

    /*
     * Process qset and hints, update fields in grp template.
     * construct the qual bitmap structure.
     */

    /* Verify requested QSET is supported by the stage. */
    if (FALSE == bcmint_field_qset_is_subset(
                  unit, &group_config->qset, stage_info, 0, &qual_count)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_process_group_qset(unit,
                                   &group_config->qset,
                                   group_config->hintid,
                                   stage_info, 1, 0,
                                   &grp_template));

    /*
     * Handle group config ports for per pipe configuration.
     */
    if (group_config->flags & BCM_FIELD_GROUP_CREATE_WITH_PORT) {
        uint8_t pipe = 0;

        /* Get pipe to be configured based on ports
         * passed in group config structure. */
        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_field_group_pipe_get(unit, stage_info,
                                            group_config->ports,
                                            &pipe));
        grp_pipe = pipe;

        /* Add pipe field value in group template. */
        if (stage_oper_mode == bcmiFieldOperModePipeUnique) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(grp_template, "PIPE", grp_pipe));
        }
    } else {
        if (stage_oper_mode == bcmiFieldOperModePipeUnique) {
            /* If group is not created with ports, then group cannot
             * be created in a stage in pipe unique mode. */
            LOG_DEBUG(BSL_LS_BCM_FP,
               (BSL_META_U(unit, "Error: Stage cannot be in per pipe mode"
                         " when group is created without"
                         " BCM_FIELD_GROUP_CREATE_WITH_PORT flag.\n")));
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }
    }

    if ((stage_info->flags & BCMINT_FIELD_STAGE_POLICER_ACTION_SUPPORT) &&
        (stage_info->flags & BCMINT_FIELD_STAGE_POLICY_TYPE_PDD) &&
        (BCM_FIELD_ASET_TEST
         (group_config->aset, bcmFieldActionPolicerGroup))) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_field_color_action_mode_get(unit,
                                                stage_info,
                                                &color_action_mode));
        if ((stage_oper_mode == bcmiFieldOperModePipeUnique) &&
            (color_action_mode != bcmiFieldOperModePipeUnique)) {
            LOG_DEBUG(BSL_LS_BCM_FP,
                      (BSL_META_U(unit, "Error: Meter action tables cannot "
                                  "be in global mode when FP stage is "
                                  "in per pipe mode.\n")));
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_CONFIG);
        }

        if ((stage_oper_mode != bcmiFieldOperModePipeUnique) &&
            (color_action_mode == bcmiFieldOperModePipeUnique)) {
            LOG_DEBUG(BSL_LS_BCM_FP,
                      (BSL_META_U(unit, "Error: Meter action tables cannot "
                                  "be in per pipe mode when FP stage is not "
                                  "in per pipe mode.\n")));
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_CONFIG);
        }

        if (stage_oper_mode == bcmiFieldOperModePipeUnique) {
            act_pipe = grp_pipe;
        } else {
            act_pipe = -1;
        }
    }

    /*
     * Validate and Add the compression support.
     */
    if (group_config->flags & BCM_FIELD_GROUP_CREATE_WITH_COMPRESSION) {
        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_field_group_compression_add(unit, stage_info,
                                              group_config->flags,
                                              &(group_config->qset),
                                              grp_template));
    }

    /*
     * Check for the presel group support, create the default presel
     * group if presel group ID is not mentioned.
     */
    if ((stage_info->flags & BCMINT_FIELD_STAGE_PRESEL_KEY_DYNAMIC) &&
        (group_config->flags & BCM_FIELD_GROUP_CREATE_WITH_PRESELSET)) {
        if (group_config->flags & BCM_FIELD_GROUP_CREATE_WITH_PRESEL_GROUP) {
            presel_group = group_config->presel_group;
            /* Verify whether the given ID is in group ID's range */
            if (!((presel_group > stage_info->stage_base_id) &&
                  (presel_group <= (stage_info->stage_base_id +
                                    stage_info->presel_info->gid_max)))) {
                LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META_U(unit,
                   "Error: This Presel Group ID[0x%x] is not in valid range\n"),
                                                                 presel_group));
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
            }

            presel_group = presel_group & BCM_FIELD_ID_MASK;
            /* Lookup Presel group database */
            if (SHR_BITGET(stage_info->presel_info->pgid_bmp.w,
                           presel_group) == 0) {
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
            }
        } else {
            /* Defalut presel group creation is not enabled as
             * the total size of presel is more than the max allowed width
             */
            LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META_U(unit,
                "Error: The presel group is not created.\n")));
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_CONFIG);
#if 0
            /* Insert default presel group */
            SHR_IF_ERR_VERBOSE_EXIT
                 (bcmint_field_presel_default_group_create(unit, stage_info,
                                                           &presel_group));
#endif
        }

        /* Add presel_grp_template_id field to Group LT. */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(grp_template,
                                   tbls_info->presel_group_key_fid,
                                   presel_group));
    }

    if ((stage_info->flags & BCMINT_FIELD_STAGE_POLICY_TYPE_PDD) &&
            (BCM_FIELD_ASET_TEST
             (group_config->aset, bcmFieldActionPolicerGroup))) {
        if (stage_info->flags & BCMINT_FIELD_STAGE_POLICER_POOL_HINT_SUPPORT) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmint_field_group_policer_pool_get(unit,
                                                     group_id,
                                                     group_config->hintid,
                                                     &policer_pool_id));
        }
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_field_group_policer_pool_alloc(unit,
                                                   stage_info,
                                                   grp_pipe,
                                                   group_config->priority,
                                                   &policer_pool_id));
    }

    /* Handle SBR based on the group's priority */
    if (stage_info->flags & BCMINT_FIELD_STAGE_ACTION_RESOLUTION_SBR) {
        if (!(group_config->flags & BCM_FIELD_GROUP_ENT_LVL_ACTION_RESOLVE)) {
            /* Create SBR strength profile */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmint_field_group_strength_prof_create(
                                            unit, stage_info,
                                            (grp_pipe == -1) ? 0 : grp_pipe,
                                            group_config->priority,
                                            group_config->hintid,
                                            policer_pool_id,
                                            &group_config->aset,
                                            &strength_idx,
                                            &color_sbr_idx));

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(grp_template,
                                       tbls_info->sbr_key_fid,
                                       strength_idx));
        } else {
            grp_based_sbr = 0;
        }
    }

    /* Presel flag and presel set handling, nof of presels. */
    if (group_config->flags & BCM_FIELD_GROUP_CREATE_WITH_PRESELSET) {
        if ((stage_info->flags & BCMINT_FIELD_STAGE_KEY_TYPE_PRESEL) == 0) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }

        /* Presel ID attached to group config */
        for (i = 1; i < BCM_FIELD_PRESEL_SEL_MAX; i++) {
            if (SHR_BITGET(group_config->preselset.w, i)) {
                /* Check whether there exists a valid presel with this ID */
                if (SHR_BITGET(
                            stage_info->presel_info->pid_bmp.w, i)) {
                    if ((group_config->flags &
                          BCM_FIELD_GROUP_CREATE_WITH_PRESEL_GROUP) &&
                          (stage_info->flags &
                           BCMINT_FIELD_STAGE_PRESEL_KEY_DYNAMIC)) {
                        SHR_IF_ERR_VERBOSE_EXIT
                            (bcmint_field_presel_oper_lookup(unit, i,
                                                         stage_info,
                                                         &presel_oper));
                        if (presel_oper->group_id != 0) {
                            LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META_U(unit,
                                      "Error: Not a validPresel group Id"
                                      " is already in use.%d \n"), i));
                            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_CONFIG);
                        }
                        /* Validate presel group qset */
                        SHR_IF_ERR_VERBOSE_EXIT
                            (bcmint_presel_qual_validate(unit,
                                                        stage_info,
                                                        presel_group,
                                                        i));
                        presel_oper->group_id = presel_group;
                    }
                    presel_id[presel_count] = i;
                    presel_count++;

                } else {
                    LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META_U(unit,
                            "Error: Not a valid"
                            " Presel ID.%d \n"), i));
                    SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
                }
            }
        }
        if (presel_count > 0) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_array_add(grp_template,
                                            tbls_info->grp_presel_entry_key_fid,
                                            0, presel_id, presel_count));
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(grp_template,
                                       tbls_info->presel_entry_count_fid,
                                       presel_count));
        }
    }

    if (reserved_group == FALSE) {
        /* go thru the groups present*/
        for (index = 1;
             index <= ((stage_info->group_info->gid_max)-
             BCMINT_FIELD_GROUP_RESERVE_GID_COUNT); index++) {
            if (SHR_BITGET(stage_info->group_info->grpid_bmp.w, index)) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmint_field_group_oper_lookup(unit, index,
                                                    stage_info,
                                                    &oper_temp));
                if (stage_oper_mode == bcmiFieldOperModePipeUnique) {
                    int pipe = -1;
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmint_field_group_pipe_get(unit, stage_info,
                                                     oper_temp,
                                                     &pipe));
                    if (grp_pipe != pipe) {
                        continue;
                    }
                }
                if (stage_info->flags & BCMINT_FIELD_STAGE_SINGLE_SLICE_ONLY) {
                    /*
                     * incase of stage em,cannot create group with different prio
                     * and can have multiple groups only if presel are present
                     */
                    if (group_config->priority != oper_temp->priority) {
                        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
                    }
                } else if (group_config->priority != oper_temp->priority) {
                    continue;
                }

                /*
                 * Same priority groups can't exist without presel.
                 */
                if ((!(stage_info->flags & BCMINT_FIELD_STAGE_KEY_TYPE_FIXED)) &&
                   ((oper_temp->presel_cnt == 0) ||
                    (presel_count == 0))) {
                    SHR_IF_ERR_VERBOSE_EXIT(SHR_E_CONFIG);
                }
            }
        }
    }

    /* Process aset and insert PDD template. */
    if (stage_info->flags & BCMINT_FIELD_STAGE_POLICY_TYPE_PDD) {
        /* Entry handle allocate for PDD Template */
        SHR_IF_ERR_VERBOSE_EXIT
           (bcmlt_entry_allocate(dunit,
                                 tbls_info->pdd_sid,
                                 &pdd_template));

        /* Add pdd_template_id field to Group LT. */
        SHR_IF_ERR_VERBOSE_EXIT
           (bcmlt_entry_field_add(grp_template,
                                  tbls_info->pdd_key_fid,
                                  group_id));

        /* Add pdd_template_id field to PDD LT. */
        SHR_IF_ERR_VERBOSE_EXIT
           (bcmlt_entry_field_add(pdd_template,
                                  tbls_info->pdd_key_fid,
                                  group_id));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_field_process_group_aset(unit,
                                       stage_info, 1,
                                       act_pipe,
                                       &group_config->aset,
                                       group_config->hintid,
                                       &pdd_template,
                                       policer_pool_id,
                                       &color_pdd_idx));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_commit(unit, pdd_template,
                                  BCMLT_OPCODE_INSERT,
                                  BCMLT_PRIORITY_NORMAL));
        group_pdd_template_created = TRUE;
    }

    if (group_config->flags & BCM_FIELD_GROUP_CREATE_SMALL) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_symbol_add(grp_template,
                                   "GRP_SLICE_TYPE", "SMALL"));
    } else if (group_config->flags & BCM_FIELD_GROUP_CREATE_LARGE) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_symbol_add(grp_template,
                                   "GRP_SLICE_TYPE", "LARGE"));
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, grp_template,
                              BCMLT_OPCODE_INSERT,
                              BCMLT_PRIORITY_NORMAL));
    group_template_created = TRUE;

    /* Increment hint associated to group */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_hint_group_count_update(unit,
                            group_config->hintid, 1));

    group_hint_incr = TRUE;


    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_field_group_oper_insert(unit, stage_info,
                                      group_id, group_mode,
                                      group_config,
                                      &group_oper));

    /* Update Strength profile index in group operational structure */
    if (strength_idx != 0) {
        group_oper->strength_prof_id = strength_idx;
    } else if (grp_based_sbr == 0) {
        group_oper->group_flags |= BCMINT_FIELD_GROUP_ENT_LVL_ACTION_RESOLVE;
    }

    /* Update Presel Group Id */
    if (presel_group != 0) {
        group_oper->presel_group = presel_group;
    }

    if (presel_count) {
        for (index = 0; index < presel_count; index++) {
            group_oper->presel_arr[index] = presel_id[index];
        }
        group_oper->presel_cnt = presel_count;
        group_oper->group_flags |= BCMINT_FIELD_GROUP_CREATED_WITH_PRESEL;
    }

    /* Update Color PDD Table Id */
    group_oper->color_pdd_id = color_pdd_idx;

    /* Update Color SBR Table Id */
    group_oper->color_sbr_id = color_sbr_idx;

    group_oper->policer_pool_id = policer_pool_id;

    /* Set the group id in bmp */
    SHR_BITSET(stage_info->group_info->grpid_bmp.w, group_id);
    group_created = TRUE;

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_field_internal_entry_enable_get(unit, stage_info, &enable));

    if (!(flags & BCMINT_FIELD_GROUP_CREATE_WITH_RESERVE_ID)) {
        /*
         * At this level, it is not sure that the created group is valid
         * That is, mode configured for the given QSET is supported
         * or there are free slices available for the allocation.
         * In order to provide the legacy behavaior, a dummy entry needs to
         * be created and installed to ensure the mode and slice can be
         * allocated.
         */
        if (stage_info->flags & BCMINT_FIELD_STAGE_FLEXSTAT_SLICE_INFO) {
            clear = FALSE;
        } else {
            clear = TRUE;
        }
        rv = ltsw_field_group_create_validate(unit, stage_info,
                                              group_config, clear);
        if (SHR_FAILURE(rv)) {
            group_created = TRUE;
            SHR_IF_ERR_VERBOSE_EXIT(rv);
        }
    }

exit:
    if (SHR_FUNC_ERR()) {
        if (group_created == TRUE) {
            (void) bcmint_field_group_destroy(unit, group_config->group);
        } else {
            if (group_hint_incr) {
                /* Decrement hint associated to group */
                (void)(bcmint_field_hint_group_count_update(unit,
                                              group_config->hintid, 0));
            }
            if (group_template_created) {
                (void)(bcmi_lt_entry_commit(unit, grp_template,
                                            BCMLT_OPCODE_DELETE,
                                            BCMLT_PRIORITY_NORMAL));
            }

            if (group_pdd_template_created) {
                (void)(bcmi_lt_entry_commit(unit, pdd_template,
                                            BCMLT_OPCODE_DELETE,
                                            BCMLT_PRIORITY_NORMAL));
            }
            if ((strength_idx != 0) &&
                (strength_idx != stage_info->sbr_max_limit)) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmint_field_group_strength_prof_destroy(
                            unit,
                            stage_info,
                            (grp_pipe == -1) ? 0 : grp_pipe,
                            group_config->priority,
                            strength_idx,
                            color_sbr_idx));
            }
            if ((presel_group != 0) &&
                (presel_group == stage_info->presel_group_def)) {
                /* Destroy default presel group */
                (void)(bcmint_field_presel_default_group_destroy(
                           unit, stage_info));
            }

            if ((reserved_group == FALSE) &&
                (group_config->flags & BCM_FIELD_GROUP_CREATE_WITH_PRESELSET)) {
                for (i = 0; i < presel_count; i++) {
                     if ((group_config->flags &
                            BCM_FIELD_GROUP_CREATE_WITH_PRESEL_GROUP) &&
                            (stage_info->flags &
                            BCMINT_FIELD_STAGE_PRESEL_KEY_DYNAMIC)) {
                           (void)(bcmint_field_presel_oper_lookup(unit,
                                    presel_id[i],
                                    stage_info,
                                    &presel_oper));
                         presel_oper->group_id = 0;
                     }
                }
            }

            if (color_pdd_idx != -1) {
                (void) (ltsw_fp_group_color_pdd_template_delete(
                        unit,stage_info, act_pipe, color_pdd_idx));
            }

            if ((stage_info->flags & BCMINT_FIELD_STAGE_POLICY_TYPE_PDD)
                    && (policer_pool_id != -1)) {
                (void)(bcmint_field_group_policer_pool_free(
                    unit, stage_info, grp_pipe,
                    policer_pool_id));
            }
        }
    }

    if (BCMLT_INVALID_HDL != grp_template) {
        (void) bcmlt_entry_free(grp_template);
    }

    if (BCMLT_INVALID_HDL != pdd_template) {
        (void) bcmlt_entry_free(pdd_template);
    }
    SHR_FUNC_EXIT();
}

int
bcmint_field_group_pipe_get(
    int unit,
    bcmint_field_stage_info_t *stage_info,
    bcmi_field_ha_group_oper_t *group_oper,
    int *pipe)
{
    int oper_mode = 0;
    uint64_t pipe_val = 0;
    bcmlt_entry_handle_t grp_template = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_oper_mode_status_get(unit, stage_info, &oper_mode));

    if (oper_mode == bcmiFieldOperModePipeUnique) {
        /* Group template lookup */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_field_lt_entry_commit(unit,
                                          &grp_template,
                                          BCMLT_OPCODE_LOOKUP,
                                          BCMLT_PRIORITY_NORMAL,
                                          stage_info->tbls_info->group_sid,
                                          stage_info->tbls_info->group_key_fid,
                                          0, group_oper->group_id, NULL,
                                          NULL, 0, 0, 0, 0));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(grp_template,
                                   "PIPE",
                                   &pipe_val));
        *pipe = (int)pipe_val;
    } else {
        *pipe = -1;
    }

exit:
    if (BCMLT_INVALID_HDL != grp_template) {
        (void) bcmlt_entry_free(grp_template);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief bcmint_field_group_destroy
 *
 * Destroy group with provided id.
 *
 * \param [in] unit     - BCM device number.
 * \param [in] group_id - Group Identifier.
 *
 * Returns:
 *     BCM_E_INIT      - BCM unit not initialized
 *     BCM_E_NONE      - Success
 */
int
bcmint_field_group_destroy(int unit,
                           bcm_field_group_t group)
{
    int dunit = 0;
    int rv = 0;
    int group_id = 0;
    int entry_count = 0;
    int pipe = -1;
    int act_pipe = -1;
    bcmint_field_tbls_info_t *tbls_info = NULL;
    bcmint_field_stage_info_t *stage_info = NULL;
    bcmi_field_ha_group_oper_t *group_oper = NULL;
    bcmlt_entry_handle_t grp_template = BCMLT_INVALID_HDL;
    bcmlt_entry_handle_t pdd_template = BCMLT_INVALID_HDL;
    bcmint_field_color_action_tbls_info_t *color_tbl_info = NULL;
    bcmi_field_ha_presel_oper_t *presel_oper = NULL;
    int i = 0;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_stage_from_group_get(unit, group, &stage_info));

    tbls_info = stage_info->tbls_info;
    color_tbl_info = stage_info->color_action_tbls_info;
    group_id = group & BCM_FIELD_ID_MASK;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_group_oper_lookup(unit, group_id,
                                        stage_info,
                                        &group_oper));

    /* Check all entries are deleted from group before destroying */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_entry_multi_get(unit,
                                      group,
                                      0,
                                      NULL,
                                      &entry_count));
    if ((stage_info->flags & BCMINT_FIELD_STAGE_FLEXSTAT_SLICE_INFO) &&
            (entry_count == 1)) {
        rv = bcmi_field_group_delete_slice_reserve_entry(unit,  group);
        SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_NOT_FOUND);
        if (rv == SHR_E_NOT_FOUND)  {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_BUSY);
        }
    } else if (entry_count)  {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_BUSY);
    }

    /* Decrement hint associated to group */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_hint_group_count_update(unit,
                                              group_oper->hintid, 0));

    /* Group pipe info get */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_group_pipe_get(unit, stage_info, group_oper, &pipe));

    /* Get pipe information of color action tables. */
    if (stage_info->flags & BCMINT_FIELD_STAGE_POLICER_ACTION_SUPPORT) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_field_color_action_pipe_get(unit, stage_info,
                                                group_id,
                                                &act_pipe));
    }

    /* Delete the Group template */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_lt_entry_commit(unit,
                                      &grp_template,
                                      BCMLT_OPCODE_DELETE,
                                      BCMLT_PRIORITY_NORMAL,
                                      stage_info->tbls_info->group_sid,
                                      stage_info->tbls_info->group_key_fid,
                                      0, group_id, NULL,
                                      NULL, 0, 0, 0, 1));

    if (stage_info->flags & BCMINT_FIELD_STAGE_POLICY_TYPE_PDD) {
        /* Entry handle allocate for PDD Template */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_allocate(dunit,
                                  tbls_info->pdd_sid,
                                  &pdd_template));
        /* Add pdd_template_id field to PDD LT. */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(pdd_template,
                                   tbls_info->pdd_key_fid,
                                   group_id));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_commit(unit, pdd_template,
                                  BCMLT_OPCODE_DELETE,
                                  BCMLT_PRIORITY_NORMAL));
    }

    if (stage_info->flags & BCMINT_FIELD_STAGE_ACTION_RESOLUTION_SBR) {
        /* Delete SBR template profile */
        if ((group_oper->strength_prof_id != 0) &&
            (group_oper->strength_prof_id != stage_info->sbr_max_limit)) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmint_field_group_strength_prof_destroy(
                        unit,
                        stage_info,
                        ((pipe == -1) ? 0 : pipe),
                        group_oper->priority,
                        group_oper->strength_prof_id,
                        group_oper->color_sbr_id));
        }
    }

    if (stage_info->flags & BCMINT_FIELD_STAGE_PRESEL_KEY_DYNAMIC) {
        if ((group_oper->presel_group != 0) &&
            (group_oper->presel_group == stage_info->presel_group_def)) {
            /* Destroy default presel group */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmint_field_presel_default_group_destroy(unit, stage_info));
        }
        if (group_oper->group_flags & BCMINT_FIELD_GROUP_CREATED_WITH_PRESEL) {
            for (i = 0; i < group_oper->presel_cnt; i++) {
                SHR_IF_ERR_VERBOSE_EXIT(
                         bcmint_field_presel_oper_lookup(unit,
                                    group_oper->presel_arr[i],
                                    stage_info, &presel_oper));
                presel_oper->group_id = 0;
            }
        }
    }

    if ((stage_info->flags & BCMINT_FIELD_STAGE_POLICER_ACTION_SUPPORT)
        && (color_tbl_info != NULL)) {
        if (group_oper->color_pdd_id != -1) {
            SHR_IF_ERR_VERBOSE_EXIT
                (ltsw_fp_group_color_pdd_template_delete(
                        unit,stage_info, act_pipe,
                        group_oper->color_pdd_id));
        }
    }

    if ((stage_info->flags & BCMINT_FIELD_STAGE_POLICY_TYPE_PDD)
        && (group_oper->policer_pool_id != -1)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_field_group_policer_pool_free(
                    unit,
                    stage_info,
                    pipe,
                    group_oper->policer_pool_id));
    }

    /* Delete from group operational database */
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_field_group_oper_remove(unit,
                                      stage_info,
                                      group_id));

exit:
    if (BCMLT_INVALID_HDL != pdd_template) {
        (void) bcmlt_entry_free(pdd_template);
    }
    SHR_FUNC_EXIT();
}

/*
 * Function: bcmint_field_group_entries_destroy
 * Purpose:
 *     Delete all entries for provided group if any
 * Parameters:
 *     unit  - BCM device number
 *     entry - (IN) New entry
 * Returns:
 *     BCM_E_INIT        BCM unit not initialized
 *     BCM_E_NOT_FOUND   group not found in unit
 */
int
bcmint_field_group_entries_destroy(int unit,
                                   bcm_field_group_t group,
                                   bcmint_field_stage_info_t *stage_info)
{
    int entry_id = 0;
    int group_id = 0;
    bcm_field_entry_t entry = 0;
    bcmi_field_ha_entry_oper_t *entry_oper = NULL;

    SHR_FUNC_ENTER(unit);

    if (stage_info == NULL) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_field_stage_from_group_get(unit, group, &stage_info));
    }

    group_id = group & BCM_FIELD_ID_MASK;

    /* Destroy all entries associated to group */
    for (entry_id = 1;
         entry_id <= stage_info->entry_info->eid_max;
         entry_id++) {
        if (SHR_BITGET(stage_info->entry_info->eid_bmp.w, entry_id)) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmint_field_entry_oper_lookup(unit, entry_id,
                                                stage_info,
                                                &entry_oper));
            if (entry_oper->group_id == group_id) {
                entry = stage_info->stage_base_id | entry_id;
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmint_field_entry_destroy(unit, entry, entry_oper));

            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief bcmint_field_group_entries_remove
 *
 * Remove all field entries from group with provided id.
 *
 * \param [in] unit     - BCM device number.
 * \param [in] group_id - Group Identifier.
 *
 * Returns:
 *     BCM_E_INIT      - BCM unit not initialized
 *     BCM_E_NONE      - Success
 */
int
bcmint_field_group_entries_remove(int unit,
                                  bcm_field_group_t group)
{
    bcmint_field_stage_info_t *stage_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_stage_from_group_get(unit, group, &stage_info));

    /* Remove all entries associated to group */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_group_entries_destroy(unit, group, stage_info));
exit:
    SHR_FUNC_EXIT();
}

int
bcmint_field_group_presel_get(int unit,
                              bcm_field_group_t group,
                              bcm_field_presel_set_t *presel)
{
    int dunit = 0;
    bcm_field_group_t group_id = 0;
    bcmint_field_stage_info_t *stage_info = NULL;
    bcmi_field_ha_group_oper_t *group_oper = NULL;
    bcmlt_entry_handle_t group_handle= BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);

    /* Retreive stage info from group ID. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_stage_from_group_get(unit, group, &stage_info));

    /* Retreive group ID. */
    group_id = (BCM_FIELD_ID_MASK) & group;

    /* Retreive group oper entry. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_group_oper_lookup(unit, group_id,
                                        stage_info,
                                        &group_oper));

    /* Group entry handle allocate for group template. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit,
                              stage_info->tbls_info->group_sid,
                              &group_handle));

    /* Add group_id field to group LT. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(group_handle,
                               stage_info->tbls_info->group_key_fid,
                               group_id));

    /* Search and retreive group LT entry. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, group_handle, BCMLT_OPCODE_LOOKUP,
                            BCMLT_PRIORITY_NORMAL));

    FP_HA_GROUP_OPER_PRESEL_ARR_GET(unit, group_oper, *presel);

exit:
    if (BCMLT_INVALID_HDL != group_handle) {
        (void) bcmlt_entry_free(group_handle);
    }
    SHR_FUNC_EXIT();
}

int
bcmint_field_group_presel_set(int unit,
                              bcm_field_group_t group,
                              bcm_field_presel_set_t *presel)
{
    int i = 0;
    int dunit = 0;
    uint32_t presel_count = 0;
    uint64_t presel_id[32] = {0};
    bcm_field_group_t group_id = 0;
    bcmint_field_stage_info_t *stage_info = NULL;
    bcmi_field_ha_group_oper_t *group_oper = NULL;
    bcmlt_entry_handle_t group_handle= BCMLT_INVALID_HDL;
    bcmi_field_ha_presel_oper_t *presel_oper = NULL;
    uint16_t idx = 0;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);

    /* Retreive stage info from group ID. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_stage_from_group_get(unit, group, &stage_info));

    /* Retreive group ID. */
    group_id = (BCM_FIELD_ID_MASK) & group;

    /* Retreive group hash entry. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_group_oper_lookup(unit, group_id,
                                        stage_info,
                                        &group_oper));

    /* Add checks for new presel ids if they are attached to other groups
     * or not.
     */
    for (i = 1; i < BCM_FIELD_PRESEL_SEL_MAX; i++) {
        if (SHR_BITGET(presel->w, i)) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmint_field_presel_oper_lookup(unit,
                                                 i,
                                                 stage_info,
                                                 &presel_oper));
            if ((presel_oper->group_id != 0) &&
                (presel_oper->group_id != group_oper->presel_group)) {
                    LOG_DEBUG(BSL_LS_BCM_FP,
                            (BSL_META_U(unit, "Error: Presel ID.%d is"
                                        " attached to another group \n"),
                                        i));
                    SHR_IF_ERR_VERBOSE_EXIT(SHR_E_CONFIG);
            }
        }
    }


    

    /* Group entry handle allocate for group template. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit,
                              stage_info->tbls_info->group_sid,
                              &group_handle));

    /* Add group_id field to group LT. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(group_handle,
                               stage_info->tbls_info->group_key_fid,
                               group_id));

    /* Search and retreive group LT entry. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, group_handle, BCMLT_OPCODE_LOOKUP,
                            BCMLT_PRIORITY_NORMAL));

    if (group_oper->group_flags & BCMINT_FIELD_GROUP_CREATED_WITH_PRESEL) {
        /* Presel ID attached to group config */
        presel_count = 0;
        for (idx = 0; idx < group_oper->presel_cnt; idx++) {
            presel_id[presel_count++] = group_oper->presel_arr[idx];
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmint_field_presel_oper_lookup(unit,
                                                 group_oper->presel_arr[idx],
                                                 stage_info,
                                                 &presel_oper));
            presel_oper->group_id = 0;
        }

        if (presel_count > 0) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_array_add(group_handle,
                                stage_info->tbls_info->grp_presel_entry_key_fid,
                                0, presel_id, presel_count));
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(group_handle,
                                stage_info->tbls_info->presel_entry_count_fid,
                                0));
        }

        /* Set new presel */
        presel_count = 0;
        for (i = 1; i < BCM_FIELD_PRESEL_SEL_MAX; i++) {
            if (SHR_BITGET(presel->w, i)) {
                /* Check whether there exists a valid presel with this ID */
                if (SHR_BITGET(
                        stage_info->presel_info->pid_bmp.w, i)) {
                    presel_id[presel_count] = i;
                    presel_count++;
                } else {
                    LOG_DEBUG(BSL_LS_BCM_FP,
                            (BSL_META_U(unit, "Error: Not a valid"
                                        " Presel ID.%d \n"), i));
                    SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
                }
            }
        }
        if (presel_count > 0) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_array_add(group_handle,
                                             stage_info->tbls_info->grp_presel_entry_key_fid,
                                             0, presel_id, presel_count));
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(group_handle,
                                       stage_info->tbls_info->presel_entry_count_fid,
                                       presel_count));
        }
    }

    /* Remove read only fields */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_remove(group_handle,
                                  stage_info->tbls_info->grp_oper_mode_fid));

    /* Insert entry template */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_set_commit(unit, group_handle,
                                  BCMLT_PRIORITY_NORMAL));

    FP_HA_GROUP_OPER_PRESEL_ARR_SET(unit, group_oper, *presel);
    for (idx = 0; idx < group_oper->presel_cnt; idx++) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_field_presel_oper_lookup(unit,
                                             group_oper->presel_arr[idx],
                                             stage_info,
                                             &presel_oper));
        presel_oper->group_id = group_oper->presel_group;
    }
exit:
    if (BCMLT_INVALID_HDL != group_handle) {
        (void) bcmlt_entry_free(group_handle);
    }
    SHR_FUNC_EXIT();
}

int
bcmint_field_group_qset_get(int unit,
                            bcm_field_group_t group,
                            bcm_field_qset_t *qset)
{
    int dunit = 0;
    bcm_field_group_t group_id = 0;
    bcmint_field_stage_info_t *stage_info = NULL;
    bcmi_field_ha_group_oper_t *group_oper = NULL;
    bcmlt_entry_handle_t group_handle = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);

    /* Retreive stage info from group ID. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_stage_from_group_get(unit, group, &stage_info));

    /* Retreive group ID. */
    group_id = (BCM_FIELD_ID_MASK) & group;

    /* Retreive group hash entry. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_group_oper_lookup(unit, group_id,
                                        stage_info,
                                        &group_oper));
    if (!group_oper) {
        SHR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }

    /* Group entry handle allocate for group template. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit,
                              stage_info->tbls_info->group_sid,
                              &group_handle));

    /* Add group_id field to group LT. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(group_handle,
                               stage_info->tbls_info->group_key_fid,
                               group_id));

    /* Search and retreive group LT entry. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, group_handle, BCMLT_OPCODE_LOOKUP,
                            BCMLT_PRIORITY_NORMAL));

    FP_HA_GROUP_OPER_QSET_ARR_GET(unit, group_oper, *qset);

exit:
    if (BCMLT_INVALID_HDL != group_handle) {
        (void) bcmlt_entry_free(group_handle);
    }
    SHR_FUNC_EXIT();
}

int
bcmint_field_group_validate_qset_set(int unit,
                            bcm_field_group_t group,
                            bcmint_field_stage_info_t *stage_info,
                            bcmi_field_ha_group_oper_t *group_oper,
                            bcm_field_qset_t curr_qset,
                            bcm_field_qset_t new_qset)
{
    int i;
    int rv = 0;
    bcm_pbmp_t pbmp_grp;
    bool mode_auto = false;
    bcm_field_group_mode_t mode, old_mode, new_mode;
    bcm_port_config_t  port_config;
    bcm_field_group_config_t group_config;
    bcm_field_group_t group_id = 0;
    bcmlt_entry_handle_t group_handle= BCMLT_INVALID_HDL;
    int dunit = 0;
    bcmi_field_ha_group_oper_t *group_oper_new = NULL;
    bool temp_grp_created = FALSE;
    bcm_field_presel_set_t curr_pset;
    bcm_field_group_mode_type_t mode_type = bcmFieldGroupModeTypeAuto;
    bcm_field_group_packet_type_t packet_type = bcmFieldGroupPacketTypeDefault;

    SHR_FUNC_ENTER(unit);

    if ((NULL == stage_info) || (NULL == group_oper)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    /*now update the group with new qset */
    dunit = bcmi_ltsw_dev_dunit(unit);
    group_id = (BCM_FIELD_ID_MASK) & (group);

    /* set group ID */
    sal_memset(&group_config, 0, sizeof(bcm_field_group_config_t));

    /* the gid_max group is reserved for testing (first reserved grp ID) */
    group_config.group = (((stage_info->stage_base_id) |
                          (stage_info->group_info->gid_max)) -
                          BCMINT_FIELD_GROUP_RESERVE_GID_COUNT + 1);
    group_config.flags = BCM_FIELD_GROUP_CREATE_WITH_ID;


    FP_HA_GROUP_OPER_ASET_ARR_GET(unit, group_oper, group_config.aset);
    group_config.qset = curr_qset;

    if (stage_info->flags & BCMINT_FIELD_STAGE_KEY_TYPE_FIXED) {
        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_field_egr_group_qset_validate(dunit,
                                                     group_id,
                                                     stage_info,
                                                     new_qset,
                                                     &mode_type,
                                                     &packet_type));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_field_group_mode_get(unit,
                                         group,
                                         &mode));
        group_config.mode = mode;
    }
    if (!(stage_info->flags & BCMINT_FIELD_STAGE_KEY_TYPE_FIXED)) {
        /* set the group mode */
        SHR_IF_ERR_VERBOSE_EXIT(bcmint_field_group_mode_auto_get(
                    unit, group, &mode_auto));
        if (!mode_auto) {
            SHR_IF_ERR_VERBOSE_EXIT(bcmint_field_group_mode_get(
                        unit,group, &mode));
            group_config.mode = mode;
            group_config.flags |= BCM_FIELD_GROUP_CREATE_WITH_MODE;
        }
    }
    /* set the group ports */
    SHR_IF_ERR_VERBOSE_EXIT(bcmint_field_group_ports_get(
                            unit,group, &pbmp_grp));
    SHR_IF_ERR_VERBOSE_EXIT(bcm_ltsw_port_config_get(
                            unit, &port_config));
    if (!(BCM_PBMP_EQ(pbmp_grp, port_config.all))) {
        BCM_PBMP_ASSIGN(group_config.ports, pbmp_grp);
        group_config.flags |= BCM_FIELD_GROUP_CREATE_WITH_PORT;
    }

    /* set the presel group */
    if (group_oper->group_flags & BCMINT_FIELD_GROUP_CREATED_WITH_PRESEL) {

        if ((group_oper->presel_group != 0) &&
            (group_oper->presel_group == stage_info->presel_group_def)) {
        } else {
            group_config.presel_group = group_oper->presel_group;
            group_config.flags |= BCM_FIELD_GROUP_CREATE_WITH_PRESEL_GROUP;
       }
       group_config.flags |= BCM_FIELD_GROUP_CREATE_WITH_PRESELSET;

       BCM_FIELD_PRESEL_INIT(curr_pset);
       for (i = 0; i < group_oper->presel_cnt; i++) {
           BCM_FIELD_PRESEL_ADD(curr_pset, group_oper->presel_arr[i]);
       }
       sal_memcpy(&group_config.preselset, &curr_pset,
                  sizeof(bcm_field_presel_set_t));
    }

    /* copy the hint id */
    group_config.hintid = group_oper->hintid;
    group_config.priority = group_oper->priority;
    group_config.mode_type = mode_type;
    group_config.packet_type = packet_type;
    rv = bcmint_field_group_config_create(unit, &group_config,
             BCMINT_FIELD_GROUP_CREATE_WITH_RESERVE_ID);
    if (SHR_FAILURE(rv)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
    }
    temp_grp_created = TRUE;

    if (!(stage_info->flags & BCMINT_FIELD_STAGE_KEY_TYPE_FIXED)) {
        /* check if the group oper mode is same as original group oper mode */
        SHR_IF_ERR_VERBOSE_EXIT(bcmint_field_group_mode_get(
                    unit,group, &old_mode));

        SHR_IF_ERR_VERBOSE_EXIT(bcmint_field_group_mode_get(
                    unit,group_config.group, &new_mode));
        if ( old_mode != new_mode) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_CONFIG);
        }
    }

    group_id = (BCM_FIELD_ID_MASK) & (group_config.group);
    /* Retreive group hash entry. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_group_oper_lookup(unit, group_id,
                                        stage_info,
                                        &group_oper_new));

    /* Group entry handle allocate for group template. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit,
                              stage_info->tbls_info->group_sid,
                              &group_handle));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(group_handle,
                               stage_info->tbls_info->group_key_fid,
                               group_id));

    /* Search and retreive group LT entry. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, group_handle, BCMLT_OPCODE_LOOKUP,
                            BCMLT_PRIORITY_NORMAL));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_process_group_qset(unit,
                                         &curr_qset,
                                         group_oper_new->hintid,
                                         stage_info, 0, 0,
                                         &group_handle));

    SHR_IF_ERR_VERBOSE_EXIT(bcmint_field_process_group_qset(unit,
        &new_qset, group_oper_new->hintid, stage_info, 1, 0, &group_handle));

    /* remove the read only field as readonly field cannot be updated */
    if (!(stage_info->flags & BCMINT_FIELD_STAGE_KEY_TYPE_FIXED)) {
        SHR_IF_ERR_VERBOSE_EXIT(bcmlt_entry_field_remove(group_handle,
                    stage_info->tbls_info->grp_oper_mode_fid));
    }

    /* Insert entry template */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_set_commit(unit, group_handle, BCMLT_PRIORITY_NORMAL));

    if (!(stage_info->flags & BCMINT_FIELD_STAGE_KEY_TYPE_FIXED)) {
        SHR_IF_ERR_VERBOSE_EXIT(bcmint_field_group_mode_get(
                    unit,group_config.group, &new_mode));
        /* group mode should not change*/
        if ( old_mode != new_mode) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_CONFIG);
        }
    }

exit:
    if (BCMLT_INVALID_HDL != group_handle) {
        (void) bcmlt_entry_free(group_handle);
    }

    if (temp_grp_created) {
        SHR_IF_ERR_VERBOSE_EXIT(bcmint_field_group_destroy(
            unit, group_config.group));
    }
    SHR_FUNC_EXIT();
}

int
bcmint_field_group_qset_set(int unit,
                            bcm_field_group_t group,
                            bcm_field_qset_t qset)
{
    int dunit = 0;
    uint16_t qual_count = 0;
    bcm_field_group_t group_id = 0;
    bcmint_field_stage_info_t *stage_info = NULL;
    bcmi_field_ha_group_oper_t *group_oper = NULL;
    bcmlt_entry_handle_t group_handle= BCMLT_INVALID_HDL;
    bcm_field_qset_t new_qset, curr_qset;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);

    /* Retreive stage info from group ID. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_stage_from_group_get(unit, group, &stage_info));

    /* Retreive group ID. */
    group_id = (BCM_FIELD_ID_MASK) & group;

    /* Retreive group hash entry. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_group_oper_lookup(unit, group_id,
                                        stage_info,
                                        &group_oper));

    /* Verify requested QSET is supported by the stage. */
    if (FALSE == bcmint_field_qset_is_subset(
                unit, &qset, stage_info, 0, &qual_count)) {
        LOG_DEBUG(BSL_LS_BCM_FP,
                (BSL_META_U(unit, "Error: Qualifier set is not"
                            " supported by the device.\n")));
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    BCM_FIELD_QSET_INIT(curr_qset);
    FP_HA_GROUP_OPER_QSET_ARR_GET(unit, group_oper, curr_qset);

    /* append the given qset */
    BCM_FIELD_QSET_INIT(new_qset);
    BCMI_FIELD_QSET_OR(new_qset, curr_qset,
                       bcmiFieldQualifyLastCount);
    BCMI_FIELD_QSET_OR(new_qset, qset,
                       bcmiFieldQualifyLastCount);

    /* validate to make sure qset update doesnot change the grp mode. */
    SHR_IF_ERR_VERBOSE_EXIT(bcmint_field_group_validate_qset_set(
        unit, group, stage_info, group_oper, curr_qset, new_qset));

    /* update the group qset */
    /* Group entry handle allocate for group template. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit,
                              stage_info->tbls_info->group_sid,
                              &group_handle));

    /* Add group_id field to group LT. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(group_handle,
                               stage_info->tbls_info->group_key_fid,
                               group_id));

    /* Search and retreive group LT entry. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, group_handle, BCMLT_OPCODE_LOOKUP,
                            BCMLT_PRIORITY_NORMAL));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_process_group_qset(unit,
                                         &curr_qset,
                                         group_oper->hintid,
                                         stage_info, 0, 0,
                                         &group_handle));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_process_group_qset(unit,
                                         &new_qset,
                                         group_oper->hintid,
                                         stage_info, 1, 0,
                                         &group_handle));
    /* remove the read only field */
    if (!(stage_info->flags & BCMINT_FIELD_STAGE_KEY_TYPE_FIXED)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_remove(group_handle,
             stage_info->tbls_info->grp_oper_mode_fid));
    }

    /* Insert entry template */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_set_commit(unit, group_handle,
                                  BCMLT_PRIORITY_NORMAL));

     /*sal_memcpy(&group_hash->grp_hash_qset, &qset, sizeof(bcm_field_qset_t));*/
     FP_HA_GROUP_OPER_QSET_ARR_SET(unit, group_oper, new_qset);

exit:
    if (BCMLT_INVALID_HDL != group_handle) {
        (void) bcmlt_entry_free(group_handle);
    }
    SHR_FUNC_EXIT();
}

int
bcmint_field_group_aset_get(int unit,
                            bcm_field_group_t group,
                            bcm_field_aset_t *aset)
{
    int dunit = 0;
    bcm_field_group_t group_id = 0;
    bcmint_field_stage_info_t *stage_info = NULL;
    bcmi_field_ha_group_oper_t *group_oper = NULL;
    bcmlt_entry_handle_t group_handle= BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);

    /* Retreive stage info from group ID. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_stage_from_group_get(unit, group, &stage_info));

    /* Retreive group ID. */
    group_id = (BCM_FIELD_ID_MASK) & group;

    /* Retreive group hash entry. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_group_oper_lookup(unit, group_id,
                                        stage_info,
                                        &group_oper));

    /* Group entry handle allocate for group template. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit,
                              stage_info->tbls_info->group_sid,
                              &group_handle));

    /* Add group_id field to group LT. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(group_handle,
                               stage_info->tbls_info->group_key_fid,
                               group_id));

    /* Search and retreive group LT entry. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, group_handle, BCMLT_OPCODE_LOOKUP,
                            BCMLT_PRIORITY_NORMAL));

    FP_HA_GROUP_OPER_ASET_ARR_GET(unit, group_oper, *aset);
exit:
    if (BCMLT_INVALID_HDL != group_handle) {
        (void) bcmlt_entry_free(group_handle);
    }
    SHR_FUNC_EXIT();
}


int
bcmint_field_group_aset_set(int unit,
                            bcm_field_group_t group,
                            bcm_field_aset_t aset)
{
    int dunit = 0;
    int pipe = -1;
    int act_pipe = -1;
    int entry_count = 0;
    int color_pdd_idx = -1;
    bcm_field_group_t group_id = 0;
    bcm_field_aset_t grp_aset;
    int color_sbr_idx = -1;
    uint64_t strength_idx = 0;
    int policer_pool_id = -1;
    int stage_oper_mode = 0;
    int color_action_mode = 0;
    bcmint_field_stage_info_t *stage_info = NULL;
    bcmi_field_ha_group_oper_t *group_oper = NULL;
    bcmlt_entry_handle_t pdd_handle = BCMLT_INVALID_HDL;
    bcmlt_entry_handle_t group_handle = BCMLT_INVALID_HDL;
    bcmlt_entry_handle_t grp_template = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);

    /* Retreive stage info from group ID. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_stage_from_group_get(unit, group, &stage_info));

    /* Retreive group ID. */
    group_id = (BCM_FIELD_ID_MASK) & group;

    /* Retreive group hash entry. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_group_oper_lookup(unit, group_id,
                                        stage_info,
                                        &group_oper));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_oper_mode_status_get(unit,
                                stage_info, &stage_oper_mode));
    /* Group pipe info get */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_group_pipe_get(unit, stage_info, group_oper, &pipe));

    if ((stage_info->flags & BCMINT_FIELD_STAGE_POLICER_ACTION_SUPPORT) &&
        (stage_info->flags & BCMINT_FIELD_STAGE_POLICY_TYPE_PDD) &&
        (BCM_FIELD_ASET_TEST
         (aset, bcmFieldActionPolicerGroup))) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_field_color_action_mode_get(unit,
                                                stage_info,
                                                &color_action_mode));
        if ((stage_oper_mode == bcmiFieldOperModePipeUnique) &&
            (color_action_mode != bcmiFieldOperModePipeUnique)) {
            LOG_DEBUG(BSL_LS_BCM_FP,
                      (BSL_META_U(unit, "Error: Meter action tables cannot "
                                  "be in global mode when FP stage is "
                                  "in per pipe mode.\n")));
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_CONFIG);
        }

        if ((stage_oper_mode != bcmiFieldOperModePipeUnique) &&
            (color_action_mode == bcmiFieldOperModePipeUnique)) {
            LOG_DEBUG(BSL_LS_BCM_FP,
                      (BSL_META_U(unit, "Error: Meter action tables cannot "
                                  "be in per pipe mode when FP stage is not "
                                  "in per pipe mode.\n")));
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_CONFIG);
        }

        if (stage_oper_mode == bcmiFieldOperModePipeUnique) {
            act_pipe = pipe;
        } else {
            act_pipe = -1;
        }
    }

    if ((stage_info->flags & BCMINT_FIELD_STAGE_POLICY_TYPE_PDD) &&
        BCM_FIELD_ASET_TEST(aset, bcmFieldActionPolicerGroup) &&
        (group_oper->policer_pool_id == -1)) {
        /* Group pipe info get */
        if (stage_info->flags & BCMINT_FIELD_STAGE_POLICER_POOL_HINT_SUPPORT) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmint_field_group_policer_pool_get(unit,
                                                     group_id,
                                                     group_oper->hintid,
                                                     &policer_pool_id));
        }
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_field_group_policer_pool_alloc(unit,
                                                   stage_info,
                                                   pipe,
                                                   group_oper->priority,
                                                   &policer_pool_id));
        group_oper->policer_pool_id = policer_pool_id;
    }

    if (stage_info->flags & BCMINT_FIELD_STAGE_POLICY_TYPE_PDD) {
        if (group_oper->group_flags & BCMINT_FIELD_GROUP_ENT_LVL_ACTION_RESOLVE) {
            /* Doesn't allow ASET update if the SBR is handled at entry level */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmint_field_entry_multi_get(unit, group, 0,
                                              NULL, &entry_count));
            if (entry_count) {
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_BUSY);
            }
        }

        /* Group entry handle allocate for group template. */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_allocate(dunit,
                                  stage_info->tbls_info->group_sid,
                                  &group_handle));

        /* PDD handle allocate */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_allocate(dunit,
                                  stage_info->tbls_info->pdd_sid,
                                  &pdd_handle));

        /* Add group_id field to group LT. */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(group_handle,
                                   stage_info->tbls_info->group_key_fid,
                                   group_id));

        /* Add pdd_template_id field to Group LT. */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(pdd_handle,
                                   stage_info->tbls_info->pdd_key_fid,
                                   group_id));

        /* Search and retreive group LT entry. */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_commit(unit, group_handle, BCMLT_OPCODE_LOOKUP,
                                BCMLT_PRIORITY_NORMAL));

        /* Search and retreive group LT entry. */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_commit(unit, pdd_handle, BCMLT_OPCODE_LOOKUP,
                                BCMLT_PRIORITY_NORMAL));

        BCM_FIELD_ASET_INIT(grp_aset);
        FP_HA_GROUP_OPER_ASET_ARR_GET(unit, group_oper, grp_aset);

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_field_process_group_aset(unit,
                                             stage_info, 0,
                                             act_pipe,
                                             &grp_aset,
                                             group_oper->hintid,
                                             &pdd_handle,
                                             group_oper->policer_pool_id,
                                             &group_oper->color_pdd_id));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_field_process_group_aset(unit,
                                             stage_info, 1,
                                             act_pipe,
                                             &aset,
                                             group_oper->hintid,
                                             &pdd_handle,
                                             group_oper->policer_pool_id,
                                             &color_pdd_idx));

        /* Insert entry template */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_set_commit(unit, pdd_handle,
                                      BCMLT_PRIORITY_NORMAL));

        FP_HA_GROUP_OPER_ASET_ARR_SET(unit, group_oper, aset);
        group_oper->color_pdd_id = color_pdd_idx;
    }

    /* Update SBR profile associated with the group. */
    if ((stage_info->flags & BCMINT_FIELD_STAGE_ACTION_RESOLUTION_SBR) &&
        !(group_oper->group_flags & BCMINT_FIELD_GROUP_ENT_LVL_ACTION_RESOLVE)) {

        /* Create SBR strength profile */
        if ((group_oper->strength_prof_id != 0) &&
            (group_oper->strength_prof_id != stage_info->sbr_max_limit)) {
            /* Destroy old profile first associated to group. */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmint_field_group_strength_prof_destroy(
                        unit, stage_info,
                        ((pipe == -1) ? 0 : pipe),
                        group_oper->priority,
                        group_oper->strength_prof_id,
                        group_oper->color_sbr_id));
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_field_group_strength_prof_create(unit, stage_info,
                                                     ((pipe == -1) ? 0 : pipe),
                                                     group_oper->priority,
                                                     group_oper->hintid,
                                                     group_oper->policer_pool_id,
                                                     &aset,
                                                     &strength_idx,
                                                     &color_sbr_idx));
        group_oper->strength_prof_id = strength_idx;
        group_oper->color_sbr_id = color_sbr_idx;

        /* Update new strength index in group table. */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_field_lt_entry_commit(unit,
                                          &grp_template,
                                          BCMLT_OPCODE_UPDATE,
                                          BCMLT_PRIORITY_NORMAL,
                                          stage_info->tbls_info->group_sid,
                                          stage_info->tbls_info->group_key_fid,
                                          0, group_id, NULL,
                                          stage_info->tbls_info->sbr_key_fid,
                                          strength_idx,
                                          0, 0,
                                          1));
    }

exit:
    if (BCMLT_INVALID_HDL != group_handle) {
        (void) bcmlt_entry_free(group_handle);
    }
    if (BCMLT_INVALID_HDL != pdd_handle) {
        (void) bcmlt_entry_free(pdd_handle);
    }
    SHR_FUNC_EXIT();
}

int
bcmint_field_group_mode_auto_get(int unit,
                                bcm_field_group_t group,
                                bool *mode_auto)
{
    int dunit = 0;
    uint64_t mode = 0;
    bcm_field_group_t group_id = 0;
    bcmint_field_stage_info_t *stage_info = NULL;
    bcmlt_entry_handle_t group_handle= BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_stage_from_group_get(unit, group, &stage_info));

    /* Mask the stage bits from given entry ID */
    group_id = (BCM_FIELD_ID_MASK) & group;

    /* Entry handle allocate for Entry Template */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit,
                              stage_info->tbls_info->group_sid,
                              &group_handle));

    /* Add entry_id field to Entry LT. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(group_handle,
                               stage_info->tbls_info->group_key_fid,
                               group_id));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, group_handle, BCMLT_OPCODE_LOOKUP,
                            BCMLT_PRIORITY_NORMAL));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(group_handle,
                               "MODE_AUTO",
                               &mode));
    *mode_auto = mode;

exit:
    if (BCMLT_INVALID_HDL != group_handle) {
        (void) bcmlt_entry_free(group_handle);
    }
    SHR_FUNC_EXIT();
}

int
bcmint_field_group_priority_get(int unit,
                                bcm_field_group_t group,
                                int *priority)
{
    bcm_field_group_t group_id = 0;
    bcmint_field_stage_info_t *stage_info = NULL;
    bcmi_field_ha_group_oper_t *group_oper = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_stage_from_group_get(unit, group, &stage_info));

    /* Mask the stage bits from given group ID */
    group_id = (BCM_FIELD_ID_MASK) & group;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_group_oper_lookup(unit, group_id,
                                        stage_info,
                                        &group_oper));
    *priority = group_oper->priority;

exit:
    SHR_FUNC_EXIT();
}

int
bcmint_field_group_priority_set(int unit,
                                bcm_field_group_t group,
                                int prio)
{
    int dunit = 0;
    int pipe = -1, curr_prio;
    int color_sbr_idx = -1;
    bcm_field_aset_t grp_aset;
    uint64_t strength_idx = 0;
    bcm_field_group_t gid, group_id = 0;
    bcmint_field_stage_info_t *stage_info = NULL;
    bcmi_field_ha_group_oper_t *g_oper = NULL;
    bcmi_field_ha_group_oper_t *group_oper = NULL;
    uint64_t grp_lt_prio_min = 0, grp_lt_prio_max = 0;
    bcmlt_entry_handle_t group_handle = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_stage_from_group_get(unit, group, &stage_info));

    /* Mask the stage bits from given group ID */
    group_id = (BCM_FIELD_ID_MASK) & group;

    /* Find group operational entry */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_group_oper_lookup(unit, group_id,
                                        stage_info,
                                        &group_oper));

    /* If old priority is same as new one, exit
     * without doing anything. */
    if (group_oper->priority == prio) {
        SHR_EXIT();
    }

    curr_prio = group_oper->priority;
    /* check the range of entry priority */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_field_value_range_get(unit,
                                       stage_info->tbls_info->group_sid,
                                       "ENTRY_PRIORITY",
                                       &grp_lt_prio_min,
                                       &grp_lt_prio_max));

    /* Priority set should be within allowed range. */
    if ((prio < grp_lt_prio_min) || (prio > grp_lt_prio_max)) {
        LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META_U(unit,
           "Error: group priority should be in range "
           "%d-%d.\n"), (int)grp_lt_prio_min, (int)grp_lt_prio_max));
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    /* Verify whether the given prio is already configured to any other group */
    for (gid = stage_info->stage_base_id;
         gid <= (stage_info->stage_base_id +
                 stage_info->group_info->gid_max);
         gid++) {
        /* Check whether the corresponding group ID exists. */
        if (SHR_BITGET(stage_info->group_info->grpid_bmp.w,
                       (gid & BCM_FIELD_ID_MASK))) {
            /* Find group operational entry */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmint_field_group_oper_lookup(unit, (gid & BCM_FIELD_ID_MASK),
                                                stage_info,
                                                &g_oper));
            if (g_oper->priority == prio) {
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
            }
            /*
             * Can't change the priority of a group that is shared
             * with another same priority group.
             */
            if ((g_oper->priority == group_oper->priority) &&
                (g_oper->group_id != group_oper->group_id)) {
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
            }
        }
    }

    dunit = bcmi_ltsw_dev_dunit(unit);
    /* Entry handle allocate for group template */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit,
                              stage_info->tbls_info->group_sid,
                              &group_handle));

    /* Add group_id key field to LT. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(group_handle,
                               stage_info->tbls_info->group_key_fid,
                               group_id));

    group_oper->priority = prio;

    /* Group pipe info get */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_group_pipe_get(unit, stage_info, group_oper, &pipe));

    /* Destroy SBR profile associated for group. */
    if (stage_info->flags & BCMINT_FIELD_STAGE_ACTION_RESOLUTION_SBR) {
        if (group_oper->group_flags &
            BCMINT_FIELD_GROUP_ENT_LVL_ACTION_RESOLVE) {
            /* update the strength for all the entries in the group */
            SHR_IF_ERR_VERBOSE_EXIT
               (bcmint_field_group_entry_strength_update(unit, stage_info,
                                                         group_oper, curr_prio));
        } else if ((group_oper->strength_prof_id != 0) &&
                   (group_oper->strength_prof_id != stage_info->sbr_max_limit)) {
            /* Destroy old profile first associated to group. */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmint_field_group_strength_prof_destroy(unit,
                       stage_info,
                       ((pipe == -1) ? 0 : pipe),
                       group_oper->priority,
                       group_oper->strength_prof_id,
                       group_oper->color_sbr_id));

            BCM_FIELD_ASET_INIT(grp_aset);
            FP_HA_GROUP_OPER_ASET_ARR_GET(unit, group_oper, grp_aset);
            /* Create SBR strength profile */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmint_field_group_strength_prof_create(unit, stage_info,
                                                         ((pipe == -1) ? 0 : pipe),
                                                         prio,
                                                         group_oper->hintid,
                                                         group_oper->policer_pool_id,
                                                         &grp_aset,
                                                         &strength_idx,
                                                         &color_sbr_idx));
            group_oper->strength_prof_id = strength_idx;
            group_oper->color_sbr_id = color_sbr_idx;

            /* Update new strength index in group table. */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(group_handle,
                                       stage_info->tbls_info->sbr_key_fid,
                                       strength_idx));
        }
    }

    if (group_oper->policer_pool_id != -1) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_field_group_policer_prio_update(unit,
                                                    stage_info,
                                                    pipe,
                                                    group_oper->policer_pool_id,
                                                    prio));
    }

    /* Update new priority in group table. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(group_handle,
                               "ENTRY_PRIORITY",
                               prio));

    /* Update group template */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, group_handle,
                              BCMLT_OPCODE_UPDATE,
                              BCMLT_PRIORITY_NORMAL));

    if (stage_info->flags & BCMINT_FIELD_STAGE_FLEX_CTR_SUPPORT) {
        bcm_flexctr_source_t source = bcmFlexctrSourceCount;

        switch (stage_info->stage_id) {
            case bcmiFieldStageIngress:
                 source = bcmFlexctrSourceIfp;
                 break;
            case bcmiFieldStageEgress:
                 source = bcmFlexctrSourceEfp;
                 break;
            case bcmiFieldStageVlan:
                 source = bcmFlexctrSourceVfp;
                 break;
            case bcmiFieldStageExactMatch:
            case bcmiFieldStageFlowTracker:
                 source = bcmFlexctrSourceExactMatch;
                 break;
            default:
                 source = bcmFlexctrSourceCount;
                 break;
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_flexctr_field_group_update(unit, source, group,
                                                  group_oper->priority,
                                                  prio));
    }

    group_oper->priority = prio;
exit:
    if (SHR_FUNC_ERR()) {
        if (strength_idx != 0) {
            (void)bcmint_field_group_strength_prof_destroy(
                                       unit,
                                       stage_info,
                                       ((pipe == -1) ? 0 : pipe),
                                       prio,
                                       strength_idx,
                                       group_oper->color_sbr_id);
            group_oper->strength_prof_id = 0;
            group_oper->color_sbr_id = -1;
        }

        if (group_oper) {
            group_oper->priority = curr_prio;
        }
    }
    if (BCMLT_INVALID_HDL != group_handle) {
        (void) bcmlt_entry_free(group_handle);
    }
    SHR_FUNC_EXIT();
}

int
bcmint_field_group_enable_get(
    int unit,
    bcm_field_group_t group,
    int *enable)
{
    bcm_field_group_t group_id = 0;
    bcmint_field_stage_info_t *stage_info = NULL;
    bcmi_field_ha_group_oper_t *group_oper = NULL;

    SHR_FUNC_ENTER(unit);

    /* Validate the entry */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_stage_from_group_get(unit, group,
                                          &stage_info));

    /* Mask the stage bits from given group ID */
    group_id = group & BCM_FIELD_ID_MASK;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_group_oper_lookup(unit, group_id,
                                        stage_info,
                                        &group_oper));

    if (!group_oper) {
        SHR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }

    *enable = (group_oper->group_flags & BCMINT_FIELD_GROUP_DISABLED) ? 0 : 1;

exit:
    SHR_FUNC_EXIT();
}

int
bcmint_field_group_enable_set(
    int unit,
    bcm_field_group_t group,
    int enable)
{
    int dunit = 0;
    uint32 group_flags = 0x0;
    bcm_field_group_t group_id = 0;
    bcmint_field_stage_info_t *stage_info = NULL;
    bcmi_field_ha_group_oper_t *group_oper = NULL;
    bcmlt_entry_handle_t group_template = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);
    dunit = bcmi_ltsw_dev_dunit(unit);

    /* Validate the entry */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_stage_from_group_get(unit, group,
                                           &stage_info));

    /* Mask the stage bits from given group ID */
    group_id = group & BCM_FIELD_ID_MASK;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_group_oper_lookup(unit,
                                        group_id,
                                        stage_info,
                                        &group_oper));

    /* To disable the group */
    if (0 == enable) {

        /* check if group is already diabled. if yes do nothing */
        if (group_oper->group_flags & BCMINT_FIELD_GROUP_DISABLED) {
            /*Do Nothing */
            SHR_EXIT();
        }

        /* Set the group disable status */
        group_flags = (group_oper->group_flags | BCMINT_FIELD_GROUP_DISABLED);
    } else { /* Enable the group */

        /* Check if group is already enabled. if yes do nothing */
        if (!(group_oper->group_flags & BCMINT_FIELD_GROUP_DISABLED)) {
            /*Do Nothing */
            SHR_EXIT();
        }

        /* Reset the group disable status */
        group_flags = (group_oper->group_flags & ~BCMINT_FIELD_GROUP_DISABLED);
    }

    /* Entry handle allocate for Entry Template */
    SHR_IF_ERR_VERBOSE_EXIT
       (bcmlt_entry_allocate(dunit,
                             stage_info->tbls_info->group_sid,
                             &group_template));

    /* Add entry_id field to Entry LT. */
    SHR_IF_ERR_VERBOSE_EXIT
       (bcmlt_entry_field_add(group_template,
                              stage_info->tbls_info->group_key_fid,
                              group_id));

    /* Check Group disbaled flag and
     * set accordingly the ENABLE field in group Template. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(group_template,
                               "ENABLE",
                               !(group_flags & BCMINT_FIELD_GROUP_DISABLED)));

    /* Update group template */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, group_template,
                              BCMLT_OPCODE_UPDATE,
                              BCMLT_PRIORITY_NORMAL));

    group_oper->group_flags = group_flags;

exit:
    if (BCMLT_INVALID_HDL != group_template) {
        (void) bcmlt_entry_free(group_template);
    }
    SHR_FUNC_EXIT();
}

int
bcmint_field_group_mode_get(
    int unit,
    bcm_field_group_t group,
    bcm_field_group_mode_t *mode)
{
    bcmint_field_stage_info_t *stage_info = NULL;
    bcmi_field_ha_group_oper_t *group_oper = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_stage_from_group_get(unit, group, &stage_info));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_group_oper_lookup(unit,
                                        group & BCM_FIELD_ID_MASK,
                                        stage_info,
                                        &group_oper));

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_field_group_mode_get(unit, stage_info,
                                        group_oper, mode));

exit:
    SHR_FUNC_EXIT();
}

int
bcmint_field_group_config_validate(
        int unit,
        bcm_field_group_config_t *group_config,
        bcm_field_group_mode_t *mode)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_group_config_create(unit, group_config, 0));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_group_mode_get(unit, group_config->group, mode));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_group_destroy(unit, group_config->group));

exit:
    SHR_FUNC_EXIT();
}

int
bcmint_field_group_ports_get(int unit,
                             bcm_field_group_t group,
                             bcm_pbmp_t *pbmp)
{
    bcm_field_group_config_t *grp_config = NULL;
    bcmint_field_stage_info_t *stage_info = NULL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(pbmp, SHR_E_INTERNAL);

    BCMINT_FIELD_MEM_ALLOC
        (grp_config, sizeof(bcm_field_group_config_t), "grp_config");
    grp_config->group = group;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_stage_from_group_get(unit, group, &stage_info));

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_field_group_pbmp_get(unit, grp_config, stage_info, pbmp));

exit:
    if (grp_config != NULL) {
        BCMINT_FIELD_MEM_FREE(grp_config);
    }
    SHR_FUNC_EXIT();
}

int
bcmint_field_group_traverse(int unit,
                            bcm_field_group_traverse_cb callback,
                            void *user_data)
{
    int rv;
    int group_id, gid;
    bcmi_ltsw_field_stage_t stage_id;
    bcmint_field_stage_info_t *stage_info;

    SHR_FUNC_ENTER(unit);

    for (stage_id = bcmiFieldStageIngress;
         stage_id < bcmiFieldStageCount; stage_id++) {
        rv = bcmint_field_stage_info_get(unit,
                                         stage_id,
                                         &stage_info);
        if (SHR_FAILURE(rv)) {
            continue;
        }

        /* Verify wheter the given ID is in group ID's range */
        for (gid = stage_info->stage_base_id;
             gid <= (stage_info->stage_base_id +
                     stage_info->group_info->gid_max);
             gid++) {
            /* Mask the stage bits from given group ID */
            group_id = gid & BCM_FIELD_ID_MASK;

            /* Check whether the corresponding group ID exists. */
            if (SHR_BITGET(stage_info->group_info->grpid_bmp.w, group_id)) {
                rv = (*callback)(unit, gid, user_data);
                SHR_IF_ERR_VERBOSE_EXIT(rv);
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmint_field_group_oper_mode_get(int unit,
                                 bcm_field_qualify_t stage,
                                 bcm_field_group_oper_mode_t *mode)
{
    int oper_mode = 0;
    int stage_id = bcmiFieldStageCount;
    bcmint_field_stage_info_t *stage_info = NULL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(mode, BCM_E_PARAM);

    /* Get internal stage id. */
    BCMINT_FIELD_STAGE_ID_GET(stage, stage_id);
    if (stage_id == bcmiFieldStageCount) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    /* Retrieve stage info. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_stage_info_get(unit, stage_id, &stage_info));

    /* Retrieve operational mode status. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_oper_mode_status_get(unit, stage_info, &oper_mode));

    if (oper_mode == bcmiFieldOperModePipeUnique) {
        *mode = bcmFieldGroupOperModePipeLocal;
    } else {
        *mode = bcmFieldGroupOperModeGlobal;
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmint_field_qset_id_multi_set(
    int unit,
    bcm_field_qualify_t qual,
    int num_objects,
    int objects_list[],
    bcm_field_qset_t *qset)
{
    SHR_FUNC_ENTER(unit);

    if (qual != bcmFieldQualifyUdf) {
        SHR_IF_ERR_VERBOSE_EXIT(BCM_E_UNAVAIL);
    }

    if (qset == NULL) {
        SHR_IF_ERR_VERBOSE_EXIT(BCM_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_field_udf_qset_set(unit, num_objects,
                                      objects_list,
                                      qset));
exit:
    SHR_FUNC_EXIT();
}


int
bcmint_field_qset_id_multi_get(
    int unit,
    bcm_field_qset_t qset,
    bcm_field_qualify_t qual,
    int max,
    int *objects_list,
    int *actual)
{
    SHR_FUNC_ENTER(unit);

    if (qual != bcmFieldQualifyUdf) {
        SHR_IF_ERR_VERBOSE_EXIT(BCM_E_UNAVAIL);
    }

    if ((objects_list == NULL) || (actual == NULL)) {
        SHR_IF_ERR_VERBOSE_EXIT(BCM_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_field_udf_obj_get(unit, qset, max,
                                     objects_list,
                                     actual));
exit:
    SHR_FUNC_EXIT();
}

int
bcmint_field_qset_id_multi_delete(
    int unit,
    bcm_field_qualify_t qual,
    int num_objects,
    int objects_list[],
    bcm_field_qset_t *qset)
{
    SHR_FUNC_ENTER(unit);

    if (qual != bcmFieldQualifyUdf) {
        SHR_IF_ERR_VERBOSE_EXIT(BCM_E_UNAVAIL);
    }

    if (qset == NULL) {
        SHR_IF_ERR_VERBOSE_EXIT(BCM_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_field_udf_qset_del(unit, num_objects,
                                      objects_list,
                                      qset));
exit:
    SHR_FUNC_EXIT();
}

int
bcmint_field_group_action_strength_get(
    int unit,
    bcmint_field_stage_info_t *stage_info,
    int pipe,
    int group_prio,
    bcm_field_action_t action,
    uint32_t *strength)
{
    uint16_t iter_idx = 0;
    bool found = false;
    bcmint_field_action_strength_prof_t *base_ptr = NULL;
    bcm_field_action_t iter_action;

    SHR_FUNC_ENTER(unit);

    base_ptr = stage_info->strength_base_prof[pipe].strength_prof;
    SHR_NULL_CHECK(base_ptr, SHR_E_PARAM);

    /* Get the base_ptr corresponding to this group */
    for (; base_ptr != NULL; base_ptr = base_ptr->next) {
        if (base_ptr->group_priority == group_prio) {
            found = true;
            break;
        }
    }

    if (found == false) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (!(BCM_FIELD_QSET_TEST(base_ptr->aset, action))){
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    SHR_BIT_ITER(((base_ptr->aset).w), bcmFieldActionCount, iter_action) {
        if (iter_action == action) {
            *strength = base_ptr->action_strength[iter_idx];
            break;
        }
        iter_idx++;
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmint_field_group_primary_slice_id_get(
        int unit,
        bcm_field_group_t group,
        uint32_t *slice_id)
{
    int dunit = 0;
    uint32_t num_elem = 0;
    bcmlt_entry_handle_t lt_hdl = BCMLT_INVALID_HDL;
    bcmlt_field_def_t fld_defs;
    bcmint_field_stage_info_t *stage_info = NULL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(slice_id, SHR_E_PARAM);

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
       (bcmint_field_stage_from_group_get(unit, group, &stage_info));

    SHR_NULL_CHECK(stage_info->tbls_info->group_info_partition_sid, SHR_E_UNAVAIL);

    /* Entry handle allocate for operational mode. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit,
                              stage_info->tbls_info->group_info_partition_sid,
                              &lt_hdl));

    group = group & BCM_FIELD_ID_MASK;

    /* Add group_template_id field to Group part info LT. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(lt_hdl,
                               stage_info->tbls_info->group_info_partition_key_fid,
                               group));

    /*
     * Add partiotion ID field to Group part info LT with
     * value 1 for getting primary slice ID.
     */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(lt_hdl,
                               stage_info->tbls_info->group_info_common_partition_key_fid,
                               1));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, lt_hdl, BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));

    sal_memset(&fld_defs, 0, sizeof(bcmlt_field_def_t));

    /* Retreive field definition for provided lt field */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_field_def_get(unit,
                               stage_info->tbls_info->group_info_partition_sid,
                               stage_info->tbls_info->group_info_slice_fid,
                               &fld_defs));

    if ((0 == fld_defs.elements) || (fld_defs.symbol)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
    }

    if (fld_defs.depth > 1) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_array_get(lt_hdl,
                                         stage_info->tbls_info->group_info_slice_fid,
                                         0,
                                         (uint64_t *)slice_id,
                                         1,
                                         &num_elem));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(lt_hdl,
                                   stage_info->tbls_info->group_info_slice_fid,
                                   (uint64_t *)slice_id));
    }
exit:
    if (lt_hdl != BCMLT_INVALID_HDL) {
        (void)(bcmlt_entry_free(lt_hdl));
    }

    SHR_FUNC_EXIT();
}


static int
ltsw_fp_group_action_sbr_lt_insert(
    int unit,
    bcmint_field_stage_info_t *stage_info,
    int pipe,
    int group_priority,
    bcmint_field_action_strength_prof_t *str_prof,
    int update_all)
{
    int mode = 0;
    int dunit = 0, iter, iter1;
    uint32_t siter = 0;
    bool found = 0;
    char *sbr_sid = NULL;
    char *sbr_key_fid = NULL;
    char *pipe_fid = NULL;
    bcmi_field_action_t action;
    const bcm_field_action_map_t *lt_map_ref = NULL;
    bcmlt_entry_handle_t sbr_template = BCMLT_INVALID_HDL;
    bcmint_field_action_strength_prof_t *base_ptr = NULL;
    const bcm_field_action_internal_map_info_t *lt_intr_map = NULL;

    SHR_FUNC_ENTER(unit);

    if (update_all == 1) {
        base_ptr = stage_info->strength_base_prof[pipe].strength_prof;
    } else {
        base_ptr = str_prof;
    }

    dunit = bcmi_ltsw_dev_dunit(unit);

    if (str_prof->color) {
        if (stage_info->color_action_tbls_info == NULL) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
        }
        sbr_sid = stage_info->color_action_tbls_info->sbr_sid;
        sbr_key_fid = stage_info->color_action_tbls_info->sbr_key_fid;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_field_color_action_mode_get(unit,
                                                stage_info,
                                                &mode));
        if (mode == bcmiFieldOperModePipeUnique) {
            pipe_fid = stage_info->color_action_tbls_info->pipe_fid;
        }
    } else {
        sbr_sid = stage_info->tbls_info->sbr_sid;
        sbr_key_fid = stage_info->tbls_info->sbr_key_fid;
    }

    if ((sbr_sid == NULL) || (sbr_key_fid == NULL)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
    }

    for (; base_ptr != NULL; base_ptr = base_ptr->next) {
        if (base_ptr->update == 0) {
            continue;
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_allocate(dunit,
                                  sbr_sid,
                                  &sbr_template));
        SHR_IF_ERR_VERBOSE_EXIT
           (bcmlt_entry_field_add(sbr_template,
                                  sbr_key_fid,
                                  base_ptr->strength_prof_idx));

        if (pipe_fid != NULL) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(sbr_template,
                                       pipe_fid,
                                       pipe));
        }
        siter = 0;
        for (action = 0; action < bcmiFieldActionCount; action++) {
            if (BCM_FIELD_QSET_TEST(base_ptr->aset, action) == 0) {
                continue;
            }

            if ((bcm_field_action_t)action == bcmFieldActionStatGroup) {
                continue;
            }

            SHR_IF_ERR_VERBOSE_EXIT
               (bcmint_field_action_map_find(unit, action,
                                             stage_info, &lt_map_ref));

            /* Iterate through all lt maps */
            found = 0;
            for (iter = 0; iter < lt_map_ref->num_maps; iter++) {
                if ((lt_map_ref->map[iter]).sbr_enabled == FALSE) {
                    continue;
                }

                if ((str_prof->color) &&
                    ((lt_map_ref->map[iter]).policer == FALSE)) {
                    continue;
                }

                found = 1;
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_add(sbr_template,
                            (lt_map_ref->map[iter]).lt_field_name,
                            base_ptr->action_strength[siter]));

                /* Set internal maps associated if any. */
                if ((lt_map_ref->map[iter]).num_internal_maps > 0) {
                    lt_intr_map = (lt_map_ref->map[iter]).internal_map;

                    for (iter1 = 0;
                         iter1 < (lt_map_ref->map[iter]).num_internal_maps;
                         iter1++) {
                         if (lt_intr_map[iter1].sbr_enabled == FALSE) {
                             continue;
                         }

                         if (found == 0) {
                             /*
                              * If the main mapping action is not SBR enabled,
                              * return error.
                              */
                             SHR_IF_ERR_VERBOSE_EXIT(SHR_E_CONFIG);
                         }

                         SHR_IF_ERR_VERBOSE_EXIT
                             (bcmlt_entry_field_add(
                                        sbr_template,
                                        lt_intr_map[iter1].lt_field_name,
                                        base_ptr->action_strength[siter]));
                    }
                }
            }

            if (found) {
                siter++;
            }
        }

        /* Insert/update strength template */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_set_commit(unit, sbr_template,
                                      BCMLT_PRIORITY_NORMAL));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_free(sbr_template));
        sbr_template = BCMLT_INVALID_HDL;

        base_ptr->update = 0;

        if (update_all == 0) {
            break;
        }
    }

exit:
    if (sbr_template != BCMLT_INVALID_HDL) {
        (void)(bcmlt_entry_free(sbr_template));
    }
    SHR_FUNC_EXIT();
}

void
ltsw_fp_aset_action_print(int unit, bcm_field_aset_t *aset)
{
    bcm_field_action_t a;
    static char *action_name[bcmFieldActionCount] = BCM_FIELD_ACTION_STRINGS;

    for (a = 0; ((aset != NULL) && (a < bcmFieldActionCount)); a++) {
        if (BCM_FIELD_ASET_TEST(*aset, a)) {
            LOG_CLI((BSL_META("%s\n\r"), action_name[a]));
        }
    }
}        

static int
ltsw_fp_group_sbr_strength_calc(
    int unit,
    bcmint_field_stage_info_t *stage_info,
    int pipe,
    int group_priority,
    bcmint_field_action_strength_prof_t *str_prof,
    bool *update_all)
{
    int *action_str = NULL;
    int *grp_prio = NULL;
    int iter = 0, siter = 0, giter = 0;
    bcmi_field_action_t action;
    bcmint_field_action_strength_prof_t *base_ptr = NULL;

    SHR_FUNC_ENTER(unit);

    /* Allocate action stength */
    BCMINT_FIELD_MEM_ALLOC(action_str,
            sizeof(int) * stage_info->sbr_aset_cnt,
            "action strength values");
    BCMINT_FIELD_MEM_ALLOC(grp_prio,
            sizeof(int) * stage_info->sbr_aset_cnt,
            "group priority action strength values");

    /* Loop through all groups & populate strength values */
    for (iter = 0; iter < stage_info->sbr_aset_cnt; iter++) {
        grp_prio[iter] = -1;
        action_str[iter] = -1;
    }

    base_ptr = stage_info->strength_base_prof[pipe].strength_prof;
    for (; base_ptr != NULL; base_ptr = base_ptr->next) {
        if (base_ptr->color != str_prof->color) {
            continue;
        }

        siter = 0;
        giter = 0;
        for (action = 0; action < bcmiFieldActionCount; action++) {
            if (!(BCM_FIELD_QSET_TEST(stage_info->sbr_aset, action))){
                continue;
            }

            if (BCM_FIELD_QSET_TEST(base_ptr->aset, action)) {
                if (grp_prio[siter] == -1) {
                    action_str[siter] = 7;
                    grp_prio[siter] = base_ptr->group_priority;
                } else if (grp_prio[siter] != base_ptr->group_priority) {
                    action_str[siter] -= 1;
                    grp_prio[siter] = base_ptr->group_priority;
                }

                if (base_ptr->action_strength[giter] != action_str[siter]) {
                    base_ptr->action_strength[giter] = action_str[siter];
                    base_ptr->update = 1;
                    if (base_ptr != str_prof) {
                        *update_all = TRUE;
                    }
                }
                giter++;
            }
            siter++;
        }
    }

exit:
    if (action_str != NULL) {
        BCMINT_FIELD_MEM_FREE(action_str);
    }
    SHR_FUNC_EXIT();
}

static int
ltsw_fp_group_strength_insert(
    int unit,
    bcmint_field_stage_info_t *stage_info,
    int pipe,
    int group_priority,
    int strength_idx,
    bcmi_field_aset_t *sbr_aset,
    uint32_t a_cnt,
    bool color,
    bcmint_field_action_strength_prof_t **prof)
{
    bool update_all = 0;
    bcmint_field_action_strength_prof_t *str_prof = NULL;

    SHR_FUNC_ENTER(unit);
    /* Allocate strength profile */
    BCMINT_FIELD_MEM_ALLOC(str_prof,
           sizeof(bcmint_field_action_strength_prof_t),
           "Group strength prof");

    /* Allocate stength profile action values */
    BCMINT_FIELD_MEM_ALLOC(str_prof->action_strength,
           sizeof(uint32_t) * a_cnt,
           "strength prof values");

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_field_group_strength_insert(unit,
                                          stage_info,
                                          pipe,
                                          group_priority,
                                          str_prof));

    /* Update strength profile members */
    str_prof->group_priority = group_priority;
    sal_memcpy(&str_prof->aset, sbr_aset, sizeof(bcmi_field_aset_t));
    str_prof->action_strength_cnt = a_cnt;
    str_prof->strength_prof_idx = strength_idx;
    str_prof->color = color;
    str_prof->ref_cnt = 1;

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_fp_group_sbr_strength_calc(
               unit, stage_info, pipe, group_priority,
               str_prof, &update_all));

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_fp_group_action_sbr_lt_insert(
               unit, stage_info, pipe,
               group_priority,
               str_prof,
               update_all));

    *prof = str_prof;
exit:
    if (SHR_FUNC_ERR()) {
        if (str_prof != NULL) {
            if (str_prof->action_strength != NULL) {
                BCMINT_FIELD_MEM_FREE(str_prof->action_strength);
            }
            BCMINT_FIELD_MEM_FREE(str_prof);
        }
        *prof = NULL;
    }

    SHR_FUNC_EXIT();
}

static int
ltsw_fp_group_action_sbr_alloc(
    int unit,
    bcmint_field_stage_info_t *stage_info,
    int pipe,
    int group_priority,
    bcmi_field_aset_t *sbr_aset,
    uint32_t a_cnt,
    bcmi_field_aset_t *color_sbr_aset,
    uint32_t color_a_cnt,
    bcmint_field_action_strength_prof_t **strength_prof,
    bcmint_field_action_strength_prof_t **color_strength_prof)
{
    uint8_t prio_ct = 0;
    int pipe_iter = 0;
    int prev_grp_prio;
    bcmi_field_action_t action;
    bool exist = FALSE, found = FALSE;
    bcmint_field_action_strength_prof_t *base_ptr = NULL;

    SHR_FUNC_ENTER(unit);

    /* Count number of priorties used & group with same priority */
    if (a_cnt) {
        prev_grp_prio = -1;
        for (action = 0; action < bcmiFieldActionCount; action++) {
            if (BCM_FIELD_ASET_TEST(*sbr_aset, action) == 0) {
                continue;
            }

            if ((bcm_field_action_t)action == bcmFieldActionStatGroup) {
                continue;
            }

            prio_ct = 0;
            base_ptr = stage_info->strength_base_prof[pipe].strength_prof;
            for (; base_ptr != NULL; base_ptr = base_ptr->next) {
                if (BCM_FIELD_ASET_TEST(base_ptr->aset, action)) {
                    if ((base_ptr->group_priority != group_priority) &&
                        (base_ptr->group_priority != prev_grp_prio)) {
                        prio_ct++;
                        prev_grp_prio = base_ptr->group_priority;
                    }
                }
            }

            if (prio_ct == 7) {
                LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META_U(unit,
                   "Error: Exceeded strength action "
                   "priorities(%d) allowed.\n"), prio_ct));
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_RESOURCE);
            }
        }
    }

    if (color_a_cnt) {
        prev_grp_prio = -1;
        for (action = 0; action < bcmiFieldActionCount; action++) {
            if (BCM_FIELD_ASET_TEST(*color_sbr_aset, action) == 0) {
                continue;
            }

            prio_ct = 0;
            base_ptr = stage_info->strength_base_prof[pipe].strength_prof;
            for (; base_ptr != NULL; base_ptr = base_ptr->next) {
                if (BCM_FIELD_ASET_TEST(base_ptr->aset, action)) {
                    if ((base_ptr->group_priority != group_priority) &&
                        (base_ptr->group_priority != prev_grp_prio)) {
                        prio_ct++;
                        prev_grp_prio = base_ptr->group_priority;
                    }
                }
            }

            if (prio_ct == 7) {
                LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META_U(unit,
                   "Error: Exceeded strength action "
                   "priorities(%d) allowed.\n"), prio_ct));
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_RESOURCE);
            }
        }
    }

    if (a_cnt) {
        uint32_t strength_id = 1;
        int oper_mode = 0;
        uint8_t num_pipe = 0;

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_field_oper_mode_status_get(unit, stage_info, &oper_mode));
        if (oper_mode == bcmiFieldOperModePipeUnique) {
            num_pipe = bcmi_ltsw_dev_max_pp_pipe_num(unit);
        } else {
            num_pipe = 1;
        }

        /* Generate strength ID */
        for (strength_id = 1;
             strength_id <= stage_info->strength_id_max;
             strength_id++) {
            exist = FALSE;
            for (pipe_iter = 0; pipe_iter < num_pipe; pipe_iter++) {
                base_ptr =
                    stage_info->strength_base_prof[pipe_iter].strength_prof;
                for (; base_ptr != NULL; base_ptr = base_ptr->next) {
                    if ((base_ptr->color == 0) &&
                        (base_ptr->strength_prof_idx == strength_id)) {
                        exist = TRUE;
                        break;
                    }
                }
                if (exist == true) {
                    break;
                }
            }
            if (exist == FALSE) {
                found = TRUE;
                break;
            }
        }

        if (found == FALSE) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_RESOURCE);
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_fp_group_strength_insert(unit, stage_info,
                                           pipe,
                                           group_priority,
                                           strength_id,
                                           sbr_aset,
                                           a_cnt,
                                           0, strength_prof));
    }

    if (color_a_cnt) {
        int color_sbr_idx = 0;
        int mode = 0, act_pipe = -1;

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_field_color_action_mode_get(unit,
                                                stage_info,
                                                &mode));
        if (mode == bcmiFieldOperModePipeUnique) {
            act_pipe = pipe;
        }
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_field_color_sbr_id_alloc(unit, stage_info,
                                             act_pipe,
                                             &color_sbr_idx));
        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_fp_group_strength_insert(unit, stage_info,
                                           pipe,
                                           group_priority,
                                           color_sbr_idx,
                                           color_sbr_aset,
                                           color_a_cnt,
                                           1, color_strength_prof));
    }

exit:
    SHR_FUNC_EXIT();
}

static int
ltsw_fp_group_action_sbr_find(
    int unit,
    bcmint_field_stage_info_t *stage_info,
    int pipe,
    int group_priority,
    bcmi_field_aset_t *sbr_aset,
    uint32_t a_cnt,
    bcmi_field_aset_t *color_sbr_aset,
    uint32_t color_a_cnt,
    bcmint_field_action_strength_prof_t **strength_prof,
    bcmint_field_action_strength_prof_t **color_strength_prof)
{
    bcmint_field_action_strength_prof_t *base_ptr = NULL;

    SHR_FUNC_ENTER(unit);

    *strength_prof = NULL;
    *color_strength_prof = NULL;
    base_ptr = stage_info->strength_base_prof[pipe].strength_prof;
    for (; base_ptr != NULL; base_ptr = base_ptr->next) {
         if (base_ptr->group_priority != group_priority) {
             continue;
         }

         if ((a_cnt) &&
             (sal_memcmp(&base_ptr->aset, sbr_aset,
                         sizeof(bcmi_field_aset_t)) == 0)) {
             *strength_prof = base_ptr;
         }

         if ((color_a_cnt) &&
             (sal_memcmp(&base_ptr->aset, color_sbr_aset,
                         sizeof(bcmi_field_aset_t)) == 0)) {
             *color_strength_prof = base_ptr;
         }
    }

    SHR_FUNC_EXIT();
}

int
bcmint_field_group_strength_prof_create(
    int unit,
    bcmint_field_stage_info_t *stage_info,
    int pipe,
    int group_priority,
    int hintid,
    int policer_pool_id,
    bcm_field_aset_t *aset,
    uint64_t *strength_prof_id,
    int *color_sbr_idx)
{
    bcmi_field_aset_t sbr_aset;
    bcmi_field_aset_t valid_aset;
    bcmi_field_aset_t color_sbr_aset;
    uint8_t a_ct = 0, color_a_ct = 0;
    bcmint_field_action_strength_prof_t *str_prof = NULL;
    bcmint_field_action_strength_prof_t *color_str_prof = NULL;

    SHR_FUNC_ENTER(unit);
    BCMI_FIELD_ASET_INIT(sbr_aset);
    BCMI_FIELD_ASET_INIT(valid_aset);
    BCMI_FIELD_ASET_INIT(color_sbr_aset);

    sal_memcpy(&valid_aset, aset, sizeof(bcm_field_aset_t));

    /* retrieve the stat related actions */
    if (BCM_FIELD_ASET_TEST(*aset, bcmFieldActionStatGroup)) {
        int a, action_stat_cnt = 0;
        bcmi_field_action_t action_stat_arr[10];

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_field_flexctr_valid_action_get(
                           unit, stage_info,
                           hintid, &valid_aset,
                           10, action_stat_arr,
                           &action_stat_cnt));
        for (a = 0; a < action_stat_cnt; a++) {
             BCM_FIELD_ASET_ADD(valid_aset, action_stat_arr[a]);
        }
    }

    if ((stage_info->flags & BCMINT_FIELD_STAGE_POLICY_TYPE_PDD) &&
        (BCM_FIELD_ASET_TEST(*aset, bcmFieldActionPolicerGroup)) &&
        (stage_info->flags & BCMINT_FIELD_STAGE_POLICER_ACTION_SUPPORT) &&
        (policer_pool_id != -1)) {
        int idx = 0;
        const bcm_field_action_map_t *lt_map = NULL;

        BCM_FIELD_ASET_REMOVE(valid_aset, bcmFieldActionPolicerGroup);

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_field_action_map_find(unit,
                                          bcmFieldActionPolicerGroup,
                                          stage_info,
                                          &lt_map));
        for (idx = 0; idx < lt_map->num_maps; idx++) {
            if (lt_map->map[idx].sbr_enabled != TRUE) {
                continue;
            }
            if ((lt_map->map[idx]).color_index == policer_pool_id) {
                BCM_FIELD_ASET_ADD(valid_aset, bcmFieldActionPolicerGroup);
            }
        }
    }

    /*
     * Validate Group's ASET to check whether
     * the actions in the ASET needs strength resolution.
     */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_action_strength_aset_get(unit, stage_info,
                                          &valid_aset,
                                          &sbr_aset,
                                          &a_ct,
                                          &color_sbr_aset,
                                          &color_a_ct));
    if ((a_ct == 0) && (color_a_ct == 0)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_fp_group_default_strength_add(unit, stage_info,
                                                strength_prof_id));
        SHR_EXIT();
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_fp_group_action_sbr_find(unit, stage_info,
                                       pipe,
                                       group_priority,
                                       &sbr_aset,
                                       a_ct,
                                       &color_sbr_aset,
                                       color_a_ct,
                                       &str_prof,
                                       &color_str_prof));
    if (str_prof != NULL) {
        str_prof->ref_cnt += 1;
        a_ct = 0;
    }

    if (color_str_prof != NULL) {
        color_str_prof->ref_cnt += 1;
        color_a_ct = 0;
    }

    if ((str_prof == NULL) || (color_str_prof == NULL)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_fp_group_action_sbr_alloc(unit, stage_info,
                                            pipe,
                                            group_priority,
                                            &sbr_aset,
                                            a_ct,
                                            &color_sbr_aset,
                                            color_a_ct,
                                            &str_prof,
                                            &color_str_prof));
    }

    if (str_prof != NULL) {
        *strength_prof_id = str_prof->strength_prof_idx;
    }

    if (color_str_prof != NULL) {
        *color_sbr_idx = color_str_prof->strength_prof_idx;
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmint_field_group_strength_prof_destroy(
    int unit,
    bcmint_field_stage_info_t *stage_info,
    int pipe,
    int group_priority,
    uint32 strength_prof_id,
    int color_sbr_idx)
{
    bool delete = false;
    char *pipe_fid = NULL;
    bcmlt_entry_handle_t sbr_template = BCMLT_INVALID_HDL;
    bcmlt_entry_handle_t color_sbr_template = BCMLT_INVALID_HDL;
    bcmint_field_color_action_tbls_info_t *color_tbl_info = NULL;
    bcmint_field_action_strength_prof_t *base_ptr = NULL;

    SHR_FUNC_ENTER(unit);

    base_ptr = stage_info->strength_base_prof[pipe].strength_prof;
    for (; base_ptr != NULL; base_ptr = base_ptr->next) {
        if (base_ptr->group_priority != group_priority) {
            continue;
        }

        if ((base_ptr->color == 0) &&
            (base_ptr->strength_prof_idx == strength_prof_id)) {
            base_ptr->ref_cnt -= 1;
            if (base_ptr->ref_cnt != 0) {
                strength_prof_id = 0;
            }
        }

        if ((base_ptr->color == 1) &&
            (base_ptr->strength_prof_idx == color_sbr_idx)) {
            base_ptr->ref_cnt -= 1;
            if (base_ptr->ref_cnt != 0) {
                color_sbr_idx = -1;
            }
        }
    }

    color_tbl_info = stage_info->color_action_tbls_info;
    if (color_tbl_info != NULL && color_sbr_idx != -1) {
        int mode = 0, act_pipe = -1;

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_field_color_action_mode_get(unit,
                                                stage_info,
                                                &mode));
        if (mode == bcmiFieldOperModePipeUnique) {
            act_pipe= pipe;
        }
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_field_color_sbr_id_free(unit,
                                            stage_info,
                                            color_sbr_idx,
                                            act_pipe,
                                            &delete));
        if (delete == true) {
            if (act_pipe != -1) {
                pipe_fid = color_tbl_info->pipe_fid;
            }
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmint_field_lt_entry_commit(unit, &color_sbr_template,
                                              BCMLT_OPCODE_DELETE,
                                              BCMLT_PRIORITY_NORMAL,
                                              color_tbl_info->sbr_sid,
                                              color_tbl_info->sbr_key_fid,
                                              0, color_sbr_idx, NULL,
                                              pipe_fid,
                                              act_pipe,
                                              0, 0,
                                              1));
        }

        /* Remove color strength profile from local db */
        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_fp_group_strength_prof_dealloc(unit, stage_info,
                                                 pipe, 1,
                                                 color_sbr_idx));
    }

    if ((strength_prof_id != 0) &&
        (strength_prof_id != stage_info->sbr_max_limit)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_field_lt_entry_commit(unit, &sbr_template,
                                          BCMLT_OPCODE_DELETE,
                                          BCMLT_PRIORITY_NORMAL,
                                          stage_info->tbls_info->sbr_sid,
                                          stage_info->tbls_info->sbr_key_fid,
                                          0, strength_prof_id, NULL,
                                          NULL,
                                          0,
                                          0, 0,
                                          1));
        /* Remove strength profile from local db */
        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_fp_group_strength_prof_dealloc(unit, stage_info,
                                                 pipe, 0,
                                                 strength_prof_id));
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmint_field_group_destroy_all(
    int unit,
    bcmint_field_stage_info_t *stage_info)
{
    int stage_id;
    uint32_t idx;
    bcm_field_group_t group_id;
    bcmi_field_ha_group_oper_t *grp_oper = NULL;
    bcmi_field_ha_blk_info_t *grp_base = NULL;

    SHR_FUNC_ENTER(unit);

    stage_id = stage_info->stage_id;

    if (FP_HA_INFO(unit) == NULL) {
        SHR_EXIT();
    }

    /* Field Stage clear */
    FP_HA_GRP_BASE_BLK_GET(unit,
                           stage_id,
                           0,
                           grp_base);
    if (grp_base == NULL) {
        SHR_EXIT();
    }

    for (idx = 0; idx < stage_info->group_info->hash_size; idx++) {
        grp_oper = NULL;
        FP_HA_GRP_BASE_OPER_GET(unit,
                                stage_id,
                                grp_base,
                                idx,
                                grp_oper);
        if (grp_oper == NULL) {
            continue;
        }
        do {
            group_id = grp_oper->group_id;
            FP_HA_GRP_OPER_NEXT(unit, grp_oper, grp_oper);

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmint_field_entry_destroy_all(unit,
                                                group_id));
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmint_field_group_destroy(unit,
                                            group_id));
        } while (grp_oper != NULL);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmint_field_strength_destroy_all(
    int unit,
    bcmint_field_stage_info_t *stage_info)
{
    int pipe = -1, num_pipe = 0;
    int oper_mode = 0;
    bcmint_field_action_strength_prof_t *base_ptr = NULL;
    bcmint_field_action_strength_prof_t *next_ptr = NULL;

    SHR_FUNC_ENTER(unit);

    if (!(stage_info->flags & BCMINT_FIELD_STAGE_ACTION_RESOLUTION_SBR)) {
        SHR_EXIT();
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_oper_mode_status_get(unit, stage_info, &oper_mode));
    if (oper_mode == bcmiFieldOperModePipeUnique) {
        num_pipe = bcmi_ltsw_dev_max_pp_pipe_num(unit);
    } else {
        num_pipe = 1;
    }

    for (pipe = 0; pipe < num_pipe; pipe++) {
        base_ptr = stage_info->strength_base_prof[pipe].strength_prof;
        while (base_ptr != NULL) {
            next_ptr = base_ptr->next;
            BCMINT_FIELD_MEM_FREE(base_ptr->action_strength);
            BCMINT_FIELD_MEM_FREE(base_ptr);
            base_ptr = next_ptr;
        }
        stage_info->strength_base_prof[pipe].strength_prof = NULL;
    }

exit:
    SHR_FUNC_EXIT();
}

static int
ltsw_field_strength_reallocate(
    int unit,
    bcmint_field_stage_info_t *stage_info,
    int pipe,
    int group_priority,
    uint32 strength_idx,
    bcmi_field_aset_t *sbr_aset,
    uint8_t a_cnt,
    bool color)
{
    int dunit;
    bool found = FALSE;
    int iter, cnt = 0;
    uint64_t act_strength;
    char *sbr_sid = NULL;
    char *sbr_key_fid = NULL;
    bcmi_field_action_t action;
    int *act_strength_arr = NULL;
    bcmint_field_action_strength_prof_t *str_prof = NULL;
    const bcm_field_action_map_t *lt_map_ref = NULL;
    bcmlt_entry_handle_t sbr_templ = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);
    dunit = bcmi_ltsw_dev_dunit(unit);

    /* Allocate strength profile */
    BCMINT_FIELD_MEM_ALLOC(str_prof,
           sizeof(bcmint_field_action_strength_prof_t),
           "Group strength prof");

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_field_group_strength_insert(unit,
                                          stage_info,
                                          pipe,
                                          group_priority,
                                          str_prof));

    /* Update strength profile members */
    str_prof->group_priority = group_priority;
    sal_memcpy(&str_prof->aset, sbr_aset, sizeof(bcm_field_aset_t));
    str_prof->action_strength_cnt = a_cnt;
    str_prof->strength_prof_idx = strength_idx;
    str_prof->color = color;
    str_prof->ref_cnt = 1;

    dunit = bcmi_ltsw_dev_dunit(unit);

    if (color) {
        if (stage_info->color_action_tbls_info == NULL) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
        }
        sbr_sid = stage_info->color_action_tbls_info->sbr_sid;
        sbr_key_fid = stage_info->color_action_tbls_info->sbr_key_fid;
    } else {
        sbr_sid = stage_info->tbls_info->sbr_sid;
        sbr_key_fid = stage_info->tbls_info->sbr_key_fid;
    }

    if ((sbr_sid == NULL) || (sbr_key_fid == NULL)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit,
                              sbr_sid,
                              &sbr_templ));
    SHR_IF_ERR_VERBOSE_EXIT
       (bcmlt_entry_field_add(sbr_templ,
                              sbr_key_fid,
                              strength_idx));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, sbr_templ,
                              BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));

    /* Allocate stength profile action values */
    BCMINT_FIELD_MEM_ALLOC(act_strength_arr,
            sizeof(uint32_t) * a_cnt,
            "WB local strength prof values");


    for (action = 0; action < bcmiFieldActionCount; action++) {
        if (BCM_FIELD_ASET_TEST(*sbr_aset, action) == 0) {
            continue;
        }

        SHR_IF_ERR_VERBOSE_EXIT
           (bcmint_field_action_map_find(unit, action,
                                         stage_info, &lt_map_ref));
        for (iter = 0; iter < lt_map_ref->num_maps; iter++) {
            if ((lt_map_ref->map[iter]).sbr_enabled == FALSE) {
                continue;
            }

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_get(sbr_templ,
                    (lt_map_ref->map[iter]).lt_field_name,
                    &act_strength));
            found = TRUE;
        }

        if (found == TRUE) {
            act_strength_arr[cnt++] = act_strength;
            found = FALSE;
        }
    }

    /* Allocate stength profile action values */
    BCMINT_FIELD_MEM_ALLOC(str_prof->action_strength,
            sizeof(uint32_t) * cnt,
            "strength prof values");

    /* Update strength profile members */
    sal_memcpy(str_prof->action_strength,
               act_strength_arr, sizeof(uint32_t) * cnt);

exit:
    if (act_strength_arr != NULL) {
        BCMINT_FIELD_MEM_FREE(act_strength_arr);
    }
    if (BCMLT_INVALID_HDL != sbr_templ) {
        (void) bcmlt_entry_free(sbr_templ);
    }

    SHR_FUNC_EXIT();
}

static int
ltsw_field_strength_reinsert(
    int unit,
    bcmint_field_stage_info_t *stage_info,
    int pipe,
    int group_priority,
    int hintid,
    int policer_pool_id,
    uint16_t *action_arr,
    uint8_t num_actions,
    uint32_t strength_idx,
    int color_strength_idx)
{
    bcmi_field_aset_t aset;
    bcmi_field_aset_t sbr_aset;
    bcmi_field_aset_t color_sbr_aset;
    uint8_t a_ct = 0, color_a_ct = 0;
    bcmint_field_action_strength_prof_t *str_prof = NULL;
    bcmint_field_action_strength_prof_t *color_str_prof = NULL;
    SHR_FUNC_ENTER(unit);

    BCMI_FIELD_ASET_INIT(aset);
    BCMI_FIELD_ASET_INIT(sbr_aset);
    BCMI_FIELD_ASET_INIT(color_sbr_aset);
    BCMINT_FIELD_ACTION_ARR_TO_ASET(num_actions,
                                    action_arr,
                                    aset);

    /* retrieve the stat related actions */
    if (BCM_FIELD_ASET_TEST(aset, bcmFieldActionStatGroup)) {
        int a, action_stat_cnt = 0;
        bcmi_field_action_t action_stat_arr[10];

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_field_flexctr_valid_action_get(
                           unit, stage_info,
                           hintid, &aset,
                           10, action_stat_arr,
                           &action_stat_cnt));
        for (a = 0; a < action_stat_cnt; a++) {
            BCM_FIELD_ASET_ADD(aset, action_stat_arr[a]);
        }
    }

    if (BCM_FIELD_ASET_TEST(aset, bcmFieldActionPolicerGroup) &&
        (stage_info->flags & BCMINT_FIELD_STAGE_POLICER_ACTION_SUPPORT) &&
        (policer_pool_id != -1)) {
        int idx = 0;
        const bcm_field_action_map_t *lt_map = NULL;

        BCM_FIELD_ASET_REMOVE(aset, bcmFieldActionPolicerGroup);

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_field_action_map_find(unit,
                                          bcmFieldActionPolicerGroup,
                                          stage_info,
                                          &lt_map));
        for (idx = 0; idx < lt_map->num_maps; idx++) {
            if (lt_map->map[idx].sbr_enabled != TRUE) {
                continue;
            }
            if ((lt_map->map[idx]).color_index == policer_pool_id) {
                BCM_FIELD_ASET_ADD(aset, bcmFieldActionPolicerGroup);
            }
        }
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_action_strength_aset_get(
                                          unit, stage_info,
                                          &aset,
                                          &sbr_aset,
                                          &a_ct,
                                          &color_sbr_aset,
                                          &color_a_ct));
    if ((a_ct == 0) && (color_a_ct == 0)) {
        uint64_t s_idx = strength_idx;

        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_fp_group_default_strength_add(unit, stage_info,
                                                &s_idx));
        SHR_EXIT();
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_fp_group_action_sbr_find(unit, stage_info,
                                       pipe,
                                       group_priority,
                                       &sbr_aset,
                                       a_ct,
                                       &color_sbr_aset,
                                       color_a_ct,
                                       &str_prof,
                                       &color_str_prof));
    if (str_prof != NULL) {
        if (str_prof->strength_prof_idx != strength_idx) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
        }
        str_prof->ref_cnt += 1;
    } else if (a_ct) {
        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_field_strength_reallocate(
                  unit, stage_info, pipe,
                  group_priority,
                  strength_idx,
                  &sbr_aset,
                  a_ct,
                  0));
    }

    if (color_str_prof != NULL) {
        if (color_str_prof->strength_prof_idx != color_strength_idx) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
        }
        color_str_prof->ref_cnt += 1;
    } else if (color_a_ct) {
        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_field_strength_reallocate(
                  unit, stage_info, pipe,
                  group_priority,
                  color_strength_idx,
                  &color_sbr_aset,
                  color_a_ct,
                  1));
    }

exit:
    SHR_FUNC_EXIT();
}


static int
ltsw_field_group_strength_realloc(
    int unit,
    bcmint_field_stage_info_t *stage_info,
    bcmi_field_ha_group_oper_t *g_oper)
{
    int idx;
    int pipe = -1;
    bcmi_field_ha_entry_oper_t *e_oper;
    bcmi_field_ha_blk_info_t *entry_base = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_group_pipe_get(unit, stage_info,
                                     g_oper, &pipe));
    pipe = (pipe == -1) ? 0 : pipe;

    if ((g_oper->group_flags & BCMINT_FIELD_GROUP_ENT_LVL_ACTION_RESOLVE) == 0) {
        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_field_strength_reinsert(unit, stage_info, pipe,
                                          g_oper->priority,
                                          g_oper->hintid,
                                          g_oper->policer_pool_id,
                                          g_oper->aset_arr,
                                          g_oper->action_cnt,
                                          g_oper->strength_prof_id,
                                          g_oper->color_sbr_id));
        SHR_EXIT();
    }

    FP_HA_ENT_BASE_BLK_GET(unit,
                           stage_info->stage_id,
                           0,
                           entry_base);
    for (idx = 0; idx < stage_info->entry_info->hash_size; idx++) {
        e_oper = NULL;
        FP_HA_ENT_BASE_OPER_GET(unit,
                                stage_info->stage_id,
                                entry_base,
                                idx,
                                e_oper);
        while (e_oper != NULL) {
            if (e_oper->group_id == g_oper->group_id) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (ltsw_field_strength_reinsert(unit, stage_info, pipe,
                                                  g_oper->priority,
                                                  g_oper->hintid,
                                                  g_oper->policer_pool_id,
                                                  e_oper->action_arr,
                                                  e_oper->num_actions,
                                                  e_oper->strength_idx,
                                                  e_oper->color_strength_idx));
            }
            FP_HA_ENT_OPER_NEXT(unit, e_oper, e_oper);
        } while (e_oper != NULL);
    }

exit:
    SHR_FUNC_EXIT();
}


int
bcmint_field_group_strength_reinit(
    int unit,
    bcmint_field_stage_info_t *stage_info)
{
    uint32_t idx;
    bcmi_field_ha_group_oper_t *g_oper = NULL;
    bcmi_field_ha_blk_info_t *grp_base = NULL;
    SHR_FUNC_ENTER(unit);

    if (!(stage_info->flags & BCMINT_FIELD_STAGE_ACTION_RESOLUTION_SBR)) {
        SHR_EXIT();
    }

    /* Input parameter check. */
    SHR_NULL_CHECK(stage_info, SHR_E_PARAM);

    FP_HA_GRP_BASE_BLK_GET(unit,
                           stage_info->stage_id,
                           0,
                           grp_base);
    for (idx = 0; idx < stage_info->group_info->hash_size; idx++) {
        g_oper = NULL;
        FP_HA_GRP_BASE_OPER_GET(unit,
                                stage_info->stage_id,
                                grp_base,
                                idx,
                                g_oper);
        if (g_oper == NULL) {
            continue;
        }

        /* Search the list of entries */
        do {
            SHR_IF_ERR_VERBOSE_EXIT
                (ltsw_field_group_strength_realloc(unit, stage_info,
                                                   g_oper));
            FP_HA_GRP_OPER_NEXT(unit, g_oper, g_oper);
        } while (g_oper != NULL);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmint_field_color_action_mode_get(int unit,
                                   bcmint_field_stage_info_t *stage_info,
                                   int *mode)
{
    int dunit = 0;
    uint64_t oper_mode = 0x0;
    bcmlt_entry_handle_t oper_mode_template = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(stage_info, SHR_E_INTERNAL);
    SHR_NULL_CHECK(mode, SHR_E_INTERNAL);
    dunit = bcmi_ltsw_dev_dunit(unit);

    if (stage_info->flags & BCMINT_FIELD_STAGE_PIPE_MODE_NOT_SUPPORTED) {
        *mode = bcmiFieldOperModeGlobal;
        SHR_EXIT();
    }

    /* Entry handle allocate for operational mode. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit,
                              stage_info->color_action_tbls_info->pipe_cfg_sid,
                              &oper_mode_template));

    /* Lookup mode template using handle. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, oper_mode_template,
                              BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));

    /* Retreive operation mode field from fetched entry. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(oper_mode_template,
                               stage_info->color_action_tbls_info->pipe_cfg_fid,
                               &oper_mode));

    *mode = oper_mode;

exit:
    if (BCMLT_INVALID_HDL != oper_mode_template) {
        (void) bcmlt_entry_free(oper_mode_template);
    }
    SHR_FUNC_EXIT();
}

int
bcmint_field_color_action_pipe_get(int unit,
                                   bcmint_field_stage_info_t *stage_info,
                                   bcm_field_group_t group,
                                   int *pipe)
{
    int group_id = 0;
    int oper_mode = 0;
    uint64_t pipe_val = 0;
    bcmi_field_ha_group_oper_t *group_oper = NULL;
    bcmlt_entry_handle_t grp_template = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    group_id = group & BCM_FIELD_ID_MASK;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_group_oper_lookup(unit, group_id,
                                        stage_info,
                                        &group_oper));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_color_action_mode_get(unit, stage_info, &oper_mode));

    if (oper_mode == bcmiFieldOperModePipeUnique) {
        /* Group template lookup */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_field_lt_entry_commit(unit,
                                          &grp_template,
                                          BCMLT_OPCODE_LOOKUP,
                                          BCMLT_PRIORITY_NORMAL,
                                          stage_info->tbls_info->group_sid,
                                          stage_info->tbls_info->group_key_fid,
                                          0, group_id, NULL,
                                          NULL, 0, 0, 0, 0));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(grp_template,
                                   "PIPE",
                                   &pipe_val));
        *pipe = (int)pipe_val;
    } else {
        *pipe = -1;
    }

exit:
    if (BCMLT_INVALID_HDL != grp_template) {
        (void) bcmlt_entry_free(grp_template);
    }
    SHR_FUNC_EXIT();
}

int
bcmint_field_flexctr_valid_action_get(
    int unit,
    bcmint_field_stage_info_t *stage_info,
    int hintid,
    bcmi_field_aset_t *aset,
    int max_action_cnt,
    bcmi_field_action_t *action_arr,
    int *action_cnt)
{
    bool is_valid = false;
    int idx, iter, iter1;
    const bcm_field_action_map_t *ltmap = NULL;
    const bcm_field_action_map_t *ltmap_ref = NULL;
    const bcm_field_action_map_t *ltmap_tmp = NULL;

    SHR_FUNC_ENTER(unit);

    if (max_action_cnt == 0) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
    }

    *action_cnt = 0;
    if (!BCM_FIELD_ASET_TEST(*aset, bcmFieldActionStatGroup)) {
        SHR_EXIT();
    } else {
        BCM_FIELD_ASET_REMOVE(*aset, bcmFieldActionStatGroup);
    }

    if (bcmint_field_action_map_find
          (unit, bcmFieldActionStatGroup,
           stage_info, &ltmap_ref) != SHR_E_NONE) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
    }

    for (iter = 0; iter < ltmap_ref->num_maps; iter++) {
        /* include the field based on opaque used used by group */
        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_field_action_stat_group_lt_field_validate(
                unit, stage_info, hintid,
                (ltmap_ref->map[iter]).lt_field_name,
                &is_valid));
        if (!is_valid) {
            continue;
        }

        ltmap = stage_info->action_lt_map_db;
        for (idx = 0; idx < stage_info->action_db_count; idx++) {
            ltmap_tmp = ltmap + idx;

            for (iter1 = 0; iter1 < ltmap_tmp->num_maps; iter1++) {
                if ((ltmap_tmp != NULL) &&
                    (sal_strcmp((ltmap_ref->map[iter]).lt_field_name,
                                (ltmap_tmp->map[iter1]).lt_field_name) == 0)) {
                    if (ltmap_tmp->map[iter1].sbr_enabled == false) {
                        continue;
                    }

                    if ((ltmap_tmp->flags & BCMI_FIELD_ACTION_INTERNAL) == 0) {
                        continue;
                    }
    
                    action_arr[*action_cnt] = ltmap_tmp->action;
                    *action_cnt += 1;
                }
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_group_expansion_enable_get(
    int unit,
    bcm_field_group_t group,
    int *enable)
{
    uint64_t enable_val = 0;
    bcmlt_entry_handle_t grp_template = BCMLT_INVALID_HDL;
    bcmint_field_stage_info_t *stage_info = NULL;
    int gid = 0;

    SHR_FUNC_ENTER(unit);

    /* Validate the group */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_stage_from_group_get(unit, group,
                                           &stage_info));
    gid = (group & BCM_FIELD_ID_MASK);
    /* Group template lookup */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_lt_entry_commit(unit,
                                      &grp_template,
                                      BCMLT_OPCODE_LOOKUP,
                                      BCMLT_PRIORITY_NORMAL,
                                      stage_info->tbls_info->group_sid,
                                      stage_info->tbls_info->group_key_fid,
                                      0, gid, NULL,
                                      NULL, 0, 0, 0, 0));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(grp_template,
                               "AUTO_EXPAND",
                               &enable_val));
    *enable = COMPILER_64_LO(enable_val);

exit:
    if (BCMLT_INVALID_HDL != grp_template) {
        (void) bcmlt_entry_free(grp_template);
    }
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_field_group_expansion_enable_set(
    int unit,
    bcm_field_group_t group,
    int enable)
{
    uint64_t enable_val = 0;
    bcmlt_entry_handle_t grp_template = BCMLT_INVALID_HDL;
    bcmint_field_stage_info_t *stage_info = NULL;
    int gid = 0;
    SHR_FUNC_ENTER(unit);

    /* Validate the group */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_stage_from_group_get(unit, group,
                                           &stage_info));
    enable_val = enable;
    gid = (group & BCM_FIELD_ID_MASK);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_lt_entry_commit(unit,
                                      &grp_template,
                                      BCMLT_OPCODE_UPDATE,
                                      BCMLT_PRIORITY_NORMAL,
                                      stage_info->tbls_info->group_sid,
                                      stage_info->tbls_info->group_key_fid,
                                      0, gid, NULL,
                                      "AUTO_EXPAND",
                                      enable_val,
                                      0, 0,
                                      1));
exit:
    SHR_FUNC_EXIT();
}

int
bcmi_field_group_active_entries_get(
    int unit,
    bcm_field_group_t group,
    uint64_t *entries_created)
{
    bcmlt_entry_handle_t grp_info_templ = BCMLT_INVALID_HDL;
    bcmint_field_stage_info_t *stage_info = NULL;
    int gid = 0;

    SHR_FUNC_ENTER(unit);

    /* Validate the group */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_stage_from_group_get(unit, group,
                                           &stage_info));
    gid = (group & BCM_FIELD_ID_MASK);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_field_lt_entry_commit(unit,
                                      &grp_info_templ,
                                      BCMLT_OPCODE_LOOKUP,
                                      BCMLT_PRIORITY_NORMAL,
                                      stage_info->tbls_info->group_info_sid,
                                      stage_info->tbls_info->group_info_key_fid,
                                      0, gid, NULL,
                                      NULL,
                                      0,
                                      0, 0,
                                      0));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(grp_info_templ,
                               stage_info->tbls_info->group_info_entries_created_fid,
                               entries_created));

exit:
    if (BCMLT_INVALID_HDL != grp_info_templ) {
        (void) bcmlt_entry_free(grp_info_templ);
    }
    SHR_FUNC_EXIT();
}

int
bcmint_field_group_stage_based_id_get(int unit,
                                      bcmi_ltsw_field_stage_t stage_id,
                                      bcm_field_group_t group_id,
                                      bcm_field_group_t *stage_base_group_id)
{
    SHR_FUNC_ENTER(unit);

    switch (stage_id) {
        case bcmiFieldStageIngress:
            *stage_base_group_id = group_id | BCM_FIELD_IFP_ID_BASE;
            break;
        case bcmiFieldStageEgress:
            *stage_base_group_id = group_id | BCM_FIELD_EFP_ID_BASE;
            break;
        case bcmiFieldStageVlan:
            *stage_base_group_id = group_id | BCM_FIELD_VFP_ID_BASE;
            break;
        case bcmiFieldStageExactMatch:
            *stage_base_group_id = group_id | BCM_FIELD_EM_ID_BASE;
            break;
        default:
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
    }

exit:
    SHR_FUNC_EXIT();
}
