/*! \file bcma_bcmlt_capture.c
 *
 * Logical table capture API.
 *
 * The LT capture API utilizes the BCMLT capture feature and implements
 * the "capture" functionality.
 *
 * The capture API uses the BCMLT capture API callback to log LT operations
 * to a file.
 *
 * The captured LT operations can not be decoded to the CLI commands without
 * invoking the LT replay API (see \ref bcma_bcmlt_replay.c).
 * If real-time display of the captured LT operation is requested,
 * LT replay API will be executed simultaneously to start the replay engine
 * in a separate thread in order to support the blocking read callback.
 * This blocking read callback function can wait for the blocking write callback
 * from LT capture API when an LT operation occurs and read whatever is
 * written from the BCMLT capture API to the BCMLT replay engine. The replay
 * engine will compose the data to an entry-based or a transaction-based
 * operation and let the CLI to decode it to a human-readable output.
 * Note that for the real-time display functionality the replay engine is run
 * in "view only" mode such that no new operations are submitted.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>

#include <sal/sal_alloc.h>
#include <sal/sal_libc.h>
#include <sal/sal_mutex.h>
#include <sal/sal_sem.h>

#include <shr/shr_debug.h>
#include <shr/shr_thread.h>

#include <bcmdrd/bcmdrd_dev.h>

#include <bcma/io/bcma_io_file.h>
#include <bcma/bcmlt/bcma_bcmlt.h>
#include <bcma/bcmlt/bcma_bcmlt_capture_replay.h>

#define BSL_LOG_MODULE BSL_LS_APPL_BCMLT

/*******************************************************************************
 * Local definitions
 */

/* Information for support real-time replay in LT capture. */
typedef struct realtime_replay_data_s {

    /* Parameters for starting the replay thread API. */
    bcma_bcmlt_replay_params_t rparams;

    /* Semaphore to indicate that the data from capture write is ready. */
    sal_sem_t cap_write_done_sem;

    /* Semaphore to indicate that the data from replay read is done. */
    sal_sem_t rep_read_done_sem;

    /* Flag to indicate that the replay thread is done. */
    bool done;

    /* Buffer for capture write data. */
    void *buf;

    /* Data size of capture write data. */
    uint32_t bytes;

} realtime_replay_data_t;

/* Capture information */
typedef struct capture_info_s {

    /* The file to store the captured raw data (in binary) if not NULL. */
    char *cap_file;

    /* If 'cap_file' is not NULL, 'fh' is the file handle of 'cap_file'. */
    bcma_io_file_handle_t fh;

    /* Mutex lock for writing data to 'fh' in capture. */
    sal_mutex_t lock;

    /*
     * If not NULL, the capture will be started by running the replay thread
     * API to support the real-time decoding captured data to human-readable
     * CLI commands.
     */
    realtime_replay_data_t *rt_rpd;

    /*
     * Skip the LOOKUP and TRAVERSE operations (read operations) in capture
     * if set to true.
     */
    bool write_only;

} capture_info_t;

static capture_info_t *capture_info[BCMDRD_CONFIG_MAX_UNITS];

/*******************************************************************************
 * Private functions
 */

static void
capture_rt_replay_done(int unit, int rv)
{
    capture_info_t *cap_info = capture_info[unit];
    realtime_replay_data_t *rpd;

    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Failed to replay the captured data in realtime: "
                              "%s (%d)\n"), shr_errmsg(rv), rv));
    }

    if (cap_info == NULL) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "Capture info was destroyed before realtime replay "
                             "thread is done\n")));
        return;
    }
    rpd = cap_info->rt_rpd;
    rpd->done = true;

    /*
     * Make sure the capture write will not wait for the replay read
     * since now the replay engine thread is done.
     */
    sal_sem_give(rpd->rep_read_done_sem);
}

/*
 * Callback function from bcmlt_capture_replay() to read data for replay.
 * This is called from a separate thread when real-time verbose replay is
 * enabled in capture.
 */
static uint32_t
capture_rt_replay_read(void *fd, void *buffer, uint32_t nbyte)
{
    realtime_replay_data_t *rpd = (realtime_replay_data_t *)fd;
    uint32_t rb = 0;

    /* Wait for the data from capture write callback */
    sal_sem_take(rpd->cap_write_done_sem, SAL_SEM_FOREVER);
    /* Read replay data from capture write */
    if (rpd->buf) {
        if (rpd->bytes == nbyte) {
            sal_memcpy(buffer, rpd->buf, nbyte);
            rpd->buf = NULL;
            rpd->bytes = 0;
            rb = nbyte;
        } else {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META("Failed to read data for Real-time replay "
                                "(wb = %"PRIu32", rb = %"PRIu32")\n"),
                                rpd->bytes, nbyte));
        }
    }
    /*
     * Notify the capture write callback that the read from
     * the replay thread is done.
     */
    sal_sem_give(rpd->rep_read_done_sem);
    return rb;
}

/*
 * Callback function from bcmlt_capture_start() to write the captured data.
 */
static uint32_t
capture_blocking_write(void *fd, void *buffer, uint32_t nbyte)
{
    int wb = 0;
    capture_info_t *cap_info = (capture_info_t *)fd;
    realtime_replay_data_t *rpd = cap_info->rt_rpd;

    /* Lock for file write */
    sal_mutex_take(cap_info->lock, SAL_MUTEX_FOREVER);
    /* Realtime replay the captured data in verbose mode */
    if (rpd && !rpd->done) {
        rpd->buf = buffer;
        rpd->bytes = nbyte;
        /*
         * Notify the read callback from the realtime replay thread
         * that the data is ready for reading.
         */
        sal_sem_give(rpd->cap_write_done_sem);
        /* Wait till the read from realtime replay thread is done. */
        sal_sem_take(rpd->rep_read_done_sem, SAL_SEM_FOREVER);
        wb = (rpd->bytes == 0) ? (int)nbyte : -1;
    }
    /* Log the captured data to a file */
    if (cap_info->fh) {
        wb = bcma_io_file_write(cap_info->fh, buffer, 1, nbyte);
        bcma_io_file_flush(cap_info->fh);
    }
    sal_mutex_give(cap_info->lock);

    return (wb < 0) ? 0 : wb;
}

/*
 * Callback function from bcmlt_capture_start() to write the captured data.
 */
static uint32_t
capture_nonblocking_write(void *fd, void *buffer, uint32_t nbyte)
{
    capture_info_t *cap_info = (capture_info_t *)fd;
    int wb;

    /* Lock for file write */
    sal_mutex_take(cap_info->lock, SAL_MUTEX_FOREVER);
    wb = bcma_io_file_write(cap_info->fh, buffer, 1, nbyte);
    bcma_io_file_flush(cap_info->fh);
    sal_mutex_give(cap_info->lock);

    return (wb < 0) ? 0 : wb;
}

static void
capture_rt_replay_data_cleanup(int unit, realtime_replay_data_t *rpd)
{
    if (rpd) {
        int rv;

        /*
         * Make sure the realtime replay read thread will not wait for
         * the capture write since the capture has been stopped.
         */
        if (rpd->cap_write_done_sem) {
            sal_sem_give(rpd->cap_write_done_sem);
        }
        rv = bcma_bcmlt_replay_thread_stop(unit);
        if (SHR_FAILURE(rv)) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "Failed to stop realtime replay (%d).\n"),
                       rv));
        }
        if (rpd->rep_read_done_sem) {
            sal_sem_destroy(rpd->rep_read_done_sem);
        }
        if (rpd->cap_write_done_sem) {
            sal_sem_destroy(rpd->cap_write_done_sem);
        }
        if (rpd->rparams.text_file) {
            sal_free(rpd->rparams.text_file);
        }
        sal_free(rpd);
    }
}

static int
capture_rt_replay_data_init(int unit,
                            bcma_bcmlt_capture_params_t *cparams,
                            realtime_replay_data_t **rt_rpd)
{
    realtime_replay_data_t *rpd;
    bcmlt_replay_cb_t replay_cb, *cb = &replay_cb;

    SHR_FUNC_ENTER(unit);

    rpd = sal_alloc(sizeof(*rpd), "bcmaBcmltCapRtrpData");
    if (rpd == NULL) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }
    sal_memset(rpd, 0, sizeof(*rpd));

    rpd->rep_read_done_sem = sal_sem_create("bcmaBcmltCapRtrpReadSem",
                                            SAL_SEM_BINARY, 0);
    if (rpd->rep_read_done_sem == NULL) {
        SHR_ERR_EXIT(SHR_E_FAIL);
    }
    rpd->cap_write_done_sem = sal_sem_create("bcmaBcmltCapRtrpWriteSem",
                                             SAL_SEM_BINARY, 0);
    if (rpd->cap_write_done_sem == NULL) {
        SHR_ERR_EXIT(SHR_E_FAIL);
    }

    sal_memset(cb, 0, sizeof(*cb));
    cb->fd = rpd;
    cb->read = capture_rt_replay_read;

    if (cparams->rtrp_text_file) {
        rpd->rparams.text_file = sal_strdup(cparams->rtrp_text_file);
    }
    rpd->rparams.console_output = cparams->rtrp_console_output;
    rpd->rparams.custom_replay_cb = cb;

    SHR_IF_ERR_EXIT
        (bcma_bcmlt_replay_thread_start(unit, &rpd->rparams,
                                        capture_rt_replay_done));

    *rt_rpd = rpd;

exit:
    if (SHR_FUNC_ERR()) {
        capture_rt_replay_data_cleanup(unit, rpd);
    }
    SHR_FUNC_EXIT();
}

static void
capture_info_cleanup(int unit, capture_info_t *cap_info)
{
    if (cap_info) {
        capture_rt_replay_data_cleanup(unit, cap_info->rt_rpd);
        if (cap_info->cap_file) {
            sal_free(cap_info->cap_file);
        }
        if (cap_info->fh) {
            bcma_io_file_close(cap_info->fh);
        }
        if (cap_info->lock) {
            sal_mutex_destroy(cap_info->lock);
        }
        sal_free(cap_info);
    }
}

static int
capture_info_init(int unit, bcma_bcmlt_capture_params_t *cparams,
                  capture_info_t **ci)
{
    capture_info_t *cap_info = NULL;

    SHR_FUNC_ENTER(unit);

    /* Sanity check for capture parameters */
    if (cparams->cap_file == NULL &&
        cparams->rtrp_text_file == NULL && !cparams->rtrp_console_output) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    cap_info = sal_alloc(sizeof(*cap_info), "bcmaBcmltCaptureInfo");
    if (cap_info == NULL) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }
    sal_memset(cap_info, 0, sizeof(*cap_info));

    /* Open the file for capture to write to */
    if (cparams->cap_file) {
        cap_info->cap_file = sal_strdup(cparams->cap_file);
        cap_info->fh = bcma_io_file_open(cparams->cap_file, "w");
        if (cap_info->fh == NULL) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "Failed to open %s\n"), cparams->cap_file));
            SHR_ERR_EXIT(SHR_E_FAIL);
        }
    }

    cap_info->lock = sal_mutex_create("bcmaBcmltCaptureLock");
    if (cap_info->lock == NULL) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "Failed to create mutex for LT capture.\n")));
        SHR_ERR_EXIT(SHR_E_FAIL);
    }

    if (cparams->rtrp_console_output || cparams->rtrp_text_file) {
        SHR_IF_ERR_EXIT
            (capture_rt_replay_data_init(unit, cparams, &cap_info->rt_rpd));
    }

    cap_info->write_only = cparams->write_only;

    *ci = cap_info;

exit:
    if (SHR_FUNC_ERR()) {
        capture_info_cleanup(unit, cap_info);
    }
    SHR_FUNC_EXIT();
}

/*
 * Create capture information and start the capture operation.
 */
static int
capture_start(int unit, bcma_bcmlt_capture_params_t *cparams)
{
    capture_info_t *cap_info = NULL;
    bcmlt_capture_cb_t capture_cb, *cb = &capture_cb;
    bcmlt_opcode_t wofltrs[] = {
        BCMLT_OPCODE_LOOKUP,
        BCMLT_OPCODE_TRAVERSE,
    };

    SHR_FUNC_ENTER(unit);

    if (capture_info[unit] != NULL) {
        SHR_ERR_EXIT(SHR_E_EXISTS);
    }

    SHR_IF_ERR_EXIT
        (capture_info_init(unit, cparams, &cap_info));

    /* Set capture opcode filters */
    if (cap_info->write_only) {
        bcmlt_capture_filter_set(unit, wofltrs, COUNTOF(wofltrs));
    } else {
        bcmlt_capture_filter_set(unit, NULL, 0);
    }

    /* Start capture */
    sal_memset(cb, 0, sizeof(*cb));
    cb->fd = cap_info;
    if (cap_info->rt_rpd) {
        cb->write = capture_blocking_write;
    } else {
        cb->write = capture_nonblocking_write;
    }
    SHR_IF_ERR_EXIT
        (bcmlt_capture_start(unit, cb));

    capture_info[unit] = cap_info;

exit:
    if (SHR_FUNC_ERR()) {
        capture_info_cleanup(unit, cap_info);
    }
    SHR_FUNC_EXIT();
}

/*
 * Stop the capture operation and destroy the capture information.
 */
static int
capture_stop(int unit)
{
    int rv;
    capture_info_t *cap_info = capture_info[unit];

    if (cap_info == NULL) {
        return SHR_E_NOT_FOUND;
    }

    rv = bcmlt_capture_stop(unit);
    bcmlt_capture_filter_set(unit, NULL, 0);

    capture_info_cleanup(unit, cap_info);
    capture_info[unit] = NULL;

    return rv;
}

/*******************************************************************************
 * Public functions
 */

int
bcma_bcmlt_capture_start(int unit, bcma_bcmlt_capture_params_t *cparams)
{
    if (!BCMDRD_UNIT_VALID(unit)) {
        return SHR_E_UNIT;
    }
    if (cparams == NULL) {
        return SHR_E_PARAM;
    }
    return capture_start(unit, cparams);
}

int
bcma_bcmlt_capture_stop(int unit)
{
    if (!BCMDRD_UNIT_VALID(unit)) {
        return SHR_E_UNIT;
    }
    return capture_stop(unit);
}

bool
bcma_bcmlt_capture_started(int unit)
{
    if (!BCMDRD_UNIT_VALID(unit)) {
        return false;
    }
    return capture_info[unit] != NULL;
}

int
bcma_bcmlt_capture_params_get(int unit, bcma_bcmlt_capture_params_t *cparams)
{
    capture_info_t *cap_info;
    realtime_replay_data_t *rpd;

    if (!BCMDRD_UNIT_VALID(unit)) {
        return SHR_E_UNIT;
    }
    if (cparams == NULL) {
        return SHR_E_PARAM;
    }
    cap_info = capture_info[unit];
    if (cap_info == NULL) {
        return SHR_E_NOT_FOUND;
    }
    rpd = cap_info->rt_rpd;

    sal_memset(cparams, 0, sizeof(*cparams));
    cparams->cap_file = cap_info->cap_file;
    if (rpd) {
        cparams->rtrp_text_file = rpd->rparams.text_file;
        cparams->rtrp_console_output = rpd->rparams.console_output;
    }
    cparams->write_only = cap_info->write_only;

    return SHR_E_NONE;
}
