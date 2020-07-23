/** \file diag_dnx_port.c
 * 
 * DNX PORT DIAG PACK - diagnostic pack for module port
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_APPL_PORT

/*
 * INCLUDE FILES:
 * {
 */
/** appl */
#include <appl/diag/diag.h>
#include <appl/diag/sand/diag_sand_framework.h>
/** bcm */
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
/** sal */
#include <sal/appl/sal.h>
/** local */
#include "diag_dnx_port.h"
#include "diag_dnx_port_mgmt.h"
#include "diag_dnx_port_autoneg.h"
#include "../../dnxc/diag_dnxc.h"
/*
 * }
 */

/**
 * DNX PORT diagnostic pack
 * List of the supported commands, pointer to command function and command usage function.
 */

/* *INDENT-OFF* */
sh_sand_cmd_t sh_dnx_port_cmds[] = {
    /*
     * keyword, action, command, options, man callback legacy
     */
    {"ManaGeMenT", NULL,                    sh_dnx_port_mgmt_cmds, NULL,                          &sh_dnx_port_mgmt_man},
    {"status",     cmd_dnxc_port_status,    NULL,                  sh_dnxc_port_status_options,   &sh_dnxc_port_status_man, NULL, NULL, SH_CMD_NO_XML_VERIFY},
    {"loopback",   cmd_dnxc_port_loopback,  NULL,                  sh_dnxc_port_loopback_options, &sh_dnxc_port_loopback_man},
    {"autoneg",    sh_dnx_port_autoneg_cmd, NULL,                  sh_dnx_port_autoneg_options,   &sh_dnx_port_autoneg_man},
    {"enable",     cmd_dnxc_port_enable,    NULL,                  sh_dnxc_port_enable_options,   &sh_dnxc_port_enable_man},
    {"portmod",    NULL,                    sh_dnxc_port_pm_cmds,  NULL,                          &sh_dnxc_port_pm_man},
    {NULL}
};

sh_sand_man_t sh_dnx_port_man = {
    .brief = "Diagnostic pack for port module",
    .full = NULL
};

/* *INDENT-ON* */
/*
 * }
 */
