/*! \file bcm56880_a0_fp_common_custom_buffers.c
 *
 * APIs to intsall/uninstall to configurations to h/w
 * memories/registers to create/destroy the IFP/EFP/VFP
 * PDD for Trident4 device(56880_A0).
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
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmdrd/bcmdrd_field.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmdrd/chip/bcm56880_a0_enum.h>
#include <bcmfp/bcmfp_oper_internal.h>
#include <bcmfp/bcmfp_ha_internal.h>
#include <bcmfp/bcmfp_oper_internal.h>
#include <bcmfp/bcmfp_pdd_internal.h>
#include <bcmfp/bcmfp_cth_pdd.h>

#define BSL_LOG_MODULE BSL_LS_BCMFP_DEV

int
bcm56880_a0_fp_common_buffers_custom_set(int unit,
                               uint32_t trans_id,
                               uint32_t flags,
                               bcmfp_stage_t *stage,
                               bcmfp_group_oper_info_t *opinfo,
                               bcmfp_pdd_config_t *pdd_config,
                               bcmfp_key_ids_t *key_ids,
                               bcmfp_buffers_t *buffers)
{
    bool is_valid_pdd_oper = FALSE;
    bcmfp_action_data_t *pdd_data = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(opinfo, SHR_E_PARAM);
    SHR_NULL_CHECK(key_ids, SHR_E_PARAM);
    SHR_NULL_CHECK(buffers, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
       (bcmfp_pdd_oper_status_get(unit,
                              stage->stage_id,
                              key_ids->pdd_id,
                              opinfo->pdd_type,
                              &is_valid_pdd_oper));

    if (is_valid_pdd_oper == FALSE) {
        SHR_EXIT();
    }

    pdd_data = pdd_config->action_data;

    while (pdd_data != NULL) {

        if (((stage->action_cfg_db->action_cfg[pdd_data->action]->flags)
             & BCMFP_ACTION_IN_AUX_CONTAINER)) {

            if (pdd_data->action == stage->tbls.pdd_tbl->aux_drop_code_fid) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmfp_pdd_action_set(unit,
                                          stage->stage_id,
                                          key_ids->pdd_id,
                                          key_ids->group_id,
                                          opinfo->pdd_type,
                                          opinfo,
                                          pdd_data->action,
                                          1,
                                          buffers->edw));
            } else if (pdd_data->action == stage->tbls.pdd_tbl->aux_drop_priority_fid) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmfp_pdd_action_set(unit,
                                          stage->stage_id,
                                          key_ids->pdd_id,
                                          key_ids->group_id,
                                          opinfo->pdd_type,
                                          opinfo,
                                          pdd_data->action,
                                          1,
                                          buffers->edw));
            } else if (pdd_data->action == stage->tbls.pdd_tbl->aux_trace_id_fid) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmfp_pdd_action_set(unit,
                                          stage->stage_id,
                                          key_ids->pdd_id,
                                          key_ids->group_id,
                                          opinfo->pdd_type,
                                          opinfo,
                                          pdd_data->action,
                                          1,
                                          buffers->edw));
            } else {
                /* Do nothing */
            }
        }
        pdd_data = pdd_data->next;
    }


exit:
   SHR_FUNC_EXIT();
}
