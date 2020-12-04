/*! \file ser_lt_ser_injection_status.c
 *
 * Functions used to get or update IMM LT SER_INJECTION_STATUS
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcmimm/bcmimm_int_comp.h>
#include <bsl/bsl.h>
#include <shr/shr_debug.h>
#include <bcmltd/chip/bcmltd_id.h>

#include "ser_lts.h"
#include "ser_lt_ser_injection_status.h"

/******************************************************************************
 * Local definitions
 */
#define  BSL_LOG_MODULE  BSL_LS_BCMPTM_SER

/* Cache for IMM LT SER_INJECTION_STATUS */
static bcmptm_ser_lt_field_data_t
    lt_ser_injection_status_cache[SER_INJECTION_STATUSt_FIELD_COUNT];

/******************************************************************************
 * Public Functions
 */
/* Update cache of LT SER_INJECTION_STATUS */
int
bcmptm_ser_injection_status_cache_update(int unit, uint32_t fid_lt, uint32_t data)
{
    bcmptm_ser_lt_field_data_t *cache_ptr = &lt_ser_injection_status_cache[0];

    cache_ptr[fid_lt].fld_data[unit] = data;
    return SHR_E_NONE;
}

int
bcmptm_ser_injection_status_imm_update(int unit, int insert)
{
    shr_fmm_t** fld_array = NULL;
    int rv = SHR_E_NONE;
    size_t j, actual_fld_num, field_info_size;
    uint32_t fld_num = SER_INJECTION_STATUSt_FIELD_COUNT, lt_fid;
    bcmltd_fields_t lt_flds;
    bcmptm_ser_lt_field_data_t *cache_ptr = &lt_ser_injection_status_cache[0];
    bcmlrd_client_field_info_t *fields_info = NULL;

    SHR_FUNC_ENTER(unit);

    fld_array = bcmptm_ser_imm_flds_allocate(unit, fld_num);
    SHR_NULL_CHECK(fld_array, SHR_E_MEMORY);

    lt_flds.field = fld_array;
    lt_flds.count = fld_num;
    if (insert) {
        field_info_size = sizeof(bcmlrd_client_field_info_t) * fld_num;
        fields_info = sal_alloc(field_info_size , "bcmptmSerInjectStFldInfo");
        SHR_NULL_CHECK(fields_info, SHR_E_MEMORY);

        sal_memset(fields_info, 0, field_info_size);
        rv = bcmlrd_table_fields_def_get(unit, "SER_INJECTION_STATUS",
                                         (size_t)fld_num, fields_info,
                                         &actual_fld_num);
        SHR_IF_ERR_EXIT(rv);
    }
    for (lt_fid = 0; lt_fid < fld_num; lt_fid++) {
        /* first time to access SER_INJECTION_STATUSt */
        if (insert) {
            for (j = 0; j < actual_fld_num; j++) {
                if (fields_info[j].id == lt_fid) {
                    break;
                }
            }
            if (j == actual_fld_num) {
                SHR_IF_ERR_EXIT(SHR_E_NOT_FOUND);
            }
            BCMPTM_SER_FLD_DEF_VAL_GET(fields_info[j],
                                       cache_ptr[lt_fid].fld_data[unit]);
        }
        bcmptm_ser_imm_fld_set(lt_flds, lt_fid, lt_fid,
                               0, cache_ptr[lt_fid].fld_data[unit]);
    }
    if (insert == 0) {
        rv = bcmimm_entry_update(unit, true, SER_INJECTION_STATUSt, &lt_flds);
    } else {
        rv = bcmimm_entry_insert(unit, SER_INJECTION_STATUSt, &lt_flds);
    }
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
                              "Fail to commit data to SER_INJECTION_STATUSt\n")));
    }
    SHR_FUNC_EXIT();
}

