/*! \file bcmfp_cth_meter.c
 *
 * APIs for communicating with meter module.
 *
 * This file contains functions to communicate with meter
 * module and update FP policy table with meter parameters.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*******************************************************************************
  Includes
 */
#include <shr/shr_debug.h>
#include <bsl/bsl.h>
#include <shr/shr_util.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmlrd/bcmlrd_client.h>
#include <bcmltd/bcmltd_types.h>
#include <bcmlrd/bcmlrd_types.h>
#include <bcmfp/bcmfp_internal.h>
#include <bcmfp/bcmfp_stage_internal.h>
#include <bcmfp/bcmfp_group_internal.h>
#include <bcmfp/bcmfp_cth_common.h>
#include <bcmfp/bcmfp_tbl_internal.h>
#include <bcmcth/bcmcth_meter_util.h>
#include <bcmimm/bcmimm_basic.h>

#define BSL_LOG_MODULE BSL_LS_BCMFP_DEV

static int
bcmfp_meter_event_process(int unit,
                          const char *event,
                          uint64_t ev_data)
{
    int rv;
    bool not_mapped = FALSE;
    uint32_t stage_id = BCMFP_STAGE_ID_COUNT;
    uint32_t meter_id = 0;
    uint32_t trans_id = 0;
    bcmcth_meter_fp_policy_fields_t meter_entry;
    bcmfp_stage_t *stage = NULL;
    size_t size = 0;
    bcmfp_idp_info_t idp_info;
    void *user_data = NULL;
    bcmfp_entry_id_traverse_cb cb_func;
    bcmcth_meter_fp_event_info_t *meter_info = NULL;
    bcmltd_op_arg_t op_arg;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(event, SHR_E_PARAM);

    meter_info = (bcmcth_meter_fp_event_info_t *)(unsigned long)ev_data;
    meter_id = meter_info->meter_id;
    trans_id = meter_info->trans_id;
    sal_memset(&op_arg, 0, sizeof(bcmltd_op_arg_t));
    op_arg.trans_id = trans_id;

    if ((sal_strcmp(event, "METER_ING_INSERT") == 0) ||
        (sal_strcmp(event, "METER_ING_UPDATE") == 0) ||
        (sal_strcmp(event, "METER_ING_DELETE") == 0)) {
        stage_id = BCMFP_STAGE_ID_INGRESS;
    } else {
        stage_id = BCMFP_STAGE_ID_EGRESS;
    }

    size = sizeof(bcmcth_meter_fp_policy_fields_t);
    sal_memset(&meter_entry, 0, size);
    /*
     * Meter entry lookup should not fail as callback is triggered
     * only when some operation(insert/update/delete) is issues on
     * meter entry.
     */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_meter_entry_lookup(unit,
                                   stage_id,
                                   meter_id,
                                   &meter_entry));

    /* Check if the meter template id is mapped to any entry ids. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_meter_entry_map_check(unit,
                                     stage_id,
                                     meter_id,
                                     &not_mapped));

    if (not_mapped == TRUE) {
        if (stage_id == BCMFP_STAGE_ID_INGRESS) {
            /*
             * IFP_METER is shared between IFP and EM. So check
             * if this meter_id is linked with any EM entry.
             */
            stage_id = BCMFP_STAGE_ID_EXACT_MATCH;
            /* Check if EM stage exists on the device. */
            rv = bcmfp_stage_get(unit, stage_id, &stage);
            if (rv == SHR_E_NOT_FOUND) {
                SHR_EXIT();
            }
            SHR_IF_ERR_EXIT(rv);
            /*
             * Check if the meter template id is mapped to any
             * entry ids.
             */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_meter_entry_map_check(unit,
                                             stage_id,
                                             meter_id,
                                             &not_mapped));
        }
        /*
         * As the notification is received this meter should be
         * referenced by some entries.
         */
        if (not_mapped == TRUE) {
            LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit,
                      "BCMFP:  Could not find any entry mapped"
                      " with meter_id %d \n"), meter_id));
            SHR_EXIT();
        }
    }

    sal_memset(&idp_info, 0, sizeof(bcmfp_idp_info_t));
    idp_info.op_arg = &op_arg;
    idp_info.sid_type = BCMFP_SID_TYPE_METER_TEMPLATE;
    idp_info.key = NULL;
    idp_info.data = NULL;
    idp_info.output_fields = NULL;
    if (sal_strcmp(event, "METER_ING_INSERT") == 0 ||
        sal_strcmp(event, "METER_EGR_INSERT") == 0) {
        idp_info.event_reason = BCMIMM_ENTRY_INSERT;
    } else if (sal_strcmp(event, "METER_ING_DELETE") == 0 ||
               sal_strcmp(event, "METER_EGR_DELETE") == 0 ) {
        idp_info.event_reason = BCMIMM_ENTRY_DELETE;
    } else if (sal_strcmp(event, "METER_ING_UPDATE") == 0 ||
               sal_strcmp(event, "METER_EGR_UPDATE") == 0 ) {
        idp_info.event_reason = BCMIMM_ENTRY_UPDATE;
    } else {
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit,
                 "BCMFP:  Event received is not valid for"
                 "meter_id %d \n"), meter_id));
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    user_data = (void *)(&idp_info);
    cb_func = bcmfp_entry_idp_meter_process;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_meter_entry_map_traverse(unit,
                                        stage_id,
                                        meter_id,
                                        cb_func,
                                        user_data));
exit:
    SHR_FUNC_EXIT();
}

void
bcmfp_meter_event_cb(int unit,
                     const char *event,
                     uint64_t ev_data)
{
     (void)bcmfp_meter_event_process(unit, event, ev_data);
}

