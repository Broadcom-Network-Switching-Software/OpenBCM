/*! \file bcm56780_a0_tm_ing_thd_headroom_pool.c
 *
 * IMM handlers for threshold LTs.
 *
 * This file contains callbcak handlers for LT TM_ING_THD_HEADROOM_POOL.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bcmdrd/chip/bcm56780_a0_enum.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmlrd/bcmlrd_map.h>
#include <bcmimm/bcmimm_int_comp.h>
#include <bcmtm/bcmtm_utils_internal.h>
#include <bcmtm/bcmtm_drv.h>
#include <bcmtm/bcmtm_pt_internal.h>
#include <bcmtm/chip/bcm56780_a0_tm_thd_internal.h>
#include <bcmtm/chip/bcm56780_a0_tm.h>

#include "bcm56780_a0_tm_imm_internal.h"

/* TM_ING_THD_HEADROOM_POOL_ENTRY_STATE_T */
#define VALID                       0
#define INCORRECT_HEADROOM_LIMIT    1

#define BSL_LOG_MODULE  BSL_LS_BCMTM_IMM_CHIP

static int
tm_thd_ing_headroom_pool_populate_entry(int unit, int itm, int pool)
{
    bcmltd_fields_t entry;
    bcmltd_fields_t out;
    bcmltd_sid_t sid = TM_ING_THD_HEADROOM_POOLt;
    bcmdrd_sid_t ptid = MMU_THDI_HDRM_BUFFER_CELL_LIMIT_HPr;
    bcmdrd_fid_t fid = LIMITf;
    bcmtm_pt_info_t pt_dyn_info = {0};
    uint32_t ltmbuf[BCMTM_MAX_THD_ENTRY_WSIZE] = {0};
    uint32_t limit;
    size_t num_fid;

    SHR_FUNC_ENTER(unit);

    sal_memset(&entry, 0, sizeof(bcmltd_fields_t));
    sal_memset(&out, 0, sizeof(bcmltd_fields_t));

    /*
     * Populate default entry (value fields) with HW default values.
     * Key fields must be set.
     */
    SHR_IF_ERR_EXIT
        (bcmlrd_field_count_get(unit, sid, &num_fid));
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_alloc(unit, num_fid, &entry));
    SHR_IF_ERR_EXIT
        (bcmtm_default_entry_array_construct(unit, sid, &entry));
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_set(unit, &entry,
            TM_ING_THD_HEADROOM_POOLt_BUFFER_POOLf, 0, itm));
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_set(unit, &entry,
            TM_ING_THD_HEADROOM_POOLt_TM_HEADROOM_POOL_IDf, 0, pool));

    /* Read the current limit from HW. */
    BCMTM_PT_DYN_INFO(pt_dyn_info, pool, itm);
    sal_memset(ltmbuf, 0, sizeof(ltmbuf));
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, ptid, sid, &pt_dyn_info, ltmbuf));
    SHR_IF_ERR_EXIT
        (bcmtm_field_get(unit, ptid, fid, ltmbuf, &limit));

    /* Write to field list. */
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_set(unit, &entry,
            TM_ING_THD_HEADROOM_POOLt_LIMIT_CELLS_OPERf, 0, limit));
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_set(unit, &entry,
            TM_ING_THD_HEADROOM_POOLt_LIMIT_CELLSf, 0, limit));

    /* Insert/update the LT. (Update is needed during flexport operation.) */
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_alloc(unit, num_fid, &out));
    if (SHR_SUCCESS(bcmimm_entry_lookup(unit, sid, &entry, &out))) {
        SHR_IF_ERR_EXIT
            (bcmimm_entry_update(unit, FALSE, sid, &entry));
    } else {
        SHR_IF_ERR_EXIT
            (bcmimm_entry_insert(unit, sid, &entry));
    }

exit:
    bcmtm_field_list_free(&entry);
    bcmtm_field_list_free(&out);
    SHR_FUNC_EXIT();
}

static int
tm_thd_ing_headroom_pool_update(int unit,
                                bcmltd_sid_t sid,
                                int itm,
                                int pool,
                                const bcmltd_field_t *data_fields,
                                bcmltd_fields_t *output_fields)
{
    const bcmltd_field_t *data_ptr;
    bcmltd_fields_t entry;
    bcmltd_fields_t keys;
    bcmdrd_sid_t ptid = MMU_THDI_HDRM_BUFFER_CELL_LIMIT_HPr;
    bcmdrd_fid_t fid = LIMITf;
    bcmtm_pt_info_t pt_dyn_info = {0};
    uint32_t ltmbuf[BCMTM_MAX_THD_ENTRY_WSIZE] = {0};
    uint32_t limit, new_limit;
    long int delta;
    uint64_t field_data;
    size_t num_fid;
    int rv;

    SHR_FUNC_ENTER(unit);

    /* Try to retrieve operational fields from the LT entry. */
    sal_memset(&entry, 0, sizeof(bcmltd_fields_t));
    sal_memset(&keys, 0, sizeof(bcmltd_fields_t));
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_alloc(unit, 2, &keys));
    keys.field[0]->id   = TM_ING_THD_HEADROOM_POOLt_BUFFER_POOLf;
    keys.field[0]->data = itm;
    keys.field[1]->id   = TM_ING_THD_HEADROOM_POOLt_TM_HEADROOM_POOL_IDf;
    keys.field[1]->data = pool;
    SHR_IF_ERR_EXIT
        (bcmlrd_field_count_get(unit, sid, &num_fid));
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_alloc(unit, num_fid, &entry));

    /* Initialize output_ields. */
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_size_check(unit, output_fields, 2));
    output_fields->count = 0;

    if (SHR_SUCCESS(bcmimm_entry_lookup(unit, sid, &keys, &entry))) {
        /* LT entry available. */
        SHR_IF_ERR_EXIT
            (bcmtm_field_list_get(unit, &entry,
                TM_ING_THD_HEADROOM_POOLt_LIMIT_CELLS_OPERf, 0, &field_data));
        SHR_IF_ERR_EXIT
            (bcmtm_field_list_set(unit, output_fields,
                TM_ING_THD_HEADROOM_POOLt_LIMIT_CELLS_OPERf, 0, field_data));
        limit = (uint32_t)field_data;
        /* Free field list at the exit label. */
    } else {
        /* Read HW to get the values. */
        BCMTM_PT_DYN_INFO(pt_dyn_info, pool, itm);
        sal_memset(ltmbuf, 0, sizeof(ltmbuf));
        SHR_IF_ERR_EXIT
            (bcmtm_pt_indexed_read(unit, ptid, sid, &pt_dyn_info, ltmbuf));
        SHR_IF_ERR_EXIT
            (bcmtm_field_get(unit, ptid, fid, ltmbuf, &limit));
        SHR_IF_ERR_EXIT
            (bcmtm_field_list_set(unit, output_fields,
                TM_ING_THD_HEADROOM_POOLt_LIMIT_CELLS_OPERf, 0, limit));

        /* OPERATIONAL_STATE is VALID by default. */
        SHR_IF_ERR_EXIT
            (bcmtm_field_list_set(unit, output_fields,
                TM_ING_THD_HEADROOM_POOLt_OPERATIONAL_STATEf, 0, VALID));
    }


    /* Insert/update command with user configured value. */
    data_ptr = data_fields;
    while (data_ptr) {
        if (data_ptr->id == TM_ING_THD_HEADROOM_POOLt_LIMIT_CELLSf) {
            new_limit = data_ptr->data;
        } else {
            data_ptr = data_ptr->next;
            continue;
        }
        /*
         * Calculate delta of available shared size.
         * delta of shared_size always equals to -(delta of reserved_size).
         */
        delta = -((long int) new_limit - limit);
        /* Check if there is enough buffer size to allocate the delta. */
        rv = bcm56780_a0_tm_shared_buffer_update(unit, itm, pool, delta,
                                                 RESERVED_BUFFER_UPDATE, FALSE);
        if (rv == SHR_E_NONE) {
            /*
             * Request accepted. Other limmits have been adjusted. Update
             * headroom limit in HW and update corresponding operational field.
             */
            BCMTM_PT_DYN_INFO(pt_dyn_info, pool, itm);
            sal_memset(ltmbuf, 0, sizeof(ltmbuf));
            SHR_IF_ERR_EXIT
                (bcmtm_pt_indexed_read(unit, ptid, sid, &pt_dyn_info, ltmbuf));
            SHR_IF_ERR_EXIT
                (bcmtm_field_set(unit, ptid, fid, ltmbuf, &new_limit));
            SHR_IF_ERR_EXIT
                (bcmtm_pt_indexed_write(unit, ptid, sid, &pt_dyn_info, ltmbuf));

            /* Update operational fields. */
            SHR_IF_ERR_EXIT
                (bcmtm_field_list_set(unit, output_fields,
                    TM_ING_THD_HEADROOM_POOLt_LIMIT_CELLS_OPERf, 0, new_limit));
            SHR_IF_ERR_EXIT
                (bcmtm_field_list_set(unit, output_fields,
                    TM_ING_THD_HEADROOM_POOLt_OPERATIONAL_STATEf, 0, VALID));
        } else if (rv == SHR_E_PARAM) {
            /* Request declined. Update operational state field. */
            SHR_IF_ERR_EXIT
                (bcmtm_field_list_set(unit, output_fields,
                    TM_ING_THD_HEADROOM_POOLt_OPERATIONAL_STATEf, 0, INCORRECT_HEADROOM_LIMIT));
        } else {
            return rv;
        }
        /* No need to traverse the linked list if LIMIT_CELLSf is processed. */
        break;
    }

exit:
    bcmtm_field_list_free(&keys);
    bcmtm_field_list_free(&entry);
    SHR_FUNC_EXIT();
}

static int
tm_thd_ing_headroom_pool_delete(int unit, bcmltd_sid_t sid, int itm, int pool)
{
    /* Should reset all fields to HW default when delete. */
    bcmltd_field_t *data_fields = NULL;
    bcmltd_fields_t dummy_fields;
    size_t num_fid;

    SHR_FUNC_ENTER(unit);

    sal_memset(&dummy_fields, 0, sizeof(bcmltd_fields_t));

    /* Create data_fields with default field values. */
    SHR_IF_ERR_EXIT
        (bcmtm_default_entry_ll_construct(unit, sid, &data_fields));

    /* Reuse update callback to write HW. */
    SHR_IF_ERR_EXIT
        (bcmlrd_field_count_get(unit, sid, &num_fid));
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_alloc(unit, num_fid, &dummy_fields));

    SHR_IF_ERR_EXIT
        (tm_thd_ing_headroom_pool_update(unit, sid, itm, pool, data_fields,
                                         &dummy_fields));

exit:
    SHR_FREE(data_fields);
    bcmtm_field_list_free(&dummy_fields);
    SHR_FUNC_EXIT();
}

/*!
 * \brief In-memory logical table staging callback function.
 *
 * \param [in] unit This is device unit number.
 * \param [in] sid This is the logical table ID.
 * \param [in] trans_id is the transaction ID associated with this operation.
 * \param [in] event_reason This is the reason for the entry event.
 * \param [in] key_fields This is a linked list of the key fields identifying
 * the entry.
 * \param [in] data_field This is a linked list of the data fields in the
 * modified entry.
 * \param [in] context Is a pointer that was given during registration.
 * The callback can use this pointer to retrieve some context.
 * \param [out] output_fields This output parameter can be used by the component
 * to add fields into the entry. Typically this should be used for read-only
 * type fields that otherwise can not be set by the application.
 * The component must set the count field of the \c output_fields parameter to
 * indicate the actual number of fields that were set.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
tm_thd_ing_headroom_pool_stage(int unit,
                               bcmltd_sid_t sid,
                               uint32_t trans_id,
                               bcmimm_entry_event_t event_reason,
                               const bcmltd_field_t *key_fields,
                               const bcmltd_field_t *data_fields,
                               void *context,
                               bcmltd_fields_t *output_fields)
{
    const bcmltd_field_t *key_ptr;
    int pool = -1;
    int itm = -1;

     SHR_FUNC_ENTER(unit);

    key_ptr = key_fields;
    while (key_ptr) {
        switch (key_ptr->id) {
            case TM_ING_THD_HEADROOM_POOLt_TM_HEADROOM_POOL_IDf:
                pool = key_ptr->data;
                break;
            case TM_ING_THD_HEADROOM_POOLt_BUFFER_POOLf:
                itm = key_ptr->data;
                break;
            default:
                SHR_ERR_EXIT(SHR_E_PARAM);
        }
        key_ptr = key_ptr->next;
    }

    if (pool < 0 || itm < 0) {
        return SHR_E_PARAM;
    }

    switch (event_reason) {
        case BCMIMM_ENTRY_INSERT:
            /* Insert shares the same callback with update. */
        case BCMIMM_ENTRY_UPDATE:
            SHR_IF_ERR_EXIT
                (tm_thd_ing_headroom_pool_update(unit, sid, itm, pool,
                                                 data_fields, output_fields));
            break;
        case BCMIMM_ENTRY_DELETE:
            SHR_IF_ERR_EXIT
                (tm_thd_ing_headroom_pool_delete(unit, sid, itm, pool));
            break;
        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public Functions
 */

int
bcm56780_a0_tm_thd_ing_headroom_pool_register(int unit)
{
    bcmimm_lt_cb_t tm_thd_cb_handler;
    const bcmlrd_map_t *map = NULL;
    int rv;

    SHR_FUNC_ENTER(unit);

    /*
     * Assign callback functions to IMM event callback handler.
     * commit and abort functions can be added if necessary.
     */
    sal_memset(&tm_thd_cb_handler, 0, sizeof(tm_thd_cb_handler));
    tm_thd_cb_handler.stage    = tm_thd_ing_headroom_pool_stage;

    rv = bcmlrd_map_get(unit, TM_ING_THD_HEADROOM_POOLt, &map);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_UNAVAIL);

    if ((rv == SHR_E_UNAVAIL)|| (!map)) {
        SHR_EXIT();
    }

    /* Register callback handler for the LT. */
    SHR_IF_ERR_EXIT
        (bcmimm_lt_event_reg(unit, TM_ING_THD_HEADROOM_POOLt,
                             &tm_thd_cb_handler, NULL));

exit:
    SHR_FUNC_EXIT();
}

int
bcm56780_a0_tm_thd_ing_headroom_pool_populate(int unit)
{
    int pool, rv;
    const bcmlrd_map_t *map = NULL;

    SHR_FUNC_ENTER(unit);
    rv = bcmlrd_map_get(unit, TM_ING_THD_HEADROOM_POOLt, &map);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_UNAVAIL);
    if ((rv == SHR_E_UNAVAIL)|| (!map)) {
        SHR_EXIT();
    }

    for (pool = 0; pool < SOC_MMU_CFG_SERVICE_POOL_MAX; pool++) {
        SHR_IF_ERR_EXIT
            (tm_thd_ing_headroom_pool_populate_entry(unit, 0, pool));
    }

exit:
    SHR_FUNC_EXIT();
}
