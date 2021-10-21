/**
 * \file diag_dnx_egr_mirror_rcy.c
 *
 * diagnostics for egress mirror recycle
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_APPLDNX_COSQ

/*
 * INCLUDE FILES:
 * {
 */
/** appl */
#include <appl/diag/diag.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_prt.h>
 /** bcm */
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
 /** sal */
#include <sal/appl/sal.h>
 /** local */
#include "diag_dnx_egr_mirror_rcy.h"
/**  soc  */
#include <soc/intr.h>
/*
 * }
 */

sh_sand_option_t sh_dnx_egr_mirror_rcy_options[] = {
    /*
     * name type desc default ext 
     */
    {"port", SAL_FIELD_TYPE_PORT, "Forward Port", "all", NULL}
    ,
    {NULL}
};

sh_sand_man_t sh_dnx_egr_mirror_rcy_man = {
    .brief = "prints the mapping between forwarding port and mirror recycle port",
    .full = "prints the mapping between forwarding port and mirror recycle port",
    .synopsis = "[port=<port number>]",
    .examples = "\n" "port=13"
};

/*
 * see header
 */
shr_error_e
sh_dnx_tm_egr_mirror_rcy(
    int unit,
    bcm_pbmp_t logical_ports,
    sh_sand_control_t * sand_control)
{
    bcm_port_t logical_port;
    int isHigh, isLow;
    bcm_mirror_port_to_rcy_map_info_t rcy_map_info;
    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Print table header
     */
    PRT_TITLE_SET("MIRROR RECYCLE PORT");
    PRT_COLUMN_ADD("Forward Port");
    PRT_COLUMN_ADD("RCY Mirror Port");
    PRT_COLUMN_ADD("Priority High");
    PRT_COLUMN_ADD("Priority Low");

    /*
     * table
     */
    BCM_PBMP_ITER(logical_ports, logical_port)
    {
        /*
         * Get data
         */
        SHR_IF_ERR_EXIT(bcm_mirror_port_to_rcy_port_map_get(unit, 0, logical_port, &rcy_map_info));

        if (rcy_map_info.rcy_mirror_port == -1)
        {
            continue;
        }

        isHigh = (rcy_map_info.priority_bitmap & BCM_MIRROR_PORT_TO_RCY_MAP_INFO_PRIORITY_HIGH) > 0;
        isLow = (rcy_map_info.priority_bitmap & BCM_MIRROR_PORT_TO_RCY_MAP_INFO_PRIORITY_NORMAL) > 0;

        /*
         * table line
         */
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);

        PRT_CELL_SET("%d", logical_port);
        PRT_CELL_SET("%d", rcy_map_info.rcy_mirror_port);
        PRT_CELL_SET("%d", isHigh);
        PRT_CELL_SET("%d", isLow);
    }

    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/*
 * see header
 */
shr_error_e
sh_dnx_tm_egr_mirror_rcy_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    bcm_pbmp_t logical_ports;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_PORT("port", logical_ports);

    SHR_IF_ERR_EXIT(sh_dnx_tm_egr_mirror_rcy(unit, logical_ports, sand_control));

exit:
    SHR_FUNC_EXIT;
}
