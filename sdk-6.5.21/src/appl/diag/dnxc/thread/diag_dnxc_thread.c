/** \file diag_dnxc_thread.c
 *
 * Diagnostic for sh_thread.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_APPL_SHELL

/*
 * {
 */
/*************
 * INCLUDES  *
 */
#include <appl/diag/diag.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <sal/appl/sal.h>
#include <shared/shr_pb.h>
#include <shared/shr_thread.h>
#include <assert.h>
#include "diag_dnxc_thread.h"

static shr_error_e
sh_dnxc_thread_stop_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int timeout = 0;
    sal_usecs_t usecs = SHR_THREAD_FOREVER;
    char *name = NULL;
    shr_thread_ctrl_t *th = NULL;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_INT32("timeout", timeout);
    SH_SAND_GET_STR("name", name);

    if (timeout < 0)
    {
        usecs = SHR_THREAD_FOREVER;
    }
    else
    {
        usecs = timeout * SECOND_USEC;
    }

    th = shr_thread_get_by_name(name);

    if (!th || shr_thread_stopping(th))
    {
        cli_out("Thread is not running\n");
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(shr_thread_stop(th, usecs));

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
sh_dnxc_thread_wake_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    shr_thread_ctrl_t *th = NULL;
    char *name = NULL;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_STR("name", name);

    th = shr_thread_get_by_name(name);

    if (!th || shr_thread_stopping(th))
    {
        cli_out("Thread is not running\n");
        SHR_EXIT();
    }
    SHR_IF_ERR_EXIT(shr_thread_wake(th));
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
sh_dnxc_thread_status_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    shr_pb_t *pb;
    SHR_FUNC_INIT_VARS(unit);

    pb = shr_pb_create();
    shr_thread_dump(pb);
    cli_out("Installed threads:\n%s", shr_pb_str(pb));
    shr_pb_destroy(pb);
    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

/* *INDENT-OFF* */
/**
 * \brief list Thread commands.
 */
sh_sand_cmd_t sh_dnxc_thread_cmds[] = {
    {"stop",        sh_dnxc_thread_stop_cmd,        NULL,   sh_dnxc_thread_options,         &sh_dnxc_thread_stop_man, NULL, NULL, SH_CMD_SKIP_MEMORY_CHECK},
    {"wake",        sh_dnxc_thread_wake_cmd,        NULL,   sh_dnxc_thread_wake_options,    &sh_dnxc_thread_wake_man},
    {"status",      sh_dnxc_thread_status_cmd,      NULL,   NULL,                           &sh_dnxc_thread_status_man},
    {NULL}
};

sh_sand_man_t sh_dnxc_thread_man = {
    .brief = "Thread commands."
};

sh_sand_man_t sh_dnxc_thread_stop_man = {
    .brief = "Thread stop commands.",
    .full = "A timeout value may also be specified for the stop command to \
 determine the time to wait for the thread to terminate.",
    .synopsis = NULL,
    .examples = "timeout=-1"
};

sh_sand_man_t sh_dnxc_thread_wake_man = {
    .brief = "Thread wake command.",
    .full = "Wake up the thread",
    .synopsis = NULL,
    .examples = ""
};

sh_sand_man_t sh_dnxc_thread_status_man = {
    .brief = "Thread status command.",
    .full = "Display the status of all threads.\
 The status command may show any of the following letters:\
       A - Thread is active.\n\
       S - Thread received a stop request.\n\
       W - Thread is waiting for a wake event.\n\
       D - Thread is done.\n\
       E - Thread error.",
    .synopsis = NULL,
    .examples = ""
};

/*
 * Supported options.
 */
sh_sand_option_t sh_dnxc_thread_options[] = {
    {"timeout", SAL_FIELD_TYPE_INT32, "Timeout in seconds.",    "-1"},
    {"name",    SAL_FIELD_TYPE_STR,     "Thread name.",         "shrTestThreadName"},
    {NULL}
};

sh_sand_option_t sh_dnxc_thread_wake_options[] = {
    {"name",    SAL_FIELD_TYPE_STR,     "Thread name.",         "shrTestThreadName"},
    {NULL}
};

/* *INDENT-ON* */
/*
 * }
 */
