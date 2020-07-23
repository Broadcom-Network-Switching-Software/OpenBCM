/*! \file ser_lts.c
 *
 * SER LTs initialization functions
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_alloc.h>
#include <sal/sal_libc.h>
#include <bsl/bsl.h>
#include <shr/shr_debug.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmptm/bcmptm_ser_internal.h>
#include <bcmptm/bcmptm_ser_cth.h>
#include <bcmptm/bcmptm_ser_cth_internal.h>
#include <bcmptm/bcmptm_ser_chip_internal.h>
#include <bcmdrd/bcmdrd_feature.h>
#include <bcmimm/bcmimm_int_comp.h>
#include <bcmcfg/bcmcfg_lt.h>
#include <bcmptm/bcmptm_ser_misc_internal.h>

#include "ser_lts.h"
#include "ser_lt_ser_control.h"
#include "ser_lt_ser_pt_control.h"
#include "ser_lt_ser_injection.h"
#include "ser_lt_ser_injection_status.h"
#include "ser_lt_ser_stats.h"
#include "ser_lt_ser_notification.h"
#include "ser_lt_ser_log.h"
#include "ser_lt_ser_pt_status.h"
#include "ser_lt_ser_log_status.h"

/******************************************************************************
 * Local definitions
 */
#define  BSL_LOG_MODULE  BSL_LS_BCMPTM_SER

/*! Default depth of INTR dispatching message queue */
#define MSG_Q_DEFAULT_DEPTH   8

static uint32_t
bcmptm_ser_msg_q_depth_get(int unit);

static bcmptm_ser_cth_drv_t bcmptm_cth_drv = {

    .ctrl_fld_val_get = &bcmptm_ser_control_field_val_get,

    .pt_ctrl_fld_val_get = &bcmptm_ser_pt_control_field_val_get,

    .pt_ctrl_imm_update = &bcmptm_ser_pt_control_imm_update,

    .pt_ctrl_imm_insert = &bcmptm_ser_pt_control_imm_insert,

    .ser_hw_fault_notify = &bcmptm_ser_hw_fault_notify,

    .ser_stats_update = &bcmptm_ser_stats_update,

    .ser_log_cache_get = bcmptm_ser_log_cache_get,

    .ser_log_imm_update = bcmptm_ser_log_imm_update,

    .ser_check_enable = &bcmptm_ser_checking_enable,

    .pt_status_fld_val_get = &bcmptm_ser_pt_status_field_val_get,
};

/******************************************************************************
 * Private Functions
 */
/* Get field value of SER_CONFIG. The field value can be provided in 'config.yml' */
static int
bcmptm_ser_config_field_data_get(int unit, uint32_t lt_fid, uint32_t *lt_fld_data)
{
    int rv = SHR_E_NONE;
    uint64_t fld_val;

    SHR_FUNC_ENTER(unit);
    rv = bcmcfg_field_get(unit, SER_CONFIGt, lt_fid, &fld_val);
    SHR_IF_ERR_EXIT(rv);

    *lt_fld_data = (uint32_t)fld_val;

exit:
    SHR_FUNC_EXIT();
}

/* Get depth of SER message queue. */
static uint32_t
bcmptm_ser_msg_q_depth_get(int unit)
{
    uint32_t intr_msg_q_depth = MSG_Q_DEFAULT_DEPTH;

    (void)bcmptm_ser_config_field_data_get(unit, SER_CONFIGt_MESSAGE_Q_DEPTHf,
                                           &intr_msg_q_depth);
    return intr_msg_q_depth;
}

/*
 * Insert entries to all SER LTs, so application only needs to do
 * 'update' instead of 'insert' operation.
 */
static int
bcmptm_ser_lts_imm_create(int unit, bool warm)
{
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    if (!warm) {
        rv = bcmptm_ser_control_imm_init(unit);
        SHR_IF_ERR_EXIT(rv);

        rv = bcmptm_ser_stats_imm_update(unit, 1);
        SHR_IF_ERR_EXIT(rv);

        rv = bcmptm_ser_injection_status_imm_update(unit, 1);
        SHR_IF_ERR_EXIT(rv);

        rv = bcmptm_ser_notification_init(unit);
        SHR_IF_ERR_EXIT(rv);

        rv = bcmptm_ser_injection_imm_init(unit);
        SHR_IF_ERR_EXIT(rv);

        rv = bcmptm_ser_log_status_imm_init(unit);
        SHR_IF_ERR_EXIT(rv);
        /* SER_PT_CONTROL IMM LT is created in routine bcmptm_ser_pt_ser_enable */
    }

exit:
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Fail to create SER LTs\n")));
    }
    SHR_FUNC_EXIT();
}

/* During warmboot, synchronize data from HA IMM to cache of all SER LTs. */
static int
bcmptm_ser_lts_cache_sync(int unit, int warm)
{
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    /* Not need to sync cache of SER_NOTIFICATIONt/SER_INJECTION_STATUSt/SER_INJECTIONt */
    if (warm) {
        rv = bcmptm_ser_control_cache_sync(unit);
        SHR_IF_ERR_EXIT(rv);

        rv = bcmptm_ser_pt_control_cache_sync(unit);
        SHR_IF_ERR_EXIT(rv);

        /* If cold booting, default value of this LT is zero */
        rv = bcmptm_ser_stats_cache_sync(unit);
        SHR_IF_ERR_EXIT(rv);

        rv = bcmptm_ser_log_status_cache_sync(unit);
        SHR_IF_ERR_EXIT(rv);
    }

exit:
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Fail to sync data of IMM LTs to cache\n")));
    }
    SHR_FUNC_EXIT();
}

/* Register validation and stage callback routines to every SER LT. */
static int
bcmptm_ser_lts_imm_register(int unit)
{
    SHR_FUNC_ENTER(unit);

    /* To register callback for SER LTs here. */
    SHR_IF_ERR_EXIT
        (bcmptm_ser_control_callback_register(unit));

    SHR_IF_ERR_EXIT
        (bcmptm_ser_pt_control_callback_register(unit));

    SHR_IF_ERR_EXIT
        (bcmptm_ser_injection_callback_register(unit));

    SHR_IF_ERR_EXIT
        (bcmptm_ser_stats_callback_register(unit));

    SHR_IF_ERR_EXIT
        (bcmptm_ser_pt_status_callback_register(unit));

exit:
    SHR_FUNC_EXIT();
}

/******************************************************************************
 * Public Functions
 */
int
bcmptm_ser_init(int unit, bool warm)
{
    bool ser_drv_init = 0;

    SHR_FUNC_ENTER(unit);

    bcmptm_ser_cth_drv_register(&bcmptm_cth_drv);

    /* space for slice mode information */
    bcmptm_ser_slice_mode_info_init(unit, warm);

    /* If SER not enabled, done. */
    if (!bcmptm_ser_checking_enable(unit)) {
        SHR_EXIT();
    }

    ser_drv_init = bcmptm_ser_drv_populated(unit);

    if (ser_drv_init == FALSE) {
        SHR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

exit:
    if (SHR_FUNC_ERR()) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Fail to initialize SER component\n")));
    }
    SHR_FUNC_EXIT();
}

int
bcmptm_ser_lt_comp_config(int unit, bool warm)
{
    uint32_t length = 0;

    SHR_FUNC_ENTER(unit);

    /* If SER not enabled, done. */
    if (!bcmptm_ser_checking_enable(unit)) {
        SHR_EXIT();
    }

    length = bcmptm_ser_msg_q_depth_get(unit);
    bcmptm_ser_intr_msg_q_length_set(unit, length);
    SHR_IF_ERR_EXIT
        (bcmptm_ser_intr_msg_q_create(unit));

    /* Allocate HA memory space */
    SHR_IF_ERR_EXIT
        (bcmptm_ser_ha_info_init(unit, warm));

    /* Create cache for SER_PT_CONTROL */
    SHR_IF_ERR_EXIT
        (bcmptm_ser_pt_control_cache_init(unit));

    /* Create cache for SER_STATS */
    SHR_IF_ERR_EXIT
        (bcmptm_ser_stats_cache_allocate(unit));

    bcmptm_ser_log_cache_allocate(unit);

    SHR_IF_ERR_EXIT
        (bcmptm_ser_lts_imm_create(unit, warm));

    SHR_IF_ERR_EXIT
        (bcmptm_ser_lts_cache_sync(unit, warm));

    SHR_IF_ERR_EXIT
        (bcmptm_ser_lts_imm_register(unit));

exit:
    if (SHR_FUNC_ERR()) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Fail to configure SER LTs\n")));
    }
    SHR_FUNC_EXIT();
}

int
bcmptm_ser_cleanup(int unit, bool graceful)
{
    /* space for slice mode information */
    bcmptm_ser_slice_mode_info_deinit(unit, graceful);

    bcmptm_ser_intr_msg_q_destroy(unit);

    /* Destroy cache for SER_PT_CONTROL */
    bcmptm_ser_pt_control_cache_cleanup(unit);

    /* Destroy cache for SER_STATS */
    bcmptm_ser_stats_cache_destroy(unit);

    bcmptm_ser_log_cache_free(unit);

    return SHR_E_NONE;
}

void
bcmptm_ser_imm_fld_set(bcmltd_fields_t lt_flds, int index, uint32_t fid_lt,
                       int array_fld_idx, uint32_t data)
{
    lt_flds.field[index]->id = fid_lt;
    lt_flds.field[index]->data = data;
    lt_flds.field[index]->idx = array_fld_idx;
}

void
bcmptm_ser_imm_flds_free(int unit, shr_fmm_t** fld_array, uint32_t fld_num)
{
    int i = (int)fld_num;

    for (i -= 1; i >= 0; i--) {
        sal_free(fld_array[i]);
        fld_array[i] = NULL;
    }
    sal_free(fld_array);
    fld_array = NULL;

}

shr_fmm_t **
bcmptm_ser_imm_flds_allocate(int unit, uint32_t fld_num)
{
    shr_fmm_t** fld_array = NULL;
    int i = 0, num = (int)fld_num;

    fld_array = sal_alloc(sizeof(shr_fmm_t *) * num, "bcmptmSerFmmPointer");
    if (fld_array == NULL) {
        return NULL;
    }
    for (i = 0; i < num; i++) {
        fld_array[i] = sal_alloc(sizeof(shr_fmm_t), "bcmptmSerFmm");
        sal_memset(fld_array[i], 0, sizeof(shr_fmm_t));
        if (fld_array[i] == NULL) {
            break;
        }
    }
    if (i == num) {
        return fld_array;
    } else {
        bcmptm_ser_imm_flds_free(unit, fld_array, i);
        return NULL;
    }
}

bool
bcmptm_ser_checking_enable(int unit)
{
    uint32_t  ser_checking = SER_CHECKING_DISABLE;
    bool      psim = FALSE, bcmsim = FALSE;
    bool      enable = FALSE;

    psim = bcmdrd_feature_enabled(unit, BCMDRD_FT_PASSIVE_SIM);
    bcmsim = bcmdrd_feature_enabled(unit, BCMDRD_FT_ACTIVE_SIM);
    if (psim || bcmsim) {
        ser_checking = SER_CHECKING_DISABLE;
    } else {
        (void)bcmptm_ser_config_field_data_get(unit, SER_CONFIGt_SER_ENABLEf,
                                               &ser_checking);
    }
    enable = ser_checking ? SER_CHECKING_ENABLE : SER_CHECKING_DISABLE;
    return enable;
}

