/** \file diag_dnx_l3.c
 *
 * main file for l3 diagnostics
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_DIAG

/*
 * INCLUDE FILES:
 * {
 */
/** appl */
#include <appl/diag/sand/diag_sand_framework.h>
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
    {"route", NULL, sh_dnx_l3_route_cmds, NULL, &sh_dnx_l3_route_man}
    ,
    {"host", NULL, sh_dnx_l3_host_cmds, NULL, &sh_dnx_l3_host_man}
    ,
    {"EGress", NULL, sh_dnx_l3_egr_cmds, NULL, &sh_dnx_l3_egr_man}
    ,
    {NULL}
};

sh_sand_man_t sh_dnx_l3_man = {
    .brief = "L3 commands"
};
