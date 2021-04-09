/** \file diag_dnxc_thread.c
 *
 * Diagnostic for sh_thread.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
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
#include <appl/diag/sand/diag_sand_prt.h>
#include <sal/appl/sal.h>
#include <shared/shr_thread_manager.h>
#include <assert.h>
#include "diag_dnxc_thread.h"

static shr_error_e
sh_dnxc_thread_stop_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    char *name = NULL;
    shr_thread_manager_handler_t shr_thread_manager_handler = NULL;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_STR("name", name);

    SHR_IF_ERR_EXIT(shr_thread_manager_get_by_name(unit, name, &shr_thread_manager_handler));
    SHR_IF_ERR_EXIT(shr_thread_manager_stop(shr_thread_manager_handler, FALSE));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
sh_dnxc_thread_trigger_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    char *name = NULL;
    shr_thread_manager_handler_t shr_thread_manager_handler = NULL;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_STR("name", name);

    SHR_IF_ERR_EXIT(shr_thread_manager_get_by_name(unit, name, &shr_thread_manager_handler));
    SHR_IF_ERR_EXIT(shr_thread_manager_trigger(shr_thread_manager_handler));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
sh_dnxc_thread_status_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int nof_shr_threads = 0;
    int tmp_nof_shr_threads = 0;
    shr_thread_manager_info_t *threads_info = NULL;
    int index = 0;
    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    nof_shr_threads = shr_thread_manager_count_get(unit);

    if (nof_shr_threads < 1)
    {
        SHR_EXIT();
    }

    threads_info = sal_alloc(nof_shr_threads * sizeof(shr_thread_manager_info_t), "thread_info");
    sal_memset(threads_info, 0, nof_shr_threads * sizeof(shr_thread_manager_info_t));

    tmp_nof_shr_threads = nof_shr_threads;

    SHR_IF_ERR_EXIT_WITH_LOG(shr_thread_manager_info_data_get(unit, &tmp_nof_shr_threads, threads_info),
                             "Difference in the thread count. Expected:%d, Received:%d \n%s",
                             nof_shr_threads, tmp_nof_shr_threads, EMPTY);

    PRT_TITLE_SET("Installed threads");
    PRT_COLUMN_ADD("Name");
    PRT_COLUMN_ADD("Interval(usec)");
    PRT_COLUMN_ADD("Is Quit signaled");

    for (index = 0; index < nof_shr_threads; index++)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        PRT_CELL_SET("%s", threads_info[index].name);
        PRT_CELL_SET("%d", threads_info[index].interval);
        PRT_CELL_SET("%d", threads_info[index].is_quit_signaled);
    }

    PRT_COMMITX;
    SHR_EXIT();

exit:
    PRT_FREE;
    SHR_FREE(threads_info);
    SHR_FUNC_EXIT;
}

/* *INDENT-OFF* */
/**
 * \brief list Thread commands.
 */
sh_sand_cmd_t sh_dnxc_thread_cmds[] = {
    {"stop",        sh_dnxc_thread_stop_cmd,        NULL,   sh_dnxc_thread_options,         &sh_dnxc_thread_stop_man, NULL, NULL, SH_CMD_SKIP_MEMORY_CHECK},
    {"trigger",     sh_dnxc_thread_trigger_cmd,     NULL,   sh_dnxc_thread_options,         &sh_dnxc_thread_trigger_man},
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
    .examples = ""
};

sh_sand_man_t sh_dnxc_thread_trigger_man = {
    .brief = "Thread trigger command.",
    .full = "Trigger the thread",
    .synopsis = NULL,
    .examples = ""
};

sh_sand_man_t sh_dnxc_thread_status_man = {
    .brief = "Thread status command.",
    .full = "Display the status of all threads.",
    .synopsis = NULL,
    .examples = ""
};

/*
 * Supported options.
 */
sh_sand_option_t sh_dnxc_thread_options[] = {
    {"name",    SAL_FIELD_TYPE_STR,     "Thread name.",         "shrTestThreadName"},
    {NULL}
};

/* *INDENT-ON* */
/*
 * }
 */
