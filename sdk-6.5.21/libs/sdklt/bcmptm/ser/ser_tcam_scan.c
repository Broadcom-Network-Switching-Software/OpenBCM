/*! \file ser_tcam_scan.c
 *
 * Functions for SER TCAM S/W scan.
 *
 * Include SER TCAM S/W scan.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_sem.h>
#include <sal/sal_mutex.h>
#include <bcmptm/bcmptm_internal.h>
#include <bcmptm/bcmptm_scor_internal.h>
#include <bsl/bsl.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <shr/shr_debug.h>
#include <shr/shr_thread.h>
#include <sal/sal_alloc.h>
#include <sal/sal_sleep.h>
#include <bcmptm/bcmptm_ser_misc_internal.h>
#include <bcmptm/bcmptm_ser_chip_internal.h>
#include <bcmptm/bcmptm_ser_internal.h>
#include <bcmptm/bcmptm_chip_internal.h>

#include "ser_tcam_scan.h"
#include "ser_bd.h"
#include "ser_config.h"

/******************************************************************************
 * Local definitions
 */
#define  BSL_LOG_MODULE  BSL_LS_BCMPTM_SER

/* Maximum number of work per DMA batch read */
#define MAX_NUM_WORK_DMA_BATCH_READ     16

/* Take slice mode mutex if TCAM has  */
#define SLICE_MODE_MUTEX_TAKE(slice_mode_mutex) \
    do { \
        if (slice_mode_mutex != NULL) {  \
            (void)sal_mutex_take(slice_mode_mutex, SAL_MUTEX_FOREVER); \
        } \
    } while(0)

/* give slice mode mutex if TCAM has  */
#define SLICE_MODE_MUTEX_GIVE(slice_mode_mutex) \
    do { \
        if (slice_mode_mutex != NULL) {  \
            (void)sal_mutex_give(slice_mode_mutex); \
        } \
    } while(0)

/*! Cache table-specific info to avoid recalculating on each pass */
typedef struct bcmptm_ser_mem_scan_table_info_s {
    /*! SID */
    bcmdrd_sid_t mem;
    /*! Parameters configured by the applications */
    uint32_t     ser_flags;
    /*! min index */
    int          index_min;
    /*! max index */
    int          index_max;
} bcmptm_ser_mem_scan_table_info_t;

/*! Information can be used to help Tcam scan */
typedef struct bcmptm_ser_mem_scan_info_s {
    /* Device number */
    int unit;
    /*! Number of TCAM tables */
    int num_tables;
    /*! information of every tcam */
    bcmptm_ser_mem_scan_table_info_t *table_info;
    /*! Tcam scan thread ctrl info */
    shr_thread_t tcam_scan_thread_ctrl;
    /*! Notify Tcam scan thread DMA scan has been done */
    sal_sem_t  tcam_scan_sbusdma_sync;
} bcmptm_ser_mem_scan_info_t;

/*! Cache used to save scanning information for all TCAMs */
static bcmptm_ser_mem_scan_info_t *tcam_scan_info[BCMDRD_CONFIG_MAX_UNITS];

/*! Cache table-specific info to avoid recalculating on each pass */
#define TCAM_MEM_SCAN_TABLE_INFO(unit, table_index) \
    (&(tcam_scan_info[unit]->table_info[table_index]))

/******************************************************************************
 * Private Functions
 */
/* Gets a contiguous index ranges. */
static int
bcmptm_ser_tcam_index_range_get(int unit, bcmptm_ser_mem_scan_table_info_t *table_info,
                     int begin_index, int chunk_size, int max_range_num,
                     int *start_indexes, int *index_ranges, int *actual_range_num,
                     int *index_scan_num, int *begin_index_next)
{
    bcmdrd_sid_t mem = INVALIDm;
    int rv = SHR_E_NONE;
    int index_start = 0, index_count = 0;
    int i = 0, index_range_sum = 0;

    mem = table_info->mem;

    *begin_index_next = table_info->index_max + 1;
    *index_scan_num = 0;
    *actual_range_num = 0;

    index_count = 0;
    index_range_sum = 0;
    index_count = 1;
    for (i = begin_index; i <= table_info->index_max; ) {
        index_start = i;
        if (table_info->ser_flags & BCMPTM_SER_FLAG_OVERLAY) {
            /* L3_DEFIPm or L3_DEFIP_PAIR_128m */
            rv = bcmptm_ser_overlay_index_stride_get(unit, mem,
                                                     index_start, &index_count);
            if (SHR_FAILURE(rv)) {
                /* There is failure, skip scan this memory */
                *begin_index_next = table_info->index_max + 1;
                break;
            } else if (index_count >= 1) {
                if (index_count > chunk_size) {
                    index_count = chunk_size;
                }
                start_indexes[0] = index_start;
                index_ranges[0] = index_count;
                *actual_range_num = 1;
                *begin_index_next = index_start + index_count;
                *index_scan_num = index_count;
                break;
            }
            i += 1;
        } else {
            /* tcam has slices or tcam has no slices*/
            rv = bcmptm_ser_slice_index_adjust(unit, mem,
                                               &index_start, &index_count);
            if (rv == SHR_E_NOT_FOUND) {
                /* tcam has no slices */
                index_count = table_info->index_max - index_start + 1;
                rv = SHR_E_NONE;
            }
            if (index_count <= 0 || SHR_FAILURE(rv)) {
                /* all valid indexes are scanned, index_start = max_index + 1 */
                *begin_index_next = table_info->index_max + 1;
                break;
            } else if (index_count > chunk_size) {
                index_count = chunk_size;
            }
            index_range_sum += index_count;
            if (index_range_sum > chunk_size) {
                break;
            }
            *index_scan_num = index_range_sum;
            start_indexes[*actual_range_num] = index_start;
            index_ranges[*actual_range_num] = index_count;
            *begin_index_next = index_start + index_count;
            (*actual_range_num)++;
            if (*actual_range_num >= max_range_num) {
                break;
            }
            i = *begin_index_next;
        }
    }
    return SHR_E_NONE;
}

/*
 * When a DMA scan work is done, this callback will be called to
 * notify TCAM scan thread.
 */
static void
bcmptm_ser_tcam_sbusdma_cb(int unit, void *data)
{
    sal_sem_give(tcam_scan_info[unit]->tcam_scan_sbusdma_sync);
}

/* Wait for completion of a DMA scan work, after that delete the work. */
static bool
bcmptm_ser_tcam_sbusdma_wait(int unit, bcmbd_sbusdma_work_t *work)
{
    bool is_done = FALSE;

    sal_sem_take(tcam_scan_info[unit]->tcam_scan_sbusdma_sync, SAL_SEM_FOREVER);
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
bcmptm_ser_tcam_batch_scan(int unit, bcmptm_ser_mem_scan_table_info_t *table_info,
                           int pipe_no, int begin_index, int chunk_size,
                           int *index_num_scanned, int *begin_index_next,
                           uint32_t work_flags)
{
    int actual_range_num = 0, rv = SHR_E_NONE;
    int start_indexes[MAX_NUM_WORK_DMA_BATCH_READ];
    int index_ranges[MAX_NUM_WORK_DMA_BATCH_READ];
    uint32_t entry_data[BCMDRD_MAX_PT_WSIZE];
    bcmbd_sbusdma_work_t *scan_work = NULL;
    sal_usecs_t time_to_wait;
    uint32_t retry_count, retry_time = 0;
    bcmptm_ser_cth_drv_t *cth_drv = bcmptm_ser_cth_drv_get();

    /* value of index_num_scanned may be less than value of chunk_size */
    rv = bcmptm_ser_tcam_index_range_get(unit, table_info,
                                         begin_index, chunk_size,
                                         MAX_NUM_WORK_DMA_BATCH_READ,
                                         start_indexes, index_ranges,
                                         &actual_range_num,
                                         index_num_scanned,
                                         begin_index_next);
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Fail to get indexes to be scanned!\n")));
        return rv;
    }
    if (actual_range_num <= 0) {
        return rv;
    }
    if (*index_num_scanned == 1) {
        /* only one entry need to be read */
        rv = bcmptm_ser_pt_read(unit, table_info->mem, start_indexes[0], pipe_no,
                                -1, entry_data, BCMDRD_MAX_PT_WSIZE,
                                BCMPTM_SCOR_REQ_FLAGS_DONT_USE_CACHE);
        if(SHR_FAILURE(rv)) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "SER error is triggered.\n")));
            rv = SHR_E_NONE;
        }
        return rv;
    } else {
        scan_work =
            bcmptm_ser_sbusdma_work_init(unit, table_info->mem, pipe_no,
                                         -1, start_indexes, index_ranges,
                                         actual_range_num, 0, work_flags);
        if (scan_work == NULL) {
            return SHR_E_INIT;
        }
    }
    time_to_wait = (sal_usecs_t)cth_drv->ctrl_fld_val_get
        (unit, SER_CONTROLt_MEM_SCAN_TIME_TO_WAITf);
    time_to_wait *= 1000;
    retry_count = cth_drv->ctrl_fld_val_get
        (unit, SER_CONTROLt_MEM_SCAN_RETRY_COUNTf);

retry:
    rv = bcmptm_ser_sbusdma_work_exec(unit, scan_work,
                                      bcmptm_ser_tcam_sbusdma_cb);
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Fail to start SBUS DMA to scan TCAM.\n")));
        bcmptm_ser_sbusdma_work_cleanup(unit, scan_work);
        scan_work = NULL;
        return rv;
    }
    if (bcmptm_ser_tcam_sbusdma_wait(unit, scan_work)) {
        bcmptm_ser_sbusdma_work_cleanup(unit, scan_work);
        rv = SHR_E_NONE;
        return rv;
    } else if (retry_time < retry_count) {
        LOG_INFO(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "TCAM SER error is triggered.\n")));
        rv = SHR_E_NONE;
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
                             "Fail to scan TCAM[%s].\n"),
                  bcmdrd_pt_sid_to_name(unit, table_info->mem)));
        rv = SHR_E_NONE;
        bcmptm_ser_sbusdma_work_cleanup(unit, scan_work);
        return rv;
    }
}

/* Clean up resources used by TCAM scan. */
static void
bcmptm_ser_tcam_scan_info_cleanup(int unit)
{
    bcmptm_ser_mem_scan_info_t *scan_info_ptr = NULL;

    if (tcam_scan_info[unit] == NULL) {
        return;
    }
    scan_info_ptr = tcam_scan_info[unit];
    tcam_scan_info[unit] = NULL;

    if (scan_info_ptr->table_info != NULL) {
        sal_free(scan_info_ptr->table_info);
        scan_info_ptr->table_info = NULL;
    }
    if (scan_info_ptr->tcam_scan_sbusdma_sync) {
        sal_sem_destroy(scan_info_ptr->tcam_scan_sbusdma_sync);
        scan_info_ptr->tcam_scan_sbusdma_sync = NULL;
    }
    sal_free(scan_info_ptr);
    scan_info_ptr = NULL;
}

/* Allocate and initialize resources used by TCAM scan. */
static int
bcmptm_ser_tcam_scan_info_init(int unit)
{
    bcmptm_ser_tcam_generic_info_t *tcam_ser_info = NULL;
    int tcam_info_num = 0, rv = SHR_E_NONE, alloc_size = 0;
    bcmptm_ser_mem_scan_info_t *scan_info_ptr = NULL;
    int ser_idx = 0;
    bcmptm_ser_mem_scan_table_info_t *table_info = NULL;
    bcmdrd_sid_t mem_sid = INVALIDm;

    SHR_FUNC_ENTER(unit);

    if (tcam_scan_info[unit] != NULL) {
        return SHR_E_NONE;
    }
    rv = bcmptm_ser_tcam_info_get(unit, &tcam_ser_info, &tcam_info_num);
    SHR_IF_ERR_VERBOSE_EXIT(rv);

    alloc_size = sizeof(bcmptm_ser_mem_scan_info_t);
    scan_info_ptr = sal_alloc(alloc_size, "bcmptmSerTcamScanInfo");
    SHR_NULL_CHECK(scan_info_ptr, SHR_E_MEMORY);

    tcam_scan_info[unit] = scan_info_ptr;
    sal_memset(scan_info_ptr, 0, alloc_size);

    scan_info_ptr->num_tables = tcam_info_num;
    alloc_size = tcam_info_num * sizeof(bcmptm_ser_mem_scan_table_info_t);
    scan_info_ptr->table_info = sal_alloc(alloc_size, "bcmptmSerTcamTblInfo");
    if ( scan_info_ptr->table_info == NULL) {
        sal_free(scan_info_ptr);
        scan_info_ptr = NULL;
        rv = SHR_E_MEMORY;
        SHR_NULL_CHECK(NULL, rv);
    }
    sal_memset(scan_info_ptr->table_info, 0, alloc_size);

    for (ser_idx = 0; ser_idx < scan_info_ptr->num_tables; ser_idx++) {
        table_info = &scan_info_ptr->table_info[ser_idx];
        mem_sid = tcam_ser_info[ser_idx].mem;
        table_info->ser_flags = tcam_ser_info[ser_idx].ser_flags;

        table_info->mem = mem_sid;
        table_info->index_max = bcmptm_scor_pt_index_max(unit, mem_sid);
        table_info->index_min = bcmptm_scor_pt_index_min(unit, mem_sid);
    }
    scan_info_ptr->tcam_scan_sbusdma_sync = sal_sem_create("memscan sbus dma sync", SAL_SEM_BINARY, 0);
    if (scan_info_ptr->tcam_scan_sbusdma_sync == NULL) {
        bcmptm_ser_tcam_scan_info_cleanup(unit);
        rv = SHR_E_MEMORY;
        SHR_NULL_CHECK(NULL, rv);
    }
    scan_info_ptr->tcam_scan_thread_ctrl = NULL;
    scan_info_ptr->unit = unit;

exit:
    if (SHR_FAILURE(rv) && (rv != SHR_E_UNAVAIL)) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             " Fail to init tcam scan information!\n")));
    }
    SHR_FUNC_EXIT();
}

/* Main routine of TCAM scan thread. */
static void
bcmptm_ser_tcam_scan_thread(shr_thread_t tc, void *arg)
{
    bcmptm_ser_mem_scan_info_t *scan_info = (bcmptm_ser_mem_scan_info_t *)arg;
    int unit = scan_info->unit;
    int interval_u = 0, interval_m = 0;
    int index_num_not_scanned, index_num_scanned;
    uint32_t chunk_size = 0;
    int num_tables = 0, ser_range_idx = 0;
    bcmptm_ser_mem_scan_table_info_t *table_info = NULL;
    bcmdrd_sid_t mem = INVALIDm;
    sal_mutex_t slice_mode_mutex;
    int rv = SHR_E_NONE, pipe_no = -1, pipe_num = 0, skip_num = 0;
    int begin_index = 0, begin_index_next = 0, copy_num = 0;
    int is_stopping = TRUE;
    bcmptm_ser_sinfo_t *ser_ha_info = bcmptm_ser_ha_info_get(unit);
    uint32_t used_ser_range = 0, scan_mode_oper = 0, ser_en = 0;
    bcmptm_ser_cth_drv_t *cth_drv = bcmptm_ser_cth_drv_get();

    SHR_FUNC_ENTER(unit);

    is_stopping = shr_thread_stopping(tc);
    if (is_stopping) {
        SHR_ERR_EXIT(rv);
    }
    index_num_not_scanned = cth_drv->ctrl_fld_val_get
        (unit, SER_CONTROLt_TCAM_ENTRIES_READ_PER_INTERVALf);
    chunk_size = cth_drv->ctrl_fld_val_get
        (unit, SER_CONTROLt_TCAM_SCAN_CHUNK_SIZEf);

    num_tables = scan_info->num_tables;
    ser_range_idx = 0;
    for (; ; ser_range_idx++) {
        /* Nothing needs to be scanned */
        if (skip_num >= num_tables) {
            rv = SHR_E_NONE;
            SHR_ERR_EXIT(rv);
        }
        /* new round */
        if (ser_range_idx == num_tables) {
            skip_num = 0;
            ser_range_idx = 0;
        }

        used_ser_range = ser_ha_info->ser_range_in_usage;
        /* There is global or unique mode for the same PT */
        if (0 == (used_ser_range & (1 << ser_range_idx))) {
            skip_num++;
            continue;
        }
        table_info = TCAM_MEM_SCAN_TABLE_INFO(unit, ser_range_idx);
        mem = table_info->mem;
        scan_mode_oper = cth_drv->pt_ctrl_fld_val_get
            (unit, mem, SER_PT_CONTROLt_SCAN_MODE_OPERf);
        ser_en = cth_drv->pt_ctrl_fld_val_get
            (unit, mem, SER_PT_CONTROLt_ECC_PARITY_CHECKf);
        if (ser_en == 0 || scan_mode_oper != SCAN_USING_SW_SCAN) {
            skip_num++;
            continue;
        }
        pipe_num = 0;
        if (table_info->ser_flags & BCMPTM_SER_FLAG_MULTI_PIPE) {
            rv = bcmptm_ser_tbl_inst_num_get(unit, mem, &pipe_num, &copy_num);
            SHR_IF_ERR_EXIT(rv);
        }
        rv = bcmptm_ser_slice_mode_mutex_get(unit, mem, &slice_mode_mutex);
        if (SHR_FAILURE(rv)) {
            slice_mode_mutex = NULL;
        }
        begin_index = table_info->index_min;
        pipe_no = 0;
        while(pipe_no < pipe_num) {
            /* Maybe need to exit */
            is_stopping = shr_thread_stopping(tc);
            if (is_stopping) {
                rv = SHR_E_NONE;
                SHR_ERR_EXIT(rv);
            }
            SLICE_MODE_MUTEX_TAKE(slice_mode_mutex);
            rv = bcmptm_ser_tcam_batch_scan(unit, table_info, pipe_no,
                                            begin_index, chunk_size,
                                            &index_num_scanned,
                                            &begin_index_next,
                                            SBUSDMA_WF_DUMB_READ);
            SLICE_MODE_MUTEX_GIVE(slice_mode_mutex);
            if (SHR_FAILURE(rv)) {
                break;
            }
            begin_index = begin_index_next;
            if (begin_index > table_info->index_max) {
                pipe_no++;
                begin_index = table_info->index_min;
            }
            index_num_not_scanned -= index_num_scanned;
            if (index_num_not_scanned > 0) {
                continue;
            }
            /* Interval should be more than 1000000, refer to definition of LT SER_CONTROLt */
            interval_m = cth_drv->ctrl_fld_val_get
                (unit, SER_CONTROLt_TCAM_SCAN_INTERVALf);
            interval_u = interval_m * 1000;
            /* Overflow */
            interval_u = (interval_u < interval_m) ? interval_m : interval_u;
            /* Fall asleep */
            (void)shr_thread_sleep(tc, interval_u);
            /* wake up */
            /* update latest data from LT SER_CONTROLt */
            chunk_size = cth_drv->ctrl_fld_val_get
                (unit, SER_CONTROLt_TCAM_SCAN_CHUNK_SIZEf);
            index_num_not_scanned = cth_drv->ctrl_fld_val_get
                (unit, SER_CONTROLt_TCAM_ENTRIES_READ_PER_INTERVALf);
        }
    }

exit:
    if (SHR_FUNC_ERR()) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             " There is failure in TCAM memory scan thread!\n")));
    }
    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         " TCAM memory scan thread exited!\n")));
}

/******************************************************************************
 * Public Functions
 */
int
bcmptm_ser_tcam_scan_start(int unit)
{
    uint32_t tcam_scan_enable = 0;
    bcmptm_ser_mem_scan_info_t *scan_info = NULL;
    int rv = SHR_E_NONE;
    bcmptm_ser_cth_drv_t *cth_drv = bcmptm_ser_cth_drv_get();
    shr_thread_t thread_ctrl = NULL;

    tcam_scan_enable =
        cth_drv->ctrl_fld_val_get(unit, SER_CONTROLt_TCAM_SCANf);
    if (tcam_scan_enable == 0) {
        LOG_INFO(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "TCAM_SCANf of SER_CONTROLt is disable,"
                             " so TCAM scan thread is not started!\n")));
        return SHR_E_NONE;
    }
    scan_info = tcam_scan_info[unit];
    if (scan_info != NULL) {
        thread_ctrl = scan_info->tcam_scan_thread_ctrl;
        if (thread_ctrl == NULL) {
            LOG_INFO(BSL_LOG_MODULE,
                     (BSL_META_U(unit,
                                 "TCAM scan thread is exiting!\n")));
            return SHR_E_INIT;
        }
    } else {
        rv = bcmptm_ser_tcam_scan_info_init(unit);
        if (rv == SHR_E_UNAVAIL) {
            /* Enable H/W TCAM scan */
            rv = bcmptm_ser_tcam_hw_scan_init(unit, 1);
            /* exit */
            return rv;
        } else if (SHR_FAILURE(rv)) {
            LOG_WARN(BSL_LOG_MODULE,
                     (BSL_META_U(unit,
                                 "Fail to allocate resource for TCAM thread!\n")));
            return rv;
        }
    }
    scan_info = tcam_scan_info[unit];
    thread_ctrl = scan_info->tcam_scan_thread_ctrl;

    if (thread_ctrl == NULL) {
        thread_ctrl = shr_thread_start("bcmptmSerTcamScan", -1,
                                       bcmptm_ser_tcam_scan_thread,
                                       (void *)scan_info);
        if (thread_ctrl == NULL) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "Fail to create TCAM scan thread!\n")));
            return SHR_E_FAIL;
        }
        scan_info->tcam_scan_thread_ctrl = thread_ctrl;
    } else if (shr_thread_done(thread_ctrl)) {
        /* The thread exited abnormally */
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "TCAM scan thread has exited abnormally!\n")));
        bcmptm_ser_tcam_scan_stop(unit);

        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "Please restart it once again!\n")));
        return SHR_E_FAIL;
    } else {
        /* Wake up the thread */
        (void)shr_thread_wake(thread_ctrl);
        LOG_INFO(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "TCAM scan thread has been started!\n")));
    }
    return SHR_E_NONE;
}

int
bcmptm_ser_tcam_scan_stop(int unit)
{
    int rv = SHR_E_NONE, is_stopping;
    bcmptm_ser_mem_scan_info_t *scan_info = tcam_scan_info[unit];
    shr_thread_t thread_ctrl = NULL;

    if (scan_info != NULL) {
        thread_ctrl = scan_info->tcam_scan_thread_ctrl;
        if (thread_ctrl == NULL) {
            LOG_WARN(BSL_LOG_MODULE,
                     (BSL_META_U(unit,
                                 "TCAM scan thread is exiting!\n")));
            return SHR_E_NONE;
        }
    } else {
        /* Disable H/W TCAM scan if supports */
        rv = bcmptm_ser_tcam_hw_scan_init(unit, 0);
        LOG_INFO(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "TCAM scan thread have been stopped!\n")));
        return rv;
    }
    scan_info->tcam_scan_thread_ctrl = NULL;
    /* Notify tcam scan thread to exit */
    do {
        is_stopping = shr_thread_stop(thread_ctrl, 1000000);
        if (SHR_FAILURE(is_stopping)) {
            LOG_INFO(BSL_LOG_MODULE,
                     (BSL_META_U(unit,
                                 "TCAM scan thread is exiting: %d!\n"),
                      is_stopping));
        }
    } while (SHR_FAILURE(is_stopping));

    bcmptm_ser_tcam_scan_info_cleanup(unit);
    return rv;
}

