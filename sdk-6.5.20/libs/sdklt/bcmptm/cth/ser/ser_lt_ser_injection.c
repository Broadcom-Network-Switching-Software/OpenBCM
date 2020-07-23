/*! \file ser_lt_ser_injection.c
 *
 * Functions used to get or update IMM LT SER_INJECTION
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcmimm/bcmimm_int_comp.h>
#include <bsl/bsl.h>
#include <shr/shr_debug.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmdrd/bcmdrd_pt_ser.h>
#include <bcmptm/bcmptm_scor_internal.h>
#include <bcmptm/bcmptm_ser_misc_internal.h>
#include <bcmptm/bcmptm_ser_chip_internal.h>
#include <bcmptm/bcmptm_ser_internal.h>
#include <bcmptm/bcmptm_chip_internal.h>

#include "ser_lts.h"
#include "ser_lt_ser_injection.h"
#include "ser_lt_ser_injection_status.h"
#include "ser_lt_ser_pt_control.h"
#include "ser_inject_and_test.h"
#include "ser_lt_ser_control.h"

/******************************************************************************
 * Local definitions
 */

#define  BSL_LOG_MODULE  BSL_LS_BCMPTM_SER

/*!
 * \brief Get data of a field from cache of LT SER_INJECTIONt.
 *
 *
 * \param [in] unit           Unit number.
 * \param [in] fid_lt         field id.
 *
 * \retval Field data or BCMPTM_SER_INVALID_FIELD_VAL
 */
static uint32_t
bcmptm_ser_injection_field_val_get(int unit, uint32_t fid_lt);

/*! Macros to get value of every field of SER_INJECTIONt */
#define  BCMPTM_SER_INJECTION_PT_ID(_unit) \
    bcmptm_ser_injection_field_val_get(_unit, SER_INJECTIONt_PT_IDf)

#define  BCMPTM_SER_INJECTION_PT_INSTANCE(_unit) \
    bcmptm_ser_injection_field_val_get(_unit, SER_INJECTIONt_PT_INSTANCEf)

#define  BCMPTM_SER_INJECTION_PT_COPY(_unit) \
    bcmptm_ser_injection_field_val_get(_unit, SER_INJECTIONt_PT_COPYf)

#define  BCMPTM_SER_INJECTION_PT_INDEX(_unit) \
    bcmptm_ser_injection_field_val_get(_unit, SER_INJECTIONt_PT_INDEXf)

#define  BCMPTM_SER_INJECTION_XOR_BANK(_unit) \
    bcmptm_ser_injection_field_val_get(_unit, SER_INJECTIONt_XOR_BANKf)

#define  BCMPTM_SER_INJECTION_INJECT_ERR_BIT_NUM(_unit) \
    bcmptm_ser_injection_field_val_get(_unit, SER_INJECTIONt_INJECT_ERR_BIT_NUMf)

#define  BCMPTM_SER_INJECTION_INJECT_VALIDATE(_unit) \
    bcmptm_ser_injection_field_val_get(_unit, SER_INJECTIONt_INJECT_VALIDATEf)

#define  BCMPTM_SER_INJECTION_TIME_TO_WAIT(_unit) \
    bcmptm_ser_injection_field_val_get(_unit, SER_INJECTIONt_TIME_TO_WAITf)

/* Cache for IMM LT SER_INJECTION */
static bcmptm_ser_lt_field_data_t
    lt_ser_injection_cache[SER_INJECTIONt_FIELD_COUNT];

/*!
 * \brief SER LTs In-memory event callback structure.
 *
 * This structure contains callback functions that will be corresponding
 * to SER_INJECTIONt logical table for entry validation.
 */
static int
bcmptm_ser_injection_imm_validation_callback(int unit, bcmltd_sid_t ltid,
                                             bcmimm_entry_event_t event_reason,
                                             const bcmltd_field_t *key,
                                             const bcmltd_field_t *data,
                                             void *context);
/*!
 * \brief SER LTs In-memory event callback structure.
 *
 * This structure contains callback functions that will be corresponding
 * to SER_INJECTIONt logical table entry commit stages.
 */
static int
bcmptm_ser_injection_imm_stage_callback(int unit, bcmltd_sid_t ltid,
                                        uint32_t trans_id,
                                        bcmimm_entry_event_t event_reason,
                                        const bcmltd_field_t *key,
                                        const bcmltd_field_t *data,
                                        void *context,
                                        bcmltd_fields_t *output_fields);

static bcmimm_lt_cb_t bcmptm_ser_injection_imm_callback = {

    /*! Validate function. */
    .validate = bcmptm_ser_injection_imm_validation_callback,

    /*! Staging function. */
    .stage = bcmptm_ser_injection_imm_stage_callback,

    /*! Commit function. */
    .commit = NULL,

    /*! Abort function. */
    .abort = NULL
};

/******************************************************************************
 * Private Functions
 */
/* Save field data of SER_INJECTION to cache. */
static int
bcmptm_ser_injection_cache_update(int unit, uint32_t fid_lt, uint32_t data)
{
    lt_ser_injection_cache[fid_lt].fld_data[unit] = data;
    return SHR_E_NONE;
}

/*
 * During SER injection, we need to take ireq_lock and mreq_lock at first.
 * In order to error data is over-written.
 */
static int
bcmptm_ser_injection_mutex_take(int unit)
{
    int rv;

    rv = bcmptm_ireq_lock(unit);
    if (SHR_FAILURE(rv)) {
        return rv;
    }
    rv = bcmptm_mreq_lock(unit);
    if (SHR_FAILURE(rv)) {
        (void)bcmptm_ireq_unlock( unit);
        return rv;
    }
    return SHR_E_NONE;
}

/*
 * Release ireq_lock and mreq_lock .
 */
static void
bcmptm_ser_injection_mutex_give(int unit)
{
    (void)bcmptm_mreq_unlock(unit);
    (void)bcmptm_ireq_unlock(unit);
}

/*
 * IMM validation callback routine of SER_INJECTION.
 * Use to check inputting parameters from application.
 */
static int
bcmptm_ser_injection_imm_validation_callback(int unit, bcmltd_sid_t ltid,
                                             bcmimm_entry_event_t event_reason,
                                             const bcmltd_field_t *key,
                                             const bcmltd_field_t *data,
                                             void *context)
{
    const bcmltd_field_t *lt_field;
    uint32_t lt_fid, fval;
    int rv = SHR_E_NONE;
    bcmdrd_sid_t pt_id;
    bcmdrd_sid_t tcam_only;
    int inject_err_to_key_fld = 0;
    int index = 0, valid = 0, max_index = 0;
    uint32_t inject_copy = 0, inject_inst = 0;
    uint32_t xor_bank = 0;
    bcmdrd_pt_ser_info_t ser_info;

    SHR_FUNC_ENTER(unit);

    if (!bcmptm_ser_checking_enable(unit)) {
        return SHR_E_DISABLED;
    }
    if (event_reason == BCMIMM_ENTRY_LOOKUP) {
        return SHR_E_NONE;
    } else if (event_reason != BCMIMM_ENTRY_UPDATE) {
        return SHR_E_UNAVAIL;
    }
    /* Parse data field */
    lt_field = data;

    while (lt_field) {
        lt_fid = lt_field->id;
        fval = lt_field->data;
        rv = bcmptm_ser_injection_cache_update(unit, lt_fid, fval);
        if (SHR_FAILURE(rv)) {
            return rv;
        }
        lt_field = lt_field->next;
    }
    pt_id = BCMPTM_SER_INJECTION_PT_ID(unit);
    inject_inst = BCMPTM_SER_INJECTION_PT_INSTANCE(unit);
    inject_copy = BCMPTM_SER_INJECTION_PT_COPY(unit);
    index = (int)BCMPTM_SER_INJECTION_PT_INDEX(unit);
    xor_bank = BCMPTM_SER_INJECTION_XOR_BANK(unit);

    if (bcmdrd_pt_is_valid(unit, pt_id) == 0) {
        return SHR_E_NONE;
    }
    valid = bcmptm_ser_sram_index_valid(unit, pt_id, index, &max_index);
    if (valid == 0) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "Injection index of [%s] is invalid,"
                             " Min index [0], Max index [%d]\n"),
                  bcmdrd_pt_sid_to_name(unit, pt_id), max_index));
        rv = SHR_E_PARAM;
        SHR_ERR_EXIT(rv);
    }
    if (bcmdrd_pt_attr_is_fifo(unit, pt_id)) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "FIFO [%s] cannot be injected SER error\n"),
                  bcmdrd_pt_sid_to_name(unit, pt_id)));
        rv = SHR_E_UNAVAIL;
        SHR_ERR_EXIT(rv);
    }
    /* no ser info */
    (void)bcmptm_ser_tcam_remap(unit, pt_id, &tcam_only, &inject_err_to_key_fld);
    rv = bcmdrd_pt_ser_info_get(unit, tcam_only, &ser_info);
    if (SHR_FAILURE(rv)) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "[%s][%d] may not support SER protection\n"),
                  bcmdrd_pt_sid_to_name(unit, pt_id), pt_id));
        SHR_ERR_EXIT(rv);
    }
    /*
    * If xor_bank is 1, inject SER error to XOR bank.
    * If xor_bank is 0, inject SER error to table bank.
    */
    if (xor_bank == 0) {
        rv = SHR_E_NONE;
    } else if (ser_info.type != BCMDRD_SER_TYPE_DIRECT_XOR &&
               ser_info.type != BCMDRD_SER_TYPE_DIRECT_XOR_LP &&
               ser_info.type != BCMDRD_SER_TYPE_HASH_XOR &&
               ser_info.type != BCMDRD_SER_TYPE_HASH_XOR_LP) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "[%s][%d] may have no XOR bank, its SER type is [%d],"
                             " please assign XOR_BANKf as [0]\n"),
                  bcmdrd_pt_sid_to_name(unit, pt_id), pt_id, ser_info.type));
        rv = SHR_E_PARAM;
        SHR_ERR_EXIT(rv);
    }
    SHR_ERR_EXIT(rv);

exit:
    if (event_reason == BCMIMM_ENTRY_UPDATE) {
        BCMPTM_SER_INJECTION_STATUS_PT_ID_SET(unit, pt_id);
        BCMPTM_SER_INJECTION_STATUS_PT_INSTANCE_SET(unit, inject_inst);
        BCMPTM_SER_INJECTION_STATUS_PT_COPY_SET(unit, inject_copy);
        BCMPTM_SER_INJECTION_STATUS_PT_INDEX_SET(unit, index);
        BCMPTM_SER_INJECTION_STATUS_XOR_BANK_SET(unit, xor_bank);
        BCMPTM_SER_INJECTION_STATUS_SER_ERR_CORRECTED_SET(unit, 0);
        BCMPTM_SER_INJECTION_STATUS_SER_ERR_INJECTED_SET(unit, 0);
        (void)bcmptm_ser_injection_status_imm_update(unit, 0);
    }
    SHR_FUNC_EXIT();
}

/*
 * Check whether instance number and copy number
 * can be used both to access a PT.
 */
static int
bcmptm_ser_pt_inst_check(int unit, bcmdrd_sid_t pt_id, int tbl_inst, int *tbl_copy)
{
    int rv = SHR_E_NONE, tbl_inst_num = 0, pipe_num = 0, copy_num = 0;
    uint32_t pt_depth_per_inst = 0, pt_depth_inst_start = 0, pt_depth_inst_end = 0;
    uint32_t max_index = 0, min_index = 0;
    const char *acctype_str = NULL;
    uint32_t acc_type = 0;
    bool copy_id_valid = FALSE;

    rv = bcmptm_ser_tbl_inst_num_get(unit, pt_id, &tbl_inst_num, &copy_num);
    if(SHR_FAILURE(rv)) {
        return rv;
    }
    copy_id_valid = bcmptm_ser_pt_copy_no_valid (unit, pt_id, tbl_inst, *tbl_copy);
    if (FALSE == copy_id_valid) {
        if (*tbl_copy == 0) {
            /* ovrr_info should be NULL */
            *tbl_copy = -1;
        }
    }
    rv = bcmptm_ser_pt_acctype_get(unit, pt_id, &acc_type, &acctype_str);
    if(SHR_FAILURE(rv)) {
        return rv;
    }
    /*
    * Memory EGR_1588_SAm just have one instance, its access type is ADDR_SPLIT_SPLIT.
    * Register IBCAST_64r etc. have 140 instance, its access type also is ADDR_SPLIT_SPLIT.
    */
    if ((PT_IS_ADDR_SPLIT_ACC_TYPE(acctype_str) ||
         PT_IS_ADDR_SPLIT_SPLIT_ACC_TYPE(acctype_str))) {
        if (tbl_inst < tbl_inst_num) {
            return SHR_E_NONE;
        } else {
            LOG_WARN(BSL_LOG_MODULE,
                     (BSL_META_U(unit,
                                 "Injection instance [%d] of [%s] should be less than [%d]\n"),
                      tbl_inst, bcmdrd_pt_sid_to_name(unit, pt_id), tbl_inst_num));
        }
        max_index = bcmptm_scor_pt_index_max(unit, pt_id);
        min_index = bcmptm_scor_pt_index_min(unit, pt_id);
        /* Registers which access type is ADDR_SPLIT_SPLIT */
        if (max_index == min_index) {
            return SHR_E_PARAM;
        }
        pipe_num = bcmdrd_pt_num_pipe_inst(unit, pt_id);
        if (pipe_num <= tbl_inst) {
            return SHR_E_PARAM;
        }
        pt_depth_per_inst = (max_index - min_index + 1) / pipe_num;
        pt_depth_inst_start = tbl_inst * pt_depth_per_inst;
        pt_depth_inst_end = pt_depth_inst_start + pt_depth_per_inst - 1;

        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "Access-type of [%s] is [%s], BD does not support inst [%d].\n"),
                  bcmdrd_pt_sid_to_name(unit, pt_id),
                  PT_IS_ADDR_SPLIT_ACC_TYPE(acctype_str) ? "Addr-split" : "Addr-split-split",
                  tbl_inst));
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "Indexes from [%d] to [%d] are in inst [%d].\n"),
                             pt_depth_inst_start, pt_depth_inst_end, tbl_inst));
        return SHR_E_PARAM;
    }
    return SHR_E_NONE;
}

/*
 * IMM stage callback routine.
 * When a application update SER_INJECTION, this callback will be called to
 * inject SER error into a PT.
 */
static int
bcmptm_ser_injection_imm_stage_callback(int unit, bcmltd_sid_t ltid,
                                        uint32_t trans_id,
                                        bcmimm_entry_event_t event_reason,
                                        const bcmltd_field_t *key,
                                        const bcmltd_field_t *data,
                                        void *context,
                                        bcmltd_fields_t *output_fields)
{
    bcmdrd_sid_t pt_id, pt_id_mapped;
    uint32_t fv_index = 0, flags = 0, xor_bank = 0;
    uint32_t single_bit_err = SER_SINGLE_BIT_ERR, validate = SER_VALIDATION;
    int rv = SHR_E_NONE, is_tcam = 0, inject_err_to_key_fld = 0;
    int tbl_inst = 0, remap_idx = 0, tbl_copy = 0;
    sal_time_t usleep = 0;
    const char *acctype_str = NULL;
    uint32_t acc_type = 0;
    uint32_t sram_scan_enable = 0, tcam_scan_enable = 0;

    SHR_FUNC_ENTER(unit);

    if (output_fields) {
        output_fields->count = 0;
    }
    pt_id = BCMPTM_SER_INJECTION_PT_ID(unit);
    tbl_inst = (int)BCMPTM_SER_INJECTION_PT_INSTANCE(unit);
    tbl_copy = (int)BCMPTM_SER_INJECTION_PT_COPY(unit);
    fv_index = BCMPTM_SER_INJECTION_PT_INDEX(unit);
    validate = BCMPTM_SER_INJECTION_INJECT_VALIDATE(unit);
    xor_bank = BCMPTM_SER_INJECTION_XOR_BANK(unit);
    single_bit_err = BCMPTM_SER_INJECTION_INJECT_ERR_BIT_NUM(unit);
    flags |= (single_bit_err == SER_SINGLE_BIT_ERR) ? 0 : BCMPTM_SER_INJECT_ERROR_2BIT;
    flags |= (xor_bank) ? BCMPTM_SER_INJECT_ERROR_XOR_BANK : 0;
    pt_id_mapped = pt_id;

    if (bcmdrd_pt_is_valid(unit, pt_id) == 0) {
        return SHR_E_NONE;
    }

    rv = bcmptm_ser_refresh_regs_config(unit, 1);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    tcam_scan_enable = bcmptm_ser_control_field_val_get(unit, SER_CONTROLt_TCAM_SCANf);
    sram_scan_enable = bcmptm_ser_control_field_val_get(unit, SER_CONTROLt_SRAM_SCANf);
    if (tcam_scan_enable) {
        rv = bcmptm_ser_tcam_scan_stop(unit);
        SHR_IF_ERR_EXIT(rv);
    }
    if (sram_scan_enable) {
        rv = bcmptm_ser_sram_scan_stop(unit);
        SHR_IF_ERR_EXIT(rv);
    }
    is_tcam = bcmdrd_pt_attr_is_cam(unit, pt_id);
    if (is_tcam) {
        rv = bcmptm_ser_pt_inst_check(unit, pt_id, tbl_inst, &tbl_copy);
        SHR_IF_ERR_VERBOSE_EXIT(rv);
        /* Map pt_id to pt_id_mapped which is protected by TCAM SER engine */
        (void)bcmptm_ser_tcam_remap(unit, pt_id, &pt_id_mapped,
                                    &inject_err_to_key_fld);
        remap_idx = fv_index;
        /* TCAM_ONLY view, inject SER error to key and mask field */
        if (inject_err_to_key_fld) {
            flags |= BCMPTM_SER_INJECT_ERROR_KEY_MASK;
        }
        rv = bcmptm_ser_pt_acctype_get(unit, pt_id_mapped, &acc_type, &acctype_str);
        SHR_IF_ERR_VERBOSE_EXIT(rv);
        /* Use AGGR view instead of Tcam_only view to validate SER error */
        /* access type of AGGR view is addr_split; TCAM only view is duplicate */
        if (PT_IS_ADDR_SPLIT_ACC_TYPE(acctype_str)) {
            tbl_inst = 0;
        }
        rv = bcmptm_ser_injection_mutex_take(unit);
        if (SHR_FAILURE(rv)) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit, "Fail to take ireq or mreq locks\n")));
            SHR_IF_ERR_VERBOSE_EXIT(rv);
        }
        /* inject SER error to Tcam_only instead of AGGR view */
        rv = bcmptm_ser_tcam_ser_inject(unit, pt_id_mapped,
                                        tbl_inst, remap_idx, flags);
    } else {
        /* SER error is injected to ECC view, Take L2 memory for example, access type
         * of functional view is DATA-SPLIT, however ECC view is duplicate.
         * SER error can be injected to instance 1, 2, 3 of ECC view,
         * but can't be injected to instance 1, 2, 3 of functional view.
         */
        rv = bcmptm_ser_mem_ecc_info_get(unit, pt_id, fv_index,
                                         &pt_id_mapped, &remap_idx, NULL);
        SHR_IF_ERR_VERBOSE_EXIT(rv);

        rv = bcmptm_ser_pt_inst_check(unit, pt_id_mapped, tbl_inst, &tbl_copy);
        SHR_IF_ERR_VERBOSE_EXIT(rv);

        rv = bcmptm_ser_injection_mutex_take(unit);
        if (SHR_FAILURE(rv)) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit, "Fail to take ireq or mreq locks\n")));
            SHR_IF_ERR_VERBOSE_EXIT(rv);
        }
        if (xor_bank) {
            rv = bcmptm_ser_xor_bank_ser_inject(unit, pt_id_mapped,
                                                tbl_inst, remap_idx, flags);
        } else {
            rv = bcmptm_ser_sram_ser_inject(unit, pt_id_mapped, tbl_inst,
                                            tbl_copy, remap_idx, flags);
        }
    }
    (void)bcmptm_ser_injection_mutex_give(unit);

    SHR_IF_ERR_VERBOSE_EXIT(rv);

    /* update PT information to cache of SER_INJECTION_STATUS LT */
    BCMPTM_SER_INJECTION_STATUS_SER_ERR_INJECTED_SET(unit, 1);

    if (validate == SER_VALIDATION) {
        usleep = (sal_time_t)BCMPTM_SER_INJECTION_TIME_TO_WAIT(unit) * 1000;

        rv = bcmptm_ser_err_injection_validate(unit, pt_id_mapped, tbl_inst, tbl_copy,
                                               remap_idx, usleep, flags);
        /* update PT information to cache of SER_INJECTION_STATUS LT */
        BCMPTM_SER_INJECTION_STATUS_SER_ERR_CORRECTED_SET(unit, SHR_FAILURE(rv) ? 0 : 1);
    }
    SHR_ERR_EXIT(rv);

exit:
    if (tcam_scan_enable) {
        (void)bcmptm_ser_tcam_scan_start(unit);
    }
    if (sram_scan_enable) {
        (void)bcmptm_ser_sram_scan_start(unit, pt_id_mapped);
    }
    (void)bcmptm_ser_refresh_regs_config(unit, 0);

    if (!SHR_FUNC_ERR()) {
        BCMPTM_SER_INJECTION_STATUS_PT_ID_SET(unit, pt_id_mapped);
        BCMPTM_SER_INJECTION_STATUS_PT_INSTANCE_SET(unit, tbl_inst);
        BCMPTM_SER_INJECTION_STATUS_PT_INDEX_SET(unit, remap_idx);
        BCMPTM_SER_INJECTION_STATUS_XOR_BANK_SET(unit, xor_bank);
    }
    /* update PT information to IMM of SER_INJECTION_STATUS LT */
    (void)bcmptm_ser_injection_status_imm_update(unit, 0);
    SHR_FUNC_EXIT();
}

/*  Get field data from cache of SER_INJECTIONt */
static uint32_t
bcmptm_ser_injection_field_val_get(int unit, uint32_t fid_lt)
{
    return lt_ser_injection_cache[fid_lt].fld_data[unit];
}

/******************************************************************************
 * Public Functions
 */
int
bcmptm_ser_injection_imm_init(int unit)
{
    shr_fmm_t** fld_array = NULL;
    int rv = SHR_E_NONE;
    bcmltd_fields_t lt_flds;
    uint32_t fld_num = SER_INJECTIONt_FIELD_COUNT, lt_fid;
    bcmptm_ser_lt_field_data_t *cache_ptr = lt_ser_injection_cache;
    bcmlrd_client_field_info_t *fields_info = NULL;
    size_t j, actual_fld_num, field_info_size;

    SHR_FUNC_ENTER(unit);

    fld_array = bcmptm_ser_imm_flds_allocate(unit, fld_num);
    SHR_NULL_CHECK(fld_array, SHR_E_MEMORY);

    lt_flds.field = fld_array;
    lt_flds.count = fld_num;

    field_info_size = sizeof(bcmlrd_client_field_info_t) * fld_num;
    fields_info = sal_alloc(field_info_size , "bcmptmSerInjectFldInfo");
    SHR_NULL_CHECK(fields_info, SHR_E_MEMORY);

    sal_memset(fields_info, 0, field_info_size);
    rv = bcmlrd_table_fields_def_get(unit, "SER_INJECTION",
                                     (size_t)fld_num, fields_info, &actual_fld_num);
    SHR_IF_ERR_EXIT(rv);

    for (lt_fid = 0; lt_fid < fld_num; lt_fid++) {
        for (j = 0; j < actual_fld_num; j++) {
            if (fields_info[j].id == lt_fid) {
                break;
            }
        }
        if (j == actual_fld_num) {
            SHR_IF_ERR_EXIT(SHR_E_NOT_FOUND);
        }
        BCMPTM_SER_FLD_DEF_VAL_GET(fields_info[j], cache_ptr[lt_fid].fld_data[unit]);

        bcmptm_ser_imm_fld_set(lt_flds, lt_fid, lt_fid, 0,
                               cache_ptr[lt_fid].fld_data[unit]);
    }
    rv = bcmimm_entry_insert(unit, SER_INJECTIONt, &lt_flds);
    SHR_IF_ERR_EXIT(rv);

exit:
    if (fld_array) {
        bcmptm_ser_imm_flds_free(unit, fld_array, fld_num);
    }
    if (fields_info) {
        sal_free(fields_info);
    }
    if (SHR_FUNC_ERR()) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Fail to commit data to SER_INJECTIONt\n")));
    }
    SHR_FUNC_EXIT();
}

int
bcmptm_ser_injection_callback_register(int unit)
{
    SHR_FUNC_ENTER(unit);
    /* To register callback for SER LTs here. */
    SHR_IF_ERR_EXIT
        (bcmimm_lt_event_reg(unit,
                             SER_INJECTIONt,
                             &bcmptm_ser_injection_imm_callback,
                             NULL));
exit:
    SHR_FUNC_EXIT();
}

