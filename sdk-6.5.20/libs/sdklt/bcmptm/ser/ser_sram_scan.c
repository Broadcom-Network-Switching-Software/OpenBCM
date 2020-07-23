/*! \file ser_sram_scan.c
 *
 * Functions for SER SRAM S/W scan.
 *
 * Include SER SRAM S/W scan.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_sem.h>
#include <bcmdrd_config.h>
#include <bcmdrd/bcmdrd_hal.h>
#include <bsl/bsl.h>
#include <sal/sal_alloc.h>
#include <shr/shr_debug.h>
#include <shr/shr_thread.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmdrd/bcmdrd_feature.h>
#include <sal/sal_sleep.h>
#include <bcmdrd/bcmdrd_pt_ser.h>
#include <bcmptm/bcmptm_ser_internal.h>
#include <bcmptm/bcmptm_ser_chip_internal.h>
#include <sal/sal_libc.h>
#include <bcmevm/bcmevm_api.h>
#include <bcmptm/bcmptm_chip_internal.h>
#include <bcmptm/bcmptm_internal.h>

#include "ser_sram_scan.h"
#include "ser_tcam_scan.h"
#include "ser_bd.h"
#include "ser_config.h"

/******************************************************************************
 * Local definitions
 */
#define  BSL_LOG_MODULE  BSL_LS_BCMPTM_SER

/*! Information can be used to help sram scan */
typedef struct bcmptm_ser_sram_scan_info_s {
    /*! Device number */
    int unit;
    /*! Tcam scan thread ctrl info */
    shr_thread_t sram_scan_thread_ctrl;
    /*! Sram scan thread will scan this PT firstly */
    bcmdrd_sid_t begin_sid;
    /*! Notify sram scan thread DMA scan has been done */
    sal_sem_t  sram_scan_sbusdma_sync;
} bcmptm_ser_sram_scan_info_t;

/*! Cache used to save sram scanning information */
static bcmptm_ser_sram_scan_info_t *sram_scan_info[BCMDRD_CONFIG_MAX_UNITS];

/******************************************************************************
 * Private Functions
 */
/* Free resources used for SRAM scan. */
static void
bcmptm_ser_sram_scan_info_cleanup(int unit)
{
    bcmptm_ser_sram_scan_info_t *scan_info = sram_scan_info[unit];

    if (scan_info == NULL) {
        return;
    }
    sram_scan_info[unit] = NULL;

    if (scan_info->sram_scan_sbusdma_sync) {
        sal_sem_destroy(scan_info->sram_scan_sbusdma_sync);
        scan_info->sram_scan_sbusdma_sync = NULL;
    }
    sal_free(scan_info);
    scan_info = NULL;
}

/* Allocated and initialize resources used for SRAM scan. */
static int
bcmptm_ser_sram_scan_info_init(int unit)
{
    bcmptm_ser_sram_scan_info_t *scan_info = sram_scan_info[unit];
    size_t alloc_size = 0;

    SHR_FUNC_ENTER(unit);

    if (scan_info != NULL) {
        return SHR_E_NONE;
    }
    alloc_size = sizeof(bcmptm_ser_sram_scan_info_t);
    scan_info = sal_alloc(alloc_size, "bcmptmSerSramScanInfo");
    SHR_NULL_CHECK(scan_info, SHR_E_MEMORY);

    sram_scan_info[unit] = scan_info;

    sal_memset(scan_info, 0, alloc_size);

    scan_info->begin_sid = 0;
    scan_info->unit = unit;

    scan_info->sram_scan_sbusdma_sync = sal_sem_create("scan sbus dma sync", SAL_SEM_BINARY, 0);
    if (scan_info->sram_scan_sbusdma_sync == NULL) {
        bcmptm_ser_sram_scan_info_cleanup(unit);
        SHR_NULL_CHECK(NULL, SHR_E_MEMORY);
    }
    scan_info->sram_scan_thread_ctrl = NULL;

exit:
    if (SHR_FUNC_ERR()) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             " Fail to init sram scan information!\n")));
    }
    SHR_FUNC_EXIT();
}

/*
 * When a DMA scan work is done, this callback will be called to
 * notify SRAM scan thread.
 */
static void
bcmptm_ser_sram_sbusdma_cb(int unit, void *data)
{
    sal_sem_give(sram_scan_info[unit]->sram_scan_sbusdma_sync);
}

/* Wait for completion of a DMA scan work, after that delete the work. */
static bool
bcmptm_ser_sram_work_wait(int unit, bcmbd_sbusdma_work_t *work)
{
    bool is_done = FALSE;

    sal_sem_take(sram_scan_info[unit]->sram_scan_sbusdma_sync, SAL_SEM_FOREVER);
    if (work->state == SBUSDMA_WORK_DONE) {
        is_done = TRUE;
    }
    if (work->items > 1) {
        (void)bcmbd_sbusdma_batch_work_delete(unit, work);
    }
    return is_done;
}

/* Conduct a DMA scan work. */
static int
bcmptm_ser_sram_scan(int unit, bcmdrd_sid_t sid, int tbl_inst,
                     int tbl_copy, int begin_index,
                     int index_num, uint32_t work_flags)
{
    int rv = SHR_E_NONE;
    bcmbd_sbusdma_work_t *work = NULL;
    sal_usecs_t time_to_wait;
    uint32_t retry_count, retry_time = 0;
    bcmptm_ser_cth_drv_t *cth_drv = bcmptm_ser_cth_drv_get();

    work = bcmptm_ser_sbusdma_work_init(unit, sid, tbl_inst, tbl_copy,
                                        &begin_index, &index_num, 1,
                                        0, work_flags);
    if (work == NULL) {
        return SHR_E_INIT;
    }
    time_to_wait = (sal_usecs_t)cth_drv->ctrl_fld_val_get
        (unit, SER_CONTROLt_MEM_SCAN_TIME_TO_WAITf);
    time_to_wait *= 1000;
    retry_count = cth_drv->ctrl_fld_val_get
        (unit, SER_CONTROLt_MEM_SCAN_RETRY_COUNTf);

retry:
    rv = bcmptm_ser_sbusdma_work_exec(unit, work,
                                      bcmptm_ser_sram_sbusdma_cb);
    if (SHR_FAILURE(rv)) {
        bcmptm_ser_sbusdma_work_cleanup(unit, work);
        return rv;
    }
    if (bcmptm_ser_sram_work_wait(unit, work)) {
        bcmptm_ser_sbusdma_work_cleanup(unit, work);
        rv = SHR_E_NONE;
        return rv;
    } else if (retry_time < retry_count) {
        LOG_INFO(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "SRAM SER error is triggered.\n")));
        retry_time++;
        /* wait for SERC thread to correct SER error */
        sal_usleep(time_to_wait);
        /*
        * there may be multiple ser errors in DMA range,
        * re-scan this DMA range to trigger other SER errors.
        */
        goto retry;
    } else {
        LOG_INFO(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "Fail to scan SRAM[%s] indexes[%d...%d].\n"),
                  bcmdrd_pt_sid_to_name(unit, sid), begin_index,
                  begin_index + index_num));
        bcmptm_ser_sbusdma_work_cleanup(unit, work);
        rv = SHR_E_NONE;
        return rv;
    }
}

/* Check whether a PT needs to be scaned or not. */
static int
bcmptm_ser_sram_is_eligible(int unit, bcmdrd_sid_t sid)
{
    bcmptm_ser_cth_drv_t *cth_drv = bcmptm_ser_cth_drv_get();
    uint32_t scan_mode_oper = 0, ser_en = 0;
    const char *acctype_str = NULL;
    uint32_t acc_type = 0;
    int rv = SHR_E_NONE;

    if (bcmdrd_pt_is_valid(unit, sid) == FALSE) {
        return FALSE;
    }
    scan_mode_oper = cth_drv->pt_ctrl_fld_val_get
        (unit, sid, SER_PT_CONTROLt_SCAN_MODE_OPERf);
    ser_en = cth_drv->pt_ctrl_fld_val_get
        (unit, sid, SER_PT_CONTROLt_ECC_PARITY_CHECKf);
    /* For memory should be skipped, its scan_mode_opera is equal to 0 */
    if (ser_en == 0 || scan_mode_oper != SCAN_USING_SW_SCAN) {
        return FALSE;
    }
    /* TCAM is scanned by SW TCAM scan thread or H/W TCAM engine */
    if (bcmdrd_pt_attr_is_cam(unit, sid) == TRUE) {
        return FALSE;
    }
    if (bcmdrd_feature_enabled(unit, BCMDRD_FT_EMUL)) {
        rv = bcmptm_ser_pt_acctype_get(unit, sid, &acc_type, &acctype_str);
        if (SHR_FAILURE(rv)) {
            LOG_WARN(BSL_LOG_MODULE,
                     (BSL_META_U(unit,
                                 " Fail to get access type of PT(%s)\n"),
                      bcmdrd_pt_sid_to_name(unit, sid)));
            return FALSE;
        }
        if (PT_IS_ADDR_SPLIT_SPLIT_ACC_TYPE(acctype_str) ||
            PT_IS_PORT_OR_SW_PORT_REG(unit, sid)) {
            return FALSE;
        }
    }
    return TRUE;
}

static int
bcmptm_ser_get_valid_index_range(int unit, bcmdrd_sid_t sid, int inst,
                                 int idx_min, int idx_max, int *idx_start,
                                 int *idx_end)
{
    int is, ie;
    int rv = SHR_E_NONE;
    const char *acctype_str = NULL;
    uint32_t acc_type = 0;

    rv = bcmptm_ser_pt_acctype_get(unit, sid, &acc_type, &acctype_str);
    if (PT_IS_ADDR_SPLIT_SPLIT_ACC_TYPE(acctype_str)) {
        /* Get first valid index. */
        is = idx_min;
        while (is <= idx_max) {
            if (bcmdrd_pt_index_valid(unit, sid, inst, is)) {
                break;
            }

            is++;
        }

        /* No valid index found. */
        if (is > idx_max) {
            return SHR_E_FAIL;
        }

        /* Get last valid index. */
        ie = is + 1;
        while (ie <= idx_max) {
            if (!bcmdrd_pt_index_valid(unit, sid, inst, ie)) {
                break;
            }

            ie++;
        };

        *idx_start = is;
        *idx_end = ie - 1;
    } else {
        *idx_start = idx_min;
        *idx_end = idx_max;
    }

    return rv;
}

/* Main routine of SRAM scan thread. */
static void
bcmptm_ser_sram_scan_thread(shr_thread_t tc, void *arg)
{
    bcmptm_ser_sram_scan_info_t *scan_info = (bcmptm_ser_sram_scan_info_t *)arg;
    int unit = scan_info->unit;
    int rv = SHR_E_NONE;
    int chunk_size = 0;
    int inst = 0, inst_num = 0, copy_num = 0, tbl_copy = 0, scan_sid_num = 0;
    int begin_index = 0, end_index = 0, max_index = 0, min_index = 0, index_num = 0;
    int entry_per_interval = 0, interval_u, interval_m;
    int is_stopping = FALSE;
    size_t sid_count, sid = scan_info->begin_sid;
    bcmptm_ser_cth_drv_t *cth_drv = bcmptm_ser_cth_drv_get();
    int blktype;
    uint32_t pipe_map;

    SHR_FUNC_ENTER(unit);

    is_stopping = shr_thread_stopping(tc);
    if (is_stopping) {
        SHR_ERR_EXIT(rv);
    }
    entry_per_interval = cth_drv->ctrl_fld_val_get
        (unit, SER_CONTROLt_SRAM_ENTRIES_READ_PER_INTERVALf);
    chunk_size = cth_drv->ctrl_fld_val_get
        (unit, SER_CONTROLt_SRAM_SCAN_CHUNK_SIZEf);

    rv = bcmptm_pt_sid_count_get(unit, &sid_count);
    SHR_IF_ERR_EXIT(rv);

    for (; ; sid++) {
        /* new round */
        if (sid >= sid_count) {
            sid = 0;
            if (scan_sid_num == 0) {
                /* No scanned PT */
                break;
            } else {
                scan_sid_num = 0;
            }
        }
        rv = bcmptm_ser_sram_is_eligible(unit, sid);
        if (rv == FALSE) {
            continue;
        }
        scan_sid_num++;
        inst_num = 0;
        rv = bcmptm_ser_tbl_inst_num_get(unit, sid, &inst_num, &copy_num);
        SHR_IF_ERR_EXIT(rv);

        if (PT_IS_PORT_OR_SW_PORT_REG(unit, sid)) {
            /* instance number */
            max_index = inst_num - 1;
            min_index = 0;
            begin_index = 0;
            inst_num = 1;
        } else {
            min_index = bcmptm_scor_pt_index_min(unit, sid);
            rv = bcmptm_ser_sram_index_valid(unit, sid, min_index, &max_index);
            if (rv == FALSE) {
                continue;
            }
            begin_index = min_index;
            end_index = max_index;
        }
        tbl_copy = -1;
        while (tbl_copy < copy_num) {
            inst = 0;
            while (inst < inst_num) {
                /* Maybe need to exit */
                is_stopping = shr_thread_stopping(tc);
                if (is_stopping) {
                    SHR_EXIT();
                }
                blktype = bcmdrd_pt_blktype_get(unit, sid, 0);
                (void)bcmdrd_dev_valid_blk_pipes_get(unit, 0, blktype, &pipe_map);
                if (!(pipe_map & (1 << inst)) ||
                    !bcmptm_ser_pt_copy_no_valid(unit, sid, inst, tbl_copy)) {
                    inst++;
                    continue;
                }

                if (begin_index > max_index) {
                    inst++;
                    begin_index = min_index;
                    continue;
                }

                bcmptm_ser_get_valid_index_range(unit, sid, inst, begin_index, max_index,
                                                 &begin_index, &end_index);
                index_num = ((begin_index + chunk_size) < end_index) ?
                    chunk_size : (end_index - begin_index + 1);

                rv = bcmptm_ser_sram_scan(unit, sid, inst, tbl_copy, begin_index,
                                          index_num, SBUSDMA_WF_DUMB_READ);
                if(SHR_FAILURE(rv)) {
                    /* Skip scanning this PT */
                    break;
                }
                begin_index += index_num;
                entry_per_interval -= index_num;
                if (entry_per_interval > 0) {
                    continue;
                }
                /* Interval should be more than 10000000, refer to definition of LT SER_CONTROLt */
                interval_m = cth_drv->ctrl_fld_val_get
                    (unit, SER_CONTROLt_SRAM_SCAN_INTERVALf);
                interval_u = interval_m * 1000;
                /* Overflow */
                interval_u = (interval_u < interval_m) ? interval_m : interval_u;
                /* Fall asleep */
                (void)shr_thread_sleep(tc, interval_u);
                /* wake up */
                /* update latest data from LT SER_CONTROLt */
                chunk_size = cth_drv->ctrl_fld_val_get
                    (unit, SER_CONTROLt_SRAM_SCAN_CHUNK_SIZEf);
                entry_per_interval = cth_drv->ctrl_fld_val_get
                    (unit, SER_CONTROLt_SRAM_ENTRIES_READ_PER_INTERVALf);
            }
            tbl_copy++;
        }
    }
exit:
    if (SHR_FUNC_ERR()) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             " There is failure in SRAM memory scan thread!\n")));
    }
    /* Keep the SID  */
    scan_info->begin_sid = sid;
    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "SRAM memory scan thread exited!\n")));
}

/******************************************************************************
 * Public Functions
 */
int
bcmptm_ser_sram_scan_start(int unit, bcmdrd_sid_t beginning_sid)
{
    uint32_t sram_scan_enable = 0;
    bcmptm_ser_sram_scan_info_t *scan_info = NULL;
    int rv = SHR_E_NONE;
    bcmptm_ser_cth_drv_t *cth_drv = bcmptm_ser_cth_drv_get();
    shr_thread_t thread_ctrl = NULL;

    sram_scan_enable =
        cth_drv->ctrl_fld_val_get(unit, SER_CONTROLt_SRAM_SCANf);
    if (sram_scan_enable == 0) {
        LOG_INFO(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "SRAM_SCANf of SER_CONTROLt is disable,"
                             " so SRAM scan thread is not started!\n")));
        return SHR_E_NONE;
    }
    scan_info = sram_scan_info[unit];
    if (scan_info != NULL) {
        thread_ctrl = scan_info->sram_scan_thread_ctrl;
        if (thread_ctrl == NULL) {
            LOG_INFO(BSL_LOG_MODULE,
                     (BSL_META_U(unit,
                                 "SRAM scan thread is exiting!\n")));
            return SHR_E_INIT;
        }
    } else {
        rv = bcmptm_ser_sram_scan_info_init(unit);
        if (SHR_FAILURE(rv)) {
            LOG_WARN(BSL_LOG_MODULE,
                     (BSL_META_U(unit,
                                 "Fail to allocate resource for SRAM thread!\n")));
            return rv;
        }
    }
    scan_info = sram_scan_info[unit];
    thread_ctrl = scan_info->sram_scan_thread_ctrl;
    if (beginning_sid != 0) {
        scan_info->begin_sid = beginning_sid;
    }
    if (thread_ctrl == NULL) {
        thread_ctrl = shr_thread_start("bcmptmSerSram", -1,
                                       bcmptm_ser_sram_scan_thread,
                                       (void *)scan_info);
        if (thread_ctrl == NULL) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "Fail to create SRAM scan thread!\n")));
            return SHR_E_FAIL;
        }
        scan_info->sram_scan_thread_ctrl = thread_ctrl;
    } else if (shr_thread_done(thread_ctrl)) {
        /* The thread exited abnormally */
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "SRAM scan thread has exited abnormally!\n")));
        bcmptm_ser_sram_scan_stop(unit);

        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "Please restart it once again!\n")));
        return SHR_E_FAIL;
    } else {
        /* Wake up the thread */
        (void)shr_thread_wake(thread_ctrl);
        LOG_INFO(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "SRAM scan thread has been started!\n")));
    }
    return SHR_E_NONE;
}

int
bcmptm_ser_sram_scan_stop(int unit)
{
    int is_stopping;
    bcmptm_ser_sram_scan_info_t *scan_info = sram_scan_info[unit];
    shr_thread_t thread_ctrl = NULL;

    if (scan_info != NULL) {
        thread_ctrl = scan_info->sram_scan_thread_ctrl;
        if (thread_ctrl == NULL) {
            LOG_WARN(BSL_LOG_MODULE,
                     (BSL_META_U(unit,
                                 "SRAM scan thread is exiting!\n")));
            return SHR_E_NONE;
        }
    } else {
        LOG_INFO(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "SRAM scan thread have been stopped!\n")));
        return SHR_E_NONE;
    }
    scan_info->sram_scan_thread_ctrl = NULL;
    /* Notify SRAM scan thread to exit */
    do {
        is_stopping = shr_thread_stop(thread_ctrl, 1000000);
        if (SHR_FAILURE(is_stopping)) {
            LOG_INFO(BSL_LOG_MODULE,
                     (BSL_META_U(unit,
                                 "SRAM scan thread is exiting, %d!\n"),
                      is_stopping));
        }
    } while (SHR_FAILURE(is_stopping));

    bcmptm_ser_sram_scan_info_cleanup(unit);

    return SHR_E_NONE;
}

int
bcmptm_ser_sram_scan_resume(int unit)
{
    bcmptm_ser_sram_scan_info_t *scan_info = sram_scan_info[unit];
    shr_thread_t thread_ctrl = NULL;

    if (scan_info == NULL) {
        LOG_INFO(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "SRAM scan thread have been stopped!\n")));
        return SHR_E_NONE;
    } else {
        thread_ctrl = scan_info->sram_scan_thread_ctrl;
        if (thread_ctrl == NULL) {
            LOG_INFO(BSL_LOG_MODULE,
                     (BSL_META_U(unit,
                                 "SRAM scan thread have been stopped!\n")));
            return SHR_E_NONE;
        }
    }
    /* Wake up the thread */
    (void)shr_thread_wake(thread_ctrl);
    return SHR_E_NONE;
}

int
bcmptm_ser_sram_index_valid(int unit, bcmdrd_sid_t sid, int index, int *max_index)
{
    uint32_t index_count = 0;

    index_count = bcmptm_scor_pt_index_count(unit, sid);
    if (index_count == 0) {
        *max_index = 0;
        return FALSE;
    } else {
        *max_index = index_count - 1;
        if (index <= *max_index) {
            return TRUE;
        } else {
            return FALSE;
        }
    }
}

