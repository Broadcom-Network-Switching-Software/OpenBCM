/*! \file bcmcth_mon_imm.c
 *
 * BCMCTH Monitor IMM handler.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <shr/shr_debug.h>
#include <sal/sal_libc.h>

#include <bcmltd/chip/bcmltd_id.h>
#include <bcmcth/bcmcth_imm_util.h>
#include <bcmcth/bcmcth_mon_drv.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmlrd/bcmlrd_map.h>
#include <bcmimm/bcmimm_int_comp.h>
#include <bcmdrd_config.h>
#include <bcmcth/bcmcth_mon_collector.h>
#include <bcmcth/bcmcth_mon_telemetry_drv.h>
#include <bcmcth/bcmcth_mon_int_drv.h>
#include <bcmcth/bcmcth_mon_flowtracker_drv.h>
#include <bcmlrd/bcmlrd_client.h>

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMCTH_MON

/*******************************************************************************
 * Local definitions
 */

/*******************************************************************************
 * Private Functions
 */

/*!
 * \brief imm MON_ING_TRACE_EVENTt notification input fields parsing.
 *
 * Parse imm MON_ING_TRACE_EVENTt input fields.
 *
 * \param [in] unit Unit number.
 * \param [in] key IMM input key field array.
 * \param [in] data IMM input data field array.
 * \param [out] entry Trace event info.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Faild to convert \c key \c data to \c ltcfg.
 */
static int
mon_ing_trace_lt_fields_parse(
    int unit,
    const bcmltd_field_t *key,
    const bcmltd_field_t *data,
    bcmcth_mon_trace_t *entry)
{
    const bcmltd_field_t *gen_field;
    uint32_t fid;
    uint64_t fval;
    uint32_t idx;

    SHR_FUNC_ENTER(unit);

    sal_memset(entry, 0, sizeof(*entry));

    /* Parse key field */
    gen_field = key;
    while (gen_field) {
        fid = gen_field->id;
        fval = gen_field->data;

        switch (fid) {
            case MON_ING_TRACE_EVENTt_MON_ING_TRACE_EVENT_IDf:
                entry->ievent = fval;
                SHR_BITSET(entry->fbmp, fid);
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
        idx = gen_field->idx;

        switch (fid) {
            case MON_ING_TRACE_EVENTt_CPUf:
                entry->cpu = fval;
                SHR_BITSET(entry->fbmp, fid);
                break;
            case MON_ING_TRACE_EVENTt_MIRRORf:
                entry->mirr[idx] = fval;
                SHR_BITSET(entry->fbmp, fid);
                SHR_BITSET(entry->fbmp_mir, idx);
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
 * \brief imm MON_ING_DROP_EVENTt notification input fields parsing.
 *
 * Parse imm MON_ING_DROP_EVENTt input fields.
 *
 * \param [in] unit Unit number.
 * \param [in] key IMM input key field array.
 * \param [in] data IMM input data field array.
 * \param [out] drop Drop event info.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Faild to convert \c key \c data to \c ltcfg.
 */
static int
mon_ing_drop_lt_fields_parse(
    int unit,
    const bcmltd_field_t *key,
    const bcmltd_field_t *data,
    bcmcth_mon_drop_t *entry)
{
    const bcmltd_field_t *gen_field;
    uint32_t fid;
    uint64_t fval;

    SHR_FUNC_ENTER(unit);

    sal_memset(entry, 0, sizeof(*entry));

    /* Parse key field */
    gen_field = key;
    while (gen_field) {
        fid = gen_field->id;
        fval = gen_field->data;

        switch (fid) {
            case MON_ING_DROP_EVENTt_MON_ING_DROP_EVENT_IDf:
                entry->ievent = fval;
                SHR_BITSET(entry->fbmp, fid);
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
            case MON_ING_DROP_EVENTt_CPUf:
                entry->cpu = fval;
                SHR_BITSET(entry->fbmp, fid);
                break;
            case MON_ING_DROP_EVENTt_MIRRORf:
                entry->mirr = fval;
                SHR_BITSET(entry->fbmp, fid);
                break;
            case MON_ING_DROP_EVENTt_LOOPBACKf:
                entry->loopback = fval;
                SHR_BITSET(entry->fbmp, fid);
                break;
            default:
                SHR_ERR_EXIT(SHR_E_PARAM);
        }
        gen_field = gen_field->next;
    }

exit:
    SHR_FUNC_EXIT();
}

/* MON counter fid mapping. */
const uint32_t mon_ctr_lt_fid_map[BCMCTH_MON_CTR_COUNT] =
{
    /* BCMCTH_MON_CTR_ETRAP_CANDIDATE_FILTER_EXCEEDED */
    CTR_ETRAPt_CANDIDATE_FILTER_EXCEEDEDf,

    /* BCMCTH_MON_CTR_ETRAP_FLOW_INSERT_SUCCESS */
    CTR_ETRAPt_FLOW_INSERT_SUCCESSf,

    /* BCMCTH_MON_CTR_ETRAP_FLOW_INSERT_FAILURE */
    CTR_ETRAPt_FLOW_INSERT_FAILUREf,

    /* BCMCTH_MON_CTR_ETRAP_FLOW_ELEPHANT */
    CTR_ETRAPt_FLOW_ELEPHANTf
};

/*!
 * \brief imm MON related CTR_XXXt notification input fields parsing.
 *
 * Parse imm CTR_XXXt input fields.
 *
 * \param [in] unit Unit number.
 * \param [in] entry Counter info.
 * \param [out] output_fields IMM input data field array.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Faild to convert \c ltcfg to \c data.
 */
static int
mon_ctr_lt_fields_lookup(
    int unit,
    bcmcth_mon_ctr_t *entry,
    bcmltd_fields_t *output_fields)
{
    bcmcth_mon_ctr_type_t ctype;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(entry, SHR_E_PARAM);
    SHR_NULL_CHECK(output_fields, SHR_E_PARAM);

    if (output_fields) {
        output_fields->count = 0;
    }

    for (ctype = BCMCTH_MON_CTR_FIRST;
         ctype < BCMCTH_MON_CTR_COUNT;
         ctype++) {

        if (SHR_BITGET(entry->fbmp, ctype)) {
            output_fields->field[output_fields->count]->id =
                mon_ctr_lt_fid_map[ctype];
            output_fields->field[output_fields->count]->data =
                entry->cnt[ctype];
            output_fields->count++;
        }
    }
exit:
    SHR_FUNC_EXIT();
}

static int
mon_ing_trace_cpu_lt_update(
    int unit,
    uint32_t trans_id,
    bcmltd_sid_t sid,
    bcmcth_mon_trace_t *entry)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_mon_trace_op(unit, trans_id, sid,
                             BCMCTH_MON_ING_TRACE_CPU_OP_SET, entry));
exit:
    SHR_FUNC_EXIT();
}

static int
mon_ing_trace_cpu_lt_delete(
    int unit,
    uint32_t trans_id,
    bcmltd_sid_t sid,
    bcmcth_mon_trace_t *entry)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_mon_trace_op(unit, trans_id, sid,
                             BCMCTH_MON_ING_TRACE_CPU_OP_DEL, entry));
exit:
    SHR_FUNC_EXIT();
}

static int
mon_redirect_trace_lt_update(
    int unit,
    uint32_t trans_id,
    bcmltd_sid_t sid,
    bcmcth_mon_redirect_trace_t *entry)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_mon_redirect_trace_op(unit,
                                      trans_id,
                                      sid,
                                      BCMCTH_MON_EGR_TRACE_REDIRECT_OP_SET,
                                      entry));
exit:
    SHR_FUNC_EXIT();
}

static int
mon_redirect_trace_lt_delete(
    int unit,
    uint32_t trans_id,
    bcmltd_sid_t sid,
    bcmcth_mon_redirect_trace_t *entry)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_mon_redirect_trace_op(unit,
                                      trans_id,
                                      sid,
                                      BCMCTH_MON_EGR_TRACE_REDIRECT_OP_DEL,
                                      entry));
exit:
    SHR_FUNC_EXIT();
}

static int
mon_ing_drop_cpu_lt_update(
    int unit,
    uint32_t trans_id,
    bcmltd_sid_t sid,
    bcmcth_mon_drop_t *entry)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_mon_drop_op(unit, trans_id, sid,
                            BCMCTH_MON_ING_DROP_CPU_OP_SET, entry));
exit:
    SHR_FUNC_EXIT();
}

static int
mon_ing_drop_cpu_lt_delete(
    int unit,
    uint32_t trans_id,
    bcmltd_sid_t sid,
    bcmcth_mon_drop_t *entry)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_mon_drop_op(unit, trans_id, sid,
                            BCMCTH_MON_ING_DROP_CPU_OP_DEL, entry));
exit:
    SHR_FUNC_EXIT();

}

static int
mon_ing_trace_mirr_lt_update(
    int unit,
    uint32_t trans_id,
    bcmltd_sid_t sid,
    bcmcth_mon_trace_t *entry)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_mon_trace_op(unit, trans_id, sid,
                             BCMCTH_MON_ING_TRACE_MIRR_OP_SET, entry));
exit:
    SHR_FUNC_EXIT();
}

static int
mon_ing_trace_mirr_lt_delete(
    int unit,
    uint32_t trans_id,
    bcmltd_sid_t sid,
    bcmcth_mon_trace_t *entry)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_mon_trace_op(unit, trans_id, sid,
                             BCMCTH_MON_ING_TRACE_MIRR_OP_DEL, entry));
exit:
    SHR_FUNC_EXIT();

}

static int
mon_ing_drop_mirr_lt_update(
    int unit,
    uint32_t trans_id,
    bcmltd_sid_t sid,
    bcmcth_mon_drop_t *entry)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_mon_drop_op(unit, trans_id, sid,
                            BCMCTH_MON_ING_DROP_MIRR_OP_SET, entry));
exit:
    SHR_FUNC_EXIT();
}

static int
mon_ing_drop_mirr_lt_delete(
    int unit,
    uint32_t trans_id,
    bcmltd_sid_t sid,
    bcmcth_mon_drop_t *entry)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_mon_drop_op(unit, trans_id, sid,
                            BCMCTH_MON_ING_DROP_MIRR_OP_DEL, entry));
exit:
    SHR_FUNC_EXIT();

}

static int
mon_ing_drop_loopback_lt_update(
    int unit,
    uint32_t trans_id,
    bcmltd_sid_t sid,
    bcmcth_mon_drop_t *entry)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_mon_drop_op(unit, trans_id, sid,
                            BCMCTH_MON_ING_DROP_LOOPBACK_OP_SET, entry));
exit:
    SHR_FUNC_EXIT();
}

static int
mon_ing_drop_loopback_lt_delete(
    int unit,
    uint32_t trans_id,
    bcmltd_sid_t sid,
    bcmcth_mon_drop_t *entry)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_mon_drop_op(unit, trans_id, sid,
                            BCMCTH_MON_ING_DROP_LOOPBACK_OP_DEL, entry));
exit:
    SHR_FUNC_EXIT();

}
static int
mon_ctr_lt_get(
    int unit,
    uint32_t trans_id,
    bcmltd_sid_t sid,
    bcmcth_mon_ctr_t *entry)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_mon_ctr_op(unit, trans_id, sid,
                           BCMCTH_MON_CTR_OP_GET, entry));
exit:
    SHR_FUNC_EXIT();
}

static int
mon_ctr_lt_clear(int unit)
{
    bcmltd_sid_t sid = CTR_ETRAPt;
    uint32_t trans_id = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_mon_ctr_op(unit, trans_id, sid,
                           BCMCTH_MON_CTR_OP_CLR, NULL));
exit:
    SHR_FUNC_EXIT();
}

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
mon_ing_trace_stage(
    int unit,
    bcmltd_sid_t sid,
    uint32_t trans_id,
    bcmimm_entry_event_t event,
    const bcmltd_field_t *key,
    const bcmltd_field_t *data,
    void *context,
    bcmltd_fields_t *output_fields)
{
    bcmcth_mon_trace_t entry;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mon_ing_trace_lt_fields_parse(unit, key, data, &entry));

    if (output_fields) {
        output_fields->count = 0;
    }
    switch (event) {
        case BCMIMM_ENTRY_INSERT:
        case BCMIMM_ENTRY_UPDATE:
            if (SHR_BITGET(entry.fbmp, MON_ING_TRACE_EVENTt_CPUf)) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (mon_ing_trace_cpu_lt_update(unit, trans_id, sid, &entry));
            }
            if (SHR_BITGET(entry.fbmp, MON_ING_TRACE_EVENTt_MIRRORf)) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (mon_ing_trace_mirr_lt_update(unit, trans_id, sid, &entry));
            }
            break;
        case BCMIMM_ENTRY_DELETE:
            SHR_IF_ERR_VERBOSE_EXIT
                (mon_ing_trace_cpu_lt_delete(unit, trans_id, sid, &entry));
            SHR_IF_ERR_VERBOSE_EXIT
                (mon_ing_trace_mirr_lt_delete(unit, trans_id, sid, &entry));
            break;
        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
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
mon_ing_drop_stage(
    int unit,
    bcmltd_sid_t sid,
    uint32_t trans_id,
    bcmimm_entry_event_t event,
    const bcmltd_field_t *key,
    const bcmltd_field_t *data,
    void *context,
    bcmltd_fields_t *output_fields)
{

    bcmcth_mon_drop_t entry;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mon_ing_drop_lt_fields_parse(unit, key, data, &entry));

    if (output_fields) {
        output_fields->count = 0;
    }
    switch (event) {
        case BCMIMM_ENTRY_INSERT:
        case BCMIMM_ENTRY_UPDATE:
            if (SHR_BITGET(entry.fbmp, MON_ING_DROP_EVENTt_CPUf)) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (mon_ing_drop_cpu_lt_update(unit, trans_id, sid, &entry));
            }
            if (SHR_BITGET(entry.fbmp, MON_ING_DROP_EVENTt_MIRRORf)) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (mon_ing_drop_mirr_lt_update(unit, trans_id, sid, &entry));
            }
            if (SHR_BITGET(entry.fbmp, MON_ING_DROP_EVENTt_LOOPBACKf)) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (mon_ing_drop_loopback_lt_update(unit,
                                                     trans_id,
                                                     sid,
                                                     &entry));
            }
            break;
        case BCMIMM_ENTRY_DELETE:
            SHR_IF_ERR_VERBOSE_EXIT
                (mon_ing_drop_cpu_lt_delete(unit, trans_id, sid, &entry));
            SHR_IF_ERR_VERBOSE_EXIT
                (mon_ing_drop_mirr_lt_delete(unit, trans_id, sid, &entry));
            SHR_IF_ERR_VERBOSE_EXIT
                (mon_ing_drop_loopback_lt_delete(unit, trans_id, sid, &entry));
            break;
        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief imm MON_EGR_REDIRECT_TRACE_EVENTt notification input fields parsing.
 *
 * Parse imm MON_EGR_REDIRECT_TRACE_EVENTt input fields.
 *
 * \param [in]   unit  Unit number.
 * \param [in]   key   IMM input key field array.
 * \param [in]   data  IMM input data field array.
 * \param [out]  entry Redirect trace event info.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Faild to convert key and data to ltcfg.
 */
static int
mon_redirect_trace_lt_fields_parse(
    int unit,
    const bcmltd_field_t *key,
    const bcmltd_field_t *data,
    bcmcth_mon_redirect_trace_t *entry)
{
    const bcmltd_field_t *gen_field;
    uint32_t fid;
    uint64_t fval;

    SHR_FUNC_ENTER(unit);

    sal_memset(entry, 0, sizeof(*entry));

    /* Parse key field */
    gen_field = key;
    while (gen_field) {
        fid = gen_field->id;
        fval = gen_field->data;

        switch (fid) {
            case MON_EGR_REDIRECT_TRACE_EVENTt_MON_EGR_REDIRECT_TRACE_EVENT_IDf:
                entry->eevent = fval;
                SHR_BITSET(entry->fbmp, fid);
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
            case MON_EGR_REDIRECT_TRACE_EVENTt_REDIRECTf:
                entry->redirect = fval;
                SHR_BITSET(entry->fbmp, fid);
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
 * \brief Stage callback function of IMM event handler (bcmimm_lt_cb_t).
 *
 * \param [in] unit           Device unit number.
 * \param [in] sid            Logical table ID.
 * \param [in] trans_id       Transaction ID associated with this operation.
 * \param [in] event          Reason for the entry event.
 * \param [in] key_flds       Linked list of the key fields identifying
 *                            the entry.
 * \param [in] data_flds      Linked list of the data fields in the
 *                            modified entry.
 * \param [in] context        Is a pointer that was given during registration.
 *                            The callback can use this pointer to retrieve some
 *                            context.
 * \param [out] output_fields IMM output data field array.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
mon_redirect_trace_stage(
    int unit,
    bcmltd_sid_t sid,
    uint32_t trans_id,
    bcmimm_entry_event_t event,
    const bcmltd_field_t *key,
    const bcmltd_field_t *data,
    void *context,
    bcmltd_fields_t *output_fields)
{
    bcmcth_mon_redirect_trace_t entry;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mon_redirect_trace_lt_fields_parse(unit, key, data, &entry));

    if (output_fields) {
        output_fields->count = 0;
    }
    switch (event) {
        case BCMIMM_ENTRY_INSERT:
        case BCMIMM_ENTRY_UPDATE:
            if (SHR_BITGET(entry.fbmp,
                           MON_EGR_REDIRECT_TRACE_EVENTt_REDIRECTf)) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (mon_redirect_trace_lt_update(unit, trans_id, sid, &entry));
            }
            break;
        case BCMIMM_ENTRY_DELETE:
            SHR_IF_ERR_VERBOSE_EXIT
                (mon_redirect_trace_lt_delete(unit, trans_id, sid, &entry));
            break;
        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief populate MON related CTR_xxxt entry.
 *
 * \param [in] unit This is device unit number.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
mon_ctr_populate(int unit)
{
    bcmltd_fields_t ctr_lt_flds = {0};
    size_t num_fields, f_cnt, count;
    bcmlrd_fid_t *fid_list = NULL;
    static const bcmlrd_sid_t sid = CTR_ETRAPt;

    SHR_FUNC_ENTER(unit);

    /* Get number of fields. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlrd_field_count_get(unit,
                                sid,
                                &num_fields));

    SHR_ALLOC(fid_list, sizeof(bcmlrd_fid_t) * num_fields,
              "bcmcthMonCtrFields");
    SHR_NULL_CHECK(fid_list, SHR_E_MEMORY);
    sal_memset(fid_list, 0, sizeof(bcmlrd_fid_t) * num_fields);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlrd_field_list_get(unit,
                               sid,
                               num_fields,
                               fid_list,
                               &count));

    ctr_lt_flds.field = NULL;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_imm_fields_alloc(unit,
                                 &ctr_lt_flds,
                                 num_fields));

    for (f_cnt = 0; f_cnt < count; f_cnt++) {
        ctr_lt_flds.field[f_cnt]->id = fid_list[f_cnt];
        ctr_lt_flds.field[f_cnt]->data = 0;
    }

    ctr_lt_flds.count = count;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmimm_entry_insert(unit,
                             sid,
                             &ctr_lt_flds));

exit:
    bcmcth_imm_fields_free(unit, &ctr_lt_flds);
    SHR_FREE(fid_list);
    SHR_FUNC_EXIT();
}


/*!
 * \brief populate MON_FLOWTRACKER_STATS entry.
 *
 * \param [in] unit This is device unit number.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
mon_ft_stats_populate(int unit)
{
    bcmltd_fields_t ft_lt_flds = {0};
    size_t num_fields, f_cnt, count;
    bcmlrd_fid_t *fid_list = NULL;
    static const bcmlrd_sid_t sid = MON_FLOWTRACKER_STATSt;

    SHR_FUNC_ENTER(unit);

    /* Get number of fields. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlrd_field_count_get(unit,
                                sid,
                                &num_fields));

    SHR_ALLOC(fid_list, sizeof(bcmlrd_fid_t) * num_fields,
              "bcmcthMonFtStatsFields");
    SHR_NULL_CHECK(fid_list, SHR_E_MEMORY);
    sal_memset(fid_list, 0, sizeof(bcmlrd_fid_t) * num_fields);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlrd_field_list_get(unit,
                               sid,
                               num_fields,
                               fid_list,
                               &count));

    ft_lt_flds.field = NULL;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_imm_fields_alloc(unit,
                                 &ft_lt_flds,
                                 num_fields));

    for (f_cnt = 0; f_cnt < count; f_cnt++) {
        ft_lt_flds.field[f_cnt]->id = fid_list[f_cnt];
        ft_lt_flds.field[f_cnt]->data = 0;
    }

    ft_lt_flds.count = count;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmimm_entry_insert(unit,
                             sid,
                             &ft_lt_flds));

exit:
    bcmcth_imm_fields_free(unit, &ft_lt_flds);
    SHR_FREE(fid_list);
    SHR_FUNC_EXIT();
}


/*!
 * \brief populate MON_FLOWTRACKER_GROUP_STATUS entry.
 *
 * \param [in] unit This is device unit number.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
mon_ft_group_status_populate(int unit)
{
    bcmltd_fields_t ft_lt_flds = {0};
    size_t num_fields, f_cnt, count;
    uint64_t key_min = 0, key_max = 0;
    uint32_t i, num, table_size = 0;
    bcmlrd_fid_t *fid_list = NULL;
    static const bcmlrd_sid_t sid = MON_FLOWTRACKER_GROUP_STATUSt;
    static const bcmlrd_fid_t key_fid =
                    MON_FLOWTRACKER_GROUP_STATUSt_MON_FLOWTRACKER_GROUP_IDf;

    SHR_FUNC_ENTER(unit);

    /* Get the min and max values of keys to calculate the table size. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlrd_field_min_get(unit, sid, key_fid, 1, &key_min, &num));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlrd_field_max_get(unit, sid, key_fid, 1, &key_max, &num));
    table_size = key_max - key_min + 1;

    /* Get number of fields. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlrd_field_count_get(unit,
                                sid,
                            &num_fields));

    SHR_ALLOC(fid_list, sizeof(bcmlrd_fid_t) * num_fields,
              "bcmcthMonFtGroupStatusFields");
    SHR_NULL_CHECK(fid_list, SHR_E_MEMORY);
    sal_memset(fid_list, 0, sizeof(bcmlrd_fid_t) * num_fields);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlrd_field_list_get(unit,
                               sid,
                               num_fields,
                               fid_list,
                               &count));

    ft_lt_flds.field = NULL;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_imm_fields_alloc(unit,
                                 &ft_lt_flds,
                                 num_fields));
    for (i = 0; i < table_size; i++)
    {
        for (f_cnt = 0; f_cnt < count; f_cnt++) {
            ft_lt_flds.field[f_cnt]->id = fid_list[f_cnt];
            if (ft_lt_flds.field[f_cnt]->id ==
                        MON_FLOWTRACKER_GROUP_STATUSt_MON_FLOWTRACKER_GROUP_IDf)
            {
                /* Flow Group ID starts from 1 */
                ft_lt_flds.field[f_cnt]->data = i + 1;
            } else {
                ft_lt_flds.field[f_cnt]->data = 0;
            }
        }

        ft_lt_flds.count = count;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmimm_entry_insert(unit,
                                 sid,
                                 &ft_lt_flds));
    }
exit:
    bcmcth_imm_fields_free(unit, &ft_lt_flds);
    SHR_FREE(fid_list);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Lookup callback function of IMM event handler (bcmimm_lt_cb_t).
 *
 * \param [in] unit This is device unit number.
 * \param [in] sid This is the logical table ID.
 * \param [in] trans_id is the transaction ID associated with this operation.
 * \param [in] context Is a pointer that was given during registration.
 *                     The callback can use this pointer to retrieve some
 *                     context.
 * \param [in] lkup_type Indicates the type of lookup that is being performed.
 * \param [in] in Is a structure which contains the key fields but may include
 *                also other fields. The component should only focus on the key
 *                fields.
 * \param [in,out] out this structure contains all the fields of the table, so
 *                     that the component should not delete or add any field to
 *                     this structure. The data values of the field were set by
 *                     the IMM so the component may only overwrite the data
 *                     section of the fields.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
mon_ctr_lookup(
    int unit,
    bcmltd_sid_t sid,
    uint32_t trans_id,
    void *context,
    bcmimm_lookup_type_t lkup_type,
    const bcmltd_fields_t *in,
    bcmltd_fields_t *out)
{
    bcmcth_mon_ctr_t entry;

    SHR_FUNC_ENTER(unit);
    if (lkup_type == BCMIMM_LOOKUP) {
        sal_memset(&entry, 0, sizeof(bcmcth_mon_ctr_t));
        SHR_IF_ERR_VERBOSE_EXIT
            (mon_ctr_lt_get(unit, trans_id, sid, &entry));
        SHR_IF_ERR_VERBOSE_EXIT
            (mon_ctr_lt_fields_lookup(unit, &entry, out));
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief MON_ING_TRACE_EVENT In-memory event callback structure.
 *
 * This structure contains callback functions that will be corresponding
 * to MON_ING_TRACE_EVENT logical table entry commit stages.
 */
static bcmimm_lt_cb_t ing_trace_event_hdl = {

    /*! Validate function. */
    .validate = NULL,

    /*! Staging function. */
    .stage = mon_ing_trace_stage,

    /*! Commit function. */
    .commit = NULL,

    /*! Abort function. */
    .abort = NULL
};

/*!
 * \brief MON_ING_DROP_EVENT In-memory event callback structure.
 *
 * This structure contains callback functions that will be corresponding
 * to MON_ING_DROP_EVENT logical table entry commit stages.
 */
static bcmimm_lt_cb_t ing_drop_event_hdl = {

    /*! Validate function. */
    .validate = NULL,

    /*! Staging function. */
    .stage = mon_ing_drop_stage,

    /*! Commit function. */
    .commit = NULL,

    /*! Abort function. */
    .abort = NULL
};

/*!
 * \brief MON related CTR_xxxt In-memory event callback structure.
 *
 * This structure contains callback functions that will be corresponding
 * to MON related CTR_xxxt logical table entry commit stages.
 */
static bcmimm_lt_cb_t ctr_event_hdl = {

    /*! Validate function. */
    .validate = NULL,

    /*! Staging function. */
    .stage = NULL,

    /*! Commit function. */
    .commit = NULL,

    /*! Abort function. */
    .abort = NULL,

    /*! Abort function. */
    .lookup = mon_ctr_lookup
};

/*!
 * \brief MON_EGR_REDIRECT_TRACE_EVENT In-memory event callback structure.
 *
 * This structure contains callback functions that will be corresponding
 * to MON_EGR_REDIRECT_TRACE_EVENT logical table entry commit stages.
 */
static bcmimm_lt_cb_t redirect_trace_event_hdl = {

    /*! Validate function. */
    .validate = NULL,

    /*! Staging function. */
    .stage = mon_redirect_trace_stage,

    /*! Commit function. */
    .commit = NULL,

    /*! Abort function. */
    .abort = NULL
};

bool
bcmcth_mon_imm_mapped(int unit, const bcmdrd_sid_t sid)
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

static int
bcmcth_mon_imm_register(int unit, bool warm)
{
    SHR_FUNC_ENTER(unit);

    /*
     * To register callback for MON LTs here.
     */
    if (bcmcth_mon_imm_mapped(unit, MON_ING_TRACE_EVENTt)) {
        SHR_IF_ERR_EXIT
            (bcmimm_lt_event_reg(unit, MON_ING_TRACE_EVENTt,
                                 &ing_trace_event_hdl, NULL));
    }
    if (bcmcth_mon_imm_mapped(unit, MON_ING_DROP_EVENTt)) {
        SHR_IF_ERR_EXIT
            (bcmimm_lt_event_reg(unit, MON_ING_DROP_EVENTt,
                                 &ing_drop_event_hdl, NULL));
    }
    if (bcmcth_mon_imm_mapped(unit, CTR_ETRAPt)) {
        SHR_IF_ERR_EXIT
            (bcmimm_lt_event_reg(unit, CTR_ETRAPt,
                                 &ctr_event_hdl, NULL));
    }

    if (bcmcth_mon_imm_mapped(unit, MON_EGR_REDIRECT_TRACE_EVENTt)) {
        SHR_IF_ERR_EXIT
            (bcmimm_lt_event_reg(unit, MON_EGR_REDIRECT_TRACE_EVENTt,
                                 &redirect_trace_event_hdl, NULL));
    }


    /* Register callback for collector LTs */
    SHR_IF_ERR_EXIT
        (bcmcth_mon_collector_imm_register(unit));

    SHR_IF_ERR_EXIT
        (bcmcth_mon_telemetry_register(unit));

    /* Register callbacks for INT LTs. */
    SHR_IF_ERR_EXIT(bcmcth_mon_int_imm_register(unit, warm));

    SHR_IF_ERR_EXIT
        (bcmcth_mon_ft_imm_register(unit, warm));
exit:
    SHR_FUNC_EXIT();
}

static int
bcmcth_mon_imm_prepolulate(int unit)
{
    SHR_FUNC_ENTER(unit);

    if (bcmcth_mon_imm_mapped(unit, CTR_ETRAPt)) {
        SHR_IF_ERR_EXIT(mon_ctr_lt_clear(unit));
        SHR_IF_ERR_EXIT(mon_ctr_populate(unit));
    }
    if (bcmcth_mon_imm_mapped(unit, MON_FLOWTRACKER_STATSt)) {
        SHR_IF_ERR_EXIT(mon_ft_stats_populate(unit));
    }

    if (bcmcth_mon_imm_mapped(unit, MON_FLOWTRACKER_GROUP_STATUSt)) {
        SHR_IF_ERR_EXIT(mon_ft_group_status_populate(unit));
    }
exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public Functions
 */

int
bcmcth_mon_imm_init(int unit, bool warm)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmcth_mon_imm_register(unit, warm));

    /* Prepopulate LT only during coldboot */
    if (!warm) {
        SHR_IF_ERR_EXIT
            (bcmcth_mon_imm_prepolulate(unit));
    }
exit:
    SHR_FUNC_EXIT();
}

