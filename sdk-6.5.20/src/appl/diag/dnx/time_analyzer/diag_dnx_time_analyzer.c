/** \file diag_dnx_time_analyzer.c
 * 
 * diagnostic pack for time analyzer
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_APPLDNX_COUNTER

/*
 * INCLUDE FILES:
 * {
 */
 /*
  * shared
  */
#include <shared/bsl.h>
#include <shared/utilex/utilex_integer_arithmetic.h>

/*appl*/
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_prt.h>

/*shared*/
#include <shared/utilex/utilex_time_analyzer.h>

/*sal*/
#include <sal/appl/sal.h>

/*enum header*/
#include <bcm_int/dnx/init/init_time_analyzer.h>

/*
 * }
 */

static dnx_init_time_analyzer_operations_t diag_dnx_init_time_analyzer_ops[] = DNX_INIT_TIME_ANALYZER_OPS_INIT;

/*
 * dump
 */

/* *INDENT-OFF* */
sh_sand_man_t dnx_time_analyzer_dump_man = {
    .brief =    "Dump time analyzer table"
};
/* *INDENT-ON* */

/**
 * \brief - display time analyzer statistics
 */
shr_error_e
cmd_dnx_time_analyzer_dump(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int nof_modules;
    utilex_time_analyzer_diag_info_t diag_info[UTILEX_TIME_ANALYZER_MAX_NOF_MODULES];
    int module_index;
    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Print time analyzer statistics table
     */
    PRT_TITLE_SET("Time Analyzer");
    PRT_COLUMN_ADD("Name");
    PRT_COLUMN_ADD("Total Time");
    PRT_COLUMN_ADD("Occurrences");
    PRT_COLUMN_ADD("Avg Time");

    SHR_IF_ERR_EXIT(utilex_time_analyzer_diag_info_get
                    (unit, UTILEX_TIME_ANALYZER_MAX_NOF_MODULES, diag_info, &nof_modules));

    for (module_index = 0; module_index < nof_modules; module_index++)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("%s", diag_info[module_index].name);
        /** if bigger than 2 seconds - print in seconds units  - otherwise milliseconds*/
        if (diag_info[module_index].time > /* 2 seconds */ 2 * 1000 * 1000)
        {
            PRT_CELL_SET("%u [seconds]", diag_info[module_index].time / (1000 * 1000));
        }
        else
        {
            PRT_CELL_SET("%u [milliseconds]", diag_info[module_index].time / (1000));
        }
        PRT_CELL_SET("%d", diag_info[module_index].occurences);
        if (diag_info[module_index].occurences == 0)
        {
            PRT_CELL_SET("0 [microseconds]");
        }
        else
        {
            PRT_CELL_SET("%d [microseconds]", diag_info[module_index].time / diag_info[module_index].occurences);
        }
    }

    PRT_COMMITX;
exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/*
 * Clear
 */
/* *INDENT-OFF* */
sh_sand_man_t dnx_time_analyzer_clear_man = {
    .brief    = "Diagnostic for time analyzer clear",
    .full     = "Clear Time analyzer table and restart the counting"
};
/* *INDENT-ON* */

/**
 * \brief - time analyzer clear command
 */
shr_error_e
cmd_dnx_time_analyzer_clear(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(utilex_time_analyzer_clear(unit));

exit:
    SHR_FUNC_EXIT;
}

/*
 * Start
 */
/* *INDENT-OFF* */

sh_sand_man_t dnx_time_analyzer_start_man = {
    .brief    = "Starts time analyzer",
    .full     = "Starts the time analyzer for a certain module",
    .synopsis = NULL,
    .examples = "module_id=4"
};

/* *INDENT-ON* */

/**
 * \brief - time analyzer start command
 */

shr_error_e
cmd_dnx_time_analyzer_start(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int module_id = -1;

    int module_idx = 0;
    int last_module = 0;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_INT32("module_id", module_id);

    if (module_id < 0)
    {
        module_idx = 0;
        last_module = UTILEX_TIME_ANALYZER_MAX_NOF_MODULES;
    }
    else
    {
        module_idx = module_id;
        last_module = module_id + 1;
    }

    for (module_idx = module_id; module_idx < last_module; module_idx++)
    {
        utilex_time_analyzer_module_start(unit, module_idx);
    }
    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

/*
 * Stop
 */
/* *INDENT-OFF* */

sh_sand_man_t dnx_time_analyzer_stop_man = {
    .brief    = "Stops time analyzer",
    .full     = "Stops the time analyzer for a certain module",
    .synopsis = NULL,
    .examples = "module_id=4"
};

/* *INDENT-ON* */

/**
 * \brief - time analyzer stop command
 */

shr_error_e
cmd_dnx_time_analyzer_stop(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int module_id = -1;

    int module_idx = 0;
    int last_module = 0;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_INT32("module_id", module_id);

    if (module_id < 0)
    {
        module_idx = 0;
        last_module = UTILEX_TIME_ANALYZER_MAX_NOF_MODULES;
    }
    else
    {
        module_idx = module_id;
        last_module = module_id + 1;
    }

    for (module_idx = module_id; module_idx < last_module; module_idx++)
    {
        utilex_time_analyzer_module_stop(unit, module_idx);
    }
    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

/*
 * Remove
 */
/* *INDENT-OFF* */

sh_sand_man_t dnx_time_analyzer_remove_man = {
    .brief    = "Remove a module",
    .full     = "Remove a module / operation to capture statistics for",
    .synopsis = NULL,
    .examples = "module_id=4"
};

/* *INDENT-ON* */

/**
 * \brief - time analyzer remove command
 */

shr_error_e
cmd_dnx_time_analyzer_remove(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int module_id = -1;

    int module_idx = 0;
    int last_module = 0;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_INT32("module_id", module_id);

    if (module_id < 0)
    {
        module_idx = 0;
        last_module = UTILEX_TIME_ANALYZER_MAX_NOF_MODULES;
    }
    else
    {
        module_idx = module_id;
        last_module = module_id + 1;
    }

    for (module_idx = module_id; module_idx < last_module; module_idx++)
    {
        utilex_time_analyzer_module_remove(unit, module_idx);
    }

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

sh_sand_option_t sh_dnx_time_analyzer_start_stop_remove_options[] = {
    {"module_id", SAL_FIELD_TYPE_INT32, "Id of module", "-1"}
    ,
    {NULL}
};

/*
 * Add
 */
/* *INDENT-OFF* */

sh_sand_man_t dnx_time_analyzer_add_man = {
    .brief    = "Add a module",
    .full     = "Add a module / operation to capture statistics for",
    .synopsis = NULL,
    .examples = "module_id=4\n"
};

/* *INDENT-ON* */

/**
 * \brief - time analyzer add command
 */

shr_error_e
cmd_dnx_time_analyzer_add(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int module_id = -1;

    int module_idx = 0;
    int last_module = 0;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_INT32("module_id", module_id);

    if (module_id < 0)
    {
        module_idx = 0;
        last_module = COUNTOF(diag_dnx_init_time_analyzer_ops);
    }
    else
    {
        module_idx = module_id;
        last_module = module_id + 1;
    }

    for (module_idx = module_id; module_idx < last_module; module_idx++)
    {
        utilex_time_analyzer_module_add(unit, module_idx, diag_dnx_init_time_analyzer_ops[module_idx].name);
    }
    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

sh_sand_option_t sh_dnx_time_analyzer_add_options[] = {
    {"module_id", SAL_FIELD_TYPE_INT32, "Id of module", "-1"}
    ,
    {NULL}
};

/*
 * Diag info get
 */
/* *INDENT-OFF* */

sh_sand_man_t dnx_time_analyzer_module_list_man = {
    .brief    = "Statistics",
    .full     = "Get statistics for all modules / operations",
    .synopsis = NULL, 
};

/* *INDENT-ON* */

/**
 * \brief - time analyzer diag_info command
 */
shr_error_e
cmd_dnx_time_analyzer_module_list(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int module_idx;
    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Print time analyzer statistics table
     */
    PRT_TITLE_SET("Time Analyzer");
    PRT_COLUMN_ADD("ID");
    PRT_COLUMN_ADD("Name");

    for (module_idx = 0; module_idx < COUNTOF(diag_dnx_init_time_analyzer_ops); module_idx++)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("%d", diag_dnx_init_time_analyzer_ops[module_idx].module_id);
        PRT_CELL_SET("%s", diag_dnx_init_time_analyzer_ops[module_idx].name);
    }

    PRT_COMMITX;
exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/* *INDENT-OFF* */
sh_sand_man_t sh_dnx_time_analyzer_man = {
    .brief =    "Various Time Analyzer controls",
    .full =     "Used to control Time Analyzer - special tool to analyze operation times"
};

sh_sand_cmd_t sh_dnx_time_analyzer_cmds[] = {
    /*
     * keyword, action, command, options, man callback legacy
     */
    {"dump",        cmd_dnx_time_analyzer_dump,        NULL,   NULL,                                             &dnx_time_analyzer_dump_man},
    {"clear",       cmd_dnx_time_analyzer_clear,       NULL,   NULL,                                             &dnx_time_analyzer_clear_man},
    {"start",       cmd_dnx_time_analyzer_start,       NULL,   sh_dnx_time_analyzer_start_stop_remove_options,   &dnx_time_analyzer_start_man},
    {"stop",        cmd_dnx_time_analyzer_stop,        NULL,   sh_dnx_time_analyzer_start_stop_remove_options,   &dnx_time_analyzer_stop_man},
    {"add",         cmd_dnx_time_analyzer_add,         NULL,   sh_dnx_time_analyzer_add_options,                 &dnx_time_analyzer_add_man},
    {"remove",      cmd_dnx_time_analyzer_remove,      NULL,   sh_dnx_time_analyzer_start_stop_remove_options,   &dnx_time_analyzer_remove_man},
    {"module_list", cmd_dnx_time_analyzer_module_list, NULL,   NULL,                                             &dnx_time_analyzer_module_list_man,      NULL, NULL, SH_CMD_NO_XML_VERIFY},
    {NULL}
};
/* *INDENT-ON* */
/*
 * }
 */
