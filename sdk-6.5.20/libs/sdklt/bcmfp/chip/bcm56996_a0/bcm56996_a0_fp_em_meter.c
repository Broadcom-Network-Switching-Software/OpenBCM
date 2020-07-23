/*! \file bcm56996_a0_fp_em_meter.c
 *
 * APIs to update meter related info
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
#include <bcmcth/bcmcth_meter_util.h>
#include <bcmfp/bcmfp_oper_internal.h>
#include <bcmdrd/bcmdrd_field.h>
#include <bcmfp/bcmfp_pdd.h>
#include <bcmfp/chip/bcm56996_a0_fp.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmfp/bcmfp_action_internal.h>

#define BSL_LOG_MODULE BSL_LS_BCMFP_DEV

int
bcmfp_bcm56996_a0_em_meter_actions_set(int unit,
                                bcmfp_group_id_t group_id,
                                bcmfp_pdd_id_t pdd_id,
                                bcmfp_group_oper_info_t *opinfo,
                                bcmcth_meter_fp_policy_fields_t *meter_entry,
                                uint32_t **edw)
{
    int rv = SHR_E_NONE;
    uint32_t meter_set = 0;
    uint32_t value = 0;
    uint32_t pdd_meter_fid = 0;
    bcmfp_action_t action = BCMFP_ACTION_NONE;
    bcmfp_stage_id_t stage_id = BCMFP_STAGE_ID_EXACT_MATCH;
    bcmfp_action_oper_info_t a_offset;
    bcmfp_stage_t *stage = NULL;
    const bcmltd_op_arg_t *op_arg = NULL;
    bcmfp_group_config_t *group_config = NULL;
    bcmfp_pdd_oper_type_t pdd_type = BCMFP_PDD_OPER_TYPE_NONE;
    bcmfp_pdd_id_t default_pdd_id = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(opinfo, SHR_E_PARAM);
    SHR_NULL_CHECK(meter_entry, SHR_E_PARAM);
    SHR_NULL_CHECK(edw, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_stage_get(unit, stage_id, &stage));
    pdd_meter_fid = FP_EM_PDD_TEMPLATEt_ACTION_METER_ENABLEf;
    action = stage->tbls.pdd_tbl->actions_fid_info[pdd_meter_fid].action;

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
            /* Meter set is not enabled in PDD */
            SHR_EXIT();
        }
    } else {
        SHR_EXIT();
    }

    /* METER_PAIR_MODE_MODIFIER */
    value = meter_entry->mode_modifier;
    bcmdrd_field_set(&meter_set, 11, 11, &(value));

    /* METER_PAIR_MODE */
    value = meter_entry->meter_pair_mode;
    bcmdrd_field_set(&meter_set, 8, 10, &value);

    /* METER_PAIR_INDEX */
    value = meter_entry->meter_pair_index;
    bcmdrd_field_set(&meter_set, 0, 7, &value);

    /* METER_UPDATE_EVEN */
    value = meter_entry->meter_update_even;
    bcmdrd_field_set(&meter_set, 14, 14, &value);

    /* METER_TEST_EVEN */
    value = meter_entry->meter_test_even;
    bcmdrd_field_set(&meter_set, 12, 12, &value);

    /* METER_UPDATE_ODD */
    value = meter_entry->meter_update_odd;
    bcmdrd_field_set(&meter_set, 15, 15, &value);

    /* METER_TEST_ODD */
    value = meter_entry->meter_test_odd;
    bcmdrd_field_set(&meter_set, 13, 13, &value);

    /* Set Meter set in edw words */
    bcmdrd_field_set(edw[0],
                     a_offset.offset[0],
                     a_offset.offset[0]+ a_offset.width[0] - 1,
                     &meter_set);
 exit:
    if (group_config != NULL) {
        bcmfp_group_config_free(unit, group_config);
    }
    SHR_FUNC_EXIT();
}
