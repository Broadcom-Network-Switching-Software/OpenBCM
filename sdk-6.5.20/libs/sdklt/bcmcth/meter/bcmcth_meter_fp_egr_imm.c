/*! \file bcmcth_meter_fp_egr_imm.c
 *
 * This file contains IFP Meter Action custom handler functions.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <shr/shr_ha.h>
#include <bsl/bsl.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmlrd/bcmlrd_map.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmdrd/bcmdrd_dev.h>
#include <bcmdrd/bcmdrd_field.h>
#include <bcmbd/bcmbd.h>
#include <bcmptm/bcmptm.h>
#include <bcmcfg/bcmcfg_lt.h>
#include <sal/sal_time.h>
#include <bcmmgmt/bcmmgmt_sysm.h>
#include <bcmcth/bcmcth_meter_sysm.h>
#include <bcmcth/bcmcth_meter_util.h>
#include <bcmcth/bcmcth_meter_action_internal.h>
#include <bcmcth/bcmcth_meter_pdd_internal.h>
#include <bcmcth/bcmcth_meter_fp_egr.h>
#include <bcmcth/bcmcth_meter_fp_egr_action.h>

#include <bcmcth/bcmcth_meter_fp_egr_imm.h>

#define BSL_LOG_MODULE  BSL_LS_BCMCTH_METER

/*******************************************************************************
 * Local definitions and macros
 */

static shr_famm_hdl_t egr_pdd_fld_array_hdl[BCMDRD_CONFIG_MAX_UNITS];
static shr_famm_hdl_t egr_template_fld_array_hdl[BCMDRD_CONFIG_MAX_UNITS];

/*******************************************************************************
 * Private functions
 */
static int
meter_fp_egr_template_stage(int unit,
                            bcmltd_sid_t sid,
                            uint32_t trans_id,
                            bcmimm_entry_event_t event_reason,
                            const bcmltd_field_t *key,
                            const bcmltd_field_t *data,
                            void *context,
                            bcmltd_fields_t *output_fields)
{
    SHR_FUNC_ENTER(unit);

    if (output_fields) {
        output_fields->count = 0;
    }

    switch (event_reason) {
    case BCMIMM_ENTRY_INSERT:
        SHR_IF_ERR_VERBOSE_EXIT(
            bcmcth_meter_egr_template_insert(unit,
                                             sid,
                                             trans_id,
                                             key,
                                             data));
        break;
    case BCMIMM_ENTRY_UPDATE:
        SHR_IF_ERR_VERBOSE_EXIT(
            bcmcth_meter_egr_template_update(unit,
                                             sid,
                                             trans_id,
                                             key,
                                             data));
        break;
    case BCMIMM_ENTRY_DELETE:
        SHR_IF_ERR_VERBOSE_EXIT(
            bcmcth_meter_egr_template_delete(unit,
                                             sid,
                                             trans_id,
                                             key));
        break;
    default:
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

static int
meter_fp_egr_action_stage(int unit,
                          bcmltd_sid_t sid,
                          uint32_t trans_id,
                          bcmimm_entry_event_t event_reason,
                          const bcmltd_field_t *key,
                          const bcmltd_field_t *data,
                          void *context,
                          bcmltd_fields_t *output_fields)
{
    SHR_FUNC_ENTER(unit);
    if (output_fields) {
        output_fields->count = 0;
    }

    switch (event_reason) {
    case BCMIMM_ENTRY_INSERT:
        SHR_IF_ERR_VERBOSE_EXIT(
            bcmcth_meter_egr_action_entry_insert(unit,
                                                 sid,
                                                 trans_id,
                                                 key,
                                                 data,
                                                 output_fields));
        break;
    case BCMIMM_ENTRY_UPDATE:
        SHR_IF_ERR_VERBOSE_EXIT(
            bcmcth_meter_egr_action_entry_update(unit,
                                                 sid,
                                                 trans_id,
                                                 key,
                                                 data,
                                                 output_fields));
        break;
    case BCMIMM_ENTRY_DELETE:
        SHR_IF_ERR_VERBOSE_EXIT(
            bcmcth_meter_egr_action_entry_delete(unit,
                                                 sid,
                                                 trans_id,
                                                 key,
                                                 data));
        break;
    default:
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

static int
meter_fp_egr_pdd_stage(int unit,
                       bcmltd_sid_t sid,
                       uint32_t trans_id,
                       bcmimm_entry_event_t event_reason,
                       const bcmltd_field_t *key,
                       const bcmltd_field_t *data,
                       void *context,
                       bcmltd_fields_t *output_fields)
{
    SHR_FUNC_ENTER(unit);

    if (output_fields) {
        output_fields->count = 0;
    }

    switch (event_reason) {
    case BCMIMM_ENTRY_INSERT:
    case BCMIMM_ENTRY_UPDATE:
        SHR_IF_ERR_VERBOSE_EXIT(
            bcmcth_meter_egr_pdd_entry_insert(unit,
                                              sid,
                                              trans_id,
                                              event_reason,
                                              key,
                                              data));
        break;
    case BCMIMM_ENTRY_DELETE:
        SHR_IF_ERR_VERBOSE_EXIT(
            bcmcth_meter_egr_pdd_entry_delete(unit,
                                              sid,
                                              trans_id,
                                              key,
                                              data));
        break;
    default:
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

static int
meter_fp_egr_sbr_stage(int unit,
                       bcmltd_sid_t sid,
                       uint32_t trans_id,
                       bcmimm_entry_event_t event_reason,
                       const bcmltd_field_t *key,
                       const bcmltd_field_t *data,
                       void *context,
                       bcmltd_fields_t *output_fields)
{
    SHR_FUNC_ENTER(unit);

    if (output_fields) {
        output_fields->count = 0;
    }

    switch (event_reason) {
    case BCMIMM_ENTRY_INSERT:
    case BCMIMM_ENTRY_UPDATE:
        SHR_IF_ERR_VERBOSE_EXIT(
            bcmcth_meter_egr_sbr_entry_insert(unit,
                                              sid,
                                              trans_id,
                                              key,
                                              data));
        break;
    case BCMIMM_ENTRY_DELETE:
        SHR_IF_ERR_VERBOSE_EXIT(
            bcmcth_meter_egr_sbr_entry_delete(unit,
                                              sid,
                                              trans_id,
                                              key,
                                              data));
        break;
    default:
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief METER_EGR_FP_TEMPLATE In-memory event callback structure.
 *
 * This structure contains callback functions that will be corresponding
 * to METER_EGR_FP_TEMPLATE logical table entry commit stages.
 */
static bcmimm_lt_cb_t meter_fp_egr_template_imm_callback = {

    /*! Validate function. */
    .validate = NULL,

    /*! Staging function. */
    .stage = meter_fp_egr_template_stage,

    /*! Commit function. */
    .commit = bcmcth_meter_egr_template_commit,

    /*! Abort function. */
    .abort = bcmcth_meter_egr_template_abort,

    /*! Lookup handler. */
    .lookup = bcmcth_meter_egr_template_lookup,
};

/*!
 * \brief METER_EGR_FP_ACTION_TEMPLATE In-memory event callback structure.
 *
 * This structure contains callback functions that will be corresponding
 * to METER_EGR_FP_ACTION_TEMPLATE logical table entry commit stages.
 */
static bcmimm_lt_cb_t meter_fp_egr_action_imm_callback = {

    /*! Validate function. */
    .validate = NULL,

    /*! Staging function. */
    .stage = meter_fp_egr_action_stage,

    /*! Commit function. */
    .commit = NULL,

    /*! Abort function. */
    .abort =NULL,
};

/*!
 * \brief METER_EGR_FP_PDD_TEMPLATE In-memory event callback structure.
 *
 * This structure contains callback functions that will be corresponding
 * to METER_EGR_FP_PDD_TEMPLATE logical table entry commit stages.
 */
static bcmimm_lt_cb_t meter_fp_egr_pdd_imm_callback = {

    /*! Validate function. */
    .validate = NULL,

    /*! Staging function. */
    .stage = meter_fp_egr_pdd_stage,

    /*! Commit function. */
    .commit = bcmcth_meter_fp_egr_action_commit,

    /*! Abort function. */
    .abort = bcmcth_meter_fp_egr_action_abort,
};

/*!
 * \brief METER_EGR_FP_SBR_TEMPLATE In-memory event callback structure.
 *
 * This structure contains callback functions that will be corresponding
 * to METER_EGR_FP_SBR_TEMPLATE logical table entry commit stages.
 */
static bcmimm_lt_cb_t meter_fp_egr_sbr_imm_callback = {

    /*! Validate function. */
    .validate = NULL,

    /*! Staging function. */
    .stage = meter_fp_egr_sbr_stage,

    /*! Commit function. */
    .commit = NULL,

    /*! Abort function. */
    .abort = NULL,
};

int
bcmcth_meter_fp_egr_imm_register(int unit)
{
    int rv = 0;
    uint32_t lt_idx = 0;
    bcmcth_meter_fp_attrib_t *attr = NULL;
    bcmcth_meter_action_drv_t *act_drv = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (shr_famm_hdl_init(64, &egr_pdd_fld_array_hdl[unit]));

    SHR_IF_ERR_EXIT
        (shr_famm_hdl_init(32, &egr_template_fld_array_hdl[unit]));

    rv = bcmcth_meter_fp_egr_action_drv_get(unit, &act_drv);
    if (SHR_SUCCESS(rv) && act_drv != NULL) {
        if (act_drv->action_lt_info != NULL) {
            for (lt_idx = 0; lt_idx < act_drv->num_action_lts; lt_idx++) {
                SHR_IF_ERR_EXIT(
                    bcmimm_lt_event_reg(unit,
                                        act_drv->action_lt_info[lt_idx].ltid,
                                        &meter_fp_egr_action_imm_callback,
                                        NULL));
            }
        }

        if (act_drv->pdd_lt_info != NULL) {
            SHR_IF_ERR_EXIT(bcmimm_lt_event_reg(unit,
                                                act_drv->pdd_lt_info->ltid,
                                                &meter_fp_egr_pdd_imm_callback,
                                                NULL));
        }

        if (act_drv->sbr_lt_info != NULL) {
            SHR_IF_ERR_EXIT(bcmimm_lt_event_reg(unit,
                                                act_drv->sbr_lt_info->ltid,
                                                &meter_fp_egr_sbr_imm_callback,
                                                NULL));
        }
    }

    rv = bcmcth_meter_fp_egr_attrib_get(unit, &attr);
    if (SHR_SUCCESS(rv) && attr != NULL) {
        SHR_IF_ERR_EXIT(bcmimm_lt_event_reg(unit,
                                            attr->ltid,
                                            &meter_fp_egr_template_imm_callback,
                                            NULL));
    }
exit:
    SHR_FUNC_EXIT();
}

void
bcmcth_meter_fp_egr_imm_db_cleanup(int unit)
{
    if (egr_pdd_fld_array_hdl[unit]) {
        shr_famm_hdl_delete(egr_pdd_fld_array_hdl[unit]);
        egr_pdd_fld_array_hdl[unit] = 0;
    }

    if (egr_template_fld_array_hdl[unit]) {
        shr_famm_hdl_delete(egr_template_fld_array_hdl[unit]);
        egr_template_fld_array_hdl[unit] = 0;
    }
}

int
bcmcth_meter_fp_egr_template_fld_arr_hdl_get(int unit,
                                             shr_famm_hdl_t *hdl)
{
    SHR_FUNC_ENTER(unit);

    *hdl = egr_template_fld_array_hdl[unit];

    SHR_FUNC_EXIT();
}

int
bcmcth_meter_fp_egr_pdd_fld_arr_hdl_get(int unit,
                                        shr_famm_hdl_t *hdl)
{
    SHR_FUNC_ENTER(unit);

    *hdl = egr_pdd_fld_array_hdl[unit];

    SHR_FUNC_EXIT();
}
