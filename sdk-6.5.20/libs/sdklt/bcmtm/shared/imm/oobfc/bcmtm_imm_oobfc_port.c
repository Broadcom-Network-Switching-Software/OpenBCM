/*! \file bcmtm_imm_oobfc_port.c
 *
 * In-memory call back functions for OOBFC ports.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcmlrd/bcmlrd_map.h>
#include <bcmltd/bcmltd_types.h>
#include <bcmltd/bcmltd_handler.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <shr/shr_debug.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmimm/bcmimm_int_comp.h>
#include <bcmtm/bcmtm_utils_internal.h>
#include <bcmtm/bcmtm_types.h>
#include <bcmtm/bcmtm_drv.h>
#include <bcmtm/bcmtm_utils.h>
#include <bcmtm/imm/bcmtm_imm_defines_internal.h>
#include <bcmtm/oobfc/bcmtm_oobfc_internal.h>

/*******************************************************************************
 * Local definitions
 */
#define BSL_LOG_MODULE BSL_LS_BCMTM_IMM

/*******************************************************************************
 * Private functions
 */
/*!
 * \brief Parse the key and data fields.
 *
 * \param [in] unit Unit number.
 * \param [in] data Array of Data fields
 * \param [out] port_cfg Port configuration for OOBFC port.
 */
static void
bcmtm_oobfc_port_lt_fields_parse (int unit,
                                  const bcmltd_field_t *data,
                                  bcmtm_oobfc_port_cfg_t *port_cfg)
{
    uint32_t fid;
    uint64_t fval;
    bcmltd_field_t *fld = (bcmltd_field_t *)data;

    port_cfg->oob_port = -1;
    while (fld) {
        fid = fld->id;
        fval = fld->data;
        switch (fid) {
            case TM_OOBFC_PORTt_OOB_PORTf:
                port_cfg->oob_port = fval;
                break;
            case TM_OOBFC_PORTt_INGRESSf:
                port_cfg->ingress = fval;
                break;
            case TM_OOBFC_PORTt_EGRESSf:
                port_cfg->egress = fval;
                break;
            case TM_OOBFC_PORTt_CNG_REPORTf:
                port_cfg->cng_report = fval;
                break;
            case TM_OOBFC_PORTt_TM_OOBFC_Q_MAP_PROFILE_IDf:
                port_cfg->q_map_profile_id = fval;
                break;
            case TM_OOBFC_PORTt_OPERATIONAL_STATEf:
                port_cfg->opcode = fval;
                break;
            default:
                break;
        }
        fld = fld->next;
    }
}

/*!
 * \brief OOBFC port entry imm lookup.
 *
 * \param [in] unit Logical unit.
 * \param [in] ltid Logical table ID.
 * \param [in] lport Logical port ID.
 * \param [out] port_cfg Logical table port configuration for given lport.
 *
 * \retval SHR_E_NONE No error.
 * \retval !SHR_E_NONE Error code.
 */
static int
bcmtm_oobfc_port_imm_lkup(int unit,
                          bcmltd_sid_t ltid,
                          bcmtm_lport_t lport,
                          bcmtm_oobfc_port_cfg_t *port_cfg)
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
    fid = TM_OOBFC_PORTt_PORT_IDf;
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_set(unit, &in, fid, 0, lport));

    if (SHR_FAILURE(bcmimm_entry_lookup(unit, ltid, &in, &out))) {
        port_cfg->opcode = ENTRY_INVALID;
    } else {
        bcmtm_oobfc_port_lt_fields_parse (unit, *(out.field), port_cfg);
    }
exit:
    bcmtm_field_list_free(&in);
    bcmtm_field_list_free(&out);
    SHR_FUNC_EXIT();
}


/*!
 * \brief OOBFC port configuration configure to hardware.
 *
 * \param [in] unit Logical unit number.
 * \param [in] ltid Logical table ID.
 * \param [in] lport Logical port ID.
 * \param [in] port_cfg Port configuration to be programmed to hardware.
 *
 * \retval SHR_E_NONE No error.
 * \retval !SHR_E_NONE Error code.
 */
static int
bcmtm_oobfc_port_pt_set(int unit,
                        bcmltd_sid_t ltid,
                        bcmtm_lport_t lport,
                        bcmtm_oobfc_port_cfg_t *port_cfg)
{
    bcmtm_drv_t *drv;
    bcmtm_oobfc_drv_t oobfc_drv;

    SHR_FUNC_ENTER(unit);
    sal_memset(&oobfc_drv, 0, sizeof(bcmtm_oobfc_drv_t));

    SHR_IF_ERR_EXIT(bcmtm_drv_get(unit, &drv));
    SHR_NULL_CHECK(drv->oobfc_drv_get, SHR_E_UNAVAIL);

    SHR_IF_ERR_EXIT(drv->oobfc_drv_get(unit, &oobfc_drv));
    /* update physical table. */
    if (oobfc_drv.port_set) {
        SHR_IF_ERR_EXIT
            (oobfc_drv.port_set(unit, ltid, lport, port_cfg));
    }
exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief OOBFC port configuration imm update.
 *
 * \param [in] unit Logical unit number.
 * \param [in] ltid Logical table ID.
 * \param [in] lport Logical port ID.
 * \param [in] port_cfg Port configuration to be update in IMM.
 *
 * \retval SHR_E_NONE No error.
 * \retval !SHR_E_NONE Error code.
 */
static int
bcmtm_oobfc_port_imm_update (int unit,
                             bcmltd_sid_t ltid,
                             bcmtm_lport_t lport,
                             bcmtm_oobfc_port_cfg_t *port_cfg)
{
    bcmltd_fields_t in;
    bcmltd_fid_t fid;

    SHR_FUNC_ENTER(unit);

    sal_memset(&in, 0, sizeof(bcmltd_fields_t));
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_alloc(unit, 2, &in));

    in.count = 0;

    /* TM_OOBFC_PORTt_PORT_IDf */
    fid = TM_OOBFC_PORTt_PORT_IDf;
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_set(unit, &in, fid, 0, lport));

    /* TM_OOBFC_PORTt_OPERATIONAL_STATEf */
    fid = TM_OOBFC_PORTt_OPERATIONAL_STATEf;
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_set(unit, &in, fid, 0, port_cfg->opcode));

    SHR_IF_ERR_EXIT
        (bcmimm_entry_update(unit, 0, ltid, &in));
exit:
    bcmtm_field_list_free(&in);
    SHR_FUNC_EXIT();
}

/*!
 * \brief BCMTM oobfc port reset.
 *
 * \param [in] unit Logical unit.
 * \param [in] ltid Logical table ID.
 * \param [in] lport Logical port.
 *
 * \retval SHR_E_NONE No error.
 * \retval !SHR_E_NONE Error code.
 */
static int
bcmtm_oobfc_port_reset (int unit,
                        bcmltd_sid_t ltid,
                        bcmtm_lport_t lport)
{
    bcmtm_oobfc_port_cfg_t port_cfg;

    SHR_FUNC_ENTER(unit);

    sal_memset(&port_cfg, 0, sizeof(bcmtm_oobfc_port_cfg_t));

    SHR_IF_ERR_EXIT
        (bcmtm_oobfc_port_imm_lkup(unit, ltid, lport, &port_cfg));
    if (port_cfg.opcode == VALID) {
        sal_memset(&port_cfg, 0, sizeof(bcmtm_oobfc_port_cfg_t));

        /* Physical table update*/
        SHR_IF_ERR_EXIT
            (bcmtm_oobfc_port_pt_set(unit, ltid, lport, &port_cfg));
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief OOBFC port configuration set.
 *
 * \param [in] unit Logical unit.
 * \param [in] ltid Logical table ID.
 * \param [in] lport Logical port.
 * \param [in] port_cfg Port configuration to be updated.
 *
 * \retval SHR_E_NONE No error.
 * \retval !SHR_E_NONE Error code.
 */
static int
bcmtm_oobfc_port_set (int unit,
                      bcmltd_sid_t ltid,
                      bcmtm_lport_t lport,
                      bcmtm_oobfc_port_cfg_t *port_cfg)
{
    int mport;
    uint8_t action;

    SHR_FUNC_ENTER(unit);

    action = port_cfg->action;

    switch (action) {
        case ACTION_INSERT:
            /*
             * TM local port information.
             * Return errors if local port to physical port map is not available.
             * Updates operational state.
             */
            if (SHR_FAILURE(bcmtm_lport_mmu_local_port_get(unit, lport, &mport))) {
                port_cfg->opcode = PORT_INFO_UNAVAIL;
                SHR_EXIT();
            }

            /* Fall through */
        case ACTION_PORT_UPDATE_INT:
            /* PT update */
            SHR_IF_ERR_EXIT
                (bcmtm_oobfc_port_pt_set(unit, ltid, lport, port_cfg));
            port_cfg->opcode = VALID;
            break;

        case ACTION_PORT_DELETE_INT:
            /* PT update. */
            SHR_IF_ERR_EXIT
                (bcmtm_oobfc_port_reset(unit, ltid, lport));
            port_cfg->opcode = PORT_INFO_UNAVAIL;
            break;

        default:
            break;
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief OOBFC port IMM table change callback function for staging.
 *
 * Handle OOBFC port IMM table change.
 *
 * \param [in] unit Unit number.
 * \param [in] sid This is the logical table ID.
 * \param [in] trans_id Transaction ID.
 * \param [in] event_reason IMM event.
 * \param [in] key This is a linked list of the key fields identifying
 * the entry.
 * \param [in] data This is a linked list of the data fields in the
 * modified entry.
 * \param [in] context Is a pointer that was given during registration.
 * The callback can use this pointer to retrieve some context.
 * \param [out] output_fields Readonly fields to be updated in IMM.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL BST fails to handle LT change events.
 */
static int
bcmtm_oobfc_port_stage (int unit,
                        bcmltd_sid_t sid,
                        uint32_t trans_id,
                        bcmimm_entry_event_t event_reason,
                        const bcmltd_field_t *key,
                        const bcmltd_field_t *data,
                        void *context,
                        bcmltd_fields_t *output_fields)
{
    bcmtm_oobfc_port_cfg_t  port_cfg;
    uint64_t fval =0;
    bcmtm_lport_t lport;
    bcmltd_fid_t fid;

    SHR_FUNC_ENTER(unit);
    sal_memset(&port_cfg, 0, sizeof(bcmtm_oobfc_port_cfg_t));

    /* TM_OOBFC_PORTt_PORT_IDf */
    fid = TM_OOBFC_PORTt_PORT_IDf;
    SHR_IF_ERR_EXIT
        (bcmtm_fval_get_from_field_array(unit, fid, 0, key, &fval));
    lport = (bcmtm_lport_t) fval;


    switch (event_reason) {
        case BCMIMM_ENTRY_UPDATE:
            SHR_IF_ERR_EXIT
                (bcmtm_oobfc_port_imm_lkup(unit, sid, lport, &port_cfg));

            /* Fall through */
        case BCMIMM_ENTRY_INSERT:
            SHR_NULL_CHECK (output_fields, SHR_E_PARAM);
            output_fields->count = 0;
            bcmtm_oobfc_port_lt_fields_parse(unit, data, &port_cfg);
            port_cfg.action = ACTION_INSERT;
            SHR_IF_ERR_EXIT
                (bcmtm_oobfc_port_set(unit, sid, lport, &port_cfg));

            /* Operational state update. */
            fid = TM_OOBFC_PORTt_OPERATIONAL_STATEf;
            SHR_IF_ERR_EXIT
                (bcmtm_field_list_set(unit, output_fields,
                                      fid, 0, port_cfg.opcode));
            break;
        case BCMIMM_ENTRY_DELETE:
            SHR_IF_ERR_EXIT
                (bcmtm_oobfc_port_reset(unit, sid, lport));
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
int
bcmtm_oobfc_port_internal_update(int unit,
                                 bcmtm_lport_t lport,
                                 uint8_t action)
{
    bcmltd_sid_t ltid = TM_OOBFC_PORTt;
    bcmtm_oobfc_port_cfg_t port_cfg;
    const bcmlrd_map_t *map = NULL;
    int rv;

    SHR_FUNC_ENTER(unit);

    /* check for if mapping is valid. */
    rv = bcmlrd_map_get(unit, ltid, &map);
    if ((SHR_FAILURE(rv)) || (!map)) {
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT
        (bcmtm_oobfc_port_imm_lkup(unit, ltid, lport, &port_cfg));
    port_cfg.action = action;
    if (port_cfg.opcode != ENTRY_INVALID) {
        SHR_IF_ERR_EXIT
            (bcmtm_oobfc_port_set(unit, ltid, lport, &port_cfg));
        SHR_IF_ERR_EXIT
            (bcmtm_oobfc_port_imm_update(unit, ltid, lport, &port_cfg));
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmtm_imm_oobfc_port_imm_register(int unit)
{
    /*! Inmemory callback functions for TM_OOBFC_PORTt logical table. */
    bcmimm_lt_cb_t  bcmtm_oobfc_port_imm_cb = {
        /*! Staging function. */
        .stage = bcmtm_oobfc_port_stage,
    };
    const bcmlrd_map_t *map = NULL;
    int rv;

    SHR_FUNC_ENTER(unit);
    rv = bcmlrd_map_get(unit, TM_OOBFC_PORTt, &map);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_UNAVAIL);

    if ((rv == SHR_E_UNAVAIL) || (!map)) {
        SHR_EXIT();
    }
    /*! Registers callback functions for OOBFC port settings. */
    SHR_IF_ERR_EXIT
        (bcmimm_lt_event_reg(unit, TM_OOBFC_PORTt,
                             &bcmtm_oobfc_port_imm_cb, NULL));
exit:
    SHR_FUNC_EXIT();
}
