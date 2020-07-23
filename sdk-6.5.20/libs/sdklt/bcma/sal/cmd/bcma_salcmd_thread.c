/*! \file bcma_salcmd_thread.c
 *
 * Test thread API.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_libc.h>
#include <sal/sal_assert.h>
#include <sal/sal_time.h>
#include <sal/sal_sleep.h>

#include <shr/shr_debug.h>
#include <shr/shr_thread.h>

#include <bcma/cli/bcma_cli_parse.h>
#include <bcma/sal/bcma_salcmd_thread.h>

static void
dump_threads(void)
{
    shr_pb_t *pb;

    pb = shr_pb_create();
    shr_thread_dump(pb);
    cli_out("Installed threads:\n%s", shr_pb_str(pb));
    shr_pb_destroy(pb);
}

static void
test_thread(shr_thread_t th, void *arg)
{
    sal_usecs_t to_usec;

    assert(arg);
    to_usec = *((sal_usecs_t *)arg);

    cli_out("[Test thread]: Start\n");

    if (to_usec) {
        while (1) {
            cli_out("[Test thread]: Sleep\n");
            shr_thread_sleep(th, to_usec);
            cli_out("[Test thread]: Wake up\n");
            if (shr_thread_stopping(th)) {
                break;
            }
            cli_out("[Test thread]: Processing\n");
            sal_sleep(1);
        }
    }
    cli_out("[Test thread]: Exit\n");
}

int
bcma_salcmd_thread(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    static shr_thread_t th;
    sal_usecs_t to_usecs = SHR_THREAD_FOREVER;
    int rv = BCMA_CLI_CMD_FAIL;
    int to_val;
    const char *cmd;
    const char *to_str;

    cmd = BCMA_CLI_ARG_GET(args);
    if (!cmd) {
        return BCMA_CLI_CMD_USAGE;
    }

    to_str = BCMA_CLI_ARG_GET(args);
    if (to_str) {
        if (bcma_cli_parse_int(to_str, &to_val) < 0) {
            cli_out("%sInvalid timeout value: '%s'\n",
                    BCMA_CLI_CONFIG_ERROR_STR, to_str);
            return BCMA_CLI_CMD_FAIL;
        }
        if (to_val < 0) {
            to_usecs = SHR_THREAD_FOREVER;
        } else {
            to_usecs = to_val * SECOND_USEC;
        }
    }

    if (sal_strcmp(cmd, "status") == 0) {
        dump_threads();
    } else if (sal_strcmp(cmd, "start") == 0) {
        if (th) {
            cli_out("Thread already running\n");
            return BCMA_CLI_CMD_OK;
        }
        th = shr_thread_start("utshrTestStart", -1, test_thread, &to_usecs);
        if (th) {
            rv = BCMA_CLI_CMD_OK;
        }
    } else if (sal_strcmp(cmd, "stop") == 0) {
        if (!th) {
            cli_out("Thread is not running\n");
            return BCMA_CLI_CMD_OK;
        }
        rv = shr_thread_stop(th, to_usecs);
        if (SHR_SUCCESS(rv)) {
            th = NULL;
        }
    } else if (sal_strcmp(cmd, "wake") == 0) {
        if (!th) {
            cli_out("Thread is not running\n");
            return BCMA_CLI_CMD_OK;
        }
        rv = shr_thread_wake(th);
    } else {
        cli_out("%sunknown thread commands: '%s'\n",
                BCMA_CLI_CONFIG_ERROR_STR, cmd);
    }

    return rv;
}
