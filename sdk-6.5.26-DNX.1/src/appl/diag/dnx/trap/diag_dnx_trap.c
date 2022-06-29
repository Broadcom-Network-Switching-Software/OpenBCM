/** \file diag_dnx_trap.c
 *
 * main file for trap diagnostics
 *
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
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
#include <appl/diag/diag.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_prt.h>
#include "include/bcm_int/dnx/rx/rx_trap.h"
#include "diag_dnx_trap.h"
/** bcm */
#include <shared/bslnames.h>
#include <bcm/tunnel.h>
#include <bcm/l3.h>
/** sal */
#include <sal/appl/sal.h>
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

void
dnx_egress_trap_id_to_string(
    int trap_id,
    char *trap_id_str)
{

    if ((trap_id == BCM_RX_TRAP_EG_TX_TRAP_ID_DEFAULT) || (trap_id == BCM_RX_TRAP_EG_TRAP_ID_DEFAULT) || (trap_id == 0))
    {
        sal_strncpy(trap_id_str, "DEFAULT", DIAG_DNX_TRAP_ID_STRING_SIZE);
    }
    else if (trap_id == BCM_RX_TRAP_EG_TX_TRAP_ID_DROP || (trap_id == 1))
    {
        sal_strncpy(trap_id_str, "DROP", DIAG_DNX_TRAP_ID_STRING_SIZE);
    }
    else
    {
        sal_snprintf(trap_id_str, DIAG_DNX_TRAP_ID_STRING_SIZE, "%d", trap_id);
    }

}

const sh_sand_enum_t Trap_block_enum_table[] = {
    /**String      Value                                      Description*/
    {"IRPP", DNX_RX_TRAP_BLOCK_INGRESS, "Ingress block"}
    ,
    {"ERPP", DNX_RX_TRAP_BLOCK_ERPP, "ERPP block"}
    ,
    {"ETPP", DNX_RX_TRAP_BLOCK_ETPP, "ETPP block"}
    ,
    {"ALL", DNX_RX_TRAP_BLOCK_NUM_OF, "All blocks"}
    ,
    {NULL}
};

static sh_sand_man_t sh_dnx_trap_list_man = {
    .brief = "List traps per different parameters",
    .full = "List trap per type and block\n",
};

static sh_sand_man_t sh_dnx_trap_map_man = {
    .brief = "Map info of traps per different parameters",
    .full = "Map info of trap per type and block\n",
};

static sh_sand_man_t sh_dnx_trap_action_man = {
    .brief = "Show trap's action info",
    .full = "Show trap's action info per block and trap_id\n",
};

static sh_sand_man_t sh_dnx_trap_last_pkt_man = {
    .brief = "Show last packet trap info",
    .full = "Show last packet trap info per block\n",
};

/**
 * \brief DNX Traps diagnostics
 * List of the supported commands, pointer to command function and command usage function.
 * This is the entry point for Traps diagnostic commands
 */
sh_sand_cmd_t sh_dnx_trap_cmds[] = {
    /*
     * keyword, action, command, options, man callback legacy
     */
    {"LIST", NULL, sh_dnx_trap_list_cmds, NULL, &sh_dnx_trap_list_man, NULL, NULL, SH_CMD_NO_XML_VERIFY}
    ,
    {"MAPping", NULL, sh_dnx_trap_map_cmds, NULL, &sh_dnx_trap_map_man, NULL, NULL, SH_CMD_NO_XML_VERIFY}
    ,
    {"ACTion", NULL, sh_dnx_trap_action_cmds, NULL, &sh_dnx_trap_action_man, NULL, NULL, SH_CMD_NO_XML_VERIFY}
    ,
    {"LAST", NULL, sh_dnx_trap_last_pkt_cmds, NULL, &sh_dnx_trap_last_pkt_man}
    ,
    {NULL}
};

sh_sand_man_t sh_dnx_trap_man = {
    .brief = "trap commands",
};
