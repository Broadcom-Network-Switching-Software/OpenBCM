/** \file diag_dnxc_sw_state_logging.c
 *
 * sw state diagnostics config definitions
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/* 
 * Note! 
 * This include statement must be at the top of every sw state .c file 
 * It points to a set of in-code compilation flags that must be taken into 
 * account for every sw state componnent compilation 
 */
#include <soc/dnxc/swstate/sw_state_features.h>
/* ---------- */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#include <soc/dnxc/swstate/sw_state_defs.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_prt.h>

#if defined(DNX_SW_STATE_DIAGNOSTIC)
#include <appl/diag/dnxc/swstate/diag_dnxc_sw_state_logging.h>
#include <soc/dnxc/swstate/dnx_sw_state_dump.h>
#include <soc/dnxc/swstate/dnx_sw_state_logging.h>
#include <soc/dnxc/swstate/auto_generated/access/dnxc_module_ids_access.h>

#if defined(DNX_SW_STATE_LOGGING)
sh_sand_cmd_t dnxc_swstate_logging_cmds[] = {
    {"start", sh_dnxc_swstate_logging_start_cmd, NULL, dnxc_swstate_log_options, &dnxc_swstate_log_start_man}
    ,
    {"stop", sh_dnxc_swstate_logging_stop_cmd, NULL, dnxc_swstate_log_options, &dnxc_swstate_log_stop_man}
    ,
    {"add", sh_dnxc_swstate_logging_start_cmd, NULL, dnxc_swstate_log_add_remove_options, &dnxc_swstate_log_add_man}
    ,
    {"remove", sh_dnxc_swstate_logging_stop_cmd, NULL, dnxc_swstate_log_add_remove_options, &dnxc_swstate_log_remove_man}
    ,
    {"print", sh_dnxc_swstate_logging_print_cmd, NULL, dnxc_swstate_log_options, &dnxc_swstate_log_print_man}
    ,
    {NULL}
};

sh_sand_man_t dnxc_swstate_logging_man = {
    cmd_dnxc_swstate_logging_desc,
    NULL,
    NULL,
    NULL,
};

const char cmd_dnxc_swstate_logging_desc[] = "swstate logger";

static uint32
sh_dnxc_swstate_logging_find_module_id(
    char *module_name)
{
    int module_id = NOF_MODULE_ID;
    const char *m_name = NULL;

    for (module_id = INVALID_MODULE_ID; module_id < NOF_MODULE_ID; module_id++)
    {
        m_name = dnxc_module_id_e_get_name(module_id);
        if (!sal_strcmp(module_name, m_name))
        {
            return (uint32) module_id;
        }
    }

    return NOF_MODULE_ID;
}

static shr_error_e
sh_dnxc_swstate_logging_by_module_set(
    int unit,
    uint32 module_id,
    uint8 enable)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * If the user input for the module name does not exist module_id will be equal to NOF_MODULE_ID.
     */
    if (module_id == NOF_MODULE_ID)
    {
        SHR_EXIT();
    }

    if (enable)
    {
        dnx_sw_state_log_by_module_id_enable(unit, module_id);
    }
    else
    {
        dnx_sw_state_log_by_module_id_disable(unit, module_id);
    }

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
sh_dnxc_swstate_logging_by_func_type_set(
    int unit,
    uint32 func_type,
    uint8 enable)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * If the user input for the module name does not exist func_type will be equal to DNX_SW_STATE_COUNT_LOGGING.
     */
    if (func_type == DNX_SW_STATE_COUNT_LOGGING)
    {
        SHR_EXIT();
    }

    if (enable)
    {
        dnx_sw_state_log_by_func_type_enable(unit, func_type);
    }
    else
    {
        dnx_sw_state_log_by_func_type_disable(unit, func_type);
    }

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

static uint32
sh_dnxc_swstate_logging_find_function_name(
    char *function_name)
{
    uint32 f_type = DNX_SW_STATE_COUNT_LOGGING;
    char *f_name = NULL;

    for (f_type = 0; f_type < DNX_SW_STATE_COUNT_LOGGING; f_type++)
    {
        f_name = dnxc_fuction_type_e_get_name(f_type);
        if (!sal_strcmp(function_name, f_name))
        {
            return f_type;
        }
    }

    return DNX_SW_STATE_COUNT_LOGGING;
}

shr_error_e
sh_dnxc_swstate_logging_start_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    char *module_id;
    char *func_type;
    uint32 thread;
    /*
     * local variables.
     */
    uint32 _module;
    uint32 _f_type;
    uint32 _module_count;
    uint32 _f_type_count;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_STR("module_id", module_id);
    SH_SAND_GET_STR("function_type", func_type);
    SH_SAND_GET_UINT32("thread", thread);

    utilex_str_to_upper(module_id);
    utilex_str_to_upper(func_type);

    /*
     * Set main thread logging by default.
     */
    if (thread == DNX_SW_STATE_NONE_THREAD || thread == DNX_SW_STATE_MAIN_THREAD)
    {
        dnx_sw_state_log_by_thread_enable(unit, DNX_SW_STATE_MAIN_THREAD);
    }
    else
    {
        dnx_sw_state_log_by_thread_enable(unit, thread);
    }

    if (!sal_strcmp(module_id, "ALL"))
    {
        _module = 0;
        _module_count = NOF_MODULE_ID;
    }
    else if (sal_strcmp(module_id, "INVALID"))
    {
        _module = sh_dnxc_swstate_logging_find_module_id(module_id);
        _module_count = _module + 1;
    }
    else
    {
        _module = 0;
        _module_count = _module;
    }

    if (!sal_strcmp(func_type, "ALL"))
    {
        _f_type = 0;
        _f_type_count = DNX_SW_STATE_COUNT_LOGGING;
    }
    else if (sal_strcmp(func_type, "INVALID"))
    {
        _f_type = sh_dnxc_swstate_logging_find_function_name(func_type);
        _f_type_count = _f_type + 1;
    }
    else
    {
        _f_type = 0;
        _f_type_count = _f_type;
    }

    for (; _module < _module_count; _module++)
    {
        SHR_IF_ERR_EXIT(sh_dnxc_swstate_logging_by_module_set(unit, _module, TRUE));
    }

    for (; _f_type < _f_type_count; _f_type++)
    {
        sh_dnxc_swstate_logging_by_func_type_set(unit, _f_type, TRUE);
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
sh_dnxc_swstate_logging_stop_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    /*
     * input args.
     */
    char *module_id;
    char *func_type;
    uint32 thread;
    /*
     * local variables.
     */
    uint32 _module;
    uint32 _f_type;
    uint32 _module_count;
    uint32 _f_type_count;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_STR("module_id", module_id);
    SH_SAND_GET_STR("function_type", func_type);
    SH_SAND_GET_UINT32("thread", thread);

    utilex_str_to_upper(module_id);
    utilex_str_to_upper(func_type);

    dnx_sw_state_log_by_thread_disable(unit, thread);

    if (!sal_strcmp(module_id, "ALL"))
    {
        _module = 0;
        _module_count = NOF_MODULE_ID;
    }
    else if (sal_strcmp(module_id, "INVALID"))
    {
        _module = sh_dnxc_swstate_logging_find_module_id(module_id);
        _module_count = _module + 1;
    }
    else
    {
        _module = 0;
        _module_count = _module;
    }

    if (!sal_strcmp(func_type, "ALL"))
    {
        _f_type = 0;
        _f_type_count = DNX_SW_STATE_COUNT_LOGGING;
    }
    else if (sal_strcmp(func_type, "INVALID"))
    {
        _f_type = sh_dnxc_swstate_logging_find_function_name(func_type);
        _f_type_count = _f_type + 1;
    }
    else
    {
        _f_type = 0;
        _f_type_count = _f_type;
    }

    for (; _module < _module_count; _module++)
    {
        SHR_IF_ERR_EXIT(sh_dnxc_swstate_logging_by_module_set(unit, _module, FALSE));
    }

    for (; _f_type < _f_type_count; _f_type++)
    {
        sh_dnxc_swstate_logging_by_func_type_set(unit, _f_type, FALSE);
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
sh_dnxc_swstate_logging_print_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    /*
     * input args.
     */
    char *module_id;
    char *func_type;

    /*
     * local variables.
     */
    uint32 _module;
    uint32 _f_type;
    uint32 _module_count;
    uint32 _f_type_count;
    uint8 do_display = FALSE;
    uint32 thread = 0;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_STR("module_id", module_id);
    SH_SAND_GET_STR("function_type", func_type);

    utilex_str_to_upper(module_id);
    utilex_str_to_upper(func_type);

    PRT_TITLE_SET("Logging: Threads");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "ID");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Thread");
    for (thread = DNX_SW_STATE_NONE_THREAD; thread < DNX_SW_STATE_ALL_THREADS; thread++)
    {
        if (sh_dnxc_swstate_logging_by_thread_get(unit, thread))
        {
            break;
        }
    }
    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
    PRT_CELL_SET("%d", thread);
    PRT_CELL_SET("%s", dnx_sw_state_supported_threads_logging_e_get_name(thread));
    PRT_COMMITX;

    if (!sal_strcmp(module_id, "ALL"))
    {
        _module = 0;
        _module_count = NOF_MODULE_ID;
    }
    else if (sal_strcmp(module_id, "INVALID"))
    {
        _module = sh_dnxc_swstate_logging_find_module_id(module_id);
        _module_count = _module + 1;
    }
    else
    {
        _module = 0;
        _module_count = _module;
    }

    if (!sal_strcmp(func_type, "ALL"))
    {
        _f_type = 0;
        _f_type_count = DNX_SW_STATE_COUNT_LOGGING;
    }
    else if (sal_strcmp(func_type, "INVALID"))
    {
        _f_type = sh_dnxc_swstate_logging_find_function_name(func_type);
        _f_type_count = _f_type + 1;
    }
    else
    {
        _f_type = 0;
        _f_type_count = _f_type;
    }

    do_display = FALSE;

    PRT_TITLE_SET("Logging: Modules");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "ID");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Module");

    for (; _module < _module_count; _module++)
    {
        if (sh_dnxc_swstate_logging_by_module_id_get(unit, _module))
        {
            do_display = TRUE;
            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);

            PRT_CELL_SET("%d", _module);
            PRT_CELL_SET("%s", dnxc_module_id_e_get_name(_module));
        }
    }

    if (do_display)
    {
        PRT_COMMITX;
    }
    else
    {
        /*
         * If nothing was enabaled then just clear loaded PRT memory.
         */
        PRT_FREE;
    }

    do_display = FALSE;

    PRT_TITLE_SET("Logging: Function Types");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "ID");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Function");

    do_display = FALSE;
    for (; _f_type < _f_type_count; _f_type++)
    {
        if (sh_dnxc_swstate_logging_by_function_get(unit, _f_type))
        {
            do_display = TRUE;
            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);

            PRT_CELL_SET("%d", _f_type);
            PRT_CELL_SET("%s", dnxc_fuction_type_e_get_name(_f_type));
        }
    }

    if (do_display)
    {
        PRT_COMMITX;
    }
    else
    {
        /*
         * If nothing was enabaled then just clear loaded PRT memory.
         */
        PRT_FREE;
    }

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

sh_sand_option_t dnxc_swstate_log_options[] = {
    {"module_id", SAL_FIELD_TYPE_STR, "Module ID", "ALL"}
    ,
    {"function_type", SAL_FIELD_TYPE_STR, "function type", "ALL"}
    ,
    {"thread", SAL_FIELD_TYPE_UINT32, "enable/disable logging threads", "0"}
    ,
    {NULL}
};

sh_sand_option_t dnxc_swstate_log_add_remove_options[] = {
    {"module_id", SAL_FIELD_TYPE_STR, "Module ID", "INVALID"}
    ,
    {"function_type", SAL_FIELD_TYPE_STR, "function type", "INVALID"}
    ,
    {"thread", SAL_FIELD_TYPE_UINT32, "enable/disable logging threads", "3"}
    ,
    {NULL}
};

sh_sand_man_t dnxc_swstate_log_start_man = {
    .brief = "swstate logger start",
    .full = "swstate logger start \n" "Supported thread options \n" "NONE = 0" "MAIN = 1" "Background = 2" "All = 3 \n",
    .synopsis = "swstate logger start [module_id=<string>] [function_type=<string>]",
    .examples = "swstate logger start module_id=EXAMPLE_MODULE_ID \n"
};

sh_sand_man_t dnxc_swstate_log_stop_man = {
    .brief = "swstate logger stop",
    .full = "swstate logger stop \n" "Supported thread options \n" "NONE = 0" "MAIN = 1" "Background = 2" "All = 3 \n",
    .synopsis = "swstate logger stop [module_id=<string>] [function_type=<string>]",
    .examples = "swstate logger stop module_id=EXAMPLE_MODULE_ID \n"
};

sh_sand_man_t dnxc_swstate_log_add_man = {
    .brief = "swstate logger add",
    .full = "adding swstate logger rules the existing filser \n"
        "Supported thread options \n" "NONE = 0" "MAIN = 1" "Background = 2" "All = 3 \n",
    .synopsis = "swstate logger add [module_id=<string>] [function_type=<string>]",
    .examples = "swstate logger add module_id=EXAMPLE_MODULE_ID \n"
};

sh_sand_man_t dnxc_swstate_log_remove_man = {
    .brief = "swstate logger remove",
    .full = "removing swstate logger rules the existing filter \n"
        "Supported thread options \n" "NONE = 0" "MAIN = 1" "Background = 2" "All = 3 \n",
    .synopsis = "swstate logger stop [module_id=<string>] [function_type=<string>] [thread=<string>]",
    .examples = "swstate logger stop module_id=EXAMPLE_MODULE_ID \n"
};

sh_sand_man_t dnxc_swstate_log_print_man = {
    .brief = "swstate logger print",
    .full = "prints swstate logger rules the existing filser"
        "Supported thread options \n" "NONE = 0" "MAIN = 1" "Background = 2" "All = 3 \n",
    .synopsis = "swstate logger print [module_id=<string>] [function_type=<string>] [thread=<string>]",
    .examples = "swstate logger print module_id=EXAMPLE_MODULE_ID \n"
};
#endif

#endif /* DNX_SW_STATE_DIAGNOSTIC */
#undef _ERR_MSG_MODULE_NAME
