/*! \file bcm56880_a0_fp_common_action_db.c
 *
 * IFP/EFP/VFP action configuration DB initialization
 * function for Trident4(56880_A0).
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bcmfp/bcmfp_internal.h>
#include <bcmfp/chip/bcm56880_a0_fp.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmlrd/bcmlrd_types.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmfp/bcmfp_strings_internal.h>

#define BSL_LOG_MODULE BSL_LS_BCMFP_DEV

uint16_t bcm56880_a0_ifp_pdd_containers_size[BCMFP_PDD_MAX_CONTAINERS];
uint16_t bcm56880_a0_efp_pdd_containers_size[BCMFP_PDD_MAX_CONTAINERS];
uint16_t bcm56880_a0_vfp_pdd_containers_size[BCMFP_PDD_MAX_CONTAINERS];
uint16_t bcm56880_a0_fp_em_pdd_containers_size[BCMFP_PDD_MAX_CONTAINERS];


STATIC int
bcm56880_a0_fp_aux_actions_init(int unit,
                                bcmfp_stage_t *stage,
                                uint16_t *aux_cont_id)
{
    uint32_t flags = 0;
    BCMFP_ACTION_CFG_DECL;
    uint16_t cont_id = 0;
    bcmltd_fid_t fid = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage, SHR_E_PARAM);

    /* Aux actions init */
    flags = BCMFP_ACTION_IN_AUX_CONTAINER;

    *aux_cont_id = 0;

    if (stage->stage_id == BCMFP_STAGE_ID_INGRESS) {

        cont_id = 182;

    } else if (stage->stage_id == BCMFP_STAGE_ID_EGRESS) {

        cont_id = 196;

    } else if (stage->stage_id == BCMFP_STAGE_ID_LOOKUP) {

        cont_id = 120;

    } else if (stage->stage_id == BCMFP_STAGE_ID_EXACT_MATCH ||
               stage->stage_id == BCMFP_STAGE_ID_FLOW_TRACKER) {

        cont_id = 176;

    } else {

        /* Stages not supporting aux actions */
        SHR_EXIT();
    }

    /* Drop Code */
    fid = stage->tbls.pdd_tbl->aux_drop_code_fid;
    BCMFP_ACTION_CFG_INIT(unit);
    BCMFP_ACTION_CFG_OFFSET_ADD(0, cont_id, 0, 1, 1);
    BCMFP_ACTION_CFG_INSERT(unit, stage, fid, flags);

    /* Drop Priority */
    fid = stage->tbls.pdd_tbl->aux_drop_priority_fid;
    BCMFP_ACTION_CFG_INIT(unit);
    BCMFP_ACTION_CFG_OFFSET_ADD(0, cont_id, 0, 1, 1);
    BCMFP_ACTION_CFG_INSERT(unit, stage, fid, flags);

    /* Trace ID */
    fid = stage->tbls.pdd_tbl->aux_trace_id_fid;
    BCMFP_ACTION_CFG_INIT(unit);
    BCMFP_ACTION_CFG_OFFSET_ADD(0, cont_id, 1, 1, 1);
    BCMFP_ACTION_CFG_INSERT(unit, stage, fid, flags);

    /* FLEX_CTR_VALID */
    fid = stage->tbls.pdd_tbl->aux_flex_ctr_valid_fid;
    BCMFP_ACTION_CFG_INIT(unit);
    BCMFP_ACTION_CFG_OFFSET_ADD(0, cont_id, 2, 6, -1);
    BCMFP_ACTION_CFG_INSERT(unit, stage, fid, flags);

    *aux_cont_id = cont_id;

exit:
    SHR_FUNC_EXIT();

}

int
bcm56880_a0_fp_common_action_db_init(int unit,
                                  bcmfp_stage_t *stage)
{
    uint32_t flags = 0;
    BCMFP_ACTION_CFG_DECL;
    uint16_t action_idx = 0;
    uint16_t cont_idx = 0;
    uint16_t cont_id = 0;
    uint16_t sbr_cont_id = 0;
    uint16_t cont_size = 0;
    uint16_t offset = 0;
    uint16_t width = 0;
    bcmltd_fid_t fid = 0;
    bcmltd_sid_t sid = BCMLTD_SID_INVALID;
    const bcmlrd_table_pcm_info_t *pcm_info = NULL;
    const bcmlrd_field_pdd_info_t *action_pdd_info = NULL;
    const bcmlrd_pdd_info_t *action_cont_pdd_info = NULL;
    uint16_t *pdd_containers_size = NULL;
    uint16_t max_phy_cont_id = 0;
    uint16_t num_valid_actions = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stage, SHR_E_PARAM);

    if (stage->tbls.policy_tbl != NULL ) {
        sid = stage->tbls.policy_tbl->sid;
    } else {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlrd_table_pcm_conf_get(unit, sid, &pcm_info));
    if (!pcm_info->field_count || pcm_info->field_info == NULL) {
        /* For any FP stage to operate policy LT is mandatory. */
        LOG_ERROR(BSL_LOG_MODULE,
                 (BSL_META_U(unit, "Policy LT is missing for %s "
                 "stage\n"), bcmfp_stage_string(stage->stage_id)));
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    BCMFP_ALLOC(stage->action_cfg_db,
                sizeof(bcmfp_action_cfg_db_t),
                "bcmfpActionCfgDb");

    if (stage->stage_id == BCMFP_STAGE_ID_INGRESS) {
        pdd_containers_size = bcm56880_a0_ifp_pdd_containers_size;
    } else if (stage->stage_id == BCMFP_STAGE_ID_EGRESS) {
        pdd_containers_size = bcm56880_a0_efp_pdd_containers_size;
    } else if (stage->stage_id == BCMFP_STAGE_ID_LOOKUP) {
        pdd_containers_size = bcm56880_a0_vfp_pdd_containers_size;
    } else if (stage->stage_id == BCMFP_STAGE_ID_EXACT_MATCH ||
               stage->stage_id == BCMFP_STAGE_ID_FLOW_TRACKER) {
        pdd_containers_size = bcm56880_a0_fp_em_pdd_containers_size;
    } else {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    for (action_idx = 0; action_idx < pcm_info->field_count; action_idx++) {
        action_pdd_info = pcm_info->field_info[action_idx].pdd_info;
        if (action_pdd_info != NULL) {
            BCMFP_ACTION_CFG_INIT(unit);
            for (cont_idx = 0;
                cont_idx < action_pdd_info->count;
                cont_idx++) {
                action_cont_pdd_info =
                    &(action_pdd_info->info[cont_idx]);
                cont_id = action_cont_pdd_info->phy_cont_id;
                sbr_cont_id = action_cont_pdd_info->sbr_phy_cont_id;
                cont_size = action_cont_pdd_info->phy_cont_size;
                offset = action_cont_pdd_info->offset;
                width = action_cont_pdd_info->width;
                if (cont_id >= BCMFP_PDD_MAX_CONTAINERS) {
                    LOG_VERBOSE(BSL_LOG_MODULE,
                               (BSL_META_U(unit,
                               "Number of PDD physical containers are "
                               "more than expected. Bump up the "
                               "BCMFP_PDD_MAX_CONTAINERS value.\n")));
                    SHR_ERR_EXIT(SHR_E_INTERNAL);
                }
                BCMFP_ACTION_CFG_OFFSET_ADD(cont_idx, cont_id, offset, width, -1);
                BCMFP_ACTION_CFG_OFFSET_SBR_CONT_ID_ADD(cont_idx, sbr_cont_id);
                pdd_containers_size[cont_id] = cont_size;
                if (max_phy_cont_id < cont_id) {
                    max_phy_cont_id = cont_id;
                }
            }
            fid = pcm_info->field_info[action_idx].id;
            if (action_pdd_info->sbr_type == BCMLRD_SBR_NONE) {
                flags = BCMFP_ACTION_SBR_NONE;
            }
            else if (action_pdd_info->sbr_type == BCMLRD_SBR_INTERNAL) {
                flags = BCMFP_ACTION_SBR_INTERNAL;
            }
            BCMFP_ACTION_CFG_NAME_ADD(pcm_info->field_info[action_idx].name);
            BCMFP_ACTION_CFG_INSERT(unit, stage, fid, flags);
            LOG_VERBOSE(BSL_LOG_MODULE,
                       (BSL_META_U(unit,
                        "%s action is initialized in %s stage\n"),
                        pcm_info->field_info[action_idx].name,
                        bcmfp_stage_string(stage->stage_id)));
            flags = 0;
            num_valid_actions++;
        } else {
            fid = pcm_info->field_info[action_idx].id;
            BCMFP_ACTION_CFG_INIT(unit);
            BCMFP_ACTION_CFG_NAME_ADD(pcm_info->field_info[action_idx].name);
            flags |= BCMFP_ACTION_NOT_VALID;
            BCMFP_ACTION_CFG_INSERT(unit, stage, fid, flags);
            flags = 0;
            LOG_VERBOSE(BSL_LOG_MODULE,
                       (BSL_META_U(unit,
                        "%s stage doest not support %s action\n"),
                        bcmfp_stage_string(stage->stage_id),
                        pcm_info->field_info[action_idx].name));
        }
    }

    /* Aux actions init */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcm56880_a0_fp_aux_actions_init(unit, stage, &cont_id));

    if (cont_id != 0) {

        pdd_containers_size[cont_id] = 8;

        if (max_phy_cont_id < cont_id) {
            max_phy_cont_id = cont_id;
        }
    }

    if (num_valid_actions == 0) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                   (BSL_META_U(unit,
                    "%s stage doest not suppor any actions\n"),
                    bcmfp_stage_string(stage->stage_id)));
    }
    stage->action_cfg_db->container_size = pdd_containers_size;
    if (max_phy_cont_id != 0) {
       stage->action_cfg_db->num_containers = (max_phy_cont_id + 1);
    }

    SHR_IF_ERR_VERBOSE_EXIT(bcm56880_a0_fp_common_presel_action_db_init
                                (unit, stage));
exit:
    SHR_FUNC_EXIT();
}
