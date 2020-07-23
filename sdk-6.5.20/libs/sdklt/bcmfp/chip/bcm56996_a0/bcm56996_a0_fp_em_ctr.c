/*! \file bcm56996_a0_fp_em_ctr.c
 *
 * APIs to update counter related info
 * in entry policy words for
 * TH4G device(56996_A0).
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bcmfp/bcmfp_internal.h>
#include <bcmfp/bcmfp_oper_internal.h>
#include <bcmdrd/bcmdrd_field.h>
#include <bcmfp/bcmfp_pdd.h>
#include <bcmfp/chip/bcm56996_a0_fp.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmfp/bcmfp_action_internal.h>
#include <bcmfp/bcmfp_cth_ctr.h>

#define BSL_LOG_MODULE BSL_LS_BCMFP_DEV

int
bcmfp_bcm56996_a0_em_counter_actions_set(int unit,
                                   bcmfp_stage_id_t stage_id,
                                   bcmfp_group_id_t group_id,
                                   bcmfp_pdd_id_t pdd_id,
                                   bcmfp_group_oper_info_t *opinfo,
                                   bcmfp_ctr_params_t *ctr_params,
                                   uint32_t **edw)
{
    int rv = SHR_E_NONE;
    uint32_t ctr_set = 0;
    uint32_t value = 0;
    uint32_t pdd_ctr_fid = 0;
    bcmfp_action_t action = BCMFP_ACTION_NONE;
    bcmfp_action_oper_info_t a_offset;
    bcmfp_stage_t *stage = NULL;
    const bcmltd_op_arg_t *op_arg = NULL;
    bcmfp_group_config_t *group_config = NULL;
    bcmfp_pdd_oper_type_t pdd_type = BCMFP_PDD_OPER_TYPE_NONE;
    bcmfp_pdd_id_t default_pdd_id = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(opinfo, SHR_E_PARAM);
    SHR_NULL_CHECK(ctr_params, SHR_E_PARAM);
    SHR_NULL_CHECK(edw, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));
    pdd_ctr_fid = FP_EM_PDD_TEMPLATEt_ACTION_CTR_ENABLEf;
    action = stage->tbls.pdd_tbl->actions_fid_info[pdd_ctr_fid].action;

    /* Get the group configuration. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_group_remote_config_get(unit,
                                       op_arg,
                                       stage_id,
                                       group_id,
                                       &group_config));
    default_pdd_id = group_config->default_pdd_id;

    if (pdd_id == default_pdd_id) {
        pdd_type = BCMFP_PDD_OPER_TYPE_DEFAULT_POLICY;
    } else {
        pdd_type = opinfo->pdd_type;
    }

    /*
     * Retrieve the action offsets for the particular
     * entry part.
     */
    if (pdd_id != 0) {
        rv = bcmfp_pdd_action_offset_info_get(unit,
                                              stage_id,
                                              pdd_id,
                                              0,
                                              action,
                                              pdd_type,
                                              &a_offset);
        SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_NOT_FOUND);
        if (rv == SHR_E_NOT_FOUND) {
            /* Counter set is not enabled in PDD*/
            SHR_EXIT();
        }
    } else {
        SHR_EXIT();
    }

    /* FLEX_CTR_ACTION */
    value = ctr_params->flex_ctr_action;
    bcmdrd_field_set(&ctr_set, 0, 4, &(value));

    /* FLEX_CTR_OBJECT */
    value = ctr_params->flex_ctr_object;
    bcmdrd_field_set(&ctr_set, 5, 18, &(value));

    /* FLEX_CTR_CONTAINER_ID */
    value = ctr_params->flex_ctr_container_id;
    bcmdrd_field_set(&ctr_set, 19, 22, &(value));

    /* G_COUNT */
    value = ctr_params->flex_ctr_g_count;
    bcmdrd_field_set(&ctr_set, 23, 24, &(value));

    /* R_COUNT */
    value = ctr_params->flex_ctr_r_count;
    bcmdrd_field_set(&ctr_set, 25, 26, &(value));

    /* Y_COUNT */
    value = ctr_params->flex_ctr_y_count;
    bcmdrd_field_set(&ctr_set, 27, 28, &(value));

    /* Set counter set in edw words */
    bcmdrd_field_set(edw[0],
                     a_offset.offset[0],
                     a_offset.offset[0]+ a_offset.width[0] - 1,
                     &ctr_set);
exit:
    if (group_config != NULL) {
        bcmfp_group_config_free(unit, group_config);
    }
    SHR_FUNC_EXIT();
}
