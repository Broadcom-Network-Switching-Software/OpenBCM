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
#include <sal/sal_sleep.h>

#include <shr/shr_debug.h>

#include <bcmdrd/bcmdrd_types.h>

#include <bcma/cli/bcma_cli_parse.h>
#include <bcma/cli/bcma_cli_ctrlc.h>

#include <bcma/bcmlt/bcma_bcmlt.h>
#include <bcma/bcmlt/bcma_bcmlt_capture_replay.h>
#include <bcma/bcmlt/bcma_bcmltcmd_ltcapture.h>

/*******************************************************************************
 * Local definitions
 */

#ifndef BCMA_BCMLT_CONFIG_DEFAULT_CAPTURE_FILE
#define BCMA_BCMLT_CONFIG_DEFAULT_CAPTURE_FILE "bcmlt.lcap"
#endif

typedef struct replay_ctrlc_data_s {
    int unit;
    bcma_bcmlt_replay_params_t *rp;
} replay_ctrlc_data_t;

static bool replay_done[BCMDRD_CONFIG_MAX_UNITS];

/*******************************************************************************
 * Private functions
 */

/*
 * Call-back function to indicate that the replay thread is done.
 */
static void
capture_replay_done(int unit, int rv)
{
    if (SHR_FAILURE(rv)) {
        cli_out("%sFailure returned from the replay engine: "
                BCMA_BCMLT_ERRMSG_FMT"\n",
                BCMA_CLI_CONFIG_ERROR_STR, BCMA_BCMLT_ERRMSG(rv));
    }
    replay_done[unit] = true;
}

/*
 * Ctrl-C wrapper function for replaying captured data.
 */
static int
capture_replay_ctrlc(void *data)
{
    int rv;
    replay_ctrlc_data_t *ctrlc_data = (replay_ctrlc_data_t *)data;
    int unit = ctrlc_data->unit;
    bcma_bcmlt_replay_params_t *rp = ctrlc_data->rp;
    int ctrlc_break = false;

    rv = bcma_bcmlt_replay_thread_start(unit, rp, capture_replay_done);
    if (SHR_FAILURE(rv)) {
        cli_out("%sFailed to start replay thread: "BCMA_BCMLT_ERRMSG_FMT"\n",
                BCMA_CLI_CONFIG_ERROR_STR, BCMA_BCMLT_ERRMSG(rv));
        return BCMA_CLI_CMD_FAIL;
    }

    /* Wait till the replay engine thread is done. */
    while (!replay_done[unit]) {
        if (bcma_cli_ctrlc_break()) {
            ctrlc_break = true;
            break;
        }
        sal_usleep(500 * MSEC_USEC);
    }

    rv = bcma_bcmlt_replay_thread_stop(unit);
    if (SHR_FAILURE(rv)) {
        cli_out("%sFailed to stop replay thread: "BCMA_BCMLT_ERRMSG_FMT"\n",
                BCMA_CLI_CONFIG_ERROR_STR, BCMA_BCMLT_ERRMSG(rv));
    }

    /* Clear the replay-done flag */
    replay_done[unit] = false;

    if (ctrlc_break) {
        return BCMA_CLI_CMD_INTR;
    }
    return rv == SHR_E_NONE ? BCMA_CLI_CMD_OK : BCMA_CLI_CMD_FAIL;
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
    char *vrtfile = NULL;
    int vrt = 0;
    int write_only = 0;
    bcma_bcmlt_capture_params_t capture_params, *cp = &capture_params;
    int unit = cli->cmd_unit;

    bcma_cli_parse_table_init(cli, &pt);
    bcma_cli_parse_table_add(&pt, "LogFile", "str", &logfile, NULL);
    bcma_cli_parse_table_add(&pt, "VerboseRealTime", "bool", &vrt, NULL);
    bcma_cli_parse_table_add(&pt, "VerboseRealTimeLogFile", "str",
                             &vrtfile, NULL);
    bcma_cli_parse_table_add(&pt, "WriteOnly", "bool", &write_only, NULL);

    if (bcma_cli_parse_table_do_args(&pt, args) < 0) {
        bcma_cli_parse_table_done(&pt);
        return BCMA_CLI_CMD_USAGE;
    }

    sal_memset(cp, 0, sizeof(*cp));
    if (logfile && sal_strcmp(logfile, "-") != 0) {
        cp->cap_file = logfile;
    }
    if (vrtfile && sal_strcmp(vrtfile, "-") != 0) {
        cp->rtrp_text_file = vrtfile;
    }
    cp->rtrp_console_output = vrt;
    cp->write_only = write_only;

    rv = bcma_bcmlt_capture_start(unit, cp);
    if (SHR_FAILURE(rv)) {
        if (rv == SHR_E_EXISTS) {
            cli_out("%sCapture is already started.\n",
                    BCMA_CLI_CONFIG_ERROR_STR);
        } else {
            cli_out("%sFailed to start capture: "BCMA_BCMLT_ERRMSG_FMT"\n",
                    BCMA_CLI_CONFIG_ERROR_STR, BCMA_BCMLT_ERRMSG(rv));
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
    int unit = cli->cmd_unit;

    rv = bcma_bcmlt_capture_stop(unit);
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
    int unit = cli->cmd_unit;
    bcma_cli_parse_table_t pt;
    char *logfile = BCMA_BCMLT_CONFIG_DEFAULT_CAPTURE_FILE;
    char *ofile = NULL;
    int timing = 0, verbose = 1, submit = 0, raw = 0;
    bcma_bcmlt_replay_params_t replay_params, *rp = &replay_params;
    replay_ctrlc_data_t ctrlc_data;

    if (bcma_bcmlt_capture_started(unit)) {
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
    rp->cap_file = logfile;
    rp->timing = timing;
    rp->console_output = verbose;
    rp->submit = submit;
    rp->text_file = ofile;
    rp->raw = raw;

    sal_memset(&ctrlc_data, 0, sizeof(ctrlc_data));
    ctrlc_data.unit = cli->cmd_unit;
    ctrlc_data.rp = rp;

    rv = bcma_cli_ctrlc_exec(cli, capture_replay_ctrlc, &ctrlc_data, 1);
    if (rv == BCMA_CLI_CMD_INTR) {
        cli_out("%sReplay aborted.\n", BCMA_CLI_CONFIG_ERROR_STR);
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
    int rv;
    int unit = cli->cmd_unit;
    bcma_bcmlt_capture_params_t cparams;

    rv = bcma_bcmlt_capture_params_get(unit, &cparams);
    if (SHR_FAILURE(rv)) {
        if (rv == SHR_E_NOT_FOUND) {
            cli_out("Stopped capturing.\n");
            return BCMA_CLI_CMD_OK;
        }
        cli_out("%sFailed to get capture status: "BCMA_BCMLT_ERRMSG_FMT"\n",
                BCMA_CLI_CONFIG_ERROR_STR, BCMA_BCMLT_ERRMSG(rv));
        return BCMA_CLI_CMD_FAIL;
    }

    if (cparams.cap_file != NULL) {
        cli_out("Started capturing%s to %s.\n",
                cparams.write_only ? " write-only operations" : "",
                cparams.cap_file);
    }
    if (cparams.rtrp_text_file != NULL || cparams.rtrp_console_output) {
        cli_out("Started VerboseRealTime capturing%s",
                cparams.write_only ? " write-only operations" : "");
        if (cparams.rtrp_text_file != NULL) {
            cli_out(" to %s", cparams.rtrp_text_file);
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
    int unit;

    for (unit = 0; unit < BCMDRD_CONFIG_MAX_UNITS; unit++) {
        bcma_bcmlt_capture_stop(unit);
    }

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
