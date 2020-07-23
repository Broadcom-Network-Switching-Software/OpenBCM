/*! \file bcmtm_imm_ct_port.c
 *
 * In-memory call back function for cut-through port settings for front panel
 * ports.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcmdrd/bcmdrd_dev.h>
#include <bcmlrd/bcmlrd_map.h>
#include <bcmltd/bcmltd_types.h>
#include <bcmltd/bcmltd_handler.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <shr/shr_debug.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmimm/bcmimm_int_comp.h>
#include <bcmtm/bcmtm_drv.h>
#include <bcmtm/bcmtm_utils.h>
#include <bcmtm/bcmtm_utils_internal.h>
#include <bcmtm/bcmtm_types.h>
#include <bcmtm/ct/bcmtm_ct_internal.h>
#include <bcmtm/imm/bcmtm_imm_defines_internal.h>
/*******************************************************************************
 * Local definitions
 */
#define BSL_LOG_MODULE  BSL_LS_BCMTM_IMM

/*******************************************************************************
 * Private functions
 */
/*!
 * \brief  Cut-through physical table configuration based on device type.
 *
 * \param [in] unit  Unit number.
 * \param [in] ltid Logical table ID.
 * \param [in] lport Logical port number.
 * \param [in] ct_enable Enable/disable cut-through mode.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
static int
bcmtm_ct_port_pt_set(int unit,
                     bcmltd_sid_t ltid,
                     bcmtm_lport_t lport,
                     uint32_t ct_enable)
{
    bcmtm_drv_t *drv;
    bcmtm_ct_drv_t ct_drv;

    SHR_FUNC_ENTER(unit);

    sal_memset(&ct_drv, 0, sizeof(bcmtm_ct_drv_t));
    SHR_IF_ERR_EXIT(bcmtm_drv_get(unit, &drv));
    SHR_NULL_CHECK(drv->ct_drv_get, SHR_E_UNAVAIL);

    SHR_IF_ERR_EXIT(drv->ct_drv_get(unit, &ct_drv));
    /* update physical table. */
    if (ct_drv.port_set) {
        SHR_IF_ERR_EXIT
            (ct_drv.port_set(unit, ltid, lport, ct_enable));
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief  Resets the physical table to default values.
 *
 * \param [in] unit Unit number.
 * \param [in] ltid Logical table ID.
 * \param [in] lport Logical port ID.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
static int
bcmtm_ct_port_reset(int unit,
                    bcmltd_sid_t ltid,
                    bcmtm_lport_t lport)
{
    uint32_t ct_enable = 0;
    int mport;

    SHR_FUNC_ENTER(unit);

    /* Cut-through is valid for front panel ports only. */
    if (!(bcmtm_lport_is_fp(unit, lport))) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (SHR_FAILURE(bcmtm_lport_mmu_local_port_get(unit, lport, &mport))) {
        SHR_EXIT();
    }
    /* Physical table update */
    SHR_IF_ERR_EXIT
        (bcmtm_ct_port_pt_set(unit, ltid, lport, ct_enable));
exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Updates IMM for cut through LT.
 *
 * \param [in] unit Unit number.
 * \param [in] ltid Logical table ID.
 * \param [in] lport Logical port ID.
 * \param [in] opcode Operational state to be updated to IMM.
 *
 * \retval SHR_E_NONE No error.
 * \retval !SHR_E_NONE Error code.
 */
static int
bcmtm_ct_port_imm_update(int unit,
                         bcmltd_sid_t ltid,
                         bcmtm_lport_t lport,
                         uint8_t opcode)
{
    bcmltd_fields_t in;
    bcmltd_fid_t fid;

    SHR_FUNC_ENTER(unit);

    sal_memset(&in, 0, sizeof(bcmltd_fields_t));
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_alloc(unit, 2, &in));

    in.count = 0;

    /* TM_CUT_THROUGH_PORTt_PORT_IDf */
    fid =TM_CUT_THROUGH_PORTt_PORT_IDf;
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_set(unit, &in, fid, 0, lport));

    /* TM_CUT_THROUGH_PORTt_OPERATIONAL_STATEf */
    fid = TM_CUT_THROUGH_PORTt_OPERATIONAL_STATEf;
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_set(unit, &in, fid, 0, opcode));

    SHR_IF_ERR_EXIT
        (bcmimm_entry_update(unit, 0, ltid, &in));
exit:
    bcmtm_field_list_free(&in);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Configures the physical table with user defined values.
 *
 * \param [in] unit Unit number.
 * \param [in] ltid Logical table ID.
 * \param [in] lport Logical port ID.
 * \param [in] ct_enable Cut-Through enable/disable.
 * \param [in] action Action to be performed.
 * \param [out] opcode Operational status code.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
static int
bcmtm_ct_port_set(int unit,
                  bcmltd_sid_t ltid,
                  bcmtm_lport_t lport,
                  uint32_t ct_enable,
                  uint8_t action,
                  uint8_t *opcode)
{
    int mport;

    SHR_FUNC_ENTER(unit);

    /* Cut-through is valid for front panel ports only. */
    if (!(bcmtm_lport_is_fp(unit, lport))) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    switch (action) {
        case ACTION_INSERT:
            /* TM local port information.
             * Returns error if local port to physical port map is not available.
             * Updates opeartional state.
             */
            if (SHR_FAILURE(bcmtm_lport_mmu_local_port_get(unit, lport, &mport))) {
                *opcode = PORT_INFO_UNAVAIL;
                SHR_EXIT();
            }
        /* Fall through */
        case ACTION_PORT_UPDATE_INT:
            /* PT update */
            SHR_IF_ERR_EXIT
                (bcmtm_ct_port_pt_set(unit, ltid, lport, ct_enable));
            /* opcode success */
            *opcode = VALID;
            break;

        case ACTION_PORT_DELETE_INT:
            SHR_IF_ERR_EXIT
                (bcmtm_ct_port_reset(unit, ltid, lport));
            *opcode = PORT_INFO_UNAVAIL;
            break;

        default:
            break;
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Cut through LT imm lookup.
 *
 * \param [in] unit Unit number.
 * \param [in] ltid Logical table ID.
 * \param [in] lport Logical port ID.
 * \param [out] ct_enable Cut through state.
 * \param [out] opcode Operational State code.
 *
 * \retval SHR_E_NONE No error.
 * \retval !SHR_E_NONE Error code.
 */
static int
bcmtm_ct_port_imm_lkup(int unit,
                       bcmltd_sid_t ltid,
                       bcmtm_lport_t lport,
                       uint32_t *ct_enable,
                       uint8_t *opcode)
{
    bcmltd_fields_t out, in;
    bcmltd_fid_t fid;
    size_t num_fid;
    uint64_t fval;

    SHR_FUNC_ENTER(unit);
    sal_memset(&in, 0, sizeof(bcmltd_fields_t));
    sal_memset(&out, 0, sizeof(bcmltd_fields_t));

    *ct_enable = 0;

    SHR_IF_ERR_EXIT(bcmlrd_field_count_get(unit, ltid, &num_fid));
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_alloc(unit, num_fid, &out));
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_alloc(unit, 1, &in));
    in.count = 0;

    fid = TM_CUT_THROUGH_PORTt_PORT_IDf;
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_set(unit, &in, fid, 0, lport));

    if (SHR_FAILURE(bcmimm_entry_lookup(unit, ltid, &in, &out))) {
        *opcode = ENTRY_INVALID;
    } else {
        /* TM_CUT_THROUGH_PORTt_CUT_THROUGHf */
        fid = TM_CUT_THROUGH_PORTt_CUT_THROUGHf;
        if (SHR_SUCCESS(bcmtm_fval_get_from_field_array(unit, fid, 0,
                        *(out.field), &fval))) {
            *ct_enable = (uint32_t)fval;
        }
        /* TM_CUT_THROUGH_PORTt_OPERATIONAL_STATEf */
        fid = TM_CUT_THROUGH_PORTt_OPERATIONAL_STATEf;
        SHR_IF_ERR_EXIT
            (bcmtm_fval_get_from_field_array(unit, fid, 0, *(out.field), &fval));
        *opcode = (uint8_t)fval;
    }
exit:
    bcmtm_field_list_free(&in);
    bcmtm_field_list_free(&out);
    SHR_FUNC_EXIT();
}

/*!
 * \brief TM_CUT_THROUGH_PORT logical table callback function for staging.
 *
 * Handle TM_CUT_THROUGH_PORT IMM table change events.
 *
 * \param [in] unit Unit number.
 * \param [in] sid This is the logical table ID.
 * \param [in] trans_id LT Transaction Id
 * \param [in] event_reason Call back reason.
 * \param [in] key This is a linked list of the key fields identifying
 * the entry.
 * \param [in] data This is a linked list of the data fields in
 * modified entry.
 * \param [in] context Is a pointer that was given during registration.
 * \param [out] data. This is a linked list of extra added data fields (mostly
 * read only fields).
 * The callback can use this pointer to retrieve some context.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL LM fails to handle LT change events.
 */
static int
bcmtm_ct_port_stage(int unit,
                    bcmltd_sid_t ltid,
                    uint32_t trans_id,
                    bcmimm_entry_event_t event_reason,
                    const bcmltd_field_t *key,
                    const bcmltd_field_t *data,
                    void *context,
                    bcmltd_fields_t *output_fields)
{
    bcmtm_lport_t lport;
    bcmltd_fid_t fid;
    uint64_t fval = 0;
    uint32_t ct_enable = 0;
    uint8_t opcode;
    uint8_t action;

    SHR_FUNC_ENTER(unit);

   /* TM_CUT_THROUGH_PORTt_PORT_IDf */
    fid = TM_CUT_THROUGH_PORTt_PORT_IDf;
    SHR_IF_ERR_EXIT
        (bcmtm_fval_get_from_field_array(unit, fid, 0, key, &fval));
    lport = (bcmtm_lport_t)fval;

    switch (event_reason) {
        case BCMIMM_ENTRY_UPDATE:
        case BCMIMM_ENTRY_INSERT:
            if (data) {
                fid = TM_CUT_THROUGH_PORTt_CUT_THROUGHf;
                SHR_IF_ERR_EXIT
                    (bcmtm_fval_get_from_field_array(unit, fid, 0, data, &fval));
                ct_enable = (uint32_t)fval;
            }
            SHR_NULL_CHECK(output_fields, SHR_E_PARAM);
            output_fields->count = 0;
            action = ACTION_INSERT;
            SHR_IF_ERR_EXIT
                (bcmtm_ct_port_set(unit, ltid, lport, ct_enable,
                                   action, &opcode));
            fid = TM_CUT_THROUGH_PORTt_OPERATIONAL_STATEf;
            SHR_IF_ERR_EXIT
                (bcmtm_field_list_set(unit, output_fields, fid, 0, opcode));
            break;
        case BCMIMM_ENTRY_DELETE:
            /* disable cut-through. */
            SHR_IF_ERR_EXIT
                (bcmtm_ct_port_reset(unit, ltid, lport));
            break;
        default:
            break;
    }
exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public functions
 */
int bcmtm_ct_port_internal_update(int unit,
                                  bcmtm_lport_t lport,
                                  uint8_t action)
{
    bcmltd_sid_t ltid = TM_CUT_THROUGH_PORTt;
    uint32_t ct_enable = 0;
    uint8_t opcode;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmtm_ct_port_imm_lkup(unit, ltid, lport, &ct_enable, &opcode));

    if (opcode != ENTRY_INVALID) {
        SHR_IF_ERR_EXIT
            (bcmtm_ct_port_set(unit, ltid, lport, ct_enable, action, &opcode));
        SHR_IF_ERR_EXIT
            (bcmtm_ct_port_imm_update(unit, ltid, lport, opcode));
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmtm_ct_port_imm_register(int unit)
{
    /*! Inmemory callback functions for TM_CUT_THROUGH_PORTt logical table. */
    bcmimm_lt_cb_t bcmtm_ct_port_imm_cb = {
        /*! Staging function. */
        .stage = bcmtm_ct_port_stage,
    };
    const bcmlrd_map_t *map = NULL;
    int rv;

    SHR_FUNC_ENTER(unit);
    rv = bcmlrd_map_get(unit, TM_CUT_THROUGH_PORTt, &map);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_UNAVAIL);

    if ((rv == SHR_E_UNAVAIL) || (!map)) {
        SHR_EXIT();
    }
    /*! Registers callback functions for cut-through settings. */
    SHR_IF_ERR_EXIT
        (bcmimm_lt_event_reg(unit, TM_CUT_THROUGH_PORTt,
                             &bcmtm_ct_port_imm_cb, NULL));
exit:
    SHR_FUNC_EXIT();
}
