/*! \file ser_lt_ser_stats.c
 *
 * Functions used to get data from SER_STATS IMM LT
 * or update data to SER_STATS IMM LT
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
#include <sal/sal_libc.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmptm/bcmptm_ptcache_internal.h>
#include <bcmlrd/bcmlrd_client.h>
#include <bcmptm/bcmptm_ser_chip_internal.h>

#include "ser_lts.h"
#include "ser_lt_ser_stats.h"
#include "ser_lt_ser_control.h"
#include "ser_lt_ser_injection_status.h"
#include "ser_lt_ser_log.h"

/******************************************************************************
 * Local definitions
 */
#define  BSL_LOG_MODULE  BSL_LS_BCMPTM_SER
#define ACC_FOR_SER TRUE

/* Info of one counter */
typedef struct ser_stats_cache_info_s {
    /* Counter is updated by SERC */
    bool     updated;
    /* value of Counter  */
    uint32_t value;
} bcmptm_ser_stats_cache_t;

/* Cache of LT SER_STATS bcmptm_ser_stats_cache[unit][blk_type][recovery_type][fid]*/
static bcmptm_ser_stats_cache_t *bcmptm_ser_stats_cache[BCMDRD_CONFIG_MAX_UNITS];

/*!
 * \brief SER LTs In-memory event callback structure.
 *
 * This structure contains callback functions that will be corresponding
 * to SER_STATSt logical table for entry validation.
 */
static int
bcmptm_ser_stats_imm_validation_callback(int unit, bcmltd_sid_t ltid,
                                         bcmimm_entry_event_t event_reason,
                                         const bcmltd_field_t *key,
                                         const bcmltd_field_t *data,
                                         void *context);

/*!
 * \brief SER LTs In-memory event callback structure.
 *
 * This structure contains callback functions that will be corresponding
 * to SER_STATSt logical table entry commit stages.
 */
static int
bcmptm_ser_stats_imm_stage_callback(int unit, bcmltd_sid_t ltid,
                                    uint32_t trans_id,
                                    bcmimm_entry_event_t event_reason,
                                    const bcmltd_field_t *key,
                                    const bcmltd_field_t *data,
                                    void *context,
                                    bcmltd_fields_t *output_fields);

static bcmimm_lt_cb_t bcmptm_ser_stats_imm_callback = {

    /*! Validate function. */
    .validate = bcmptm_ser_stats_imm_validation_callback,

    /*! Staging function. */
    .stage = bcmptm_ser_stats_imm_stage_callback,

    /*! Commit function. */
    .commit = NULL,

    /*! Abort function. */
    .abort = NULL
};

/* Number of block type defined in map file of LT SER_STATS */
static uint32_t blk_type_num[BCMDRD_CONFIG_MAX_UNITS];

/* Number of recovery type defined in map file of LT SER_STATS */
static uint32_t recovery_type_num[BCMDRD_CONFIG_MAX_UNITS];

/* Number of block type defined in map file of LT SER_STATS */
#define BCMPTM_SER_BLK_NUM(_unit) blk_type_num[_unit]

/* Number of recovery type defined in map file of LT SER_STATS */
#define SER_RECOVERY_TYPE_NUM(_unit) recovery_type_num[_unit]

/* Field number of LT SER_STATS */
#define LT_SER_STATS_FIELD_NUM  SER_STATSt_FIELD_COUNT

/* Key Field number of LT SER_STATS */
#define LT_SER_STATS_KEY_FIELD_NUM 2

/* DATA Field number of LT SER_STATS */
#define LT_SER_STATS_DATA_FIELD_NUM (LT_SER_STATS_FIELD_NUM - LT_SER_STATS_KEY_FIELD_NUM)

/*
 * Get index of bcmptm_ser_stats_cache[unit] according to block type,
 * recovery type and data field ID.
 */
#define SER_STATS_CACHE_INDEX_MAP(_unit, _blk_type, _recovery_type, _data_fid) \
    ((_blk_type * SER_RECOVERY_TYPE_NUM(_unit) + _recovery_type) * LT_SER_STATS_DATA_FIELD_NUM + _data_fid)

/*
 * Get block type, recovery type and data field ID according to
 * index of bcmptm_ser_stats_cache[unit].
 */
#define SER_STATS_CACHE_INDEX_UNMAP(_unit, _index, _blk_type, _recovery_type, _data_fid) \
    do { \
        _data_fid = _index % LT_SER_STATS_DATA_FIELD_NUM; \
        _recovery_type = _index / LT_SER_STATS_DATA_FIELD_NUM; \
        _blk_type = _recovery_type / SER_RECOVERY_TYPE_NUM(_unit); \
        _recovery_type = _recovery_type % SER_RECOVERY_TYPE_NUM(_unit); \
    } while(0)

/* Increase one counter of SER_STATS */
#define SER_STATS_CACHE_COUNTER_INCR(_unit, _index) \
    do { \
        bcmptm_ser_stats_cache[_unit][_index].value++; \
        bcmptm_ser_stats_cache[_unit][_index].updated = 1; \
    } while(0)

/* Decrease one counter of SER_STATS */
#define SER_STATS_CACHE_COUNTER_DECR(_unit, _index, _derc_val) \
    do { \
        if (bcmptm_ser_stats_cache[_unit][_index].value < _derc_val) { \
            LOG_WARN(BSL_LOG_MODULE, \
                     (BSL_META_U(_unit, \
                                 "There are issues in LT SER_STATS\n"))); \
            bcmptm_ser_stats_cache[_unit][_index].value = 0; \
        } else { \
            bcmptm_ser_stats_cache[_unit][_index].value -= _derc_val; \
        } \
        bcmptm_ser_stats_cache[_unit][_index].updated = 1; \
    } while(0)

/* Clear one counter of SER_STATS */
#define SER_STATS_CACHE_COUNTER_CLR(_unit, _index, _value) \
    do { \
        _value = bcmptm_ser_stats_cache[_unit][_index].value; \
        bcmptm_ser_stats_cache[_unit][_index].value = 0; \
        bcmptm_ser_stats_cache[_unit][_index].updated = 1; \
    } while(0)

/*
 * Maximum index of bcmptm_ser_stats_cache[unit].
 */
#define SER_STATS_CACHE_MAX_INDEX(_unit) \
    (LT_SER_STATS_DATA_FIELD_NUM * SER_RECOVERY_TYPE_NUM(_unit) * BCMPTM_SER_BLK_NUM(_unit))

/* All block type defined in map file of SER_STATS */
#define   SER_BLK_TYPE_ALL    0

/******************************************************************************
 * Private Functions
 */
/* Increase value of one counter field of LT SER_STATS by SER logic. */
static int
bcmptm_ser_stats_cache_incr(int unit,
                            int ser_blk_type,
                            bcmptm_ser_recovery_type_t recovery_type,
                            uint32_t fid_lt)
{
    int rv = SHR_E_NONE;
    uint32_t data_fid_lt = fid_lt - LT_SER_STATS_KEY_FIELD_NUM;
    uint32_t cache_index = 0;

    SHR_FUNC_ENTER(unit);

    if (bcmptm_ser_stats_cache[unit] == NULL) {
        rv = SHR_E_INIT;
        SHR_IF_ERR_EXIT(rv);
    }
    if (ser_blk_type >= (int)BCMPTM_SER_BLK_NUM(unit) ||
        ser_blk_type == SER_BLK_TYPE_ALL ||
        recovery_type >= SER_RECOVERY_TYPE_NUM(unit)) {
        rv = SHR_E_INTERNAL;
        SHR_IF_ERR_EXIT(rv);
    }

    data_fid_lt = fid_lt - LT_SER_STATS_KEY_FIELD_NUM;
    cache_index = SER_STATS_CACHE_INDEX_MAP
        (unit, ser_blk_type, recovery_type, data_fid_lt);
    SER_STATS_CACHE_COUNTER_INCR(unit, cache_index);

    cache_index = SER_STATS_CACHE_INDEX_MAP
        (unit, SER_BLK_TYPE_ALL, recovery_type, data_fid_lt);
    SER_STATS_CACHE_COUNTER_INCR(unit, cache_index);

    cache_index = SER_STATS_CACHE_INDEX_MAP
        (unit, ser_blk_type, SER_RECOVERY_ALL, data_fid_lt);
    SER_STATS_CACHE_COUNTER_INCR(unit, cache_index);

    cache_index = SER_STATS_CACHE_INDEX_MAP
        (unit, SER_BLK_TYPE_ALL, SER_RECOVERY_ALL, data_fid_lt);
    SER_STATS_CACHE_COUNTER_INCR(unit, cache_index);

exit:
    SHR_FUNC_EXIT();
}

/*
 * Clear value of a non TOTAL_ERR_CNT counter,
 * and then decrease value of TOTAL_ERR_CNT counters.
 */
static void
bcmptm_ser_stats_non_total_fld_clear(int unit,
                                     int ser_blk_type,
                                     bcmptm_ser_recovery_type_t recovery_type,
                                     uint32_t fid_lt)
{
    uint32_t data_fid = 0;
    uint32_t cache_index = 0, i = 0, item_num = 2;
    uint32_t old_value = 0;

    if (fid_lt == SER_STATSt_TOTAL_ERR_CNTf) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Need specific field id (non TOTAL_ERR_CNT)\n")));
        return;
    }
    data_fid = fid_lt - LT_SER_STATS_KEY_FIELD_NUM;

    cache_index = SER_STATS_CACHE_INDEX_MAP
        (unit, ser_blk_type, recovery_type, data_fid);
    SER_STATS_CACHE_COUNTER_CLR(unit, cache_index, old_value);

    item_num = 2;
    if (SER_STATSt_HW_FAULT_CNTf == fid_lt ||
        SER_STATSt_HIGH_SEVERITY_ERR_CNTf == fid_lt) {
        /* Need not to decrease counter SER_STATSt_TOTAL_ERR_CNTf */
        item_num = 1;
    }
    /* Decreare SER_STATSt_TOTAL_ERR_CNTf and 'fid_lt' */
    for (i = 0; i < item_num; i++) {
        if (i == 1) {
            /* Decreare SER_STATSt_TOTAL_ERR_CNTf */
            data_fid = SER_STATSt_TOTAL_ERR_CNTf - LT_SER_STATS_KEY_FIELD_NUM;
            cache_index = SER_STATS_CACHE_INDEX_MAP
                (unit, ser_blk_type, recovery_type, data_fid);
            SER_STATS_CACHE_COUNTER_DECR(unit, cache_index, old_value);
        }
        cache_index = SER_STATS_CACHE_INDEX_MAP
            (unit, SER_BLK_TYPE_ALL, recovery_type, data_fid);
        SER_STATS_CACHE_COUNTER_DECR(unit, cache_index, old_value);

        cache_index = SER_STATS_CACHE_INDEX_MAP
            (unit, ser_blk_type, SER_RECOVERY_ALL, data_fid);
        SER_STATS_CACHE_COUNTER_DECR(unit, cache_index, old_value);

        cache_index = SER_STATS_CACHE_INDEX_MAP
            (unit, SER_BLK_TYPE_ALL, SER_RECOVERY_ALL, data_fid);
        SER_STATS_CACHE_COUNTER_DECR(unit, cache_index, old_value);
    }
}

/*
 * Clear value of one counter, and then
 * decrease value of corresponding counters.
 */
static void
bcmptm_ser_stats_fld_cache_clear(int unit,
                                 int ser_blk_type,
                                 bcmptm_ser_recovery_type_t recovery_type,
                                 uint32_t fid_lt)
{
    uint32_t fid_lt_temp = 0;

    if (ser_blk_type == SER_BLK_TYPE_ALL ||
        recovery_type == SER_RECOVERY_ALL) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Need specific block type and recovery type\n")));
        return;
    }
    if (fid_lt == SER_STATSt_TOTAL_ERR_CNTf) {
        fid_lt_temp = LT_SER_STATS_KEY_FIELD_NUM;
        for (; fid_lt_temp < SER_STATSt_TOTAL_ERR_CNTf; fid_lt_temp++) {
            bcmptm_ser_stats_non_total_fld_clear(unit, ser_blk_type,
                                                 recovery_type, fid_lt_temp);
        }
    } else {
        bcmptm_ser_stats_non_total_fld_clear(unit, ser_blk_type,
                                             recovery_type, fid_lt);
    }
}

/*
 * Clear value of one counter field of LT SER_STATS.
 * And then update cache data to IMM.
 */
static int
bcmptm_ser_stats_clear(int unit,
                       int ser_blk_type,
                       bcmptm_ser_recovery_type_t recovery_type,
                       uint32_t fid_lt)
{
    int rv = SHR_E_NONE;
    bcmptm_ser_recovery_type_t res_type_tmp;
    int blk_type_tmp;

    SHR_FUNC_ENTER(unit);

    if (bcmptm_ser_stats_cache[unit] == NULL) {
        rv = SHR_E_INIT;
        SHR_IF_ERR_EXIT(rv);
    }
    if (ser_blk_type >= (int)BCMPTM_SER_BLK_NUM(unit) ||
        recovery_type >= SER_RECOVERY_TYPE_NUM(unit) ||
        fid_lt >= LT_SER_STATS_FIELD_NUM) {
        rv = SHR_E_INTERNAL;
        SHR_IF_ERR_EXIT(rv);
    }
    if (ser_blk_type == SER_BLK_TYPE_ALL &&
        recovery_type == SER_RECOVERY_ALL) {
        /* all blk types and all recovery types */
        blk_type_tmp = SER_BLK_TYPE_ALL + 1;
        for (; blk_type_tmp < (int)BCMPTM_SER_BLK_NUM(unit); blk_type_tmp++) {
            res_type_tmp = SER_RECOVERY_ALL + 1;
            for (; res_type_tmp < SER_RECOVERY_TYPE_NUM(unit); res_type_tmp++) {
                bcmptm_ser_stats_fld_cache_clear(unit, blk_type_tmp,
                                                 res_type_tmp, fid_lt);
            }
        }
    } else if (ser_blk_type == SER_BLK_TYPE_ALL) {
        /* all blk types */
        blk_type_tmp = SER_BLK_TYPE_ALL + 1;
        for (; blk_type_tmp < (int)BCMPTM_SER_BLK_NUM(unit); blk_type_tmp++) {
            bcmptm_ser_stats_fld_cache_clear(unit, blk_type_tmp,
                                             recovery_type, fid_lt);
        }
    } else if (recovery_type == SER_RECOVERY_ALL) {
        /* all recovery types */
        res_type_tmp = SER_RECOVERY_ALL + 1;
        for (; res_type_tmp < SER_RECOVERY_TYPE_NUM(unit); res_type_tmp++) {
            bcmptm_ser_stats_fld_cache_clear
                (unit, ser_blk_type, res_type_tmp, fid_lt);
        }
    } else {
        bcmptm_ser_stats_fld_cache_clear
            (unit, ser_blk_type, recovery_type, fid_lt);
    }
    /* update data to IMM SER_STATS */
    rv = bcmptm_ser_stats_imm_update(unit, 0);
    SHR_IF_ERR_EXIT(rv);

exit:
    SHR_FUNC_EXIT();
}

/* IMM validation callback routine. */
static int
bcmptm_ser_stats_imm_validation_callback(int unit, bcmltd_sid_t ltid,
                                         bcmimm_entry_event_t event_reason,
                                         const bcmltd_field_t *key,
                                         const bcmltd_field_t *data,
                                         void *context)
{
    const bcmltd_field_t *lt_field;
    uint32_t fval;

    if (!bcmptm_ser_checking_enable(unit)) {
        return SHR_E_DISABLED;
    }
    if (event_reason != BCMIMM_ENTRY_LOOKUP &&
        event_reason != BCMIMM_ENTRY_UPDATE) {
        return SHR_E_UNAVAIL;
    }
    if (event_reason == BCMIMM_ENTRY_LOOKUP) {
        return SHR_E_NONE;
    }
    /* Check data field */
    lt_field = data;
    while (lt_field) {
        fval = lt_field->data;
        if (fval != 0) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "Only support to clear one or more fields\n")));
            return SHR_E_PARAM;
        }
        lt_field = lt_field->next;
    }
    return SHR_E_NONE;
}

/* IMM stage callback routine. */
static int
bcmptm_ser_stats_imm_stage_callback(int unit, bcmltd_sid_t ltid,
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
    int ser_blk_type = SER_BLK_TYPE_ALL;
    bcmptm_ser_recovery_type_t recovery_type = SER_RECOVERY_ALL;
    int rv = SHR_E_NONE;

    if (output_fields) {
        output_fields->count = 0;
    }
    /* Parse key field */
    lt_field = key;
    while (lt_field) {
        lt_fid = lt_field->id;
        fval = lt_field->data;
        if (lt_fid == SER_STATSt_BLK_TYPEf) {
            ser_blk_type = fval;
        } else if (lt_fid == SER_STATSt_RECOVERY_TYPEf) {
            recovery_type = fval;
        } else {
            return SHR_E_PARAM;
        }
        lt_field = lt_field->next;
    }
    /* Parse data field */
    lt_field = data;
    while (lt_field) {
        lt_fid = lt_field->id;
        /* Field value is zero always */
        rv = bcmptm_ser_stats_clear(unit, ser_blk_type, recovery_type, lt_fid);
        if (SHR_FAILURE(rv)) {
            return rv;
        }
        lt_field = lt_field->next;
    }
    return SHR_E_NONE;
}

/* Get symbol count of key fields. */
static int
bcmptm_ser_stats_key_fields_num_get(int unit)
{
    int rv = SHR_E_NONE;
    size_t count = 0;

    /* Only need to do this during initialization */
    rv = bcmlrd_field_symbols_count_get(unit, SER_STATSt,
                                        SER_STATSt_BLK_TYPEf, &count);
    if (SHR_FAILURE(rv)) {
        return rv;
    }
    BCMPTM_SER_BLK_NUM(unit) = (uint32_t)count;

    rv = bcmlrd_field_symbols_count_get(unit, SER_STATSt,
                                        SER_STATSt_RECOVERY_TYPEf, &count);
    if (SHR_FAILURE(rv)) {
        return rv;
    }
    SER_RECOVERY_TYPE_NUM(unit) = (uint32_t)count;
    return SHR_E_NONE;
}

/******************************************************************************
 * Public Functions
 */
int
bcmptm_ser_stats_imm_update(int unit, int insert)
{
    shr_fmm_t** fld_array = NULL;
    int rv = SHR_E_NONE, fid_lt = 0;
    size_t cache_idx = 0;
    bcmltd_fields_t lt_flds;
    bcmptm_ser_stats_cache_t *cache_ptr = bcmptm_ser_stats_cache[unit];
    int ser_blk_type;
    bcmptm_ser_recovery_type_t recovery_type;
    bool flds_updated = 0;

    SHR_FUNC_ENTER(unit);

    if (cache_ptr == NULL) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Data cache of SER_STATSt is not allocated.\n")));
        return SHR_E_INIT;
    }
    fld_array = bcmptm_ser_imm_flds_allocate(unit, LT_SER_STATS_FIELD_NUM);
    if (fld_array == NULL) {
        return SHR_E_MEMORY;
    }
    lt_flds.field = fld_array;
    lt_flds.count = LT_SER_STATS_FIELD_NUM;

    cache_idx = 0;
    while(cache_idx < SER_STATS_CACHE_MAX_INDEX(unit)) {
        SER_STATS_CACHE_INDEX_UNMAP
            (unit, cache_idx, ser_blk_type, recovery_type, fid_lt);
        if (fid_lt != 0) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "Field index should be zero.\n")));
            rv = SHR_E_INTERNAL;
            SHR_IF_ERR_EXIT(rv);
        }
        /* key field */
        bcmptm_ser_imm_fld_set(lt_flds, fid_lt, (uint32_t)fid_lt,
                               0, ser_blk_type);
        fid_lt = 1;
        bcmptm_ser_imm_fld_set(lt_flds, fid_lt, (uint32_t)fid_lt,
                               0, recovery_type);
        /* data field */
        flds_updated = 0;
        fid_lt = LT_SER_STATS_KEY_FIELD_NUM;
        for (; fid_lt < LT_SER_STATS_FIELD_NUM; fid_lt++) {
            bcmptm_ser_imm_fld_set(lt_flds, fid_lt, (uint32_t)fid_lt,
                                   0, cache_ptr[cache_idx].value);
            cache_idx++;
            if (cache_ptr[cache_idx].updated) {
                flds_updated = 1;
                /* can't break */
            }
        }
        /*
         * Cache of bcmptm_ser_stats_cache[unit][blk_type][recovery_type]
         * may not be updated.
         */
        if (flds_updated == 0 && insert == 0) {
            continue;
        }
        if (insert == 0) {
            rv = bcmimm_entry_update(unit, true, SER_STATSt, &lt_flds);
        } else {
            rv = bcmimm_entry_insert(unit, SER_STATSt, &lt_flds);
        }
        SHR_IF_ERR_EXIT(rv);
    }

exit:
    bcmptm_ser_imm_flds_free(unit, fld_array, LT_SER_STATS_FIELD_NUM);
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Fail to commit data to SER_STATSt\n")));
    }
    SHR_FUNC_EXIT();
}

int
bcmptm_ser_stats_cache_sync(int unit)
{
    shr_fmm_t** fld_array = NULL;
    int rv = SHR_E_NONE;
    size_t data_fld_id = 0, cache_idx = 0, cache_old_idx = 0;
    bcmltd_fields_t lt_flds_in, lt_flds_out;
    uint32_t data = 0, i = 0, fid_lt;
    bcmptm_ser_stats_cache_t *cache_ptr = bcmptm_ser_stats_cache[unit];
    int ser_blk_type;
    bcmptm_ser_recovery_type_t recovery_type;

    SHR_FUNC_ENTER(unit);

    lt_flds_in.field = NULL;
    lt_flds_out.field = NULL;
    if (cache_ptr == NULL) {
        rv = SHR_E_INIT;
        SHR_IF_ERR_EXIT(rv);
    }
    /* key fields */
    fld_array = bcmptm_ser_imm_flds_allocate(unit, LT_SER_STATS_KEY_FIELD_NUM);
    SHR_NULL_CHECK(fld_array, SHR_E_MEMORY);
    lt_flds_in.count = LT_SER_STATS_KEY_FIELD_NUM;
    lt_flds_in.field = fld_array;

    /* Data fields */
    fld_array = bcmptm_ser_imm_flds_allocate(unit, LT_SER_STATS_DATA_FIELD_NUM);
    SHR_NULL_CHECK(fld_array, SHR_E_MEMORY);
    lt_flds_out.field = fld_array;
    lt_flds_out.count = LT_SER_STATS_DATA_FIELD_NUM;

    cache_idx = 0;
    while(cache_idx < SER_STATS_CACHE_MAX_INDEX(unit)) {
        SER_STATS_CACHE_INDEX_UNMAP
            (unit, cache_idx, ser_blk_type, recovery_type, fid_lt);
        if (fid_lt != 0) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "Field index should be zero.\n")));
            rv = SHR_E_INTERNAL;
            SHR_IF_ERR_EXIT(rv);
        }
        /* key field */
        bcmptm_ser_imm_fld_set(lt_flds_in, fid_lt, fid_lt,
                               0, ser_blk_type);
        fid_lt = 1;
        bcmptm_ser_imm_fld_set(lt_flds_in, fid_lt, fid_lt,
                               0, recovery_type);
        rv = bcmimm_entry_lookup(unit, SER_STATSt, &lt_flds_in, &lt_flds_out);
        SHR_IF_ERR_EXIT(rv);

        if (lt_flds_out.count != LT_SER_STATS_DATA_FIELD_NUM) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "Number of field of SER_STATS is [%d] <> [%d]\n"),
                       LT_SER_STATS_DATA_FIELD_NUM, (int)(lt_flds_out.count)));
            rv = SHR_E_INTERNAL;
            SHR_IF_ERR_EXIT(rv);
        }
        /* get data fields */
        cache_old_idx = cache_idx;
        for (i = 0; i < LT_SER_STATS_DATA_FIELD_NUM; i++) {
            data_fld_id = lt_flds_out.field[i]->id;
            if (data_fld_id >= LT_SER_STATS_FIELD_NUM ||
                data_fld_id < LT_SER_STATS_KEY_FIELD_NUM) {
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U(unit,
                                      "Field id [%d] of SER_STATS should be not larger than [%d]\n"),
                           (int)data_fld_id, LT_SER_STATS_FIELD_NUM));
                rv = SHR_E_INTERNAL;
                SHR_IF_ERR_EXIT(rv);
            }
            data_fld_id -= LT_SER_STATS_KEY_FIELD_NUM;
            data = lt_flds_out.field[i]->data;
            cache_idx = SER_STATS_CACHE_INDEX_MAP
                (unit, ser_blk_type, recovery_type, data_fld_id);
            cache_ptr[cache_idx].value = data;
            cache_ptr[cache_idx].updated = 0;
        }
        cache_idx = cache_old_idx + LT_SER_STATS_DATA_FIELD_NUM;
    }

exit:
    if (lt_flds_out.field) {
        /* lt_flds_out.count may be modified by bcmimm_entry_lookup */
        bcmptm_ser_imm_flds_free(unit, lt_flds_out.field, LT_SER_STATS_DATA_FIELD_NUM);
    }
    if (lt_flds_in.field) {
        bcmptm_ser_imm_flds_free(unit, lt_flds_in.field, LT_SER_STATS_KEY_FIELD_NUM);
    }
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Fail to sync IMM SER_STATSt data to cache\n")));
    }
    SHR_FUNC_EXIT();
}

void
bcmptm_ser_stats_update(int unit,
                        bcmdrd_sid_t pt_id, uint32_t flags,
                        int blk_type,
                        bcmptm_ser_recovery_type_t recovery_type)
{
    int is_reg = 0, is_cci = 0;
    bcmptm_ptcache_ctr_info_t ctr_info;
    int ser_blk_type = 0;

    SHR_FUNC_ENTER(unit);

    if (bcmptm_ser_event_squash(unit)) {
        return;
    }
    (void)bcmptm_ser_blk_type_map(unit, blk_type, NULL, &ser_blk_type);

    if (flags & BCMPTM_SER_FLAG_HF) {
        SHR_IF_ERR_EXIT(
            bcmptm_ser_stats_cache_incr(unit, ser_blk_type, recovery_type,
                                        SER_STATSt_HW_FAULT_CNTf));
    }
    if (flags & BCMPTM_SER_FLAG_HSF) {
        SHR_IF_ERR_EXIT(
            bcmptm_ser_stats_cache_incr(unit, ser_blk_type, recovery_type,
                                        SER_STATSt_HIGH_SEVERITY_ERR_CNTf));
    }
    if (flags & BCMPTM_SER_FLAG_NON_SBUS_BUS) {
        /* exit */
        SHR_ERR_EXIT(
            bcmptm_ser_stats_cache_incr(unit, ser_blk_type, recovery_type,
                                        SER_STATSt_ECC_PARITY_ERR_INT_BUS_CNTf));
    }
    if (flags & BCMPTM_SER_FLAG_NON_SBUS_MEM) {
        /* exit */
        SHR_ERR_EXIT(
            bcmptm_ser_stats_cache_incr(unit, ser_blk_type, recovery_type,
                                        SER_STATSt_ECC_PARITY_ERR_INT_MEM_CNTf));
    }
    if (bcmdrd_pt_is_valid(unit, pt_id) == 0) {
        /* exit */
        SHR_ERR_EXIT(
            bcmptm_ser_stats_cache_incr(unit, ser_blk_type, recovery_type,
                                        SER_STATSt_ECC_PARITY_ERR_INT_MEM_CNTf));
    }
    /* physical tables */
    is_reg = bcmdrd_pt_is_reg(unit, pt_id);
    ctr_info.pt_is_ctr = 0;
    (void)bcmptm_ptcache_ctr_info_get(unit, ACC_FOR_SER, pt_id, &ctr_info);
    is_cci = ctr_info.pt_is_ctr;

    if (flags & BCMPTM_SER_FLAG_TCAM) {
        SHR_IF_ERR_EXIT(
            bcmptm_ser_stats_cache_incr(unit, ser_blk_type, recovery_type,
                                        SER_STATSt_PARITY_ERR_TCAM_CNTf));
    } else if (flags & BCMPTM_SER_FLAG_PARITY) {
        if (is_cci) {
            SHR_IF_ERR_EXIT(
                bcmptm_ser_stats_cache_incr(unit, ser_blk_type, recovery_type,
                                            SER_STATSt_PARITY_ERR_CTR_CNTf));
        } else if (is_reg) {
            SHR_IF_ERR_EXIT(
                bcmptm_ser_stats_cache_incr(unit, ser_blk_type, recovery_type,
                                            SER_STATSt_PARITY_ERR_REG_CNTf));
        } else {
            SHR_IF_ERR_EXIT(
                bcmptm_ser_stats_cache_incr(unit, ser_blk_type, recovery_type,
                                            SER_STATSt_PARITY_ERR_MEM_CNTf));
        }
    } else if (flags & BCMPTM_SER_FLAG_DOUBLEBIT) {
        if (is_cci) {
            SHR_IF_ERR_EXIT(
                bcmptm_ser_stats_cache_incr(unit, ser_blk_type, recovery_type,
                                            SER_STATSt_ECC_DBE_CTR_CNTf));
        } else if (is_reg) {
            SHR_IF_ERR_EXIT(
                bcmptm_ser_stats_cache_incr(unit, ser_blk_type, recovery_type,
                                            SER_STATSt_ECC_DBE_REG_CNTf));
        } else {
            SHR_IF_ERR_EXIT(
                bcmptm_ser_stats_cache_incr(unit, ser_blk_type, recovery_type,
                                            SER_STATSt_ECC_DBE_MEM_CNTf));
        }
    } else {
        if (is_cci) {
            SHR_IF_ERR_EXIT(
                bcmptm_ser_stats_cache_incr(unit, ser_blk_type, recovery_type,
                                            SER_STATSt_ECC_SBE_CTR_CNTf));
        } else if (is_reg) {
            SHR_IF_ERR_EXIT(
                bcmptm_ser_stats_cache_incr(unit, ser_blk_type, recovery_type,
                                            SER_STATSt_ECC_SBE_REG_CNTf));
        } else {
            SHR_IF_ERR_EXIT(
                bcmptm_ser_stats_cache_incr(unit, ser_blk_type, recovery_type,
                                            SER_STATSt_ECC_SBE_MEM_CNTf));
        }
    }

exit:
    if (SHR_FUNC_ERR()) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Fail to update data to cache of SER_STATSt\n")));
        /* Revert cache data from IMM */
        (void)bcmptm_ser_stats_cache_sync(unit);
    } else {
        (void)bcmptm_ser_stats_cache_incr(unit, ser_blk_type, recovery_type,
                                          SER_STATSt_TOTAL_ERR_CNTf);
        /* update to IMM */
        bcmptm_ser_stats_imm_update(unit, 0);
    }
}

int
bcmptm_ser_stats_cache_allocate(int unit)
{
    size_t cache_size = sizeof(bcmptm_ser_stats_cache_t);
    int rv = SHR_E_NONE;

    rv = bcmptm_ser_stats_key_fields_num_get(unit);
    if (SHR_FAILURE(rv)) {
        return rv;
    }
    cache_size *= SER_STATS_CACHE_MAX_INDEX(unit);

    if (bcmptm_ser_stats_cache[unit] != NULL) {
        return SHR_E_NONE;
    }
    bcmptm_ser_stats_cache[unit] =
        (bcmptm_ser_stats_cache_t *)sal_alloc(cache_size, "bcmptmSerStatsCache");
    if (bcmptm_ser_stats_cache[unit] == NULL) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Fail to allocate memory for"
                              "IMM SER_STATSt to cache data\n")));
        return SHR_E_MEMORY;
    }
    sal_memset(bcmptm_ser_stats_cache[unit], 0, cache_size);
    return SHR_E_NONE;
}

void
bcmptm_ser_stats_cache_destroy(int unit)
{
    bcmptm_ser_stats_cache_t *cache_ptr = bcmptm_ser_stats_cache[unit];

    bcmptm_ser_stats_cache[unit] = NULL;

    if (cache_ptr == NULL) {
        return ;
    }
    sal_free(cache_ptr);
    cache_ptr = NULL;
}

int
bcmptm_ser_stats_callback_register(int unit)
{
    SHR_FUNC_ENTER(unit);
    /* To register callback for SER LTs here. */
    SHR_IF_ERR_EXIT
        (bcmimm_lt_event_reg(unit,
                             SER_STATSt,
                             &bcmptm_ser_stats_imm_callback,
                             NULL));
exit:
    SHR_FUNC_EXIT();
}

