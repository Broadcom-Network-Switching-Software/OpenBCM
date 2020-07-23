/*! \file ser_lt_ser_pt_control.c
 *
 * Functions used to get or update IMM SER_PT_CONTROL LT
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
#include <sal/sal_libc.h>
#include <sal/sal_mutex.h>
#include <bcmptm/bcmptm_ser_cth_internal.h>
#include <bcmptm/bcmptm_ser_chip_internal.h>
#include <bcmptm/bcmptm_ser_internal.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmdrd/bcmdrd_pt_ser.h>
#include <bcmptm/bcmptm_ptcache_internal.h>
#include <bcmptm/bcmptm_chip_internal.h>
#include <bcmptm/bcmptm_internal.h>

#include "ser_lts.h"
#include "ser_lt_ser_pt_control.h"
#include "ser_lt_ser_pt_status.h"

/******************************************************************************
 * Local definitions
 */
#define  BSL_LOG_MODULE  BSL_LS_BCMPTM_SER

/* Bit offset of every data field of LT SER_PT_CONTROL */
#define  BCMPTM_SER_PT_CONTROL_ENTRY_USED_OFFSET      0
#define  BCMPTM_SER_ECC_PARITY_CHECK_BIT_OFFSET      1
#define  BCMPTM_SER_SCAN_MODE_BIT_OFFSET             2
#define  BCMPTM_SER_SCAN_MODE_OPERA_BIT_OFFSET       6
#define  BCMPTM_SER_SINGLE_BIT_REPORT_OFFSET         10
#define  BCMPTM_SER_HSF_SUPPRESSION_OFFSET           11

/* PT_ID field mask */
#define  BCMPTM_SER_SCAN_MODE_MASK         0xF
#define  BCMPTM_SER_SCAN_MODE_OPERA_MASK   0xF

/* Used to save field id and field offset of SER_PT_CONTROLt */
typedef struct lt_ser_control_pt_field_s {
    uint32_t  bit_offset;
    uint32_t  mask;
} bcmptm_ser_pt_control_field_info_t;

/* cache handler for LT SER_PT_CONTROL */
static uint32_t *lt_ser_pt_control_cache[BCMDRD_CONFIG_MAX_UNITS];

/* Used to check whether fid is valid or not */
static const bcmptm_ser_pt_control_field_info_t lt_ser_pt_control_info[] = {
    { BCMPTM_SER_PT_CONTROL_ENTRY_USED_OFFSET, 1},
    { BCMPTM_SER_ECC_PARITY_CHECK_BIT_OFFSET, 1 },
    { BCMPTM_SER_SCAN_MODE_BIT_OFFSET, BCMPTM_SER_SCAN_MODE_MASK },
    { BCMPTM_SER_SCAN_MODE_OPERA_BIT_OFFSET, BCMPTM_SER_SCAN_MODE_OPERA_MASK },
    { BCMPTM_SER_SINGLE_BIT_REPORT_OFFSET, 1 },
    { BCMPTM_SER_HSF_SUPPRESSION_OFFSET, 1 }
};

/*!
 * \brief SER LTs In-memory event callback structure.
 *
 * This structure contains callback functions that will be corresponding
 * to SER_PT_CONTROLt logical table for entry validation.
 */
static int
bcmptm_ser_pt_control_imm_validation_callback(int unit, bcmltd_sid_t ltid,
                                              bcmimm_entry_event_t event_reason,
                                              const bcmltd_field_t *key,
                                              const bcmltd_field_t *data,
                                              void *context);

/*!
 * \brief SER LTs In-memory event callback structure.
 *
 * This structure contains callback functions that will be corresponding
 * to SER_PT_CONTROLt logical table entry commit stages.
 */
static int
bcmptm_ser_pt_control_imm_stage_callback(int unit, bcmltd_sid_t ltid,
                                         uint32_t trans_id,
                                         bcmimm_entry_event_t event_reason,
                                         const bcmltd_field_t *key,
                                         const bcmltd_field_t *data,
                                         void *context,
                                         bcmltd_fields_t *output_fields);

static bcmimm_lt_cb_t bcmptm_ser_pt_control_callback = {

    /*! Validate function. */
    .validate = bcmptm_ser_pt_control_imm_validation_callback,

    /*! Staging function. */
    .stage = bcmptm_ser_pt_control_imm_stage_callback,

    /*! Commit function. */
    .commit = NULL,

    /*! Abort function. */
    .abort = NULL
};

/******************************************************************************
 * Private Functions
 */
/* Update data cache of SER_PT_CONTROL */
static void
bcmptm_ser_pt_control_cache_update(int unit, uint32_t pt_id,
                                   uint32_t fid_lt, uint32_t data)
{
    uint32_t bit_offset = 0, mask = 0;

    bit_offset = lt_ser_pt_control_info[fid_lt].bit_offset;
    mask = lt_ser_pt_control_info[fid_lt].mask;

    lt_ser_pt_control_cache[unit][pt_id] &= ~(mask << bit_offset);
    lt_ser_pt_control_cache[unit][pt_id] |= ((data & mask) << bit_offset);
}

/* Check whether scan mode configured by application is valid or not. */
static bool
bcmptm_ser_pt_scan_mode_check(int unit, uint32_t sid,
                              uint32_t scan_mode, uint32_t *scan_mode_opera)
{
    int rv = SHR_E_NONE;
    bool is_tcam, skip = 0, is_counter, is_write_only;
    bool none_or_clr = 0, is_fifo, ser_check;
    bcmdrd_ser_en_type_t ctrl_type;
    bcmdrd_pt_ser_en_ctrl_t ctrl_info;
    bcmptm_ser_mem_scan_status_t scan_supported = 0;
    bcmdrd_pt_ser_info_t ser_info;

    (void)bcmptm_ser_sram_scan_mem_skip(unit, sid, &skip);
    is_tcam = bcmdrd_pt_attr_is_cam(unit, sid);
    is_counter = bcmdrd_pt_is_counter(unit, sid);
    is_write_only = bcmdrd_pt_is_writeonly(unit, sid);
    rv = bcmdrd_pt_ser_info_get(unit, sid, &ser_info);
    if (ser_info.resp == BCMDRD_SER_RESP_NONE ||
        ser_info.resp == BCMDRD_SER_RESP_ENTRY_CLEAR) {
        none_or_clr = 1;
    }
    is_fifo = bcmdrd_pt_attr_is_fifo(unit, sid);
    ctrl_type = (is_tcam) ? BCMDRD_SER_EN_TYPE_TCAM : BCMDRD_SER_EN_TYPE_EC;
    rv = bcmdrd_pt_ser_en_ctrl_get(unit, ctrl_type, sid, &ctrl_info);
    ser_check = SHR_SUCCESS(rv) ? 1 : 0;
    if (skip || is_counter || none_or_clr || is_fifo || is_write_only) {
        scan_supported = SCAN_DISABLE;
    } else {
        if (ser_check) {
            if (!is_tcam) {
                scan_supported = SCAN_USING_SW_SCAN;
            } else {
                /* TCAM H/W scan engine */
                scan_supported = SCAN_USING_HW_SCAN;
            }
        } else {
            if (!is_tcam) {
                scan_supported = SCAN_DISABLE;
            } else {
                /* CMIC SER engine */
                scan_supported = SCAN_USING_SW_SCAN;
            }
        }
    }
    switch (scan_mode) {
        case DISABLE_SCAN_MODE:
            if (scan_supported == SCAN_USING_HW_SCAN) {
                LOG_WARN(BSL_LOG_MODULE,
                         (BSL_META_U(unit,
                                     "[%s] support HW scan mode,"
                                     "and can't disable HW scan per PT.\n"),
                          bcmdrd_pt_sid_to_name(unit, sid)));
                return FALSE;
            } else {
                *scan_mode_opera = SCAN_DISABLE;
                return TRUE;
            }
        case DEFAULT_SCAN_MODE:
            *scan_mode_opera = scan_supported;
            return TRUE;

        case SW_SCAN_MODE:
            if (scan_supported == SCAN_USING_SW_SCAN) {
                *scan_mode_opera = scan_supported;
                return TRUE;
            } else if (ser_check && (skip || is_counter || none_or_clr)) {
                *scan_mode_opera = scan_supported;
                LOG_INFO(BSL_LOG_MODULE,
                         (BSL_META_U(unit,
                                     "[%s] is not scanned by default.\n"),
                          bcmdrd_pt_sid_to_name(unit, sid)));
                return TRUE;
            } else {
                LOG_WARN(BSL_LOG_MODULE,
                         (BSL_META_U(unit,
                                     "[%s] does not support SW scan mode.\n"),
                          bcmdrd_pt_sid_to_name(unit, sid)));
                return FALSE;
            }
        case HW_SCAN_MODE:
            if (scan_supported == SCAN_USING_HW_SCAN) {
                *scan_mode_opera = scan_supported;
                return TRUE;
            } else {
                LOG_WARN(BSL_LOG_MODULE,
                         (BSL_META_U(unit,
                                     "[%s] does not support HW scan mode.\n"),
                          bcmdrd_pt_sid_to_name(unit, sid)));
                return FALSE;
            }
        default:
            return FALSE;
    }
}

/* IMM validation callback routine. */
static int
bcmptm_ser_pt_control_imm_validation_callback(int unit, bcmltd_sid_t ltid,
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
 * IMM stage callback routine.
 * Enable/disable SER check of a PT, configure scan mode of a PT etc.
 */
static int
bcmptm_ser_pt_control_imm_stage_callback(int unit, bcmltd_sid_t ltid,
                                         uint32_t trans_id,
                                         bcmimm_entry_event_t event_reason,
                                         const bcmltd_field_t *key,
                                         const bcmltd_field_t *data,
                                         void *context,
                                         bcmltd_fields_t *output_fields)
{
    const bcmltd_field_t *lt_field;
    uint32_t lt_fid, pt_id = INVALIDm;
    uint32_t fval;
    int rv = SHR_E_NONE, rv_ser_en = SHR_E_NONE;
    uint32_t ser_enable, ser_enable_update = 0;
    uint32_t ser_1bit_update = 0, ser_1bit_enable = 0;
    uint32_t scan_mode, scan_mode_opera = 0;
    uint32_t mode_valid = 0;
    bcmltd_field_t *ltd_field = NULL;
    int ecc_checking = 0;

    if (output_fields) {
        output_fields->count = 0;
    }
    lt_field = key;
    /* Find PT ID, and scan_mode if has */
    while (lt_field) {
        lt_fid = lt_field->id;
        if (lt_fid == SER_PT_CONTROLt_PT_IDf) {
            pt_id = lt_field->data;
        }
        lt_field = lt_field->next;
    }
    lt_field = data;
    /* Find PT ID, and scan_mode if has */
    while (lt_field) {
        lt_fid = lt_field->id;
        if (lt_fid == SER_PT_CONTROLt_SCAN_MODEf) {
            scan_mode = lt_field->data;
            mode_valid = bcmptm_ser_pt_scan_mode_check(unit, pt_id, scan_mode,
                                                       &scan_mode_opera);
            if (mode_valid == FALSE) {
                return SHR_E_CONFIG;
            }
        } else if (lt_fid == SER_PT_CONTROLt_REPORT_SINGLE_BIT_ECCf) {
            (void)bcmptm_ser_mem_ecc_info_get(unit, pt_id, 0, NULL, NULL,
                                              &ecc_checking);
            if (ecc_checking == 0) {
                LOG_WARN(BSL_LOG_MODULE,
                         (BSL_META_U(unit,
                                     "PT [%s] is protected by parity checking, so"
                                     " can't enable 1bit error reporting for it.\n"),
                                     bcmdrd_pt_sid_to_name(unit, pt_id)));
                return SHR_E_CONFIG;
            }
            ser_1bit_update = 1;
        } else if (lt_fid == SER_PT_CONTROLt_ECC_PARITY_CHECKf) {
            ser_enable_update = 1;
        }
        lt_field = lt_field->next;
    }
    if (mode_valid && output_fields) {
        /* update scan_mode_opera */
        ltd_field = *(output_fields->field);
        sal_memset(ltd_field, 0, sizeof(bcmltd_field_t));
        ltd_field[0].id = SER_PT_CONTROLt_SCAN_MODE_OPERf;
        ltd_field[0].data = scan_mode_opera;
        output_fields->count = 1;
    }
    /* Update cache */
    if (mode_valid) {
        bcmptm_ser_pt_control_cache_update
            (unit, pt_id, SER_PT_CONTROLt_SCAN_MODE_OPERf, scan_mode_opera);
    }
    /* Parse data field */
    lt_field = data;
    while (lt_field) {
        lt_fid = lt_field->id;
        fval = lt_field->data;
        bcmptm_ser_pt_control_cache_update(unit, pt_id, lt_fid, fval);
        lt_field = lt_field->next;
    }

    if (ser_enable_update) {
        /* Get new value */
        ser_enable = bcmptm_ser_pt_control_field_val_get
            (unit, pt_id, SER_PT_CONTROLt_ECC_PARITY_CHECKf);
        /* H/W write, disable or re-enable SER checking for one PT */
        if (!bcmdrd_pt_attr_is_cam(unit,  pt_id)) {
            rv_ser_en = bcmptm_ser_pt_ser_enable
                (unit, pt_id, BCMDRD_SER_EN_TYPE_EC, ser_enable);
        } else {
            rv_ser_en = bcmptm_ser_tcam_pt_ser_enable(unit, pt_id, ser_enable);
        }
    }
    if (ser_1bit_update) {
        ser_1bit_enable = bcmptm_ser_pt_control_field_val_get
            (unit, pt_id, SER_PT_CONTROLt_REPORT_SINGLE_BIT_ECCf);
        rv = bcmptm_ser_pt_ser_enable
            (unit, pt_id, BCMDRD_SER_EN_TYPE_ECC1B, ser_1bit_enable);
    }
    if (SHR_FAILURE(rv_ser_en)) {
        bcmptm_ser_pt_control_cache_update
            (unit, pt_id, SER_PT_CONTROLt_ECC_PARITY_CHECKf, 0);
        return rv_ser_en;
    }
    return rv;
}

/* Insert an entry to SER_PT_CONTROL for every PT. */
static int
bcmptm_ser_pt_control_insert(int unit, bcmdrd_sid_t sid,
                             bcmptm_ser_lt_flds_update_t *fld_info_array,
                             uint32_t num)
{
    int rv = SHR_E_NONE;
    uint32_t i = 0;
    bcmltd_fields_t lt_flds;

    lt_flds.count = num + 1;
    lt_flds.field = bcmptm_ser_imm_flds_allocate(unit, num + 1);
    if (lt_flds.field == NULL) {
        return SHR_E_MEMORY;
    }
    for (i = 0; i < num; i++) {
        bcmptm_ser_imm_fld_set(lt_flds, i, fld_info_array[i].lt_fid, 0,
                               fld_info_array[i].data);
        bcmptm_ser_pt_control_cache_update(unit, sid,
                                           fld_info_array[i].lt_fid,
                                           fld_info_array[i].data);
    }
    /* key */
    bcmptm_ser_imm_fld_set(lt_flds, i, SER_PT_CONTROLt_PT_IDf, 0, sid);
    bcmptm_ser_pt_control_cache_update(unit, sid, 0, 1);

    rv = bcmimm_entry_insert(unit, SER_PT_CONTROLt, &lt_flds);

    bcmptm_ser_imm_flds_free(unit, lt_flds.field, num + 1);
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Fail to update IMM SER_PT_CONTROL\n")));
    }
    return rv;
}

/******************************************************************************
 * Public Functions
 */
int
bcmptm_ser_pt_control_imm_update(int unit, bcmdrd_sid_t pt_id,
                                 uint32_t fid_lt, uint32_t data)
{
    bcmltd_fields_t lt_flds;
    int rv = SHR_E_NONE;

    if (fid_lt == SER_PT_CONTROLt_PT_IDf) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Can't to update field SER_PT_CONTROLt_PT_IDf\n")));
        return SHR_E_PARAM;
    }
    lt_flds.count = 2;
    lt_flds.field = bcmptm_ser_imm_flds_allocate(unit, 2);
    if (lt_flds.field == NULL) {
        return SHR_E_MEMORY;
    }
    /* key */
    bcmptm_ser_imm_fld_set(lt_flds, 0, SER_PT_CONTROLt_PT_IDf, 0, pt_id);
    /* data */
    bcmptm_ser_imm_fld_set(lt_flds, 1, fid_lt, 0, data);

    rv = bcmimm_entry_update(unit, true, SER_PT_CONTROLt, &lt_flds);

    bcmptm_ser_imm_flds_free(unit, lt_flds.field, 2);

    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Fail to update data of SER_PT_CONTROLt\n")));
        return rv;
    }

    bcmptm_ser_pt_control_cache_update(unit, pt_id, fid_lt, data);

    return SHR_E_NONE;
}

int
bcmptm_ser_pt_control_imm_insert(int unit, bcmdrd_sid_t sid,
                                 int enable, int enable_1bit)
{
    int rv = SHR_E_NONE;
    bcmptm_ser_lt_flds_update_t flds_info[] = {
        {SER_PT_CONTROLt_ECC_PARITY_CHECKf, 1},
        {SER_PT_CONTROLt_REPORT_SINGLE_BIT_ECCf, 1},
        {SER_PT_CONTROLt_SCAN_MODE_OPERf, SCAN_DISABLE},
    };
    uint32_t scan_mode_opera = 0;

    SHR_FUNC_ENTER(unit);

    flds_info[0].data = enable;
    flds_info[1].data = enable_1bit;

    (void)bcmptm_ser_pt_scan_mode_check(unit, sid, DEFAULT_SCAN_MODE, &scan_mode_opera);
    flds_info[2].data = scan_mode_opera;

    rv = bcmptm_ser_pt_control_insert(unit, sid, flds_info, COUNTOF(flds_info));
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    rv = bcmptm_ser_pt_status_imm_insert(unit, sid);
    SHR_IF_ERR_EXIT(rv);

exit:
    if (SHR_FAILURE(rv)) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "Fail to record control or status data for PT[%s]\n"),
                  bcmdrd_pt_sid_to_name(unit, sid)));
    }
    SHR_FUNC_EXIT();
}

uint32_t
bcmptm_ser_pt_control_field_val_get(int unit, bcmdrd_sid_t pt_id, uint32_t fid_lt)
{
    uint32_t bit_offset = 0, mask = 0, data = 0;

    bit_offset = lt_ser_pt_control_info[fid_lt].bit_offset;
    mask = lt_ser_pt_control_info[fid_lt].mask;

    data = ((lt_ser_pt_control_cache[unit][pt_id] >> bit_offset) & mask);

    return data;
}

int
bcmptm_ser_pt_control_cache_init(int unit)
{
    size_t sid_num = 0;
    uint32_t *ptr = NULL, cache_size = 0;
    int rv = SHR_E_NONE;

    if (lt_ser_pt_control_cache[unit] == NULL) {
        rv = bcmptm_pt_sid_count_get(unit, &sid_num);
        if (SHR_FAILURE(rv)) {
            return rv;
        }
        cache_size = sizeof(uint32_t) * sid_num;
        ptr = sal_alloc(cache_size, "bcmptmSerControlPtCache");
        if (ptr == NULL) {
            LOG_WARN(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "Fail to allocate memory space "
                                "for SER_PT_CONTROL cache\n")));
            return SHR_E_MEMORY;
        }
        sal_memset(ptr, 0, cache_size);
        lt_ser_pt_control_cache[unit] = ptr;
    }
    return SHR_E_NONE;
}

void
bcmptm_ser_pt_control_cache_cleanup(int unit)
{

    if (lt_ser_pt_control_cache[unit] != NULL) {
        sal_free(lt_ser_pt_control_cache[unit]);
        lt_ser_pt_control_cache[unit] = NULL;
    }
}

int
bcmptm_ser_pt_control_cache_sync(int unit)
{
    shr_fmm_t** fld_array = NULL;
    int rv = SHR_E_NONE;
    size_t i = 0;
    bcmltd_fields_t lt_flds_in, lt_flds_out;
    uint32_t pt_id = 0;
    size_t sid_num;

    SHR_FUNC_ENTER(unit);

    lt_flds_in.field = NULL;
    lt_flds_out.field = NULL;

    fld_array = bcmptm_ser_imm_flds_allocate(unit, 1);
    SHR_NULL_CHECK(fld_array, SHR_E_MEMORY);

    lt_flds_in.count = 1;
    lt_flds_in.field = fld_array;

    fld_array = bcmptm_ser_imm_flds_allocate(unit, SER_PT_CONTROLt_FIELD_COUNT);
    SHR_NULL_CHECK(fld_array, SHR_E_MEMORY);

    lt_flds_out.count = SER_PT_CONTROLt_FIELD_COUNT;
    lt_flds_out.field = fld_array;

    rv = bcmptm_pt_sid_count_get(unit, &sid_num);
    SHR_IF_ERR_EXIT(rv);

    for (pt_id = 0; pt_id < sid_num; pt_id++) {
        /* key */
        bcmptm_ser_imm_fld_set(lt_flds_in, 0, SER_PT_CONTROLt_PT_IDf, 0, pt_id);

        rv = bcmimm_entry_lookup(unit, SER_PT_CONTROLt, &lt_flds_in, &lt_flds_out);
        if (rv == SHR_E_NOT_FOUND) {
            continue;
        }
        SHR_IF_ERR_EXIT(rv);
        /* Mark this entry is used. */
        bcmptm_ser_pt_control_cache_update(unit, pt_id, 0, 1);

        for (i = 0; i < lt_flds_out.count; i++) {
            bcmptm_ser_pt_control_cache_update(unit, pt_id, lt_flds_out.field[i]->id,
                                               lt_flds_out.field[i]->data);
        }
    }
    rv = SHR_E_NONE;

exit:
    if (lt_flds_in.field) {
        bcmptm_ser_imm_flds_free(unit, lt_flds_in.field, lt_flds_in.count);
    }
    if (lt_flds_out.field != NULL) {
        /* lt_flds_out.count may be modified by bcmimm_entry_lookup */
        bcmptm_ser_imm_flds_free(unit, lt_flds_out.field, SER_PT_CONTROLt_FIELD_COUNT);
    }
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Fail to sync IMM SER_PT_CONTROLt data to cache\n")));
    }
    SHR_FUNC_EXIT();
}

int
bcmptm_ser_pt_control_callback_register(int unit)
{
    SHR_FUNC_ENTER(unit);
    /* To register callback for SER LTs here. */
    SHR_IF_ERR_EXIT
        (bcmimm_lt_event_reg(unit,
                             SER_PT_CONTROLt,
                             &bcmptm_ser_pt_control_callback,
                             NULL));
exit:
    SHR_FUNC_EXIT();
}

