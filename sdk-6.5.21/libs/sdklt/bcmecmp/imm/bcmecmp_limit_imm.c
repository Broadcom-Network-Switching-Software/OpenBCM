/*! \file bcmecmp_limit_imm.c
 *
 * BCMECMP ECMP_LIMIT_CONTROLt IMM handler.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_libc.h>

#include <shr/shr_debug.h>
#include <shr/shr_bitop.h>

#include <bcmbd/bcmbd_ts.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmlrd/bcmlrd_map.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmdrd_config.h>
#include <bcmimm/bcmimm_int_comp.h>
#include <bcmecmp/bcmecmp_group.h>
#include <bcmecmp/bcmecmp_imm.h>
#include <bcmecmp/bcmecmp_common_imm.h>

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMLTX_ECMP
#define ECMP_LIMIT_CTRL_LT_SID ECMP_LIMIT_CONTROLt
#define BCMECMP_LIMIT_FIELD_COUNT_MAX 5

static shr_famm_hdl_t bcmecmp_limit_famm_hdl[BCMDRD_CONFIG_MAX_UNITS];

/*!
 * \brief ECMP limit control input fields parsing.
 *
 * \param [in] unit Unit number.
 * \param [in] data IMM input data field array.
 * \param [out] ecmp_limit Protection entry buffer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Faild to parse.
 */
static int
ecmp_limit_parse(int unit,
              const bcmltd_field_t *data,
              bcmecmp_limit_t *ecmp_limit)
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
            case ECMP_LIMIT_CONTROLt_MEMBER_OVERLAY_LIMITf:
                ecmp_limit->num_o_memb_bank = fval;
                ecmp_limit->num_o_memb_bank_valid = TRUE;
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
ecmp_limit_ctrl_callback_stage(int unit,
                            bcmltd_sid_t sid,
                            const bcmltd_op_arg_t *op_arg,
                            bcmimm_entry_event_t event_reason,
                            const bcmltd_field_t *key,
                            const bcmltd_field_t *data,
                            void *context,
                            bcmltd_fields_t *output_fields)
{
    bcmecmp_limit_t ecmp_limit;
    bcmecmp_limit_t cur_ecmp_limit;

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
            sal_memset(&ecmp_limit, 0, sizeof(bcmecmp_limit_t));
            SHR_IF_ERR_EXIT
                (ecmp_limit_parse(unit, data, &ecmp_limit));
            SHR_IF_ERR_EXIT
                (bcmecmp_limit_get(unit, op_arg, sid, &cur_ecmp_limit));
            if (ecmp_limit.num_o_memb_bank == cur_ecmp_limit.num_o_memb_bank) {
               LOG_VERBOSE(BSL_LOG_MODULE,
                          (BSL_META_U(unit, "The number of overlay memb bank"
                                      " is not changed, return.\n")));
               SHR_EXIT();
            }
            SHR_IF_ERR_EXIT
                (bcmecmp_limit_set(unit, op_arg, sid, &ecmp_limit));
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
ecmp_limit_ctrl_callback_lookup(int unit,
                             bcmltd_sid_t sid,
                             const bcmltd_op_arg_t *op_arg,
                             void *context,
                             bcmimm_lookup_type_t lkup_type,
                             const bcmltd_fields_t *in,
                             bcmltd_fields_t *out)
{
    bcmecmp_limit_t ecmp_limit;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(op_arg, SHR_E_PARAM);
    /* Return if not getting from HW. */
    if (!(op_arg->attrib & BCMLT_ENT_ATTR_GET_FROM_HW)) {
        SHR_EXIT();
    }

    /* No key fields. */
    sal_memset(&ecmp_limit, 0, sizeof(bcmecmp_limit_t));
    ecmp_limit.num_o_memb_bank_valid = 1;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmecmp_limit_get(unit, op_arg, sid, &ecmp_limit));

    SHR_IF_ERR_EXIT
        (bcmecmp_lt_field_update(unit,
                                 out,
                                 L3_LIMIT_CONTROLt_NHOP_OVERLAY_LIMITf,
                                 ecmp_limit.num_o_memb_bank));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief L3_LIMIT_CONTROL In-memory event callback structure.
 *
 * This structure contains callback functions corresponding to
 * L3_LIMIT_CONTROL logical table entry commit stages.
 */
static bcmimm_lt_cb_t ecmp_limit_ctrl_imm_callback = {

    /*! Validate function. */
    .validate = NULL,

    /*! Extended staging function. */
    .op_stage    = ecmp_limit_ctrl_callback_stage,

    /*! Extended lookup function. */
    .op_lookup    = ecmp_limit_ctrl_callback_lookup,

    /*! Commit function. */
    .commit   = NULL,

    /*! Abort function. */
    .abort    = NULL
};

/*******************************************************************************
 * Public Functions
 */
int
bcmecmp_limit_imm_register(int unit)
{
    const bcmlrd_map_t *map = NULL;
    int rv = 0;

    SHR_FUNC_ENTER(unit);

    /* To register callback for ECMP_LIMIT_CONTROL LT. */
    rv = bcmlrd_map_get(unit, ECMP_LIMIT_CTRL_LT_SID, &map);
    if (SHR_SUCCESS(rv) && map) {
        (void) bcmimm_lt_event_reg(unit,
                                   ECMP_LIMIT_CTRL_LT_SID,
                                   &ecmp_limit_ctrl_imm_callback,
                                   NULL);
    }

    SHR_IF_ERR_EXIT(SHR_E_NONE);
exit:
    SHR_FUNC_EXIT();
}

int
bcmecmp_limit_control_pre_insert(int u)
{
    bcmltd_fields_t fields = {0};
    uint32_t f_cnt = 0;

    SHR_FUNC_ENTER(u);
    if (!bcmecmp_limit_famm_hdl[u]) {
        SHR_IF_ERR_EXIT(
            shr_famm_hdl_init(BCMECMP_LIMIT_FIELD_COUNT_MAX,
                              &bcmecmp_limit_famm_hdl[u]));
    }
    fields.field = shr_famm_alloc(bcmecmp_limit_famm_hdl[u],
                                  BCMECMP_LIMIT_FIELD_COUNT_MAX);
    if (fields.field == NULL) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }

    f_cnt = 0;
    fields.field[f_cnt  ]->id   = ECMP_LIMIT_CONTROLt_MEMBER_OVERLAY_LIMITf;
    fields.field[f_cnt++]->data = 0;
    fields.count = f_cnt;
    SHR_IF_ERR_EXIT(
        bcmimm_entry_insert(u, ECMP_LIMIT_CONTROLt, &fields));
exit:
    if (fields.field) {
        shr_famm_free(bcmecmp_limit_famm_hdl[u], fields.field,
                      BCMECMP_LIMIT_FIELD_COUNT_MAX);
    }
    if (bcmecmp_limit_famm_hdl[u]) {
        shr_famm_hdl_delete(bcmecmp_limit_famm_hdl[u]);
        bcmecmp_limit_famm_hdl[u] = 0;
    }
    SHR_FUNC_EXIT();
}
