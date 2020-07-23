/*! \file bcm56990_b0_tm_ing_thd_service_pool.c
 *
 * IMM handlers for threshold LTs.
 *
 * This file contains callbcak handlers for LT TM_ING_THD_SERVICE_POOL.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <shr/shr_bitop.h>
#include <bcmdrd/chip/bcm56990_b0_enum.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmlrd/bcmlrd_map.h>
#include <bcmimm/bcmimm_int_comp.h>
#include <bcmtm/bcmtm_utils_internal.h>
#include <bcmtm/bcmtm_drv.h>
#include <bcmtm/bcmtm_pt_internal.h>
#include <bcmtm/chip/bcm56990_b0_tm_thd_internal.h>
#include <bcmtm/chip/bcm56990_b0_tm.h>
#include <bcmcth/bcmcth_imm_util.h>

#include "bcm56990_b0_tm_imm_internal.h"

/* TM_ING_THD_SERVICE_POOL_ENTRY_STATE_T */
#define VALID                   0
#define INCORRECT_SHARED_LIMIT  1
#define INCORRECT_RESUME_LIMIT  2
#define INCORRECT_COLOR_LIMIT   3

#define BSL_LOG_MODULE  BSL_LS_BCMTM_IMM_CHIP

static int
tm_thd_ing_service_pool_populate_entry(int unit, int itm, int pool)
{
    bcmltd_fields_t entry;
    bcmltd_fields_t out;
    bcmltd_sid_t sid = TM_ING_THD_SERVICE_POOLt;
    bcmtm_pt_info_t pt_dyn_info = {0};
    bcmdrd_sid_t ptid;
    uint32_t shared_limit, resume_offset, yellow_offset, red_offset;
    SHR_BITDCL color_limit_bmp[1] = {0};
    uint32_t ltmbuf[BCMTM_MAX_THD_ENTRY_WSIZE] = {0};
    bool enable_color_limit;
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
            TM_ING_THD_SERVICE_POOLt_BUFFER_POOLf, 0, itm));
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_set(unit, &entry,
            TM_ING_THD_SERVICE_POOLt_TM_ING_SERVICE_POOL_IDf, 0, pool));

    /* Read the current limits from HW. */
    /* Per pool per ITM reg/mem. */
    BCMTM_PT_DYN_INFO(pt_dyn_info, pool, itm);

    /* Read pool shared limit. */
    ptid = MMU_THDI_BUFFER_CELL_LIMIT_SPr;
    sal_memset(ltmbuf, 0, sizeof(ltmbuf));
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, ptid, sid, &pt_dyn_info, ltmbuf));
    SHR_IF_ERR_EXIT
        (bcmtm_field_get(unit, ptid, LIMITf, ltmbuf, &shared_limit));

    /* Read pool resume offset. */
    ptid = MMU_THDI_CELL_RESET_LIMIT_OFFSET_SPr;
    sal_memset(ltmbuf, 0, sizeof(ltmbuf));
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, ptid, sid, &pt_dyn_info, ltmbuf));
    SHR_IF_ERR_EXIT
        (bcmtm_field_get(unit, ptid, OFFSETf, ltmbuf, &resume_offset));

    /* Read pool yellow offset. */
    ptid = MMU_THDI_CELL_SPAP_YELLOW_OFFSET_SPr;
    sal_memset(ltmbuf, 0, sizeof(ltmbuf));
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, ptid, sid, &pt_dyn_info, ltmbuf));
    SHR_IF_ERR_EXIT
        (bcmtm_field_get(unit, ptid, OFFSETf, ltmbuf, &yellow_offset));

    /* Read pool red offset. */
    ptid = MMU_THDI_CELL_SPAP_RED_OFFSET_SPr;
    sal_memset(ltmbuf, 0, sizeof(ltmbuf));
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, ptid, sid, &pt_dyn_info, ltmbuf));
    SHR_IF_ERR_EXIT
        (bcmtm_field_get(unit, ptid, OFFSETf, ltmbuf, &red_offset));

    /* Read per ITM color limit control bitmap. */
    ptid = MMU_THDI_POOL_CONFIGr;
    BCMTM_PT_DYN_INFO(pt_dyn_info, 0, itm);
    sal_memset(ltmbuf, 0, sizeof(ltmbuf));
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, ptid, sid, &pt_dyn_info, ltmbuf));
    SHR_IF_ERR_EXIT
        (bcmtm_field_get(unit, ptid, COLOR_AWAREf, ltmbuf, color_limit_bmp));
    enable_color_limit = (bool) SHR_BITGET(color_limit_bmp, pool);

    /* Write to field list. */
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_set(unit, &entry,
                TM_ING_THD_SERVICE_POOLt_OPERATIONAL_STATEf, 0, VALID));
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_set(unit, &entry,
                TM_ING_THD_SERVICE_POOLt_RED_OFFSET_CELLSf, 0, red_offset));
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_set(unit, &entry,
            TM_ING_THD_SERVICE_POOLt_YELLOW_OFFSET_CELLSf, 0, yellow_offset));
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_set(unit, &entry,
            TM_ING_THD_SERVICE_POOLt_COLOR_SPECIFIC_LIMITSf, 0, enable_color_limit));
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_set(unit, &entry,
            TM_ING_THD_SERVICE_POOLt_SHARED_RESUME_OFFSET_CELLSf, 0, resume_offset));
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_set(unit, &entry,
            TM_ING_THD_SERVICE_POOLt_SHARED_LIMIT_CELLS_OPERf, 0, shared_limit));
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_set(unit, &entry,
            TM_ING_THD_SERVICE_POOLt_SHARED_LIMIT_CELLSf, 0, shared_limit));

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
tm_thd_ing_service_pool_update(int unit,
                               bcmltd_sid_t sid,
                               const bcmltd_op_arg_t *op_arg,
                               int itm,
                               int pool,
                               const bcmltd_field_t *data_fields,
                               bcmltd_fields_t *output_fields)
{
    const bcmltd_field_t *data_ptr;
    bcmtm_pt_info_t pt_dyn_info = {0};
    bcmdrd_sid_t ptid;
    SHR_BITDCL color_limit_bmp[1] = {0};
    uint32_t ltmbuf[BCMTM_MAX_THD_ENTRY_WSIZE] = {0};
    uint32_t shared_limit, fval;
    bool write_flag;
    long int delta;
    int rv;
    uint64_t flags = 0;

    SHR_FUNC_ENTER(unit);
    if (op_arg) {
        flags = bcmptm_lt_entry_attrib_to_ptm_req_flags(op_arg->attrib);
    }

    SHR_IF_ERR_EXIT
        (bcmtm_field_list_size_check(unit, output_fields, 2));
    output_fields->count = 0;

    data_ptr = data_fields;
    while (data_ptr) {
        write_flag = TRUE;
        switch (data_ptr->id) {
            case TM_ING_THD_SERVICE_POOLt_RED_OFFSET_CELLSf:
                ptid = MMU_THDI_CELL_SPAP_RED_OFFSET_SPr;
                break;
            case TM_ING_THD_SERVICE_POOLt_YELLOW_OFFSET_CELLSf:
                ptid = MMU_THDI_CELL_SPAP_YELLOW_OFFSET_SPr;
                break;
            case TM_ING_THD_SERVICE_POOLt_SHARED_RESUME_OFFSET_CELLSf:
                ptid = MMU_THDI_CELL_RESET_LIMIT_OFFSET_SPr;
                break;
            case TM_ING_THD_SERVICE_POOLt_COLOR_SPECIFIC_LIMITSf:
                ptid = MMU_THDI_POOL_CONFIGr;
                /* BMP requires different sequence. */
                BCMTM_PT_DYN_INFO(pt_dyn_info, 0, itm);
                pt_dyn_info.flags = flags;
                sal_memset(ltmbuf, 0, sizeof(ltmbuf));
                SHR_IF_ERR_EXIT
                    (bcmtm_pt_indexed_read(unit, ptid, sid, &pt_dyn_info, ltmbuf));
                SHR_IF_ERR_EXIT
                    (bcmtm_field_get(unit, ptid, COLOR_AWAREf, ltmbuf, color_limit_bmp));
                if (data_ptr->data) {
                    SHR_BITSET(color_limit_bmp, pool);
                } else {
                    SHR_BITCLR(color_limit_bmp, pool);
                }
                SHR_IF_ERR_EXIT
                    (bcmtm_field_set(unit, ptid, COLOR_AWAREf, ltmbuf, color_limit_bmp));
                SHR_IF_ERR_EXIT
                    (bcmtm_pt_indexed_write(unit, ptid, sid, &pt_dyn_info, ltmbuf));
                /* Skip the common write sequence. */
                write_flag = FALSE;
                break;
            case TM_ING_THD_SERVICE_POOLt_SHARED_LIMIT_CELLSf:
                /* Check if there is enough buffer size to allocate the new limit. */
                ptid = MMU_THDI_BUFFER_CELL_LIMIT_SPr;
                BCMTM_PT_DYN_INFO(pt_dyn_info, pool, itm);
                pt_dyn_info.flags = flags;
                sal_memset(ltmbuf, 0, sizeof(ltmbuf));
                SHR_IF_ERR_EXIT
                    (bcmtm_pt_indexed_read(unit, ptid, sid, &pt_dyn_info, ltmbuf));
                SHR_IF_ERR_EXIT
                    (bcmtm_field_get(unit, ptid, LIMITf, ltmbuf, &shared_limit));
                delta = (long int) data_ptr->data - shared_limit;
                rv = bcm56990_b0_tm_shared_buffer_update(unit, itm, pool, delta,
                                                         ING_SERVICE_POOL_UPDATE,
                                                         FALSE);
                if (rv == SHR_E_NONE) {
                    /*
                     * Request accepted. All limits have been adjusted. Update
                     * corresponding operational field.
                     */
                    SHR_IF_ERR_EXIT
                        (bcmtm_field_list_set(unit, output_fields,
                            TM_ING_THD_SERVICE_POOLt_SHARED_LIMIT_CELLS_OPERf,
                            0, data_ptr->data));
                    SHR_IF_ERR_EXIT
                        (bcmtm_field_list_set(unit, output_fields,
                            TM_ING_THD_SERVICE_POOLt_OPERATIONAL_STATEf,
                            0, VALID));
                } else if (rv == SHR_E_PARAM) {
                    /* Request declined. Update operational state field. */
                    SHR_IF_ERR_EXIT
                        (bcmtm_field_list_set(unit, output_fields,
                            TM_ING_THD_SERVICE_POOLt_OPERATIONAL_STATEf,
                            0, INCORRECT_SHARED_LIMIT));
                } else {
                    SHR_ERR_EXIT(rv);
                }
                write_flag = FALSE;
                break;
            default:
                write_flag = FALSE;
                break;
        }
        if (write_flag) {
            fval = data_ptr->data;
            BCMTM_PT_DYN_INFO(pt_dyn_info, pool, itm);
            sal_memset(ltmbuf, 0, sizeof(ltmbuf));
            /* Read - modify - write. */
            SHR_IF_ERR_EXIT
                (bcmtm_pt_indexed_read(unit, ptid, sid, &pt_dyn_info, ltmbuf));
            SHR_IF_ERR_EXIT
                (bcmtm_field_set(unit, ptid, OFFSETf, ltmbuf, &fval));
            SHR_IF_ERR_EXIT
                (bcmtm_pt_indexed_write(unit, ptid, sid, &pt_dyn_info, ltmbuf));
        }
        data_ptr = data_ptr->next;
    }

exit:
    SHR_FUNC_EXIT();
}

static int
tm_thd_ing_service_pool_delete(int unit, bcmltd_sid_t sid,
                               const bcmltd_op_arg_t *op_arg,
                               int itm, int pool)
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
        (tm_thd_ing_service_pool_update(unit, sid, op_arg, itm, pool, data_fields,
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
tm_thd_ing_service_pool_stage(int unit,
                              bcmltd_sid_t sid,
                              const bcmltd_op_arg_t *op_arg,
                              bcmimm_entry_event_t event_reason,
                              const bcmltd_field_t *key_fields,
                              const bcmltd_field_t *data_fields,
                              void *context,
                              bcmltd_fields_t *output_fields)
{
    const bcmltd_field_t *key_ptr;
    int itm = -1;
    int pool = -1;

    SHR_FUNC_ENTER(unit);

    key_ptr = key_fields;
    while (key_ptr) {
        switch (key_ptr->id) {
            case TM_ING_THD_SERVICE_POOLt_BUFFER_POOLf:
                itm = key_ptr->data;
                break;
            case TM_ING_THD_SERVICE_POOLt_TM_ING_SERVICE_POOL_IDf:
                pool = key_ptr->data;
                break;
            default:
                SHR_ERR_EXIT(SHR_E_PARAM);
        }
        key_ptr = key_ptr->next;
    }

    if (itm < 0 || pool < 0) {
        return SHR_E_PARAM;
    }

    switch (event_reason) {
        case BCMIMM_ENTRY_INSERT:
            /* Insert shares the same callback with update. */
        case BCMIMM_ENTRY_UPDATE:
            SHR_IF_ERR_EXIT
                (tm_thd_ing_service_pool_update(unit, sid, op_arg, itm, pool,
                                                data_fields, output_fields));
            break;
        case BCMIMM_ENTRY_DELETE:
            SHR_IF_ERR_EXIT
                (tm_thd_ing_service_pool_delete(unit, sid, op_arg, itm, pool));
            break;
        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

static int
tm_thd_ing_service_pool_lookup(int unit,
                               bcmltd_sid_t sid,
                               const bcmltd_op_arg_t *op_arg,
                               void *context,
                               bcmimm_lookup_type_t lkup_type,
                               const bcmltd_fields_t *in,
                               bcmltd_fields_t *out)
{
    const bcmltd_fields_t *key;
    bcmtm_pt_info_t pt_dyn_info = {0};
    bcmdrd_sid_t ptid;
    uint32_t fid = 0, fval = 0;
    uint64_t idx = 0, req_flags;
    int itm, pool;
    SHR_BITDCL color_limit_bmp[1] = {0};
    uint32_t ltmbuf[BCMTM_MAX_THD_ENTRY_WSIZE] = {0};

    SHR_FUNC_ENTER(unit);

    if (!(op_arg->attrib & BCMLT_ENT_ATTR_GET_FROM_HW)) {
        SHR_EXIT();
    }
    req_flags = bcmptm_lt_entry_attrib_to_ptm_req_flags(op_arg->attrib);

    key = (lkup_type == BCMIMM_TRAVERSE) ? out : in;
    fid = TM_ING_THD_SERVICE_POOLt_BUFFER_POOLf;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_imm_fields_get(unit, key, fid, &idx));
    itm = idx;
    fid = TM_ING_THD_SERVICE_POOLt_TM_ING_SERVICE_POOL_IDf;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_imm_fields_get(unit, key, fid, &idx));
    pool = idx;

    BCMTM_PT_DYN_INFO(pt_dyn_info, pool, itm);
    pt_dyn_info.flags = req_flags;

    ptid = MMU_THDI_CELL_SPAP_RED_OFFSET_SPr;
    sal_memset(ltmbuf, 0, sizeof(ltmbuf));
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, ptid, sid, &pt_dyn_info, ltmbuf));
    SHR_IF_ERR_EXIT(
        bcmtm_field_get(unit, ptid, OFFSETf, ltmbuf, &fval));
    SHR_IF_ERR_EXIT(
        bcmcth_imm_field_update(unit, out,
                                TM_ING_THD_SERVICE_POOLt_RED_OFFSET_CELLSf,
                                fval));

    ptid = MMU_THDI_CELL_SPAP_YELLOW_OFFSET_SPr;
    sal_memset(ltmbuf, 0, sizeof(ltmbuf));
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, ptid, sid, &pt_dyn_info, ltmbuf));
    SHR_IF_ERR_EXIT(
        bcmtm_field_get(unit, ptid, OFFSETf, ltmbuf, &fval));
    SHR_IF_ERR_EXIT(
        bcmcth_imm_field_update(unit, out,
                                TM_ING_THD_SERVICE_POOLt_YELLOW_OFFSET_CELLSf,
                                fval));

    ptid = MMU_THDI_CELL_RESET_LIMIT_OFFSET_SPr;
    sal_memset(ltmbuf, 0, sizeof(ltmbuf));
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, ptid, sid, &pt_dyn_info, ltmbuf));
    SHR_IF_ERR_EXIT(
        bcmtm_field_get(unit, ptid, OFFSETf, ltmbuf, &fval));
    SHR_IF_ERR_EXIT(
        bcmcth_imm_field_update(unit, out,
                                TM_ING_THD_SERVICE_POOLt_SHARED_RESUME_OFFSET_CELLSf,
                                fval));

    ptid = MMU_THDI_BUFFER_CELL_LIMIT_SPr;
    sal_memset(ltmbuf, 0, sizeof(ltmbuf));
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, ptid, sid, &pt_dyn_info, ltmbuf));
    SHR_IF_ERR_EXIT(
        bcmtm_field_get(unit, ptid, LIMITf, ltmbuf, &fval));
    SHR_IF_ERR_EXIT(
        bcmcth_imm_field_update(unit, out,
                                TM_ING_THD_SERVICE_POOLt_SHARED_LIMIT_CELLSf,
                                fval));

    BCMTM_PT_DYN_INFO(pt_dyn_info, 0, itm);
    pt_dyn_info.flags = req_flags;
    ptid = MMU_THDI_POOL_CONFIGr;
    sal_memset(ltmbuf, 0, sizeof(ltmbuf));
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, ptid, sid, &pt_dyn_info, ltmbuf));
    SHR_IF_ERR_EXIT(
        bcmtm_field_get(unit, ptid, COLOR_AWAREf, ltmbuf, color_limit_bmp));
    SHR_IF_ERR_EXIT(
        bcmcth_imm_field_update(unit, out,
                                TM_ING_THD_SERVICE_POOLt_COLOR_SPECIFIC_LIMITSf,
                                SHR_BITGET(color_limit_bmp, pool) ? 1 : 0));

exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public Functions
 */
int
bcm56990_b0_tm_thd_ing_service_pool_register(int unit)
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
    tm_thd_cb_handler.op_stage = tm_thd_ing_service_pool_stage;
    tm_thd_cb_handler.op_lookup = tm_thd_ing_service_pool_lookup;

    rv = bcmlrd_map_get(unit, TM_ING_THD_SERVICE_POOLt, &map);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_UNAVAIL);

    if ((rv == SHR_E_UNAVAIL) || (!map)) {
        SHR_EXIT();
    }

    /* Register callback handler for the LT. */
    SHR_IF_ERR_EXIT
        (bcmimm_lt_event_reg(unit, TM_ING_THD_SERVICE_POOLt,
                             &tm_thd_cb_handler, NULL));

exit:
    SHR_FUNC_EXIT();
}

int
bcm56990_b0_tm_thd_ing_service_pool_populate(int unit)
{
    int itm, pool, rv;
    const bcmlrd_map_t *map = NULL;

    SHR_FUNC_ENTER(unit);
    rv = bcmlrd_map_get(unit, TM_ING_THD_SERVICE_POOLt, &map);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_UNAVAIL);

    if ((rv == SHR_E_UNAVAIL) || (!map)) {
        SHR_EXIT();
    }


    for (itm = 0; itm < SOC_MMU_CFG_ITM_MAX; itm++) {
        if (bcm56990_b0_tm_check_valid_itm(unit, itm) != SHR_E_NONE) {
            continue;
        }
        for (pool = 0; pool < SOC_MMU_CFG_SERVICE_POOL_MAX; pool++) {
            SHR_IF_ERR_EXIT
                (tm_thd_ing_service_pool_populate_entry(unit, itm, pool));
        }
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcm56990_b0_tm_ing_pool_shared_limit_lt_update(int unit, int itm, int pool,
                                               uint32_t fval)
{
    bcmltd_fields_t in;
    bcmltd_fields_t out;
    bcmltd_sid_t sid = TM_ING_THD_SERVICE_POOLt;
    uint64_t cur_limit;
    size_t num_fid;

    SHR_FUNC_ENTER(unit);

    sal_memset(&in, 0, sizeof(bcmltd_fields_t));
    sal_memset(&out, 0, sizeof(bcmltd_fields_t));

    /* Allocate in field list. */
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_alloc(unit, 3, &in));
    in.field[0]->id   = TM_ING_THD_SERVICE_POOLt_TM_ING_SERVICE_POOL_IDf;
    in.field[0]->data = pool;
    in.field[1]->id   = TM_ING_THD_SERVICE_POOLt_BUFFER_POOLf;
    in.field[1]->data = itm;
    in.field[2]->id   = TM_ING_THD_SERVICE_POOLt_SHARED_LIMIT_CELLS_OPERf;
    in.field[2]->data = fval;

    /* Allocate out field list. */
    SHR_IF_ERR_EXIT
        (bcmlrd_field_count_get(unit, sid, &num_fid));
    SHR_IF_ERR_EXIT
        (bcmtm_field_list_alloc(unit, num_fid, &out));

    /* Check if entry exist before update. */
    if (SHR_SUCCESS(bcmimm_entry_lookup(unit, sid, &in, &out))) {
        SHR_IF_ERR_EXIT
            (bcmtm_field_list_get(unit, &out,
                TM_ING_THD_SERVICE_POOLt_SHARED_LIMIT_CELLS_OPERf, 0, &cur_limit));
        if (fval == cur_limit) {
            SHR_EXIT();
        }

        /* Update. (bcmimm_entry_update will call notify API automatically.) */
        SHR_IF_ERR_EXIT
            (bcmimm_entry_update(unit, TRUE, sid, &in));
    }

exit:
    bcmtm_field_list_free(&in);
    bcmtm_field_list_free(&out);
    SHR_FUNC_EXIT();
}
