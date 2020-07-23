/*! \file bcmcth_tnl_imm.c
 *
 * TNL interface to in-memory table.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmlrd/bcmlrd_map.h>
#include <bcmimm/bcmimm_int_comp.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmcth/bcmcth_tnl_mpls_prot.h>
#include <bcmcth/bcmcth_tnl_internal.h>
#include <bcmcth/bcmcth_tnl_mpls_drv.h>
#include <bcmcth/bcmcth_tnl_drv.h>
#include <bcmcth/bcmcth_util.h>
#include <bcmcth/bcmcth_imm_util.h>

/*******************************************************************************
 * Local definitions
 */

/* BSL Module */
#define BSL_LOG_MODULE  BSL_LS_BCMCTH_TNL

/*******************************************************************************
 * Private functions
 */

/*!
 * \brief TNL_MPLS_PROTECTION_ENABLEt input fields parsing.
 *
 * Parse IMM TNL_MPLS_PROTECTION_ENABLEt input fields.
 *
 * \param [in] unit Unit number.
 * \param [in] key IMM input key field array.
 * \param [in] data IMM input data field array.
 * \param [out] prot_entry Protection entry buffer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Faild to parse.
 */
static int
tnl_mpls_prot_enable_parse(int unit,
                           const bcmltd_field_t *key,
                           const bcmltd_field_t *data,
                           bcmcth_tnl_mpls_prot_en_lt_t *prot_entry)
{
    const bcmltd_field_t *gen_field;
    uint32_t fid;
    uint64_t fval;

    SHR_FUNC_ENTER(unit);

    /* Parse key. */
    if (key->id != TNL_MPLS_PROTECTION_ENABLEt_NHOP_IDf) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    prot_entry->nhop_id = key->data;

    /* Parse data field. */
    gen_field = data;
    while (gen_field) {
        fid = gen_field->id;
        fval = gen_field->data;
        switch (fid) {
        case TNL_MPLS_PROTECTION_ENABLEt_PROTECTIONf:
            prot_entry->protection = fval;
            prot_entry->protection_valid = TRUE;
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
 * \brief TNL_MPLS_PROTECTION_ENABLE IMM table change callback for staging.
 *
 * Handle TNL_MPLS_PROTECTION_ENABLE IMM table change events.
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
tnl_mpls_prot_en_callback_stage(int unit,
                                bcmltd_sid_t sid,
                                const bcmltd_op_arg_t *op_arg,
                                bcmimm_entry_event_t event_reason,
                                const bcmltd_field_t *key,
                                const bcmltd_field_t *data,
                                void *context,
                                bcmltd_fields_t *output_fields)
{
    bcmcth_tnl_mpls_prot_en_lt_t prot_entry;
    uint32_t nhop_id;
    bool protection;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(op_arg, SHR_E_PARAM);

    if (output_fields) {
        output_fields->count = 0;
    }

    sal_memset(&prot_entry, 0, sizeof(prot_entry));
    SHR_IF_ERR_VERBOSE_EXIT
        (tnl_mpls_prot_enable_parse(unit, key, data, &prot_entry));
    nhop_id = prot_entry.nhop_id;
    switch (event_reason) {
    case BCMIMM_ENTRY_INSERT:
    case BCMIMM_ENTRY_UPDATE:
        if (prot_entry.protection_valid == false) {
            if (event_reason == BCMIMM_ENTRY_UPDATE) {
                /* Data fields not updated, return. */
                SHR_EXIT();
            } else {
                protection = FALSE;
            }
        } else {
            protection = prot_entry.protection;
        }
        break;
    case BCMIMM_ENTRY_DELETE:
        protection = FALSE;
        break;
    default:
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_tnl_mpls_prot_enable_set(unit, op_arg, sid,
                                         nhop_id, protection));
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
tnl_mpls_prot_callback_lookup(int unit,
                              bcmltd_sid_t sid,
                              const bcmltd_op_arg_t *op_arg,
                              void *context,
                              bcmimm_lookup_type_t lkup_type,
                              const bcmltd_fields_t *in,
                              bcmltd_fields_t *out)
{
    uint32_t nhop_id;
    bool protection = 0;
    const bcmltd_fields_t *key;
    uint32_t fid = 0;
    uint64_t fval = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(op_arg, SHR_E_PARAM);
    /* Return if not getting from HW. */
    if (!(op_arg->attrib & BCMLT_ENT_ATTR_GET_FROM_HW)) {
        SHR_EXIT();
    }

    /* key fields are in the "out" parameter for traverse operations */
    key = (lkup_type == BCMIMM_TRAVERSE) ? out : in;
    fid = TNL_MPLS_PROTECTION_ENABLEt_NHOP_IDf;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_imm_fields_get(unit, key, fid, &fval));
    nhop_id = fval;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_tnl_mpls_prot_enable_get(unit, op_arg, sid,
                                         nhop_id, &protection));

    SHR_IF_ERR_EXIT(
        bcmcth_imm_field_update(unit,
                                out,
                                TNL_MPLS_PROTECTION_ENABLEt_PROTECTIONf,
                                protection));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief TNL_IPV4/IPV6/MPLS_ENCAP IMM table change callback for staging.
 *
 * Handle TNL_IPV4/IPV6/MPLS_ENCAP IMM table change events.
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
tnl_encap_callback_stage(int unit,
                         bcmltd_sid_t sid,
                         const bcmltd_op_arg_t *op_arg,
                         bcmimm_entry_event_t event_reason,
                         const bcmltd_field_t *key,
                         const bcmltd_field_t *data,
                         void *context,
                         bcmltd_fields_t *output_fields)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(op_arg, SHR_E_PARAM);

    if (output_fields) {
        output_fields->count = 0;
    }

    switch (event_reason) {
    case BCMIMM_ENTRY_INSERT:
        SHR_IF_ERR_EXIT(bcmcth_tnl_encap_entry_insert(unit,
                                                      sid,
                                                      op_arg,
                                                      key,
                                                      data,
                                                      output_fields));
        break;
    case BCMIMM_ENTRY_UPDATE:
        SHR_IF_ERR_EXIT(bcmcth_tnl_encap_entry_update(unit,
                                                      sid,
                                                      op_arg,
                                                      key,
                                                      data,
                                                      output_fields));
        break;
    case BCMIMM_ENTRY_DELETE:
        SHR_IF_ERR_EXIT(bcmcth_tnl_encap_entry_delete(unit,
                                                      sid,
                                                      op_arg,
                                                      key,
                                                      data,
                                                      output_fields));
        break;
    default:
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief TNL_MPLS_PROTECTION_ENABLE In-memory event callback structure.
 *
 * This structure contains callback functions corresponding to
 * TNL_MPLS_PROTECTION_ENABLE logical table entry commit stages.
 */
static bcmimm_lt_cb_t tnl_mpls_prot_en_imm_callback = {

    /*! Validate function. */
    .validate  = NULL,

    /*! Extended staging function. */
    .op_stage  = tnl_mpls_prot_en_callback_stage,

    /*! Commit function. */
    .commit    = NULL,

    /*! Abort function. */
    .abort     = NULL,

    /*! Extended Lookup function. */
    .op_lookup = tnl_mpls_prot_callback_lookup,
};

/*!
 * \brief TNL_IPV4/IPV6/MPLS_ENCAP In-memory event callback structure.
 *
 * This structure contains callback functions that will be corresponding
 * to TNL_IPV4/IPV6/MPLS_ENCAP logical table entry commit stages.
 */
static bcmimm_lt_cb_t tnl_encap_imm_callback = {

    /*! Validate function. */
    .validate = NULL,

    /*! Extended Staging function. */
    .op_stage = tnl_encap_callback_stage,

    /*! Commit function. */
    .commit = bcmcth_tnl_transaction_commit,

    /*! Abort function. */
    .abort = bcmcth_tnl_transaction_abort,

    /*! Extended Lookup function. */
    .op_lookup = bcmcth_tnl_encap_entry_lookup,
};

/*!
 * \brief imm TNL_MPLS_EXP_QOS_SELECTIONt  notification input fields parsing.
 *
 * Parse imm TNL_MPLS_EXP_QOS_SELECTIONt input fields.
 *
 * \param [in] unit Unit number.
 * \param [in] key IMM input key field array.
 * \param [in] data IMM input data field array.
 * \param [out] exp to ecn map data buffer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Faild to convert \c key \c data to \c lt_info.
 */
static int
tnl_mpls_exp_qos_ctrl_lt_fields_parse(int unit,
                                      const bcmltd_field_t *key,
                                      const bcmltd_field_t *data,
                                      bcmcth_tnl_mpls_exp_qos_ctrl_t *lt_entry)
{
    const bcmltd_field_t *gen_field;
    uint32_t fid;
    uint64_t fval;

    SHR_FUNC_ENTER(unit);

    sal_memset(lt_entry, 0, sizeof(bcmcth_tnl_mpls_exp_qos_ctrl_t));

    /* Parse key field */
    gen_field = key;
    while (gen_field) {
        fid = gen_field->id;
        fval = gen_field->data;

        switch (fid) {
            case TNL_MPLS_EXP_QOS_SELECTIONt_TNL_MPLS_EXP_QOS_SELECTION_ID_1f:
                BCMCTH_LT_FIELD_SET(
                    lt_entry->fbmp, BCMCTH_TNL_MPLS_EXP_QOS_CONTROL_ID_1);
                lt_entry->map_id[0] = (uint8_t) fval;
                break;
            case TNL_MPLS_EXP_QOS_SELECTIONt_TNL_MPLS_EXP_QOS_SELECTION_ID_2f:
                BCMCTH_LT_FIELD_SET(
                    lt_entry->fbmp, BCMCTH_TNL_MPLS_EXP_QOS_CONTROL_ID_2);
                lt_entry->map_id[1] = (uint8_t) fval;
                break;
            case TNL_MPLS_EXP_QOS_SELECTIONt_TNL_MPLS_EXP_QOS_SELECTION_ID_3f:
                BCMCTH_LT_FIELD_SET(
                    lt_entry->fbmp, BCMCTH_TNL_MPLS_EXP_QOS_CONTROL_ID_3);
                lt_entry->map_id[2] = (uint8_t) fval;
                break;
            case TNL_MPLS_EXP_QOS_SELECTIONt_EFFECTIVE_EXP_QOSf:
                BCMCTH_LT_FIELD_SET(
                    lt_entry->fbmp, BCMCTH_TNL_MPLS_EFFECTIVE_EXP_QOS_PROFILE);
                lt_entry->effective = (bool) fval;
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
            case TNL_MPLS_EXP_QOS_SELECTIONt_EXP_QOS_SELECTf:
                BCMCTH_LT_FIELD_SET(
                    lt_entry->fbmp, BCMCTH_TNL_MPLS_EXP_QOS_SEL);
                lt_entry->qos_sel = (uint8_t) fval;
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
 * \param [in] lt_entry MPLS_EXP_QOS_CONTROLt LT entry data buffer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Input parameter failed validation check.
 */
static int
tnl_mpls_exp_qos_ctrl_default_values_init(int unit,
                                          bcmcth_tnl_mpls_exp_qos_ctrl_t *lt_entry)
{
    uint64_t def_val = 0;
    uint32_t num;
    SHR_FUNC_ENTER(unit);

    /* validate input parameter. */
    SHR_NULL_CHECK(lt_entry, SHR_E_PARAM);

    if (!BCMCTH_LT_FIELD_GET(
             lt_entry->fbmp, BCMCTH_TNL_MPLS_EXP_QOS_SEL)) {
        SHR_IF_ERR_EXIT(
            bcmlrd_field_default_get(unit,
                                     TNL_MPLS_EXP_QOS_SELECTIONt,
                                     TNL_MPLS_EXP_QOS_SELECTIONt_EXP_QOS_SELECTf,
                                     1, &def_val, &num));
        lt_entry->qos_sel = (uint8_t) def_val;
        BCMCTH_LT_FIELD_SET(lt_entry->fbmp, BCMCTH_TNL_MPLS_EXP_QOS_SEL);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Stage callback function of IMM event handler (bcmimm_lt_cb_t).
 *
 * \param [in] unit This is device unit number.
 * \param [in] sid This is the logical table ID.
 * \param [in] op_arg The operation arguments.
 * \param [in] event This is the reason for the entry event.
 * \param [in] key_flds This is a linked list of the key fields identifying
 *                        the entry.
 * \param [in] data_flds This is a linked list of the data fields in the
 *                        modified entry.
 * \param [in] context Is a pointer that was given during registration.
 *                     The callback can use this pointer to retrieve some
 *                     context.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
tnl_mpls_exp_qos_ctrl_stage(int unit,
                        bcmltd_sid_t sid,
                        const bcmltd_op_arg_t *op_arg,
                        bcmimm_entry_event_t event,
                        const bcmltd_field_t *key,
                        const bcmltd_field_t *data,
                        void *context,
                        bcmltd_fields_t *output_fields)
{
    bcmcth_tnl_mpls_exp_qos_ctrl_t lt_entry;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (tnl_mpls_exp_qos_ctrl_lt_fields_parse(unit, key, data, &lt_entry));

    lt_entry.op_arg = op_arg;
    lt_entry.glt_sid = sid;

    if (output_fields) {
        output_fields->count = 0;
    }

    switch (event) {
        case BCMIMM_ENTRY_UPDATE:
            break;
        case BCMIMM_ENTRY_INSERT:
        case BCMIMM_ENTRY_DELETE:
            SHR_IF_ERR_VERBOSE_EXIT(
                tnl_mpls_exp_qos_ctrl_default_values_init(unit, &lt_entry));
            break;
        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }
    SHR_IF_ERR_VERBOSE_EXIT(
        bcmcth_tnl_mpls_exp_qos_ctrl_set(unit, &lt_entry));

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief MPLS_EXP_QOS_CONTROL In-memory event callback structure.
 *
 * This structure contains callback functions that will be conresponding
 * to MPLS_EXP_QOS_CONTROL logical table entry commit stages.
 */
static bcmimm_lt_cb_t tnl_mpls_exp_qos_ctrl_imm_hdl = {

    /*! Validate function. */
    .validate = NULL,

    /*! Extended staging function. */
    .op_stage = tnl_mpls_exp_qos_ctrl_stage,

    /*! Commit function. */
    .commit = NULL,

    /*! Abort function. */
    .abort = NULL
};


/*!
 * \brief imm MPLS_EXP_REMARK_CONTROLt  notification input fields parsing.
 *
 * Parse imm MPLS_EXP_REMARK_CONTROLt input fields.
 *
 * \param [in] unit Unit number.
 * \param [in] key IMM input key field array.
 * \param [in] data IMM input data field array.
 * \param [out] exp to ecn map data buffer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Faild to convert \c key \c data to \c lt_info.
 */
static int
tnl_mpls_exp_remark_ctrl_lt_fields_parse(int unit,
                                         const bcmltd_field_t *key,
                                         const bcmltd_field_t *data,
                                         bcmcth_tnl_mpls_exp_remark_ctrl_t *lt_entry)
{
    const bcmltd_field_t *gen_field;
    uint32_t fid;
    uint64_t fval;
    uint32_t arr_idx;

    SHR_FUNC_ENTER(unit);

    sal_memset(lt_entry, 0, sizeof(bcmcth_tnl_mpls_exp_remark_ctrl_t));

    /* Parse key field */
    gen_field = key;
    while (gen_field) {
        fid = gen_field->id;
        fval = gen_field->data;
        arr_idx = gen_field->idx;

        switch (fid) {
            case TNL_MPLS_EXP_REMARK_SELECTIONt_TNL_MPLS_EXP_REMARK_SELECTION_ID_1f:
                BCMCTH_LT_FIELD_SET(
                    lt_entry->fbmp, BCMCTH_TNL_MPLS_EXP_REMARK_CONTROL_ID_1);
                lt_entry->map_id[arr_idx] = (uint8_t) fval;
                break;
            case TNL_MPLS_EXP_REMARK_SELECTIONt_TNL_MPLS_EXP_REMARK_SELECTION_ID_2f:
                BCMCTH_LT_FIELD_SET(
                    lt_entry->fbmp, BCMCTH_TNL_MPLS_EXP_REMARK_CONTROL_ID_2);
                lt_entry->map_id[arr_idx] = (uint8_t) fval;
                break;
            case TNL_MPLS_EXP_REMARK_SELECTIONt_TNL_MPLS_EXP_REMARK_SELECTION_ID_3f:
                BCMCTH_LT_FIELD_SET(
                    lt_entry->fbmp, BCMCTH_TNL_MPLS_EXP_REMARK_CONTROL_ID_3);
                lt_entry->map_id[arr_idx] = (uint8_t) fval;
                break;
            case TNL_MPLS_EXP_REMARK_SELECTIONt_EFFECTIVE_EXP_REMARKf:
                BCMCTH_LT_FIELD_SET(
                    lt_entry->fbmp, BCMCTH_TNL_MPLS_EFFECTIVE_EXP_REMARK_PROFILE);
                lt_entry->effective = (bool) fval;
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
            case TNL_MPLS_EXP_REMARK_SELECTIONt_EXP_REMARK_SELECTf:
                BCMCTH_LT_FIELD_SET(
                    lt_entry->fbmp, BCMCTH_TNL_MPLS_EXP_REMARK_SEL);
                lt_entry->remark_sel = (uint8_t) fval;
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
 * \param [in] lt_entry MPLS_EXP_REMARK_CONTROLt LT entry data buffer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Input parameter failed validation check.
 */
static int
tnl_mpls_exp_remark_ctrl_default_values_init(int unit,
                                             bcmcth_tnl_mpls_exp_remark_ctrl_t *lt_entry)
{
    uint64_t def_val = 0;
    uint32_t num;
    SHR_FUNC_ENTER(unit);

    /* validate input parameter. */
    SHR_NULL_CHECK(lt_entry, SHR_E_PARAM);

    if (!BCMCTH_LT_FIELD_GET(
             lt_entry->fbmp, BCMCTH_TNL_MPLS_EXP_REMARK_SEL)) {
        SHR_IF_ERR_EXIT(
            bcmlrd_field_default_get(unit,
                                     TNL_MPLS_EXP_REMARK_SELECTIONt,
                                     TNL_MPLS_EXP_REMARK_SELECTIONt_EXP_REMARK_SELECTf,
                                     1, &def_val, &num));
        lt_entry->remark_sel = (uint8_t)def_val;
        BCMCTH_LT_FIELD_SET(lt_entry->fbmp, BCMCTH_TNL_MPLS_EXP_REMARK_SEL);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Stage callback function of IMM event handler (bcmimm_lt_cb_t).
 *
 * \param [in] unit This is device unit number.
 * \param [in] sid This is the logical table ID.
 * \param [in] op_arg The operation arguments.
 * \param [in] event This is the reason for the entry event.
 * \param [in] key_flds This is a linked list of the key fields identifying
 *                        the entry.
 * \param [in] data_flds This is a linked list of the data fields in the
 *                        modified entry.
 * \param [in] context Is a pointer that was given during registration.
 *                     The callback can use this pointer to retrieve some
 *                     context.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
tnl_mpls_exp_remark_ctrl_stage(int unit,
                               bcmltd_sid_t sid,
                               const bcmltd_op_arg_t *op_arg,
                               bcmimm_entry_event_t event,
                               const bcmltd_field_t *key,
                               const bcmltd_field_t *data,
                               void *context,
                               bcmltd_fields_t *output_fields)
{
    bcmcth_tnl_mpls_exp_remark_ctrl_t lt_entry;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (tnl_mpls_exp_remark_ctrl_lt_fields_parse(unit, key, data, &lt_entry));

    lt_entry.op_arg = op_arg;
    lt_entry.glt_sid = sid;

    if (output_fields) {
        output_fields->count = 0;
    }

    switch (event) {
        case BCMIMM_ENTRY_UPDATE:
            break;
        case BCMIMM_ENTRY_INSERT:
        case BCMIMM_ENTRY_DELETE:
            SHR_IF_ERR_VERBOSE_EXIT(
                tnl_mpls_exp_remark_ctrl_default_values_init(unit, &lt_entry));
            break;
        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }
    SHR_IF_ERR_VERBOSE_EXIT(
        bcmcth_tnl_mpls_exp_remark_ctrl_set(unit, &lt_entry));

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief MPLS_EXP_REMARK_CONTROL In-memory event callback structure.
 *
 * This structure contains callback functions that will be conresponding
 * to MPLS_EXP_REMARK_CONTROL logical table entry commit stages.
 */
static bcmimm_lt_cb_t tnl_mpls_exp_remark_ctrl_imm_hdl = {

    /*! Validate function. */
    .validate = NULL,

    /*! Extended staging function. */
    .op_stage = tnl_mpls_exp_remark_ctrl_stage,

    /*! Commit function. */
    .commit = NULL,

    /*! Abort function. */
    .abort = NULL
};


/*******************************************************************************
 * Public Functions
 */

int
bcmcth_tnl_imm_register(int unit)
{
    const bcmlrd_map_t *map = NULL;
    int rv = 0;

    SHR_FUNC_ENTER(unit);

    /* To register callback for TNL_MPLS_PROTECTION_ENABLE LT. */
    rv = bcmlrd_map_get(unit, TNL_MPLS_PROTECTION_ENABLEt, &map);
    if (SHR_SUCCESS(rv) && map) {
        (void) bcmimm_lt_event_reg(unit,
                                   TNL_MPLS_PROTECTION_ENABLEt,
                                   &tnl_mpls_prot_en_imm_callback,
                                   NULL);
    }

    rv = bcmlrd_map_get(unit, TNL_IPV4_ENCAPt, &map);
    if (SHR_SUCCESS(rv) && map) {
        SHR_IF_ERR_EXIT(bcmimm_lt_event_reg(unit,
                                            TNL_IPV4_ENCAPt,
                                            &tnl_encap_imm_callback,
                                            NULL));
    }

    rv = bcmlrd_map_get(unit, TNL_IPV6_ENCAPt, &map);
    if (SHR_SUCCESS(rv) && map) {
        SHR_IF_ERR_EXIT(bcmimm_lt_event_reg(unit,
                                            TNL_IPV6_ENCAPt,
                                            &tnl_encap_imm_callback,
                                            NULL));
    }

    rv = bcmlrd_map_get(unit, TNL_MPLS_ENCAPt, &map);
    if (SHR_SUCCESS(rv) && map) {
        SHR_IF_ERR_EXIT(bcmimm_lt_event_reg(unit,
                                            TNL_MPLS_ENCAPt,
                                            &tnl_encap_imm_callback,
                                            NULL));
    }

    rv = bcmlrd_map_get(unit, TNL_MPLS_EXP_QOS_SELECTIONt, &map);
    if (SHR_SUCCESS(rv) && map) {
        (void)bcmimm_lt_event_reg(unit,
                                  TNL_MPLS_EXP_QOS_SELECTIONt,
                                  &tnl_mpls_exp_qos_ctrl_imm_hdl,
                                  NULL);
    }

    rv = bcmlrd_map_get(unit, TNL_MPLS_EXP_REMARK_SELECTIONt, &map);
    if (SHR_SUCCESS(rv) && map) {
        (void)bcmimm_lt_event_reg(unit,
                                  TNL_MPLS_EXP_REMARK_SELECTIONt,
                                  &tnl_mpls_exp_remark_ctrl_imm_hdl,
                                  NULL);
    }

exit:
    SHR_FUNC_EXIT();
}
