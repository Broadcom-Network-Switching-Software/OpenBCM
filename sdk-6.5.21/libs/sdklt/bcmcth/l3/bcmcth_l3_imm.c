/*! \file bcmcth_l3_imm.c
 *
 * L3 interface to in-memory table.
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
#include <bcmcth/bcmcth_l3_drv.h>
#include <bcmcth/bcmcth_l3_util.h>
#include <bcmcth/bcmcth_l3_prot.h>
#include <bcmcth/bcmcth_l3_bank.h>
#include <bcmcth/bcmcth_imm_util.h>
/*******************************************************************************
 * Local definitions
 */

/* BSL Module */
#define BSL_LOG_MODULE  BSL_LS_BCMCTH_L3
#define L3_BANK_CTRL_LT_SID L3_LIMIT_CONTROLt

/*******************************************************************************
 * Private functions
 */

/*!
 * \brief L3_PROTECTION_ENABLEt input fields parsing.
 *
 * Parse IMM L3_PROTECTION_ENABLEt input fields.
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
l3_prot_enable_parse(int unit,
                     const bcmltd_field_t *key,
                     const bcmltd_field_t *data,
                     bcmcth_l3_prot_en_lt_t *prot_entry)
{
    const bcmltd_field_t *gen_field;
    uint32_t fid;
    uint64_t fval;

    SHR_FUNC_ENTER(unit);

    /* Parse key. */
    if (key->id != L3_PROTECTION_ENABLEt_NHOP_IDf) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    prot_entry->nhop_id = key->data;

    /* Parse data field. */
    gen_field = data;
    while (gen_field) {
        fid = gen_field->id;
        fval = gen_field->data;
        switch (fid) {
        case L3_PROTECTION_ENABLEt_PROTECTIONf:
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
 * \brief L3_PROTECTION_ENABLE IMM table change callback for staging.
 *
 * Handle L3_PROTECTION_ENABLE IMM table change events.
 *
 * \param [in] unit Unit number.
 * \param [in] sid This is the logical table ID.
 * \param [in] trans_id is the transaction ID associated with this operation.
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
l3_prot_en_callback_stage(int unit,
                          bcmltd_sid_t sid,
                          uint32_t trans_id,
                          bcmimm_entry_event_t event_reason,
                          const bcmltd_field_t *key,
                          const bcmltd_field_t *data,
                          void *context,
                          bcmltd_fields_t *output_fields)
{
    bcmcth_l3_prot_en_lt_t prot_entry;
    uint32_t nhop_id;
    bool protection;

    SHR_FUNC_ENTER(unit);

    if (output_fields) {
        output_fields->count = 0;
    }

    sal_memset(&prot_entry, 0, sizeof(prot_entry));
    SHR_IF_ERR_VERBOSE_EXIT
        (l3_prot_enable_parse(unit, key, data, &prot_entry));
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
        (bcmcth_l3_prot_enable_set(unit, trans_id, sid,
                                         nhop_id, protection));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief L3_PROTECTION_ENABLE In-memory event callback structure.
 *
 * This structure contains callback functions corresponding to
 * L3_PROTECTION_ENABLE logical table entry commit stages.
 */
static bcmimm_lt_cb_t l3_prot_en_imm_callback = {

    /*! Validate function. */
    .validate = NULL,

    /*! Staging function. */
    .stage    = l3_prot_en_callback_stage,

    /*! Commit function. */
    .commit   = NULL,

    /*! Abort function. */
    .abort    = NULL
};

/*!
 * \brief L3 limit control input fields parsing.
 *
 * \param [in] unit Unit number.
 * \param [in] data IMM input data field array.
 * \param [out] prot_entry Protection entry buffer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Faild to parse.
 */
static int
l3_bank_parse(int unit,
              const bcmltd_field_t *data,
              bcmcth_l3_bank_t *l3_bank)
{
    const bcmltd_field_t *gen_field;
    uint32_t fid;
    uint64_t fval;

    SHR_FUNC_ENTER(unit);

    /* Parse data field. */
    gen_field = data;
    while (gen_field) {
        fid = gen_field->id;
        fval = gen_field->data;
        switch (fid) {
        case L3_LIMIT_CONTROLt_NHOP_OVERLAY_LIMITf:
            l3_bank->num_o_nhop = fval;
            l3_bank->num_o_nhop_valid = TRUE;
            break;
        case L3_LIMIT_CONTROLt_EIF_OVERLAY_LIMITf:
            l3_bank->num_o_eif= fval;
            l3_bank->num_o_eif_valid = TRUE;
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
 * \brief Validate callback function of IMM event handler (bcmimm_lt_cb_t).
 *
 * \param [in] unit This is device unit number.
 * \param [in] sid This is the logical table ID.
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
l3_bank_ctrl_callback_validate(int unit,
                                   bcmltd_sid_t sid,
                                   bcmimm_entry_event_t event,
                                   const bcmltd_field_t *key,
                                   const bcmltd_field_t *data,
                                   void *context)
{   bcmcth_l3_bank_t l3_bank;
    SHR_FUNC_ENTER(unit);

    switch (event) {
        case BCMIMM_ENTRY_INSERT:
        case BCMIMM_ENTRY_DELETE:
            SHR_ERR_EXIT(SHR_E_ACCESS);
            break;
        case BCMIMM_ENTRY_UPDATE:
            sal_memset(&l3_bank, 0, sizeof(bcmcth_l3_bank_t));
            SHR_IF_ERR_EXIT
                (l3_bank_parse(unit, data, &l3_bank));
            SHR_IF_ERR_EXIT
                (bcmcth_l3_bank_usage_check(unit, 0, l3_bank));
            break;
        default:
            break;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief L3 bank control IMM table change callback for staging.
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
l3_bank_ctrl_callback_stage(int unit,
                            bcmltd_sid_t sid,
                            const bcmltd_op_arg_t *op_arg,
                            bcmimm_entry_event_t event_reason,
                            const bcmltd_field_t *key,
                            const bcmltd_field_t *data,
                            void *context,
                            bcmltd_fields_t *output_fields)
{
    bcmcth_l3_bank_t l3_bank;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(op_arg, SHR_E_PARAM);
    if (output_fields) {
        output_fields->count = 0;
    }

    switch (event_reason) {
        case BCMIMM_ENTRY_INSERT:
        case BCMIMM_ENTRY_DELETE:
            SHR_ERR_EXIT(SHR_E_ACCESS);
            break;
        case BCMIMM_ENTRY_UPDATE:
            sal_memset(&l3_bank, 0, sizeof(bcmcth_l3_bank_t));
            SHR_IF_ERR_EXIT
                (l3_bank_parse(unit, data, &l3_bank));
            SHR_IF_ERR_EXIT
                (bcmcth_l3_bank_set(unit, op_arg, sid, l3_bank));
            break;
        default:
            break;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Lookup an entry from the L3 bank control Table.
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
l3_bank_ctrl_callback_lookup(int unit,
                             bcmltd_sid_t sid,
                             const bcmltd_op_arg_t *op_arg,
                             void *context,
                             bcmimm_lookup_type_t lkup_type,
                             const bcmltd_fields_t *in,
                             bcmltd_fields_t *out)
{
    bcmcth_l3_bank_t l3_bank;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(op_arg, SHR_E_PARAM);
    /* Return if not getting from HW. */
    if (!(op_arg->attrib & BCMLT_ENT_ATTR_GET_FROM_HW)) {
        SHR_EXIT();
    }

    /* No key fields. */
    sal_memset(&l3_bank, 0, sizeof(bcmcth_l3_bank_t));
    l3_bank.num_o_nhop_valid = 1;
    l3_bank.num_o_eif_valid = 1;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_l3_bank_get(unit, op_arg, sid, &l3_bank));

    SHR_IF_ERR_EXIT(
        bcmcth_imm_field_update(unit,
                                out,
                                L3_LIMIT_CONTROLt_NHOP_OVERLAY_LIMITf,
                                l3_bank.num_o_nhop));
    SHR_IF_ERR_EXIT(
        bcmcth_imm_field_update(unit,
                                out,
                                L3_LIMIT_CONTROLt_EIF_OVERLAY_LIMITf,
                                l3_bank.num_o_eif));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief L3_LIMIT_CONTROL In-memory event callback structure.
 *
 * This structure contains callback functions corresponding to
 * L3_LIMIT_CONTROL logical table entry commit stages.
 */
static bcmimm_lt_cb_t l3_bank_ctrl_imm_callback = {

    /*! Validate function. */
    .validate = l3_bank_ctrl_callback_validate,

    /*! Extended staging function. */
    .op_stage    = l3_bank_ctrl_callback_stage,

    /*! Extended lookup function. */
    .op_lookup    = l3_bank_ctrl_callback_lookup,

    /*! Commit function. */
    .commit   = NULL,

    /*! Abort function. */
    .abort    = NULL
};

/*******************************************************************************
 * Public Functions
 */

int
bcmcth_l3_imm_register(int unit)
{
    const bcmlrd_map_t *map = NULL;
    int rv = 0;

    SHR_FUNC_ENTER(unit);

    /* To register callback for L3_PROTECTION_ENABLE LT. */
    rv = bcmlrd_map_get(unit, L3_PROTECTION_ENABLEt, &map);
    if (SHR_SUCCESS(rv) && map) {
        (void) bcmimm_lt_event_reg(unit,
                                   L3_PROTECTION_ENABLEt,
                                   &l3_prot_en_imm_callback,
                                   NULL);
    }

    /* To register callback for L3_LIMIT_CONTROL LT. */
    rv = bcmlrd_map_get(unit, L3_BANK_CTRL_LT_SID, &map);
    if (SHR_SUCCESS(rv) && map) {
        (void) bcmimm_lt_event_reg(unit,
                                   L3_BANK_CTRL_LT_SID,
                                   &l3_bank_ctrl_imm_callback,
                                   NULL);
    }

    SHR_IF_ERR_EXIT(SHR_E_NONE);
exit:
    SHR_FUNC_EXIT();
}
