/** \file diag_dnxc_appl.c
 * 
 * DNX APPL DIAG PACK - diagnostic pack for application reference
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_APPL_SHELL

/*
 * INCLUDE FILES:
 * {
 */
/** appl */
#include <appl/diag/sand/diag_sand_framework.h>
/** bcm */
/** sal */
#include <sal/appl/sal.h>
/** local */
#include "diag_dnxc_appl_run.h"
#include "diag_dnxc_appl_list.h"
/*
 * }
 */

 /*
  * LOCAL DIAG PACK:
  * {
  */

/**
 * DNX PORT diagnostic pack
 * List of the supported commands, pointer to command function and command usage function.
 */

sh_sand_cmd_t sh_dnxc_appl_cmds[] = {
    /*
     * keyword, action, command, options, man 
     */
    {"run", sh_dnxc_appl_run_cmd, NULL, sh_dnxc_appl_run_options, &sh_dnxc_appl_run_man, NULL, NULL,
     SH_CMD_NO_XML_VERIFY | SH_CMD_SKIP_EXEC}
    ,
    {"list", sh_dnxc_appl_list_cmd, NULL, NULL, &sh_dnxc_appl_list_man, NULL, NULL,
     SH_CMD_NO_XML_VERIFY | SH_CMD_SKIP_EXEC}
    ,
    {NULL}
};

sh_sand_man_t sh_dnxc_appl_man = {
    .brief = "Reference application diagnostic pack"
};

/*
 * }
 */
