/*! \file ser_lt_ser_log.c
 *
 * NGSDK
 *
 * Functions used to get data from SER_LOG IMM LT
 * or update data to SER_LOG IMM LT
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcmimm/bcmimm_int_comp.h>
#include <shr/shr_debug.h>
#include <bsl/bsl.h>
#include <bcmptm/bcmptm_ser_cth.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <sal/sal_libc.h>
#include <bcmcfg/bcmcfg_lt.h>
#include <bcmptm/bcmptm_ser_testutil.h>
#include <bcmptm/bcmptm_ser_chip_internal.h>
#include <sal/sal_mutex.h>

#include "ser_lts.h"
#include "ser_lt_ser_log.h"
#include "ser_lt_ser_control.h"
#include "ser_lt_ser_log_status.h"

/******************************************************************************
 * Local definitions
 */
#define  BSL_LOG_MODULE  BSL_LS_BCMPTM_SER

/* Field number of SER_LOG */
#define  SER_LOG_FIELD_NUM    SER_LOGt_FIELD_COUNT

/* Default depth of SER_LOG */
#define  SER_LOG_DEFAULT_DEPTH   10

/* Cache the latest log information */
static bcmptm_ser_log_flds_t *latest_ser_log_cache[BCMDRD_CONFIG_MAX_UNITS];

/* Use to protect latest_ser_log_cache */
static sal_mutex_t lt_ser_log_mutex[BCMDRD_CONFIG_MAX_UNITS];

/* Depth of SER_LOG configured by the application */
static uint32_t ser_log_depth[BCMDRD_CONFIG_MAX_UNITS];

/* Depth of SER_LOG */
#define  SER_LOG_ENTRY_DEPTH(_unit)  ser_log_depth[_unit]

/* Increase index of LT SER_LOG */
#define  SER_LOG_ID_INC(_unit, _logging) bcmptm_ser_log_id_incr(_unit, 1, _logging)

/* Index of LT SER_LOG */
#define  SER_LOG_INDEX(_unit) (bcmptm_ser_log_id_get(_unit))

/* LT SER_LOG is full */
#define  SER_LOG_IS_FULL(_unit) (bcmptm_ser_log_is_full(_unit))

/* Invalid PT ID */
static bcmdrd_sid_t ser_invalid_pt_id[BCMDRD_CONFIG_MAX_UNITS];

/******************************************************************************
 * Private Functions
 */
/* Get value of 'INVALID_PT' defined by LT */
static bcmdrd_sid_t
bcmptm_ser_log_invalid_pt_id_get(int unit)
{
    return ser_invalid_pt_id[unit];
}

/* Move SER log from record index to log index */
static void
bcmptm_ser_log_move(int unit)
{
    uint32_t record_log_index = SER_LOG_ENTRY_DEPTH(unit);
    uint32_t log_index = SER_LOG_INDEX(unit);
    bcmptm_ser_log_flds_t *log_info = NULL, *record_log_info = NULL;

    record_log_info = &latest_ser_log_cache[unit][record_log_index];
    log_info = &latest_ser_log_cache[unit][log_index];
    sal_memcpy(log_info, record_log_info, sizeof(bcmptm_ser_log_flds_t));
}

/* Prepare to sync cache data of SER_LOG to IMM */
static void
bcmptm_ser_log_fields_set(int unit, bcmltd_fields_t lt_flds, int log_index)
{
    uint32_t fld_id = 0, index = 0, i = 0;
    int ser_blk_type = 1;
    bcmptm_ser_log_flds_t *log_info = &latest_ser_log_cache[unit][log_index];

    for (fld_id = 0; fld_id < SER_LOG_FIELD_NUM; fld_id++) {
        switch(fld_id) {
            /* key */
            case SER_LOGt_SER_LOG_IDf:
                bcmptm_ser_imm_fld_set(lt_flds, index++, fld_id, 0,
                                       log_index);
                break;
            /* data fields */
            case SER_LOGt_TIMESTAMPf:
                bcmptm_ser_imm_fld_set(lt_flds, index++, fld_id, 0,
                                       (uint32_t)log_info->timestamp);
                break;
            case SER_LOGt_PT_IDf:
                if (!bcmdrd_pt_is_valid(unit, log_info->pt_id)) {
                    /* INVALID_PT */
                    log_info->pt_id = bcmptm_ser_log_invalid_pt_id_get(unit);
                }
                bcmptm_ser_imm_fld_set(lt_flds, index++, fld_id, 0,
                                       log_info->pt_id);
                break;
            case SER_LOGt_PT_INSTANCEf:
                bcmptm_ser_imm_fld_set(lt_flds, index++, fld_id, 0,
                                       log_info->pt_instance);
                break;
            case SER_LOGt_PT_INDEXf:
                bcmptm_ser_imm_fld_set(lt_flds, index++, fld_id, 0,
                                       log_info->pt_index);
                break;
            case SER_LOGt_SER_ERR_TYPEf:
                bcmptm_ser_imm_fld_set(lt_flds, index++, fld_id, 0,
                                       log_info->ser_err_type);
                break;
            case SER_LOGt_SER_ERR_CORRECTEDf:
                bcmptm_ser_imm_fld_set(lt_flds, index++, fld_id, 0,
                                       log_info->ser_err_corrected);
                break;
            case SER_LOGt_SER_RECOVERY_TYPEf:
                bcmptm_ser_imm_fld_set(lt_flds, index++, fld_id, 0,
                                       log_info->ser_recovery_type);
                break;
            case SER_LOGt_SER_INSTRUCTION_TYPEf:
                bcmptm_ser_imm_fld_set(lt_flds, index++, fld_id, 0,
                                       log_info->ser_instruction_type);
                break;
            case SER_LOGt_BLK_TYPEf:
                (void)bcmptm_ser_blk_type_map(unit, log_info->blk_type,
                                              NULL, &ser_blk_type);
                bcmptm_ser_imm_fld_set(lt_flds, index++, fld_id, 0,
                                       ser_blk_type);
                break;
            case SER_LOGt_HW_FAULTf:
                bcmptm_ser_imm_fld_set(lt_flds, index++, fld_id, 0,
                                       log_info->hw_fault);
                break;
            case SER_LOGt_HIGH_SEVERITY_ERRf:
                bcmptm_ser_imm_fld_set(lt_flds, index++, fld_id, 0,
                                       log_info->high_severity_err);
                break;
            case SER_LOGt_ERR_ENTRY_CONTENTf:
                for (i = 0; i < BCMDRD_MAX_PT_WSIZE; i++) {
                    bcmptm_ser_imm_fld_set(lt_flds, index++, fld_id, i,
                                           log_info->err_entry_content[i]);
                }
                break;
        }
    }
}

/******************************************************************************
 * Public Functions
 */
uint32_t
bcmptm_ser_log_depth_get(int unit)
{
    uint32_t  ser_log_length = SER_LOG_DEFAULT_DEPTH;
    uint64_t fld_val;
    int rv = SHR_E_NONE;

    rv = bcmcfg_field_get(unit, SER_CONFIGt, SER_CONFIGt_SER_LOG_DEPTHf, &fld_val);
    if(SHR_SUCCESS(rv)) {
        ser_log_length = (uint32_t)fld_val;
    }
    ser_log_length = (ser_log_length == 0) ? SER_LOG_DEFAULT_DEPTH : ser_log_length;
    return ser_log_length;
}

bcmptm_ser_log_flds_t *
bcmptm_ser_log_cache_get(int unit)
{
    uint32_t record_log_index = SER_LOG_ENTRY_DEPTH(unit);

    return &latest_ser_log_cache[unit][record_log_index];
}

int
bcmptm_ser_log_imm_update(int unit)
{
    int rv = SHR_E_NONE;
    shr_fmm_t** fld_array = NULL;
    size_t fld_num = 0;
    bcmltd_fields_t lt_flds;

    SHR_FUNC_ENTER(unit);

    (void)sal_mutex_take(lt_ser_log_mutex[unit], SAL_MUTEX_FOREVER);

    if (bcmptm_ser_event_squash(unit)) {
        /* Squash the same SER event, exit */
        rv = SHR_E_NONE;
        SHR_ERR_EXIT(rv);
    }
    bcmptm_ser_log_move(unit);

    fld_num = SER_LOG_FIELD_NUM + BCMDRD_MAX_PT_WSIZE - 1;
    fld_array = bcmptm_ser_imm_flds_allocate(unit, fld_num);
    SHR_NULL_CHECK(fld_array, SHR_E_MEMORY);

    lt_flds.field = fld_array;
    lt_flds.count = fld_num;
    bcmptm_ser_log_fields_set(unit, lt_flds, SER_LOG_INDEX(unit));

    if (bcmptm_ser_control_field_val_get(unit, SER_CONTROLt_SER_LOGGINGf) == 0) {
        /* Increase index of array fields */
        SER_LOG_ID_INC(unit, 0);
        rv = SHR_E_NONE;
        /* exit */
        SHR_ERR_EXIT(rv);
    }

    if(SER_LOG_IS_FULL(unit)) {
        rv = bcmimm_entry_update(unit, true, SER_LOGt, &lt_flds);
    } else {
        /* Init SER_LOG */
        rv = bcmimm_entry_insert(unit, SER_LOGt, &lt_flds);
    }
    SHR_IF_ERR_EXIT(rv);

    SER_LOG_ID_INC(unit, 1);

exit:
    (void)sal_mutex_give(lt_ser_log_mutex[unit]);

    if (fld_array != NULL) {
        bcmptm_ser_imm_flds_free(unit, fld_array, fld_num);
    }
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Fail to update data of SER_LOG to IMM data base\n")));
    }
    SHR_FUNC_EXIT();
}

bcmdrd_sid_t
bcmptm_ser_log_cache_pt_id_get(int unit)
{
    uint32_t record_log_index = SER_LOG_ENTRY_DEPTH(unit);

    return latest_ser_log_cache[unit][record_log_index].pt_id;
}

void
bcmptm_ser_log_cache_allocate(int unit)
{
    size_t log_cache_size = 0;

    ser_log_depth[unit] = bcmptm_ser_log_depth_get(unit);
    log_cache_size = sizeof(bcmptm_ser_log_flds_t) * (ser_log_depth[unit] + 1);
    bcmptm_ser_log_cache_free(unit);
    latest_ser_log_cache[unit] = sal_alloc(log_cache_size, "bcmptmSerLog");
    sal_memset(latest_ser_log_cache[unit], 0, log_cache_size);

    lt_ser_log_mutex[unit] = sal_mutex_create("SerLogCache");
}

void
bcmptm_ser_log_cache_free(int unit)
{
    if (latest_ser_log_cache[unit] != NULL) {
        sal_free(latest_ser_log_cache[unit]);
        latest_ser_log_cache[unit] = NULL;
    }
    if (lt_ser_log_mutex[unit] != NULL) {
        sal_mutex_destroy(lt_ser_log_mutex[unit]);
        lt_ser_log_mutex[unit] = NULL;
    }
}

int
bcmptm_ser_log_cache_clear(int unit, ser_control_cache_update_f updating_cb,
                           uint32_t data)
{
    size_t log_cache_size = 0;
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    (void)sal_mutex_take(lt_ser_log_mutex[unit], SAL_MUTEX_FOREVER);

    if (data) {
        rv = bcmptm_ser_log_status_cache_sync(unit);
        SHR_IF_ERR_EXIT(rv);

        log_cache_size = sizeof(bcmptm_ser_log_flds_t) * ser_log_depth[unit];
        if (latest_ser_log_cache[unit] != NULL) {
            sal_memset(latest_ser_log_cache[unit], 0, log_cache_size);
        }
    }
    /* Update cache of SER_CONTROL */
    updating_cb(unit, SER_CONTROLt_SER_LOGGINGf, data);

exit:
    (void)sal_mutex_give(lt_ser_log_mutex[unit]);

    SHR_FUNC_EXIT();
}

bool
bcmptm_ser_event_squash(int unit)
{
    uint32_t record_log_index = SER_LOG_ENTRY_DEPTH(unit);
    bcmptm_ser_log_flds_t *record_log_info = NULL, *log_info_ptr = NULL;
    sal_usecs_t distance = 0, internal;
    int log_index = SER_LOG_INDEX(unit), i;
    /* ms */
    internal = bcmptm_ser_control_field_val_get
        (unit, SER_CONTROLt_SQUASH_DUPLICATED_SER_EVENT_INTERVALf);
    /* us */
    internal *= 1000;

    /* Circle buffer, from latest log to oldest log */
    record_log_info = &latest_ser_log_cache[unit][record_log_index];
    i = log_index - 1;
    for ( ; i >= 0 ; i--) {
        log_info_ptr = &latest_ser_log_cache[unit][i];
        if (log_info_ptr->timestamp == 0) {
            /* NULL */
            return FALSE;
        }
        distance = (record_log_info->timestamp > log_info_ptr->timestamp) ?
            (record_log_info->timestamp - log_info_ptr->timestamp) :
            ((sal_usecs_t)(-1) - log_info_ptr->timestamp + record_log_info->timestamp);
        if (distance > internal) {
            return FALSE;
        }
        if (log_info_ptr->pt_id == record_log_info->pt_id &&
            log_info_ptr->pt_index == record_log_info->pt_index &&
            log_info_ptr->pt_instance == record_log_info->pt_instance &&
            log_info_ptr->ser_err_type == record_log_info->ser_err_type &&
            log_info_ptr->high_severity_err == record_log_info->high_severity_err &&
            log_info_ptr->hw_fault == record_log_info->hw_fault) {
            return TRUE;
        }
    }
    i = SER_LOG_ENTRY_DEPTH(unit) - 1;
    for ( ; i >= log_index ; i--) {
        log_info_ptr = &latest_ser_log_cache[unit][i];
        if (log_info_ptr->timestamp == 0) {
            /* NULL */
            return FALSE;
        }
        distance = (record_log_info->timestamp > log_info_ptr->timestamp) ?
            (record_log_info->timestamp - log_info_ptr->timestamp) :
            ((sal_usecs_t)(-1) - log_info_ptr->timestamp + record_log_info->timestamp);
        if (distance > internal) {
            return FALSE;
        }
        if (log_info_ptr->pt_id == record_log_info->pt_id &&
            log_info_ptr->pt_index == record_log_info->pt_index &&
            log_info_ptr->pt_instance == record_log_info->pt_instance &&
            log_info_ptr->ser_err_type == record_log_info->ser_err_type &&
            log_info_ptr->high_severity_err == record_log_info->high_severity_err &&
            log_info_ptr->hw_fault == record_log_info->hw_fault) {
            return TRUE;
        }
    }
    return FALSE;
}

void
bcmptm_ser_log_invalid_pt_id_set(int unit, bcmdrd_sid_t sid)
{
    ser_invalid_pt_id[unit] = sid;
}

