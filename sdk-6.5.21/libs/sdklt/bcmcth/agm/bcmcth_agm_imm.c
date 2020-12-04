/*! \file bcmcth_agm_imm.c
 *
 * BCMCTH Aggregation Group Monitor IMM handler.
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

#include <bcmcth/bcmcth_agm_drv.h>

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMCTH_AGM

/*******************************************************************************
 * Local definitions
 */

/*! Logical table ID for this handler. */
#define IMM_SID MON_AGM_CONTROLt

/*******************************************************************************
 * Private Functions
 */

/*!
 * \brief Init all uninitialized LT fields value as SW default value.
 *
 * This initialization will set default value to all uninitialized fields.
 *
 * \param [in] unit Unit number.
 * \param [out] entry MON_AGM_CONTROL LT entry data buffer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Faild to set default value to \c entry.
 */
static int
agm_control_lt_fields_init(int unit,
                           bcmcth_agm_control_t *entry)
{
    uint64_t def_val = 0;
    uint32_t num;
    SHR_FUNC_ENTER(unit);

    /* Validate input parameter. */
    SHR_NULL_CHECK(entry, SHR_E_PARAM);

    if (!SHR_BITGET(entry->fbmp, MON_AGM_CONTROLt_MONITORf)) {
        SHR_IF_ERR_EXIT(
            bcmlrd_field_default_get(unit,
                                     MON_AGM_CONTROLt,
                                     MON_AGM_CONTROLt_MONITORf,
                                     1, &def_val, &num));
        entry->monitor = def_val;
        SHR_BITSET(entry->fbmp, MON_AGM_CONTROLt_MONITORf);
    }

    if (!SHR_BITGET(entry->fbmp, MON_AGM_CONTROLt_START_TIME_AUTOf)) {
        SHR_IF_ERR_EXIT(
            bcmlrd_field_default_get(unit,
                                     MON_AGM_CONTROLt,
                                     MON_AGM_CONTROLt_START_TIME_AUTOf,
                                     1, &def_val, &num));
        entry->start_time_auto = def_val;
        SHR_BITSET(entry->fbmp, MON_AGM_CONTROLt_START_TIME_AUTOf);
    }

    if (!SHR_BITGET(entry->fbmp, MON_AGM_CONTROLt_START_TIMEf)) {
        SHR_IF_ERR_EXIT(
            bcmlrd_field_default_get(unit,
                                     MON_AGM_CONTROLt,
                                     MON_AGM_CONTROLt_START_TIMEf,
                                     1, &def_val, &num));
        entry->start_time = def_val;
        SHR_BITSET(entry->fbmp, MON_AGM_CONTROLt_START_TIMEf);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief imm MON_AGM_CONTROLt notification input fields parsing.
 *
 * Parse imm MON_AGM_CONTROLt input key fields.
 *
 * \param [in] unit Unit number.
 * \param [in] key IMM input key field array.
 * \param [out] entry MON_AGM_CONTROL LT entry data buffer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Faild to convert \c key  to \c entry.
 */
static int
agm_control_lt_fields_key_parse(int unit,
                                const bcmltd_field_t *key,
                                bcmcth_agm_control_t *entry)
{
    const bcmltd_field_t *gen_field;
    uint32_t fid;
    uint64_t fval;

    SHR_FUNC_ENTER(unit);

    /* Parse key field */
    gen_field = key;
    while (gen_field) {
        fid = gen_field->id;
        fval = gen_field->data;

        switch (fid) {
            case MON_AGM_CONTROLt_MON_AGM_IDf:
                entry->agm_id = fval;
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
 * \brief imm MON_AGM_CONTROLt notification input fields parsing.
 *
 * Parse imm MON_AGM_CONTROLt input data fields.
 *
 * \param [in] unit Unit number.
 * \param [in] data IMM input data field array.
 * \param [out] entry MON_AGM_CONTROL LT entry data buffer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Faild to convert \c data to \c entry.
 */
static int
agm_control_lt_fields_data_parse(int unit,
                                 const bcmltd_field_t *data,
                                 bcmcth_agm_control_t *entry)
{
    const bcmltd_field_t *gen_field;
    uint32_t fid;
    uint64_t fval;

    SHR_FUNC_ENTER(unit);

    /* Parse data field */
    gen_field = data;
    while (gen_field) {
        fid = gen_field->id;
        fval = gen_field->data;

        switch (fid) {
            case MON_AGM_CONTROLt_START_TIME_AUTOf:
                entry->start_time_auto = fval;
                SHR_BITSET(entry->fbmp, fid);
                break;
            case MON_AGM_CONTROLt_START_TIMEf:
                entry->start_time = fval;
                SHR_BITSET(entry->fbmp, fid);
                break;
            case MON_AGM_CONTROLt_MONITORf:
                entry->monitor = fval;
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

static int
agm_control_lt_fields_free(int unit,
                           bcmltd_fields_t* in)
{
    uint32_t i;

    SHR_FUNC_ENTER(unit);

    if (in->field) {
        for (i = 0; i < in->count; i++) {
            if (in->field[i]) {
                SHR_FREE(in->field[i]);
            }
        }
        SHR_FREE(in->field);
    }

    SHR_FUNC_EXIT();
}

static int
agm_control_lt_fields_allocate (int unit,
                                bcmltd_fields_t* in,
                                size_t num_fields)
{
    uint32_t i;
    size_t alloc_sz;

    SHR_FUNC_ENTER(unit);

    /* Allocate fields buffers */
    alloc_sz = sizeof(bcmltd_field_t *) * num_fields;
    SHR_ALLOC(in->field, alloc_sz, "bcmcthAgmEntryFields");
    SHR_NULL_CHECK(in->field, SHR_E_MEMORY);
    sal_memset(in->field, 0, alloc_sz);

    in->count = num_fields;

    for (i = 0; i < num_fields; i++) {
        SHR_ALLOC(in->field[i], sizeof(bcmltd_field_t), "bcmcthAgmEntryField");
        SHR_NULL_CHECK(in->field[i], SHR_E_MEMORY);
        sal_memset(in->field[i], 0, sizeof(bcmltd_field_t));
    }

    SHR_FUNC_EXIT();

exit:
    agm_control_lt_fields_free(unit, in);

    SHR_FUNC_EXIT();
}

/*!
 * \brief Get all current imm MON_AGM_CONTROL LT fields.
 *
 * Get the current fields from imm MON_AGM_CONTROL LT
 *
 * \param [in] unit Unit number.
 * \param [out] entry MON_AGM_CONTROL LT entry data buffer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Faild to geet current LT entry fields.
 */
static int
agm_control_lt_fields_lookup(int unit,
                             bcmcth_agm_control_t *entry)
{
    bcmltd_fields_t key_fields = {0};
    bcmltd_fields_t imm_fields = {0};
    bcmltd_fields_t *flist;
    bcmltd_field_t *data_field;
    size_t num_fields, i;
    uint32_t fid;
    SHR_FUNC_ENTER(unit);

    /* Validate input parameter. */
    SHR_NULL_CHECK(entry, SHR_E_PARAM);

    /* Allocate key field */
    SHR_IF_ERR_EXIT
        (agm_control_lt_fields_allocate(unit,
                                        &key_fields,
                                        1));

    key_fields.field[0]->id   = MON_AGM_CONTROLt_MON_AGM_IDf;
    key_fields.field[0]->data = entry->agm_id;

    /* Allocate imm fields */
    SHR_IF_ERR_EXIT
        (bcmlrd_field_count_get(unit, MON_AGM_CONTROLt, &num_fields));

    SHR_IF_ERR_EXIT
        (agm_control_lt_fields_allocate(unit,
                                        &imm_fields,
                                        num_fields));

    /* Lookup IMM table; error if entry not found */
    SHR_IF_ERR_EXIT
        (bcmimm_entry_lookup(unit,
                             MON_AGM_CONTROLt,
                             &key_fields,
                             &imm_fields));

    flist = &imm_fields;

    /* Parse IMM table data fields */
    for (i = 0; i < flist->count; i++) {
        data_field = flist->field[i];
        fid = data_field->id;

        switch (fid) {
            case MON_AGM_CONTROLt_START_TIME_AUTOf:
                entry->start_time_auto = data_field->data;
                SHR_BITSET(entry->fbmp, fid);
                break;
            case MON_AGM_CONTROLt_START_TIMEf:
                entry->start_time = data_field->data;
                SHR_BITSET(entry->fbmp, fid);
                break;
            case MON_AGM_CONTROLt_MONITORf:
                entry->monitor = data_field->data;
                SHR_BITSET(entry->fbmp, fid);
                break;
            case MON_AGM_CONTROLt_START_TIME_OPERf:
                SHR_BITSET(entry->fbmp, fid);
                break;
            default:
                SHR_ERR_EXIT(SHR_E_PARAM);
        }
    }

exit:
    agm_control_lt_fields_free(unit, &key_fields);
    agm_control_lt_fields_free(unit, &imm_fields);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Do the internal hardware configuration for UPDATE operation.
 *
 * \param [in] unit Unit number.
 * \param [in] entry MON_AGM_CONTROL LT entry data buffer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
agm_control_lt_update(int unit, bcmcth_agm_control_t *entry)
{
    uint64_t timestamp;

    SHR_FUNC_ENTER(unit);

    if (entry->monitor && entry->start_time_auto) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmbd_ts_timestamp_nanosec_get(unit,
                                            BCMAGM_TS_INST,
                                            &timestamp));
        entry->start_time = timestamp;
        entry->start_time_oper = entry->start_time;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_agm_control_set(unit, entry));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Do the internal hardware configuration for DELETE operation.
 *
 * \param [in] unit Unit number.
 * \param [in] entry MON_AGM_CONTROL LT entry data buffer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
agm_control_lt_delete(int unit, bcmcth_agm_control_t *entry)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_agm_control_set(unit, entry));
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
mon_agm_control_validate(int unit,
                         bcmltd_sid_t sid,
                         bcmimm_entry_event_t event,
                         const bcmltd_field_t *key,
                         const bcmltd_field_t *data,
                         void *context)
{

    bcmcth_agm_control_t cur_entry = {0};
    bcmcth_agm_control_t new_entry = {0};

    SHR_FUNC_ENTER(unit);

    if (event == BCMIMM_ENTRY_UPDATE) {
        /* Parse input key fields */
        SHR_IF_ERR_VERBOSE_EXIT
            (agm_control_lt_fields_key_parse(unit, key, &cur_entry));

        /* Get current fields */
        SHR_IF_ERR_VERBOSE_EXIT
            (agm_control_lt_fields_lookup(unit, &cur_entry));

        /* Get fields default value */
        SHR_IF_ERR_VERBOSE_EXIT
            (agm_control_lt_fields_init(unit, &cur_entry));

        /* Parse input key fields */
        SHR_IF_ERR_VERBOSE_EXIT
            (agm_control_lt_fields_key_parse(unit, key, &new_entry));

        /* Parse input data fields, may overwrite previous data fields */
        SHR_IF_ERR_VERBOSE_EXIT
            (agm_control_lt_fields_data_parse(unit, data, &new_entry));

        /* Current AGM is running */
        if (cur_entry.monitor) {
            if (!(SHR_BITGET(new_entry.fbmp, MON_AGM_CONTROLt_MONITORf) &&
                  (new_entry.monitor == 0))) {
                LOG_WARN(BSL_LOG_MODULE,
                        (BSL_META_U(unit,
                                    "Current AGM %"PRIu32" is running, "
                                    "need to stop first.\n"),
                        cur_entry.agm_id));
                SHR_ERR_EXIT(SHR_E_PARAM);
            }
        }

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
mon_agm_control_stage(int unit,
                      bcmltd_sid_t sid,
                      uint32_t trans_id,
                      bcmimm_entry_event_t event,
                      const bcmltd_field_t *key,
                      const bcmltd_field_t *data,
                      void *context,
                      bcmltd_fields_t *output_fields)
{

    bcmcth_agm_control_t entry = {0};

    SHR_FUNC_ENTER(unit);

    /* Parse input key fields */
    SHR_IF_ERR_VERBOSE_EXIT
        (agm_control_lt_fields_key_parse(unit, key, &entry));

    if (event == BCMIMM_ENTRY_UPDATE) {
        /* Get current fields */
        SHR_IF_ERR_VERBOSE_EXIT
            (agm_control_lt_fields_lookup(unit, &entry));
    }

    /* Get fields default value */
    SHR_IF_ERR_VERBOSE_EXIT
        (agm_control_lt_fields_init(unit, &entry));

    switch (event) {
        case BCMIMM_ENTRY_INSERT:
        case BCMIMM_ENTRY_UPDATE:
            /* Parse input data fields, may overwrite previous data fields */
            SHR_IF_ERR_VERBOSE_EXIT
                (agm_control_lt_fields_data_parse(unit, data, &entry));

            SHR_IF_ERR_VERBOSE_EXIT
                (agm_control_lt_update(unit, &entry));
            break;
        case BCMIMM_ENTRY_DELETE:
            SHR_IF_ERR_VERBOSE_EXIT
                (agm_control_lt_delete(unit, &entry));
            break;
        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (output_fields) {
        output_fields->count = 1;
        output_fields->field[0]->id = MON_AGM_CONTROLt_START_TIME_OPERf;
        output_fields->field[0]->data = entry.start_time_oper;
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief MON_AGM_CONTROL In-memory event callback structure.
 *
 * This structure contains callback functions that will be corresponding
 * to MON_AGM_CONTROL logical table entry commit stages.
 */
static bcmimm_lt_cb_t event_hdl = {

    /*! Validate function. */
    .validate = mon_agm_control_validate,

    /*! Staging function. */
    .stage = mon_agm_control_stage,

    /*! Commit function. */
    .commit = NULL,

    /*! Abort function. */
    .abort = NULL
};

static int
bcmcth_agm_imm_register(int unit)
{
    const bcmlrd_map_t *map = NULL;
    int rv;

    SHR_FUNC_ENTER(unit);

    /*
     * To register callback for MON_AGM_CONTROL LT here.
     */
    rv = bcmlrd_map_get(unit, IMM_SID, &map);
    if (SHR_SUCCESS(rv) && map) {
        SHR_IF_ERR_EXIT
            (bcmimm_lt_event_reg(unit,
                                 IMM_SID,
                                 &event_hdl,
                                 NULL));
    }

exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public Functions
 */

int
bcmcth_agm_imm_init(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmcth_agm_imm_register(unit));
exit:
    SHR_FUNC_EXIT();
}
