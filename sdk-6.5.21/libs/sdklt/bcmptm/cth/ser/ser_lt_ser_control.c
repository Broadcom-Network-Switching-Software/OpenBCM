/*! \file ser_lt_ser_control.c
 *
 * Functions used to get or update cache and IMM of SER_CONTROLt
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcmimm/bcmimm_int_comp.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bsl/bsl.h>
#include <shr/shr_debug.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmdrd/bcmdrd_ser.h>
#include <bcmptm/bcmptm_ser_internal.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmptm/bcmptm_ser_chip_internal.h>
#include <bcmptm/bcmptm_scor_internal.h>

#include "ser_lts.h"
#include "ser_lt_ser_control.h"
#include "ser_lt_ser_pt_control.h"
#include "ser_lt_ser_log.h"

/******************************************************************************
 * Local definitions
 */
#define  BSL_LOG_MODULE  BSL_LS_BCMPTM_SER

/* Cache for IMM LT SER_CONTROL */
static bcmptm_ser_lt_field_data_t lt_ser_control_cache[SER_CONTROLt_FIELD_COUNT];

/*!
 * \brief SER LTs In-memory event callback structure.
 *
 * This structure contains callback functions that will be corresponding
 * to SER_CONTROLt logical table for entry validation.
 */
static int
bcmptm_ser_control_imm_validation_callback(int unit, bcmltd_sid_t ltid,
                                           bcmimm_entry_event_t event_reason,
                                           const bcmltd_field_t *key,
                                           const bcmltd_field_t *data,
                                           void *context);
/*!
 * \brief SER LTs In-memory event callback structure.
 *
 * This structure contains callback functions that will be corresponding
 * to SER_CONTROLt logical table entry commit stages.
 */
static int
bcmptm_ser_control_imm_stage_callback(int unit, bcmltd_sid_t ltid,
                                      uint32_t trans_id,
                                      bcmimm_entry_event_t event_reason,
                                      const bcmltd_field_t *key,
                                      const bcmltd_field_t *data,
                                      void *context,
                                      bcmltd_fields_t *output_fields);

static bcmimm_lt_cb_t bcmptm_ser_control_imm_callback = {

    /*! Validate function. */
    .validate = bcmptm_ser_control_imm_validation_callback,

    /*! Staging function. */
    .stage = bcmptm_ser_control_imm_stage_callback,

    /*! Commit function. */
    .commit = NULL,

    /*! Abort function. */
    .abort = NULL
};

/******************************************************************************
 * Private Functions
 */
/* Save field data of SER_CONTROL to cache. */
static void
bcmptm_ser_control_cache_update(int unit, uint32_t fid_lt, uint32_t data)
{
    lt_ser_control_cache[fid_lt].fld_data[unit] = data;
}

/* IMM validation callback routine of SER_CONTROL. */
static int
bcmptm_ser_control_imm_validation_callback(int unit, bcmltd_sid_t ltid,
                                           bcmimm_entry_event_t event_reason,
                                           const bcmltd_field_t *key,
                                           const bcmltd_field_t *data,
                                           void *context)
{
    if (!bcmptm_ser_checking_enable(unit)) {
        return SHR_E_DISABLED;
    }
    if (event_reason != BCMIMM_ENTRY_LOOKUP &&
        event_reason != BCMIMM_ENTRY_UPDATE) {
        return SHR_E_UNAVAIL;
    }
    return SHR_E_NONE;
}

/*
 * A packet can be cast away due to a SER error,
 * if 'copy-to-CPU' is enabled by this function, the packet will
 * be copied to CPU instead of being cast away.
 */
static int
bcmptm_ser_control_copy_to_cpu(int unit,
                                uint32_t parity_err_enable)
{
    bcmptm_ser_ctrl_reg_info_t *regs_ctrl_info = NULL;
    int  reg_num = 0;
    int rv = SHR_E_NONE;
    int  index = 0;
    bcmdrd_fid_t fid = 0;
    uint32_t entry_data[BCMDRD_MAX_PT_WSIZE] = {0};

    SHR_FUNC_ENTER(unit);

    rv = bcmptm_ser_copy_to_cpu_regs_info_get(unit, &regs_ctrl_info, &reg_num);
    SHR_IF_ERR_VERBOSE_EXIT(rv);
    rv = bcmptm_ser_pt_read(unit, regs_ctrl_info[0].ctrl_reg,
                            index, -1, -1, entry_data, BCMDRD_MAX_PT_WSIZE,
                            0);
    SHR_IF_ERR_VERBOSE_EXIT(rv);
    fid = regs_ctrl_info[0].ctrl_flds_val[0].ctrl_fld;
    rv = bcmdrd_pt_field_set(unit, regs_ctrl_info[0].ctrl_reg,
                             entry_data, fid, &parity_err_enable);
    SHR_IF_ERR_VERBOSE_EXIT(rv);
    rv = bcmptm_ser_pt_write(unit, regs_ctrl_info[0].ctrl_reg,
                             index, -1, -1, entry_data,
                             BCMPTM_SCOR_REQ_FLAGS_ALSO_WRITE_CACHE);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

exit:
    SHR_FUNC_EXIT();
}

/*
 * IMM stage callback routine.
 * When a application update SER_CONTROL, this callback will be called to
 * enable or disable memory scan, SER check, etc.
 */
static int
bcmptm_ser_control_imm_stage_callback(int unit, bcmltd_sid_t ltid,
                                      uint32_t trans_id,
                                      bcmimm_entry_event_t event_reason,
                                      const bcmltd_field_t *key,
                                      const bcmltd_field_t *data,
                                      void *context,
                                      bcmltd_fields_t *output_fields)
{
    const bcmltd_field_t *lt_field;
    uint32_t lt_fid;
    uint32_t fval;
    int rv = SHR_E_NONE;
    uint32_t update_1bit = 0, en_1bit = 1, scan_interval[2];
    uint32_t tcam_scan_enable = 0, tcam_update = 0;
    uint32_t sram_scan_enable = 0, sram_update = 0;
    uint32_t update_hsf = 0, hsf_suppression = 0;
    uint32_t update_single = 0, auto_correction = 0;
    uint32_t pt_id;
    size_t sid_num;
    uint32_t parity_err_to_cpu[2], logging[2];

    SHR_FUNC_ENTER(unit);

    if (output_fields) {
        output_fields->count = 0;
    }
    parity_err_to_cpu[0] = bcmptm_ser_control_field_val_get
        (unit, SER_CONTROLt_PARITY_ERROR_TO_CPUf);
    scan_interval[0] = bcmptm_ser_control_field_val_get
        (unit, SER_CONTROLt_SRAM_SCAN_INTERVALf);
    logging[0] = bcmptm_ser_control_field_val_get
        (unit, SER_CONTROLt_SER_LOGGINGf);
    logging[1] = logging[0];

    /* Parse data field */
    lt_field = data;
    while (lt_field) {
        lt_fid = lt_field->id;
        fval = lt_field->data;
        if (lt_fid == SER_CONTROLt_SER_LOGGINGf) {
            logging[1] = fval;
            lt_field = lt_field->next;
            continue;
        }
        bcmptm_ser_control_cache_update(unit, lt_fid, fval);

        if (lt_fid == SER_CONTROLt_SRAM_SCANf) {
            sram_update = 1;
            sram_scan_enable = fval;
        } else if (lt_fid == SER_CONTROLt_TCAM_SCANf) {
            tcam_update = 1;
            tcam_scan_enable = fval;
        } else if (lt_fid == SER_CONTROLt_REPORT_SINGLE_BIT_ECCf) {
            update_1bit = 1;
            en_1bit = fval;
        } else if (lt_fid == SER_CONTROLt_HIGH_SEVERITY_ERR_SUPPRESSIONf) {
            update_hsf = 1;
            hsf_suppression = fval;
        } else if (lt_fid == SER_CONTROLt_TCAM_SINGLE_BIT_AUTO_CORRECTIONf) {
            update_single = 1;
            auto_correction = fval;
        }
        lt_field = lt_field->next;
    }
    if (sram_update) {
        if (sram_scan_enable) {
            rv = bcmptm_ser_sram_scan_start(unit, 0);
        } else {
            rv = bcmptm_ser_sram_scan_stop(unit);
        }
        SHR_IF_ERR_EXIT(rv);
    }
    if (tcam_update) {
        if (tcam_scan_enable) {
            rv = bcmptm_ser_tcam_scan_start(unit);
        } else {
            rv = bcmptm_ser_tcam_scan_stop(unit);
        }
        SHR_IF_ERR_EXIT(rv);
    }
    parity_err_to_cpu[1] = bcmptm_ser_control_field_val_get
        (unit, SER_CONTROLt_PARITY_ERROR_TO_CPUf);
    scan_interval[1] = bcmptm_ser_control_field_val_get
        (unit, SER_CONTROLt_SRAM_SCAN_INTERVALf);

    /* disable or enable all PTs ECC 1bit report */
    if (update_1bit) {
        rv = bcmptm_pt_sid_count_get(unit, &sid_num);
        SHR_IF_ERR_EXIT(rv);

        for (pt_id = 0; pt_id < sid_num; pt_id++) {
            if (bcmptm_ser_pt_control_field_val_get(unit, pt_id, 0) == 0) {
                continue;
            }
            rv = bcmptm_ser_pt_ser_enable(unit, pt_id,
                                          BCMDRD_SER_EN_TYPE_ECC1B,
                                          en_1bit);
            (void)bcmptm_ser_pt_control_imm_update
                (unit, pt_id, SER_PT_CONTROLt_REPORT_SINGLE_BIT_ECCf,
                 (en_1bit && SHR_SUCCESS(rv)) ? 1 : 0);
        }
    }
    if (update_hsf) {
        rv = bcmptm_pt_sid_count_get(unit, &sid_num);
        SHR_IF_ERR_EXIT(rv);

        for (pt_id = 0; pt_id < sid_num; pt_id++) {
            if (bcmptm_ser_pt_control_field_val_get(unit, pt_id, 0) == 0) {
                continue;
            }
            (void)bcmptm_ser_pt_control_imm_update
                (unit, pt_id, SER_PT_CONTROLt_HIGH_SEVERITY_ERR_SUPPRESSIONf,
                 hsf_suppression ? 1 : 0);
        }
    }
    if (update_single) {
        rv = bcmptm_ser_tcam_single_bit_auto_correct_enable(unit, auto_correction);
        SHR_IF_ERR_EXIT(rv);
    }
    if (parity_err_to_cpu[0] != parity_err_to_cpu[1]) {
        rv = bcmptm_ser_control_copy_to_cpu(unit, parity_err_to_cpu[1]);
        SHR_IF_ERR_EXIT(rv);
    }
    if (scan_interval[0] != scan_interval[1]) {
        rv = bcmptm_ser_sram_scan_resume(unit);
        SHR_IF_ERR_EXIT(rv);
    }
    if (logging[0] != logging[1]) {
        rv = bcmptm_ser_log_cache_clear(unit, bcmptm_ser_control_cache_update,
                                        logging[1]);
        SHR_IF_ERR_EXIT(rv);
    }

exit:
    SHR_FUNC_EXIT();
}

/******************************************************************************
 * Public Functions
 */
uint32_t
bcmptm_ser_control_field_val_get(int unit, uint32_t fid_lt)
{
    return lt_ser_control_cache[fid_lt].fld_data[unit];
}

int
bcmptm_ser_control_callback_register(int unit)
{
    SHR_FUNC_ENTER(unit);
    /* To register callback for SER LTs here. */
    SHR_IF_ERR_EXIT
        (bcmimm_lt_event_reg(unit,
                             SER_CONTROLt,
                             &bcmptm_ser_control_imm_callback,
                             NULL));
exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_ser_control_imm_init(int unit)
{
    shr_fmm_t** fld_array = NULL;
    bcmltd_fields_t lt_flds;
    uint32_t count = 0, lt_fid;
    bcmptm_ser_lt_field_data_t *cache_ptr = lt_ser_control_cache;
    uint32_t fld_num = SER_CONTROLt_FIELD_COUNT;
    int rv;
    bcmlrd_client_field_info_t *fields_info = NULL;
    size_t j, actual_fld_num, field_info_size;

    SHR_FUNC_ENTER(unit);

    field_info_size = sizeof(bcmlrd_client_field_info_t) * fld_num;
    fields_info = sal_alloc(field_info_size , "bcmptmSerCtrlFldInfo");
    SHR_NULL_CHECK(fields_info, SHR_E_MEMORY);

    sal_memset(fields_info, 0, field_info_size);
    rv = bcmlrd_table_fields_def_get(unit, "SER_CONTROL", (size_t)fld_num,
                                     fields_info, &actual_fld_num);
    SHR_IF_ERR_EXIT(rv);

    fld_array = bcmptm_ser_imm_flds_allocate(unit, fld_num);
    SHR_NULL_CHECK(fields_info, SHR_E_MEMORY);

    lt_flds.field = fld_array;
    for (lt_fid = 0; lt_fid < fld_num; lt_fid++) {
        /* Set field value as default value */
        for (j = 0; j < actual_fld_num; j++) {
            if (fields_info[j].id == lt_fid) {
                break;
            }
        }
        if (j == actual_fld_num) {
            continue;
        }
        BCMPTM_SER_FLD_DEF_VAL_GET(fields_info[j],
                                   cache_ptr[lt_fid].fld_data[unit]);
        bcmptm_ser_imm_fld_set(lt_flds, count, lt_fid,
                               0, cache_ptr[lt_fid].fld_data[unit]);
        count++;
    }
    lt_flds.count = count;
    rv = bcmimm_entry_insert(unit, SER_CONTROLt, &lt_flds);
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
                              "Fail to commit data to SER_CONTROLt\n")));
    }
    SHR_FUNC_EXIT();
}

int
bcmptm_ser_control_cache_sync(int unit)
{
    shr_fmm_t** fld_array = NULL;
    int rv = SHR_E_NONE;
    size_t i = 0;
    bcmltd_fields_t lt_flds_in, lt_flds_out;

    SHR_FUNC_ENTER(unit);

    lt_flds_out.field = NULL;

    lt_flds_in.count = 0;
    lt_flds_in.field = NULL;

    fld_array = bcmptm_ser_imm_flds_allocate(unit, SER_CONTROLt_FIELD_COUNT);
    SHR_NULL_CHECK(fld_array, SHR_E_MEMORY);

    lt_flds_out.field = fld_array;
    lt_flds_out.count = SER_CONTROLt_FIELD_COUNT;

    rv = bcmimm_entry_lookup(unit, SER_CONTROLt, &lt_flds_in, &lt_flds_out);
    SHR_IF_ERR_EXIT(rv);

    for (i = 0; i < lt_flds_out.count; i++) {
        bcmptm_ser_control_cache_update(unit, lt_flds_out.field[i]->id,
                                        lt_flds_out.field[i]->data);
    }
exit:
    if (lt_flds_out.field != NULL) {
        /* lt_flds_out.count may be modified by bcmimm_entry_lookup */
        bcmptm_ser_imm_flds_free(unit, lt_flds_out.field, SER_CONTROLt_FIELD_COUNT);
    }
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Fail to sync IMM SER_CONTROLt data to cache\n")));
    }
    SHR_FUNC_EXIT();
}

