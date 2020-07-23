/** \file diag_dnx_nif.c
 *
 * main file for nif diagnostics
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*
 * INCLUDE FILES:
 * {
 */
/** appl */
#include <appl/diag/sand/diag_sand_framework.h>
#include <bcm_int/dnx/init/init.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_nif.h>
#include "diag_dnx_nif.h"

/** local */
#include "diag_dnx_nif_lane_map.h"
#include "diag_dnx_nif_status.h"
#include "diag_dnx_nif_txrx.h"

/*
 * }
 */
shr_error_e
sh_dnx_is_ilkn_suported(
    int unit,
    rhlist_t * test_list)
{
    SHR_FUNC_INIT_VARS(unit);

    if (dnx_init_is_init_done_get(unit))
    {
        if (!(dnx_data_nif.ilkn.feature_get(unit, dnx_data_nif_ilkn_is_supported)))
        {
            SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
        }
    }
    SHR_FUNC_EXIT;
}
/**
 * \brief DNX NIF diagnostics
 * List of the supported commands, pointer to command function and command usage function.
 * This is the entry point for NIF diagnostic commands
 */

/* *INDENT-OFF* */

static sh_sand_man_t sh_dnx_nif_ilkn_man = {
    .brief    = "NIF ILKN commands"
};

static sh_sand_cmd_t sh_dnx_nif_ilkn_cmds[] = {
    /*keyword,   action,                       command, options,                             man                     */
    {"lane_map", sh_dnx_nif_ilkn_lane_map_cmd, NULL,    sh_dnx_nif_ilkn_lane_map_options,    &sh_dnx_nif_ilkn_lane_map_man, NULL, NULL, SH_CMD_CONDITIONAL, sh_dnx_is_ilkn_suported},
    {"rx",       sh_dnx_nif_ilkn_rx_cmd,       NULL,    sh_dnx_nif_ilkn_rx_options,          &sh_dnx_nif_ilkn_rx_man,       NULL, NULL, SH_CMD_CONDITIONAL, sh_dnx_is_ilkn_suported},
    {"tx",       sh_dnx_nif_ilkn_tx_cmd,       NULL,    sh_dnx_nif_ilkn_tx_options,          &sh_dnx_nif_ilkn_tx_man,       NULL, NULL, SH_CMD_CONDITIONAL, sh_dnx_is_ilkn_suported},
    {NULL}
};

sh_sand_cmd_t sh_dnx_nif_cmds[] = {
    /*keyword,   action,                  command,              options,                     man*/
    {"lane_map", sh_dnx_nif_lane_map_cmd, NULL,                 sh_dnx_nif_lane_map_options, &sh_dnx_nif_lane_map_man, NULL, NULL, SH_CMD_NO_XML_VERIFY},
    {"ilkn",     NULL,                    sh_dnx_nif_ilkn_cmds, NULL,                        &sh_dnx_nif_ilkn_man,  NULL, NULL, SH_CMD_CONDITIONAL, sh_dnx_is_ilkn_suported},
    {"status",   sh_dnx_nif_status_cmd,   NULL,                 sh_dnx_nif_status_options,   &sh_dnx_nif_status_man, NULL, NULL, SH_CMD_NO_XML_VERIFY},
    {"tx",       sh_dnx_nif_tx_cmd,       NULL,                 sh_dnx_nif_tx_options,       &sh_dnx_nif_tx_man},
    {"rx",       sh_dnx_nif_rx_cmd,       NULL,                 sh_dnx_nif_rx_options,       &sh_dnx_nif_rx_man},
    {NULL}
};

sh_sand_man_t sh_dnx_nif_man = {
    .brief    = "NIF commands"};

/* *INDENT-ON* */
