/*! \file ser_lt_ser_log_status.c
 *
 * NGSDK
 *
 * Functions used to get data from SER_LOG_STATUS IMM LT
 * or update data to SER_LOG_STATUS IMM LT
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcmimm/bcmimm_int_comp.h>
#include <shr/shr_debug.h>
#include <bsl/bsl.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <sal/sal_libc.h>
#include <bcmcfg/bcmcfg_lt.h>
#include <bcmptm/bcmptm_ser_internal.h>

#include "ser_lt_ser_log_status.h"
#include "ser_lts.h"
#include "ser_lt_ser_log.h"

/******************************************************************************
 * Local definitions
 */

#define  BSL_LOG_MODULE  BSL_LS_BCMPTM_SER

/* Current index of LT SER_LOG */
static uint16_t ser_log_next_id[BCMDRD_CONFIG_MAX_UNITS];

/* Depth of SER_LOG configured by the application */
static uint16_t ser_log_depth_u16[BCMDRD_CONFIG_MAX_UNITS];

/******************************************************************************
 * Private Functions
 */
/* Update cache data to IMM SER_LOG_STATUS. */
static int
bcmptm_ser_log_status_imm_update(int unit, uint16_t cur_index)
{
    int rv = SHR_E_NONE;
    shr_fmm_t** fld_array = NULL;
    size_t fld_num = 0;
    bcmltd_fields_t lt_flds;

    SHR_FUNC_ENTER(unit);

    fld_num = SER_LOG_STATUSt_FIELD_COUNT;
    fld_array = bcmptm_ser_imm_flds_allocate(unit, fld_num);
    SHR_NULL_CHECK(fld_array, SHR_E_MEMORY);

    lt_flds.field = fld_array;
    lt_flds.count = fld_num;

    bcmptm_ser_imm_fld_set(lt_flds, 0, SER_LOG_STATUSt_SER_LOG_IDf, 0,
                           cur_index);

    rv = bcmimm_entry_update(unit, true, SER_LOG_STATUSt, &lt_flds);
    SHR_IF_ERR_EXIT(rv);

exit:
    if (fld_array != NULL) {
        bcmptm_ser_imm_flds_free(unit, fld_array, fld_num);
    }
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Fail to update data of SER_LOG_STATUS to IMM data base\n")));
    }
    SHR_FUNC_EXIT();
}


/******************************************************************************
 * Public Functions
 */
void
bcmptm_ser_log_id_incr(int unit, bool update_imm, bool log_en)
{
    uint16_t cur_index;

    cur_index = ser_log_next_id[unit];
    if (ser_log_next_id[unit] < (ser_log_depth_u16[unit] - 1)) {
        ser_log_next_id[unit]++;
    } else {
        ser_log_next_id[unit] = 0;
        if (log_en) {
            bcmptm_ser_log_overlaid_set(unit, TRUE);
        }
    }
    if (update_imm && log_en) {
        (void)bcmptm_ser_log_status_imm_update(unit, cur_index);
    }
}

uint32_t
bcmptm_ser_log_id_get(int unit)
{
    return ser_log_next_id[unit];
}

bool
bcmptm_ser_log_is_full(int unit)
{
    return bcmptm_ser_log_overlaid_get(unit);
}

int
bcmptm_ser_log_status_imm_init(int unit)
{
    shr_fmm_t** fld_array = NULL;
    int rv = SHR_E_NONE;
    bcmltd_fields_t lt_flds;
    uint32_t fld_num = SER_LOG_STATUSt_FIELD_COUNT, lt_fid, fld_data;
    bcmlrd_client_field_info_t *fields_info = NULL;
    size_t j, actual_fld_num, field_info_size;

    SHR_FUNC_ENTER(unit);

    ser_log_depth_u16[unit] = bcmptm_ser_log_depth_get(unit);

    fld_array = bcmptm_ser_imm_flds_allocate(unit, fld_num);
    SHR_NULL_CHECK(fld_array, SHR_E_MEMORY);

    lt_flds.field = fld_array;
    lt_flds.count = fld_num;

    field_info_size = sizeof(bcmlrd_client_field_info_t) * fld_num;
    fields_info = sal_alloc(field_info_size, "bcmptmSerLogStatusFldInfo");
    SHR_NULL_CHECK(fields_info, SHR_E_MEMORY);

    sal_memset(fields_info, 0, field_info_size);
    rv = bcmlrd_table_fields_def_get(unit, "SER_LOG_STATUS",
                                     (size_t)fld_num, fields_info,
                                     &actual_fld_num);
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
        BCMPTM_SER_FLD_DEF_VAL_GET(fields_info[j], fld_data);

        if (lt_fid == SER_LOG_STATUSt_SER_LOG_IDf) {
            ser_log_next_id[unit] = fld_data;
        }
        bcmptm_ser_imm_fld_set(lt_flds, lt_fid, lt_fid, 0, fld_data);
    }
    rv = bcmimm_entry_insert(unit, SER_LOG_STATUSt, &lt_flds);
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
                              "Fail to commit data to SER_LOG_STATUSt\n")));
    }
    SHR_FUNC_EXIT();
}

int
bcmptm_ser_log_status_cache_sync(int unit)
{
    shr_fmm_t** fld_array = NULL;
    int rv = SHR_E_NONE;
    size_t i = 0;
    bcmltd_fields_t lt_flds_in, lt_flds_out, ser_log_flds_out;
    uint32_t ser_log_fld_num = SER_LOGt_FIELD_COUNT + BCMDRD_MAX_PT_WSIZE - 1;

    SHR_FUNC_ENTER(unit);

    ser_log_depth_u16[unit] = bcmptm_ser_log_depth_get(unit);

    lt_flds_out.field = NULL;
    ser_log_flds_out.field = NULL;

    lt_flds_in.count = 0;
    lt_flds_in.field = NULL;

    fld_array = bcmptm_ser_imm_flds_allocate(unit, SER_LOG_STATUSt_FIELD_COUNT);
    SHR_NULL_CHECK(fld_array, SHR_E_MEMORY);

    lt_flds_out.count = SER_LOG_STATUSt_FIELD_COUNT;
    lt_flds_out.field = fld_array;

    rv = bcmimm_entry_lookup(unit, SER_LOG_STATUSt, &lt_flds_in, &lt_flds_out);
    SHR_IF_ERR_EXIT(rv);

    for (i = 0; i < SER_LOG_STATUSt_FIELD_COUNT; i++) {
        if (lt_flds_out.field[i]->id == SER_LOG_STATUSt_SER_LOG_IDf) {
            ser_log_next_id[unit] = lt_flds_out.field[i]->data;
        } else {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "Don't know FID[%d]\n"),
                       lt_flds_out.field[i]->id));
        }
    }
    if (ser_log_next_id[unit] == 0) {
        fld_array = bcmptm_ser_imm_flds_allocate(unit, ser_log_fld_num);
        SHR_NULL_CHECK(fld_array, SHR_E_MEMORY);

        ser_log_flds_out.count = ser_log_fld_num;
        ser_log_flds_out.field = fld_array;
        rv = bcmimm_entry_get_first(unit, SER_LOGt, &ser_log_flds_out);
        if (SHR_SUCCESS(rv)) {
            bcmptm_ser_log_id_incr(unit, 0, 1);
        } else {
            rv = SHR_E_NONE;
            SHR_ERR_EXIT(rv);
        }
    } else {
        bcmptm_ser_log_id_incr(unit, 0, 1);
    }

exit:
    if (lt_flds_out.field != NULL) {
        /* lt_flds_out.count may be modified by bcmimm_entry_lookup */
        bcmptm_ser_imm_flds_free(unit, lt_flds_out.field, SER_LOG_STATUSt_FIELD_COUNT);
    }
    if (ser_log_flds_out.field != NULL) {
        bcmptm_ser_imm_flds_free(unit, ser_log_flds_out.field, ser_log_fld_num);
    }
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Fail to sync IMM SER_LOG_STATUSt data to cache\n")));
    }
    SHR_FUNC_EXIT();
}

