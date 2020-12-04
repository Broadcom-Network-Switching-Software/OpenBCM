/*! \file bcmcth_ecn_imm.c
 *
 * BCMCTH ECN IMM handler.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <sal/sal_libc.h>

#include <bcmltd/chip/bcmltd_id.h>
#include <bcmcth/bcmcth_ecn_drv.h>
#include <bcmlrd/bcmlrd_map.h>
#include <bcmimm/bcmimm_int_comp.h>
#include <bcmdrd_config.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmcth/bcmcth_util.h>


/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMCTH_ECN

/*******************************************************************************
 * IMM event handler
 */

/*!
 * \brief Stage callback function of IMM event handler (bcmimm_lt_cb_t).
 *
 * \param [in] unit This is device unit number.
 * \param [in] sid This is the logical table ID.
 * \param [in] trans_id is the transaction ID associated with this operation.
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
ecn_protocol_stage(int unit,
                   bcmltd_sid_t sid,
                   const bcmltd_op_arg_t *op_arg,
                   bcmimm_entry_event_t event,
                   const bcmltd_field_t *key,
                   const bcmltd_field_t *data,
                   void *context,
                   bcmltd_fields_t *output_fields)
{

    SHR_FUNC_ENTER(unit);

    if (output_fields) {
        output_fields->count = 0;
    }

    switch (event) {
        case BCMIMM_ENTRY_INSERT:
        case BCMIMM_ENTRY_UPDATE:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmcth_ecn_protocol_insert(unit, op_arg, event, key, data));
            break;
        case BCMIMM_ENTRY_DELETE:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmcth_ecn_protocol_delete(unit, op_arg, key));
            break;
        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
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
ecn_protocol_lookup(int unit,
                    bcmltd_sid_t sid,
                    const bcmltd_op_arg_t *op_arg,
                    void *context,
                    bcmimm_lookup_type_t lkup_type,
                    const bcmltd_fields_t *in,
                    bcmltd_fields_t *out)
{
    const bcmltd_fields_t *key;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(op_arg, SHR_E_PARAM);
    /* Return if not getting from HW. */
    if (!(op_arg->attrib & BCMLT_ENT_ATTR_GET_FROM_HW)) {
        SHR_EXIT();
    }

    /* key fields are in the "out" parameter for traverse operations */
    key = (lkup_type == BCMIMM_TRAVERSE) ? out : in;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ecn_protocol_lookup(unit, op_arg, key, out));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief ECN_PROTOCOL In-memory event callback structure.
 *
 * This structure contains callback functions that will be corresponding
 * to ECN_PROTOCOL logical table entry commit stages.
 */
static bcmimm_lt_cb_t ecn_protocol_imm_hdl = {

    /*! Validate function. */
    .validate = NULL,

    /*! Extended staging function. */
    .op_stage = ecn_protocol_stage,

    /*! Extended lookup function. */
    .op_lookup = ecn_protocol_lookup,

    /*! Commit function. */
    .commit = NULL,

    /*! Abort function. */
    .abort = NULL
};

/*!
 * \brief imm ECN_MPLS_EXP_TO_IP_ECNt notification input fields parsing.
 *
 * Parse imm ECN_MPLS_EXP_TO_IP_ECNt input fields.
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
ecn_exp_to_ecn_map_lt_fields_parse(int unit,
                                   const bcmltd_field_t *key,
                                   const bcmltd_field_t *data,
                                   bcmcth_ecn_exp_to_ecn_map_t *lt_info)
{
    const bcmltd_field_t *gen_field;
    uint32_t fid;
    uint64_t fval;

    SHR_FUNC_ENTER(unit);

    sal_memset(lt_info, 0, sizeof(bcmcth_ecn_exp_to_ecn_map_t));

    /* Parse key field */
    gen_field = key;
    while (gen_field) {
        fid = gen_field->id;
        fval = gen_field->data;

        switch (fid) {
            case ECN_MPLS_EXP_TO_IP_ECNt_ECN_MPLS_EXP_TO_IP_ECN_IDf:
                BCMCTH_LT_FIELD_SET(
                    lt_info->fbmp, BCMCTH_ECN_MPLS_EXP_TO_IP_ECN_ID);
                lt_info->map_id = (uint8_t) fval;
                break;
            case ECN_MPLS_EXP_TO_IP_ECNt_LAST_DERIVED_ECNf:
                BCMCTH_LT_FIELD_SET(
                    lt_info->fbmp, BCMCTH_ECN_LAST_DERIVED_ECN);
                lt_info->ecn = (uint8_t) fval;
                break;
            case ECN_MPLS_EXP_TO_IP_ECNt_EXPf:
                BCMCTH_LT_FIELD_SET(
                    lt_info->fbmp, BCMCTH_ECN_EXP);
                lt_info->exp = (uint8_t) fval;
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
            case ECN_MPLS_EXP_TO_IP_ECNt_ECNf:
                BCMCTH_LT_FIELD_SET(
                    lt_info->fbmp, BCMCTH_ECN_NEW);
                lt_info->new_ecn = (uint8_t) fval;
                break;
            case ECN_MPLS_EXP_TO_IP_ECNt_DROPf:
                BCMCTH_LT_FIELD_SET(
                    lt_info->fbmp, BCMCTH_ECN_DROP);
                lt_info->drop = (bool) fval;
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
 * \param [in] lt_entry ECN_MPLS_EXP_TO_IP_ECN LT entry data buffer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Input parameter failed validation check.
 */
static int
ecn_exp_to_ecn_map_default_values_init(int unit,
                                       bcmcth_ecn_exp_to_ecn_map_t *lt_info)
{
    uint64_t def_val = 0;
    uint32_t num;
    SHR_FUNC_ENTER(unit);

    /* validate input parameter. */
    SHR_NULL_CHECK(lt_info, SHR_E_PARAM);

    if (!BCMCTH_LT_FIELD_GET(
             lt_info->fbmp, BCMCTH_ECN_NEW)) {
        SHR_IF_ERR_EXIT(
            bcmlrd_field_default_get(unit,
                                     ECN_MPLS_EXP_TO_IP_ECNt,
                                     ECN_MPLS_EXP_TO_IP_ECNt_ECNf,
                                     1, &def_val, &num));
        lt_info->new_ecn = (uint8_t) def_val;
        BCMCTH_LT_FIELD_SET(lt_info->fbmp, BCMCTH_ECN_NEW);
    }

    if (!BCMCTH_LT_FIELD_GET(
             lt_info->fbmp, BCMCTH_ECN_DROP)) {
        SHR_IF_ERR_EXIT(
            bcmlrd_field_default_get(unit,
                                     ECN_MPLS_EXP_TO_IP_ECNt,
                                     ECN_MPLS_EXP_TO_IP_ECNt_DROPf,
                                     1, &def_val, &num));
        lt_info->drop = def_val ? true : false;
        BCMCTH_LT_FIELD_SET(lt_info->fbmp, BCMCTH_ECN_DROP);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Stage callback function of IMM event handler (bcmimm_lt_cb_t).
 *
 * \param [in] unit This is device unit number.
 * \param [in] sid This is the logical table ID.
 * \param [in] trans_id is the transaction ID associated with this operation.
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
ecn_exp_to_ecn_map_stage(int unit,
                         bcmltd_sid_t sid,
                         uint32_t trans_id,
                         bcmimm_entry_event_t event,
                         const bcmltd_field_t *key,
                         const bcmltd_field_t *data,
                         void *context,
                         bcmltd_fields_t *output_fields)
{
    bcmcth_ecn_exp_to_ecn_map_t lt_info;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (ecn_exp_to_ecn_map_lt_fields_parse(unit, key, data, &lt_info));

    lt_info.trans_id = trans_id;
    lt_info.glt_sid = sid;

    if (output_fields) {
        output_fields->count = 0;
    }

    switch (event) {
        case BCMIMM_ENTRY_UPDATE:
            break;
        case BCMIMM_ENTRY_INSERT:
        case BCMIMM_ENTRY_DELETE:
            SHR_IF_ERR_VERBOSE_EXIT(
                ecn_exp_to_ecn_map_default_values_init(unit, &lt_info));
            break;
        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }
    SHR_IF_ERR_VERBOSE_EXIT(
        bcmcth_ecn_exp_to_ecn_map_set(unit, &lt_info));

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief ECN_MPLS_EXP_TO_IP_ECN In-memory event callback structure.
 *
 * This structure contains callback functions that will be conresponding
 * to ECN_MPLS_EXP_TO_IP_ECN logical table entry commit stages.
 */
static bcmimm_lt_cb_t ecn_exp_to_ecn_map_imm_hdl = {

    /*! Validate function. */
    .validate = NULL,

    /*! Staging function. */
    .stage = ecn_exp_to_ecn_map_stage,

    /*! Commit function. */
    .commit = NULL,

    /*! Abort function. */
    .abort = NULL
};


static int
bcmcth_ecn_imm_register(int unit)
{
    const bcmlrd_map_t *map = NULL;
    int rv;

    SHR_FUNC_ENTER(unit);

    /*
     * To register callback for ECN_PROTOCOL LT here.
     */
    rv = bcmlrd_map_get(unit, ECN_PROTOCOLt, &map);
    if (SHR_SUCCESS(rv) && map) {
        SHR_IF_ERR_EXIT
            (bcmimm_lt_event_reg(unit, ECN_PROTOCOLt, &ecn_protocol_imm_hdl, NULL));
    }

    rv = bcmlrd_map_get(unit, ECN_MPLS_EXP_TO_IP_ECNt, &map);
    if (SHR_SUCCESS(rv) && map) {
        (void)bcmimm_lt_event_reg(unit, ECN_MPLS_EXP_TO_IP_ECNt, &ecn_exp_to_ecn_map_imm_hdl, NULL);
    }

exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public Functions
 */

int
bcmcth_ecn_imm_init(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmcth_ecn_imm_register(unit));
exit:
    SHR_FUNC_EXIT();
}
