/** \file diag_dnxc_lane_map.c
 * 
 * diagnostics for Lane Map
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
#include <shared/bsl.h>

#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_prt.h>
#include <appl/diag/dnxc/diag_dnxc_fabric.h>

#include <bcm/fabric.h>

#include <soc/drv.h>
#include <soc/portmod/portmod.h>
#ifdef BCM_DNX_SUPPORT
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_nif.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_fabric.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_port.h>
#include <bcm_int/dnx/port/imb/imb.h>
#include <bcm_int/dnx/port/imb/imb_common.h>
#endif

#ifdef BCM_DNXF_SUPPORT
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_port.h>
#endif

#include <soc/dnxc/dnxc_defs.h>

/*
 * }
 */

/*
 * Macros
 * {
 */
/*
 * }
 */

/**
* Functions
* {
*/

/**
 * \brief - Common function to dump lane map info
 *    for NIF and Fabric
 *
 * \param [in] unit - chip unit id
 * \param [in] flags - indicate NIF side or Farbic side
 * \param [in] print_lane - the print lane info
 * \param [in] sand_control - pointer to shell frw control structure
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
cmd_dnxc_lane_map_dump(
    int unit,
    int flags,
    int print_lane,
    sh_sand_control_t * sand_control)
{
    int start_lane, end_lane;
    int lane_id, pm_id;
    int internal_lane = -1;
    int nof_lanes = 0;
    int is_active = 0;
    bcm_port_t logical_port = -1;
    bcm_port_lane_to_serdes_map_t *serdes_map = NULL;
#ifdef BCM_DNX_SUPPORT
    bcm_pbmp_t phys;
    int phy;
    int ilkn_lane_id = -1;
    int nof_ilkn_lanes;
    int ilkn_lane_order[DNX_DATA_MAX_NIF_ILKN_LANES_MAX_NOF];
    int check_ilkn_over_fabric = 0;
    int actual_size;
    dnx_algo_port_info_s port_info;
#endif
#ifdef BCM_DNXF_SUPPORT
    bcm_port_config_t port_config;
#endif

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Lane ID verify
     */
#ifdef BCM_DNXF_SUPPORT
    if (SOC_IS_DNXF(unit))
    {
        nof_lanes = dnxf_data_port.general.nof_links_get(unit);
    }
    else
#endif
    {
#ifdef BCM_DNX_SUPPORT
        if (flags & BCM_PORT_LANE_TO_SERDES_FABRIC_SIDE)
        {
            nof_lanes = dnx_data_fabric.links.nof_links_get(unit);
        }
        else
        {
            nof_lanes = dnx_data_nif.phys.nof_phys_get(unit);
        }
#endif
    }

    if (print_lane != DIAG_DNXC_OPTION_ALL)
    {
        /*
         * When compiling as DNXF, the flags is always SOC_DNXC_MAX_NOF_FABRIC_LINKS, so nof_lanes is always
         * initialized 
         */
        /** coverity[uninit_use] */
        SHR_RANGE_VERIFY(print_lane, 0, nof_lanes - 1, _SHR_E_PARAM, "lane id out of bound.\n");
    }
    /*
     * Print table header
     */
    if (flags & BCM_PORT_LANE_TO_SERDES_FABRIC_SIDE)
    {
        PRT_TITLE_SET("Fabric Lane Map");
    }
    else
    {
        PRT_TITLE_SET("NIF Lane Map");
    }

    PRT_COLUMN_ADD("Lane#");
    PRT_COLUMN_ADD("SRD_RX#");
    PRT_COLUMN_ADD("SRD_TX#");
    PRT_COLUMN_ADD("PM ID");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Logical Port");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Port Internal Lane");
#ifdef BCM_DNX_SUPPORT
    /*
     * Only relevant for DNX devices
     */
    if (SOC_IS_DNX(unit))
    {
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "ILKN Lane ID");
    }
#endif
    /*
     * Get the lane map info
     */
    SHR_ALLOC(serdes_map, nof_lanes * sizeof(bcm_port_lane_to_serdes_map_t), "Lane to serdes mapping struct",
              "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    SHR_IF_ERR_EXIT(bcm_port_lane_to_serdes_map_get(unit, flags, nof_lanes, serdes_map));

    if (print_lane != DIAG_DNXC_OPTION_ALL)
    {
        start_lane = print_lane;
        end_lane = print_lane;
    }
    else
    {
        start_lane = 0;
        end_lane = nof_lanes - 1;
    }
    /*
     * Dump table
     */
    for (lane_id = start_lane; lane_id <= end_lane; ++lane_id)
    {

#ifdef BCM_DNXF_SUPPORT
        if (SOC_IS_DNXF(unit))
        {
            /** SKU support
             *  Skipping not supported for the device links */
            if (!BCM_PBMP_MEMBER(dnxf_data_port.general.supported_phys_get(unit)->pbmp, lane_id))
            {
                continue;
            }
        }
        else
#endif
        {
#ifdef BCM_DNX_SUPPORT
            /** SKU support
             *  Skipping not supported for the device fabric links */
            if ((flags & BCM_PORT_LANE_TO_SERDES_FABRIC_SIDE)
                && !BCM_PBMP_MEMBER(dnx_data_fabric.links.general_get(unit)->supported_links, lane_id))
            {
                continue;
            }
#endif
        }
        /*
         * Since there is no info stored for NOT_MAPPED lane, skip the NOT_MAPPED lane.
         */
        if (serdes_map[lane_id].serdes_rx_id == BCM_PORT_LANE_TO_SERDES_NOT_MAPPED)
        {
            if (print_lane != DIAG_DNXC_OPTION_ALL)
            {
                /*
                 * For per lane dump scenario, return log info. 
                 */
                LOG_CLI((BSL_META("lane %d is NOT_MAPPED. No info can be dumped! \n"), lane_id));
                SHR_EXIT();
            }
            else
            {
                /*
                 * For All lane dump scenario, skip the NOT_MAPPED lane 
                 */
                continue;
            }
        }
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);

        /*
         * Check if the phy lane is active and get the logical port number
         */
#ifdef BCM_DNXF_SUPPORT
        if (SOC_IS_DNXF(unit))
        {
            SHR_IF_ERR_EXIT(bcm_port_config_get(unit, &port_config));
            /*
             * Check if lane is active
             */
            if (BCM_PBMP_MEMBER(port_config.sfi, lane_id))
            {
                logical_port = lane_id;
                internal_lane = 0;
                is_active = 1;
            }
            else
            {
                is_active = 0;
            }
        }
        else
#endif
        {
#ifdef BCM_DNX_SUPPORT
            /*
             * Clear Flags
             */
            check_ilkn_over_fabric = 0;
            is_active = 0;

            if (flags & BCM_PORT_LANE_TO_SERDES_FABRIC_SIDE)
            {
                SHR_IF_ERR_EXIT(dnx_algo_port_fabric_phy_active_get(unit, lane_id, &is_active));
                if (is_active)
                {
                    SHR_IF_ERR_EXIT(dnx_algo_port_fabric_link_to_logical_get(unit, lane_id, &logical_port));
                    /*
                     * Fabric port is single lane port
                     */
                    internal_lane = 0;
                }
                else
                {
                    /*
                     * Need to check if there is ILKN over fabric port
                     */
                    check_ilkn_over_fabric = 1;
                }
            }
            /*
             * The code will go into the following branch under one of the follwing scenarios:
             * 1. lane map is Fabric side and the fabric lane is inactive, need to check if there is ilkn over fabric port;
             * 2. lane map is NIF side.
             */
            if (!is_active)
            {
                SHR_IF_ERR_EXIT(dnx_algo_port_nif_phy_active_get
                                (unit,
                                 check_ilkn_over_fabric ? (lane_id +
                                                           dnx_data_port.
                                                           general.fabric_phys_offset_get(unit)) : lane_id,
                                 &is_active));
                if (is_active)
                {
                    SHR_IF_ERR_EXIT(dnx_algo_port_nif_phy_to_logical_get
                                    (unit, lane_id, check_ilkn_over_fabric, 0, &logical_port));
                    /*
                     * Get the internal lane number and ilkn lane order for active lane.
                     */
                    SHR_IF_ERR_EXIT(dnx_algo_port_nif_phys_get(unit, logical_port, 0, &phys));
                    /*
                     * Get the port internal lane info
                     */
                    internal_lane = 0;
                    BCM_PBMP_ITER(phys, phy)
                    {
                        if (phy == lane_id)
                        {
                            break;
                        }
                        else
                        {
                            internal_lane++;
                        }
                    }
                    /*
                     * For ILKN ports, dump the lane order, only relevant for DNX devices
                     */
                    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, logical_port, &port_info));

                    if (DNX_ALGO_PORT_TYPE_IS_NIF_ILKN(unit, port_info, 1 /* elk */ , 1 /* L1 */ ))
                    {
                        BCM_PBMP_COUNT(phys, nof_ilkn_lanes);

                        SHR_IF_ERR_EXIT(imb_port_logical_lane_order_get
                                        (unit, logical_port, nof_ilkn_lanes, ilkn_lane_order, &actual_size));
                        ilkn_lane_id = ilkn_lane_order[internal_lane];
                    }
                    else
                    {
                        ilkn_lane_id = -1;
                    }
                }
            }
#endif
        }

        PRT_CELL_SET("%d", lane_id);
        PRT_CELL_SET("%d", serdes_map[lane_id].serdes_rx_id);
        PRT_CELL_SET("%d", serdes_map[lane_id].serdes_tx_id);
        if (is_active)
        {
            SHR_IF_ERR_EXIT(portmod_port_pm_id_get(unit, logical_port, &pm_id));
            PRT_CELL_SET("%02d", pm_id);
            PRT_CELL_SET("%d", logical_port);
            PRT_CELL_SET("%d", internal_lane);
        }
        else
        {
            PRT_CELL_SET("%s", "-");
            PRT_CELL_SET("%s", "-");
            PRT_CELL_SET("%s", "-");
        }
#ifdef BCM_DNX_SUPPORT
        if (SOC_IS_DNX(unit))
        {
            if (is_active && (ilkn_lane_id != -1))
            {
                PRT_CELL_SET("%d", ilkn_lane_id);
            }
            else
            {
                PRT_CELL_SET("%s", "N/A");
            }
        }
#endif
    }

    PRT_COMMITX;
exit:
    SHR_FREE(serdes_map);
    PRT_FREE;
    SHR_FUNC_EXIT;

}

/*
 * }
 */

#undef _ERR_MSG_MODULE_NAME
