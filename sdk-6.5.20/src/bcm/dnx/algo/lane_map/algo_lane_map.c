/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/** \file algo_lane_map.c
 *
 *  Handle the lane map algorithm functions.
 *  Include:
 *      Lane Map DB initialization and deinitialization.
 *      Lane Map DB set and get functions.
 *      Lane Map DB verify functions.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_PORT

/*
 * Include files.
 * {
 */

#include <shared/shrextend/shrextend_debug.h>

#include <soc/dnx/swstate/auto_generated/access/dnx_algo_lane_map_access.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/dnx_algo_port_access.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_fabric.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_nif.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_port.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_lane_map.h>
#include <bcm/types.h>
#include <bcm_int/dnx/algo/port/algo_port_utils.h>
#include <bcm_int/dnx/algo/lane_map/algo_lane_map.h>
#include <soc/dnxc/swstate/dnxc_sw_state_utils.h>

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
/*
 * Defines
 * {
 */
typedef enum
{
    DNX_ALGO_LANE_MAP_DIRECTION_RX = 0,
    DNX_ALGO_LANE_MAP_DIRECTION_TX,
    DNX_ALGO_LANE_MAP_NOF_DIRECTIONS
} dnx_algo_lane_map_direction_e;
/*
 * }
 */
/*
 * Module Init / Deinit  
 * { 
 */
/**
 * \brief - set default values NIF and Fabric lane to serdes map DB. 
 *          All the values for this DB should be initialized to BCM_PORT_LANE_TO_SERDES_NOT_MAPPED.
 *          Only lane2serdes intializaiton is needed. serdes2lane DB is initialized when setting this DB.
 */
static shr_error_e
dnx_algo_lane_map_lane_to_serdes_map_init(
    int unit,
    dnx_algo_lane_map_type_e type,
    int lane_map_handle)
{
    soc_dnxc_lane_map_db_map_t lane2serdes;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&lane2serdes, 0, sizeof(soc_dnxc_lane_map_db_map_t));

    lane2serdes.rx_id = BCM_PORT_LANE_TO_SERDES_NOT_MAPPED;
    lane2serdes.tx_id = BCM_PORT_LANE_TO_SERDES_NOT_MAPPED;

    SHR_IF_ERR_EXIT(dnx_algo_lane_map_db.lane_map.lane2serdes.set(unit, type, lane_map_handle, &lane2serdes));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_lane_map_init(
    int unit)
{
    int nof_fabric_links;
    int nof_nif_phys;
    int phy;
    int link;

    SHR_FUNC_INIT_VARS(unit);

    if (!sw_state_is_warm_boot(unit))
    {
        /*
         * Create SW State instance
         */
        SHR_IF_ERR_EXIT(dnx_algo_lane_map_db.init(unit));

        /*
         * Allocate SW STATE DBs  
         */
        /*
         * lane map db init
         * allocate lane map db for each NIF and Fabric phy
         */
        nof_nif_phys = dnx_data_nif.phys.nof_phys_get(unit);

        dnx_algo_lane_map_db.lane_map.lane2serdes.alloc(unit, DNX_ALGO_LANE_MAP_NIF_SIDE, nof_nif_phys);
        dnx_algo_lane_map_db.lane_map.serdes2lane.alloc(unit, DNX_ALGO_LANE_MAP_NIF_SIDE, nof_nif_phys);

        nof_fabric_links = dnx_data_fabric.links.nof_links_get(unit);

        dnx_algo_lane_map_db.lane_map.lane2serdes.alloc(unit, DNX_ALGO_LANE_MAP_FABRIC_SIDE, nof_fabric_links);
        dnx_algo_lane_map_db.lane_map.serdes2lane.alloc(unit, DNX_ALGO_LANE_MAP_FABRIC_SIDE, nof_fabric_links);

        /*
         * Set default values for DBs
         */
        /** Lane Map init - init the NIF and Fabric lane map DB */
        for (phy = 0; phy < nof_nif_phys; ++phy)
        {
            SHR_IF_ERR_EXIT(dnx_algo_lane_map_lane_to_serdes_map_init(unit, DNX_ALGO_LANE_MAP_NIF_SIDE, phy));
        }
        for (link = 0; link < nof_fabric_links; ++link)
        {
            SHR_IF_ERR_EXIT(dnx_algo_lane_map_lane_to_serdes_map_init(unit, DNX_ALGO_LANE_MAP_FABRIC_SIDE, link));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * }
 */

/*
 * Lane Map verify functions
 * {
 */

/**
 * \brief - 
 * Verify if the lane map Serdes ID is valid.
 *
 * \param [in] unit - Unit #.
 * \param [in] type - the lane map type.
 *        support types:
 *              DNX_ALGO_PORT_LANE_MAP_NIF_SIDE
 *              DNX_ALGO_PORT_LANE_MAP_FABRIC_SIDE
 *
 * \param [in] lane - logical lane ID.
 * \param [in] map_size - lane map size.
 * \param [in] serdes_rx - Serdes rx id for lane map.
 * \param [in] serdes_tx - Serdes tx id for lane map.
 * 
 * \return  
 *   see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_algo_lane_map_serdes_id_verify(
    int unit,
    dnx_algo_lane_map_type_e type,
    int lane,
    int map_size,
    int serdes_rx,
    int serdes_tx)
{

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Rx lane one to one mapping is not checked here.
     * This should be checked when attaching fabric ports,
     * as there is no such restriction for ILKN over fabric ports.
     */
    /*
     * Each lane must be both Rx and Tx mapped, or NOT_MAPPED 
     */
    if ((serdes_rx == BCM_PORT_LANE_TO_SERDES_NOT_MAPPED && serdes_tx != BCM_PORT_LANE_TO_SERDES_NOT_MAPPED) ||
        (serdes_rx != BCM_PORT_LANE_TO_SERDES_NOT_MAPPED && serdes_tx == BCM_PORT_LANE_TO_SERDES_NOT_MAPPED))
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG,
                     "lane %d: a lane must be mapped/umapped for BOTH rx and tx. Serdes rx id is %d, serdes tx id is %d\n",
                     lane, serdes_rx, serdes_tx);
    }

    /*
     * verify the serdes id is in the allowed range 
     */
    if ((serdes_rx >= map_size) || ((serdes_rx < 0) && (serdes_rx != BCM_PORT_LANE_TO_SERDES_NOT_MAPPED)))
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG, "lane %d: invalid serdes rx id %d\n", lane, serdes_rx);
    }
    if ((serdes_tx >= map_size) || ((serdes_tx < 0) && (serdes_tx != BCM_PORT_LANE_TO_SERDES_NOT_MAPPED)))
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG, "lane %d: invalid serdes tx id %d\n", lane, serdes_tx);
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */

shr_error_e
dnx_algo_lane_map_size_verify(
    int unit,
    dnx_algo_lane_map_type_e type,
    int map_size)
{
    int support_map_size;

    SHR_FUNC_INIT_VARS(unit);

    if (DNX_ALGO_LANE_MAP_IS_NIF_SIDE(unit, type))
    {
        support_map_size = dnx_data_nif.phys.nof_phys_get(unit);
    }
    else
    {
        support_map_size = dnx_data_fabric.links.nof_links_get(unit);
    }

    if (map_size != support_map_size)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid map_size parameter %d. map_size should be %d\n", map_size,
                     support_map_size);
    }
exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */

shr_error_e
dnx_algo_lane_map_type_verify(
    int unit,
    dnx_algo_lane_map_type_e type)
{
    SHR_FUNC_INIT_VARS(unit);

    if ((type < 0) || (type >= DNX_ALGO_NOF_LANE_MAP_TYPES))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Lane Map error: Lane Map type %d is not supported.\n", type);
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify the lane map parameters when adding ports.
 *
 * See .h file
 */
shr_error_e
dnx_algo_lane_map_port_add_verify(
    int unit,
    dnx_algo_lane_map_type_e lane_map_type,
    dnx_algo_port_info_s port_info,
    int is_over_fabric,
    int lane_id)
{
    int lane_core;
    int serdes_rx_core, serdes_tx_core;
    int resolution;
    int is_active = 0;
    int phy_offset;
    bcm_port_t logical_port;
    bcm_port_if_t exist_interface_type;
    dnx_algo_port_info_s tmp_port_info;
    soc_dnxc_lane_map_db_map_t lane2serdes, serdes2lane;
    dnx_algo_lane_map_direction_e serdes_direction, lane_direction;
    int serdes_id_to_check, lane_id_to_check;
    int curr_port_is_ilkn, exist_port_is_ilkn;
    int ethu_id = -1, idx;
    int imb_type, is_overlap = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_lane_map_db.lane_map.lane2serdes.get(unit, lane_map_type, lane_id, &lane2serdes));

    /*
     * 1. verify if the lanes are mapped.
     */
    if ((lane2serdes.tx_id == BCM_PORT_LANE_TO_SERDES_NOT_MAPPED) ||
        (lane2serdes.rx_id == BCM_PORT_LANE_TO_SERDES_NOT_MAPPED))
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG, "Lane Map error: lane %d is not mapped for lane map type %d.\n", lane_id,
                     lane_map_type);
    }
    /*
     * 2. verify if the rx lane map is one to one mapping for Fabric lanes
     *    For ILKN over fabric, there is no such limitation.
     */
    if (DNX_ALGO_LANE_MAP_IS_FABRIC_SIDE(unit, lane_map_type) && !is_over_fabric)
    {
        if (lane2serdes.rx_id != lane_id)
        {
            SHR_ERR_EXIT(_SHR_E_CONFIG,
                         "Lane Map Error: Rx lane map should be one to one mapping for one to one mapping for lane %d\n",
                         lane_id);
        }
    }
    /*
     * 3. verify if the lane and serdes id are in correct boundaries.
     */
    if (DNX_ALGO_PORT_TYPE_IS_NIF(unit, port_info, TRUE, TRUE, TRUE, TRUE))
    {
        SHR_IF_ERR_EXIT(dnx_algo_phy_to_ethu_id_get(unit, lane_id, &ethu_id));
    }

    if (DNX_ALGO_LANE_MAP_IS_NIF_SIDE(unit, lane_map_type))
    {
        /*
         * NIF Side
         */
        imb_type = dnx_data_nif.eth.ethu_properties_get(unit, ethu_id)->type;
        resolution = dnx_data_port.imb.imb_type_info_get(unit, imb_type)->nof_lanes_in_pm;
    }
    else if (is_over_fabric)
    {
        /*
         *  Fabric Side - ILKN port
         */
        resolution = dnx_data_fabric.blocks.nof_links_in_pm_get(unit);
    }
    else
    {
        /*
         * Fabric Side - Fabric port
         */
        resolution = dnx_data_lane_map.fabric.resolution_get(unit);
    }
    lane_core = lane_id / resolution;
    serdes_rx_core = lane2serdes.rx_id / resolution;
    serdes_tx_core = lane2serdes.tx_id / resolution;
    if ((serdes_rx_core != lane_core) || (serdes_tx_core != lane_core))
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG,
                     "Lane Map Error: lane %d, serdes rx id %d, serdes tx id %d. Swap can be only in %d lane boundary\n",
                     lane_id, lane2serdes.rx_id, lane2serdes.tx_id, resolution);
    }

    /*
     * 4. If serdes_rx[i] is assigned to ELK or ETH, serdes_tx[i] should
     *    be assigned to the same port type or NOT MAPPED, vice versa
     * For example:
     *    If lane_to_serdes_map_nif_lane0.BCM8869X=rx5:tx6 and lane0 is used for ETH port.
     *    we should ensure:
     *    a) serdes rx 5 and serdes tx 5 should not be assigned to ELK port.
     *    b) serdes rx 6 and serdes tx 6 should not be assigned to ELK port.
     * This rule is only relevant for ETH and ILKN_over_NIF ports when the lane map is not symmetric.
     */
    if (DNX_ALGO_PORT_TYPE_IS_NIF(unit, port_info, 1, 1, 1, 1) && !is_over_fabric
        && (lane2serdes.rx_id != lane2serdes.tx_id))
    {
        /*
         * Get the current port type.
         */
        curr_port_is_ilkn = DNX_ALGO_PORT_TYPE_IS_NIF_ILKN(unit, port_info, 1, 1);
        for (serdes_direction = DNX_ALGO_LANE_MAP_DIRECTION_RX; serdes_direction < DNX_ALGO_LANE_MAP_NOF_DIRECTIONS;
             serdes_direction++)
        {
            /*
             * Both serdes rx and tx should be checked.
             */
            serdes_id_to_check =
                (serdes_direction == DNX_ALGO_LANE_MAP_DIRECTION_RX) ? lane2serdes.rx_id : lane2serdes.tx_id;
            /*
             * Get the serdes to lane mapping
             */
            SHR_IF_ERR_EXIT(dnx_algo_lane_map_db.lane_map.
                            serdes2lane.get(unit, lane_map_type, serdes_id_to_check, &serdes2lane));
            /*
             * Get the Lane ID which needs to be checked
             */
            lane_id_to_check =
                (serdes_direction == DNX_ALGO_LANE_MAP_DIRECTION_RX) ? serdes2lane.tx_id : serdes2lane.rx_id;

            if (lane_id_to_check != BCM_PORT_LANE_TO_SERDES_NOT_MAPPED)
            {
                /*
                 * Check if there is active port using this lane.
                 */
                SHR_IF_ERR_EXIT(dnx_algo_port_db.nif.valid.get(unit, lane_id_to_check, &is_active));
                if (is_active)
                {
                    SHR_IF_ERR_EXIT(dnx_algo_port_db.nif.
                                    master_logical_port.get(unit, lane_id_to_check, &logical_port));
                    SHR_IF_ERR_EXIT(dnx_algo_port_db.logical.
                                    interface_type.get(unit, logical_port, &exist_interface_type));
                    exist_port_is_ilkn = (exist_interface_type == BCM_PORT_IF_ILKN);
                    /*
                     * Check if the port type is consistent.
                     */
                    if (curr_port_is_ilkn != exist_port_is_ilkn)
                    {
                        SHR_ERR_EXIT(_SHR_E_CONFIG,
                                     "Serde Rx ID %d and Serdes Tx ID %d should be assigned to same port type.\n",
                                     serdes_id_to_check, serdes_id_to_check);
                    }
                }
            }
        }
    }
    /*
     * 5. If nif_serdes[i] is assigned to ILKN, fabric_serdes[i] cannot be
     * assigned to ILKN, vice versa.
     */
    if ((DNX_ALGO_PORT_TYPE_IS_NIF_ILKN(unit, port_info, 1, 1))
        && (dnx_data_fabric.general.feature_get(unit, dnx_data_fabric_general_blocks_exist))
        && dnx_data_fabric.links.nof_links_get(unit))
    {
        /*
         * Check if there is PHY overlap
         */
        for (idx = 0; idx < dnx_data_nif.ilkn.supported_phys_info_get(unit)->key_size[0]; ++idx)
        {
            if (_SHR_PBMP_MEMBER(dnx_data_nif.ilkn.supported_phys_get(unit, idx)->nif_phys, lane_id) &&
                _SHR_PBMP_MEMBER(dnx_data_nif.ilkn.supported_phys_get(unit, idx)->fabric_phys, lane_id))
            {
                is_overlap = 1;
                break;
            }
        }
        if (is_overlap)
        {
            for (serdes_direction = DNX_ALGO_LANE_MAP_DIRECTION_RX; serdes_direction < DNX_ALGO_LANE_MAP_NOF_DIRECTIONS;
                 serdes_direction++)
            {
                /*
                 * Both serdes rx and tx should be checked.
                 */
                serdes_id_to_check =
                    (serdes_direction == DNX_ALGO_LANE_MAP_DIRECTION_RX) ? lane2serdes.rx_id : lane2serdes.tx_id;
                /*
                 * Get the serdes to lane mapping.
                 * If the ILKN is over fabric, we need to check NIF side;
                 * If the ILKN is over NIF, we need to check fabric side;
                 */
                SHR_IF_ERR_EXIT(dnx_algo_lane_map_db.lane_map.serdes2lane.get(unit,
                                                                              is_over_fabric ?
                                                                              DNX_ALGO_LANE_MAP_NIF_SIDE :
                                                                              DNX_ALGO_LANE_MAP_FABRIC_SIDE,
                                                                              serdes_id_to_check, &serdes2lane));
                /*
                 * Get the Lane ID which needs to be checked
                 */
                for (lane_direction = DNX_ALGO_LANE_MAP_DIRECTION_RX; lane_direction < DNX_ALGO_LANE_MAP_NOF_DIRECTIONS;
                     lane_direction++)
                {
                    lane_id_to_check =
                        (lane_direction == DNX_ALGO_LANE_MAP_DIRECTION_RX) ? serdes2lane.rx_id : serdes2lane.tx_id;
                    if (lane_id_to_check != BCM_PORT_LANE_TO_SERDES_NOT_MAPPED)
                    {
                        if (is_over_fabric)
                        {
                            /*
                             * Check if the serdes[i] is used for ILKN in NIF side.
                             */
                            SHR_IF_ERR_EXIT(dnx_algo_port_nif_phy_active_get(unit, lane_id_to_check, &is_active));
                            if (is_active)
                            {
                                SHR_IF_ERR_EXIT(dnx_algo_port_nif_phy_to_logical_get
                                                (unit, lane_id_to_check, 0, 0, &logical_port));
                                SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, logical_port, &tmp_port_info));
                                if (DNX_ALGO_PORT_TYPE_IS_NIF_ILKN(unit, tmp_port_info, 1, 1))
                                {
                                    SHR_ERR_EXIT(_SHR_E_CONFIG,
                                                 "%s Serdes %d in Fabric side cannot be assigned to ILKN port, because it has been used for ILKN port in NIF side.\n",
                                                 (serdes_direction == DNX_ALGO_LANE_MAP_DIRECTION_RX) ? "Rx" : "Tx",
                                                 serdes_id_to_check);
                                }
                            }
                        }
                        else
                        {
                            /*
                             * Check if the serdes[i] is used for ILKN in Fabric side.
                             */
                            phy_offset = dnx_data_port.general.fabric_phys_offset_get(unit);
                            SHR_IF_ERR_EXIT(dnx_algo_port_nif_phy_active_get
                                            (unit, lane_id_to_check + phy_offset, &is_active));
                            if (is_active)
                            {
                                SHR_ERR_EXIT(_SHR_E_CONFIG,
                                             "%s Serdes %d in NIF side cannot be assigned to ILKN port, because it has been used for ILKN port in Farbic side.\n",
                                             (serdes_direction == DNX_ALGO_LANE_MAP_DIRECTION_RX) ? "Rx" : "Tx",
                                             serdes_id_to_check);
                            }
                        }
                    }
                }
            }
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/*
 * }
 */

/*
 * Lane Map Set/Get functions
 * {
 */

/**
 * \brief
 * verify for dnx_algo_lane_map_set
 */
static shr_error_e
dnx_algo_lane_map_set_verify(
    int unit,
    dnx_algo_lane_map_type_e type,
    int map_size,
    soc_dnxc_lane_map_db_map_t * lane2serdes)
{
    int lane, i;
    soc_dnxc_lane_map_db_map_t *serdes2lane = NULL;
    soc_dnxc_lane_map_db_map_t pre_lane2serdes;
    int is_active;
    bcm_pbmp_t phys_to_check;
    int temp_lane;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_lane_map_type_verify(unit, type));
    SHR_IF_ERR_EXIT(dnx_algo_lane_map_size_verify(unit, type, map_size));

    SHR_ALLOC(serdes2lane, map_size * sizeof(soc_dnxc_lane_map_db_map_t), "Serdes to lane mapping struct",
              "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    _SHR_PBMP_CLEAR(phys_to_check);

    /** Initialized all the rx serdes and tx serdes to NOT_MAPPED */
    for (i = 0; i < map_size; ++i)
    {
        serdes2lane[i].rx_id = BCM_PORT_LANE_TO_SERDES_NOT_MAPPED;
        serdes2lane[i].tx_id = BCM_PORT_LANE_TO_SERDES_NOT_MAPPED;
    }

    for (lane = 0; lane < map_size; ++lane)
    {
        int serdes_rx = lane2serdes[lane].rx_id;
        int serdes_tx = lane2serdes[lane].tx_id;

        /** validate lane2serdes input array values are valid */
        SHR_IF_ERR_EXIT(dnx_algo_lane_map_serdes_id_verify(unit, type, lane, map_size, serdes_rx, serdes_tx));

        if (serdes_rx != BCM_PORT_LANE_TO_SERDES_NOT_MAPPED)
        {
            if (serdes2lane[serdes_rx].rx_id != BCM_PORT_LANE_TO_SERDES_NOT_MAPPED)
            {
                /** if the rx of this serdes was already mapped to another lane */
                SHR_ERR_EXIT(_SHR_E_CONFIG, "lane %d: double mapping: serdes rx id %d is already mapped to lane %d\n",
                             lane, serdes_rx, serdes2lane[serdes_rx].rx_id);
            }
            else if (serdes2lane[serdes_tx].tx_id != BCM_PORT_LANE_TO_SERDES_NOT_MAPPED)
            {
                /** if the tx of this serdes was already mapped to another lane */
                SHR_ERR_EXIT(_SHR_E_CONFIG, "lane %d: double mapping: serdes tx id %d is already mapped to lane %d\n",
                             lane, serdes_tx, serdes2lane[serdes_tx].tx_id);
            }
            serdes2lane[serdes_rx].rx_id = lane;
            serdes2lane[serdes_tx].tx_id = lane;
        }

        SHR_IF_ERR_EXIT(dnx_algo_lane_map_db.lane_map.lane2serdes.get(unit, type, lane, &pre_lane2serdes));

        /** if rx mapping or tx mapping of an active lane is changed */
        if ((serdes_rx != pre_lane2serdes.rx_id) || (serdes_tx != pre_lane2serdes.tx_id))
        {

            if (DNX_ALGO_LANE_MAP_IS_NIF_SIDE(unit, type))
            {
                dnx_algo_pm_info_t pm_info;

                SHR_IF_ERR_EXIT(dnx_algo_phy_pm_info_get(unit, lane, &pm_info));
                _SHR_PBMP_OR(phys_to_check, pm_info.phys);
            }
            else
            {
                /** fabric side */
                int nof_links_in_pm = dnx_data_fabric.blocks.nof_links_in_pm_get(unit);
                _SHR_PBMP_PORTS_RANGE_ADD(phys_to_check, lane / nof_links_in_pm * nof_links_in_pm, nof_links_in_pm);

            }

        }
    }

    BCM_PBMP_ITER(phys_to_check, temp_lane)
    {
        /** need to make sure that the mapping of PM with active ports remains unchanged. */
        if (DNX_ALGO_LANE_MAP_IS_NIF_SIDE(unit, type))
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_nif_phy_active_get(unit, temp_lane, &is_active));
        }
        else
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_fabric_phy_active_get(unit, temp_lane, &is_active));
        }
        if (is_active)
        {
            SHR_ERR_EXIT(_SHR_E_CONFIG, "Lane map for active PM cannot be changed. Lane %d is active\n", temp_lane);
        }
    }

exit:
    SHR_FREE(serdes2lane);
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_lane_map_set(
    int unit,
    dnx_algo_lane_map_type_e type,
    int map_size,
    soc_dnxc_lane_map_db_map_t * lane2serdes)
{
    int lane, i;
    soc_dnxc_lane_map_db_map_t *serdes2lane = NULL;

    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_algo_lane_map_set_verify(unit, type, map_size, lane2serdes));

    SHR_ALLOC(serdes2lane, map_size * sizeof(soc_dnxc_lane_map_db_map_t), "Serdes to lane mapping struct",
              "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    /** Initialized all the rx serdes and tx serdes to NOT_MAPPED */
    for (i = 0; i < map_size; ++i)
    {
        serdes2lane[i].rx_id = BCM_PORT_LANE_TO_SERDES_NOT_MAPPED;
        serdes2lane[i].tx_id = BCM_PORT_LANE_TO_SERDES_NOT_MAPPED;
    }

    for (lane = 0; lane < map_size; ++lane)
    {
        int serdes_rx = lane2serdes[lane].rx_id;
        int serdes_tx = lane2serdes[lane].tx_id;

        if (serdes_rx != BCM_PORT_LANE_TO_SERDES_NOT_MAPPED)
        {
            /*
             * if serdes_rx != BCM_PORT_LANE_TO_SERDES_NOT_MAPPED, 
             * then serdes_tx != BCM_PORT_LANE_TO_SERDES_NOT_MAPPED
             * verified in the verify function
             */
            serdes2lane[serdes_rx].rx_id = lane;
            serdes2lane[serdes_tx].tx_id = lane;
        }
    }

    /*
     * Configure SW state database
     */
    for (i = 0; i < map_size; ++i)
    {
        /** lane to serdes mapping - rx, tx */
        SHR_IF_ERR_EXIT(dnx_algo_lane_map_db.lane_map.lane2serdes.set(unit, type, i, &lane2serdes[i]));
        /** serdes to lane mapping - rx, tx */
        SHR_IF_ERR_EXIT(dnx_algo_lane_map_db.lane_map.serdes2lane.set(unit, type, i, &serdes2lane[i]));
    }

exit:
    SHR_FREE(serdes2lane);
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_lane_map_lane_to_serdes_map_get(
    int unit,
    dnx_algo_lane_map_type_e type,
    int map_size,
    soc_dnxc_lane_map_db_map_t * lane2serdes)
{
    int lane_id;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_lane_map_type_verify(unit, type));

    SHR_IF_ERR_EXIT(dnx_algo_lane_map_size_verify(unit, type, map_size));

    for (lane_id = 0; lane_id < map_size; lane_id++)
    {
        SHR_IF_ERR_EXIT(dnx_algo_lane_map_db.lane_map.lane2serdes.get(unit, type, lane_id, &lane2serdes[lane_id]));
    }
exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_lane_map_serdes_to_lane_map_get(
    int unit,
    dnx_algo_lane_map_type_e type,
    int map_size,
    soc_dnxc_lane_map_db_map_t * serdes2lane)
{
    int serdes_id;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_lane_map_type_verify(unit, type));

    SHR_IF_ERR_EXIT(dnx_algo_lane_map_size_verify(unit, type, map_size));

    for (serdes_id = 0; serdes_id < map_size; serdes_id++)
    {
        SHR_IF_ERR_EXIT(dnx_algo_lane_map_db.lane_map.serdes2lane.get(unit, type, serdes_id, &serdes2lane[serdes_id]));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Phymod expects lane and serdes to be with inner indexes,
 *   so this function set lane to serdes mapping to be mapped
 *   to the PM inner serdes index.
 */
static shr_error_e
dnx_algo_lane_map_pm_lane_to_serdes_assign(
    int unit,
    int nof_lanes,
    int base_lane,
    soc_dnxc_lane_map_db_map_t * lane2serdes)
{
    int lane_id;

    SHR_FUNC_INIT_VARS(unit);

    for (lane_id = 0; lane_id < nof_lanes; ++lane_id)
    {
        if (lane2serdes[lane_id].rx_id != BCM_PORT_LANE_TO_SERDES_NOT_MAPPED)
        {
            lane2serdes[lane_id].rx_id -= base_lane;
        }

        if (lane2serdes[lane_id].tx_id != BCM_PORT_LANE_TO_SERDES_NOT_MAPPED)
        {
            lane2serdes[lane_id].tx_id -= base_lane;
        }
    }

    SHR_FUNC_EXIT;
}

/**
* \brief - algorithm to get unmapped serdes in certain PortMacro.
*  
* \param [in] unit - chip unit id.
* \param [in] nof_lanes - the lane numbers in PM.
* \param [in] serdes2lane - serdes to lane mapping.
* \param [out] unmapped_serdes_rx_arr - store the unmapped serdes, zero-based.
* \param [out] unmapped_serdes_tx_arr - store the unmapped serdes, zero-based.
* \param [out] nof_unmapped_serdes - number of unmapped serdes, should be equal
*        to numbers of unmapped rx serdes or unmapped tx serdes.
*   
* \return
*   shr_error_e 
*   
* \remarks
*   * None
* \see
*   * None
*/

static shr_error_e
dnx_algo_lane_map_pm_unmapped_serdes_get(
    int unit,
    int nof_lanes,
    soc_dnxc_lane_map_db_map_t * serdes2lane,
    int *unmapped_serdes_rx_arr,
    int *unmapped_serdes_tx_arr,
    int *nof_unmapped_serdes)
{
    int serdes_id;
    int unmapped_rx_idx = 0, unmapped_tx_idx = 0;

    SHR_FUNC_INIT_VARS(unit);

    *nof_unmapped_serdes = 0;

    for (serdes_id = 0; serdes_id < nof_lanes; ++serdes_id)
    {
        /*
         * Get the NOT_MAPPED rx serdes id
         */
        if (serdes2lane[serdes_id].rx_id == BCM_PORT_LANE_TO_SERDES_NOT_MAPPED)
        {
            unmapped_serdes_rx_arr[unmapped_rx_idx] = serdes_id % nof_lanes;
            ++(unmapped_rx_idx);
        }
        /*
         * Get the NOT_MAPPED tx serdes id
         */
        if (serdes2lane[serdes_id].tx_id == BCM_PORT_LANE_TO_SERDES_NOT_MAPPED)
        {
            unmapped_serdes_tx_arr[unmapped_tx_idx] = serdes_id % nof_lanes;
            ++(unmapped_tx_idx);
        }
    }

    SHR_VAL_VERIFY(unmapped_rx_idx, unmapped_tx_idx, _SHR_E_PARAM,
                   "Lane Map Error: Number of NOT_MAPPED rx serdes should be equal to NOT_MAPPED tx serdes\n");
    *nof_unmapped_serdes = unmapped_rx_idx;

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief - algorithm to assign the unmapped serdes to each unmapped lanes.
*        When passing the lane to serdes mapping info to phymod,
*        there should be no unmapped lane.
*  
* \param [in] unit - chip unit id.
* \param [in] nof_lanes - the lane numbers in PM.
* \param [in] unmapped_serdes_rx_arr - store the unmapped rx serdes, zero-based.
* \param [in] unmapped_serdes_tx_arr - store the unmapped tx serdes, zero-based.
* \param [in] nof_unmapped_serdes - number of unmapped serdes.
* \param [out] lane2serdes - lane to serdes mapping.
*
* \return
*   shr_error_e 
*
* \remarks
*   * None
* \see
*   * None
*/

static shr_error_e
dnx_algo_lane_map_pm_unmapped_serdes_assign(
    int unit,
    int nof_lanes,
    int *unmapped_serdes_rx_arr,
    int *unmapped_serdes_tx_arr,
    int nof_unmapped_serdes,
    soc_dnxc_lane_map_db_map_t * lane2serdes)
{
    int lane_id;
    int unmapped_rx_idx = 0, unmapped_tx_idx = 0;

    SHR_FUNC_INIT_VARS(unit);

    for (lane_id = 0; lane_id < nof_lanes; ++lane_id)
    {
        /*
         * if lane is not mapped to serdes on rx side,
         * use the array of unmapped rx serdes of the pm to fill the hole.
         */
        if (lane2serdes[lane_id].rx_id == BCM_PORT_LANE_TO_SERDES_NOT_MAPPED)
        {
            lane2serdes[lane_id].rx_id = unmapped_serdes_rx_arr[unmapped_rx_idx];
            unmapped_rx_idx++;
        }

        /*
         * if lane is not mapped to serdes on tx side,
         * use the array of unmapped tx serdes of the pm to fill the hole.
         */
        if (lane2serdes[lane_id].tx_id == BCM_PORT_LANE_TO_SERDES_NOT_MAPPED)
        {
            lane2serdes[lane_id].tx_id = unmapped_serdes_tx_arr[unmapped_tx_idx];
            unmapped_tx_idx++;
        }
    }
    /*
     * unmapped lane pairs should be equal to unmapped serdes pairs.
     */
    if ((unmapped_tx_idx != nof_unmapped_serdes) || (unmapped_rx_idx != nof_unmapped_serdes))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Lane Map Error: Unmapped Rx & Tx lane numbers should be equal to Unmapped Rx & Tx serdes numbers.\n");
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief - Get the lane to serdes mapping for NIF or Fabric PM.
*  
* see .h file
*/

shr_error_e
dnx_algo_lane_map_pm_lane_to_serdes_map_get(
    int unit,
    dnx_algo_lane_map_type_e type,
    int pm_lower_bound,
    int pm_upper_bound,
    soc_dnxc_lane_map_db_map_t * lane2serdes)
{
    int index, inner_index;
    int *unmapped_serdes_rx_arr = NULL;
    int *unmapped_serdes_tx_arr = NULL;
    int nof_unmapped_serdes = 0;
    int nof_lanes;
    soc_dnxc_lane_map_db_map_t *serdes2lane = NULL;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_lane_map_type_verify(unit, type));

    nof_lanes = pm_upper_bound - pm_lower_bound + 1;

    SHR_ALLOC(unmapped_serdes_rx_arr, nof_lanes * sizeof(int), "Unmapped rx serdes", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    SHR_ALLOC(unmapped_serdes_tx_arr, nof_lanes * sizeof(int), "Unmapped tx serdes", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    SHR_ALLOC(serdes2lane, nof_lanes * sizeof(soc_dnxc_lane_map_db_map_t), "Serdes to Lane mapping", "%s%s%s\r\n",
              EMPTY, EMPTY, EMPTY);

    inner_index = 0;
    for (index = pm_lower_bound; index <= pm_upper_bound; ++index)
    {
        SHR_IF_ERR_EXIT(dnx_algo_lane_map_db.lane_map.lane2serdes.get(unit, type, index, &lane2serdes[inner_index]));
        SHR_IF_ERR_EXIT(dnx_algo_lane_map_db.lane_map.serdes2lane.get(unit, type, index, &serdes2lane[inner_index]));
        inner_index++;
    }

    /*
     * For all mapped lanes, set the serdes to be with inner PM index.
     */
    SHR_IF_ERR_EXIT(dnx_algo_lane_map_pm_lane_to_serdes_assign(unit, nof_lanes, pm_lower_bound, lane2serdes));

    /*
     * Get all the NOT_MAPPED rx and tx serdes in one PM,
     * which can be assigned to unmapped serdes later.
     */
    SHR_IF_ERR_EXIT(dnx_algo_lane_map_pm_unmapped_serdes_get
                    (unit, nof_lanes, serdes2lane, unmapped_serdes_rx_arr, unmapped_serdes_tx_arr,
                     &nof_unmapped_serdes));

    /*
     * If there is unmap serdes, should fill the unmapped lane
     * with unmapped serdes, otherwise invalid values will pass to phymod.
     */
    if (nof_unmapped_serdes)
    {
        SHR_IF_ERR_EXIT(dnx_algo_lane_map_pm_unmapped_serdes_assign
                        (unit, nof_lanes, unmapped_serdes_rx_arr, unmapped_serdes_tx_arr, nof_unmapped_serdes,
                         lane2serdes));
    }

exit:
    SHR_FREE(unmapped_serdes_rx_arr);
    SHR_FREE(unmapped_serdes_tx_arr);
    SHR_FREE(serdes2lane);
    SHR_FUNC_EXIT;

}

/**
 * \brief - Check if the lane map is symmetric, which means
 *    serdes_rx_id should be equal to serdes_tx_id for one lane.
 *    The restriction is relevant when setting one port to
 *    phy/remote loopback.
 *
 * See .h file
 */
shr_error_e
dnx_algo_lane_map_is_symmetric_get(
    int unit,
    bcm_port_t port,
    int *is_symmetric)
{
    int lane_id;
    int is_over_fabric = 0;
    dnx_algo_port_info_s port_info;
    bcm_pbmp_t phys;
    dnx_algo_lane_map_type_e lane_map_type;
    soc_dnxc_lane_map_db_map_t lane2serdes;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));

    BCM_PBMP_CLEAR(phys);
    if (DNX_ALGO_PORT_TYPE_IS_FABRIC(unit, port_info))
    {
        lane_map_type = DNX_ALGO_LANE_MAP_FABRIC_SIDE;
        SHR_IF_ERR_EXIT(dnx_algo_port_fabric_link_get(unit, port, &lane_id));
        BCM_PBMP_PORT_SET(phys, lane_id);
    }
    else
    {
        if (DNX_ALGO_PORT_TYPE_IS_NIF_ILKN(unit, port_info, 1 /* elk */ , 1 /* L1 */ ))
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_nif_ilkn_is_over_fabric_get(unit, port, &is_over_fabric));
        }
        /*
         * Check if the port is ilkn over fabric.
         * For ILKN over fabric ports, the lane map DB is stored on Fabric side.
         */
        lane_map_type = is_over_fabric ? DNX_ALGO_LANE_MAP_FABRIC_SIDE : DNX_ALGO_LANE_MAP_NIF_SIDE;
        SHR_IF_ERR_EXIT(dnx_algo_port_nif_phys_get(unit, port, 0, &phys));
    }
    /*
     * Verify if the lane map is symmetric
     */
    *is_symmetric = 1;
    BCM_PBMP_ITER(phys, lane_id)
    {
        SHR_IF_ERR_EXIT(dnx_algo_lane_map_db.lane_map.lane2serdes.get(unit, lane_map_type, lane_id, &lane2serdes));
        if (lane2serdes.rx_id != lane2serdes.tx_id)
        {
            *is_symmetric = 0;
            break;
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_lane_map_ilkn_lane_info_get(
    int unit,
    bcm_port_t port,
    dnx_algo_lane_map_type_e type,
    int lane,
    dnx_algo_port_ilkn_lane_info_t * ilkn_lane_info)
{
    int ilkn_id, ilkn_core, table_size;
    int first_phy, ilkn_offset, nof_lanes, range_iter;
    uint8 range_found = FALSE;
    const dnx_data_nif_ilkn_nif_lanes_map_t *nif_map_info;
    const dnx_data_nif_ilkn_fabric_lanes_map_t *fabric_map_info;
    soc_dnxc_lane_map_db_map_t lane2serdes;
    int support_map_size;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_interface_offset_get(unit, port, &ilkn_id));
    if (DNX_ALGO_LANE_MAP_IS_NIF_SIDE(unit, type))
    {
        support_map_size = dnx_data_nif.phys.nof_phys_get(unit);
    }
    else
    {
        support_map_size = dnx_data_fabric.links.nof_links_get(unit);
    }
    /** Verify lane id is not out of range */
    SHR_RANGE_VERIFY(lane, 0, support_map_size - 1, _SHR_E_PARAM, "lane id %d is out of range.\n", lane);

    SHR_IF_ERR_EXIT(dnx_algo_lane_map_db.lane_map.lane2serdes.get(unit, type, lane, &lane2serdes));

    if ((lane2serdes.rx_id == BCM_PORT_LANE_TO_SERDES_NOT_MAPPED)
        || (lane2serdes.tx_id == BCM_PORT_LANE_TO_SERDES_NOT_MAPPED))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "ERROR: Unmapped Rx, Tx (Lane %d)\n", lane);
    }

    ilkn_core = ilkn_id / dnx_data_nif.ilkn.ilkn_unit_if_nof_get(unit);

    /*
     * Get lane map table size 
     */
    if (type == DNX_ALGO_LANE_MAP_NIF_SIDE)
    {
        table_size = dnx_data_nif.ilkn.nif_lanes_map_info_get(unit)->key_size[1];
    }
    else
    {
        table_size = dnx_data_nif.ilkn.fabric_lanes_map_info_get(unit)->key_size[1];
    }

    /*
     * Search in lane map table 
     */
    for (range_iter = 0; range_iter < table_size; range_iter++)
    {
        if (type == DNX_ALGO_LANE_MAP_NIF_SIDE)
        {
            /*
             * NIF Side
             */
            nif_map_info = dnx_data_nif.ilkn.nif_lanes_map_get(unit, ilkn_core, range_iter);
            first_phy = nif_map_info->first_phy;
            ilkn_offset = nif_map_info->ilkn_lane_offset;
            nof_lanes = nif_map_info->nof_lanes;
        }
        else
        {
            /*
             * Fabric Side
             */
            fabric_map_info = dnx_data_nif.ilkn.fabric_lanes_map_get(unit, ilkn_core, range_iter);
            first_phy = fabric_map_info->first_phy;
            ilkn_offset = fabric_map_info->ilkn_lane_offset;
            nof_lanes = fabric_map_info->nof_lanes;
        }

        /*
         * If device lane is in range, take range info
         */
        if ((lane >= first_phy) && (lane < first_phy + nof_lanes))
        {
            range_found = TRUE;
            break;
        }
        else
        {
            continue;
        }
    }

    if (!range_found)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Ilkn lane id can't be found.\n");
    }

    ilkn_lane_info->ilkn_id = lane - first_phy + ilkn_offset;
    ilkn_lane_info->tx_id = lane2serdes.tx_id - first_phy + ilkn_offset;
    ilkn_lane_info->rx_id = lane2serdes.rx_id - first_phy + ilkn_offset;

exit:
    SHR_FUNC_EXIT;
}

/*
 * }
 */

#undef _ERR_MSG_MODULE_NAME
