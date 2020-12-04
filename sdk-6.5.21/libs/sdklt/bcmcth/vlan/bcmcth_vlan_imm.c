/*! \file bcmcth_vlan_imm.c
 *
 * BCMVLAN interface to in-memory table.
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
#include <bcmcth/bcmcth_vlan_util.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmlrd/bcmlrd_client.h>
#include <bcmcth/bcmcth_vlan_drv.h>

/*******************************************************************************
 * Local definitions
 */

/* BSL Module */
#define BSL_LOG_MODULE BSL_LS_BCMCTH_VLAN

static int
vlan_imm_lt_fields_free(int unit,
                        bcmltd_fields_t *in,
                        size_t num_fields)
{
    uint32_t i;

    SHR_FUNC_ENTER(unit);

    if (in->field) {
        for (i = 0; i < num_fields; i++) {
            if (in->field[i]) {
                SHR_FREE(in->field[i]);
            }
        }
        SHR_FREE(in->field);
    }

    SHR_FUNC_EXIT();
}

static int
vlan_imm_lt_fields_allocate (int unit,
                             bcmltd_fields_t* in,
                             size_t num_fields)
{
    uint32_t i;
    size_t alloc_sz;

    SHR_FUNC_ENTER(unit);

    /* Allocate fields buffers */
    alloc_sz = sizeof(bcmltd_field_t *) * num_fields;
    SHR_ALLOC(in->field, alloc_sz, "bcmVlanImmEntryFields");
    SHR_NULL_CHECK(in->field, SHR_E_MEMORY);
    sal_memset(in->field, 0, alloc_sz);

    in->count = num_fields;

    for (i = 0; i < num_fields; i++) {
        SHR_ALLOC(in->field[i], sizeof(bcmltd_field_t), "bcmVlanImmEntryFields");
        SHR_NULL_CHECK(in->field[i], SHR_E_MEMORY);
        sal_memset(in->field[i], 0, sizeof(bcmltd_field_t));
    }

exit:
    if (SHR_FUNC_ERR()) {
        vlan_imm_lt_fields_free(unit, in, num_fields);
    }

    SHR_FUNC_EXIT();
}


/*!
 * \brief Get all current  imm VLAN_ING_STG_PROFILE LT fields.
 *
 * Get the current fields from VLAN_ING_STG_PROFILE LT
 *
 * \param [in] unit Unit number.
 * \param [out] lt_info VLAN_ING_STG_PROFILE LT entry data buffer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Faild to get current LT entry fields.
 */
static int
vlan_ing_stg_profile_lt_fields_lookup(int unit,
                                      vlan_ing_stg_profile_t *lt_info)
{
#define VLAN_ING_STG_PRIFILE_FILED_COUNT_MAX 257
    bcmltd_fields_t key_fields = {0};
    bcmltd_fields_t imm_fields = {0};
    bcmltd_fields_t *flist;
    bcmltd_field_t *data_field;
    size_t num_fields = VLAN_ING_STG_PRIFILE_FILED_COUNT_MAX;
    size_t i;
    uint32_t fid;
    uint64_t fval;
    uint32_t arr_idx;

    SHR_FUNC_ENTER(unit);

    /* Allocate key field */
    SHR_IF_ERR_EXIT
        (vlan_imm_lt_fields_allocate(unit,
                                     &key_fields,
                                     1));

    key_fields.field[0]->id   = VLAN_ING_STG_PROFILEt_VLAN_ING_STG_PROFILE_IDf;
    key_fields.field[0]->data = lt_info->stg_id;

    SHR_IF_ERR_EXIT
        (vlan_imm_lt_fields_allocate(unit,
                                     &imm_fields,
                                     num_fields));

    /* Lookup IMM table; error if entry not found */
    SHR_IF_ERR_EXIT
        (bcmimm_entry_lookup(unit,
                             VLAN_ING_STG_PROFILEt,
                             &key_fields,
                             &imm_fields));

    flist = &imm_fields;

    /* Parse IMM table data fields */
    for (i = 0; i < flist->count; i++) {
        data_field = flist->field[i];
        fid = data_field->id;
        fval = data_field->data;
        arr_idx = data_field->idx;

        switch (fid) {
            case VLAN_ING_STG_PROFILEt_STATEf:
                lt_info->state[arr_idx] = (uint8_t) fval;
                break;
            default:
                SHR_ERR_EXIT(SHR_E_PARAM);
        }
    }

exit:
    vlan_imm_lt_fields_free(unit, &key_fields, 1);
    vlan_imm_lt_fields_free(unit, &imm_fields, num_fields);
    SHR_FUNC_EXIT();
}

static int
vlan_ing_stg_profile_lt_fields_key_parse(int unit,
                                         const bcmltd_field_t *key,
                                         vlan_ing_stg_profile_t *lt_info)
{
    const bcmltd_field_t *gen_field;
    uint32_t fid;
    uint64_t fval;

    SHR_FUNC_ENTER(unit);

    sal_memset(lt_info, 0, sizeof(vlan_ing_stg_profile_t));

    /* Parse key field */
    gen_field = key;
    while (gen_field) {
        fid = gen_field->id;
        fval = gen_field->data;

        switch (fid) {
            case VLAN_ING_STG_PROFILEt_VLAN_ING_STG_PROFILE_IDf:
                BCMCTH_VLAN_LT_FIELD_SET(
                    lt_info->fbmp, STG_PROFILE_ID);
                lt_info->stg_id = (uint8_t) fval;
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
vlan_ing_stg_profile_lt_fields_data_parse(int unit,
                                          const bcmltd_field_t *data,
                                          vlan_ing_stg_profile_t *lt_info)
{
    const bcmltd_field_t *gen_field;
    uint32_t fid;
    uint64_t fval;
    uint32_t arr_idx;

    SHR_FUNC_ENTER(unit);

    /* Parse data field */
    gen_field = data;
    while (gen_field) {
        fid = gen_field->id;
        fval = gen_field->data;
        arr_idx = gen_field->idx;

        switch (fid) {
            case VLAN_ING_STG_PROFILEt_STATEf:
                BCMCTH_VLAN_LT_FIELD_SET(
                    lt_info->fbmp, STG_STATE);
                BCMCTH_VLAN_LT_FIELD_SET(lt_info->state_array_bmp, arr_idx);
                lt_info->state[arr_idx] = (uint8_t) fval;
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
 * \param [in] lt_info VLAN_ING_STG_PROFILE LT entry data buffer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Input parameter failed validation check.
 */
static int
vlan_ing_stg_profile_default_values_init(int unit,
                                         vlan_ing_stg_profile_t *lt_info)
{
    uint64_t def_val = 0;
    uint32_t num;
    int i;
    SHR_FUNC_ENTER(unit);

    /* validate input parameter. */
    SHR_NULL_CHECK(lt_info, SHR_E_PARAM);

    if (!BCMCTH_VLAN_LT_FIELD_GET(
             lt_info->fbmp, STG_STATE)) {
        SHR_IF_ERR_EXIT(
            bcmlrd_field_default_get(unit,
                                     VLAN_ING_STG_PROFILEt,
                                     VLAN_ING_STG_PROFILEt_STATEf,
                                     1, &def_val, &num));
        for (i = 0; i < MAX_PORTS; i++) {
            lt_info->state[i] = (uint8_t)def_val;
            BCMCTH_VLAN_LT_FIELD_SET(lt_info->state_array_bmp, i);
        }
        BCMCTH_VLAN_LT_FIELD_SET(lt_info->fbmp, STG_STATE);
    } else {
        SHR_IF_ERR_EXIT(
            bcmlrd_field_default_get(unit,
                                     VLAN_ING_STG_PROFILEt,
                                     VLAN_ING_STG_PROFILEt_STATEf,
                                     1, &def_val, &num));
        for (i = 0; i < MAX_PORTS; i++) {
            if (BCMCTH_VLAN_LT_FIELD_GET(lt_info->state_array_bmp, i)) {
                continue;
            }
            lt_info->state[i] = (uint8_t)def_val;
            BCMCTH_VLAN_LT_FIELD_SET(lt_info->state_array_bmp, i);
        }
    }
exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief VLAN_ING_STG_PROFILE IMM table change callback function for staging.
 *
 * Handle VLAN_ING_STG_PROFILE IMM table change events.
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
vlan_ing_stg_profile_imm_callback_stage(int unit,
                                        bcmltd_sid_t sid,
                                        uint32_t trans_id,
                                        bcmimm_entry_event_t event_reason,
                                        const bcmltd_field_t *key,
                                        const bcmltd_field_t *data,
                                        void *context,
                                        bcmltd_fields_t *output_fields)
{
    vlan_ing_stg_profile_t lt_info;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_ing_stg_profile_lt_fields_key_parse(unit, key, &lt_info));

    if (output_fields) {
        output_fields->count = 0;
    }

    switch (event_reason) {
        case BCMIMM_ENTRY_UPDATE:
            SHR_IF_ERR_VERBOSE_EXIT
                (vlan_ing_stg_profile_lt_fields_lookup(unit, &lt_info));
            SHR_IF_ERR_VERBOSE_EXIT
                (vlan_ing_stg_profile_lt_fields_data_parse(unit, data, &lt_info));
            break;
        case BCMIMM_ENTRY_INSERT:
            SHR_IF_ERR_VERBOSE_EXIT
                (vlan_ing_stg_profile_lt_fields_data_parse(unit, data, &lt_info));
            SHR_IF_ERR_VERBOSE_EXIT(
                vlan_ing_stg_profile_default_values_init(unit, &lt_info));
            break;
        case BCMIMM_ENTRY_DELETE:
            SHR_IF_ERR_VERBOSE_EXIT(
                vlan_ing_stg_profile_default_values_init(unit, &lt_info));
            break;
        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT(
        bcmcth_vlan_ing_stg_profile_set(unit,
                                        trans_id,
                                        sid,
                                        &lt_info));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief VLAN_ING_STG_PROFILE In-memory event callback structure.
 *
 * This structure contains callback functions that will be corresponding
 * to VLAN_ING_STG_PROFILE logical table entry commit stages.
 */
static bcmimm_lt_cb_t vlan_ing_stg_profile_imm_callback = {

    /*! Validate function. */
    .validate = NULL,

    /*! Staging function. */
    .stage = vlan_ing_stg_profile_imm_callback_stage,

    /*! Commit function. */
    .commit = NULL,

    /*! Abort function. */
    .abort = NULL
};

/*!
 * \brief Get all current  imm VLAN_ING_EGR_STG_MEMBER_PROFILE LT fields.
 *
 * Get the current fields from VLAN_ING_EGR_STG_MEMBER_PROFILE LT
 *
 * \param [in] unit Unit number.
 * \param [out] VLAN_ING_EGR_STG_MEMBER_PROFILE LT entry data buffer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Faild to get current LT entry fields.
 */
static int
vlan_ing_egr_stg_member_profile_lt_fields_lookup(int unit,
                                                 vlan_ing_egr_stg_member_profile_t *lt_info)
{
#define VLAN_ING_EGR_STG_MEMBER_PRIFILE_FILED_COUNT_MAX 260
    bcmltd_fields_t key_fields = {0};
    bcmltd_fields_t imm_fields = {0};
    bcmltd_fields_t *flist;
    bcmltd_field_t *data_field;
    size_t num_fields = VLAN_ING_EGR_STG_MEMBER_PRIFILE_FILED_COUNT_MAX;
    size_t i;
    uint32_t fid;
    uint64_t fval;
    uint32_t arr_idx;

    SHR_FUNC_ENTER(unit);

    /* Allocate key field */
    SHR_IF_ERR_EXIT
        (vlan_imm_lt_fields_allocate(unit,
                                     &key_fields,
                                     1));

    key_fields.field[0]->id   = VLAN_ING_EGR_STG_MEMBER_PROFILEt_VLAN_ING_EGR_STG_MEMBER_PROFILE_IDf;
    key_fields.field[0]->data = lt_info->stg_id;

    SHR_IF_ERR_EXIT
        (vlan_imm_lt_fields_allocate(unit,
                                     &imm_fields,
                                     num_fields));

    /* Lookup IMM table; error if entry not found */
    SHR_IF_ERR_EXIT
        (bcmimm_entry_lookup(unit,
                             VLAN_ING_EGR_STG_MEMBER_PROFILEt,
                             &key_fields,
                             &imm_fields));

    flist = &imm_fields;

    /* Parse IMM table data fields */
    for (i = 0; i < flist->count; i++) {
        data_field = flist->field[i];
        fid = data_field->id;
        fval = data_field->data;
        arr_idx = data_field->idx;

        switch (fid) {
            case VLAN_ING_EGR_STG_MEMBER_PROFILEt_STATEf:
                lt_info->state[arr_idx] = (uint8_t) fval;
                break;
            case VLAN_ING_EGR_STG_MEMBER_PROFILEt_COMPARED_STATEf:
                lt_info->compared_state = (uint8_t) fval;
                break;
            case VLAN_ING_EGR_STG_MEMBER_PROFILEt_MASK_TARGETf:
                lt_info->mask_target = (uint8_t) fval;
                break;
            case VLAN_ING_EGR_STG_MEMBER_PROFILEt_MASK_ACTIONf:
                lt_info->mask_action = (uint8_t) fval;
                break;
            default:
                SHR_ERR_EXIT(SHR_E_PARAM);
        }
    }

exit:
    vlan_imm_lt_fields_free(unit, &key_fields, 1);
    vlan_imm_lt_fields_free(unit, &imm_fields, num_fields);
    SHR_FUNC_EXIT();
}

static int
vlan_ing_egr_stg_member_profile_lt_fields_key_parse(int unit,
                                                    const bcmltd_field_t *key,
                                                    vlan_ing_egr_stg_member_profile_t *lt_info)
{
    const bcmltd_field_t *gen_field;
    uint32_t fid;
    uint64_t fval;

    SHR_FUNC_ENTER(unit);

    sal_memset(lt_info, 0, sizeof(vlan_ing_egr_stg_member_profile_t));

    /* Parse key field */
    gen_field = key;
    while (gen_field) {
        fid = gen_field->id;
        fval = gen_field->data;

        switch (fid) {
            case VLAN_ING_EGR_STG_MEMBER_PROFILEt_VLAN_ING_EGR_STG_MEMBER_PROFILE_IDf:
                BCMCTH_VLAN_LT_FIELD_SET(
                    lt_info->fbmp, STG_PROFILE_ID);
                lt_info->stg_id = (uint8_t) fval;
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
vlan_ing_egr_stg_member_profile_lt_fields_data_parse(int unit,
                                                     const bcmltd_field_t *data,
                                                     vlan_ing_egr_stg_member_profile_t *lt_info)
{
    const bcmltd_field_t *gen_field;
    uint32_t fid;
    uint64_t fval;
    uint32_t arr_idx;

    SHR_FUNC_ENTER(unit);

    /* Parse data field */
    gen_field = data;
    while (gen_field) {
        fid = gen_field->id;
        fval = gen_field->data;
        arr_idx = gen_field->idx;

        switch (fid) {
            case VLAN_ING_EGR_STG_MEMBER_PROFILEt_STATEf:
                BCMCTH_VLAN_LT_FIELD_SET(
                    lt_info->fbmp, STG_STATE_MASK);
                BCMCTH_VLAN_LT_FIELD_SET(lt_info->state_array_bmp, arr_idx);
                lt_info->state[arr_idx] = (uint8_t) fval;
                break;
            case VLAN_ING_EGR_STG_MEMBER_PROFILEt_COMPARED_STATEf:
                BCMCTH_VLAN_LT_FIELD_SET(
                    lt_info->fbmp, STG_COMPARED_VALUE);
                lt_info->compared_state = (uint8_t) fval;
                break;
            case VLAN_ING_EGR_STG_MEMBER_PROFILEt_MASK_TARGETf:
                BCMCTH_VLAN_LT_FIELD_SET(
                    lt_info->fbmp, STG_MASK_TARGET);
                lt_info->mask_target = (uint8_t) fval;
                break;
            case VLAN_ING_EGR_STG_MEMBER_PROFILEt_MASK_ACTIONf:
                BCMCTH_VLAN_LT_FIELD_SET(
                    lt_info->fbmp, STG_MAKS_ACTION);
                lt_info->mask_action = (uint8_t) fval;
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
 * \param [in] lt_entry VLAN_ING_EGR_STG_MEMBER_PROFILE LT entry data buffer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Input parameter failed validation check.
 */
static int
vlan_ing_egr_stg_member_profile_default_values_init(int unit,
                                                    vlan_ing_egr_stg_member_profile_t *lt_info)
{
    uint64_t def_val = 0;
    uint32_t num;
    int i;
    SHR_FUNC_ENTER(unit);

    /* validate input parameter. */
    SHR_NULL_CHECK(lt_info, SHR_E_PARAM);

    if (!BCMCTH_VLAN_LT_FIELD_GET(
             lt_info->fbmp, STG_STATE_MASK)) {
        SHR_IF_ERR_EXIT(
            bcmlrd_field_default_get(unit,
                                     VLAN_ING_EGR_STG_MEMBER_PROFILEt,
                                     VLAN_ING_EGR_STG_MEMBER_PROFILEt_STATEf,
                                     1, &def_val, &num));
        for (i = 0; i < MAX_PORTS; i++) {
            lt_info->state[i] = (uint8_t)def_val;
            BCMCTH_VLAN_LT_FIELD_SET(lt_info->state_array_bmp, i);
        }
        BCMCTH_VLAN_LT_FIELD_SET(lt_info->fbmp, STG_STATE_MASK);
    } else {
        SHR_IF_ERR_EXIT(
            bcmlrd_field_default_get(unit,
                                     VLAN_ING_EGR_STG_MEMBER_PROFILEt,
                                     VLAN_ING_EGR_STG_MEMBER_PROFILEt_STATEf,
                                     1, &def_val, &num));
        for (i = 0; i < MAX_PORTS; i++) {
            if (BCMCTH_VLAN_LT_FIELD_GET(lt_info->state_array_bmp, i)) {
                continue;
            }
            lt_info->state[i] = (uint8_t)def_val;
            BCMCTH_VLAN_LT_FIELD_SET(lt_info->state_array_bmp, i);
        }
    }

    if (!BCMCTH_VLAN_LT_FIELD_GET(
                     lt_info->fbmp, STG_COMPARED_VALUE)) {
        SHR_IF_ERR_EXIT(
            bcmlrd_field_default_get(unit,
                                     VLAN_ING_EGR_STG_MEMBER_PROFILEt,
                                     VLAN_ING_EGR_STG_MEMBER_PROFILEt_COMPARED_STATEf,
                                     1, &def_val, &num));

        lt_info->compared_state = (uint8_t)def_val;
        BCMCTH_VLAN_LT_FIELD_SET(lt_info->fbmp, STG_COMPARED_VALUE);
    }

    if (!BCMCTH_VLAN_LT_FIELD_GET(
                     lt_info->fbmp, STG_MASK_TARGET)) {
        SHR_IF_ERR_EXIT(
            bcmlrd_field_default_get(unit,
                                     VLAN_ING_EGR_STG_MEMBER_PROFILEt,
                                     VLAN_ING_EGR_STG_MEMBER_PROFILEt_MASK_TARGETf,
                                     1, &def_val, &num));

        lt_info->mask_target = (uint8_t)def_val;
        BCMCTH_VLAN_LT_FIELD_SET(lt_info->fbmp, STG_MASK_TARGET);
    }

    if (!BCMCTH_VLAN_LT_FIELD_GET(
                     lt_info->fbmp, STG_MAKS_ACTION)) {
        SHR_IF_ERR_EXIT(
            bcmlrd_field_default_get(unit,
                                     VLAN_ING_EGR_STG_MEMBER_PROFILEt,
                                     VLAN_ING_EGR_STG_MEMBER_PROFILEt_MASK_ACTIONf,
                                     1, &def_val, &num));

        lt_info->mask_action = (uint8_t)def_val;
        BCMCTH_VLAN_LT_FIELD_SET(lt_info->fbmp, STG_MAKS_ACTION);
    }

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief VLAN_ING_EGR_STG_MEMBER_PROFILE IMM table change callback function for staging.
 *
 * Handle VLAN_ING_EGR_STG_MEMBER_PROFILE IMM table change events.
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
vlan_ing_egr_stg_member_profile_imm_callback_stage(int unit,
                                                   bcmltd_sid_t sid,
                                                   uint32_t trans_id,
                                                   bcmimm_entry_event_t event_reason,
                                                   const bcmltd_field_t *key,
                                                   const bcmltd_field_t *data,
                                                   void *context,
                                                   bcmltd_fields_t *output_fields)
{
    vlan_ing_egr_stg_member_profile_t lt_info;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_ing_egr_stg_member_profile_lt_fields_key_parse(unit, key, &lt_info));

    if (output_fields) {
        output_fields->count = 0;
    }

    switch (event_reason) {
        case BCMIMM_ENTRY_UPDATE:
            SHR_IF_ERR_VERBOSE_EXIT
                (vlan_ing_egr_stg_member_profile_lt_fields_lookup(unit, &lt_info));
            SHR_IF_ERR_VERBOSE_EXIT
                (vlan_ing_egr_stg_member_profile_lt_fields_data_parse(unit, data, &lt_info));
            break;
        case BCMIMM_ENTRY_INSERT:
            SHR_IF_ERR_VERBOSE_EXIT
                (vlan_ing_egr_stg_member_profile_lt_fields_data_parse(unit, data, &lt_info));
            SHR_IF_ERR_VERBOSE_EXIT
                (vlan_ing_egr_stg_member_profile_default_values_init(unit, &lt_info));
            break;
        case BCMIMM_ENTRY_DELETE:
            SHR_IF_ERR_VERBOSE_EXIT
                (vlan_ing_egr_stg_member_profile_default_values_init(unit, &lt_info));
            break;
        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT(
        bcmcth_vlan_ing_egr_stg_member_profile_set(unit,
                                                   trans_id,
                                                   sid,
                                                   &lt_info));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief VLAN_ING_EGR_STG_MEMBER_PROFILE In-memory event callback structure.
 *
 * This structure contains callback functions that will be corresponding
 * to VLAN_ING_EGR_STG_MEMBER_PROFILE logical table entry commit stages.
 */
static bcmimm_lt_cb_t vlan_ing_egr_stg_member_profile_imm_callback = {

    /*! Validate function. */
    .validate = NULL,

    /*! Staging function. */
    .stage = vlan_ing_egr_stg_member_profile_imm_callback_stage,

    /*! Commit function. */
    .commit = NULL,

    /*! Abort function. */
    .abort = NULL
};


/*!
 * \brief Get all current  imm VLAN_EGR_STG_PROFILE LT fields.
 *
 * Get the current fields from VLAN_EGR_STG_PROFILE LT
 *
 * \param [in] unit Unit number.
 * \param [out] VLAN_EGR_STG_PROFILE LT entry data buffer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Faild to get current LT entry fields.
 */
static int
vlan_egr_stg_profile_lt_fields_lookup(int unit,
                                      vlan_egr_stg_profile_t *lt_info)
{
#define VLAN_EGR_STG_PRIFILE_FILED_COUNT_MAX 257
    bcmltd_fields_t key_fields = {0};
    bcmltd_fields_t imm_fields = {0};
    bcmltd_fields_t *flist;
    bcmltd_field_t *data_field;
    size_t num_fields = VLAN_EGR_STG_PRIFILE_FILED_COUNT_MAX;
    size_t i;
    uint32_t fid;
    uint64_t fval;
    uint32_t arr_idx;

    SHR_FUNC_ENTER(unit);

    /* Allocate key field */
    SHR_IF_ERR_EXIT
        (vlan_imm_lt_fields_allocate(unit,
                                     &key_fields,
                                     1));

    key_fields.field[0]->id   = VLAN_EGR_STG_PROFILEt_VLAN_EGR_STG_PROFILE_IDf;
    key_fields.field[0]->data = lt_info->stg_id;

    SHR_IF_ERR_EXIT
        (vlan_imm_lt_fields_allocate(unit,
                                     &imm_fields,
                                     num_fields));

    /* Lookup IMM table; error if entry not found */
    SHR_IF_ERR_EXIT
        (bcmimm_entry_lookup(unit,
                             VLAN_EGR_STG_PROFILEt,
                             &key_fields,
                             &imm_fields));

    flist = &imm_fields;

    /* Parse IMM table data fields */
    for (i = 0; i < flist->count; i++) {
        data_field = flist->field[i];
        fid = data_field->id;
        fval = data_field->data;
        arr_idx = data_field->idx;

        switch (fid) {
            case VLAN_ING_STG_PROFILEt_STATEf:
                lt_info->state[arr_idx] = (uint8_t) fval;
                break;
            default:
                SHR_ERR_EXIT(SHR_E_PARAM);
        }
    }

exit:
    vlan_imm_lt_fields_free(unit, &key_fields, 1);
    vlan_imm_lt_fields_free(unit, &imm_fields, num_fields);
    SHR_FUNC_EXIT();
}

static int
vlan_egr_stg_profile_lt_fields_key_parse(int unit,
                                         const bcmltd_field_t *key,
                                         vlan_egr_stg_profile_t *lt_info)
{
    const bcmltd_field_t *gen_field;
    uint32_t fid;
    uint64_t fval;

    SHR_FUNC_ENTER(unit);

    sal_memset(lt_info, 0, sizeof(vlan_egr_stg_profile_t));

    /* Parse key field */
    gen_field = key;
    while (gen_field) {
        fid = gen_field->id;
        fval = gen_field->data;

        switch (fid) {
            case VLAN_EGR_STG_PROFILEt_VLAN_EGR_STG_PROFILE_IDf:
                BCMCTH_VLAN_LT_FIELD_SET(
                    lt_info->fbmp, STG_PROFILE_ID);
                lt_info->stg_id = (uint8_t) fval;
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
vlan_egr_stg_profile_lt_fields_data_parse(int unit,
                                          const bcmltd_field_t *data,
                                          vlan_egr_stg_profile_t *lt_info)
{
    const bcmltd_field_t *gen_field;
    uint32_t fid;
    uint64_t fval;
    uint32_t arr_idx;

    SHR_FUNC_ENTER(unit);

    /* Parse data field */
    gen_field = data;
    while (gen_field) {
        fid = gen_field->id;
        fval = gen_field->data;
        arr_idx = gen_field->idx;

        switch (fid) {
            case VLAN_EGR_STG_PROFILEt_STATEf:
                BCMCTH_VLAN_LT_FIELD_SET(
                    lt_info->fbmp, STG_STATE);
                BCMCTH_VLAN_LT_FIELD_SET(lt_info->state_array_bmp, arr_idx);
                lt_info->state[arr_idx] = (uint8_t) fval;
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
 * \param [in] lt_entry VLAN_EGR_STG_PROFILE LT entry data buffer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Input parameter failed validation check.
 */
static int
vlan_egr_stg_profile_default_values_init(int unit,
                                         vlan_egr_stg_profile_t *lt_info)
{
    uint64_t def_val = 0;
    uint32_t num;
    int i;
    SHR_FUNC_ENTER(unit);

    /* validate input parameter. */
    SHR_NULL_CHECK(lt_info, SHR_E_PARAM);

    if (!BCMCTH_VLAN_LT_FIELD_GET(
             lt_info->fbmp, STG_STATE)) {
        SHR_IF_ERR_EXIT(
            bcmlrd_field_default_get(unit,
                                     VLAN_EGR_STG_PROFILEt,
                                     VLAN_EGR_STG_PROFILEt_STATEf,
                                     1, &def_val, &num));
        for (i = 0; i < MAX_PORTS; i++) {
            lt_info->state[i] = (uint8_t)def_val;
            BCMCTH_VLAN_LT_FIELD_SET(lt_info->state_array_bmp, i);
        }
        BCMCTH_VLAN_LT_FIELD_SET(lt_info->fbmp, STG_STATE);
    } else {
        SHR_IF_ERR_EXIT(
            bcmlrd_field_default_get(unit,
                                     VLAN_EGR_STG_PROFILEt,
                                     VLAN_EGR_STG_PROFILEt_STATEf,
                                     1, &def_val, &num));
        for (i = 0; i < MAX_PORTS; i++) {
            if (BCMCTH_VLAN_LT_FIELD_GET(lt_info->state_array_bmp, i)) {
                continue;
            }
            lt_info->state[i] = (uint8_t)def_val;
            BCMCTH_VLAN_LT_FIELD_SET(lt_info->state_array_bmp, i);
        }
    }
exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief VLAN_EGR_STG_PROFILE IMM table change callback function for staging.
 *
 * Handle VLAN_EGR_STG_PROFILE IMM table change events.
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
vlan_egr_stg_profile_imm_callback_stage(int unit,
                                        bcmltd_sid_t sid,
                                        uint32_t trans_id,
                                        bcmimm_entry_event_t event_reason,
                                        const bcmltd_field_t *key,
                                        const bcmltd_field_t *data,
                                        void *context,
                                        bcmltd_fields_t *output_fields)
{
    vlan_egr_stg_profile_t lt_info;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (vlan_egr_stg_profile_lt_fields_key_parse(unit, key, &lt_info));

    if (output_fields) {
        output_fields->count = 0;
    }

    switch (event_reason) {
        case BCMIMM_ENTRY_UPDATE:
            SHR_IF_ERR_VERBOSE_EXIT
                (vlan_egr_stg_profile_lt_fields_lookup(unit, &lt_info));
            SHR_IF_ERR_VERBOSE_EXIT
                (vlan_egr_stg_profile_lt_fields_data_parse(unit, data, &lt_info));
            break;
        case BCMIMM_ENTRY_INSERT:
            SHR_IF_ERR_VERBOSE_EXIT
                (vlan_egr_stg_profile_lt_fields_data_parse(unit, data, &lt_info));
            SHR_IF_ERR_VERBOSE_EXIT(
                vlan_egr_stg_profile_default_values_init(unit, &lt_info));
            break;
        case BCMIMM_ENTRY_DELETE:
            SHR_IF_ERR_VERBOSE_EXIT(
                vlan_egr_stg_profile_default_values_init(unit, &lt_info));
            break;
        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }


    SHR_IF_ERR_VERBOSE_EXIT(
        bcmcth_vlan_egr_stg_profile_set(unit,
                                        trans_id,
                                        sid,
                                        &lt_info));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief VLAN_EGR_STG_PROFILE In-memory event callback structure.
 *
 * This structure contains callback functions that will be corresponding
 * to VLAN_EGR_STG_PROFILE logical table entry commit stages.
 */
static bcmimm_lt_cb_t vlan_egr_stg_profile_imm_callback = {

    /*! Validate function. */
    .validate = NULL,

    /*! Staging function. */
    .stage = vlan_egr_stg_profile_imm_callback_stage,

    /*! Commit function. */
    .commit = NULL,

    /*! Abort function. */
    .abort = NULL
};


/*******************************************************************************
 * Public Functions
 */
int
bcmcth_vlan_imm_init(int unit)
{
    const bcmlrd_map_t *map = NULL;
    int rv;

    SHR_FUNC_ENTER(unit);

    /*
     * To register callback for VLAN LTs here.
     */

    rv = bcmlrd_map_get(unit, VLAN_ING_STG_PROFILEt, &map);
    if (SHR_SUCCESS(rv) && map) {
        SHR_IF_ERR_EXIT
            (bcmimm_lt_event_reg(unit,
                                 VLAN_ING_STG_PROFILEt,
                                 &vlan_ing_stg_profile_imm_callback,
                                 NULL));
    }

    rv = bcmlrd_map_get(unit, VLAN_ING_EGR_STG_MEMBER_PROFILEt, &map);
    if (SHR_SUCCESS(rv) && map) {
        SHR_IF_ERR_EXIT
            (bcmimm_lt_event_reg(unit,
                                 VLAN_ING_EGR_STG_MEMBER_PROFILEt,
                                 &vlan_ing_egr_stg_member_profile_imm_callback,
                                 NULL));
    }

    rv = bcmlrd_map_get(unit, VLAN_EGR_STG_PROFILEt, &map);
    if (SHR_SUCCESS(rv) && map) {
        SHR_IF_ERR_EXIT
            (bcmimm_lt_event_reg(unit,
                                 VLAN_EGR_STG_PROFILEt,
                                 &vlan_egr_stg_profile_imm_callback,
                                 NULL));
    }

exit:
    SHR_FUNC_EXIT();
}
