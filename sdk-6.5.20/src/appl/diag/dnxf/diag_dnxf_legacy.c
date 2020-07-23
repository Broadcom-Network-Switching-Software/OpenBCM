/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/** \file diag_dnxf_legacy.c
 *
 * Purpose: List of commands specific for SOC_IS_DNXF devices
 */

#include <appl/diag/sand/diag_sand_framework.h>

/* *INDENT-OFF* */
sh_sand_legacy_cmd_t sh_dnxf_legacy_commands[] = {
    {
        .command = "INIT",
        .message = "init / deinit sequence might be controlled with shell command 'init_dnx'"
    },
    {
        .command = "DEInit",
        .message = "init / deinit sequence might be controlled with shell command 'init_dnx'"
    },
    {
        .command = "DETach",
        .message = "init / deinit sequence might be controlled with shell command 'init_dnx'"
    },
    {
        .command = "PROBE",
        .message = "init / deinit sequence might be controlled with shell command 'init_dnx'"
    },
    {
        .command = "REINIT",
        .message = "init / deinit sequence might be controlled with shell command 'init_dnx'"
    },
    {
        .command = "PortStat",
        .message = "Command is not supported. Use 'port status <pbmp>' instead"
    },
    {
        .command = "Fabric PROPerties",
        .message = "Command is not supported. Use 'data dump property *' instead"
    },
    {
        .command = "SwitchControl",
        .message = "Command is not supported. Use API bcm_switch_control_set()/get() instead"
    },
    {NULL} /* This line should always stay as last one */
};
/* *INDENT-ON* */
