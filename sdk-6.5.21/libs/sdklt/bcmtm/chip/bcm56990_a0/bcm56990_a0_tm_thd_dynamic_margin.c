/*! \file bcm56990_a0_tm_thd_dynamic_margin.c
 *
 * IMM handlers for threshold LTs.
 *
 * This file contains callbcak handlers for LT TM_THD_DYNAMIC_MARGIN.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bcmdrd/chip/bcm56990_a0_enum.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmlrd/bcmlrd_map.h>
#include <bcmimm/bcmimm_int_comp.h>
#include <bcmtm/bcmtm_utils_internal.h>
#include <bcmtm/bcmtm_utils.h>
#include <bcmtm/bcmtm_drv.h>
#include <bcmtm/bcmtm_pt_internal.h>
#include <bcmtm/chip/bcm56990_a0_tm_thd_internal.h>
#include <bcmtm/chip/bcm56990_a0_tm.h>

#include "bcm56990_a0_tm_imm_internal.h"

#define NUM_KEY_FIELDS          2
#define NUM_OPER_FIELDS         3

#define BSL_LOG_MODULE  BSL_LS_BCMTM_IMM_CHIP

typedef struct bcmtm_thd_margin_s {
    uint32_t margin;
    int     valid;
} bcmtm_thd_margin_t;

static int
tm_thd_dynamic_margin_populate_entry(int unit, int instance, int pool_id)
{
    bcmltd_fields_t entry;
    bcmltd_fields_t out;
    bcmltd_sid_t sid = TM_THD_DYNAMIC_MARGINt;
    size_t num_fid;
    bcmtm_pt_info_t pt_dyn_info = {0};
    bcmdrd_sid_t ptid;
    uint32_t ltmbuf[BCMTM_MAX_THD_ENTRY_WSIZE] = {0};
    uint32_t margin, fval;
    bcmtm_drv_info_t *drv_info;
    bcmlrd_fid_t fid = 0;
    int margin_level, idx_count, i = 0;
    int margin_base[] = {
        2, 4, 6, 8, 10, 12, 14, 16, 20, 24
    };
    bcmdrd_sid_t mmu_dynamic_margin[] = {
        MMU_THDO_ADT_SP0_MARGINr, MMU_THDO_ADT_SP1_MARGINr,
        MMU_THDO_ADT_SP2_MARGINr, MMU_THDO_ADT_SP3_MARGINr
    };

    SHR_FUNC_ENTER(unit);

    bcmtm_drv_info_get(unit, &drv_info);
    sal_memset(&entry, 0, sizeof(bcmltd_fields_t));
    sal_memset(&out, 0, sizeof(bcmltd_fields_t));

    fid = TM_THD_DYNAMIC_MARGINt_MARGINf;
    idx_count = bcmlrd_field_idx_count_get(unit, sid, fid);

    /*
     * Populate default entry (value fields) with HW default values.
     * Key fields must be set.
     */
    ptid = mmu_dynamic_margin[pool_id];

    SHR_IF_ERR_EXIT
        (bcmlrd_field_count_get(unit, sid, &num_fid));
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_alloc(unit, num_fid + idx_count - 1, &entry));

    entry.field[i]->id = TM_THD_DYNAMIC_MARGINt_BUFFER_POOLf;
    entry.field[i]->data = instance;
    i++;

    entry.field[i]->id = TM_THD_DYNAMIC_MARGINt_TM_EGR_SERVICE_POOL_IDf;
    entry.field[i]->data = pool_id;
    i++;

    for (margin_level = 0; margin_level < 10; margin_level++) {
        margin = margin_base[margin_level] * 1024 * 1024 / TH4_MMU_BYTES_PER_CELL;
        entry.field[i]->id = TM_THD_DYNAMIC_MARGINt_MARGINf;
        entry.field[i]->data = margin;
        entry.field[i]->idx = margin_level;
        i++;

        fval = margin;
        BCMTM_PT_DYN_INFO(pt_dyn_info, margin_level, instance);
        sal_memset(ltmbuf, 0, sizeof(ltmbuf));
        SHR_IF_ERR_EXIT
            (bcmtm_field_set(unit, ptid, LEVELf, ltmbuf, &fval));
        SHR_IF_ERR_EXIT
            (bcmtm_pt_indexed_write(unit, ptid, sid, &pt_dyn_info, ltmbuf));
    }

    /* Insert/update the LT. (Update is needed during flexport operation.) */
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_alloc(unit, num_fid + idx_count - 1, &out));
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
tm_dynamic_margin_validate(int unit, int itm, bcmdrd_sid_t ptid,
                           bcmtm_thd_margin_t *margin_array)
{
    bcmtm_pt_info_t pt_dyn_info = {0};
    uint32_t ltmbuf[BCMTM_MAX_THD_ENTRY_WSIZE] = {0};
    uint32_t fval;
    int index;

    SHR_FUNC_ENTER(unit);

    for (index = 0; index < 10; index++) {
        if (margin_array[index].valid == 1) {
            continue;
        }
        sal_memset(ltmbuf, 0, sizeof(ltmbuf));
        BCMTM_PT_DYN_INFO(pt_dyn_info, index, itm);
        SHR_IF_ERR_EXIT
            (bcmtm_pt_indexed_read(unit, ptid, -1, &pt_dyn_info, ltmbuf));
        SHR_IF_ERR_EXIT
            (bcmtm_field_get(unit, ptid, LEVELf, ltmbuf, &fval));
        margin_array[index].margin = fval;
    }

    for (index = 0; index < 9; index++) {
        if (margin_array[index].margin > margin_array[index + 1].margin) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

static int
tm_thd_dynamic_margin_update(int unit,
                             bcmltd_sid_t sid,
                             int itm,
                             int pool,
                             const bcmltd_field_t *data_fields,
                             bcmltd_fields_t *output_fields)
{
    const bcmltd_field_t *data_ptr;
    bcmltd_fields_t entry;
    bcmtm_pt_info_t pt_dyn_info = {0};
    bcmdrd_sid_t ptid;
    uint32_t fval, index;
    uint32_t ltmbuf[BCMTM_MAX_THD_ENTRY_WSIZE] = {0};
    bcmltd_fields_t keys;
    size_t num_fid;
    bcmtm_thd_margin_t margin_array[10] = {{0, 0}};

    bcmdrd_sid_t mmu_dynamic_margin[] = {
        MMU_THDO_ADT_SP0_MARGINr, MMU_THDO_ADT_SP1_MARGINr,
        MMU_THDO_ADT_SP2_MARGINr, MMU_THDO_ADT_SP3_MARGINr
    };

    SHR_FUNC_ENTER(unit);

    /* Try to retrieve operational fields from the LT entry. */
    sal_memset(&entry, 0, sizeof(bcmltd_fields_t));
    sal_memset(&keys, 0, sizeof(bcmltd_fields_t));
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_alloc(unit, 2, &keys));
    keys.field[0]->id   = TM_THD_DYNAMIC_MARGINt_BUFFER_POOLf;
    keys.field[0]->data = itm;
    keys.field[1]->id   = TM_THD_DYNAMIC_MARGINt_TM_EGR_SERVICE_POOL_IDf;
    keys.field[1]->data = pool;

    sid = TM_THD_DYNAMIC_MARGINt;
    SHR_IF_ERR_EXIT
        (bcmlrd_field_count_get(unit, sid, &num_fid));
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_alloc(unit, num_fid, &entry));

    /* Initialize output_ields. */
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_size_check(unit, output_fields, 3));
    output_fields->count = 0;

    ptid = mmu_dynamic_margin[pool];
    data_ptr = data_fields;
    while (data_ptr) {
        if (data_ptr->id == TM_THD_DYNAMIC_MARGINt_MARGINf) {
            index = data_ptr->idx;
            fval = data_ptr->data;

            margin_array[index].margin = fval;
            margin_array[index].valid = TRUE;
            data_ptr = data_ptr->next;
        } else {
            data_ptr = data_ptr->next;
        }
    }

    SHR_IF_ERR_EXIT
        (tm_dynamic_margin_validate(unit, itm, ptid, margin_array));

    for (index = 0; index < 10; index++) {
        if (margin_array[index].valid == 0) {
            continue;
        }
        BCMTM_PT_DYN_INFO(pt_dyn_info, index, itm);
        sal_memset(ltmbuf, 0, sizeof(ltmbuf));
        /* Read - modify - write. */
        SHR_IF_ERR_EXIT
            (bcmtm_pt_indexed_read(unit, ptid, sid, &pt_dyn_info, ltmbuf));
        SHR_IF_ERR_EXIT
            (bcmtm_field_set(unit, ptid, LEVELf, ltmbuf, &margin_array[index].margin));
        SHR_IF_ERR_EXIT
            (bcmtm_pt_indexed_write(unit, ptid, sid, &pt_dyn_info, ltmbuf));
    }

exit:
    bcmtm_field_list_free(&keys);
    bcmtm_field_list_free(&entry);
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
tm_thd_dynamic_margin_stage(int unit,
                            bcmltd_sid_t sid,
                            uint32_t trans_id,
                            bcmimm_entry_event_t event_reason,
                            const bcmltd_field_t *key_fields,
                            const bcmltd_field_t *data_fields,
                            void *context,
                            bcmltd_fields_t *output_fields)
{
    const bcmltd_field_t *key_ptr;
    int instance = -1;
    int index = -1;

    SHR_FUNC_ENTER(unit);

    key_ptr = key_fields;
    while (key_ptr) {
        switch (key_ptr->id) {
            case TM_THD_DYNAMIC_MARGINt_BUFFER_POOLf:
                instance = key_ptr->data;
                break;
            case TM_THD_DYNAMIC_MARGINt_TM_EGR_SERVICE_POOL_IDf:
                index = key_ptr->data;
                break;
            default:
                SHR_ERR_EXIT(SHR_E_PARAM);
        }
        key_ptr = key_ptr->next;
    }

    if (instance < 0 || index < 0) {
        return SHR_E_PARAM;
    }

    switch (event_reason) {
        case BCMIMM_ENTRY_INSERT:
            /* Insert shares the same callback with update. */
        case BCMIMM_ENTRY_UPDATE:
            SHR_IF_ERR_EXIT
                (tm_thd_dynamic_margin_update(unit, sid, instance, index,
                                              data_fields, output_fields));
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
bcm56990_a0_tm_thd_dynamic_margin_register(int unit)
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
    tm_thd_cb_handler.stage    = tm_thd_dynamic_margin_stage;

    rv = bcmlrd_map_get(unit, TM_THD_DYNAMIC_MARGINt, &map);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_UNAVAIL);

    if ((rv == SHR_E_UNAVAIL) || (!map)) {
        SHR_EXIT();
    }

    /* Register callback handler for the LT. */
    SHR_IF_ERR_EXIT
        (bcmimm_lt_event_reg(unit, TM_THD_DYNAMIC_MARGINt, &tm_thd_cb_handler, NULL));

exit:
    SHR_FUNC_EXIT();
}

int
bcm56990_a0_tm_thd_dynamic_margin_populate(int unit)
{
    int itm, pool, rv;
    const bcmlrd_map_t *map = NULL;

    SHR_FUNC_ENTER(unit);

    rv = bcmlrd_map_get(unit, TM_THD_DYNAMIC_MARGINt, &map);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_UNAVAIL);

    if ((rv == SHR_E_UNAVAIL) || (!map)) {
        SHR_EXIT();
    }
    for (itm = 0; itm < SOC_MMU_CFG_ITM_MAX; itm++) {
        if (bcm56990_a0_tm_check_valid_itm(unit, itm) != SHR_E_NONE) {
            continue;
        }
        for (pool = 0; pool < SOC_MMU_CFG_SERVICE_POOL_MAX; pool++) {
            SHR_IF_ERR_EXIT
                (tm_thd_dynamic_margin_populate_entry(unit, itm, pool));
        }
    }

exit:
    SHR_FUNC_EXIT();
}

