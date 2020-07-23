/*! \file bcma_bcmbdcmd_tsfifo.c
 *
 * CLI 'tsfifo' command handler.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_libc.h>
#include <sal/sal_alloc.h>

#include <bsl/bsl.h>

#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <shr/shr_pb.h>

#include <bcmbd/bcmbd_tsfifo.h>

#include <bcma/io/bcma_io_file.h>
#include <bcma/cli/bcma_cli_parse.h>
#include <bcma/cli/bcma_cli_var.h>
#include <bcma/cli/bcma_cli_ctrlc.h>
#include <bcma/bcmbd/bcma_bcmbd_tsfifo.h>
#include <bcma/bcmbd/bcma_bcmbdcmd_tsfifo.h>

#define BSL_LOG_MODULE BSL_LS_APPL_TIMESYNC

#ifndef BCMA_BCMBD_TSFIFO_DEFAULT_CAPTURE_FILE
#define BCMA_BCMBD_TSFIFO_DEFAULT_CAPTURE_FILE "tsfifo.cap"
#endif

/*******************************************************************************
 * Local definitions
 */
/*
 * Cookie for capture_dump_ctrlc() that contains the parameters
 * used in the dump function.
 */
typedef struct dump_data_s {
    int unit;

    /*! The name of log file to be replayed. */
    char *file;

    /* Dump data in comma-separated format. */
    bool raw;
} dump_data_t;

/* Handle of the captured file. */
static bcma_io_file_handle_t lcap_fh = NULL;

/* Whether the data dump is aborted by Ctrl-C signal. */
static bool dp_ctrlc_sig = false;

/* Use script-friendly output format (comma-separated data only). */
static bool dp_raw = false;

/* Add banner before script-friendly output format (comma-separated data only). */
static bool dp_raw_banner = false;

/*******************************************************************************
 * Private functions
 */
/*
 * Callback function from bcmbd_tsfifo_capture_start()
 * to write the captured data.
 */
static uint32_t
capture_write(void *fd, void *buffer, uint32_t nbyte)
{
    int wb = 0;

    /* Log the captured data to a file */
    if (lcap_fh) {
        wb = bcma_io_file_write(fd, buffer, 1, nbyte);
    }

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
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META("Failed to close log file (%d).\n"), rv));
        }
    }
}

/*
 * Start the capture operation.
 * The captured data will be written to the specified logfile.
 */
static int
capture_start(int unit, const char *logfile)
{
    int rv = SHR_E_NONE;
    bcmbd_tsfifo_capture_cb_t capture_cb, *cb = &capture_cb;

    if (lcap_fh != NULL) {
        return SHR_E_EXISTS;
    }
    lcap_fh = bcma_io_file_open(logfile, "w");
    if (lcap_fh == NULL) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META("Failed to open %s\n"), logfile));
        return SHR_E_FAIL;
    }

    sal_memset(cb, 0, sizeof(*cb));
    cb->fd = lcap_fh;
    cb->write = capture_write;

    rv = bcmbd_tsfifo_capture_start(unit, cb);
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Failed to start capture %s (%d)\n"),
                  shr_errmsg(rv), rv));
        capture_cleanup();
        rv = SHR_E_FAIL;
    }
    return rv;
}

/*
 * Stop the capture operation.
 */
static int
capture_stop(int unit)
{
    int rv = SHR_E_NONE;

    if (lcap_fh == NULL) {
        return SHR_E_NOT_FOUND;
    }

    rv = bcmbd_tsfifo_capture_stop(unit);
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Failed to stop capture %s (%d)\n"),
                   shr_errmsg(rv), rv));
        rv = SHR_E_FAIL;
    }

    capture_cleanup();

    return rv;
}

/*
 * Callback function from bcmbd_tsfifo_capture_dump() that carries data of
 * an entry which will be used to be format as human-readable
 * CLI commands in the application.
 */
static void
dump_action(int unit, bcmbd_tsfifo_capture_info_t *cap_info)
{
    shr_pb_t *pb;

    if (cap_info == NULL) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META("bcmbd_tsfifo_capture_info_t entry is NULL.\n")));
        return;
    }

    pb = shr_pb_create();

    if (dp_raw) {
        if (dp_raw_banner) {
            shr_pb_printf(pb, "Unit, Event-ID, offset, TS0, TS1\n");
            dp_raw_banner = false;
        }
        shr_pb_printf(pb,
                      "%d, %d, %d, %"PRIu64".%03"PRIu64" ,%"
                      PRIu64".%03"PRIu64"\n",
                      unit, cap_info->evt_id, cap_info->offset,
                      cap_info->ts0 / 1000, cap_info->ts0 % 1000,
                      cap_info->ts1 / 1000, cap_info->ts1 % 1000);
    } else {
        shr_pb_printf(pb, "%d: ", unit);
        shr_pb_printf(pb, "Event-ID=%d ", cap_info->evt_id);
        shr_pb_printf(pb, "offset=%d ", cap_info->offset);
        shr_pb_printf(pb, "TS0=%"PRIu64".%03"PRIu64" ",
                      cap_info->ts0 / 1000, cap_info->ts0 % 1000);
        shr_pb_printf(pb, "TS1=%"PRIu64".%03"PRIu64"\n",
                      cap_info->ts1 / 1000, cap_info->ts1 % 1000);
    }

    cli_out("%s", shr_pb_str(pb));
    shr_pb_destroy(pb);
}

/*
 * Callback function from bcmbd_tsfifo_capture_dump() to read data for dump.
 */
static uint32_t
dump_read(void *fd, void *buffer, uint32_t nbyte)
{
    int rb;

    /*
     * Return 0 to stop reading and return from bcmbd_tsfifo_capture_dump
     * gracefully if Ctrl-C signal is sent during the dump command.
     */
    if (bcma_cli_ctrlc_break()) {
        dp_ctrlc_sig = true;
        return 0;
    }

    rb = bcma_io_file_read(fd, buffer, 1, nbyte);
    return (rb < 0) ? 0 : rb;
}

/*
 * Setup the variables used in dump and call the BCMBD tsfifo dump API.
 */
static int
do_dump(bcmbd_tsfifo_dump_cb_t *cb, dump_data_t *dpd)
{
    bcmbd_tsfifo_dump_action_f *dump_act = NULL;

    dump_act = dump_action;
    return bcmbd_tsfifo_capture_dump(dpd->unit, dump_act, cb);

}

/*
 * Dump the captured data from a captured file.
 */
static int
capture_dump(dump_data_t *dpd)
{
    int rv = SHR_E_NONE;
    bcma_io_file_handle_t fh;
    bcmbd_tsfifo_dump_cb_t dump_cb, *cb = &dump_cb;

    /* Check whether capture in in progress. */
    if (lcap_fh != NULL) {
        return SHR_E_BUSY;
    }

    /* Open the captured file. */
    fh = bcma_io_file_open(dpd->file, "r");
    if (fh == NULL) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META("Failed to open log file %s to replay.\n"),
                  dpd->file));
        return SHR_E_FAIL;
    }

    cb->fd = fh;
    cb->read = dump_read;
    dp_raw = dpd->raw;
    if (dp_raw) {
        dp_raw_banner = true;
    }
    rv = do_dump(cb, dpd);
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META("Failed to replay the capture log file %s: "
                            "%s (%d)\n"),
                   dpd->file, shr_errmsg(rv), rv));
        rv = SHR_E_FAIL;
    }

    bcma_io_file_close(fh);
    return rv;
}

/*
 * Ctrl-C wrapper function for dumping captured data.
 */
static int
capture_dump_ctrlc(void *data)
{
    int rv;

    dp_ctrlc_sig = false;

    rv = capture_dump(data);
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
    return dp_ctrlc_sig ? BCMA_CLI_CMD_INTR : BCMA_CLI_CMD_OK;
}

/*******************************************************************************
 * Sub-command handlers
 */

static int
tsfifo_capture_start(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    int rv;
    bcma_cli_parse_table_t pt;
    char *logfile = BCMA_BCMBD_TSFIFO_DEFAULT_CAPTURE_FILE;
    int unit = cli->cmd_unit;

    bcma_cli_parse_table_init(cli, &pt);
    bcma_cli_parse_table_add(&pt, "LogFile", "str", &logfile, NULL);

    if (bcma_cli_parse_table_do_args(&pt, args) < 0 ||
        sal_strlen(logfile) == 0) {
        bcma_cli_parse_table_done(&pt);
        return BCMA_CLI_CMD_USAGE;
    }

    rv = capture_start(unit, logfile);
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
    return SHR_E_NONE;
}

static int
tsfifo_capture_stop(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    int unit = cli->cmd_unit;
    int rv;

    rv = capture_stop(unit);
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

static int
tsfifo_capture_dump(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    int rv;
    bcma_cli_parse_table_t pt;
    char *logfile = BCMA_BCMBD_TSFIFO_DEFAULT_CAPTURE_FILE;
    int unit = cli->cmd_unit;
    int raw = 0;
    dump_data_t dump_data, *dp = &dump_data;

    if (lcap_fh != NULL) {
        cli_out("%sCapture must be stopped before dump.\n",
                BCMA_CLI_CONFIG_ERROR_STR);
        return BCMA_CLI_CMD_FAIL;
    }

    bcma_cli_parse_table_init(cli, &pt);
    bcma_cli_parse_table_add(&pt, "LogFile", "str", &logfile, NULL);
    bcma_cli_parse_table_add(&pt, "Raw", "bool", &raw, NULL);

    if (bcma_cli_parse_table_do_args(&pt, args) < 0 ||
        sal_strlen(logfile) == 0) {
        bcma_cli_parse_table_done(&pt);
        return BCMA_CLI_CMD_USAGE;
    }

    sal_memset(dp, 0, sizeof(*dp));
    dp->unit = unit;
    dp->file = logfile;
    dp->raw = raw;

    rv = bcma_cli_ctrlc_exec(cli, capture_dump_ctrlc, dp, 1);
    if (rv == BCMA_CLI_CMD_INTR) {
        cli_out("%sDump aborted.\n",
                BCMA_CLI_CONFIG_ERROR_STR);
    }

    bcma_cli_parse_table_done(&pt);
    return SHR_E_NONE;
}

/*******************************************************************************
 * TSFIFO command handler
 */

int
bcma_bcmbdcmd_tsfifo(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    const char *arg;

    if (cli == NULL) {
        return BCMA_CLI_CMD_FAIL;
    }

    arg = BCMA_CLI_ARG_GET(args);
    if (arg == NULL) {
        return BCMA_CLI_CMD_USAGE;
    }

    if (bcma_cli_parse_cmp("CAPture", arg, '\0')){
        if ((arg = BCMA_CLI_ARG_GET(args)) == NULL) {
            return BCMA_CLI_CMD_USAGE;
        }
        if (sal_strcmp(arg, "start") == 0) {
            return tsfifo_capture_start(cli, args);
        } else if (sal_strcmp(arg, "stop") == 0) {
            return tsfifo_capture_stop(cli, args);
        } else if (sal_strcmp(arg, "dump") == 0) {
            return tsfifo_capture_dump(cli, args);
        }
    }

    return BCMA_CLI_CMD_USAGE;
}
