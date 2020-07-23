/** \file diag_dnx_nif_lane_map.c
 * 
 * diagnostics for NIF Lane Map
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
 * Include files.
 * {
 */
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_prt.h>
#include <shared/util.h>
#include <shared/shrextend/shrextend_debug.h>
#include <sal/appl/sal.h>

#include <bcm_int/dnx/algo/lane_map/algo_lane_map.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/algo/port/algo_port_verify.h>
#include <bcm_int/dnx/port/imb/imb.h>
#include <bcm_int/dnx/port/imb/imb_common.h>

#include <soc/dnx/dnx_data/auto_generated/dnx_data_port.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_nif.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_fabric.h>

#include "diag_dnx_nif_lane_map.h"
#include "../../dnxc/diag_dnxc.h"

/*
 * }
 */

/*
 * Macros
 * {
 */

#define  DNX_DIAG_LANE_MAP_NO_OPTION      -1
#define  DNX_DIAG_LANE_MAP_MAX_STR_LEN    8
#define  DNX_DIAG_LANE_MAP_MAX_ILKN_IF    (DNX_DATA_MAX_NIF_ILKN_ILKN_UNIT_IF_NOF * DNX_DATA_MAX_NIF_ILKN_ILKN_UNIT_NOF)

/*
 * }
 */

/**
 * \brief - Dump the NIF mapped lane info
 *
 * \param [in] unit - chip unit id
 * \param [in] args - diag command arguments
 * \param [in] sand_control - diag command control
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */

shr_error_e
sh_dnx_nif_lane_map_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int print_lane;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_INT32("lane", print_lane);

    SHR_IF_ERR_EXIT(cmd_dnxc_lane_map_dump(unit, DIAG_DNXC_LANE_TO_SERDES_NIF_SIDE, print_lane, sand_control));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Dump the ILKN lane map info
 *
 * \param [in] unit - chip unit id
 * \param [in] args - diag command arguments
 * \param [in] sand_control - diag command control
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */

shr_error_e
sh_dnx_nif_ilkn_lane_map_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int ilkn_id, ilkn_id_tmp, actual_size;
    int is_over_fabric;
    int nof_lanes;
    int lane, count, i;
    int interface_base_phy;
    bcm_pbmp_t ilkn_phys;
    bcm_port_t ilkn_port;
    bcm_pbmp_t ilkn_id_bmp;
    bcm_pbmp_t ilkn_port_bmp;
    int ilkn_lane_id_array[DNX_DATA_MAX_NIF_ILKN_LANES_MAX_NOF];
    int ilkn_lane_id_array_reverse[DNX_DATA_MAX_NIF_ILKN_LANES_MAX_NOF];
    int active_lanes[DNX_DATA_MAX_NIF_ILKN_LANES_MAX_NOF];
    int ilkn_id_port_map[DNX_DIAG_LANE_MAP_MAX_ILKN_IF] = { 0 };
    char str_base[DNX_DIAG_LANE_MAP_MAX_STR_LEN] = { 0 };
    soc_dnxc_lane_map_db_map_t *lane2serdes = NULL;
    dnx_algo_lane_map_type_e type;
    const dnx_data_nif_ilkn_supported_phys_t *interface_supported_phys;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_INT32("ilkn_id", ilkn_id);
    /*
     * Verify the ILKN Id if the ilkn_id is specified
     */
    if (ilkn_id != DNX_DIAG_LANE_MAP_NO_OPTION)
    {
        if ((ilkn_id < 0)
            || (ilkn_id >= dnx_data_nif.ilkn.ilkn_unit_nof_get(unit) * dnx_data_nif.ilkn.ilkn_unit_if_nof_get(unit)))
        {
            SHR_CLI_EXIT(_SHR_E_PARAM, "ILKN ID: %d is out of bound, please give correct ILKN id \n", ilkn_id);
        }
    }
    /*
     * Get the ILKN id to port mapping
     */
    BCM_PBMP_CLEAR(ilkn_id_bmp);
    SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get
                    (unit, BCM_CORE_ALL, DNX_ALGO_PORT_LOGICALS_TYPE_NIF_ILKN, 0, &ilkn_port_bmp));
    BCM_PBMP_ITER(ilkn_port_bmp, ilkn_port)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_interface_offset_get(unit, ilkn_port, &ilkn_id_tmp));
        if ((ilkn_id == DNX_DIAG_LANE_MAP_NO_OPTION) || (ilkn_id == ilkn_id_tmp))
        {
            ilkn_id_port_map[ilkn_id_tmp] = ilkn_port;
            BCM_PBMP_PORT_ADD(ilkn_id_bmp, ilkn_id_tmp);
        }
    }

    /*
     * Print table header
     */
    PRT_TITLE_SET("ILKN Lane Map");

    PRT_COLUMN_ADD("ILKN ID#");
    PRT_COLUMN_ADD("ILKN Lane ID#");
    PRT_COLUMN_ADD("Lane#");
    PRT_COLUMN_ADD("SRD_RX#");
    PRT_COLUMN_ADD("SRD_TX#");

    BCM_PBMP_ITER(ilkn_id_bmp, ilkn_id_tmp)
    {
        /*
         * Specify the different attributes for ILKN over fabric ports
         */
        is_over_fabric = dnx_data_nif.ilkn.properties_get(unit, ilkn_id_tmp)->is_over_fabric;
        sal_memset(str_base, 0, sizeof(str_base));
        if (is_over_fabric)
        {
            type = DNX_ALGO_LANE_MAP_FABRIC_SIDE;
            nof_lanes = dnx_data_fabric.links.nof_links_get(unit);
            /** For over fabric port, should print lane number like: "fabric:<lane_id>" */
            sal_sprintf(str_base, "fabric:");
        }
        else
        {
            type = DNX_ALGO_LANE_MAP_NIF_SIDE;
            nof_lanes = dnx_data_nif.phys.nof_phys_get(unit);
        }
        SHR_ALLOC(lane2serdes, nof_lanes * sizeof(soc_dnxc_lane_map_db_map_t), "Lane to serdes map", "%s%s%s\r\n",
                  EMPTY, EMPTY, EMPTY);
        /*
         * Get the lane map info from Lane Map DB
         */
        SHR_IF_ERR_EXIT(dnx_algo_lane_map_lane_to_serdes_map_get(unit, type, nof_lanes, lane2serdes));
        /*
         * ilkn_id_bmp has been clear before setting it. Therefore ilkn_id_tmp cannot
         * exceed the array size.
         */
         /* coverity[overrun-local : FALSE]  */
        SHR_IF_ERR_EXIT(dnx_algo_port_nif_ilkn_lanes_get(unit, ilkn_id_port_map[ilkn_id_tmp], &ilkn_phys));
        /*
         * Store the active ILKN phy lanes by order
         */
        count = 0;
        BCM_PBMP_ITER(ilkn_phys, lane)
        {
            active_lanes[count] = lane;
            count++;
        }
        /*
         * Get the ILKN lane order
         */
        SHR_IF_ERR_EXIT(imb_port_logical_lane_order_get
                        (unit, ilkn_id_port_map[ilkn_id_tmp], count, ilkn_lane_id_array, &actual_size));
        /*
         * The ilkn_lane_id_array needs to be reversed, as the
         * array value represents the ilkn_lane_id, the array indicator represents
         * the actual lane order.
         */
        for (i = 0; i < count; ++i)
        {
            ilkn_lane_id_array_reverse[ilkn_lane_id_array[i]] = i;
        }
        /*
         * Get ILKN base PHY ID
         */
        interface_supported_phys = dnx_data_nif.ilkn.supported_phys_get(unit, ilkn_id_tmp);
        if (is_over_fabric)
        {
            _SHR_PBMP_FIRST(interface_supported_phys->fabric_phys, interface_base_phy);
        }
        else
        {
            _SHR_PBMP_FIRST(interface_supported_phys->nif_phys, interface_base_phy);
        }
        /*
         * Print the table
         */
        for (i = 0; i < count; ++i)
        {
            /** Only print the ilkn_id for the first lane */
            if (i == 0)
            {
                PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
                PRT_CELL_SET("%d", ilkn_id_tmp);
            }
            else
            {
                PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                PRT_CELL_SET("%s", "");
            }
            /** Get the lane number after remapping and convert it the logical lane ID */
            lane = active_lanes[ilkn_lane_id_array_reverse[i]] + interface_base_phy;

            PRT_CELL_SET("%02d", i);
            PRT_CELL_SET("%s%02d", str_base, lane);
            PRT_CELL_SET("%s%02d", str_base, lane2serdes[lane].rx_id);
            PRT_CELL_SET("%s%02d", str_base, lane2serdes[lane].tx_id);
        }
        SHR_FREE(lane2serdes);
    }
    PRT_COMMITX;
exit:
    PRT_FREE;
    SHR_FREE(lane2serdes);
    SHR_FUNC_EXIT;

}

/**
 * \brief DNX NIF Lane Map diagnostics
 * List of the supported commands, pointer to command function and command usage function.
 * This is the entry point for NIF lane map diagnostic commands 
 */

/* *INDENT-OFF* */

sh_sand_man_t sh_dnx_nif_lane_map_man = {
    .brief    = "Diagnostic for NIF Lane Map, only dump the MAPPED lanes",
    .full     = "Include the attached ports info for each lane",
    .synopsis = "[lane=<lane_id>]",
    .examples = "\n"
                "lane=0"
};

sh_sand_option_t sh_dnx_nif_lane_map_options[] = {
    /*name       type                  desc          default    ext*/
    {"lane",     SAL_FIELD_TYPE_INT32, "Lane ID",    "-1",      NULL},
    {NULL}
};

sh_sand_man_t sh_dnx_nif_ilkn_lane_map_man = {
    .brief    = "Diagnostic for ILKN lane map for given ILKN port",
    .full     = "Include the Ilkn lane order INFO and logical to physical mapping",
    .synopsis = "[ilkn_id=<ilkn_id>]",
    .examples = "\n"
                "ilkn_id=0"
};

sh_sand_option_t sh_dnx_nif_ilkn_lane_map_options[] = {
    /*name           type                  desc          default    ext*/
    {"ilkn_id",      SAL_FIELD_TYPE_INT32, "ILKN ID",    "-1",       NULL},
    {NULL}
};

/* *INDENT-ON* */
/*
 * }
 */
#undef _ERR_MSG_MODULE_NAME
