/*! \file bcmlm_imm.c
 *
 * BCMLM interface to in-memory table.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */


#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <shr/shr_bitop.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmlrd/bcmlrd_map.h>
#include <bcmimm/bcmimm_int_comp.h>
#include <bcmpc/bcmpc_lport.h>
#include <bcmlm/bcmlm_drv_internal.h>
#include "bcmlm_internal.h"

/*******************************************************************************
 * Local definitions
 */

/* BSL Module */
#define BSL_LOG_MODULE BSL_LS_BCMLM_IMM

/*! LM fields array lmm handler. */
static shr_famm_hdl_t lm_fld_arr_hdl;

/*! The maximum field count of LM logical tables */
#define LM_FIELD_COUNT_MAX 4

/*! Data structure to save the info of LM_CONTROLt entry */
typedef struct lm_cfg_s {

    /*! Bitmap of fields to be operated on. */
    SHR_BITDCLNAME(fbmp, BCMLM_FIELDS_MAX);

    /*! Start/stop Link manager thread. */
    bool enable;

    /*! Time between scans(us). */
    uint32_t interval;

} lm_cfg_t;

/*! Data structure to save the info of LM_PORT_CONTROLt entry */
typedef struct lm_port_cfg_s {

    /*! Bitmap of Fields to be operated */
    SHR_BITDCLNAME(fbmp, BCMLM_FIELDS_MAX);

    /*! Logical port. */
    shr_port_t port;

    /*! Linkscan mode of logical port. */
    bcmlm_linkscan_mode_t linkscan_mode;

    /*! Link status of those ports with override mode. */
    bool override_link_state;

    /*! Manual sync on port up or down sequence. */
    bool manual_sync;

} lm_port_cfg_t;

/*******************************************************************************
 * Private functions
 */

/*!
 * \brief imm LM_CONTROLt notification input fields parsing.
 *
 * Parse imm LM_CONTROLt input fields.
 *
 * \param [in] unit Unit number.
 * \param [in] key IMM input key field array.
 * \param [in] data IMM input data field array.
 * \param [out] ltcfg Link manager port config data buffer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Faild to convert \c key \c data to \c ltcfg.
 */
static int
lm_cfg_lt_fields_parse(int unit,
                       const bcmltd_field_t *key,
                       const bcmltd_field_t *data,
                       lm_cfg_t *ltcfg)
{
    const bcmltd_field_t *gen_field;
    uint32_t fid;
    uint64_t fval;

    SHR_FUNC_ENTER(unit);

    sal_memset(ltcfg, 0, sizeof(*ltcfg));

    /* Parse data field */
    gen_field = data;
    while (gen_field) {
        fid = gen_field->id;
        fval = gen_field->data;

        switch (fid) {
        case LM_CONTROLt_SCAN_ENABLEf:
            ltcfg->enable = fval;
            SHR_BITSET(ltcfg->fbmp, fid);
            break;
        case LM_CONTROLt_SCAN_INTERVALf:
            ltcfg->interval = fval;
            SHR_BITSET(ltcfg->fbmp, fid);
            break;
        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
        gen_field = gen_field->next;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief LM_CONTROL IMM table change callback function for staging.
 *
 * Handle LM_CONTROL IMM table change events.
 *
 * \param [in] unit Unit number.
 * \param [in] sid This is the logical table ID.
 * \param [in] action This is the desired action for the entry.
 * \param [in] key This is a linked list of the key fields identifying
 * the entry.
 * \param [in] data This is a linked list of the data fields in the
 * modified entry.
 * \param [in] context Is a pointer that was given during registration.
 * The callback can use this pointer to retrieve some context.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL LM fails to handle LT change events.
 */
static int
lm_config_imm_stage_callback(int unit,
                             bcmltd_sid_t sid,
                             uint32_t trans_id,
                             bcmimm_entry_event_t event_reason,
                             const bcmltd_field_t *key,
                             const bcmltd_field_t *data,
                             void *context,
                             bcmltd_fields_t *output_fields)
{
    lm_cfg_t cfg;

    SHR_FUNC_ENTER(unit);

    if (output_fields) {
        output_fields->count = 0;
    }
    SHR_IF_ERR_VERBOSE_EXIT
        (lm_cfg_lt_fields_parse(unit, key, data, &cfg));

    switch (event_reason) {
    case BCMIMM_ENTRY_INSERT:
    case BCMIMM_ENTRY_UPDATE:
        if (SHR_BITGET(cfg.fbmp, LM_CONTROLt_SCAN_INTERVALf)) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlm_ctrl_interval_update(unit, cfg.interval));
        }

        if (SHR_BITGET(cfg.fbmp, LM_CONTROLt_SCAN_ENABLEf)) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlm_ctrl_linkscan_enable(unit, cfg.enable));
        }
        break;
    case BCMIMM_ENTRY_DELETE:
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlm_ctrl_linkscan_enable(unit, 0));
        break;
    default:
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief LM_CONTROL In-memory event callback structure.
 *
 * This structure contains callback functions that will be conresponding
 * to LM_CONTROL logical table entry commit stages.
 */
static bcmimm_lt_cb_t lm_config_imm_callback = {

    /*! Validate function. */
    .validate = NULL,

    /*! Staging function. */
    .stage = lm_config_imm_stage_callback,

    /*! Commit function. */
    .commit = NULL,

    /*! Abort function. */
    .abort = NULL
};

/*!
 * \brief imm LM_PORT_CONTROLt notification input fields parsing.
 *
 * Parse imm LM_PORT_CONTROLt input fields.
 *
 * \param [in] unit Unit number.
 * \param [in] key IMM input key field array.
 * \param [in] data IMM input data field array.
 * \param [out] ltcfg Link manager port config data buffer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Faild to convert \c key \c data to \c ltcfg.
 */
static int
lm_port_cfg_lt_fields_parse(int unit,
                            const bcmltd_field_t *key,
                            const bcmltd_field_t *data,
                            lm_port_cfg_t *ltcfg)
{
    const bcmltd_field_t *gen_field;
    uint32_t fid;
    uint64_t fval;

    SHR_FUNC_ENTER(unit);

    sal_memset(ltcfg, 0, sizeof(*ltcfg));

    /* Parse Key field */
    ltcfg->port = key->data;

    /* Parse data field */
    gen_field = data;
    while (gen_field) {
        fid = gen_field->id;
        fval = gen_field->data;

        switch (fid) {
        case LM_PORT_CONTROLt_LINKSCAN_MODEf:
            ltcfg->linkscan_mode = fval;
            SHR_BITSET(ltcfg->fbmp, fid);
            break;
        case LM_PORT_CONTROLt_OVERRIDE_LINK_STATEf:
            ltcfg->override_link_state = fval;
            SHR_BITSET(ltcfg->fbmp, fid);
            break;
        case LM_PORT_CONTROLt_MANUAL_SYNCf:
            ltcfg->manual_sync = fval;
            SHR_BITSET(ltcfg->fbmp, fid);
            break;
        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
        gen_field = gen_field->next;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief LM_PORT_CONTROL IMM table change callback function for staging.
 *
 * Handle LM_PORT_CONTROL IMM logical table change events.
 *
 * \param [in] unit Unit number.
 * \param [in] sid This is the logical table ID.
 * \param [in] action This is the desired action for the entry.
 * \param [in] key This is a linked list of the key fields identifying
 * the entry.
 * \param [in] data This is a linked list of the data fields in the
 * modified entry.
 * \param [in] context Is a pointer that was given during registration.
 * The callback can use this pointer to retrieve some context.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL LM fails to handle LT change events.
 */
static int
lm_port_config_imm_stage_callback(int unit,
                                  bcmltd_sid_t sid,
                                  uint32_t trans_id,
                                  bcmimm_entry_event_t event_reason,
                                  const bcmltd_field_t *key,
                                  const bcmltd_field_t *data,
                                  void *context,
                                  bcmltd_fields_t *output_fields)
{
    lm_port_cfg_t pcfg;

    SHR_FUNC_ENTER(unit);

    if (output_fields) {
        output_fields->count = 0;
    }
    SHR_IF_ERR_VERBOSE_EXIT
        (lm_port_cfg_lt_fields_parse(unit, key, data, &pcfg));

    switch (event_reason) {
    case BCMIMM_ENTRY_INSERT:
    case BCMIMM_ENTRY_UPDATE:
        if (SHR_BITGET(pcfg.fbmp, LM_PORT_CONTROLt_MANUAL_SYNCf)) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlm_ctrl_manual_sync_update(unit,
                                               pcfg.port,
                                               pcfg.manual_sync));
        }

        if (SHR_BITGET(pcfg.fbmp, LM_PORT_CONTROLt_LINKSCAN_MODEf)) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlm_ctrl_linkscan_mode_update(unit,
                                                 pcfg.port,
                                                 pcfg.linkscan_mode));
        }

        if (SHR_BITGET(pcfg.fbmp, LM_PORT_CONTROLt_OVERRIDE_LINK_STATEf)) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlm_ctrl_override_link_state_update(unit,
                                                       pcfg.port,
                                                       pcfg.override_link_state));
        }
        if (event_reason == BCMIMM_ENTRY_INSERT) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlm_ctrl_port_add(unit, pcfg.port));
        }
        break;
    case BCMIMM_ENTRY_DELETE:
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlm_ctrl_linkscan_mode_update(unit,
                                             pcfg.port,
                                             BCMLM_LINKSCAN_MODE_NONE));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlm_ctrl_manual_sync_update(unit,
                                           pcfg.port,
                                           0));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlm_ctrl_port_delete(unit, pcfg.port));
        break;
    default:
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief LM_PORT_CONTROL IMM logical table validate callback function.
 *
 * Validate the field values of the LM_PORT_CONTROL logical table entry.
 *
 * \param [in] unit Unit number.
 * \param [in] sid This is the logical table ID.
 * \param [in] action This is the desired action for the entry.
 * \param [in] key This is a linked list of the key fields identifying
 * the entry.
 * \param [in] data This is a linked list of the data fields in the
 * modified entry.
 * \param [in] context Is a pointer that was given during registration.
 * The callback can use this pointer to retrieve some context.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Invalid field value.
 */
static int
lm_port_config_imm_validate(int unit,
                            bcmltd_sid_t sid,
                            bcmimm_entry_event_t action,
                            const bcmltd_field_t *key,
                            const bcmltd_field_t *data,
                            void *context)
{
    SHR_FUNC_ENTER(unit);

    /* Validate the port is already existing in Port Control */
    if (key->id != LM_PORT_CONTROLt_PORT_IDf) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlm_linkscan_port_validate(unit, key->data));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief LM_PORT_CONTROL In-memory event callback structure.
 *
 * This structure contains callback functions that will be corresponding
 * to LM_PORT_CONTROL logical table entry commit stages.
 */
static bcmimm_lt_cb_t lm_port_config_imm_callback = {

    /*! Validate function. */
    .validate = lm_port_config_imm_validate,

    /*! Staging function. */
    .stage = lm_port_config_imm_stage_callback,

    /*! Commit function. */
    .commit = NULL,

    /*! Abort function. */
    .abort = NULL
};

/*******************************************************************************
 * Public Functions
 */

int
bcmlm_imm_db_init(void)
{
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    if (!lm_fld_arr_hdl) {
        SHR_IF_ERR_EXIT
            (shr_famm_hdl_init(LM_FIELD_COUNT_MAX, &lm_fld_arr_hdl));
    }

exit:
    SHR_FUNC_EXIT();
}

void
bcmlm_imm_db_cleanup(void)
{
    if (lm_fld_arr_hdl) {
        shr_famm_hdl_delete(lm_fld_arr_hdl);
        lm_fld_arr_hdl = 0;
    }
}

int
bcmlm_imm_register(int unit)
{
    const bcmlrd_map_t *map = NULL;
    int rv;

    SHR_FUNC_ENTER(unit);

    /*
     * To register callback for LM LTs here.
     */
    rv = bcmlrd_map_get(unit, LM_CONTROLt, &map);
    if (SHR_SUCCESS(rv) && map) {
        SHR_IF_ERR_EXIT
            (bcmimm_lt_event_reg(unit,
                                 LM_CONTROLt,
                                 &lm_config_imm_callback,
                                 NULL));
    }

    rv = bcmlrd_map_get(unit, LM_PORT_CONTROLt, &map);
    if (SHR_SUCCESS(rv) && map) {
        SHR_IF_ERR_EXIT
            (bcmimm_lt_event_reg(unit,
                                 LM_PORT_CONTROLt,
                                 &lm_port_config_imm_callback,
                                 NULL));
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmlm_imm_link_restore(int unit, bcmlm_ctrl_t *lmctrl)
{
    bcmltd_fields_t in_flds = {0}, out_flds = {0};
    int enable = 0;
    shr_port_t port;
    int mode, ovr, manual;
    uint32_t idx;
    int rv;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(lmctrl, SHR_E_PARAM);

    in_flds.count = 1;
    in_flds.field = shr_famm_alloc(lm_fld_arr_hdl, in_flds.count);
    if (in_flds.field == NULL) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }
    out_flds.count = LM_FIELD_COUNT_MAX;
    out_flds.field = shr_famm_alloc(lm_fld_arr_hdl, out_flds.count);
    if (out_flds.field == NULL) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }

    in_flds.field[0]->id = LM_CONTROLt_SCAN_ENABLEf;
    out_flds.count = LM_CONTROLt_FIELD_COUNT;
    rv = bcmimm_entry_lookup(unit, LM_CONTROLt, &in_flds, &out_flds);
    if (rv != SHR_E_NONE && rv != SHR_E_NOT_FOUND) {
        SHR_ERR_EXIT(rv);
    }
    if (SHR_SUCCESS(rv)) {
        for (idx = 0; idx < out_flds.count; idx++) {
            if (out_flds.field[idx]->id == LM_CONTROLt_SCAN_ENABLEf) {
                enable = out_flds.field[idx]->data;
            } else if (out_flds.field[idx]->id == LM_CONTROLt_SCAN_INTERVALf) {
                lmctrl->interval_us = out_flds.field[idx]->data;
            }
        }
        if (enable) {
            SHR_IF_ERR_EXIT
                (bcmlm_ctrl_linkscan_enable(unit, 1));
        }
    }

    in_flds.field[0]->id = LM_PORT_CONTROLt_PORT_IDf;
    out_flds.count = LM_PORT_CONTROLt_FIELD_COUNT;
    rv = bcmimm_entry_get_first(unit, LM_PORT_CONTROLt, &out_flds);
    if (SHR_FAILURE(rv)) {
        if (rv == SHR_E_NOT_FOUND) {
            /* No error */
            SHR_EXIT();
        }
        SHR_ERR_EXIT(rv);
    }
    do {
        port = 0;
        mode = 0;
        ovr = 0;
        manual = 0;
        for (idx = 0; idx < out_flds.count; idx++) {
            if (out_flds.field[idx]->id == LM_PORT_CONTROLt_PORT_IDf) {
                port = out_flds.field[idx]->data;
            } else if (out_flds.field[idx]->id == LM_PORT_CONTROLt_LINKSCAN_MODEf) {
                mode = out_flds.field[idx]->data;
            } else if (out_flds.field[idx]->id == LM_PORT_CONTROLt_OVERRIDE_LINK_STATEf) {
                ovr = out_flds.field[idx]->data;
            } else if (out_flds.field[idx]->id == LM_PORT_CONTROLt_MANUAL_SYNCf) {
                manual = out_flds.field[idx]->data;
            }
        }
        if (mode == BCMLM_LINKSCAN_MODE_SW) {
            BCMDRD_PBMP_PORT_ADD(lmctrl->pbm_sw, port);
            BCMDRD_PBMP_PORT_ADD(lmctrl->pbm_linkscan, port);
        } else if (mode == BCMLM_LINKSCAN_MODE_HW) {
            BCMDRD_PBMP_PORT_ADD(lmctrl->pbm_hw, port);
            BCMDRD_PBMP_PORT_ADD(lmctrl->pbm_linkscan, port);
        } else if (mode == BCMLM_LINKSCAN_MODE_OVERRIDE) {
            BCMDRD_PBMP_PORT_ADD(lmctrl->pbm_ovr, port);
            BCMDRD_PBMP_PORT_ADD(lmctrl->pbm_linkscan, port);
        }
        if (ovr) {
            BCMDRD_PBMP_PORT_ADD(lmctrl->pbm_ovr_link, port);
        }
        if (manual) {
            BCMDRD_PBMP_PORT_ADD(lmctrl->pbm_manual_sync, port);
        }
        BCMDRD_PBMP_PORT_ADD(lmctrl->pbm_valid, port);
        if (bcmpc_lport_to_pport(unit, port) == BCMPC_INVALID_PPORT) {
            BCMDRD_PBMP_PORT_ADD(lmctrl->pbm_suspend, port);
        }
        in_flds.field[0]->data = port;
        out_flds.count = LM_PORT_CONTROLt_FIELD_COUNT;
        rv = bcmimm_entry_get_next(unit, LM_PORT_CONTROLt, &in_flds, &out_flds);
        if (SHR_FAILURE(rv)) {
            if (rv == SHR_E_NOT_FOUND) {
                rv = SHR_E_NONE;
            }
            SHR_ERR_EXIT(rv);
        }
    } while (SHR_SUCCESS(rv));

exit:
    if (in_flds.field) {
        shr_famm_free(lm_fld_arr_hdl, in_flds.field, 1);
    }
    if (out_flds.field) {
        shr_famm_free(lm_fld_arr_hdl, out_flds.field, LM_FIELD_COUNT_MAX);
    }

    SHR_FUNC_EXIT();
}
