/*! \file bcma_bcmltcmd_ltcapture.c
 *
 * Logical table capture command in CLI.
 *
 * The LT capture command implements the "capture" and "replay" functionality.
 *
 * The CLI command uses the BCMLT capture API callback to log LT operations
 * to a file. Likewise, it uses the BCMLT replay API callback to let the
 * replay engine read the LT operations from a file created by the LT
 * capture command.
 *
 * The captured LT operations can not be decoded by the CLI directly without
 * invoking the BCMLT replay API. If real-time display of the captured LT
 * operation is requested, an extra thread will need to be created to run
 * the BCMLT replay engine in order to support the blocking read callback.
 * This blocking read callback function can wait for the write callback from
 * the BCMLT capture API when an LT operation occurs and read whatever is
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

#include <sal/sal_libc.h>
#include <sal/sal_mutex.h>
#include <sal/sal_sem.h>

#include <shr/shr_debug.h>
#include <shr/shr_thread.h>

#include <bcmdrd/bcmdrd_dev.h>

#include <bcmlt/bcmlt.h>

#include <bcma/io/bcma_io_file.h>

#include <bcma/cli/bcma_cli_parse.h>
#include <bcma/cli/bcma_cli_ctrlc.h>

#include <bcma/bcmlt/bcma_bcmltcmd_ltcapture.h>

#include "bcmltcmd_internal.h"

#ifndef BCMA_BCMLT_CONFIG_DEFAULT_CAPTURE_FILE
#define BCMA_BCMLT_CONFIG_DEFAULT_CAPTURE_FILE "bcmlt.lcap"
#endif

/*! Max filename length of the output file */
#define LTCAPTURE_FILENAME_LEN 128

/*! Capture configuration. */
typedef struct capture_cfg_s {

    /*! The name of the output file for LT captures. */
    char logfile[LTCAPTURE_FILENAME_LEN];

    /*! The name of the output file for VerboseRealTime logs. */
    char vrtfile[LTCAPTURE_FILENAME_LEN];

} capture_cfg_t;

/*
 * Cookie for capture_replay_ctrlc() that contains the parameters
 * used in the replay function.
 */
typedef struct replay_data_s {

    /*! The name of log file to be replayed. */
    char *file;

    /*! The name of the output file for replay commands if not NULL. */
    char *ofile;

    /*!
     * The original capture timing will be referenced in replay if 'timing'
     * is set to true.
     */
    bool timing;

    /*! The replay will output in verbose mode if 'verbose' is set to true. */
    bool verbose;

    /*! The replay will take effect if 'submit' is set to true. */
    bool submit;

    /*!
     * The CLI command generated for replay will be in raw staged result if
     * 'raw' is set to true. Otherwise the CLI friendly command will be
     * generated for replay to run withouth errors. Note that raw=0 is
     * supported only in dry-run mode (submit=0).
     */
    bool raw;

} replay_data_t;

/*******************************************************************************
 * Local definitions
 */

/* Handle of the captured file. */
static bcma_io_file_handle_t lcap_fh = NULL;

/* Handle of the replay commands output file. */
static bcma_io_file_handle_t rp_fh = NULL;

/* Whether to output the replay commands to console. */
static bool rp_verbose = false;

/* Whether to output friendly CLI results instead of raw staged replay data. */
static bool rp_raw = false;

/* Whether the replay is aborted by Ctrl-C signal. */
static bool rp_ctrlc_sig = false;

/* Mutex lock for writing to the captured file. */
static sal_mutex_t lcap_lock = NULL;

/* Thread control when realtime-verbose replay is enabled in capture. */
static shr_thread_t lcap_tc = NULL;

/* Signal the capture thread from the realtime-verbose replay thread. */
static sal_sem_t lcap_sem;

/* Handle of the realtime-verbose replay file. */
static bcma_io_file_handle_t lcap_vrt_fh = NULL;

/* Pointer to the capture-write buffer. */
static void *lcap_buf = NULL;

/* Data size of the capture-write buffer. */
static uint32_t lcap_bytes;

/* The mask of the device units that start the capture. */
static SHR_BITDCLNAME(lcap_umask, BCMDRD_CONFIG_MAX_UNITS) = {0};

/* Capture configuration. */
static capture_cfg_t lcap_cfg;

/*******************************************************************************
 * Private functions
 */

/*
 * Format the field information of the replayed data.
 */
static int
fields_show(shr_pb_t *pb, bcmlt_entry_info_t *bcmlt_ei, bool physical)
{
    int rv;
    int idx;
    bcma_bcmlt_entry_info_t *ei;
    bcma_bcmlt_display_mode_t disp_mode;

    disp_mode.scalar = BCMA_BCMLT_DISP_MODE_HEX;
    disp_mode.array = BCMA_BCMLT_DISP_MODE_HEX;
    disp_mode.array_delim = ',';
    disp_mode.field_delim = ' ';

    /*
     * When the LT replay engine is launched in "real-time verbose"
     * mode, a call to bcma_bcmlt_entry_info_create will block in
     * bcma_bcmlt_tables_search if executing an "lt traverse" command,
     * because the this command uses bcma_bcmlt_tables_search as
     * well. Instead we call bcma_bcmlt_entry_info_create_exact_name
     * to avoid the blocking issue.
     */
    ei = bcma_bcmlt_entry_info_create_exact_name(bcmlt_ei->unit,
                                                 bcmlt_ei->table_name,
                                                 physical, 0, NULL, NULL, 0);
    if (ei == NULL) {
        LOG_ERROR(BSL_LS_APPL_BCMLT,
                  (BSL_META("Failed to create entry information for "
                            "table %s.\n"), bcmlt_ei->table_name));
        return -1;
    }

    /* Free the unused entry handle */
    bcmlt_entry_free(ei->eh);

    /* Set the entry handle from BCMLT */
    ei->eh = bcmlt_ei->entry_hdl;

    rv = bcma_bcmlt_entry_fields_get(ei);

    /* Unset the entry handle to avoid being freed in the destroy function. */
    ei->eh = 0;

    if (rv < 0) {
        bcma_bcmlt_entry_info_destroy(ei);
        return -1;
    }

    /* Format the assigned value for fields. */
    for (idx = 0; idx < ei->num_def_fields; idx++) {
        bcma_bcmlt_field_t *field = &ei->fields[idx];

        if (field->name) {
            bcma_bcmlt_field_format(pb, NULL, field, disp_mode);
        }
    }
    /* Remove the last field delimiter */
    shr_pb_replace_last(pb, 0);

    bcma_bcmlt_entry_info_destroy(ei);
    return 0;
}

/*
 * Format the entry information of the replayed data.
 */
static int
entry_format(shr_pb_t *pb, const char *prefix, bcmlt_entry_info_t *bcmlt_ei,
             int *opcode, bool *logical)
{
    int rv;
    uint32_t attr = 0;
    bool physical;
    char *opcstr;
    int opc;

    rv = bcmlt_entry_attrib_get(bcmlt_ei->entry_hdl, &attr);
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_APPL_BCMLT,
                  (BSL_META("Failed to get table %s entry attribute: "
                            "%s (%d).\n"),
                   bcmlt_ei->table_name, shr_errmsg(rv), rv));
        return -1;
    }

    opcstr = bcmltcmd_opcode_format(bcmlt_ei, &opc, &physical);
    if (opcstr == NULL) {
        return -1;
    }

    if (prefix != NULL) {
        shr_pb_printf(pb, "%s", prefix);
    }
    shr_pb_printf(pb, "%s", bcmlt_ei->table_name);
    if (attr != 0) {
        int num_attrs = 0, match = 0;
        const bcma_cli_parse_enum_t *attrs = bcmltcmd_ent_attr_enum_get();

        shr_pb_printf(pb, " attr=");
        while (attrs[num_attrs].name != NULL) {
            if ((attr & attrs[num_attrs].val) != 0) {
                if (match) {
                    shr_pb_printf(pb, ",");
                }
                shr_pb_printf(pb, "%s", attrs[num_attrs].name);
                match = 1;
            }
            num_attrs++;
        };
    }
    shr_pb_printf(pb, " %s ", opcstr);
    if (fields_show(pb, bcmlt_ei, physical) != 0) {
        return -1;
    }

    if (opcode) {
        *opcode = opc;
    }
    if (logical) {
        *logical = !physical;
    }

    return 0;
}

/*
 * Format the transaction type of the CLI command for the replayed operation.
 */
static int
command_trans_type_format(shr_pb_t *pb, const char *prefix,
                          bcmlt_transaction_info_t *bcmlt_ti)
{
    if (prefix != NULL) {
        shr_pb_printf(pb, "%s", prefix);
    }

    /* Format transaction type */
    if (bcmlt_ti) {
        shr_pb_printf(pb, "tr=1 tt=%s",
                      bcmltcmd_trans_type_format(bcmlt_ti->type));
    } else {
        shr_pb_printf(pb, "tr=0");
    }

    /* Only synchronous mode is supported in the replay. */
    shr_pb_printf(pb, " a=0");

    return 0;
}

/*
 * Format the CLI command of the replayed operation.
 */
static int
command_format(shr_pb_t * pb, const char *prefix, bcmlt_entry_info_t *bcmlt_ei)
{
    int rv;
    bcmlt_unified_opcode_t uopc;

    rv = bcmlt_entry_oper_get(bcmlt_ei->entry_hdl, &uopc);
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_APPL_BCMLT,
                  (BSL_META("Failed to get table %s entry op code: "
                            "%s (%d).\n"),
                   bcmlt_ei->table_name, shr_errmsg(rv), rv));
        return -1;
    }
    if (prefix != NULL) {
        shr_pb_printf(pb, "%s", prefix);
    }
    shr_pb_printf(pb, "%d:%s", bcmlt_ei->unit, uopc.pt ? "pt" : "lt");

    return 0;
}

/*
 * Format the entry replayed data to human-readable CLI command.
 */
static void
entry_commit_replay(bcmlt_entry_handle_t eh, const char *prefix,
                    bool verbose, bcma_io_file_handle_t fh, bool raw)
{
    int rv;
    bcmlt_entry_info_t bcmlt_ei;
    shr_pb_t *pb;
    int opcode;
    bool logical;
    bool cmd_out = true;

    if (eh == BCMLT_INVALID_HDL) {
        LOG_ERROR(BSL_LS_APPL_BCMLT,
                  (BSL_META("bcmlt_entry_handle_t eh is NULL.\n")));
        return;
    }

    rv = bcmlt_entry_info_get(eh, &bcmlt_ei);
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_APPL_BCMLT,
                  (BSL_META("Failed to get entry info: %s (%d).\n"),
                   shr_errmsg(rv), rv));
        return;
    }

    pb = shr_pb_create();
    if (pb == NULL) {
        LOG_ERROR(BSL_LS_APPL_BCMLT,
                  (BSL_META("Failed to create print buffer object.\n")));
        return;
    }

    rv += command_format(pb, prefix, &bcmlt_ei);
    rv += command_trans_type_format(pb, " ", NULL);
    rv += entry_format(pb, " ", &bcmlt_ei, &opcode, &logical);
    shr_pb_printf(pb, "\n");

    if (!raw) {
        if (logical && opcode == BCMLT_OPCODE_TRAVERSE) {
            uint32_t fcnt;
            rv = bcmlt_entry_field_count(bcmlt_ei.entry_hdl, &fcnt);
            if (SHR_SUCCESS(rv) && fcnt != 0) {
                /* Skip the traverse opcode with fields specified */
                cmd_out = false;
            }
        }
    }

    if (SHR_SUCCESS(rv)) {
        if (cmd_out) {
            if (verbose) {
                cli_out("%s", shr_pb_str(pb));
            }
            if (fh) {
                bcma_io_file_puts(fh, shr_pb_str(pb));
            }
        }
    } else {
        LOG_ERROR(BSL_LS_APPL_BCMLT,
                  (BSL_META("Failed to format the replayed command for "
                            "entry commit.\n")));
    }
    shr_pb_destroy(pb);

    /* Check for entry status */
    rv = bcmlt_ei.status;
    if (SHR_FAILURE(rv)) {
        if (rv == SHR_E_NOT_FOUND &&
            logical && opcode == BCMLT_OPCODE_TRAVERSE) {
            /* End of traversing */
            return;
        }
        LOG_ERROR(BSL_LS_APPL_BCMLT,
                  (BSL_META("Invalid entry status for %s: %s (%d).\n"),
                   bcmlt_ei.table_name, shr_errmsg(rv), rv));
    }
}

/*
 * Format the transaction replayed data to human-readable CLI command.
 */
static void
transaction_commit_replay(bcmlt_transaction_hdl_t th, const char *prefix,
                          bool verbose, bcma_io_file_handle_t fh)
{
    int rv;
    uint32_t idx;
    bcmlt_transaction_info_t trans_info, *ti = &trans_info;
    shr_pb_t *pb;

    rv = bcmlt_transaction_info_get(th, ti);
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_APPL_BCMLT,
                  (BSL_META("Failed to get transaction info: %s (%d).\n"),
                   shr_errmsg(rv), rv));
        return;
    }

    pb = shr_pb_create();
    if (pb == NULL) {
        LOG_ERROR(BSL_LS_APPL_BCMLT,
                  (BSL_META("Failed to create print buffer object.\n")));
        return;
    }

    for (idx = 0; idx < ti->num_entries; idx++) {
        bcmlt_entry_info_t entry_info;

        rv = bcmlt_transaction_entry_num_get(th, idx, &entry_info);
        if (SHR_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_APPL_BCMLT,
                      (BSL_META("Failed to get the #%d entry info "
                                "in transaction: %s (%d).\n"),
                       idx + 1, shr_errmsg(rv), rv));
            if (ti->type == BCMLT_TRANS_TYPE_BATCH) {
                continue;
            } else {
                /* All or none operation for atomic transaction type. */
                break;
            }
        }
        /*
         * Except for transactions containing only a single entry,
         * always format the transaction mode command in continuous
         * command mode to support a transaction with large amount of entries
         * that might exceed the CLI input size using only one single line
         * command to commit the transaction.
         */
        command_format(pb, prefix, &entry_info);
        if (idx == 0) {
            /*
             * Only need to format the transaction parameters once
             * in continuous mode.
             */
            command_trans_type_format(pb, " ", ti);
        }
        /* Ends the continuous mode at the last entry. */
        shr_pb_printf(pb, " ccm=%d", idx == (ti->num_entries -1) ? 0 : 1);
        entry_format(pb, " ", &entry_info, NULL, NULL);
        shr_pb_printf(pb, "\n");
    }
    if (verbose) {
        cli_out("%s", shr_pb_str(pb));
    }
    if (fh) {
        bcma_io_file_puts(fh, shr_pb_str(pb));
    }

    shr_pb_destroy(pb);
}

/*
 * Callback function from bcmlt_capture_replay() that carries data of an
 * entry or transaction which will be used to be format as human-readable
 * CLI commands in the application.
 */
static void
replay_action(int unit, bcmlt_object_hdl_t *object)
{
    if (object == NULL) {
        LOG_ERROR(BSL_LS_APPL_BCMLT,
                  (BSL_META("bcmlt_object_hdl_t object is NULL.\n")));
        return;
    }

    if (object->entry) {
        entry_commit_replay(object->hdl.entry, NULL, rp_verbose, rp_fh, rp_raw);
    } else {
        transaction_commit_replay(object->hdl.trans, NULL, rp_verbose, rp_fh);
    }
}

/*
 * Callback function from bcmlt_capture_replay() to read data for replay.
 */
static uint32_t
replay_read(void *fd, void *buffer, uint32_t nbyte)
{
    int rb;

    /*
     * Return 0 to stop reading and return from bcmlt_capture_replay gracefully
     * if Ctrl-C signal is sent during the replay command.
     */
    if (bcma_cli_ctrlc_break()) {
        rp_ctrlc_sig = true;
        return 0;
    }
    rb = bcma_io_file_read(fd, buffer, 1, nbyte);
    return (rb < 0) ? 0 : rb;
}

/*
 * Callback function from bcmlt_capture_replay() to read data for replay.
 * This is called from a separate thread when real-time verbose replay is
 * enabled in capture.
 */
static uint32_t
replay_realtime_read(void *fd, void *buffer, uint32_t nbyte)
{
    do {
        if (lcap_buf && lcap_bytes == nbyte) {
            sal_memcpy(buffer, lcap_buf, nbyte);
            lcap_buf = NULL;
            lcap_bytes = 0;
            /* Signal capture_write() that the read is completed. */
            sal_sem_give(lcap_sem);
            return nbyte;
        }
        /*
         * Wake up periodically to avoid the condition that capture_write()
         * is halted by waiting for lcap_sem when shr_thread_wake in
         * capture_write() is called before the sleep here.
         */
        shr_thread_sleep(lcap_tc, 200000);
        if (shr_thread_stopping(lcap_tc)) {
            sal_sem_give(lcap_sem);
            break;
        }
    } while (true);
    return 0;
}

/*
 * Setup the variables used in replay and call the BCMLT replay API.
 */
static int
do_replay(bcmlt_replay_cb_t *cb, bcma_io_file_handle_t ofh, replay_data_t *rpd)
{
    replay_action_f *replay_act = NULL;
    bool timing;
    bool view_only;

    if (rpd == NULL) {
        return SHR_E_PARAM;
    }
    timing = rpd->timing;
    view_only = !rpd->submit;

    rp_fh = ofh;
    rp_verbose = rpd->verbose;
    rp_raw = rpd->raw;

    /*
     * If the replay option is in verbose mode to CLI or an output file, the
     * replay data will need to be formatted to human-readable CLI commands.
     */
    if (rp_verbose || rp_fh) {
        replay_act = replay_action;
    }

    return bcmlt_capture_replay(timing, replay_act, view_only, cb);
}

/*
 * A real-time replay thread to read captured data from the main thread.
 * The callback read function will be blocked until capture_write()
 * is called back.
 */
static void
capture_replay_read_thread(shr_thread_t tc, void *arg)
{
    int rv;
    bcmlt_replay_cb_t replay_cb, *cb = &replay_cb;
    replay_data_t replay_data, *rp = &replay_data;

    sal_memset(rp, 0, sizeof(*rp));
    rp->timing = false;
    rp->submit = false;
    rp->verbose = (bool)(uintptr_t)arg;
    rp->raw = false;

    sal_memset(cb, 0, sizeof(*cb));
    cb->fd = tc;
    cb->read = replay_realtime_read;

    rv = do_replay(cb, lcap_vrt_fh, rp);
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_APPL_BCMLT,
                  (BSL_META("Failed to replay the captured data in realtime: "
                            "%s (%d)\n"), shr_errmsg(rv), rv));
    }
    sal_sem_give(lcap_sem);
}

/*
 * Callback function from bcmlt_capture_start() to write the captured data.
 */
static uint32_t
capture_write(void *fd, void *buffer, uint32_t nbyte)
{
    int wb = 0;

    sal_mutex_take(lcap_lock, SAL_MUTEX_FOREVER);
    /* Realtime replay the captured data in verbose mode */
    if (lcap_tc && !shr_thread_done(lcap_tc)) {
        lcap_buf = buffer;
        lcap_bytes = nbyte;
        /* Wake up the realtime read thread. */
        shr_thread_wake(lcap_tc);
        /* Wait for the read completion done from the read thread. */
        sal_sem_take(lcap_sem, SAL_SEM_FOREVER);
        wb = (lcap_bytes == 0) ? (int)nbyte : -1;
    }
    /* Log the captured data to a file */
    if (lcap_fh) {
        wb = bcma_io_file_write(fd, buffer, 1, nbyte);
    }
    sal_mutex_give(lcap_lock);

    return (wb < 0) ? 0 : wb;
}

/*
 * Clean up capture resources.
 */
static void
capture_cleanup(void)
{
    int rv;

    if (lcap_fh) {
        rv= bcma_io_file_close(lcap_fh);
        if (rv == 0) {
            lcap_fh = NULL;
        } else {
            LOG_ERROR(BSL_LS_APPL_BCMLT,
                      (BSL_META("Failed to close log file (%d).\n"), rv));
        }
    }

    if (lcap_tc) {
        rv = shr_thread_stop(lcap_tc, SHR_THREAD_FOREVER);
        if (SHR_SUCCESS(rv)) {
            lcap_tc = NULL;
        } else {
            LOG_ERROR(BSL_LS_APPL_BCMLT,
                      (BSL_META("Failed to stop thread (%d).\n"), rv));
        }
    }

    if (lcap_sem) {
        sal_sem_destroy(lcap_sem);
        lcap_sem = NULL;
    }

    if (lcap_lock) {
        sal_mutex_destroy(lcap_lock);
        lcap_lock = NULL;
    }

    if (lcap_vrt_fh) {
        rv= bcma_io_file_close(lcap_vrt_fh);
        if (rv == 0) {
            lcap_vrt_fh = NULL;
        } else {
            LOG_ERROR(BSL_LS_APPL_BCMLT,
                      (BSL_META("Failed to close realtime verbose file (%d)."
                                "\n"), rv));
        }
    }

    SHR_BITCLR_RANGE(lcap_umask, 0, BCMDRD_CONFIG_MAX_UNITS);
}

/*
 * Start the capture operation. If verbose_realtime argument is true,
 * the captured data will be replayed in human-readable format in verbose mode.
 * Otherwise the captured data will be written to the specified logfile.
 */
static int
capture_start(const char *logfile, bool verbose_realtime, const char *vrtfile,
              bool write_only)
{
    int rv;
    int unit;
    bcmlt_capture_cb_t capture_cb, *cb = &capture_cb;
    bcmlt_opcode_t wofltrs[] = {
        BCMLT_OPCODE_LOOKUP,
        BCMLT_OPCODE_TRAVERSE,
    };

    if (lcap_fh != NULL || lcap_tc != NULL) {
        return SHR_E_EXISTS;
    }
    if (lcap_vrt_fh != NULL) {
        return SHR_E_BUSY;
    }

    if (sal_strcmp(vrtfile, "-") != 0) {
        lcap_vrt_fh = bcma_io_file_open(vrtfile, "w");
        if (lcap_vrt_fh == NULL) {
            LOG_ERROR(BSL_LS_APPL_BCMLT,
                      (BSL_META("Failed to open %s\n"), vrtfile));
            return SHR_E_FAIL;
        }
    }
    /*
     * If verbose_realtime is true, the captured data will be
     * realtime-replayed in verbose mode.
     */
    if (verbose_realtime || lcap_vrt_fh) {
        lcap_tc = shr_thread_start("bcmaLtCap", -1,
                                   capture_replay_read_thread,
                                   (void *)(uintptr_t)verbose_realtime);
        if (lcap_tc == NULL) {
            LOG_ERROR(BSL_LS_APPL_BCMLT,
                      (BSL_META("Failed to start realtime-replay thread\n")));
            return SHR_E_FAIL;
        }
        lcap_sem = sal_sem_create("bcmaLtCap", SAL_SEM_BINARY, 0);
        if (lcap_sem == NULL) {
            LOG_ERROR(BSL_LS_APPL_BCMLT,
                      (BSL_META("Failed to create semaphore\n")));
            return SHR_E_FAIL;
        }
        lcap_buf = NULL;
        lcap_bytes = 0;
    }
    /* The file logging will be disabled when set to "-" */
    if (sal_strcmp(logfile, "-") == 0) {
        if (!verbose_realtime && lcap_vrt_fh == NULL) {
            LOG_ERROR(BSL_LS_APPL_BCMLT,
                      (BSL_META("Log file can only be disabled "
                                "if VerboseRealTime=1 or "
                                "VerboseRealTimeLogFile != '-'\n")));
            return SHR_E_PARAM;
        }
    } else {
        lcap_fh = bcma_io_file_open(logfile, "w");
        if (lcap_fh == NULL) {
            LOG_ERROR(BSL_LS_APPL_BCMLT,
                      (BSL_META("Failed to open %s\n"), logfile));
            return SHR_E_FAIL;
        }
    }

    if (lcap_lock != NULL) {
        capture_cleanup();
        return SHR_E_INTERNAL;
    }
    lcap_lock = sal_mutex_create("bcmaBcmltCmdLtcapture");
    if (lcap_lock == NULL) {
        LOG_ERROR(BSL_LS_APPL_BCMLT,
                  (BSL_META("Failed to create mutex for Ltcapture.\n")));
        capture_cleanup();
        return SHR_E_FAIL;
    }

    sal_memset(cb, 0, sizeof(*cb));
    cb->fd = lcap_fh;
    cb->write = capture_write;

    for (unit = 0; unit < BCMDRD_CONFIG_MAX_UNITS; unit++) {
        if (!bcmdrd_dev_exists(unit)) {
            continue;
        }

        /* Set capture opcode filters */
        if (write_only) {
            bcmlt_capture_filter_set(unit, wofltrs, COUNTOF(wofltrs));
        } else {
            bcmlt_capture_filter_set(unit, NULL, 0);
        }

        rv = bcmlt_capture_start(unit, cb);
        if (SHR_SUCCESS(rv)) {
            SHR_BITSET(lcap_umask, unit);
        } else {
            LOG_ERROR(BSL_LS_APPL_BCMLT,
                      (BSL_META("Failed to start capture on unit %d: "
                                "%s (%d)\n"),
                       unit, shr_errmsg(rv), rv));
        }
    }
    if (SHR_BITNULL_RANGE(lcap_umask, 0, BCMDRD_CONFIG_MAX_UNITS)) {
        capture_cleanup();
        return SHR_E_FAIL;
    }

    return SHR_E_NONE;
}

/*
 * Stop the capture operation.
 */
static int
capture_stop(void)
{
    int rv = SHR_E_NONE;
    int unit, bcmlt_rv;

    if (lcap_fh == NULL && lcap_tc == NULL) {
        return SHR_E_NOT_FOUND;
    }

    SHR_BIT_ITER(lcap_umask, BCMDRD_CONFIG_MAX_UNITS, unit) {
        bcmlt_rv = bcmlt_capture_stop(unit);
        if (SHR_FAILURE(bcmlt_rv)) {
            LOG_ERROR(BSL_LS_APPL_BCMLT,
                      (BSL_META("Failed to stop capture on unit %d: "
                                "%s (%d)\n"),
                       unit, shr_errmsg(bcmlt_rv), bcmlt_rv));
            rv = SHR_E_FAIL;
        }
        bcmlt_capture_filter_set(unit, NULL, 0);
    }

    capture_cleanup();

    return rv;
}

/*
 * Replay the captured data from a captured file.
 */
static int
capture_replay(replay_data_t *rpd)
{
    int rv = SHR_E_NONE;
    bcma_io_file_handle_t fh;
    bcma_io_file_handle_t ofh = NULL;
    bcmlt_replay_cb_t replay_cb, *cb = &replay_cb;

    /* Check whether capture in in progress. */
    if (lcap_fh != NULL) {
        return SHR_E_BUSY;
    }

    /* Open the captured file. */
    fh = bcma_io_file_open(rpd->file, "r");
    if (fh == NULL) {
        LOG_ERROR(BSL_LS_APPL_BCMLT,
                  (BSL_META("Failed to open log file %s to replay.\n"),
                   rpd->file));
        return SHR_E_FAIL;
    }

    if (rpd->ofile) {
        /* Open the file to write the replay commands to. */
        ofh = bcma_io_file_open(rpd->ofile, "w");
        if (ofh == NULL) {
            LOG_ERROR(BSL_LS_APPL_BCMLT,
                      (BSL_META("Failed to open file %s to write to.\n"),
                       rpd->ofile));
            bcma_io_file_close(fh);
            return SHR_E_FAIL;
        }
    }

    cli_out("Replay options: LogFile=%s Timing=%d Verbose=%d Submit=%d",
            rpd->file, rpd->timing, rpd->verbose, rpd->submit);
    if (rpd->submit) {
        cli_out("\n");
    } else {
        cli_out(" Raw=%d\n", rpd->raw);
    }
    if (rpd->ofile) {
        cli_out("%16cOutputFile=%s\n", ' ', rpd->ofile);
    }

    cb->fd = fh;
    cb->read = replay_read;

    rv = do_replay(cb, ofh, rpd);
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_APPL_BCMLT,
                  (BSL_META("Failed to replay the capture log file %s: "
                            "%s (%d)\n"),
                   rpd->file, shr_errmsg(rv), rv));
        rv = SHR_E_FAIL;
    }

    bcma_io_file_close(fh);
    if (ofh) {
        bcma_io_file_close(ofh);
    }

    return rv;
}

/*
 * Ctrl-C wrapper function for replaying captured data.
 */
static int
capture_replay_ctrlc(void *data)
{
    int rv;

    rp_ctrlc_sig = false;

    rv = capture_replay(data);
    if (SHR_FAILURE(rv)) {
        if (rv == SHR_E_BUSY) {
            cli_out("%sCapture must be stopped before replay.\n",
                    BCMA_CLI_CONFIG_ERROR_STR);
        } else {
            cli_out("%sFailed to replay capture.\n",
                    BCMA_CLI_CONFIG_ERROR_STR);
        }
        return BCMA_CLI_CMD_FAIL;
    }
    return rp_ctrlc_sig ? BCMA_CLI_CMD_INTR : BCMA_CLI_CMD_OK;
}

/*
 * Handler for ltcapture start sub-command.
 */
static int
ltcapture_start(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    int rv;
    bcma_cli_parse_table_t pt;
    char *logfile = BCMA_BCMLT_CONFIG_DEFAULT_CAPTURE_FILE;
    char *vrtfile = "-";
    int vrt = 0;
    int write_only = 0;

    bcma_cli_parse_table_init(cli, &pt);
    bcma_cli_parse_table_add(&pt, "LogFile", "str", &logfile, NULL);
    bcma_cli_parse_table_add(&pt, "VerboseRealTime", "bool", &vrt, NULL);
    bcma_cli_parse_table_add(&pt, "VerboseRealTimeLogFile", "str",
                             &vrtfile, NULL);
    bcma_cli_parse_table_add(&pt, "WriteOnly", "bool", &write_only, NULL);

    if (bcma_cli_parse_table_do_args(&pt, args) < 0 ||
        sal_strlen(logfile) == 0) {
        bcma_cli_parse_table_done(&pt);
        return BCMA_CLI_CMD_USAGE;
    }

    sal_strncpy(lcap_cfg.logfile, logfile, sizeof(lcap_cfg.logfile) - 1);
    sal_strncpy(lcap_cfg.vrtfile, vrtfile, sizeof(lcap_cfg.vrtfile) - 1);

    rv = capture_start(logfile, vrt, vrtfile, write_only);
    if (SHR_FAILURE(rv)) {
        if (rv == SHR_E_EXISTS) {
            cli_out("%sCapture is already started.\n",
                    BCMA_CLI_CONFIG_ERROR_STR);
        } else {
            cli_out("%sFailed to start capture.\n", BCMA_CLI_CONFIG_ERROR_STR);
        }
    }

    bcma_cli_parse_table_done(&pt);

    return rv == SHR_E_NONE ? BCMA_CLI_CMD_OK : BCMA_CLI_CMD_FAIL;
}

/*
 * Handler for ltcapture stop sub-command.
 */
static int
ltcapture_stop(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    int rv;

    rv = capture_stop();
    if (SHR_FAILURE(rv)) {
        if (rv == SHR_E_NOT_FOUND) {
            cli_out("%sCapture is already stopped.\n",
                    BCMA_CLI_CONFIG_ERROR_STR);
        } else {
            cli_out("%sFailed to stop capture.\n", BCMA_CLI_CONFIG_ERROR_STR);
        }
    }

    return rv == SHR_E_NONE ? BCMA_CLI_CMD_OK : BCMA_CLI_CMD_FAIL;
}

/*
 * Handler for ltcapture replay sub-command.
 */
static int
ltcapture_replay(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    int rv = BCMA_CLI_CMD_OK;
    bcma_cli_parse_table_t pt;
    char *logfile = BCMA_BCMLT_CONFIG_DEFAULT_CAPTURE_FILE;
    char *ofile = NULL;
    int timing = 0, verbose = 1, submit = 0, raw = 0;
    replay_data_t replay_data, *rp = &replay_data;

    if (lcap_fh != NULL) {
        cli_out("%sCapture must be stopped before replay.\n",
                BCMA_CLI_CONFIG_ERROR_STR);
        return BCMA_CLI_CMD_FAIL;
    }

    bcma_cli_parse_table_init(cli, &pt);
    bcma_cli_parse_table_add(&pt, "LogFile", "str", &logfile, NULL);
    bcma_cli_parse_table_add(&pt, "Timing", "bool", &timing, NULL);
    bcma_cli_parse_table_add(&pt, "Verbose", "bool", &verbose, NULL);
    bcma_cli_parse_table_add(&pt, "Submit", "bool", &submit, NULL);
    bcma_cli_parse_table_add(&pt, "OutputFile", "str", &ofile, NULL);
    bcma_cli_parse_table_add(&pt, "Raw", "bool", &raw, NULL);

    if (bcma_cli_parse_table_do_args(&pt, args) < 0 ||
        sal_strlen(logfile) == 0) {
        bcma_cli_parse_table_done(&pt);
        return BCMA_CLI_CMD_USAGE;
    }

    /* Only support raw staged output when the replay is truly submitted */
    if (submit) {
        raw = 1;
    }

    sal_memset(rp, 0, sizeof(*rp));
    rp->file = logfile;
    rp->timing = timing;
    rp->verbose = verbose;
    rp->submit = submit;
    rp->ofile = ofile;
    rp->raw = raw;

    rv = bcma_cli_ctrlc_exec(cli, capture_replay_ctrlc, rp, 1);
    if (rv == BCMA_CLI_CMD_INTR) {
        cli_out("%sReplay aborted.\n",
                BCMA_CLI_CONFIG_ERROR_STR);
    }

    bcma_cli_parse_table_done(&pt);

    return rv;
}

/*
 * Handler for ltcapture status sub-command.
 */
static int
ltcapture_status(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    if (!lcap_fh && !lcap_tc) {
        cli_out("Stopped capturing.\n");
        return BCMA_CLI_CMD_OK;
    }

    if (lcap_fh != NULL) {
        cli_out("Started capturing to %s.\n", lcap_cfg.logfile);
    }
    if (lcap_tc != NULL) {
        cli_out("Started VerboseRealTime capturing");
        if (lcap_vrt_fh != NULL) {
            cli_out(" to %s", lcap_cfg.vrtfile);
        }
        cli_out(".\n");
    }

    return BCMA_CLI_CMD_OK;
}

/*******************************************************************************
 * Public functions
 */

int
bcma_bcmltcmd_ltcapture_cleanup(bcma_cli_t *cli)
{
    capture_stop();

    bcma_bcmlt_tables_search_cleanup();

    return 0;
}

int
bcma_bcmltcmd_ltcapture(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    const char *arg;

    if (cli == NULL || args == NULL) {
        return BCMA_CLI_CMD_FAIL;
    }

    if ((arg = BCMA_CLI_ARG_GET(args)) == NULL) {
        return BCMA_CLI_CMD_USAGE;
    }

    bcma_bcmlt_tables_search_init();

    if (sal_strcmp(arg, "start") == 0) {
        return ltcapture_start(cli, args);
    } else if (sal_strcmp(arg, "stop") == 0) {
        return ltcapture_stop(cli, args);
    } else if (sal_strcmp(arg, "replay") == 0) {
        return ltcapture_replay(cli, args);
    } else if (sal_strcmp(arg, "status") == 0) {
        return ltcapture_status(cli, args);
    }

    return BCMA_CLI_CMD_USAGE;
}
