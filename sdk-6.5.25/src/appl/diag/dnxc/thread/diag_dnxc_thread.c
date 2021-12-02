/** \file diag_dnxc_thread.c
 *
 * Diagnostic for sh_thread.
 *
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
 *************/
#include <appl/diag/diag.h>
#include <appl/diag/sand/diag_sand_prt.h>
#include <sal/appl/sal.h>
#include <shared/shr_thread_manager.h>
#include <assert.h>
#include "diag_dnxc_thread.h"

#include <soc/counter.h>
#include <soc/drv.h>

#ifdef BCM_DNXF_SUPPORT
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_linkscan.h>
#endif

#define DIAG_DNXC_THREAD_LINKSCAN_NAME "bcmLINK"
#define DIAG_DNXC_THREAD_LINKSCAN_NAME_SIZE 7

#define DIAG_DNXC_THREAD_COUNTER_NAME "bcmCNTR"
#define DIAG_DNXC_THREAD_COUNTER_NAME_SIZE 7

#define DIAG_DNXC_THREAD_L2_FLUSH_NAME "bcmL2Flush"
#define DIAG_DNXC_THREAD_L2_FLUSH_NAME_SIZE 10

/**
* \brief
*   Callback to iterate over the threads and perform specific operation.
 * \param [in] unit - unit ID
 * \param [in] name - Thread name
 * \param [in] args - arguments passed to the function according to the diag mechanism
 * \param [in] sand_control - passed according to the diag mechanism
 * \param [in] extra_arguments - used to pass extra information.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
*/
typedef shr_error_e(
    *sh_dnxc_thread_iterator_cb) (
    int unit,
    char *name,
    args_t * args,
    sh_sand_control_t * sand_control,
    void *extra_arguments);

/*
 * Global variable used in the "thread status" command.
 */
void *print_control[SOC_MAX_NUM_DEVICES] = { NULL };

int prev_counter_interval[SOC_MAX_NUM_DEVICES] = { 0 };
int prev_linkscan_interval[SOC_MAX_NUM_DEVICES] = { 0 };

static shr_error_e
sh_dnxc_thread_start(
    int unit,
    char *name,
    args_t * args,
    sh_sand_control_t * sand_control,
    void *extra_arguments)
{
    SHR_FUNC_INIT_VARS(unit);
    if (sal_strncmp(name, DIAG_DNXC_THREAD_LINKSCAN_NAME, DIAG_DNXC_THREAD_LINKSCAN_NAME_SIZE) == 0)
    {
        int interval = -1;
        int linkscan_interval_default = -1;

#ifdef BCM_DNX_SUPPORT
        if (SOC_IS_DNX(unit))
        {
            linkscan_interval_default = BCM_LINKSCAN_INTERVAL_DEFAULT;
        }
        else
#endif
#ifdef BCM_DNXF_SUPPORT
        if (SOC_IS_DNXF(unit))
        {
            linkscan_interval_default = dnxf_data_linkscan.general.interval_get(unit);
        }
        else
#endif
        {
            SHR_CLI_EXIT(_SHR_E_UNIT, "%s: Error: Device types other than DNX/DNXF are not supported\n", ARG_CMD(args));
        }

        SH_SAND_GET_INT32("interval", interval);

        /*
         * The priority of the interval value is: user specified > previously used > default value.
         */
        interval = ((interval != -1) ? interval : (prev_linkscan_interval[unit] != 0 ? prev_linkscan_interval[unit] :
                                                   linkscan_interval_default));

        SHR_IF_ERR_EXIT(bcm_linkscan_enable_set(unit, interval));
    }
    else if (sal_strncmp(name, DIAG_DNXC_THREAD_COUNTER_NAME, DIAG_DNXC_THREAD_COUNTER_NAME_SIZE) == 0)
    {
        int interval = -1;
        soc_control_t *soc = SOC_CONTROL(unit);
        uint32 flags = soc->counter_flags;
        bcm_pbmp_t counter_pbmp;
        int counter_interval_default;

        SH_SAND_GET_INT32("interval", interval);

        counter_interval_default =
            (SAL_BOOT_PLISIM) ? (SOC_COUNTER_INTERVAL_SIM_DEFAULT) * 8 : SOC_COUNTER_INTERVAL_DEFAULT;

        /*
         * The priority of the interval value is: user specified > previously used > default value.
         */
        interval = (interval != -1) ? interval : (soc->counter_interval > 0 ? soc->counter_interval
                                                  : (prev_counter_interval[unit] > 0) ?
                                                  prev_counter_interval[unit] : counter_interval_default);

        /*
         * counters are non-dmable
         */
        flags &= ~SOC_COUNTER_F_DMA;

        SOC_PBMP_ASSIGN(counter_pbmp, soc->counter_pbmp);

        SHR_IF_ERR_EXIT(soc_counter_start(unit, flags, interval, counter_pbmp));
    }
    else
    {
        shr_thread_manager_handler_t shr_thread_manager_handler = NULL;
        SHR_IF_ERR_EXIT(shr_thread_manager_get_by_name(unit, name, &shr_thread_manager_handler));
        SHR_IF_ERR_EXIT(shr_thread_manager_start(shr_thread_manager_handler));
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
sh_dnxc_thread_destroy(
    int unit,
    char *name,
    args_t * args,
    sh_sand_control_t * sand_control,
    void *extra_arguments)
{
    SHR_FUNC_INIT_VARS(unit);

    if ((sal_strncmp(name, DIAG_DNXC_THREAD_LINKSCAN_NAME, DIAG_DNXC_THREAD_LINKSCAN_NAME_SIZE) == 0)
        || (sal_strncmp(name, DIAG_DNXC_THREAD_COUNTER_NAME, DIAG_DNXC_THREAD_COUNTER_NAME_SIZE) == 0))
    {
        LOG_CLI((BSL_META
                 ("Error: Option to destroy this thread(%s) is not supported, but you can stop it with thread stop.\n"),
                 name));
        SHR_EXIT();
    }
    else
    {
        shr_thread_manager_handler_t shr_thread_manager_handler = NULL;
        SHR_IF_ERR_EXIT(shr_thread_manager_get_by_name(unit, name, &shr_thread_manager_handler));
        SHR_IF_ERR_EXIT(shr_thread_manager_destroy(&shr_thread_manager_handler));
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
sh_dnxc_thread_stop(
    int unit,
    char *name,
    args_t * args,
    sh_sand_control_t * sand_control,
    void *extra_arguments)
{

    SHR_FUNC_INIT_VARS(unit);

    if (sal_strncmp(name, DIAG_DNXC_THREAD_LINKSCAN_NAME, DIAG_DNXC_THREAD_LINKSCAN_NAME_SIZE) == 0)
    {
        int interval = -1;
        int linkscan_interval_default = -1;

#ifdef BCM_DNX_SUPPORT
        if (SOC_IS_DNX(unit))
        {
            linkscan_interval_default = BCM_LINKSCAN_INTERVAL_DEFAULT;
        }
        else
#endif
#ifdef BCM_DNXF_SUPPORT
        if (SOC_IS_DNXF(unit))
        {
            linkscan_interval_default = dnxf_data_linkscan.general.interval_get(unit);
        }
        else
#endif
        {
            SHR_CLI_EXIT(_SHR_E_UNIT, "%s: Error: Device types other than DNX/DNXF are not supported\n", ARG_CMD(args));
        }

        SHR_IF_ERR_EXIT(bcm_linkscan_enable_get(unit, &interval));

        interval =
            interval > 0 ? interval : (prev_linkscan_interval[unit] >
                                       0 ? prev_linkscan_interval[unit] : linkscan_interval_default);

        prev_linkscan_interval[unit] = interval;
        SHR_IF_ERR_EXIT(bcm_linkscan_enable_set(unit, 0));
    }
    else if (sal_strncmp(name, DIAG_DNXC_THREAD_COUNTER_NAME, DIAG_DNXC_THREAD_COUNTER_NAME_SIZE) == 0)
    {
        soc_control_t *soc = SOC_CONTROL(unit);
        int interval = soc->counter_interval;
        int counter_interval_default =
            (SAL_BOOT_PLISIM) ? (SOC_COUNTER_INTERVAL_SIM_DEFAULT) * 8 : SOC_COUNTER_INTERVAL_DEFAULT;
        interval =
            interval >
            0 ? interval : ((prev_counter_interval[unit] > 0) ? prev_counter_interval[unit] : counter_interval_default);

        prev_counter_interval[unit] = interval;

        SHR_IF_ERR_EXIT(soc_counter_stop(unit));
    }
    else
    {
        shr_thread_manager_handler_t shr_thread_manager_handler = NULL;
        SHR_IF_ERR_EXIT(shr_thread_manager_get_by_name(unit, name, &shr_thread_manager_handler));
        SHR_IF_ERR_EXIT(shr_thread_manager_stop(shr_thread_manager_handler, FALSE));
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
sh_dnxc_thread_trigger(
    int unit,
    char *name,
    args_t * args,
    sh_sand_control_t * sand_control,
    void *extra_arguments)
{
    SHR_FUNC_INIT_VARS(unit);

    if ((sal_strncmp(name, DIAG_DNXC_THREAD_LINKSCAN_NAME, DIAG_DNXC_THREAD_LINKSCAN_NAME_SIZE) == 0)
        || (sal_strncmp(name, DIAG_DNXC_THREAD_COUNTER_NAME, DIAG_DNXC_THREAD_COUNTER_NAME_SIZE) == 0)
        || (sal_strncmp(name, DIAG_DNXC_THREAD_L2_FLUSH_NAME, DIAG_DNXC_THREAD_L2_FLUSH_NAME_SIZE) == 0))
    {
        LOG_CLI((BSL_META("Error: Option to trigger this thread(%s) is not supported.\n"), name));
        SHR_EXIT();
    }
    else
    {
        shr_thread_manager_handler_t shr_thread_manager_handler = NULL;
        SHR_IF_ERR_EXIT(shr_thread_manager_get_by_name(unit, name, &shr_thread_manager_handler));
        SHR_IF_ERR_EXIT(shr_thread_manager_trigger(shr_thread_manager_handler));
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
sh_dnxc_thread_get_info_by_name(
    int unit,
    char *name,
    shr_thread_manager_info_t * thread_info)
{
    int nof_shr_threads = 0;
    int tmp_nof_shr_threads = 0;
    int index = 0;
    shr_thread_manager_info_t *threads_info = NULL;
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

    for (index = 0; index < nof_shr_threads; index++)
    {
        if (sal_strncmp(name, threads_info[index].name, SH_SAND_MAX_RESOURCE_SIZE) == 0)
        {
            sal_memcpy(thread_info, &threads_info[index], sizeof(threads_info[index]));
            SHR_EXIT();
        }
    }

exit:
    if (threads_info != NULL)
    {
        sal_free(threads_info);
    }
    SHR_FUNC_EXIT;
}

static shr_error_e
sh_dnxc_thread_status(
    int unit,
    char *name,
    args_t * args,
    sh_sand_control_t * sand_control,
    void *extra_arguments)
{

    prt_control_t *prt_ctr = NULL;
    SHR_FUNC_INIT_VARS(unit);

    prt_ctr = (prt_control_t *) print_control[unit];

    if (sal_strncmp(name, DIAG_DNXC_THREAD_LINKSCAN_NAME, DIAG_DNXC_THREAD_LINKSCAN_NAME_SIZE) == 0)
    {
        int interval = 0;
        SHR_IF_ERR_EXIT(bcm_linkscan_enable_get(unit, &interval));
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        PRT_CELL_SET(DIAG_DNXC_THREAD_LINKSCAN_NAME);
        PRT_CELL_SET("%d", interval);
        PRT_CELL_SET("%s", "N/A");
        PRT_CELL_SET("%d", interval > 0 ? 1 : 0);
    }
    else if (sal_strncmp(name, DIAG_DNXC_THREAD_COUNTER_NAME, DIAG_DNXC_THREAD_COUNTER_NAME_SIZE) == 0)
    {
        int interval = 0;
        uint32 counter_flags = 0;
        bcm_pbmp_t counter_pbmp;
        SHR_IF_ERR_EXIT(soc_counter_status(unit, &counter_flags, &interval, &counter_pbmp));
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        PRT_CELL_SET(DIAG_DNXC_THREAD_COUNTER_NAME);
        PRT_CELL_SET("%d", interval);
        PRT_CELL_SET("%s", "N/A");
        PRT_CELL_SET("%d", interval > 0 ? 1 : 0);

    }
    else
    {
        shr_thread_manager_info_t threads_info;
        sal_memset(&threads_info, 0, sizeof(threads_info));
        if (extra_arguments != NULL)
        {
            threads_info = *(shr_thread_manager_info_t *) extra_arguments;
        }
        else
        {
            SHR_IF_ERR_EXIT(sh_dnxc_thread_get_info_by_name(unit, name, &threads_info));
        }
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        PRT_CELL_SET("%s", name);
        PRT_CELL_SET("%d", threads_info.interval);
        PRT_CELL_SET("%d", threads_info.is_quit_signaled);
        PRT_CELL_SET("%d", threads_info.is_active);
    }

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *    An internal function that receives the data and a cb for a specific functionality.
 * \param [in] unit - unit ID
 * \param [in] args - arguments passed to the function according to the diag mechanism
 * \param [in] sand_control - passed according to the diag mechanism
 * \param [in] cb - callback to a specific sh_dnxc_thread_*cmd function
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
static shr_error_e
sh_dnxc_thread_iterator(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control,
    sh_dnxc_thread_iterator_cb cb)
{
    shr_thread_manager_info_t *threads_info = NULL;
    uint8 name_present = FALSE;
    char *name = NULL;

    SHR_FUNC_INIT_VARS(unit);
    SH_SAND_GET_STR("name", name);
    SH_SAND_IS_PRESENT("name", name_present);

    if (name_present && sal_strncmp(name, "all", SH_SAND_MAX_RESOURCE_SIZE) != 0)
    {
        SHR_IF_ERR_EXIT(cb(unit, name, args, sand_control, NULL));
    }
    else
    {
        int nof_shr_threads = 0;
        int tmp_nof_shr_threads = 0;
        int index = 0;

        SHR_IF_ERR_EXIT(cb(unit, DIAG_DNXC_THREAD_LINKSCAN_NAME, args, sand_control, NULL));
        SHR_IF_ERR_EXIT(cb(unit, DIAG_DNXC_THREAD_COUNTER_NAME, args, sand_control, NULL));

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

        for (index = 0; index < nof_shr_threads; index++)
        {
            SHR_IF_ERR_EXIT(cb(unit, threads_info[index].name, args, sand_control, &threads_info[index]));
        }
    }

exit:
    SHR_FREE(threads_info);
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *    Start a thread.
 * \param [in] unit - unit ID
 * \param [in] args - arguments passed to the function according to the diag mechanism
 * \param [in] sand_control - passed according to the diag mechanism
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
static shr_error_e
sh_dnxc_thread_start_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(sh_dnxc_thread_iterator(unit, args, sand_control, sh_dnxc_thread_start));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
sh_dnxc_thread_stop_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(sh_dnxc_thread_iterator(unit, args, sand_control, sh_dnxc_thread_stop));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *    Stop a thread.
 * \param [in] unit - unit ID
 * \param [in] args - arguments passed to the function according to the diag mechanism
 * \param [in] sand_control - passed according to the diag mechanism
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
static shr_error_e
sh_dnxc_thread_trigger_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(sh_dnxc_thread_iterator(unit, args, sand_control, sh_dnxc_thread_trigger));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *    Destroy a thread.
 * \param [in] unit - unit ID
 * \param [in] args - arguments passed to the function according to the diag mechanism
 * \param [in] sand_control - passed according to the diag mechanism
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
static shr_error_e
sh_dnxc_thread_destroy_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(sh_dnxc_thread_iterator(unit, args, sand_control, sh_dnxc_thread_destroy));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *    Display a thread status.
 * \param [in] unit - unit ID
 * \param [in] args - arguments passed to the function according to the diag mechanism
 * \param [in] sand_control - passed according to the diag mechanism
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
static shr_error_e
sh_dnxc_thread_status_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    PRT_TITLE_SET("Installed threads");
    PRT_COLUMN_ADD("Name");
    PRT_COLUMN_ADD("Interval(usec)");
    PRT_COLUMN_ADD("Is Quit signaled");
    PRT_COLUMN_ADD("Is Active");

    print_control[unit] = (void *) prt_ctr;

    SHR_IF_ERR_EXIT(sh_dnxc_thread_iterator(unit, args, sand_control, sh_dnxc_thread_status));

    PRT_COMMITX;
    SHR_EXIT();

exit:
    print_control[unit] = NULL;
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/* *INDENT-OFF* */
sh_sand_man_t sh_dnxc_thread_man = {
    .brief = "Thread commands."
};

sh_sand_man_t sh_dnxc_thread_stop_man = {
    .brief = "Thread stop commands.",
    .full = "Command that stops a thread by a given name. It does not destroy it.",
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

sh_sand_man_t sh_dnxc_thread_destroy_man = {
    .brief = "Thread destroy command.",
    .full = "Destroy a thread by a given name.\n Make sure you want to destroy it becase there is no create option",
    .synopsis = NULL,
    .examples = ""
};

sh_sand_man_t sh_dnxc_thread_start_man = {
    .brief = "Thread start command.",
    .full = "Start a thread by a given name. It will return an error if the thread is already started.",
    .synopsis = NULL,
    .examples = ""
};

/*
 * Supported options.
 */
sh_sand_option_t sh_dnxc_thread_start_options[] = {
    {"name",     SAL_FIELD_TYPE_STR,     "Thread name(if not listed it will start all threads).",         "all"},
    {"interval", SAL_FIELD_TYPE_INT32,   "Thread interval.",     "-1"},
    {NULL}
};

sh_sand_option_t sh_dnxc_thread_options[] = {
    {"name",    SAL_FIELD_TYPE_STR,     "Thread name(if not listed it will stop all threads).",         "all"},
    {NULL}
};

/**
 * \brief list Thread commands.
 */
sh_sand_cmd_t sh_dnxc_thread_cmds[] = {
    {"stop",        sh_dnxc_thread_stop_cmd,        NULL,   sh_dnxc_thread_options,         &sh_dnxc_thread_stop_man, NULL, NULL, SH_CMD_SKIP_MEMORY_CHECK},
    {"trigger",     sh_dnxc_thread_trigger_cmd,     NULL,   sh_dnxc_thread_options,         &sh_dnxc_thread_trigger_man},
    {"status",      sh_dnxc_thread_status_cmd,      NULL,   sh_dnxc_thread_options,         &sh_dnxc_thread_status_man},
    {"destroy",     sh_dnxc_thread_destroy_cmd,     NULL,   sh_dnxc_thread_options,         &sh_dnxc_thread_destroy_man},
    {"start",       sh_dnxc_thread_start_cmd,       NULL,   sh_dnxc_thread_start_options,   &sh_dnxc_thread_start_man},
    {NULL}
};
/* *INDENT-ON* */
/*
 * }
 */
