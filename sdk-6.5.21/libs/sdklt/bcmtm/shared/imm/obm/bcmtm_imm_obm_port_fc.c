/*! \file bcmtm_imm_obm_port_fc.c
 *
 * In-memory call back function for OBM mapping  internal prriority to
 * traffic class for all ports.
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
#include <bcmtm/imm/bcmtm_imm_defines_internal.h>
#include <bcmtm/obm/bcmtm_obm_internal.h>

/*******************************************************************************
 * Local definitions
 */
#define BSL_LOG_MODULE  BSL_LS_BCMTM_IMM

/*******************************************************************************
 * Private functions
 */
/*!
 * \brief  Populates obm flow control configuration using user defined values.
 *
 * \param [in] unit Unit number.
 * \param [in] data This is a linked list of the data fields in the
 * modified entry.
 * \param [out] obm_fc_cfg Port flow control configuration populated.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
static void
bcmtm_obm_port_fc_cfg_fill(int unit,
                           const bcmltd_field_t *data,
                           bcmtm_obm_fc_cfg_t *obm_fc_cfg)
{
    bcmltd_field_t *cur_data = (bcmltd_field_t *)data;
    while (cur_data) {
        switch (cur_data->id) {
            case TM_OBM_PORT_FLOW_CTRLt_FLOW_CTRLf:
                obm_fc_cfg->fc = cur_data->data;
                break;
            case TM_OBM_PORT_FLOW_CTRLt_FLOW_CTRL_TYPEf:
                obm_fc_cfg->fc_type = cur_data->data;
                break;
            case TM_OBM_PORT_FLOW_CTRLt_LOSSLESS0_FLOW_CTRLf:
                obm_fc_cfg->lossless0_fc = cur_data->data;
                break;
            case TM_OBM_PORT_FLOW_CTRLt_LOSSLESS1_FLOW_CTRLf:
                obm_fc_cfg->lossless1_fc = cur_data->data;
                break;
            case TM_OBM_PORT_FLOW_CTRLt_COS_BMAP_LOSSLESS0f:
                if (cur_data->data) {
                    SHR_BITSET(obm_fc_cfg->lossless0_cos_bmap, cur_data->idx);
                } else {
                    SHR_BITCLR(obm_fc_cfg->lossless0_cos_bmap, cur_data->idx);
                }
                break;
            case TM_OBM_PORT_FLOW_CTRLt_COS_BMAP_LOSSLESS1f:
                if (cur_data->data) {
                    SHR_BITSET(obm_fc_cfg->lossless1_cos_bmap, cur_data->idx);
                } else {
                    SHR_BITCLR(obm_fc_cfg->lossless1_cos_bmap, cur_data->idx);
                }
                break;
            case TM_OBM_PORT_FLOW_CTRLt_OPERATIONAL_STATEf:
                obm_fc_cfg->opcode = cur_data->data;
                break;
            default:
                break;
        }
        cur_data = cur_data->next;
    }
}

/*!
 * \brief  OBM port flow control physical table configuration
 * based on device type.
 *
 * \param [in] unit  Unit number.
 * \param [in] ltid Logical table ID.
 * \param [in] lport Logical port number.
 * \param [in] obm_fc_cfg OBM flow control configuration to be programmed.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
static int
bcmtm_obm_port_fc_pt_set(int unit,
                         bcmltd_sid_t ltid,
                         bcmtm_lport_t lport,
                         bcmtm_obm_fc_cfg_t *obm_fc_cfg)
{
    bcmtm_drv_t *drv;
    bcmtm_obm_drv_t obm_drv;

    SHR_FUNC_ENTER(unit);

    sal_memset(&obm_drv, 0, sizeof(bcmtm_obm_drv_t));
    SHR_IF_ERR_EXIT(bcmtm_drv_get(unit, &drv));
    SHR_NULL_CHECK(drv->obm_drv_get, SHR_E_UNAVAIL);

    SHR_IF_ERR_EXIT(drv->obm_drv_get(unit, &obm_drv));
    if (obm_drv.fc_set) {
        SHR_IF_ERR_EXIT
            (obm_drv.fc_set(unit, ltid, lport, obm_fc_cfg));
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
 * \param [in] obm_fc_cfg OBM flow control config.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
static int
bcmtm_obm_port_fc_reset(int unit,
                        bcmltd_sid_t ltid,
                        bcmtm_lport_t lport,
                        bcmtm_obm_fc_cfg_t *obm_fc_cfg)
{
    int mport;

    SHR_FUNC_ENTER(unit);

    if (SHR_FAILURE(bcmtm_lport_mmu_local_port_get(unit, lport, &mport))) {
        SHR_EXIT();
    }
    sal_memset(obm_fc_cfg, 0, sizeof(bcmtm_obm_fc_cfg_t));
    SHR_IF_ERR_EXIT
         (bcmtm_obm_port_fc_pt_set(unit, ltid, lport, obm_fc_cfg));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Configures the physical table with user defined values.
 *
 * \param [in] unit Unit number.
 * \param [in] ltid Logical table ID.
 * \param [in] lport Logical port ID.
 * \param [in] obm_fc_cfg OBM flow control configuration data.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
static int
bcmtm_obm_port_fc_set(int unit,
                      bcmltd_sid_t ltid,
                      bcmtm_lport_t lport,
                      bcmtm_obm_fc_cfg_t *obm_fc_cfg)
{
    int mport;
    uint8_t action;

    SHR_FUNC_ENTER(unit);

    if (!bcmtm_obm_port_validation(unit, lport)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    action = obm_fc_cfg->action;
    switch (action) {
        case ACTION_INSERT:
            /*
             * TM local port information.
             * Returns error if local port to physical port mapping
             * is not available.
             */
            if (SHR_FAILURE(bcmtm_lport_mmu_local_port_get(unit, lport, &mport))) {
                obm_fc_cfg->opcode = PORT_INFO_UNAVAIL;
                SHR_EXIT();
            }

            /* Fall through */
        case ACTION_PORT_UPDATE_INT:
            obm_fc_cfg->opcode = VALID;
            SHR_IF_ERR_EXIT
                (bcmtm_obm_port_fc_pt_set(unit, ltid, lport, obm_fc_cfg));
            break;

        case ACTION_PORT_DELETE_INT:
            SHR_IF_ERR_EXIT
                (bcmtm_obm_port_fc_reset(unit, ltid, lport, obm_fc_cfg));
            obm_fc_cfg->opcode = PORT_INFO_UNAVAIL;
            break;

        default:
            break;
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Update the IMM for OBM flow control LT.
 *
 * \param [in] unit Unit number.
 * \param [in] ltid Logical table ID.
 * \param [in] lport Logical port ID.
 * \param [in] obm_fc_cfg OBM flow control configuration data.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
static int
bcmtm_obm_port_fc_imm_update(int unit,
                             bcmltd_sid_t ltid,
                             bcmtm_lport_t lport,
                             bcmtm_obm_fc_cfg_t *obm_fc_cfg)
{
    bcmltd_fields_t in;
    bcmltd_fid_t fid;

    SHR_FUNC_ENTER(unit);

    sal_memset(&in, 0, sizeof(bcmltd_fields_t));
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_alloc(unit, 2, &in));

    in.count = 0;

    /* TM_OBM_PORT_FLOW_CTRLt_PORT_IDf */
    fid = TM_OBM_PORT_FLOW_CTRLt_PORT_IDf;
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_set(unit, &in, fid, 0, lport));

    /* TM_OBM_PORT_FLOW_CTRLt_OPERATIONAL_STATEf */
    fid = TM_OBM_PORT_FLOW_CTRLt_OPERATIONAL_STATEf;
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_set(unit, &in, fid, 0, obm_fc_cfg->opcode));

    SHR_IF_ERR_EXIT
        (bcmimm_entry_update(unit, 0, ltid, &in));

exit:
    bcmtm_field_list_free(&in);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Lookup the IMM for OBM flow control LT.
 *
 * \param [in] unit Unit number.
 * \param [in] ltid Logical table ID.
 * \param [in] lport Logical port ID.
 * \param [in] obm_fc_cfg OBM flow control configuration data.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
static int
bcmtm_obm_port_fc_lkup(int unit,
                       bcmltd_sid_t ltid,
                       bcmtm_lport_t lport,
                       bcmtm_obm_fc_cfg_t *obm_fc_cfg)
{
    bcmltd_fields_t out, in;
    bcmltd_fid_t fid;
    size_t num_fid;

    SHR_FUNC_ENTER(unit);
    sal_memset(&in, 0, sizeof(bcmltd_fields_t));
    sal_memset(&out, 0, sizeof(bcmltd_fields_t));

    SHR_IF_ERR_EXIT
        (bcmlrd_field_count_get(unit, ltid, &num_fid));
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_alloc(unit, num_fid, &out));
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_alloc(unit, 1, &in));
    in.count = 0;


    /* TM_OBM_PORT_FLOW_CTRLt_PORT_IDf */
    fid = TM_OBM_PORT_FLOW_CTRLt_PORT_IDf;
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_set(unit, &in, fid, 0, lport));

    if (SHR_FAILURE(bcmimm_entry_lookup(unit, ltid, &in, &out))) {
        obm_fc_cfg->opcode = ENTRY_INVALID;
    } else {
        bcmtm_obm_port_fc_cfg_fill(unit, *(out.field), obm_fc_cfg);
    }
exit:
    bcmtm_field_list_free(&in);
    bcmtm_field_list_free(&out);
    SHR_FUNC_EXIT();
}

/*!
 * \brief TM_OBM_PORT_FLOW_CTRL logical table callback function for staging.
 *
 * Handle TM_OBM_PORT_FLOW_CTRL table change events.
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
bcmtm_obm_port_fc_stage(int unit,
                        bcmltd_sid_t ltid,
                        uint32_t trans_id,
                        bcmimm_entry_event_t event_reason,
                        const bcmltd_field_t *key,
                        const bcmltd_field_t *data,
                        void *context,
                        bcmltd_fields_t *output_fields)
{
    bcmtm_obm_fc_cfg_t obm_fc_cfg;
    bcmltd_fid_t fid;
    bcmtm_lport_t lport;
    uint64_t fval;

    SHR_FUNC_ENTER(unit);
    sal_memset(&obm_fc_cfg, 0, sizeof(bcmtm_obm_fc_cfg_t));

    /* TM_OBM_PORT_FLOW_CTRLt_PORT_IDf */
    fid = TM_OBM_PORT_FLOW_CTRLt_PORT_IDf;
    SHR_IF_ERR_EXIT
        (bcmtm_fval_get_from_field_array(unit, fid, 0, key, &fval));
    lport = (bcmtm_lport_t)fval;

    switch (event_reason) {
        case BCMIMM_ENTRY_UPDATE:
            SHR_IF_ERR_EXIT
                (bcmtm_obm_port_fc_lkup(unit, ltid, lport, &obm_fc_cfg));

        /* Fall through */
        case BCMIMM_ENTRY_INSERT:
            SHR_NULL_CHECK(output_fields, SHR_E_INTERNAL);
            output_fields->count = 0 ;
            bcmtm_obm_port_fc_cfg_fill(unit, data, &obm_fc_cfg);
            obm_fc_cfg.action = ACTION_INSERT;
            SHR_IF_ERR_EXIT
                (bcmtm_obm_port_fc_set(unit, ltid, lport, &obm_fc_cfg));

            fid = TM_OBM_PORT_FLOW_CTRLt_OPERATIONAL_STATEf;
            SHR_IF_ERR_EXIT
                (bcmtm_field_list_set(unit, output_fields, fid, 0,
                                      obm_fc_cfg.opcode));
            break;
        case BCMIMM_ENTRY_DELETE:
            SHR_IF_ERR_EXIT
                (bcmtm_obm_port_fc_reset(unit, ltid, lport, &obm_fc_cfg));
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
bcmtm_obm_port_fc_internal_update(int unit,
                                  bcmtm_lport_t lport,
                                  uint8_t action)
{
    bcmltd_sid_t ltid = TM_OBM_PORT_FLOW_CTRLt;
    bcmtm_obm_fc_cfg_t obm_fc_cfg;

    SHR_FUNC_ENTER(unit);

    sal_memset(&obm_fc_cfg, 0, sizeof(bcmtm_obm_fc_cfg_t));
    SHR_IF_ERR_EXIT
        (bcmtm_obm_port_fc_lkup(unit, ltid, lport, &obm_fc_cfg));
    obm_fc_cfg.action = action;

    if (obm_fc_cfg.opcode != ENTRY_INVALID) {
        SHR_IF_ERR_EXIT
            (bcmtm_obm_port_fc_set(unit, ltid, lport, &obm_fc_cfg));
        SHR_IF_ERR_EXIT
            (bcmtm_obm_port_fc_imm_update(unit, ltid, lport, &obm_fc_cfg));
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmtm_obm_port_fc_imm_register(int unit)
{
    /*! Inmemory callback functions for TM_OBM_PORT_FLOW_CTRL LT. */
    bcmimm_lt_cb_t bcmtm_obm_port_fc_imm_cb = {
        /*! Staging function. */
        .stage = bcmtm_obm_port_fc_stage,
    };
    const bcmlrd_map_t *map = NULL;
    int rv;

    SHR_FUNC_ENTER(unit);
    rv = bcmlrd_map_get(unit, TM_OBM_PORT_FLOW_CTRLt, &map);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_UNAVAIL);

    if ((rv == SHR_E_UNAVAIL) || (!map)) {
        SHR_EXIT();
    }
    /*! Registers callback functions for obm flow control. */
    SHR_IF_ERR_EXIT
        (bcmimm_lt_event_reg(unit, TM_OBM_PORT_FLOW_CTRLt,
                             &bcmtm_obm_port_fc_imm_cb, NULL));
exit:
    SHR_FUNC_EXIT();
}
