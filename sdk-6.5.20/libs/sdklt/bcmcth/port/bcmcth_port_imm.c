/*! \file bcmcth_port_imm.c
 *
 * BCMPORT interface to in-memory table.
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
#include <bcmlrd/bcmlrd_table.h>
#include <bcmlrd/bcmlrd_client.h>
#include <bcmcth/bcmcth_port_drv.h>
#include <bcmcth/bcmcth_imm_util.h>

/*******************************************************************************
 * Local definitions
 */

/* BSL Module */
#define BSL_LOG_MODULE BSL_LS_BCMCTH_PORT

/*! PORT fields array lmm handler. */
static shr_famm_hdl_t port_fld_arr_hdl;

/*******************************************************************************
 * Private functions
 */
/*!
 * \brief imm PORT_ING_MIRRORt notification input fields parsing.
 *
 * Parse imm PORT_ING_MIRRORt input fields.
 *
 * \param [in] unit Unit number.
 * \param [in] key IMM input key field array.
 * \param [in] data IMM input data field array.
 * \param [out] imirror Port ingress mirror data buffer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Faild to convert \c key \c data to \c ltcfg.
 */
static int
port_imirror_lt_fields_parse(int unit,
                             const bcmltd_field_t *key,
                             const bcmltd_field_t *data,
                             port_imirror_t *imirror)
{
    const bcmltd_field_t *gen_field;
    uint32_t fid;
    uint64_t fval;

    SHR_FUNC_ENTER(unit);

    sal_memset(imirror, 0, sizeof(*imirror));

    /* Parse key field */
    gen_field = key;
    while (gen_field) {
        fid = gen_field->id;
        fval = gen_field->data;

        switch (fid) {
        case PORT_ING_MIRRORt_PORT_IDf:
            imirror->port = fval;
            SHR_BITSET(imirror->fbmp, fid);
            break;
        case PORT_ING_MIRRORt_MIRROR_CONTAINER_IDf:
            imirror->container = fval;
            SHR_BITSET(imirror->fbmp, fid);
            break;
        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
        gen_field = gen_field->next;
    }

    /* Parse data field */
    gen_field = data;
    while (gen_field) {
        fid = gen_field->id;
        fval = gen_field->data;

        switch (fid) {
        case PORT_ING_MIRRORt_MIRRORf:
            imirror->enable = fval;
            SHR_BITSET(imirror->fbmp, fid);
            break;
        case PORT_ING_MIRRORt_MIRROR_INSTANCE_IDf:
            imirror->instance_id = fval;
            SHR_BITSET(imirror->fbmp, fid);
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
 * \brief PORT_ING_MIRROR IMM table change callback function for staging.
 *
 * Handle PORT_ING_MIRROR IMM table change events.
 *
 * \param [in] unit Unit number.
 * \param [in] sid This is the logical table ID.
 * \param [in] op_arg Operation arguments.
 * \param [in] event_reason This is the reason for the entry event.
 * \param [in] key This is a linked list of the key fields identifying
 * the entry.
 * \param [in] data This is a linked list of the data fields in the
 * modified entry.
 * \param [in] context Is a pointer that was given during registration.
 * The callback can use this pointer to retrieve some context.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Fails to handle LT change events.
 */
static int
port_ing_mirror_imm_stage_callback(int unit,
                                   bcmltd_sid_t sid,
                                   const bcmltd_op_arg_t *op_arg,
                                   bcmimm_entry_event_t event_reason,
                                   const bcmltd_field_t *key,
                                   const bcmltd_field_t *data,
                                   void *context,
                                   bcmltd_fields_t *output_fields)
{
    port_imirror_t cfg;
    bool m_en = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (port_imirror_lt_fields_parse(unit, key, data, &cfg));

    if (output_fields) {
        output_fields->count = 0;
    }

    switch (event_reason) {
    case BCMIMM_ENTRY_INSERT:
    case BCMIMM_ENTRY_UPDATE:
        if (!SHR_BITGET(cfg.fbmp, PORT_ING_MIRRORt_MIRRORf)) {
            SHR_EXIT();
        }
        m_en = cfg.enable;
        break;
    case BCMIMM_ENTRY_DELETE:
        break;
    default:
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_port_ingress_mirror_set(unit, op_arg, sid, &cfg, m_en));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief PORT_ING_MIRROR In-memory event callback structure.
 *
 * This structure contains callback functions that will be corresponding
 * to PORT_ING_MIRROR logical table entry commit stages.
 */
static bcmimm_lt_cb_t port_ing_mirror_imm_callback = {

    /*! Validate function. */
    .validate = NULL,

    /*! Extended staging function. */
    .op_stage = port_ing_mirror_imm_stage_callback,

    /*! Commit function. */
    .commit = NULL,

    /*! Abort function. */
    .abort = NULL
};

/*!
 * \brief imm PORT_ENG_MIRRORt notification input fields parsing.
 *
 * Parse imm PORT_EGR_MIRRORt input fields.
 *
 * \param [in] unit Unit number.
 * \param [in] key IMM input key field array.
 * \param [in] data IMM input data field array.
 * \param [out] emirror Port egress mirror info.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Faild to convert \c key \c data to \c ltcfg.
 */
static int
port_emirror_lt_fields_parse(int unit,
                             const bcmltd_field_t *key,
                             const bcmltd_field_t *data,
                             port_emirror_t *emirror)
{
    const bcmltd_field_t *gen_field;
    uint32_t fid;
    uint64_t fval;

    SHR_FUNC_ENTER(unit);

    sal_memset(emirror, 0, sizeof(*emirror));

    /* Parse key field */
    gen_field = key;
    while (gen_field) {
        fid = gen_field->id;
        fval = gen_field->data;

        switch (fid) {
        case PORT_EGR_MIRRORt_PORT_IDf:
            emirror->port = fval;
            SHR_BITSET(emirror->fbmp, fid);
            break;
        case PORT_EGR_MIRRORt_EGR_PORT_IDf:
            emirror->egr_port = fval;
            SHR_BITSET(emirror->fbmp, fid);
            break;
        case PORT_EGR_MIRRORt_MIRROR_CONTAINER_IDf:
            emirror->container = fval;
            SHR_BITSET(emirror->fbmp, fid);
            break;
        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
        gen_field = gen_field->next;
    }

    /* Parse data field */
    gen_field = data;
    while (gen_field) {
        fid = gen_field->id;
        fval = gen_field->data;

        switch (fid) {
        case PORT_EGR_MIRRORt_MIRRORf:
            emirror->enable = fval;
            SHR_BITSET(emirror->fbmp, fid);
            break;
        case PORT_EGR_MIRRORt_MIRROR_SESSION_IDf:
            emirror->session_id = fval;
            SHR_BITSET(emirror->fbmp, fid);
            break;
        case PORT_EGR_MIRRORt_MIRROR_INSTANCE_IDf:
            emirror->instance_id = fval;
            SHR_BITSET(emirror->fbmp, fid);
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
 * \brief PORT_EGR_MIRROR IMM table change callback function for staging.
 *
 * Handle PORT_EGR_MIRROR IMM logical table change events.
 *
 * \param [in] unit Unit number.
 * \param [in] sid This is the logical table ID.
 * \param [in] op_arg Operation arguments.
 * \param [in] event_reason This is the reason for the entry event.
 * \param [in] key This is a linked list of the key fields identifying
 * the entry.
 * \param [in] data This is a linked list of the data fields in the
 * modified entry.
 * \param [in] context Is a pointer that was given during registration.
 * The callback can use this pointer to retrieve some context.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Fails to handle LT change events.
 */
static int
port_egr_mirror_imm_stage_callback(int unit,
                                   bcmltd_sid_t sid,
                                   const bcmltd_op_arg_t *op_arg,
                                   bcmimm_entry_event_t event_reason,
                                   const bcmltd_field_t *key,
                                   const bcmltd_field_t *data,
                                   void *context,
                                   bcmltd_fields_t *output_fields)
{
    port_emirror_t cfg;
    bool m_en = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (port_emirror_lt_fields_parse(unit, key, data, &cfg));

    if (output_fields) {
        output_fields->count = 0;
    }

    switch (event_reason) {
    case BCMIMM_ENTRY_INSERT:
    case BCMIMM_ENTRY_UPDATE:
        if (!SHR_BITGET(cfg.fbmp, PORT_EGR_MIRRORt_MIRRORf)) {
            SHR_EXIT();
        }
        m_en = cfg.enable;
        break;
    case BCMIMM_ENTRY_DELETE:
        break;
    default:
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_port_egress_mirror_set(unit, op_arg, sid, &cfg, m_en));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief PORT_EGR_MIRROR In-memory event callback structure.
 *
 * This structure contains callback functions that will be corresponding
 * to PORT_EGR_MIRROR logical table entry commit stages.
 */
static bcmimm_lt_cb_t port_egr_mirror_imm_callback = {

    /*! Validate function. */
    .validate = NULL,

    /*! Extended staging function. */
    .op_stage = port_egr_mirror_imm_stage_callback,

    /*! Commit function. */
    .commit = NULL,

    /*! Abort function. */
    .abort = NULL
};

/*!
 * \brief imm PORT_MIRRORt notification input fields parsing.
 *
 * Parse imm PORT_MIRRORt input fields.
 *
 * \param [in] unit Unit number.
 * \param [in] key IMM input key field array.
 * \param [in] data IMM input data field array.
 * \param [out] mirror Port egress mirror info.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Faild to convert \c key \c data to \c ltcfg.
 */
static int
port_mirror_lt_fields_parse(
    int unit,
    const bcmltd_field_t *key,
    const bcmltd_field_t *data,
    port_mirror_t *mirror)
{
    const bcmltd_field_t *gen_field;
    uint32_t fid;
    uint64_t fval;

    SHR_FUNC_ENTER(unit);

    sal_memset(mirror, 0, sizeof(*mirror));

    /* Parse key field */
    gen_field = key;
    while (gen_field) {
        fid = gen_field->id;
        fval = gen_field->data;

        switch (fid) {
        case PORT_MIRRORt_PORT_IDf:
            mirror->port = fval;
            break;
        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
        gen_field = gen_field->next;
    }

    /* Parse data field */
    gen_field = data;
    while (gen_field) {
        fid = gen_field->id;
        fval = gen_field->data;

        switch (fid) {
        case PORT_MIRRORt_CPUf:
            mirror->cpu = fval;
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
 * \brief Do the internal hardware configuration for UPDATE operation.
 *
 * \param [in] unit Unit number.
 * \param [in] op_arg Operation arguments.
 * \param [in] sid This is the logical table ID.
 * \param [in] entry Port mirror actions to update.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
port_mirror_lt_update(
    int unit,
    const bcmltd_op_arg_t *op_arg,
    bcmltd_sid_t sid,
    port_mirror_t *entry)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_port_mirror_set(unit, op_arg, sid,
                                BCMCTH_PORT_MIRROR_OP_SET, entry));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Do the internal hardware configuration for DELETE operation.
 *
 * \param [in] unit Unit number.
 * \param [in] op_arg Operation arguments.
 * \param [in] sid This is the logical table ID.
 * \param [in] entry Port mirror actions to update.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
port_mirror_lt_delete(
    int unit,
    const bcmltd_op_arg_t *op_arg,
    bcmltd_sid_t sid,
    port_mirror_t *entry)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_port_mirror_set(unit, op_arg, sid,
                                BCMCTH_PORT_MIRROR_OP_DEL, entry));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief PORT_MIRROR IMM table change callback function for staging.
 *
 * Handle PORT_MIRROR IMM logical table change events.
 *
 * \param [in] unit Unit number.
 * \param [in] sid This is the logical table ID.
 * \param [in] op_arg Operation arguments.
 * \param [in] event_reason This is the reason for the entry event.
 * \param [in] key This is a linked list of the key fields identifying
 * the entry.
 * \param [in] data This is a linked list of the data fields in the
 * modified entry.
 * \param [in] context Is a pointer that was given during registration.
 * The callback can use this pointer to retrieve some context.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Fails to handle LT change events.
 */
static int
port_mirror_imm_stage_callback(
    int unit,
    bcmltd_sid_t sid,
    const bcmltd_op_arg_t *op_arg,
    bcmimm_entry_event_t event_reason,
    const bcmltd_field_t *key,
    const bcmltd_field_t *data,
    void *context,
    bcmltd_fields_t *output_fields)
{
    port_mirror_t entry;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (port_mirror_lt_fields_parse(unit, key, data, &entry));

    if (output_fields) {
        output_fields->count = 0;
    }

    switch (event_reason) {
    case BCMIMM_ENTRY_INSERT:
    case BCMIMM_ENTRY_UPDATE:
        SHR_IF_ERR_VERBOSE_EXIT
            (port_mirror_lt_update(unit, op_arg, sid, &entry));
        break;
    case BCMIMM_ENTRY_DELETE:
        SHR_IF_ERR_VERBOSE_EXIT
            (port_mirror_lt_delete(unit, op_arg, sid, &entry));
        break;
    default:
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief PORT_MIRROR In-memory event callback structure.
 *
 * This structure contains callback functions that will be corresponding
 * to PORT_MIRROR logical table entry commit stages.
 */
static bcmimm_lt_cb_t port_mirror_imm_callback = {

    /*! Validate function. */
    .validate = NULL,

    /*! Extended staging function. */
    .op_stage = port_mirror_imm_stage_callback,

    /*! Commit function. */
    .commit = NULL,

    /*! Abort function. */
    .abort = NULL
};

/*!
 * \brief imm PORT_BRIDGEt notification input fields parsing.
 *
 * Parse imm PORT_BRIDGEt input fields.
 *
 * \param [in] unit Unit number.
 * \param [in] key IMM input key field array.
 * \param [in] data IMM input data field array.
 * \param [out] imirror Port ingress mirror data buffer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Faild to convert \c key \c data to \c lt_info.
 */
static int
port_bridge_lt_fields_parse(int unit,
                            const bcmltd_field_t *key,
                            const bcmltd_field_t *data,
                            port_bridge_info_t *lt_info)
{
    const bcmltd_field_t *gen_field;
    uint32_t fid;
    uint64_t fval;

    SHR_FUNC_ENTER(unit);

    sal_memset(lt_info, 0, sizeof(port_bridge_info_t));

    /* Parse key field */
    gen_field = key;
    while (gen_field) {
        fid = gen_field->id;
        fval = gen_field->data;

        switch (fid) {
        case PORT_BRIDGEt_PORT_IDf:
            lt_info->member_bmp |= MEMBER_BMP_PORT_ID;
            lt_info->port        = fval;
            break;
        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
        gen_field = gen_field->next;
    }

    /* Parse data field */
    gen_field = data;
    while (gen_field) {
        fid = gen_field->id;
        fval = gen_field->data;

        switch (fid) {
        case PORT_BRIDGEt_BRIDGEf:
            lt_info->member_bmp |= MEMBER_BMP_BRIDGE;
            lt_info->bridge      = fval;
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
 * \brief PORT_BRIDGE IMM table change callback function for staging.
 *
 * Handle PORT_BRIDGE IMM table change events.
 *
 * \param [in] unit Unit number.
 * \param [in] sid This is the logical table ID.
 * \param [in] op_arg The operation arguments.
 * \param [in] event_reason This is the reason for the entry event.
 * \param [in] key This is a linked list of the key fields identifying
 * the entry.
 * \param [in] data This is a linked list of the data fields in the
 * modified entry.
 * \param [in] context Is a pointer that was given during registration.
 * The callback can use this pointer to retrieve some context.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Fails to handle LT change events.
 */
static int
port_bridge_imm_callback_extended_stage(int unit,
                                        bcmltd_sid_t sid,
                                        const bcmltd_op_arg_t *op_arg,
                                        bcmimm_entry_event_t event_reason,
                                        const bcmltd_field_t *key,
                                        const bcmltd_field_t *data,
                                        void *context,
                                        bcmltd_fields_t *output_fields)
{
    port_bridge_info_t lt_info;
    bool bridge = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (port_bridge_lt_fields_parse(unit, key, data, &lt_info));

    if (output_fields) {
        output_fields->count = 0;
    }

    switch (event_reason) {
    case BCMIMM_ENTRY_INSERT:
    case BCMIMM_ENTRY_UPDATE:
        if (!(lt_info.member_bmp & MEMBER_BMP_BRIDGE)) {
            SHR_EXIT();
        }
        bridge = lt_info.bridge;
        break;
    case BCMIMM_ENTRY_DELETE:
        bridge = 0;
        break;
    default:
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT(
        bcmcth_port_bridge_set(unit,
                               op_arg,
                               sid,
                               lt_info.port,
                               bridge));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Lookup an entry from the mpls protect Table.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  sid             Logical table ID.
 * \param [in]  op_arg          The operation arguments.
 * \param [in]  context         Pointer to retrieve some context.
 * \param [in]  lkup_type       Lookup type.
 * \param [in]  In              Input key fields list.
 * \param [in/out] out          Output parameter list.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
static int
port_bridge_imm_callback_lookup(int unit,
                                bcmltd_sid_t sid,
                                const bcmltd_op_arg_t *op_arg,
                                void *context,
                                bcmimm_lookup_type_t lkup_type,
                                const bcmltd_fields_t *in,
                                bcmltd_fields_t *out)
{
    shr_port_t port_id;
    bool bridge = 0;
    const bcmltd_fields_t *key;
    uint32_t fid = 0;
    uint64_t fval = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(op_arg, SHR_E_PARAM);

    if (!(op_arg->attrib & BCMLT_ENT_ATTR_GET_FROM_HW)) {
        SHR_EXIT();
    }

    key = (lkup_type == BCMIMM_TRAVERSE) ? out : in;
    fid = PORT_BRIDGEt_PORT_IDf;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_imm_fields_get(unit, key, fid, &fval));
    port_id = fval;

    SHR_IF_ERR_VERBOSE_EXIT(
        bcmcth_port_bridge_get(unit,
                               op_arg,
                               sid,
                               port_id,
                               &bridge));

    SHR_IF_ERR_EXIT
        (bcmcth_imm_field_update(unit,
                                out,
                                PORT_BRIDGEt_BRIDGEf,
                                bridge));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief PORT_BRIDGE In-memory event callback structure.
 *
 * This structure contains callback functions that will be corresponding
 * to PORT_BRIDGE logical table entry commit stages.
 */
static bcmimm_lt_cb_t port_bridge_imm_callback = {

    /*! Validate function. */
    .validate = NULL,

    /*! Extended staging function. */
    .op_stage = port_bridge_imm_callback_extended_stage,

    /*! Extended lookup function. */
    .op_lookup = port_bridge_imm_callback_lookup,

    /*! Commit function. */
    .commit = NULL,

    /*! Abort function. */
    .abort = NULL
};

/*!
 * \brief imm PORT_MEMBERSHIP_POLICYt notification input fields parsing.
 *
 * Parse imm PORT_MEMBERSHIP_POLICYt input fields.
 *
 * \param [in] unit Unit number.
 * \param [in] key IMM input key field array.
 * \param [in] data IMM input data field array.
 * \param [out] imirror Port ingress mirror data buffer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Faild to convert \c key \c data to \c lt_info.
 */
static int
port_membership_policy_lt_fields_parse(int unit,
                                       const bcmltd_field_t *key,
                                       const bcmltd_field_t *data,
                                       port_membership_policy_info_t *lt_info)
{
    const bcmltd_field_t *gen_field;
    uint32_t fid;
    uint64_t fval;

    SHR_FUNC_ENTER(unit);

    sal_memset(lt_info, 0, sizeof(port_membership_policy_info_t));

    /* Parse key field */
    gen_field = key;
    while (gen_field) {
        fid = gen_field->id;
        fval = gen_field->data;

        switch (fid) {
            case PORT_MEMBERSHIP_POLICYt_PORT_IDf:
                BCMPORT_LT_FIELD_SET(
                    lt_info->fbmp, PORT_ID);
                lt_info->port_id = (bcmport_id_t) fval;
                break;

        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
        gen_field = gen_field->next;
    }

    /* Parse data field */
    gen_field = data;
    while (gen_field) {
        fid = gen_field->id;
        fval = gen_field->data;

        switch (fid) {
            case PORT_MEMBERSHIP_POLICYt_ING_VLAN_MEMBERSHIP_CHECKf:
                BCMPORT_LT_FIELD_SET(
                    lt_info->fbmp, ING_VLAN_MEMBERSHIP_CHECK);
                lt_info->ing_vlan_membership_check = (bool) fval;
                break;
            case PORT_MEMBERSHIP_POLICYt_EGR_VLAN_MEMBERSHIP_CHECKf:
                BCMPORT_LT_FIELD_SET(
                    lt_info->fbmp, EGR_VLAN_MEMBERSHIP_CHECK);
                lt_info->egr_vlan_membership_check = (bool) fval;
                break;
            case PORT_MEMBERSHIP_POLICYt_SKIP_VLAN_CHECKf:
                BCMPORT_LT_FIELD_SET(
                    lt_info->fbmp, SKIP_VLAN_CHECK);
                lt_info->skip_vlan_check = (bool) fval;
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
 * \brief Init all fields value as SW default value.
 *
 * This initialization is only used for Insert operation.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_entry PORT_POLICY LT entry data buffer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Input parameter failed validation check.
 */
static int
port_membership_policy_info_default_values_init(int unit,
                                                port_membership_policy_info_t *lt_info)
{
    uint64_t def_val = 0;
    uint32_t num;
    SHR_FUNC_ENTER(unit);

    /* validate input parameter. */
    SHR_NULL_CHECK(lt_info, SHR_E_PARAM);

    if (!BCMPORT_LT_FIELD_GET(
             lt_info->fbmp, ING_VLAN_MEMBERSHIP_CHECK)) {
        SHR_IF_ERR_EXIT
            (bcmlrd_field_default_get(unit,
                                      PORT_MEMBERSHIP_POLICYt,
                                      PORT_MEMBERSHIP_POLICYt_ING_VLAN_MEMBERSHIP_CHECKf,
                                      1, &def_val, &num));
        lt_info->ing_vlan_membership_check = def_val ? true : false;
        BCMPORT_LT_FIELD_SET(lt_info->fbmp, ING_VLAN_MEMBERSHIP_CHECK);
    }

    if (!BCMPORT_LT_FIELD_GET(
             lt_info->fbmp, EGR_VLAN_MEMBERSHIP_CHECK)) {
        SHR_IF_ERR_EXIT
            (bcmlrd_field_default_get(unit,
                                      PORT_MEMBERSHIP_POLICYt,
                                      PORT_MEMBERSHIP_POLICYt_EGR_VLAN_MEMBERSHIP_CHECKf,
                                      1, &def_val, &num));
        lt_info->egr_vlan_membership_check = def_val ? true : false;
        BCMPORT_LT_FIELD_SET(lt_info->fbmp, EGR_VLAN_MEMBERSHIP_CHECK);
    }

    if (!BCMPORT_LT_FIELD_GET(
                     lt_info->fbmp, SKIP_VLAN_CHECK)) {
        int rv = SHR_E_NONE;
        const bcmlrd_field_data_t *field_info;

        rv = bcmlrd_field_get(unit, PORT_MEMBERSHIP_POLICYt,
                              PORT_MEMBERSHIP_POLICYt_SKIP_VLAN_CHECKf, &field_info);
        if (rv == SHR_E_NONE) {
            SHR_IF_ERR_EXIT
                (bcmlrd_field_default_get(unit,
                                          PORT_MEMBERSHIP_POLICYt,
                                          PORT_MEMBERSHIP_POLICYt_SKIP_VLAN_CHECKf,
                                          1, &def_val, &num));
            lt_info->skip_vlan_check = def_val ? true : false;
            BCMPORT_LT_FIELD_SET(lt_info->fbmp, SKIP_VLAN_CHECK);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief PORT_POLICY IMM table change callback function for staging.
 *
 * Handle PORT_POLICY IMM table change events.
 *
 * \param [in] unit Unit number.
 * \param [in] sid This is the logical table ID.
 * \param [in] op_arg The operation arguments.
 * \param [in] event_reason This is the reason for the entry event.
 * \param [in] key This is a linked list of the key fields identifying
 * the entry.
 * \param [in] data This is a linked list of the data fields in the
 * modified entry.
 * \param [in] context Is a pointer that was given during registration.
 * The callback can use this pointer to retrieve some context.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Fails to handle LT change events.
 */
static int
port_membership_policy_imm_callback_stage(int unit,
                                          bcmltd_sid_t sid,
                                          const bcmltd_op_arg_t *op_arg,
                                          bcmimm_entry_event_t event_reason,
                                          const bcmltd_field_t *key,
                                          const bcmltd_field_t *data,
                                          void *context,
                                          bcmltd_fields_t *output_fields)
{
    port_membership_policy_info_t lt_info;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(op_arg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (port_membership_policy_lt_fields_parse(unit, key, data, &lt_info));

    if (output_fields) {
        output_fields->count = 0;
    }

    switch (event_reason) {
        case BCMIMM_ENTRY_UPDATE:
            break;
        case BCMIMM_ENTRY_INSERT:
        case BCMIMM_ENTRY_DELETE:
            SHR_IF_ERR_VERBOSE_EXIT
                (port_membership_policy_info_default_values_init
                    (unit, &lt_info));
            break;
        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_port_membership_policy_set(unit, op_arg, sid,
                                           lt_info.port_id, &lt_info));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Lookup an entry from the mpls protect Table.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  sid             Logical table ID.
 * \param [in]  op_arg          The operation arguments.
 * \param [in]  context         Pointer to retrieve some context.
 * \param [in]  lkup_type       Lookup type.
 * \param [in]  In              Input key fields list.
 * \param [in/out] out          Output parameter list.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
static int
port_membership_policy_imm_callback_lookup(int unit,
                                           bcmltd_sid_t sid,
                                           const bcmltd_op_arg_t *op_arg,
                                           void *context,
                                           bcmimm_lookup_type_t lkup_type,
                                           const bcmltd_fields_t *in,
                                           bcmltd_fields_t *out)
{
    shr_port_t port_id;
    port_membership_policy_info_t lt_info;
    const bcmltd_fields_t *key;
    uint32_t fid = 0;
    uint64_t fval = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(op_arg, SHR_E_PARAM);

    if (!(op_arg->attrib & BCMLT_ENT_ATTR_GET_FROM_HW)) {
        SHR_EXIT();
    }

    key = (lkup_type == BCMIMM_TRAVERSE) ? out : in;
    fid = PORT_MEMBERSHIP_POLICYt_PORT_IDf;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_imm_fields_get(unit, key, fid, &fval));
    port_id = fval;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_port_membership_policy_get(unit,
                                           op_arg,
                                           sid,
                                           port_id,
                                           &lt_info));

    SHR_IF_ERR_EXIT
        (bcmcth_imm_field_update(unit,
                                 out,
                                 PORT_MEMBERSHIP_POLICYt_ING_VLAN_MEMBERSHIP_CHECKf,
                                 lt_info.ing_vlan_membership_check));

    SHR_IF_ERR_EXIT
        (bcmcth_imm_field_update(unit,
                                 out,
                                 PORT_MEMBERSHIP_POLICYt_EGR_VLAN_MEMBERSHIP_CHECKf,
                                 lt_info.egr_vlan_membership_check));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief PORT_BRIDGE In-memory event callback structure.
 *
 * This structure contains callback functions that will be corresponding
 * to PORT_BRIDGE logical table entry commit stages.
 */
static bcmimm_lt_cb_t port_membership_policy_imm_callback = {

    /*! Validate function. */
    .validate = NULL,

    /*! Extended staging function. */
    .op_stage = port_membership_policy_imm_callback_stage,

    /*! Extended lookup function. */
    .op_lookup = port_membership_policy_imm_callback_lookup,

    /*! Commit function. */
    .commit = NULL,

    /*! Abort function. */
    .abort = NULL
};

/*!
 * \brief imm PORT_SYSTEM_ING_MIRROR_PROFILEt notification input fields parsing.
 *
 * Parse imm PORT_SYSTEM_ING_MIRROR_PROFILEt input fields.
 *
 * \param [in] unit Unit number.
 * \param [in] key IMM input key field array.
 * \param [in] data IMM input data field array.
 * \param [out] imirror Port system ingress mirror profile data buffer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Faild to convert \c key \c data to \c ltcfg.
 */
static int
port_system_imirror_profile_lt_fields_parse(
    int unit,
    const bcmltd_field_t *key,
    const bcmltd_field_t *data,
    port_system_imirror_profile_t *imirror)
{
    const bcmltd_field_t *gen_field;
    uint32_t fid;
    uint64_t fval;

    SHR_FUNC_ENTER(unit);

    sal_memset(imirror, 0, sizeof(*imirror));
    imirror->tbl_inst = -1;

    /* Parse key field */
    gen_field = key;
    while (gen_field) {
        fid = gen_field->id;
        fval = gen_field->data;

        switch (fid) {
        case PORT_SYSTEM_ING_MIRROR_PROFILEt_PORT_SYSTEM_PROFILE_IDf:
            imirror->port_system_profile_id = fval;
            SHR_BITSET(imirror->fbmp, fid);
            break;
        case PORT_SYSTEM_ING_MIRROR_PROFILEt_MIRROR_CONTAINER_IDf:
            imirror->container = fval;
            SHR_BITSET(imirror->fbmp, fid);
            break;
        case PORT_SYSTEM_ING_MIRROR_PROFILEt_PIPEf:
            imirror->tbl_inst= fval;
            SHR_BITSET(imirror->fbmp, fid);
            break;
        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
        gen_field = gen_field->next;
    }

    /* Parse data field */
    gen_field = data;
    while (gen_field) {
        fid = gen_field->id;
        fval = gen_field->data;

        switch (fid) {
        case PORT_SYSTEM_ING_MIRROR_PROFILEt_MIRRORf:
            imirror->enable = fval;
            SHR_BITSET(imirror->fbmp, fid);
            break;
        case PORT_SYSTEM_ING_MIRROR_PROFILEt_MIRROR_INSTANCE_IDf:
            imirror->instance_id = fval;
            SHR_BITSET(imirror->fbmp, fid);
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
 * \brief PORT_SYSTEM_ING_MIRROR_PROFILE IMM table change callback function for staging.
 *
 * Handle PORT_SYSTEM_ING_MIRROR_PROFILE IMM table change events.
 *
 * \param [in] unit Unit number.
 * \param [in] sid This is the logical table ID.
 * \param [in] op_arg Operation arguments.
 * \param [in] event_reason This is the reason for the entry event.
 * \param [in] key This is a linked list of the key fields identifying
 * the entry.
 * \param [in] data This is a linked list of the data fields in the
 * modified entry.
 * \param [in] context Is a pointer that was given during registration.
 * The callback can use this pointer to retrieve some context.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Fails to handle LT change events.
 */
static int
port_system_ing_mirror_profile_imm_stage_callback(
    int unit,
    bcmltd_sid_t sid,
    const bcmltd_op_arg_t *op_arg,
    bcmimm_entry_event_t event_reason,
    const bcmltd_field_t *key,
    const bcmltd_field_t *data,
    void *context,
    bcmltd_fields_t *output_fields)
{
    port_system_imirror_profile_t cfg;
    bool m_en = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (port_system_imirror_profile_lt_fields_parse(unit, key, data, &cfg));

    if (output_fields) {
        output_fields->count = 0;
    }

    switch (event_reason) {
    case BCMIMM_ENTRY_INSERT:
    case BCMIMM_ENTRY_UPDATE:
        if (!SHR_BITGET(cfg.fbmp, PORT_SYSTEM_ING_MIRROR_PROFILEt_MIRRORf)) {
            SHR_EXIT();
        }
        m_en = cfg.enable;
        break;
    case BCMIMM_ENTRY_DELETE:
        break;
    default:
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_port_system_ingress_mirror_profile_set(
             unit,
             op_arg,
             sid,
             cfg.tbl_inst,
             cfg.port_system_profile_id,
             cfg.container,
             m_en));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief PORT_ING_MIRROR In-memory event callback structure.
 *
 * This structure contains callback functions that will be corresponding
 * to PORT_ING_MIRROR logical table entry commit stages.
 */
static bcmimm_lt_cb_t port_system_ing_mirror_profile_imm_callback = {

    /*! Validate function. */
    .validate = NULL,

    /*! Extended staging function. */
    .op_stage = port_system_ing_mirror_profile_imm_stage_callback,

    /*! Commit function. */
    .commit = NULL,

    /*! Abort function. */
    .abort = NULL
};

/*!
 * \brief imm PORT_SYSTEM_DESTINATIONt notification input fields parsing.
 *
 * Parse imm PORT_SYSTEM_DESTINATIONt input fields.
 *
 * \param [in]  unit Unit number.
 * \param [in]  key IMM input key field array.
 * \param [in]  data IMM input data field array.
 * \param [out] entry Port system destination entry.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Faild to convert \c key \c data to \c ltcfg.
 */
static int
port_system_dest_lt_fields_parse(
    int unit,
    const bcmltd_field_t *key,
    const bcmltd_field_t *data,
    port_system_dest_entry_t *entry)
{
    const bcmltd_field_t *gen_field;
    uint32_t fid;
    uint64_t fval;

    SHR_FUNC_ENTER(unit);

    sal_memset(entry, 0, sizeof(*entry));

    /* Parse key field */
    fid = PORT_SYSTEM_DESTINATIONt_PORT_SYSTEM_IDf;
    if (key->id != fid) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    fval = key->data;
    entry->port_system_id = key->data;

    /* Parse data field */
    gen_field = data;
    while (gen_field) {
        fid = gen_field->id;
        fval = gen_field->data;

        switch (fid) {
        case PORT_SYSTEM_DESTINATIONt_IS_TRUNK_SYSTEMf:
            entry->trunk_valid = fval;
            BCMPORT_LT_FIELD_SET(
              entry->fbmp, TRUNK_VALID);
            break;
        case PORT_SYSTEM_DESTINATIONt_TRUNK_SYSTEM_IDf:
            entry->trunk_id = fval;
            BCMPORT_LT_FIELD_SET(
              entry->fbmp, TRUNK_ID);
            break;
        case PORT_SYSTEM_DESTINATIONt_PORT_IDf:
            entry->port_id = fval;
            BCMPORT_LT_FIELD_SET(
              entry->fbmp, LOCAL_PORT_ID);
            break;
        case PORT_SYSTEM_DESTINATIONt_DLB_ID_VALIDf:
            entry->dlb_id_valid = fval;
            BCMPORT_LT_FIELD_SET(
              entry->fbmp, DLB_ID_VALID);
            break;
        case PORT_SYSTEM_DESTINATIONt_DLB_IDf:
            entry->dlb_id = fval;
             BCMPORT_LT_FIELD_SET(
              entry->fbmp, DLB_ID);
            break;
        case PORT_SYSTEM_DESTINATIONt_FLEX_CTR_ACTION_IDf:
            entry->flex_ctr_action = fval;
            BCMPORT_LT_FIELD_SET(
              entry->fbmp, FLEX_CTR_ACTION);
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
 * \brief PORT_SYSTEM_DESTINATION IMM table change callback function for staging.
 *
 * Handle PORT_SYSTEM_DESTINATION IMM table change events.
 *
 * \param [in] unit Unit number.
 * \param [in] sid This is the logical table ID.
 * \param [in] op_arg Operation arguments.
 * \param [in] event_reason This is the reason for the entry event.
 * \param [in] key This is a linked list of the key fields identifying
 * the entry.
 * \param [in] data This is a linked list of the data fields in the
 * modified entry.
 * \param [in] context Is a pointer that was given during registration.
 * The callback can use this pointer to retrieve some context.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Fails to handle LT change events.
 */
static int
port_system_destination_imm_stage_callback(
    int unit,
    bcmltd_sid_t sid,
    const bcmltd_op_arg_t *op_arg,
    bcmimm_entry_event_t event_reason,
    const bcmltd_field_t *key,
    const bcmltd_field_t *data,
    void *context,
    bcmltd_fields_t *output_fields)
{
    port_system_dest_entry_t entry;


    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(key, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (port_system_dest_lt_fields_parse(unit, key, data, &entry));

    if (output_fields) {
        output_fields->count = 0;
    }

    switch (event_reason) {
    case BCMIMM_ENTRY_INSERT:
    case BCMIMM_ENTRY_UPDATE:
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_port_system_dest_set(unit, op_arg, sid, 1, &entry));
        break;
    case BCMIMM_ENTRY_DELETE:
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_port_system_dest_set(unit, op_arg, sid, 0, &entry));
        break;
    default:
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief PORT_SYSTEM_DESTINATION In-memory event callback structure.
 *
 * This structure contains callback functions that will be corresponding
 * to PORT_ING_DESTINATION logical table entry commit stages.
 */
static bcmimm_lt_cb_t port_system_destination_imm_callback = {

    /*! Validate function. */
    .validate = NULL,

    /*! Extended staging function. */
    .op_stage = port_system_destination_imm_stage_callback,

    /*! Commit function. */
    .commit = NULL,

    /*! Abort function. */
    .abort = NULL
};

/*!
 * \brief imm PORT_SVP_ING_MIRRORt notification input fields parsing.
 *
 * Parse imm PORT_SVP_ING_MIRRORt input fields.
 *
 * \param [in] unit Unit number.
 * \param [in] key IMM input key field array.
 * \param [in] data IMM input data field array.
 * \param [out] imirror Port ingress mirror data buffer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Faild to convert \c key \c data to \c ltcfg.
 */
static int
port_svp_imirror_lt_fields_parse(int unit,
                                 const bcmltd_field_t *key,
                                 const bcmltd_field_t *data,
                                 port_imirror_t *imirror)
{
    const bcmltd_field_t *gen_field;
    uint32_t fid;
    uint64_t fval;

    SHR_FUNC_ENTER(unit);

    sal_memset(imirror, 0, sizeof(*imirror));

    /* Parse key field */
    gen_field = key;
    while (gen_field) {
        fid = gen_field->id;
        fval = gen_field->data;

        switch (fid) {
        case PORT_SVP_ING_MIRRORt_PORT_SVP_IDf:
            imirror->port = fval;
            SHR_BITSET(imirror->fbmp, fid);
            break;
        case PORT_SVP_ING_MIRRORt_MIRROR_CONTAINER_IDf:
            imirror->container = fval;
            SHR_BITSET(imirror->fbmp, fid);
            break;
        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
        gen_field = gen_field->next;
    }

    /* Parse data field */
    gen_field = data;
    while (gen_field) {
        fid = gen_field->id;
        fval = gen_field->data;

        switch (fid) {
        case PORT_SVP_ING_MIRRORt_MIRRORf:
            imirror->enable = fval;
            SHR_BITSET(imirror->fbmp, fid);
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
 * \brief Lookup callback function of IMM event handler.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  sid             Logical table ID.
 * \param [in]  op_arg          The operation arguments.
 * \param [in]  context         Pointer to retrieve some context.
 * \param [in]  lkup_type       Lookup type.
 * \param [in]  In              Input key fields list.
 * \param [in/out] out          Output parameter list.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
static int
port_svp_ing_mirror_imm_lookup_callback(int unit,
                                        bcmltd_sid_t sid,
                                        const bcmltd_op_arg_t *op_arg,
                                        void *context,
                                        bcmimm_lookup_type_t lkup_type,
                                        const bcmltd_fields_t *in,
                                        bcmltd_fields_t *out)
{
    const bcmltd_fields_t *key;
    port_imirror_t imirror;
    bool m_en = 0;
    uint32_t i = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(op_arg, SHR_E_PARAM);
    /* Return if not getting from HW. */
    if (!(op_arg->attrib & BCMLT_ENT_ATTR_GET_FROM_HW)) {
        SHR_EXIT();
    }

    /* key fields are in the "out" parameter for traverse operations */
    key = (lkup_type == BCMIMM_TRAVERSE) ? out : in;

    sal_memset(&imirror, 0, sizeof(imirror));
    /* Parse key field */
    for (i = 0; i < key->count; i++) {
        if (key->field[i]->id == PORT_SVP_ING_MIRRORt_PORT_SVP_IDf) {
            imirror.port = key->field[i]->data;
        }
        if (key->field[i]->id == PORT_SVP_ING_MIRRORt_MIRROR_CONTAINER_IDf) {
            imirror.container = key->field[i]->data;
        }
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_port_svp_ingress_mirror_get(unit, op_arg, sid, &imirror, &m_en));

    for (i = 0; i < out->count; i++) {
        if (out->field[i]->id == PORT_SVP_ING_MIRRORt_MIRRORf) {
            out->field[i]->data = m_en;
            break;
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief PORT_SVP_ING_MIRRORt IMM table change callback function.
 *
 * Handle PORT_SVP_ING_MIRRORt IMM table change events.
 *
 * \param [in] unit Unit number.
 * \param [in] sid This is the logical table ID.
 * \param [in] op_arg Operation arguments.
 * \param [in] event_reason This is the reason for the entry event.
 * \param [in] key This is a linked list of the key fields identifying
 * the entry.
 * \param [in] data This is a linked list of the data fields in the
 * modified entry.
 * \param [in] context Is a pointer that was given during registration.
 * The callback can use this pointer to retrieve some context.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Fails to handle LT change events.
 */
static int
port_svp_ing_mirror_imm_stage_callback(int unit,
                                       bcmltd_sid_t sid,
                                       const bcmltd_op_arg_t *op_arg,
                                       bcmimm_entry_event_t event_reason,
                                       const bcmltd_field_t *key,
                                       const bcmltd_field_t *data,
                                       void *context,
                                       bcmltd_fields_t *output_fields)
{
    port_imirror_t cfg;
    bool m_en = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (port_svp_imirror_lt_fields_parse(unit, key, data, &cfg));

    if (output_fields) {
        output_fields->count = 0;
    }

    switch (event_reason) {
    case BCMIMM_ENTRY_INSERT:
    case BCMIMM_ENTRY_UPDATE:
        if (!SHR_BITGET(cfg.fbmp, PORT_SVP_ING_MIRRORt_MIRRORf)) {
            SHR_EXIT();
        }
        m_en = cfg.enable;
        break;
    case BCMIMM_ENTRY_DELETE:
        break;
    default:
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_port_svp_ingress_mirror_set(unit, op_arg, sid, &cfg, m_en));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief PORT_SVP_ING_MIRROR In-memory event callback structure.
 *
 * This structure contains callback functions that will be corresponding
 * to PORT_SVP_ING_MIRROR logical table entry commit stages.
 */
static bcmimm_lt_cb_t port_svp_ing_mirror_imm_callback = {

    /*! Validate function. */
    .validate = NULL,

    /*! Extended staging function. */
    .op_stage = port_svp_ing_mirror_imm_stage_callback,

    /*! Extended lookup function. */
    .op_lookup = port_svp_ing_mirror_imm_lookup_callback,

    /*! Commit function. */
    .commit = NULL,

    /*! Abort function. */
    .abort = NULL
};

static void
port_trunk_pctl_entry_t_init(port_trunk_pctl_entry_t *entry)
{
    sal_memset(entry, 0, sizeof (port_trunk_pctl_entry_t));
    return;
}

/*!
 * \brief Delete the LT entry.
 *
 * Parse Delete the
 * PORT_TRUNK LT entry
 *
 * \param [in]  unit Unit number.
 * \param [in]  op_arg Operation arguments.
 * \param [in]  sid LTD Symbol ID.
 * \param [in]  key field list.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Faild to delete the LT entry.
 */
static int
port_trunk_ctrl_delete(int unit,
                       const bcmltd_op_arg_t *op_arg,
                       bcmltd_sid_t sid,
                       const bcmltd_field_t *key)
{
    port_trunk_pctl_entry_t entry;
    uint32_t fid = 0;

    SHR_FUNC_ENTER(unit);

    port_trunk_pctl_entry_t_init(&entry);

    /* Read the PORT_ID. */
    fid = PORT_TRUNKt_PORT_IDf;
    if (key->id != fid) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    entry.port_id  = (uint32_t)key->data;

    BCMPORT_LT_FIELD_SET(
         entry.fbmp, TRUNK_PORT_ID);
    BCMPORT_LT_FIELD_SET(
         entry.fbmp, TRUNK_SYSTEM_FAILOVER);

    SHR_IF_ERR_EXIT
        (bcmcth_port_trunk_pctl_set(unit, op_arg, sid, &entry));
   exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Insert or update the LT entry.
 *
 * Parse Insert or update the
 * PORT_TRUNK LT entry
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
port_trunk_ctrl_update(int unit,
                       const bcmltd_op_arg_t *op_arg,
                       bcmltd_sid_t sid,
                       const bcmltd_field_t *key,
                       const bcmltd_field_t *data)
{
    uint64_t val = 0;
    port_trunk_pctl_entry_t entry;
    uint32_t fid = 0;
    const bcmltd_field_t *in = NULL;


    SHR_FUNC_ENTER(unit);

    port_trunk_pctl_entry_t_init(&entry);

    /* Read the PORT_ID. */
    fid = PORT_TRUNKt_PORT_IDf;
    if (key->id != fid) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    entry.port_id  = (uint32_t)key->data;
    BCMPORT_LT_FIELD_SET(entry.fbmp, TRUNK_PORT_ID);

    for (in = data; (in != NULL); in = in->next) {
        fid = in->id;
        val = in->data;
        switch (fid) {
            case PORT_TRUNKt_TRUNK_SYSTEM_FAILOVERf:
                entry.trunk_system_failover  = (uint32_t) val;
                BCMPORT_LT_FIELD_SET(
                entry.fbmp, TRUNK_SYSTEM_FAILOVER);
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

    SHR_IF_ERR_EXIT
        (bcmcth_port_trunk_pctl_set(unit, op_arg, sid, &entry));

   exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief PORT_TRUNK table change
 *  callback function for staging.
 *
 * Handle PORT_TRUNK IMM table change events.
 *
 * \param [in] unit Unit number.
 * \param [in] sid This is the logical table ID.
 * \param [in] op_arg Operation argument.
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
port_trunk_imm_stage_callback(int unit,
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
            SHR_IF_ERR_EXIT
                (port_trunk_ctrl_update(unit, op_arg, sid, key, data));
            break;
        case BCMIMM_ENTRY_DELETE:
            SHR_IF_ERR_EXIT
                (port_trunk_ctrl_delete(unit, op_arg, sid, key));

            break;
        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief PORT_TRUNK
 *  In-memory event callback structure.
 *
 * This structure contains callback functions that will be corresponding
 * to PORT_TRUNK LT entry commit stages.
 */
static bcmimm_lt_cb_t port_trunk_imm_callback = {
  /*! Valdiation function. */
  .validate = NULL,

  /*! Extended staging function. */
  .op_stage = port_trunk_imm_stage_callback,

  /*! Commit function. */
  .commit   = NULL,

  /*! Abort function. */
  .abort    = NULL
};

static bool
bcmcth_port_imm_mapped(int unit, const bcmdrd_sid_t sid)
{
    int rv;
    const bcmlrd_map_t *map = NULL;

    rv = bcmlrd_map_get(unit, sid, &map);
    if (SHR_SUCCESS(rv) &&
        map &&
        map->group &&
        map->group[0].dest.kind == BCMLRD_MAP_CUSTOM) {
        return TRUE;
    }
    return FALSE;
}


/*******************************************************************************
 * Public Functions
 */
void
bcmcth_port_imm_db_cleanup(int unit)
{
    if (port_fld_arr_hdl) {
        shr_famm_hdl_delete(port_fld_arr_hdl);
        port_fld_arr_hdl = 0;
    }
}

int
bcmcth_port_imm_init(int unit)
{
    SHR_FUNC_ENTER(unit);

    if (!port_fld_arr_hdl) {
        SHR_IF_ERR_EXIT
            (shr_famm_hdl_init(PORT_FIELD_COUNT_MAX, &port_fld_arr_hdl));
    }

    /*
     * To register callback for PORT LTs here.
     */
    if (bcmcth_port_imm_mapped(unit, PORT_ING_MIRRORt)) {
        SHR_IF_ERR_EXIT
            (bcmimm_lt_event_reg(unit,
                                 PORT_ING_MIRRORt,
                                 &port_ing_mirror_imm_callback,
                                 NULL));
    }

    if (bcmcth_port_imm_mapped(unit, PORT_EGR_MIRRORt)) {
        SHR_IF_ERR_EXIT
            (bcmimm_lt_event_reg(unit,
                                 PORT_EGR_MIRRORt,
                                 &port_egr_mirror_imm_callback,
                                 NULL));
    }

    if (bcmcth_port_imm_mapped(unit, PORT_MIRRORt)) {
        SHR_IF_ERR_EXIT
            (bcmimm_lt_event_reg(unit,
                                 PORT_MIRRORt,
                                 &port_mirror_imm_callback,
                                 NULL));
    }

    if (bcmcth_port_imm_mapped(unit,PORT_BRIDGEt)) {
        SHR_IF_ERR_EXIT(
            bcmimm_lt_event_reg(unit,
                                PORT_BRIDGEt,
                                &port_bridge_imm_callback,
                                NULL));
    }

    if (bcmcth_port_imm_mapped(unit, PORT_MEMBERSHIP_POLICYt)) {
        SHR_IF_ERR_EXIT(
            bcmimm_lt_event_reg(unit,
                                PORT_MEMBERSHIP_POLICYt,
                                &port_membership_policy_imm_callback,
                                NULL));
    }

    if (bcmcth_port_imm_mapped(unit, PORT_SYSTEM_ING_MIRROR_PROFILEt)) {
        SHR_IF_ERR_EXIT
            (bcmimm_lt_event_reg(unit,
                                 PORT_SYSTEM_ING_MIRROR_PROFILEt,
                                 &port_system_ing_mirror_profile_imm_callback,
                                 NULL));
    }

    if (bcmcth_port_imm_mapped(unit, PORT_SYSTEM_DESTINATIONt)) {
        SHR_IF_ERR_EXIT
            (bcmimm_lt_event_reg(unit,
                                 PORT_SYSTEM_DESTINATIONt,
                                 &port_system_destination_imm_callback,
                                 NULL));
    }

    if (bcmcth_port_imm_mapped(unit, PORT_TRUNKt)) {
        SHR_IF_ERR_EXIT
            (bcmimm_lt_event_reg(unit,
                                 PORT_TRUNKt,
                                 &port_trunk_imm_callback,
                                 NULL));
    }

    if (bcmcth_port_imm_mapped(unit, PORT_SVP_ING_MIRRORt)) {
        SHR_IF_ERR_EXIT
            (bcmimm_lt_event_reg(unit,
                                 PORT_SVP_ING_MIRRORt,
                                 &port_svp_ing_mirror_imm_callback,
                                 NULL));
    }

exit:
    SHR_FUNC_EXIT();
}

