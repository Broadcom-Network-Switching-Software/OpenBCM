/** \file diag_dnx_allocation_mngr.c
 *
 * Diag alias for listing all resource and template manager.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

/*
 * {
 */
/*************
 * INCLUDES  *
 */
/** appl */
#include <appl/diag/diag.h>
#include <appl/diag/sand/diag_sand_framework.h>

/** sal */
#include <sal/appl/sal.h>

/** local */
#include "diag_dnx_allocation_mngr.h"

#define DIAG_DNX_ALLOCATION_MNGR_COMMAND_LENGTH 1024

static shr_error_e
sh_dnx_allocation_mngr_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    char *name;
    int is_template;
    int is_resource;
    /*
     * Length
     */
    int command_length;
    char command[DIAG_DNX_ALLOCATION_MNGR_COMMAND_LENGTH];
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Init local variables.
     */
    is_template = 0;
    is_resource = 0;
    command_length = 1;
    sal_memset(command, 0, sizeof(command));

    /*
     * Get parameters
     */
    SH_SAND_GET_STR("name", name);
    SH_SAND_GET_BOOL("template", is_template);
    SH_SAND_GET_BOOL("resource", is_resource);

    sal_strncpy(command, "swstate dump nocontent typefilter=\"", sizeof(command) - command_length);
    command_length = sal_strnlen(command, DIAG_DNX_ALLOCATION_MNGR_COMMAND_LENGTH) + 1;

    if (is_template == is_resource)
    {
        sal_strncat_s(command, "dnx_algo_res_t|dnx_algo_template_t\"", sizeof(command));
        command_length = sal_strnlen(command, DIAG_DNX_ALLOCATION_MNGR_COMMAND_LENGTH) + 1;
    }
    else if (is_resource)
    {
        sal_strncat_s(command, "dnx_algo_res_t\"", sizeof(command));
        command_length = sal_strnlen(command, DIAG_DNX_ALLOCATION_MNGR_COMMAND_LENGTH) + 1;
    }
    else
    {
        sal_strncat_s(command, "dnx_algo_template_t\"", sizeof(command));
        command_length = sal_strnlen(command, DIAG_DNX_ALLOCATION_MNGR_COMMAND_LENGTH) + 1;
    }

    if (!ISEMPTY(name))
    {
        sal_strncat_s(command, " namefilter=\"", sizeof(command));
        command_length = sal_strnlen(command, DIAG_DNX_ALLOCATION_MNGR_COMMAND_LENGTH) + 1;

        sal_strncat_s(command, name, sizeof(command));
        command_length = sal_strnlen(command, DIAG_DNX_ALLOCATION_MNGR_COMMAND_LENGTH) + 1;

        sal_strncat_s(command, "\"", sizeof(command));
        command_length = sal_strnlen(command, DIAG_DNX_ALLOCATION_MNGR_COMMAND_LENGTH) + 1;
    }

    sh_process_command(unit, command);

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief list Allocation mngr commands.
 */
sh_sand_cmd_t sh_dnx_allocation_mngr_cmds[] = {
    {"list", sh_dnx_allocation_mngr_cmd, NULL, sh_dnx_allocation_mngr_options, &sh_dnx_allocation_mngr_man}
    ,
    {NULL}
};

sh_sand_man_t sh_dnx_allocation_mngr_man = {
    .brief = "List aliases for resource and template manager diag commands.",
    .full = "AllocationManager list",
    .synopsis = NULL,
    .examples = "template\n" "resource\n" "name=example"
};

/*
 * Supported options.
 */
sh_sand_option_t sh_dnx_allocation_mngr_options[] = {
    {"template", SAL_FIELD_TYPE_BOOL, "list template mngrs", "No"}
    ,
    {"resource", SAL_FIELD_TYPE_BOOL, "list resource mngrs", "No"}
    ,
    {"name", SAL_FIELD_TYPE_STR, "Filter for the mngr name", ""}
    ,
    {NULL}
};

/*
 * }
 */
