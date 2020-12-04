/*! \file bcmtm_imm_ebst_port.c
 *
 * BCMTM EBST interface to in-memory table for ports.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <shr/shr_debug.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmlrd/bcmlrd_map.h>
#include <bcmtm/bcmtm_drv.h>
#include <bcmtm/ebst/bcmtm_ebst_internal.h>
#include <bcmtm/bcmtm_utils_internal.h>
#include <bcmtm/bcmtm_utils.h>
#include <bcmtm/imm/bcmtm_imm_defines_internal.h>

/*******************************************************************************
 * Local definitions
 */
#define BSL_LOG_MODULE  BSL_LS_BCMTM_IMM

/*******************************************************************************
 * Private functions
 */
/*!
 * \brief Parse the key and data fields
 *
 * \param [in] unit Unit number.
 * \param [in] data Array of data field.
 * \param [out] port_cfg EBST port configuration.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Error code.
 */
static void
bcmtm_ebst_port_lt_fields_parse(int unit,
                                const bcmltd_field_t *data,
                                bcmtm_ebst_port_cfg_t *port_cfg)
{
    const bcmltd_field_t *gen_field;
    uint32_t fid;
    uint64_t fval;

    /* Parse data field */
    gen_field = data;
    while (gen_field) {
        fid = gen_field->id;
        fval = gen_field->data;

        switch (fid) {
            case TM_EBST_PORTt_MONITORf:
                port_cfg->enable = fval;
                break;
            case TM_EBST_PORTt_OPERATIONAL_STATEf:
                port_cfg->opcode = fval;
                break;
            default:
                break;
        }
        gen_field = gen_field->next;
    }
}

/*!
 * \brief EBST port configuration imm lookup.
 *
 * Performs IMM lookup and populates LT config for TM_EBST_PORT.
 *
 * \param [in] unit Logical unit number.
 * \param [in] ltid Logical table ID.
 * \param [in] lport Logical port.
 * \param [out] port_cfg EBST IMM port configuration.
 *
 * \retval SHR_E_NONE No error.
 * \retval !SHR_E_NONE Error code.
 */
static int
bcmtm_ebst_port_imm_lkup(int unit,
                         bcmltd_sid_t ltid,
                         bcmtm_lport_t lport,
                         bcmtm_ebst_port_cfg_t *port_cfg)
{
    bcmltd_fields_t out, in;
    bcmltd_fid_t fid;
    size_t num_fid;

    SHR_FUNC_ENTER(unit);
    sal_memset(&in, 0, sizeof(bcmltd_fields_t));
    sal_memset(&out, 0, sizeof(bcmltd_fields_t));

    SHR_IF_ERR_EXIT(bcmlrd_field_count_get(unit, ltid, &num_fid));
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_alloc(unit, num_fid, &out));
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_alloc(unit, 1, &in));
    in.count = 0;

    fid = TM_EBST_PORTt_PORT_IDf;
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_set(unit, &in, fid, 0, lport));

    if (SHR_FAILURE(bcmimm_entry_lookup(unit, ltid, &in, &out))) {
        port_cfg->opcode = ENTRY_INVALID;
    } else {
        bcmtm_ebst_port_lt_fields_parse(unit, *(out.field), port_cfg);
    }
exit:
    bcmtm_field_list_free(&in);
    bcmtm_field_list_free(&out);
    SHR_FUNC_EXIT();
}

/*!
 * \brief TM_EBST_PORTt imm update.
 *
 * Performs IMM updates for TM_EBST_PORT.
 *
 * \param [in] unit Logical unit number.
 * \param [in] ltid Logical table ID.
 * \param [in] lport Logical port.
 * \param [in] port_cfg Logical table entry configuration.
 *
 * \retval SHR_E_NONE No error.
 * \retval !SHR_E_NONE Error code.
 */
static int
bcmtm_ebst_port_imm_update(int unit,
                           bcmltd_sid_t ltid,
                           bcmtm_lport_t lport,
                           bcmtm_ebst_port_cfg_t *port_cfg)
{
    bcmltd_fields_t in;
    bcmltd_fid_t fid;

    SHR_FUNC_ENTER(unit);

    sal_memset(&in, 0, sizeof(bcmltd_fields_t));
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_alloc(unit, 2, &in));

    in.count = 0;

    /* TM_EBST_PORTt_PORT_IDf */
    fid = TM_EBST_PORTt_PORT_IDf;
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_set(unit, &in, fid, 0, lport));

    /* TM_EBST_PORTt_OPERATIONAL_STATEf */
    fid = TM_EBST_PORTt_OPERATIONAL_STATEf;
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_set(unit, &in, fid, 0, port_cfg->opcode));

    SHR_IF_ERR_EXIT
        (bcmimm_entry_update(unit, 0, ltid, &in));
exit:
    bcmtm_field_list_free(&in);
    SHR_FUNC_EXIT();
}

/*!
 * \brief TM_EBST_PORTt physical table update.
 * Update physical table depending on device type.
 *
 * \param [in] unit Logical unit number.
 * \param [in] ltid Logical table ID.
 * \param [in] lport Logical port.
 * \param [in] port_cfg EBST IMM port configuration.
 *
 * \retval SHR_E_NONE No error.
 * \retval !SHR_E_NONE Error code.
 */
static int
bcmtm_ebst_port_pt_set(int unit,
                       bcmltd_sid_t ltid,
                       bcmtm_lport_t lport,
                       bcmtm_ebst_port_cfg_t *port_cfg)
{
    bcmtm_ebst_drv_t ebst_drv;
    bcmtm_drv_t *drv;
    int rv, pport;
    bcmtm_ebst_dev_info_t *ebst_info;

    SHR_FUNC_ENTER(unit);
    sal_memset(&ebst_drv, 0, sizeof(bcmtm_ebst_drv_t));
    SHR_IF_ERR_EXIT(bcmtm_drv_get(unit, &drv));
    SHR_NULL_CHECK(drv->ebst_drv_get, SHR_E_UNAVAIL);
    SHR_IF_ERR_EXIT(drv->ebst_drv_get(unit, &ebst_drv));

    bcmtm_ebst_dev_info_get(unit, &ebst_info);
    switch (port_cfg->action) {
        case ACTION_INSERT:
        case ACTION_DELETE:
            /* check for logical port to physical port mapping. */
            rv = bcmtm_lport_pport_get(unit, lport, &pport);
            if (SHR_FAILURE(rv)) {
                port_cfg->opcode = EBST_OPER_PORT_INFO_UNAVAILABLE;
                SHR_EXIT();
            }
            port_cfg->opcode = EBST_OPER_VALID;
            break;
        case ACTION_PORT_UPDATE_INT:
            port_cfg->opcode = EBST_OPER_VALID;
            break;
        case ACTION_PORT_DELETE_INT:
            sal_memset(port_cfg, 0, sizeof(bcmtm_ebst_port_cfg_t));
            port_cfg->opcode = EBST_OPER_PORT_INFO_UNAVAILABLE;
            break;
        default:
            break;
    }

    SHR_IF_ERR_EXIT
        (ebst_drv.ebst_port_config_set(unit, ltid, lport, port_cfg));
    port_cfg->enable ? (ebst_info->monitor++): (ebst_info->monitor--);
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief EBST port IMM table change callback function for staging.
 *
 * Handle EBST port IMM table change events.
 *
 * \param [in] unit Unit number.
 * \param [in] sid This is the logical table ID.
 * \param [in] action This is the desired action for the entry.
 * \param [in] key This is a linked list of the key fields identifying
 * the entry.
 * \param [in] data This is a linked list of the data fields in the
 * modified entry.
 * \param [in] context Is a pointer that was given during registration.
 * \param [out] output_fields Read only fields update to imm.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL BST fails to handle LT change events.
 */
static int
bcmtm_ebst_port_imm_stage_callback(int unit,
                                   bcmltd_sid_t ltid,
                                   uint32_t trans_id,
                                   bcmimm_entry_event_t event_reason,
                                   const bcmltd_field_t *key,
                                   const bcmltd_field_t *data,
                                   void *context,
                                   bcmltd_fields_t *output_fields)
{
    bcmtm_ebst_port_cfg_t port_cfg;
    bcmltd_fid_t fid;
    uint64_t fval;
    bcmtm_lport_t lport;

    SHR_FUNC_ENTER(unit);

    sal_memset(&port_cfg, 0, sizeof(bcmtm_ebst_port_cfg_t));

    /* TM_EBST_PORTt_PORT_IDf */
    fid = TM_EBST_PORTt_PORT_IDf;
    SHR_IF_ERR_EXIT
        (bcmtm_fval_get_from_field_array(unit, fid, 0, key, &fval));
    lport = (bcmtm_lport_t)fval;

    switch (event_reason) {
        case BCMIMM_ENTRY_UPDATE:
            /* get the configuration form IMM */
            SHR_IF_ERR_EXIT
                (bcmtm_ebst_port_imm_lkup(unit, ltid, lport, &port_cfg));
            /* Fall through */
        case BCMIMM_ENTRY_INSERT:
            port_cfg.action = ACTION_INSERT;
            SHR_NULL_CHECK(output_fields, SHR_E_INTERNAL);
            output_fields->count = 0;
            bcmtm_ebst_port_lt_fields_parse (unit, data, &port_cfg);
            SHR_IF_ERR_EXIT
                (bcmtm_ebst_port_pt_set(unit, ltid, lport, &port_cfg));
            /* update the operational state */
            fid = TM_EBST_PORTt_OPERATIONAL_STATEf;
            SHR_IF_ERR_EXIT
                (bcmtm_field_list_set(unit, output_fields, fid,
                                      0, port_cfg.opcode));
            break;
        case BCMIMM_ENTRY_DELETE:
            port_cfg.action = ACTION_DELETE;
            SHR_IF_ERR_EXIT
                (bcmtm_ebst_port_pt_set(unit, ltid, lport, &port_cfg));
            break;
        default:
            break;
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmtm_imm_ebst_port_register(int unit)
{
    const bcmlrd_map_t *map = NULL;
    int rv;
    /*
     * This structure contains callback functions that will be corresponding
     * to TM_EBST_PORT logical table entry commit stages.
     */
    static bcmimm_lt_cb_t ebst_port_imm_callback = {
        /*! Staging function. */
        .stage = bcmtm_ebst_port_imm_stage_callback,
    };

    SHR_FUNC_ENTER(unit);
    rv = bcmlrd_map_get(unit, TM_EBST_PORTt, &map);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_UNAVAIL);

    if ((rv == SHR_E_UNAVAIL) || (!map)) {
        SHR_EXIT();
    }
    /*! Registers callback function for ebst port profile. */
    SHR_IF_ERR_EXIT
        (bcmimm_lt_event_reg(unit, TM_EBST_PORTt, &ebst_port_imm_callback, NULL));
exit:
    SHR_FUNC_EXIT();
}

int
bcmtm_ebst_port_internal_update(int unit,
                                bcmtm_lport_t lport,
                                uint8_t action)
{
    bcmltd_sid_t ltid = TM_EBST_PORTt;
    bcmtm_ebst_port_cfg_t port_cfg;
    int rv;
    const bcmlrd_map_t *map = NULL;

    SHR_FUNC_ENTER(unit);

    rv = bcmlrd_map_get(unit, ltid, &map);
    if ((rv  == SHR_E_UNAVAIL) || (!map)) {
        SHR_EXIT();
    }
    sal_memset(&port_cfg, 0, sizeof(bcmtm_ebst_port_cfg_t));

    SHR_IF_ERR_EXIT
        (bcmtm_ebst_port_imm_lkup(unit, ltid, lport, &port_cfg));
    port_cfg.action = action;

    if (port_cfg.opcode != ENTRY_INVALID) {
        SHR_IF_ERR_EXIT
            (bcmtm_ebst_port_pt_set(unit, ltid, lport, &port_cfg));
        SHR_IF_ERR_EXIT
            (bcmtm_ebst_port_imm_update(unit, ltid, lport, &port_cfg));
    }
exit:
    SHR_FUNC_EXIT();
}
