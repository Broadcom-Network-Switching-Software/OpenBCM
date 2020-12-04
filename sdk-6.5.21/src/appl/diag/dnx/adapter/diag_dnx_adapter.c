/** \file diag_dnx_adapter.c
 *
 * Main diagnostics for adapter applications
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_CNT

/*
 * {
 */
/*************
 * INCLUDES  *
 */
/** appl */
#include <appl/diag/diag.h>
#include <appl/diag/sand/diag_sand_framework.h>

/** bcm */
#include <soc/dnx/adapter/adapter_reg_access.h>

/** sal */
#include <sal/appl/sal.h>

/** local */
#include "diag_dnx_adapter.h"

static shr_error_e
sh_dnx_adapter_close_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(adapter_terminate_server(unit));

exit:
    SHR_FUNC_EXIT;
}

sh_sand_man_t sh_dnx_adapter_close_man = {
    .brief = "Terminates(Disconnects) the adapter`s server",
};

/**
 * \brief DNX Adapter diagnostics
 * List of the supported commands, pointer to command function and command usage function. 
 */
sh_sand_cmd_t sh_dnx_adapter_cmds[] = {
    /*
     * keyword,        action,          command,           options,                 man
     */
    {"close", sh_dnx_adapter_close_cmd, NULL, NULL, &sh_dnx_adapter_close_man, NULL, NULL, SH_CMD_SKIP_EXEC},
    {NULL}
};

sh_sand_man_t sh_dnx_adapter_man = {
    .brief = "Adapter diagnostics and commands",
};

/*
 * }
 */
