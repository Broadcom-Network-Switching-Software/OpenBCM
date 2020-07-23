/*! \file bcmcth_meter_sbr_internal.c
 *
 * This file defines HW interface handler functions for
 * FP Meter actions.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <shr/shr_error.h>
#include <shr/shr_bitop.h>
#include <bsl/bsl.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmlrd/bcmlrd_map.h>
#include <bcmcfg/bcmcfg_lt.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmdrd/bcmdrd_field.h>
#include <bcmbd/bcmbd.h>
#include <bcmptm/bcmptm_types.h>
#include <bcmptm/bcmptm.h>
#include <bcmevm/bcmevm_api.h>
#include <bcmcth/bcmcth_meter_util.h>
#include <bcmcth/bcmcth_meter_fp_ing_action.h>
#include <bcmcth/bcmcth_meter_fp_ing_imm.h>
#include <bcmcth/bcmcth_meter_fp_egr_action.h>
#include <bcmcth/bcmcth_meter_fp_egr_imm.h>
#include <bcmcth/bcmcth_meter_action_internal.h>

#include <bcmcth/bcmcth_meter_sbr_internal.h>

#define BSL_LOG_MODULE  BSL_LS_BCMCTH_METER

/*******************************************************************************
 * Private functions
 */
/*
 * Read an entry from the H/W table.
 */
static int
meter_sbr_hw_entry_read(int unit,
                        int stage,
                        uint32_t trans_id,
                        int pipe,
                        uint32_t pt_id,
                        uint32_t hw_idx,
                        uint8_t *entry_buf)
{
    uint32_t            rsp_flags;
    uint32_t            lt_id = 0;
    bcmltd_sid_t        rsp_ltid;
    bcmdrd_sym_info_t   sinfo;
    bcmbd_pt_dyn_info_t pt_dyn_info;
    bcmcth_meter_action_drv_t *drv = NULL;

    SHR_FUNC_ENTER(unit);

    pt_dyn_info.index = hw_idx;
    pt_dyn_info.tbl_inst = pipe;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmdrd_pt_info_get(unit, pt_id, &sinfo));

    if (stage == METER_FP_STAGE_ID_INGRESS) {
        bcmcth_meter_fp_ing_action_drv_get(unit, &drv);
    } else {
        bcmcth_meter_fp_egr_action_drv_get(unit, &drv);
    }
    if (drv == NULL || drv->sbr_lt_info == NULL) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    lt_id = drv->sbr_lt_info->ltid;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_ltm_mreq_indexed_lt(unit,
                                    0,
                                    pt_id,
                                    &pt_dyn_info,
                                    NULL, NULL,
                                    BCMPTM_OP_READ,
                                    NULL,
                                    sinfo.entry_wsize,
                                    lt_id,
                                    trans_id,
                                    NULL, NULL,
                                    (uint32_t *)entry_buf,
                                    &rsp_ltid,
                                    &rsp_flags));

exit:
    SHR_FUNC_EXIT();
}

/*
 * Write an entry into the H/W table.
 */
static int
meter_sbr_hw_entry_write(int unit,
                         int stage,
                         uint32_t trans_id,
                         int pipe,
                         uint32_t pt_id,
                         uint32_t hw_idx,
                         uint8_t *entry_buf)
{
    uint32_t            rsp_flags, lt_id = 0;
    bcmltd_sid_t        rsp_ltid;
    bcmdrd_sym_info_t   sinfo;
    bcmbd_pt_dyn_info_t pt_dyn_info;
    bcmcth_meter_action_drv_t *drv = NULL;

    SHR_FUNC_ENTER(unit);

    pt_dyn_info.index = hw_idx;
    pt_dyn_info.tbl_inst = pipe;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmdrd_pt_info_get(unit, pt_id, &sinfo));

    if (stage == METER_FP_STAGE_ID_INGRESS) {
        bcmcth_meter_fp_ing_action_drv_get(unit, &drv);
    } else {
        bcmcth_meter_fp_egr_action_drv_get(unit, &drv);
    }
    if (drv == NULL || drv->sbr_lt_info == NULL) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    lt_id = drv->sbr_lt_info->ltid;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmptm_ltm_mreq_indexed_lt(unit,
                                    0,
                                    pt_id,
                                    &pt_dyn_info,
                                    NULL, NULL,
                                    BCMPTM_OP_WRITE,
                                    (uint32_t *)entry_buf,
                                    0,
                                    lt_id,
                                    trans_id,
                                    NULL, NULL,
                                    NULL,
                                    &rsp_ltid,
                                    &rsp_flags));
exit:
    SHR_FUNC_EXIT();
}

/*
 * Convert a SBR LT entry into corresponding HW entry.
 */
static int
meter_sbr_hw_entry_prepare(int unit,
                           int stage,
                           uint32_t pt_id,
                           const bcmltd_field_t *data,
                           uint8_t *buf)
{
    SHR_FUNC_ENTER(unit);

    if (stage == METER_FP_STAGE_ID_INGRESS) {
        SHR_IF_ERR_VERBOSE_EXIT(
            bcmcth_meter_fp_ing_sbr_hw_entry_prepare(unit, pt_id, data, buf));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT(
            bcmcth_meter_fp_egr_sbr_hw_entry_prepare(unit, pt_id, data, buf));
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * Retrieve value corresponding to a field from
 * the input field list.
 */
static int
meter_field_value_get(int unit,
                      const bcmltd_field_t *list,
                      uint32_t fid,
                      uint32_t *data)
{
    bool  found = false;
    const bcmltd_field_t *field;

    SHR_FUNC_ENTER(unit);

    field = list;
    while (field) {
        if (field->id == fid) {
            *data = field->data;
            found = true;
            break;
        }
        field = field->next;
    }

    if (found == false) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public functions
 */
/*!
 * \brief FP meter SBR insert
 *
 * Insert an FP Meter SBR in the hardware SBR table.
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  stage         FP stage.
 * \param [in]  trans_id      LT transaction ID.
 * \param [in]  key           Input key fields.
 * \param [in]  data          Input data fields.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcmcth_meter_sbr_entry_insert(int unit,
                              int stage,
                              uint32_t trans_id,
                              const bcmltd_field_t *key,
                              const bcmltd_field_t *data)
{
    uint32_t    alloc_sz = 0;
    uint32_t    key_fid = 0, sbr_id = 0;
    uint32_t    pt_id = 0;
    uint32_t    pipe_val = 0, pipe_fid = 0;
    uint8_t     *entry_buf = NULL;
    uint8_t     oper_mode = 0;
    int         pipe = -1;
    bcmcth_meter_action_drv_t *drv = NULL;

    SHR_FUNC_ENTER(unit);

    if (stage == METER_FP_STAGE_ID_INGRESS) {
        bcmcth_meter_fp_ing_action_drv_get(unit, &drv);
        bcmcth_meter_fp_ing_oper_mode_get(unit, &oper_mode);
    } else {
        bcmcth_meter_fp_egr_action_drv_get(unit, &drv);
        bcmcth_meter_fp_egr_oper_mode_get(unit, &oper_mode);
    }
    if (drv == NULL || drv->sbr_lt_info == NULL) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    key_fid = drv->sbr_lt_info->key_fid;
    pipe_fid = drv->sbr_lt_info->pipe_fid;

    /* Parse the input to retrieve key. */
    SHR_IF_ERR_VERBOSE_EXIT
        (meter_field_value_get(unit,
                               key,
                               key_fid,
                               &sbr_id));

    if ((oper_mode == METER_FP_OPER_MODE_PIPE_UNIQUE) &&
        (pipe_fid != 0)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (meter_field_value_get(unit,
                                   key,
                                   pipe_fid,
                                   &pipe_val));
        pipe = pipe_val;
    } else {
        pipe_val = 0;
        pipe = -1;
    }

    pt_id = drv->sbr_pt_info->pt_id;

    /* Allocate buffer to read/write to HW action table. */
    alloc_sz = BCMDRD_WORDS2BYTES(bcmdrd_pt_entry_wsize(unit, pt_id));
    SHR_ALLOC(entry_buf, alloc_sz, "bcmcthMeterActionSbrEntry");
    SHR_NULL_CHECK(entry_buf, SHR_E_MEMORY);
    sal_memset(entry_buf, 0, alloc_sz);

    /* Read SBR profile entry and retrieve container bitmap. */
    SHR_IF_ERR_VERBOSE_EXIT(
        meter_sbr_hw_entry_read(unit,
                                stage,
                                trans_id,
                                pipe,
                                pt_id,
                                sbr_id,
                                entry_buf));

    /*
     * Prepare HW entry corresponding to the LT entry configuration.
     * Also update the container bitmap as per the actions enabled.
     */
    SHR_IF_ERR_VERBOSE_EXIT(
        meter_sbr_hw_entry_prepare(unit,
                                   stage,
                                   pt_id,
                                   data,
                                   entry_buf));

    /*
     * Write back the entry into HW.
     */
    SHR_IF_ERR_VERBOSE_EXIT(
        meter_sbr_hw_entry_write(unit,
                                 stage,
                                 trans_id,
                                 pipe,
                                 pt_id,
                                 sbr_id,
                                 entry_buf));

exit:
    SHR_FREE(entry_buf);
    SHR_FUNC_EXIT();
}

/*!
 * \brief FP meter SBR delete
 *
 * Delete an FP Meter SBR from the hardware SBR table.
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  stage         FP stage.
 * \param [in]  trans_id      LT transaction ID.
 * \param [in]  key           Input key fields.
 * \param [in]  data          Input data fields.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcmcth_meter_sbr_entry_delete(int unit,
                              int stage,
                              uint32_t trans_id,
                              const bcmltd_field_t *key,
                              const bcmltd_field_t *data)
{
    uint32_t    alloc_sz = 0;
    uint32_t    key_fid = 0, sbr_id = 0;
    uint32_t    pt_id = 0;
    uint32_t    pipe_val = 0, pipe_fid = 0;
    int         pipe = -1;
    uint8_t     *entry_buf = NULL;
    uint8_t     oper_mode = 0;
    bcmcth_meter_action_drv_t *drv = NULL;

    SHR_FUNC_ENTER(unit);

    if (stage == METER_FP_STAGE_ID_INGRESS) {
        bcmcth_meter_fp_ing_action_drv_get(unit, &drv);
        bcmcth_meter_fp_ing_oper_mode_get(unit, &oper_mode);
    } else {
        bcmcth_meter_fp_egr_action_drv_get(unit, &drv);
        bcmcth_meter_fp_egr_oper_mode_get(unit, &oper_mode);
    }
    if (drv == NULL || drv->sbr_lt_info == NULL) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    key_fid = drv->sbr_lt_info->key_fid;
    pipe_fid = drv->sbr_lt_info->pipe_fid;

    /* Parse the input to retrieve key values. */
    SHR_IF_ERR_VERBOSE_EXIT
        (meter_field_value_get(unit,
                               key,
                               key_fid,
                               &sbr_id));

    if ((oper_mode == METER_FP_OPER_MODE_PIPE_UNIQUE) &&
        (pipe_fid != 0)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (meter_field_value_get(unit,
                                   key,
                                   pipe_fid,
                                   &pipe_val));
        pipe = pipe_val;
    } else {
        pipe_val = 0;
        pipe = -1;
    }

    pt_id = drv->sbr_pt_info->pt_id;

    /* Allocate buffer to read/write to HW action table. */
    alloc_sz = BCMDRD_WORDS2BYTES(bcmdrd_pt_entry_wsize(unit, pt_id));
    SHR_ALLOC(entry_buf, alloc_sz, "bcmcthMeterActionSbrEntry");
    SHR_NULL_CHECK(entry_buf, SHR_E_MEMORY);
    sal_memset(entry_buf, 0, alloc_sz);

    /* Delete SBR entry by writing 0s. */
    SHR_IF_ERR_VERBOSE_EXIT(
        meter_sbr_hw_entry_write(unit,
                                 stage,
                                 trans_id,
                                 pipe,
                                 pt_id,
                                 sbr_id,
                                 entry_buf));

exit:
    SHR_FREE(entry_buf);
    SHR_FUNC_EXIT();
}
