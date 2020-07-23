/*! \file bcmsec_imm_port.c
 *
 * In-memory call back function for port and port macro related IMM LTs.
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
#include <bcmsec/imm/bcmsec_imm_defines_internal.h>
#include <bcmsec/generated/bcmsec_ha.h>
#include <bcmsec/bcmsec_drv.h>
#include <bcmsec/bcmsec_utils.h>
#include <bcmsec/bcmsec_utils_internal.h>

#define BSL_LOG_MODULE  BSL_LS_BCMSEC_PORT
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
bcmsec_pm_control_set(int unit,
                      bcmltd_sid_t ltid,
                      uint32_t pm_id,
                      uint32_t enable,
                      uint8_t *opcode)
{
    bcmsec_drv_t *drv;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT(bcmsec_drv_get(unit, &drv));
    SHR_NULL_CHECK(drv->pm_enable, SHR_E_UNAVAIL);

    /* update physical table. */
    if (drv->pm_enable) {
        SHR_IF_ERR_EXIT
            (drv->pm_enable(unit, ltid, pm_id, enable, opcode));
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief SEC_PC_PM logical table callback function for staging.
 *
 * Handle SEC_PC_PM IMM table change events.
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
bcmsec_pm_control_stage(int unit,
                    bcmltd_sid_t ltid,
                    uint32_t trans_id,
                    bcmimm_entry_event_t event_reason,
                    const bcmltd_field_t *key,
                    const bcmltd_field_t *data,
                    void *context,
                    bcmltd_fields_t *output_fields)
{
    int pm_id;
    bcmltd_fid_t fid;
    uint64_t fval = 0;
    uint32_t pm_enable = 0;
    uint8_t opcode;

    SHR_FUNC_ENTER(unit);

   /* SEC_PC_PMt_PC_PM_IDf*/
    fid = SEC_PC_PMt_PC_PM_IDf;
    SHR_IF_ERR_EXIT
        (bcmsec_fval_get_from_field_array(unit, fid, 0, key, &fval));
    pm_id = fval;
    if (pm_id == 0) {
        /* PM_ID 0 is not expected */
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* Reduce PM_ID by 1 as the range used for calculation starts at 0. */
    pm_id--;

    switch (event_reason) {
        case BCMIMM_ENTRY_UPDATE:
        case BCMIMM_ENTRY_INSERT:
            if (data) {
                fid = SEC_PC_PMt_SECf;
                SHR_IF_ERR_EXIT
                    (bcmsec_fval_get_from_field_array(unit, fid, 0, data, &fval));
                pm_enable = (uint32_t)fval;
            }
            SHR_NULL_CHECK(output_fields, SHR_E_PARAM);
            output_fields->count = 0;
            SHR_IF_ERR_EXIT
                (bcmsec_pm_control_set(unit, ltid, pm_id, pm_enable, &opcode));
            fid = SEC_PC_PMt_OPERATIONAL_STATEf;
            SHR_IF_ERR_EXIT
                (bcmsec_field_list_set(unit, output_fields, fid, 0, opcode));
            break;
        case BCMIMM_ENTRY_DELETE:
            break;
        default:
            break;
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief  Encrypt port control physical table configuration
 * based on device type.
 *
 * \param [in] unit  Unit number.
 * \param [in] ltid Logical table ID.
 * \param [in] Obm port control configuration to be programmed.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
static int
port_control_pt_set(int unit,
            bcmltd_sid_t ltid,
            bcmsec_encrypt_port_control_t *port_control,
            uint32_t *opcode)
{
    bcmsec_drv_t *drv;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT(bcmsec_drv_get(unit, &drv));
    SHR_NULL_CHECK(drv->port_ctrl_set, SHR_E_UNAVAIL);

    /* update physical table. */
    if (drv->port_ctrl_set) {
        SHR_IF_ERR_EXIT
            (drv->port_ctrl_set(unit, ltid, port_control, opcode));
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief  Populates encrypt port configuration using user defined values.
 *
 * \param [in] unit Unit number.
 * \param [in] data This is a linked list of the data fields in the
 * modified entry.
 * \param [out] obm_pc_pm_pkt_parse_cfg  Obm policy configuration populated.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
static void
encrypt_port_control_cfg_fill(int unit,
                           const bcmltd_field_t *data,
                           bcmsec_encrypt_port_control_t *port_control)
{
    bcmltd_field_t *cur_data = (bcmltd_field_t *)data;
    while (cur_data) {
          switch (cur_data->id) {
            case SEC_ENCRYPT_PORTt_PORT_BASED_SCf:
                port_control->port_based_sc = cur_data->data;
                break;
            case SEC_ENCRYPT_PORTt_REMOVE_CRCf:
                port_control->remove_crc = cur_data->data;
                break;
            case SEC_ENCRYPT_PORTt_TX_THRESHOLD_CELLSf:
                port_control->tx_threshold = cur_data->data;
                break;
            default:
                 break;
        }
        cur_data = cur_data->next;
    }
}

/*!
 * \brief
 *
 * \param [in] unit Unit number.
 * \param [in] data This is a linked list of the data fields in the
 * modified entry.
 * \param [out] obm_pc_pm_pkt_parse_cfg  Obm policy configuration populated.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
static int
bcmsec_encrypt_port_control_cfg_lookup (int unit, int lport,
                        bcmsec_encrypt_port_control_t *port_control)
{
    bcmltd_fields_t out, in;
    bcmltd_sid_t ltid = SEC_ENCRYPT_PORTt;
    bcmltd_fid_t fid;
    size_t num_fid;

    SHR_FUNC_ENTER(unit);
    sal_memset(&in, 0, sizeof(bcmltd_fields_t));
    sal_memset(&out, 0, sizeof(bcmltd_fields_t));

    SHR_IF_ERR_EXIT
        (bcmlrd_field_count_get(unit, ltid, &num_fid));
    SHR_IF_ERR_EXIT
        (bcmsec_field_list_alloc(unit, num_fid , &out));
    SHR_IF_ERR_EXIT
        (bcmsec_field_list_alloc(unit, 1, &in));

    fid = SEC_ENCRYPT_PORTt_PORT_IDf;

    SHR_IF_ERR_EXIT
        (bcmsec_field_list_set(unit, &in, fid, 0, lport));
    SHR_IF_ERR_EXIT
        (bcmimm_entry_lookup(unit, ltid, &in, &out));

    encrypt_port_control_cfg_fill(unit, *(out.field), port_control);
exit:
    bcmsec_field_list_free(&in);
    bcmsec_field_list_free(&out);
    SHR_FUNC_EXIT();
}

/*!
 * \brief SEC_ENCRYPT_PORT logical table callback function for staging.
 *
 * Handle SEC_ENCRYPT_PORT IMM table change events.
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
bcmsec_encrypt_port_control_stage(int unit,
                    bcmltd_sid_t ltid,
                    uint32_t trans_id,
                    bcmimm_entry_event_t event_reason,
                    const bcmltd_field_t *key,
                    const bcmltd_field_t *data,
                    void *context,
                    bcmltd_fields_t *output_fields)
{
    int lport, pport;
    bcmltd_fid_t fid;
    uint64_t fval = 0;
    int rv;
    uint32_t opcode = VALID;
    bcmsec_encrypt_port_control_t port_control;

    SHR_FUNC_ENTER(unit);

    /* SEC_ENCRYPT_PORTt_PORT_IDf */
    fid = SEC_ENCRYPT_PORTt_PORT_IDf;
    SHR_IF_ERR_EXIT
        (bcmsec_fval_get_from_field_array(unit, fid, 0, key, &fval));
    lport = fval;
    rv = bcmsec_port_phys_port_get(unit, lport, &pport);
    if (SHR_FAILURE(rv)) {
        LOG_VERBOSE(BSL_LOG_MODULE,(BSL_META_U(unit,
                "Unable to get sec_port for physical port %d.\n"),
                pport));
        opcode = PORT_INFO_UNAVAIL;
    }

    sal_memset(&port_control, 0, sizeof(bcmsec_encrypt_port_control_t));
    if (opcode == VALID) {
        switch (event_reason) {
            case BCMIMM_ENTRY_UPDATE:
                SHR_IF_ERR_EXIT
                    (bcmsec_encrypt_port_control_cfg_lookup(unit, lport, &port_control));
                /* coverity[unterminated_case: FALSE] */
            case BCMIMM_ENTRY_INSERT:
                encrypt_port_control_cfg_fill(unit, data, &port_control);
                /* coverity[unterminated_case: FALSE] */
            case BCMIMM_ENTRY_DELETE:
                port_control.pport = pport;
                SHR_IF_ERR_EXIT
                    (port_control_pt_set(unit, ltid, &port_control, &opcode));
                break;
            default:
                break;
        }
    }
    if (output_fields) {
        output_fields->count = 0;
        fid = SEC_ENCRYPT_PORTt_OPERATIONAL_STATEf;
        SHR_IF_ERR_EXIT
            (bcmsec_field_list_set(unit, output_fields, fid, 0, opcode));
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief  Decrypt port physical table configuration
 * based on device type.
 *
 * \param [in] unit  Unit number.
 * \param [in] lport Logical port number.
 * \param [in] ltid Logical table ID.
 * \param [in] Obm port control configuration to be programmed.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
static int
bcmsec_decrypt_control_pt_set(int unit, bcmltd_sid_t ltid,
                              int pport, bcmltd_field_t *in,
                              uint32_t *opcode)
{
    bcmsec_drv_t *drv;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT(bcmsec_drv_get(unit, &drv));
    SHR_NULL_CHECK(drv->decrypt_port_ctrl_set, SHR_E_UNAVAIL);

    /* update physical table. */
    if (drv->decrypt_port_ctrl_set) {
        SHR_IF_ERR_EXIT
            (drv->decrypt_port_ctrl_set(unit, ltid, pport, in, opcode));
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief SEC_DECRYPT_PORT logical table callback function for staging.
 *
 * Handle SEC_DECRYPT_PORT IMM table change events.
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
bcmsec_decrypt_port_control_stage(int unit,
                    bcmltd_sid_t ltid,
                    uint32_t trans_id,
                    bcmimm_entry_event_t event_reason,
                    const bcmltd_field_t *key,
                    const bcmltd_field_t *data,
                    void *context,
                    bcmltd_fields_t *output_fields)
{
    int lport, pport;
    bcmltd_fid_t fid;
    uint64_t fval = 0;
    int rv;
    uint32_t opcode = VALID;
    bcmltd_field_t *entry_data;
    bcmltd_fields_t out, in;
    size_t num_fids;

    SHR_FUNC_ENTER(unit);

    /* SEC_DECRYPT_PORTt_PORT_IDf */
    fid = SEC_DECRYPT_PORTt_PORT_IDf;
    SHR_IF_ERR_EXIT
        (bcmsec_fval_get_from_field_array(unit, fid, 0, key, &fval));
    lport = fval;
    rv = bcmsec_port_phys_port_get(unit, lport, &pport);
    if (SHR_FAILURE(rv)) {
        LOG_VERBOSE(BSL_LOG_MODULE,(BSL_META_U(unit,
                "Unable to get sec_port for physical port %d.\n"),
                pport));
        opcode = PORT_INFO_UNAVAIL;
    }

    entry_data = (bcmltd_field_t *)data;
    if (opcode == VALID) {
        switch (event_reason) {
            case BCMIMM_ENTRY_UPDATE:
                sal_memset(&in, 0, sizeof(bcmltd_fields_t));
                sal_memset(&out, 0, sizeof(bcmltd_fields_t));

                SHR_IF_ERR_EXIT(bcmlrd_field_count_get(unit, ltid, &num_fids));

                num_fids += bcmlrd_field_idx_count_get(unit, SEC_DECRYPT_PORTt,
                                            SEC_DECRYPT_PORTt_MGMT_DST_MACf);
                num_fids += bcmlrd_field_idx_count_get(unit, SEC_DECRYPT_PORTt,
                                            SEC_DECRYPT_PORTt_MGMT_ETHERTYPEf);
                num_fids += bcmlrd_field_idx_count_get(unit, SEC_DECRYPT_PORTt,
                                            SEC_DECRYPT_PORTt_MPLS_ETHERTYPEf);
                num_fids += bcmlrd_field_idx_count_get(unit, SEC_DECRYPT_PORTt,
                                            SEC_DECRYPT_PORTt_TPIDf);
                SHR_IF_ERR_EXIT
                    (bcmsec_field_list_alloc(unit, (num_fids - 4),
                                             &out));
                SHR_IF_ERR_EXIT
                    (bcmsec_field_list_alloc(unit, 1, &in));
                out.count = 0;
                SHR_IF_ERR_EXIT
                    (sec_fields_update_and_link(unit, (bcmltd_field_t *)data,
                                                &out));
                if (out.count) {
                    entry_data = out.field[0];
                } else {
                    entry_data = NULL;
                }
            case BCMIMM_ENTRY_INSERT:
            case BCMIMM_ENTRY_DELETE:
                SHR_IF_ERR_EXIT
                    (bcmsec_decrypt_control_pt_set(unit, ltid, pport, entry_data, &opcode));
                break;
            default:
                break;
        }
    }
    if (output_fields) {
        output_fields->count = 0;
        fid = SEC_DECRYPT_PORTt_OPERATIONAL_STATEf;
        SHR_IF_ERR_EXIT
            (bcmsec_field_list_set(unit, output_fields, fid, 0, opcode));
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief
 *
 * \param [in] unit Unit number.
 * \param [in] data This is a linked list of the data fields in the
 * modified entry.
 * \param [out] obm_pc_pm_pkt_parse_cfg  Obm policy configuration populated.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
static int
bcmsec_port_imm_update (int unit, bcmltd_sid_t ltid, int lport,
                        int opcode, int encrypt)
{
    bcmltd_fields_t in;
    bcmltd_fid_t fid;

    SHR_FUNC_ENTER(unit);
    sal_memset(&in, 0, sizeof(bcmltd_fields_t));

    SHR_IF_ERR_EXIT
        (bcmsec_field_list_alloc(unit, 2, &in));

    in.count = 0;
    fid = encrypt ? SEC_ENCRYPT_PORTt_PORT_IDf : SEC_DECRYPT_PORTt_PORT_IDf;
    SHR_IF_ERR_EXIT
        (bcmsec_field_list_set(unit, &in, fid, 0, lport));

    fid = encrypt ? SEC_ENCRYPT_PORTt_OPERATIONAL_STATEf :
                    SEC_DECRYPT_PORTt_OPERATIONAL_STATEf;
    SHR_IF_ERR_EXIT
        (bcmsec_field_list_set(unit, &in, fid, 0, opcode));

    SHR_IF_ERR_EXIT
        (bcmimm_entry_update(unit, 0, ltid, &in));
exit:
    bcmsec_field_list_free(&in);
    SHR_FUNC_EXIT();
}

static int
bcmsec_encrypt_port_internal_update (int unit, int lport,
                            bcmsec_port_internal_update_t port_op)
{
    bcmsec_encrypt_port_control_t port_control;
    bcmltd_sid_t ltid = SEC_ENCRYPT_PORTt;
    int pport, rv;
    uint32_t opcode, opcode1;

    SHR_FUNC_ENTER(unit);
    sal_memset(&port_control, 0, sizeof(bcmsec_encrypt_port_control_t));

    SHR_IF_ERR_EXIT
        (bcmsec_port_phys_port_get(unit, lport, &pport));

    rv = bcmsec_encrypt_port_control_cfg_lookup(unit, lport, &port_control);
    if (rv == SHR_E_NOT_FOUND) {
        SHR_EXIT();
    }

    if (port_op == ACTION_PORT_ADD_INT) {
        opcode = VALID;
        LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "Port ADD for port %d. "
                            "Update Encrypt port control.\n"),
                    lport));
    } else {
        opcode = PORT_INFO_UNAVAIL;
        sal_memset(&port_control, 0, sizeof(bcmsec_encrypt_port_control_t));
        LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "Port DELETE for port %d. "
                            "Update Encrypt port control.\n"),
                    lport));
    }

    port_control.pport = pport;
    SHR_IF_ERR_EXIT
        (port_control_pt_set(unit, ltid, &port_control, &opcode1));

    if (opcode1 == PORT_INFO_UNAVAIL) {
        opcode = PORT_INFO_UNAVAIL;
    }
    SHR_IF_ERR_EXIT
        (bcmsec_port_imm_update(unit, ltid, lport, opcode, 1));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief
 *
 * \param [in] unit Unit number.
 * \param [in] lport Logical port
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Error code for corresponding failures.
 */
static int
bcmsec_decrypt_port_lookup_and_update (int unit, int lport, uint32_t *opcode)
{
    bcmltd_fields_t out, in;
    bcmltd_sid_t ltid = SEC_DECRYPT_PORTt;
    bcmltd_fid_t fid;
    size_t num_fids;
    bcmltd_field_t *entry_data;
    int pport;

    SHR_FUNC_ENTER(unit);
    sal_memset(&in, 0, sizeof(bcmltd_fields_t));
    sal_memset(&out, 0, sizeof(bcmltd_fields_t));

    SHR_IF_ERR_EXIT
        (bcmsec_port_phys_port_get(unit, lport, &pport));
    SHR_IF_ERR_EXIT
        (bcmlrd_field_count_get(unit, ltid, &num_fids));
    num_fids += bcmlrd_field_idx_count_get(unit, SEC_DECRYPT_PORTt,
                                SEC_DECRYPT_PORTt_MGMT_DST_MACf);
    num_fids += bcmlrd_field_idx_count_get(unit, SEC_DECRYPT_PORTt,
                                SEC_DECRYPT_PORTt_MGMT_ETHERTYPEf);
    num_fids += bcmlrd_field_idx_count_get(unit, SEC_DECRYPT_PORTt,
                                SEC_DECRYPT_PORTt_MPLS_ETHERTYPEf);
    num_fids += bcmlrd_field_idx_count_get(unit, SEC_DECRYPT_PORTt,
                                SEC_DECRYPT_PORTt_TPIDf);
    SHR_IF_ERR_EXIT
        (bcmsec_field_list_alloc(unit, (num_fids - 4),
                                    &out));
    SHR_IF_ERR_EXIT
        (bcmsec_field_list_alloc(unit, 1, &in));

    fid = SEC_DECRYPT_PORTt_PORT_IDf;

    SHR_IF_ERR_EXIT
        (bcmsec_field_list_set(unit, &in, fid, 0, lport));
    SHR_IF_ERR_EXIT
        (bcmimm_entry_lookup(unit, ltid, &in, &out));

    if ((*opcode == VALID) && (out.count)) {
        entry_data = out.field[0];
    } else {
        entry_data = NULL;
    }
    SHR_IF_ERR_EXIT
        (bcmsec_decrypt_control_pt_set(unit, ltid, pport, entry_data, opcode));

exit:
    bcmsec_field_list_free(&in);
    bcmsec_field_list_free(&out);
    SHR_FUNC_EXIT();
}

static int
bcmsec_decrypt_port_internal_update (int unit, int lport,
                            bcmsec_port_internal_update_t port_op)
{
    bcmltd_sid_t ltid = SEC_DECRYPT_PORTt;
    uint32_t opcode;
    int rv;

    SHR_FUNC_ENTER(unit);

    if (port_op == ACTION_PORT_ADD_INT) {
        opcode = VALID;
        LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "Port ADD for port %d. "
                            "Update decrypt port control.\n"),
                    lport));
    } else {
        opcode = PORT_INFO_UNAVAIL;
        LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "Port DELETE for port %d. "
                            "Update decrypt port control.\n"),
                    lport));
    }

    rv = bcmsec_decrypt_port_lookup_and_update(unit, lport, &opcode);
    if (rv == SHR_E_NOT_FOUND) {
        SHR_ERR_EXIT(SHR_E_NONE);
    }


    SHR_IF_ERR_EXIT
        (bcmsec_port_imm_update(unit, ltid, lport, opcode, 0));
exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public functions
 */
int
bcmsec_pm_control_imm_register(int unit)
{
    /*! Inmemory callback functions for SEC_PC_PMt logical table. */
    bcmimm_lt_cb_t bcmsec_pm_control_imm_cb = {
        /*! Staging function. */
        .stage = bcmsec_pm_control_stage,
    };

    /*!
     * Inmemory callback functions for SEC_ENCRYPT_PORTt
     * logical table.
     */
    bcmimm_lt_cb_t bcmsec_encrypt_port_control_imm_cb = {
        /*! Staging function. */
        .stage = bcmsec_encrypt_port_control_stage,
    };

    /*!
     * Inmemory callback functions for SEC_DECRYPT_PORTt
     * logical table.
     */
    bcmimm_lt_cb_t bcmsec_decrypt_port_control_imm_cb = {
        /*! Staging function. */
        .stage = bcmsec_decrypt_port_control_stage,
    };
    const bcmlrd_map_t *map = NULL;
    int rv;

    SHR_FUNC_ENTER(unit);

    /* SEC_PC_PMt */
    rv = bcmlrd_map_get(unit, SEC_PC_PMt, &map);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_UNAVAIL);

    if ((rv == SHR_E_UNAVAIL) || (!map)) {
        SHR_EXIT();
    }
    /*! Registers callback functions for cut-through settings. */
    SHR_IF_ERR_EXIT
        (bcmimm_lt_event_reg(unit, SEC_PC_PMt,
                             &bcmsec_pm_control_imm_cb, NULL));

    /* SEC_ENCRYPT_PORTt */
    rv = bcmlrd_map_get(unit, SEC_ENCRYPT_PORTt, &map);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_UNAVAIL);

    if ((rv == SHR_E_UNAVAIL) || (!map)) {
        SHR_ERR_EXIT(SHR_E_NONE);
    }
    /*! Registers callback functions for cut-through settings. */
    SHR_IF_ERR_EXIT
        (bcmimm_lt_event_reg(unit, SEC_ENCRYPT_PORTt,
                             &bcmsec_encrypt_port_control_imm_cb, NULL));

    /* SEC_DECRYPT_PORTt */
    rv = bcmlrd_map_get(unit, SEC_DECRYPT_PORTt, &map);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_UNAVAIL);

    if ((rv == SHR_E_UNAVAIL) || (!map)) {
        SHR_ERR_EXIT(SHR_E_NONE);
    }
    /*! Registers callback functions for decrypt port settings. */
    SHR_IF_ERR_EXIT
        (bcmimm_lt_event_reg(unit, SEC_DECRYPT_PORTt,
                             &bcmsec_decrypt_port_control_imm_cb, NULL));
exit:
    SHR_FUNC_EXIT();
}


int
bcmsec_port_control_internal_update (int unit, int lport,
                            bcmsec_port_internal_update_t port_op)
{
    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_EXIT
        (bcmsec_encrypt_port_internal_update(unit, lport, port_op));
    SHR_IF_ERR_EXIT
        (bcmsec_decrypt_port_internal_update(unit, lport, port_op));
exit:
    SHR_FUNC_EXIT();
}
