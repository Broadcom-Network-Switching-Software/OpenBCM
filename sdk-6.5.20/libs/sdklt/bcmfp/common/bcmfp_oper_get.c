/*! \file bcmfp_oper_get.c
 *
 * BCMFP operational information get APIs.
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
#include <bcmcfg/bcmcfg_lt.h>
#include <bcmlrd/bcmlrd_map.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmfp/bcmfp_internal.h>
#include <bcmfp/bcmfp_ha_internal.h>
#include <bcmfp/bcmfp_types_internal.h>
#include <bcmfp/generated/bcmfp_ha.h>

#define BSL_LOG_MODULE BSL_LS_BCMFP_DEV

/*
 * \brief Get qualifier offset(s) and width(s) belongs
 *  to a partucular part. Caller will provide chain of HA
 *  elements(blk_id and blk_offset corresponds to first HA
 *  element in the chain) holding qualifiers offset(s) and
 *  width(s) information in all parts.
 *
 * \param [in] unit Logical device id
 * \param [in] blk_id HA block id of first HA element in the chain.
 * \param [in] blk_offset HA block offset of first HA element in the chain.
 * \param [in] part_id First/Second/Third Slice.
 * \param [out] q_offset Qualifer offset(s) and width(s) in part_id.
 *
 * \retval SHR_E_NONE Success, otherwise failure
 */
static int
bcmfp_qual_offset_info_get(int unit,
                           uint8_t blk_id,
                           uint32_t blk_offset,
                           bcmfp_part_id_t part_id,
                           bcmfp_qual_offset_info_t *q_offset)
{
    bool hit_status = FALSE;
    uint16_t offset_id = 0;
    uint16_t max_offset_id = 0;
    bcmfp_part_id_t part = 0;
    bcmfp_generic_data_t *ha_element = NULL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(q_offset, SHR_E_PARAM);

    sal_memset(q_offset, 0, sizeof(bcmfp_qual_offset_info_t));
    /* Loop through the chain. */
    while (blk_id != BCMFP_HA_BLK_ID_INVALID &&
           blk_offset != BCMFP_HA_BLK_OFFSET_INVALID) {
        ha_element = NULL;
        SHR_IF_ERR_EXIT(
            bcmfp_ha_element_get(unit, blk_id, blk_offset, &ha_element));

        /* HA element with type "QUAL_OFFSET_WIDTH_PART_ID" has
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
            q_offset->offset[offset_id] =
                 (ha_element->value >> 2) & 0x3FF;
            q_offset->width[offset_id] =
                 (ha_element->value >> 12) & 0x3FF;
            q_offset->num_offsets = max_offset_id + 1;
            if (q_offset->width[offset_id] != 0) {
                LOG_VERBOSE(BSL_LOG_MODULE,(BSL_META_U(unit,
                        "offset_id=%d, offset=%d, width=%u\n"),
                        offset_id,
                        q_offset->offset[offset_id],
                        q_offset->width[offset_id]));
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
bcmfp_stage_oper_info_get(int unit,
                    bcmfp_stage_id_t stage_id,
                    bcmfp_stage_oper_info_t **stage_oper_info)
{
    void *ha_mem = NULL;
    uint8_t blk_id = BCMFP_HA_BLK_ID_INVALID;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(stage_oper_info, SHR_E_PARAM);
    BCMFP_HA_STAGE_ID_TO_BLK_ID(stage_id, blk_id);
    SHR_IF_ERR_VERBOSE_EXIT(bcmfp_ha_mem_get(unit, blk_id, &ha_mem));
    *stage_oper_info = (bcmfp_stage_oper_info_t *)
                       ((uint8_t *)ha_mem + sizeof(bcmfp_ha_blk_hdr_t));
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_stage_oper_mode_get(int unit,
                          bcmfp_stage_id_t stage_id,
                          bcmfp_stage_oper_mode_t *oper_mode)
{
    int rv;
    uint64_t value = 0;
    bcmltd_fid_t field;
    const bcmlrd_map_t *map = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(oper_mode, SHR_E_PARAM);

    rv = bcmlrd_map_get(unit, FP_CONFIGt, &map);
    if (SHR_FAILURE(rv)) {
        *oper_mode = BCMFP_STAGE_OPER_MODE_GLOBAL;
        SHR_EXIT();
    }

    if (stage_id == BCMFP_STAGE_ID_INGRESS) {
        field = FP_CONFIGt_FP_ING_OPERMODEf;
    } else if (stage_id == BCMFP_STAGE_ID_EXACT_MATCH ||
               stage_id == BCMFP_STAGE_ID_FLOW_TRACKER) {
        field = FP_CONFIGt_FP_EM_OPERMODE_PIPEUNIQUEf;
    } else if (stage_id == BCMFP_STAGE_ID_EGRESS) {
        field = FP_CONFIGt_FP_EGR_OPERMODE_PIPEUNIQUEf;
    } else if (stage_id == BCMFP_STAGE_ID_LOOKUP) {
        field = FP_CONFIGt_FP_VLAN_OPERMODE_PIPEUNIQUEf;
    } else {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    SHR_IF_ERR_EXIT
        (bcmcfg_field_get(unit, FP_CONFIGt, field, &value));
    *oper_mode = value;

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_group_oper_info_check_get(int unit,
                          bcmfp_stage_id_t stage_id,
                          bcmfp_group_id_t group_id,
                          bcmfp_group_oper_info_t **oinfo)
{
    void *ha_mem = NULL;
    uint16_t idx = 0;
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
    if (group_id == 0) {
        *oinfo = group_oper_info;
        SHR_EXIT();
    }
    for (idx = 0; idx < stage_oper_info->num_groups; idx++)  {
        if (group_oper_info[idx].group_id == 0)  {
            continue;
        }
        if (group_oper_info[idx].group_id == group_id) {
            *oinfo = &(group_oper_info[idx]);
        }
    }
    if (*oinfo == NULL) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_group_oper_info_get(int unit,
                          bcmfp_stage_id_t stage_id,
                          bcmfp_group_id_t group_id,
                          bcmfp_group_oper_info_t **oinfo)
{
    void *ha_mem = NULL;
    uint16_t idx = 0;
    uint8_t blk_id = BCMFP_HA_BLK_ID_INVALID;
    uint32_t offset = 0;
    bcmfp_stage_oper_info_t *stage_oper_info = NULL;
    bcmfp_group_oper_info_t *group_oper_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(oinfo, SHR_E_PARAM);

    *oinfo = NULL;
    BCMFP_HA_STAGE_ID_TO_BLK_ID(stage_id, blk_id);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_ha_mem_get(unit, blk_id, &ha_mem));
    stage_oper_info = (bcmfp_stage_oper_info_t *)
                      ((uint8_t *)ha_mem +
                      sizeof(bcmfp_ha_blk_hdr_t));
    offset = stage_oper_info->group_oper_info_seg;
    group_oper_info = (bcmfp_group_oper_info_t *)
                      ((uint8_t *)ha_mem + offset);
    if (group_id == 0) {
        *oinfo = NULL;
        SHR_EXIT();
    } else if (group_id == BCMFP_GROUP_ID_ALL) {
        *oinfo = group_oper_info;
        SHR_EXIT();
    }
    for (idx = 0; idx < stage_oper_info->num_groups; idx++)  {
        if (group_oper_info[idx].group_id == 0)  {
            continue;
        }
        if (group_oper_info[idx].group_id == group_id) {
            *oinfo = &(group_oper_info[idx]);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_keygen_prof_ref_count_get(int unit,
                            bcmfp_stage_id_t stage_id,
                            int pipe_id,
                            bcmfp_ref_count_t **keygen_prof_ref_count)
{
    void *ha_mem = NULL;
    uint8_t blk_id = BCMFP_HA_BLK_ID_INVALID;
    bcmfp_stage_oper_mode_t oper_mode = BCMFP_STAGE_OPER_MODE_GLOBAL;
    bcmfp_stage_oper_info_t *stage_oper_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(keygen_prof_ref_count, SHR_E_PARAM);

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
         *keygen_prof_ref_count = (bcmfp_ref_count_t *)((uint8_t *)ha_mem +
                       stage_oper_info->keygen_prof_ref_count_seg +
                       (sizeof(bcmfp_ref_count_t) *
                        stage_oper_info->num_keygen_prof *
                        pipe_id));
    } else {
         *keygen_prof_ref_count = (bcmfp_ref_count_t *)((uint8_t *)ha_mem +
                                  stage_oper_info->keygen_prof_ref_count_seg);
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_action_prof_ref_count_get(int unit,
                            bcmfp_stage_id_t stage_id,
                            int pipe_id,
                            bcmfp_ref_count_t **action_prof_ref_count)
{
    void *ha_mem = NULL;
    uint8_t blk_id = BCMFP_HA_BLK_ID_INVALID;
    bcmfp_stage_oper_mode_t oper_mode = BCMFP_STAGE_OPER_MODE_GLOBAL;
    bcmfp_stage_oper_info_t *stage_oper_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(action_prof_ref_count, SHR_E_PARAM);

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
         *action_prof_ref_count = (bcmfp_ref_count_t *)((uint8_t *)ha_mem +
                       stage_oper_info->action_prof_ref_count_seg +
                       (sizeof(bcmfp_ref_count_t) *
                        stage_oper_info->num_action_prof *
                        pipe_id));
    } else {
         *action_prof_ref_count = (bcmfp_ref_count_t *)((uint8_t *)ha_mem +
                                  stage_oper_info->action_prof_ref_count_seg);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_qos_prof_ref_count_get(int unit,
                             bcmfp_stage_id_t stage_id,
                             int pipe_id,
                             bcmfp_ref_count_t **qos_prof_ref_count)
{
    void *ha_mem = NULL;
    uint8_t blk_id = BCMFP_HA_BLK_ID_INVALID;
    bcmfp_stage_oper_mode_t oper_mode = BCMFP_STAGE_OPER_MODE_GLOBAL;
    bcmfp_stage_oper_info_t *stage_oper_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(qos_prof_ref_count, SHR_E_PARAM);

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
         *qos_prof_ref_count = (bcmfp_ref_count_t *)((uint8_t *)ha_mem +
                       stage_oper_info->qos_prof_ref_count_seg +
                       (sizeof(bcmfp_ref_count_t) *
                        stage_oper_info->num_qos_prof *
                        pipe_id));
    } else {
         *qos_prof_ref_count = (bcmfp_ref_count_t *)((uint8_t *)ha_mem +
                               stage_oper_info->qos_prof_ref_count_seg);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_em_key_attrib_prof_ref_count_get(int unit,
                             bcmfp_stage_id_t stage_id,
                             int pipe_id,
                             bcmfp_ref_count_t **em_key_attrib_prof_ref_count)
{
    void *ha_mem = NULL;
    uint8_t blk_id = BCMFP_HA_BLK_ID_INVALID;
    bcmfp_stage_oper_mode_t oper_mode = BCMFP_STAGE_OPER_MODE_GLOBAL;
    bcmfp_stage_oper_info_t *stage_oper_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(em_key_attrib_prof_ref_count, SHR_E_PARAM);

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
        *em_key_attrib_prof_ref_count =
            (bcmfp_ref_count_t *)((uint8_t *)ha_mem +
                         stage_oper_info->em_key_attrib_prof_ref_count_seg +
                         (sizeof(bcmfp_ref_count_t) *
                          stage_oper_info->num_em_key_attrib_profile *
                          pipe_id));
    } else {
        *em_key_attrib_prof_ref_count =
            (bcmfp_ref_count_t *)((uint8_t *)ha_mem +
                         stage_oper_info->em_key_attrib_prof_ref_count_seg);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_action_res_id_get(int unit,
                        bcmfp_stage_id_t stage_id,
                        bcmfp_group_id_t group_id,
                        uint16_t *action_res_id)
{
    bcmfp_group_oper_info_t *group_oper_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(action_res_id, SHR_E_PARAM);

    SHR_IF_ERR_EXIT(
        bcmfp_group_oper_info_get(unit, stage_id,
                                  group_id, &group_oper_info));
    *action_res_id = group_oper_info->action_res_id;
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_group_qual_offset_info_get(int unit,
                        bcmfp_stage_id_t stage_id,
                        bcmfp_group_id_t group_id,
                        bcmfp_group_oper_info_t *opinfo,
                        bool is_presel,
                        bcmfp_part_id_t part_id,
                        bcmltd_fid_t fid,
                        uint16_t fidx,
                        bcmfp_qual_offset_info_t *q_offset)
{
    bool hit_status = FALSE;
    uint8_t blk_id = 0;
    uint32_t blk_offset = 0;
    uint8_t qid_blk_id = 0;
    uint32_t qid_blk_offset = 0;
    bcmfp_fid_offset_info_t *fid_offset_info = NULL;
    bcmfp_group_oper_info_t *group_oper_info = NULL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(q_offset, SHR_E_PARAM);

    if (is_presel == FALSE) {
        group_oper_info = opinfo;
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_presel_group_oper_info_get(unit,
                                              stage_id,
                                              group_id,
                                              &group_oper_info));
        if (!(group_oper_info->valid)) {
            SHR_ERR_EXIT(SHR_E_NOT_FOUND);
        }
    }

    blk_id = group_oper_info->qual_offset_info.blk_id;
    blk_offset = group_oper_info->qual_offset_info.blk_offset;
    while (blk_id != BCMFP_HA_BLK_ID_INVALID &&
           blk_offset != BCMFP_HA_BLK_OFFSET_INVALID) {
        fid_offset_info = NULL;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmfp_fid_offset_info_get(unit,
                                       blk_id,
                                       blk_offset,
                                       &fid_offset_info));
        if (fid_offset_info->fid == fid &&
            fid_offset_info->fidx == fidx) {
             hit_status = TRUE;
             qid_blk_id = fid_offset_info->offset_info.blk_id;
             qid_blk_offset = fid_offset_info->offset_info.blk_offset;
             LOG_VERBOSE(BSL_LOG_MODULE,(BSL_META_U(unit,
                         "---Qualifier(%d) offset(s) and"
                         " width(s) in part(%d)---\n"),
                         (fid + fidx), part_id));
             SHR_IF_ERR_VERBOSE_EXIT
                 (bcmfp_qual_offset_info_get(unit,
                                             qid_blk_id,
                                             qid_blk_offset,
                                             part_id,
                                             q_offset));
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
bcmfp_group_flags_get(int unit,
                      bcmfp_stage_id_t stage_id,
                      bcmfp_group_id_t group_id,
                      uint32_t *group_flags)
{
    bcmfp_group_oper_info_t *group_oper_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(group_flags, SHR_E_PARAM);

    SHR_IF_ERR_EXIT(
        bcmfp_group_oper_info_get(unit, stage_id,
                     group_id, &group_oper_info));

    *group_flags = group_oper_info->flags;

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_group_tbl_inst_get(int unit,
                      bcmfp_stage_id_t stage_id,
                      bcmfp_group_id_t group_id,
                      int *tbl_inst)
{
    bcmfp_group_oper_info_t *group_oper_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(tbl_inst, SHR_E_PARAM);

    SHR_IF_ERR_EXIT(
        bcmfp_group_oper_info_get(unit, stage_id,
                     group_id, &group_oper_info));

    *tbl_inst = group_oper_info->tbl_inst;

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_group_ext_info_get(int unit,
                        bcmfp_stage_id_t stage_id,
                        bcmfp_group_id_t group_id,
                        bcmfp_part_id_t part_id,
                        bcmfp_ext_codes_t **ext_codes)
{
    bcmfp_group_oper_info_t *group_oper_info = NULL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(ext_codes, SHR_E_PARAM);
    SHR_IF_ERR_EXIT(
        bcmfp_group_oper_info_get(unit, stage_id,
                     group_id, &group_oper_info));
    *ext_codes = &(group_oper_info->ext_codes[part_id]);
exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_group_compress_types_get(int unit,
                               bcmfp_stage_id_t stage_id,
                               bcmfp_group_id_t group_id,
                               bool **compress_types)
{
    bcmfp_group_oper_info_t *opinfo = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(compress_types, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_group_oper_info_get(unit,
                                   stage_id,
                                   group_id,
                                   &opinfo));

    *compress_types = opinfo->compress_types;

exit:
    SHR_FUNC_EXIT();
}

int
bcmfp_group_compress_enabled(int unit,
                             bcmfp_stage_id_t stage_id,
                             bcmfp_group_id_t group_id,
                             bool *compress_enabled)
{
    bcmfp_group_oper_info_t *opinfo = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(compress_enabled, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_group_oper_info_get(unit,
                                   stage_id,
                                   group_id,
                                   &opinfo));

    *compress_enabled = opinfo->compress;

exit:
    SHR_FUNC_EXIT();
}
