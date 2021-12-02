/** \file diag_dnx_l3.c
 *
 * main file for l3 diagnostics
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
#define BSL_LOG_MODULE BSL_LS_SOCDNX_DIAG

/*
 * INCLUDE FILES:
 * {
 */
/** appl */
#include <appl/diag/sand/diag_sand_framework.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_l3.h>
#include "diag_dnx_l3.h"
/*
 * }
 */
/*
 * Defines
 * {
 */
/*
 * }
 */
/*
 * External
 * {
 */

/*
 * }
 */
static sh_sand_man_t sh_dnx_l3_ecmp_man = {
    .brief = "Access ECMP group entries",
    .full = "Create, modify and list existing ECMP groups, their profiles and existing FEC members\n",
};

static sh_sand_man_t sh_dnx_l3_route_man = {
    .brief = "Access UC route entries\n",
    .full = "Access UC IPv4 and IPv6 route table entries - create, modify, remove and find.\n",
};

static sh_sand_man_t sh_dnx_l3_intf_man = {
    .brief = "Access interface table entries",
    .full = "Access interface table entries - create, modify, remove and find.",
};

static sh_sand_man_t sh_dnx_l3_host_man = {
    .brief = "Access UC host entries\n",
    .full = "Access UC IPv4 and IPv6 host table entries - create, modify, remove and find.\n",
};

static sh_sand_man_t sh_dnx_l3_egr_man = {
    .brief = "Access egress interface entries\n",
    .full = "Access egress interface FEC table entries - create, modify, remove and find.\n",
};

/**
 * \brief
 *   Conditional check for l3 host command support
 * \param [in] unit - unit ID
 * \param [in] test_list - test list
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error detected. See \ref shr_error_e
 */
static shr_error_e
sh_dnx_l3_is_host_command_supported(
    int unit,
    rhlist_t * test_list)
{
    SHR_FUNC_INIT_VARS(unit);

    if (FALSE == dnx_data_l3.fwd.feature_get(unit, dnx_data_l3_fwd_host_entry_support))
    {
        SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
    }

    SHR_FUNC_EXIT;
}

/**
 * \brief DNX PP diagnostics
 * List of the supported commands, pointer to command function and command usage function.
 * This is the entry point for PP diagnostic commands
 */
sh_sand_cmd_t sh_dnx_l3_cmds[] = {
    /*
     * keyword, action, command, options, man callback legacy
     */
    {"ECMP", NULL, sh_dnx_l3_ecmp_cmds, NULL, &sh_dnx_l3_ecmp_man}
    ,
    {"INTerFace", NULL, sh_dnx_l3_intf_cmds, NULL, &sh_dnx_l3_intf_man}
    ,
    {"route", NULL, sh_dnx_l3_route_cmds, NULL, &sh_dnx_l3_route_man, NULL, NULL, SH_CMD_SKIP_MEMORY_CHECK}
    ,
    {"host", NULL, sh_dnx_l3_host_cmds, NULL, &sh_dnx_l3_host_man, NULL, NULL, SH_CMD_CONDITIONAL,
     sh_dnx_l3_is_host_command_supported}
    ,
    {"EGress", NULL, sh_dnx_l3_egr_cmds, NULL, &sh_dnx_l3_egr_man}
    ,
    {NULL}
};

sh_sand_man_t sh_dnx_l3_man = {
    .brief = "L3 commands"
};
