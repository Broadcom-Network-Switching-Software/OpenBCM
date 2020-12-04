/*! \file bcmcth_udf_imm.c
 *
 * UDF interfaces to IMM.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <shr/shr_bitop.h>
#include <bcmevm/bcmevm_api.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmimm/bcmimm_int_comp.h>
#include <bcmlrd/bcmlrd_map.h>
#include <bcmcth/bcmcth_udf_drv.h>
/*******************************************************************************
 * Local definitions
 */

/* BSL Module */
#define BSL_LOG_MODULE BSL_LS_BCMCTH_UDF

/*******************************************************************************
 * Private functions
 */
/*!
 * \brief LT UDF_POLICY fields parsing.
 *
 * Parse LT UDF_POLICY input fields.
 *
 * \param [in] unit Unit number.
 * \param [in] key, LT input key field array.
 * \param [in] data, LT input data field array.
 * \param [out] lt_info Parse result of LT fields.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Faild to convert \c key \c data to \c ltcfg.
 */
static int
udf_policy_lt_fields_parse(int unit,
                           const bcmltd_field_t *key,
                           const bcmltd_field_t *data,
                           bcmcth_udf_policy_info_t *entry)
{
    const bcmltd_field_t *lt_field;
    uint32_t fid;
    uint64_t fval;
    uint32_t idx;

    SHR_FUNC_ENTER(unit);

    COMPILER_REFERENCE(unit);
    COMPILER_REFERENCE(key);

    /* Parse key fields. */
    lt_field = key;
    while (lt_field) {
        fid = lt_field->id;
        fval = lt_field->data;

        if (fid == UDF_POLICYt_UDF_POLICY_IDf) {
            entry->udf_policy_id = fval;
        } else {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }

        lt_field = lt_field->next;
    }

    /* Parse data field */
    lt_field = data;
    while (lt_field) {
        fid = lt_field->id;
        fval = lt_field->data;
        idx = lt_field->idx;

        switch (fid) {
            case UDF_POLICYt_OFFSETf:
                entry->offset = fval;
                break;
            case UDF_POLICYt_PACKET_HEADERf:
                entry->packet_header = fval;
                break;
            case UDF_POLICYt_CONTAINER_4_BYTEf:
                entry->container_4_byte[idx] = fval;
                break;
            case UDF_POLICYt_CONTAINER_2_BYTEf:
                entry->container_2_byte[idx] = fval;
                break;
            case UDF_POLICYt_CONTAINER_1_BYTEf:
                entry->container_1_byte[idx] = fval;
                break;
            default:
                SHR_ERR_EXIT(SHR_E_PARAM);
        }
        lt_field = lt_field->next;
    }

exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Private functions
 */
/*!
 * \brief LT UDF_FIELD_MUX_SELECT fields parsing.
 *
 * Parse LT UDF_FIELD_MUX_SELECT input fields.
 *
 * \param [in] unit Unit number.
 * \param [in] key, LT input key field array.
 * \param [in] data, LT input data field array.
 * \param [out] lt_info Parse result of LT fields.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Faild to convert \c key \c data to \c ltcfg.
 */
static int
udf_field_mux_select_lt_fields_parse(int unit,
                                     const bcmltd_field_t *key,
                                     const bcmltd_field_t *data,
                                     bcmcth_udf_field_mux_select_info_t *entry)
{
    const bcmltd_field_t *lt_field;
    uint32_t fid;
    uint64_t fval;
    uint32_t idx;

    SHR_FUNC_ENTER(unit);

    COMPILER_REFERENCE(unit);
    COMPILER_REFERENCE(key);

    /* Parse key fields. */
    lt_field = key;
    while (lt_field) {
        fid = lt_field->id;
        fval = lt_field->data;

        if (fid == UDF_FIELD_MUX_SELECTt_UDF_FIELD_MUX_SELECT_IDf) {
            entry->udf_field_mux_select_id = fval;
        } else {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }

        lt_field = lt_field->next;
    }

    /* Parse data field */
    lt_field = data;
    while (lt_field) {
        fid = lt_field->id;
        fval = lt_field->data;
        idx = lt_field->idx;

        switch (fid) {
            case UDF_FIELD_MUX_SELECTt_FORWARDING_CONTAINER_4_BYTEf:
                entry->container_fwd_4_byte[idx] = fval;
                break;
            case UDF_FIELD_MUX_SELECTt_FORWARDING_CONTAINER_2_BYTEf:
                entry->container_fwd_2_byte[idx] = fval;
                break;
            case UDF_FIELD_MUX_SELECTt_FORWARDING_CONTAINER_1_BYTEf:
                entry->container_fwd_1_byte[idx] = fval;
                break;
            case UDF_FIELD_MUX_SELECTt_ALTERNATE_CONTAINER_2_BYTEf:
                entry->container_alt_2_byte[idx] = fval;
                break;
            default:
                SHR_ERR_EXIT(SHR_E_PARAM);
        }
        lt_field = lt_field->next;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief UDF_POLICY IMM table change callback function for staging.
 *
 * Handle UDF_POLICY IMM table change events.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [in] trans_id  Transcation ID.
 * \param [in] entry_event  Logical table entry event.
 * \param [in] key Linked list of the key fields identifying
 * the entry.
 * \param [in] data Linked list of the data fields in the
 * modified entry.
 * \param [in] context A pointer that was given during registration.
 * The callback can use this pointer to retrieve some context.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL UDF Policy fails to handle LT change events.
 */
static int
udf_policy_stage(int unit,
                 bcmltd_sid_t sid,
                 uint32_t trans_id,
                 bcmimm_entry_event_t entry_event,
                 const bcmltd_field_t *key,
                 const bcmltd_field_t *data,
                 void *context,
                 bcmltd_fields_t *output_fields)
{

    bcmcth_udf_policy_info_t entry;
    uint64_t trans_id_udf_policy_id = 0;
    uint16_t udf_policy_id;
    const char *event;
    uint32_t update = 0;

    SHR_FUNC_ENTER(unit);

    COMPILER_REFERENCE(sid);
    COMPILER_REFERENCE(trans_id);
    COMPILER_REFERENCE(context);

    sal_memset(&entry, 0, sizeof(entry));
    SHR_IF_ERR_VERBOSE_EXIT
        (udf_policy_lt_fields_parse(unit, key, data, &entry));

    if (output_fields) {
        output_fields->count = 0;
    }

    udf_policy_id = entry.udf_policy_id;

    switch (entry_event) {
    case BCMIMM_ENTRY_INSERT:
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_udf_policy_create(unit, trans_id, sid, &entry, update));
        event = "UDF_POLICY_INSERT";
        break;
    case BCMIMM_ENTRY_UPDATE:
        update = 1;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_udf_policy_create(unit, trans_id, sid, &entry, update));
        event = "UDF_POLICY_UPDATE";
        break;
    case BCMIMM_ENTRY_DELETE:
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_udf_policy_delete(unit, trans_id, sid, &entry));
        event = "UDF_POLICY_DELETE";
        break;
    case BCMIMM_ENTRY_LOOKUP:
        SHR_EXIT();
        break;
    default:
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* Publish UDF_POLICY_XXX event */
    trans_id_udf_policy_id = ((uint64_t)trans_id << 32) | udf_policy_id;
    bcmevm_publish_event_notify(unit,
                                event,
                                trans_id_udf_policy_id);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief UDF_POLICY_INFO IMM table callback function.
 *
 * Handle UDF_POLICY_INFO IMM table change events.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [in] trans_id  Transcation ID.
 * \param [in] entry_event  Logical table entry event.
 * \param [in] key Linked list of the key fields identifying
 * the entry.
 * \param [in] data Linked list of the data fields in the
 * modified entry.
 * \param [in] context A pointer that was given during registration.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL UDF Policy fails to handle LT change events.
 */
static int
udf_policy_info_tbl_cb(int unit,
                       bcmltd_sid_t sid,
                       bcmimm_entry_event_t entry_event,
                       const bcmltd_field_t *key,
                       const bcmltd_field_t *data,
                       void *context)
{

    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    /* Insert, Update and Delete not supported for read-only logical tables */
    if ((entry_event == BCMIMM_ENTRY_INSERT) ||
        (entry_event == BCMIMM_ENTRY_DELETE) ||
        (entry_event == BCMIMM_ENTRY_UPDATE)) {
        rv = SHR_E_DISABLED;
        SHR_ERR_EXIT(rv);
    }

exit:
    SHR_FUNC_EXIT();

}

/*!
 * \brief UDF_FIELD_MUX_SELECT IMM table change callback function for staging.
 *
 * Handle UDF_FIELD_MUX_SELECT IMM table change events.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 * \param [in] trans_id  Transcation ID.
 * \param [in] entry_event  Logical table entry event.
 * \param [in] key Linked list of the key fields identifying
 * the entry.
 * \param [in] data Linked list of the data fields in the
 * modified entry.
 * \param [in] context A pointer that was given during registration.
 * The callback can use this pointer to retrieve some context.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL UDF Policy fails to handle LT change events.
 */
static int
udf_field_mux_select_stage(int unit,
                          bcmltd_sid_t sid,
                          uint32_t trans_id,
                          bcmimm_entry_event_t entry_event,
                          const bcmltd_field_t *key,
                          const bcmltd_field_t *data,
                          void *context,
                          bcmltd_fields_t *output_fields)
{

    bcmcth_udf_field_mux_select_info_t entry;

    SHR_FUNC_ENTER(unit);

    COMPILER_REFERENCE(sid);
    COMPILER_REFERENCE(trans_id);
    COMPILER_REFERENCE(context);

    sal_memset(&entry, 0, sizeof(entry));
    SHR_IF_ERR_VERBOSE_EXIT
        (udf_field_mux_select_lt_fields_parse(unit, key, data, &entry));

    if (output_fields) {
        output_fields->count = 0;
    }

    switch (entry_event) {
    case BCMIMM_ENTRY_INSERT:
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_udf_field_mux_select_create(unit, trans_id, sid, &entry));
        break;
    case BCMIMM_ENTRY_UPDATE:
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_udf_field_mux_select_create(unit, trans_id, sid, &entry));
        break;
    case BCMIMM_ENTRY_DELETE:
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_udf_field_mux_select_delete(unit, trans_id, sid, &entry));
        break;
    case BCMIMM_ENTRY_LOOKUP:
        SHR_EXIT();
        break;
    default:
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief UDF_POLICY In-memory event callback structure.
 *
 * This structure contains callback functions that is corresponding
 * to UDF_POLICY logical table entry commit stages.
 */
static bcmimm_lt_cb_t udf_policy_imm_callback = {

    /*! Validate function. */
    .validate = NULL,

    /*! Staging function. */
    .stage = udf_policy_stage,

    /*! Commit function. */
    .commit = NULL,

    /*! Abort function. */
    .abort = NULL
};

/*!
 * \brief UDF_POLICY_INFO In-memory event callback structure.
 *
 * This structure contains callback functions that is corresponding
 * to UDF_POLICY_INFO logical table entry commit stages.
 */
static bcmimm_lt_cb_t udf_policy_info_imm_callback = {

    /*! Validate function. */
    .validate = udf_policy_info_tbl_cb,

    /*! Staging function. */
    .stage = NULL,

    /*! Commit function. */
    .commit = NULL,

    /*! Abort function. */
    .abort = NULL
};

/*!
 * \brief UDF_FIELD_MUX_SELECT In-memory event callback structure.
 *
 * This structure contains callback functions that is corresponding
 * to UDF_FIELD_MUX_SELECT logical table entry commit stages.
 */
static bcmimm_lt_cb_t udf_field_mux_select_imm_callback = {

    /*! Validate function. */
    .validate = NULL,

    /*! Staging function. */
    .stage = udf_field_mux_select_stage,

    /*! Commit function. */
    .commit = NULL,

    /*! Abort function. */
    .abort = NULL
};
/*******************************************************************************
 * Public Functions
 */
int
bcmcth_udf_imm_register(int unit)
{
    const bcmlrd_map_t *map = NULL;
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    /*
     * Register callback for UDF_POLICY LT here.
     */
    rv = bcmlrd_map_get(unit, UDF_POLICYt, &map);
    if (SHR_SUCCESS(rv) && map) {
        SHR_IF_ERR_EXIT
            (bcmimm_lt_event_reg(unit,
                                 UDF_POLICYt,
                                 &udf_policy_imm_callback,
                                 NULL));
    }

    /*
     * Register callback for UDF_POLICY_INFO LT here.
     */
    rv = bcmlrd_map_get(unit, UDF_POLICY_INFOt, &map);
    if (SHR_SUCCESS(rv) && map) {
        SHR_IF_ERR_EXIT
            (bcmimm_lt_event_reg(unit,
                                 UDF_POLICY_INFOt,
                                 &udf_policy_info_imm_callback,
                                 NULL));
    }

    /*
     * Register callback for UDF_FIELD_MUX_SELECT LT here.
     */
    rv = bcmlrd_map_get(unit, UDF_FIELD_MUX_SELECTt, &map);
    if (SHR_SUCCESS(rv) && map) {
        SHR_IF_ERR_EXIT
            (bcmimm_lt_event_reg(unit,
                                 UDF_FIELD_MUX_SELECTt,
                                 &udf_field_mux_select_imm_callback,
                                 NULL));
    }

exit:
    SHR_FUNC_EXIT();
}
