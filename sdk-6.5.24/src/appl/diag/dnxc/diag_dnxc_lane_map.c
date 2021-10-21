/** \file diag_dnxc_lane_map.c
 * 
 * diagnostics for Lane Map
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
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
#include <soc/dnxc/fabric.h>
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
#include <soc/dnxf/swstate/auto_generated/access/dnxf_access.h>
#include <soc/dnxf/cmn/dnxf_lane_map_db.h>
#include <soc/dnxf/cmn/dnxf_drv.h>
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
 * \brief - convert the serdes_map to hold serdex map by fmac_lane
 *
 * \param [in] unit - chip unit id
 * \param [in] nof_lanes - indicates the number of entries in serdes_map
 * \param [in/out] serdes_map - can take in a mapping of serdeses by logical links or by fmac_lane.
 *                              outputs a  a mapping of serdeses by fmac_lane.
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
#ifdef BCM_DNXF_SUPPORT
static shr_error_e
cmd_dnxf_serdes_map_db_align(
    int unit,
    int nof_lanes,
    bcm_port_lane_to_serdes_map_t * serdes_map)
{
    int link_iter, fmac_lane;
    bcm_port_lane_to_serdes_map_t serdes_map_lane[DNXF_DATA_MAX_PORT_GENERAL_NOF_LINKS];
    bcm_port_lane_to_serdes_map_t unmapped = { BCM_PORT_LANE_TO_SERDES_NOT_MAPPED, BCM_PORT_LANE_TO_SERDES_NOT_MAPPED };

    SHR_FUNC_INIT_VARS(unit);

    if (dnxf_data_port.lane_map.feature_get(unit, dnxf_data_port_lane_map_is_flexible_link_mapping_supported))
    {
        for (link_iter = 0; link_iter < nof_lanes; ++link_iter)
        {
            serdes_map_lane[link_iter] = serdes_map[link_iter];
            serdes_map[link_iter] = unmapped;
        }

        for (link_iter = 0; link_iter < nof_lanes; ++link_iter)
        {
            SHR_IF_ERR_EXIT(soc_dnxf_lane_map_db_link_to_fmac_lane_get(unit, link_iter, &fmac_lane));

            /** if not unmapped */
            if (fmac_lane == -1)
            {
                /** skip access to invalid indice */
                continue;
            }

            serdes_map[fmac_lane] = serdes_map_lane[link_iter];
        }
    }

exit:
    SHR_FUNC_EXIT;
}
#endif

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
    int nof_lanes = 0;
    int is_active = 0;
    bcm_port_t logical_port = -1;
    bcm_port_lane_to_serdes_map_t *serdes_map = NULL;
    int is_cable_swap_supported = FALSE;
    int cable_swap_master, cable_swap_en, srd_tx_real = 0;
#ifdef BCM_DNX_SUPPORT
    bcm_pbmp_t phys;
    int phy;
    int ilkn_lane_id = -1;
    int internal_lane = -1;
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
     * Check if cable swap is supported
     */
#ifdef BCM_DNXF_SUPPORT
    if (SOC_IS_DNXF(unit))
    {
        if ((dnxf_data_port.general.feature_get(unit, dnxf_data_port_general_is_cable_swap_supported))
            && (flags & BCM_PORT_LANE_TO_SERDES_FABRIC_SIDE))
        {
            is_cable_swap_supported = TRUE;
        }
    }
    else
#endif
    {
#ifdef BCM_DNX_SUPPORT
        if ((dnx_data_port.general.feature_get(unit, dnx_data_port_general_is_cable_swap_supported))
            && (flags & BCM_PORT_LANE_TO_SERDES_FABRIC_SIDE))
        {
            is_cable_swap_supported = TRUE;
        }
#endif
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
    if (is_cable_swap_supported)
    {
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Real SRD_TX# (CS)");
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "En/Dis (CS)");
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Master/Slave (CS)");
    }
    PRT_COLUMN_ADD("PM ID");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Logical Port");
#ifdef BCM_DNX_SUPPORT
    /*
     * Only relevant for DNX devices
     */
    if (SOC_IS_DNX(unit))
    {
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Port Internal Lane");
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

#ifdef BCM_DNXF_SUPPORT
    if (SOC_IS_DNXF(unit))
    {
        /*
         * serdes_map might hold an array with logical links as index.
         * Need to modify the array to hold serdes based on fmac lanes as keys.
         */
        SHR_IF_ERR_EXIT(cmd_dnxf_serdes_map_db_align(unit, nof_lanes, serdes_map));
    }
#endif

    /*
     * Dump table
     */
    for (lane_id = start_lane; lane_id <= end_lane; ++lane_id)
    {

#ifdef BCM_DNXF_SUPPORT
        if (SOC_IS_DNXF(unit))
        {
            bcm_pbmp_t supported_lanes;

            /*
             * SKU support
             *  Skipping not supported for the device links
             */
            SHR_IF_ERR_EXIT(soc_dnxf_drv_supported_lanes_get(unit, &supported_lanes));
            if (!BCM_PBMP_MEMBER(supported_lanes, lane_id))
            {
                continue;
            }
            /*
             * FIXME SDK-240276:
             * First column title should be called FMAC Lane#
             * For Ramon2 - get the active VD0 from device OTP (SW state) and offset lanes if flavor is 1
 */
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
            int link;
            SHR_IF_ERR_EXIT(bcm_port_config_get(unit, &port_config));
            /*
             * Check if lane is active
             */
            SHR_IF_ERR_EXIT(soc_dnxf_lane_map_db_fmac_lane_to_link_get(unit, lane_id, &link));
            if (link != -1 && BCM_PBMP_MEMBER(port_config.sfi, link))
            {
                logical_port = link;
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

        if (is_cable_swap_supported)
        {
            SHR_IF_ERR_EXIT(bcm_fabric_link_control_get
                            (unit, logical_port, bcmFabricLinkCableSwapEnable, &cable_swap_en));
            SHR_IF_ERR_EXIT(bcm_fabric_link_control_get
                            (unit, logical_port, bcmFabricLinkCableSwapMasterMode, &cable_swap_master));
            
            if (cable_swap_en)
            {
                PRT_CELL_SET("%d", srd_tx_real);
                PRT_CELL_SET("en");
                PRT_CELL_SET(cable_swap_master ? "Master" : "Slave");
            }
            else
            {
                PRT_CELL_SET("N/A");
                PRT_CELL_SET("dis");
                PRT_CELL_SET("N/A");
            }
        }
        if (is_active)
        {
            SHR_IF_ERR_EXIT(portmod_port_pm_id_get(unit, logical_port, &pm_id));
            PRT_CELL_SET("%02d", pm_id);
            PRT_CELL_SET("%d", logical_port);
        }
        else
        {
            PRT_CELL_SET("%s", "-");
            PRT_CELL_SET("%s", "-");
        }
#ifdef BCM_DNX_SUPPORT
        if (SOC_IS_DNX(unit))
        {
            if (is_active)
            {
                PRT_CELL_SET("%d", internal_lane);
            }
            else
            {
                PRT_CELL_SET("%s", "-");
            }
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
