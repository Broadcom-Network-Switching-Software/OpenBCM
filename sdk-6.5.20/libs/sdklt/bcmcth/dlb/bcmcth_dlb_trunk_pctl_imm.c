/*! \file bcmcth_dlb_trunk_pctl_imm.c
 *
 * DLB_TRUNK_PORT_CONTROL Logical Table imm utilities
 *
 * This file contains DLB_TRUNK_PORT_CONTROL
 * table handler functions.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*******************************************************************************
 * Includes
 */
#include <shr/shr_debug.h>
#include <shr/shr_fmm.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmdrd/bcmdrd_field.h>
#include <bcmbd/bcmbd.h>
#include <bcmlrd/bcmlrd_map.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmimm/bcmimm_int_comp.h>
#include <bcmcth/bcmcth_dlb_util.h>
#include <bcmcth/bcmcth_dlb_drv.h>

/*******************************************************************************
 * Defines
 */
#define BSL_LOG_MODULE BSL_LS_BCMCTH_DLB

/*******************************************************************************
 * Private functions
 */
static void
bcmcth_dlb_trunk_pctl_t_init(bcmcth_dlb_trunk_pctl_t *entry)
{
    sal_memset(entry, 0, sizeof (bcmcth_dlb_trunk_pctl_t));
    return;
}

/*!
 * \brief Delete the LT entry.
 *
 * Parse Delete the
 * DLB_TRUNK_PORT_CONTROL LT entry
 *
 * \param [in]  unit Unit number.
 * \param [in]  op_arg operational argument.
 * \param [in]  sid LTD Symbol ID.
 * \param [in]  key field list.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Faild to delete the LT entry.
 */
static int
dlb_trunk_port_ctrl_delete(int unit,
                    const bcmltd_op_arg_t *op_arg,
                    bcmltd_sid_t sid,
                    const bcmltd_field_t *key)
{
    uint64_t val = 0;
    bcmcth_dlb_trunk_pctl_t entry;

    SHR_FUNC_ENTER(unit);

    bcmcth_dlb_trunk_pctl_t_init(&entry);

    entry.op_arg = op_arg;
    entry.glt_sid = sid;

    /* Read the PORT_ID. */
    SHR_IF_ERR_EXIT(
           bcmcth_dlb_imm_field_get(unit,
                                key,
                                DLB_TRUNK_PORT_CONTROLt_PORT_IDf,
                                &val));
    BCMCTH_DLB_LT_FIELD_SET(
         entry.fbmp, BCMCTH_DLB_TRUNK_PCTL_LT_FIELD_PORT_ID);
    BCMCTH_DLB_LT_FIELD_SET(
         entry.fbmp, BCMCTH_DLB_TRUNK_PCTL_LT_FIELD_OVERRIDE);
    BCMCTH_DLB_LT_FIELD_SET(
         entry.fbmp, BCMCTH_DLB_TRUNK_PCTL_LT_FIELD_FORCE_LINK_STATUS);
    entry.port_id  = (uint32_t) val;

    SHR_IF_ERR_EXIT(
          bcmcth_dlb_trunk_pctl_set(unit, &entry));
   exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Insert or update the LT entry.
 *
 * Parse Insert or update the
 * DLB_TRUNK_PORT_CONTROL LT entry
 *
 * \param [in]  unit Unit number.
 * \param [in]  op_arg Operation argument.
 * \param [in]  sid LTD Symbol ID.
 * \param [in]  key field list.
 * \param [in]  data field list.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Faild to insert or update the LT entry.
 */
static int
dlb_trunk_port_ctrl_update(int unit,
                    const bcmltd_op_arg_t *op_arg,
                    bcmltd_sid_t sid,
                    const bcmltd_field_t *key,
                    const bcmltd_field_t *data)
{
    uint64_t val = 0;
    bcmcth_dlb_trunk_pctl_t entry;
    uint32_t fid = 0;
    uint32_t arr_idx = 0;
    const bcmltd_field_t *in = NULL;


    SHR_FUNC_ENTER(unit);

    bcmcth_dlb_trunk_pctl_t_init(&entry);

    entry.op_arg = op_arg;
    entry.glt_sid = sid;

    /* Read the PORT_ID. */
    SHR_IF_ERR_EXIT(
           bcmcth_dlb_imm_field_get(unit,
                                key,
                                DLB_TRUNK_PORT_CONTROLt_PORT_IDf,
                                &val));
    entry.port_id  = (uint32_t) val;

    for (in = data; (in != NULL); in = in->next) {
        fid = in->id;
        val = in->data;
        arr_idx = in->idx;
        switch (fid) {
            case DLB_TRUNK_PORT_CONTROLt_OVERRIDEf:
                entry.override        = (uint32_t) val;
                BCMCTH_DLB_LT_FIELD_SET(
                entry.fbmp, BCMCTH_DLB_TRUNK_PCTL_LT_FIELD_OVERRIDE);
                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(
                              unit,
                              "OVERRIDEf [i=%d] fval=%d.\n"),
                              (unsigned int)arr_idx,
                              (uint32_t)val));
                break;
             case DLB_TRUNK_PORT_CONTROLt_OVERRIDE_LINK_STATEf:
                entry.port_st       = (uint32_t) val;
                BCMCTH_DLB_LT_FIELD_SET(
                entry.fbmp, BCMCTH_DLB_TRUNK_PCTL_LT_FIELD_FORCE_LINK_STATUS);
                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(
                              unit,
                              "OVERRIDE_LINK_STATEf [i=%d] fval=%d.\n"),
                              (unsigned int)arr_idx,
                              (uint32_t)val));
                break;
            default:
                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(
                                    unit,
                                    "Invalid field ID = %d.\n"),
                                    fid));
                SHR_ERR_EXIT(SHR_E_PARAM);
                break;
        }
    }

    SHR_IF_ERR_EXIT(
          bcmcth_dlb_trunk_pctl_set(unit, &entry));

   exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief DLB_TRUNK_PORT_CONTROL table change
 *  callback function for staging.
 *
 * Handle DLB_TRUNK IMM table change events.
 *
 * \param [in] unit Unit number.
 * \param [in] sid This is the logical table ID.
 * \param [in] Operational argument.
 * \param [in] event_reason Call back reason.
 * \param [in] key This is a linked list of the key fields identifying
 * the entry.
 * \param [in] data This is a linked list of the data fields in
 * modified entry.
 * \param [in] context Is a pointer that was given during registration.
 * The callback can use this pointer to retrieve some context.
 * \param [out] output_fields This is a linked list of fields,
 * can be used by the component to add fields into the entry.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL LM fails to handle LT change events.
 */
static int
dlb_trunk_port_ctrl_stage(int unit,
                          bcmltd_sid_t sid,
                          const bcmltd_op_arg_t *op_arg,
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
            SHR_IF_ERR_EXIT(dlb_trunk_port_ctrl_update(
                                              unit,
                                              op_arg,
                                              sid,
                                              key,
                                              data));
            break;
        case BCMIMM_ENTRY_DELETE:
            SHR_IF_ERR_EXIT(dlb_trunk_port_ctrl_delete(
                                             unit,
                                             op_arg,
                                             sid,
                                             key));

            break;
        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief DLB_TRUNK_PORT_CONTROL
 *  In-memory event callback structure.
 *
 * This structure contains callback functions that will be corresponding
 * to DLB_TRUNK_PORT_CONTROL LT entry commit stages.
 */
static bcmimm_lt_cb_t bcmdlb_trunk_port_ctrl_imm_callback = {
  /*! Valdiation function. */
  .validate = NULL,

  /*! Staging function. */
  .op_stage    = dlb_trunk_port_ctrl_stage,

  /*! Commit function. */
  .commit   = NULL,

  /*! Abort function. */
  .abort    = NULL
};

/**************************************************************************
 * Public functions
 */
/*!
 * \brief DLB_TRUNK_PORT_CONTROL LT imm registration
 *
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failed to register the imm callbacks.
 */
int
bcmcth_dlb_trunk_pctl_imm_init(int unit)
{
    const bcmlrd_map_t *map = NULL;
    int rv;

    SHR_FUNC_ENTER(unit);

    /*
     * To register callback for DLB LTs here.
     */
    rv = bcmlrd_map_get(unit, DLB_TRUNK_PORT_CONTROLt, &map);

    if (SHR_SUCCESS(rv) && map) {
        SHR_IF_ERR_EXIT
        (bcmimm_lt_event_reg(unit,
                             DLB_TRUNK_PORT_CONTROLt,
                             &bcmdlb_trunk_port_ctrl_imm_callback,
                             NULL));
    }

exit:
    SHR_FUNC_EXIT();
}
