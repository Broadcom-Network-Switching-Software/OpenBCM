/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/** \file algo_lane_map.h
 *
 *  Handle the lane map algorithm functions.
 *  Include:
 *      Lane Map DB initialization and deinitialization.
 *      Lane Map DB set and get functions.
 *      Lane Map DB verify functions.
 */

#ifndef ALGO_LANE_MAP_H_INCLUDED
/*
 * {
 */
#define ALGO_LANE_MAP_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

/*
 * Includes.
 * {
 */
#include <shared/shrextend/shrextend_debug.h>
#include <bcm/port.h>
#include <soc/dnxc/swstate/auto_generated/types/dnxc_types.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>

/*
 * }
 */
/*
 * Macros.
 * {
 */
/**
 * \brief Convert the Lane Map flag to Lane Map type
 */
#define DNX_ALGO_LANE_MAP_TYPE_GET(unit, flags)    \
                ((flags == BCM_PORT_LANE_TO_SERDES_FABRIC_SIDE) ? DNX_ALGO_LANE_MAP_FABRIC_SIDE : DNX_ALGO_LANE_MAP_NIF_SIDE)

/**
 * \brief return ture if lane map is fabric side
 */
#define DNX_ALGO_LANE_MAP_IS_FABRIC_SIDE(unit, type)    (type == DNX_ALGO_LANE_MAP_FABRIC_SIDE)

/**
 * \brief return ture if lane map is nif side
 */
#define DNX_ALGO_LANE_MAP_IS_NIF_SIDE(unit, type)    (type == DNX_ALGO_LANE_MAP_NIF_SIDE)

/*
 * }
 */
/*
 * TypeDefs.
 * {
 */

/**
 * \brief - Lane Map types
 */
typedef enum
{
    /** Lane map for NIF ports */
    DNX_ALGO_LANE_MAP_NIF_SIDE = 0,
    /** Lane map for fabric ports */
    DNX_ALGO_LANE_MAP_FABRIC_SIDE = 1,
    /** Nof lane map types.  Must be last!*/
    DNX_ALGO_NOF_LANE_MAP_TYPES = 2,
} dnx_algo_lane_map_type_e;

/**
 * \brief -  ILKN lane info
 */
typedef struct
{

    /**
     * Ilkn Lane Index (0-23)
     */
    int ilkn_id;
    /**
     * Ilkn RX Serdes Index (0-23)
     */
    int rx_id;
    /**
      * Ilkn RX Serdes Index (0-23)
      */
    int tx_id;

} dnx_algo_port_ilkn_lane_info_t;

/*
 * }
 */

/*
 * Module Init / Deinit 
 * { 
 */
/**
 * \brief - 
 * Init algo lane map module for cool boot mode.
 *
 * \param [in] unit - Unit #.
 * 
 * \return  
 *   see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_lane_map_init(
    int unit);

/*
 * }
 */

/*
 * Lane Map verify functions
 * {
 */

/**
 * \brief - 
 * Verify if the lane map size is correct.
 *
 * \param [in] unit - Unit #.
 * \param [in] type - the lane map type.
 *        support types:
 *              DNX_ALGO_PORT_LANE_MAP_NIF_SIDE
 *              DNX_ALGO_PORT_LANE_MAP_FABRIC_SIDE
 *
 * \param [in] map_size - lane map size.
 * 
 * \return  
 *   see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_lane_map_size_verify(
    int unit,
    dnx_algo_lane_map_type_e type,
    int map_size);

/**
 * \brief - 
 * Verify the phy lane map type is valid.
 *
 * \param [in] unit - Unit #.
 * \param [in] type - the lane map type.
 *        support types:
 *              DNX_ALGO_PORT_LANE_MAP_NIF_SIDE
 *              DNX_ALGO_PORT_LANE_MAP_FABRIC_SIDE
 *
 * \return  
 *   see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */

shr_error_e dnx_algo_lane_map_type_verify(
    int unit,
    dnx_algo_lane_map_type_e type);

/**
 * \brief - 
 * Verify the lane map parameters when adding ports.
 * There are some special limitations among NIF ports, Fabric ports
 * and ILKN over fabric ports, so some verify work should be done
 * when adding ports.
 *
 * \param [in] unit - Unit #.
 * \param [in] lane_map_type - the lane map type.
 *        support types:
 *              DNX_ALGO_PORT_LANE_MAP_NIF_SIDE
 *              DNX_ALGO_PORT_LANE_MAP_FABRIC_SIDE
 * \param [in] port_info - info about port.
 * \param [in] is_over_fabric - if the port is ILKN over fabric.
 * \param [in] lane_id - Logical lane id.
 *
 * \return  
 *   see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */

shr_error_e dnx_algo_lane_map_port_add_verify(
    int unit,
    dnx_algo_lane_map_type_e lane_map_type,
    dnx_algo_port_info_s port_info,
    int is_over_fabric,
    int lane_id);

/*
 * }
 */

/*
 * Lane Map Set/Get functions
 * {
 */

/**
 * \brief - 
 *  Set the nif or fabric lane map to lane map database,
 *  including lane to serdes mapping and serdes to lane mapping.
 * 
 * \param [in] unit - unit #.
 * \param [in] type - the lane map type.
 *        support types:
 *              DNX_ALGO_PORT_LANE_MAP_NIF_SIDE
 *              DNX_ALGO_PORT_LANE_MAP_FABRIC_SIDE
 *
 * \param [in] map_size - the lane map database size.
 * \param [in] lane2serdes - lane to serdes mapping info.
 * 
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_lane_map_set(
    int unit,
    dnx_algo_lane_map_type_e type,
    int map_size,
    soc_dnxc_lane_map_db_map_t * lane2serdes);

/**
 * \brief - 
 *  Get the nif or fabric lane to serdes mapping info from lane map database.
 * 
 * \param [in] unit - unit #.
 * \param [in] type - the lane map type.
 *        support types:
 *              DNX_ALGO_PORT_LANE_MAP_NIF_SIDE
 *              DNX_ALGO_PORT_LANE_MAP_FABRIC_SIDE
 *
 * \param [in] map_size - the lane map database size.
 * \param [out] lane2serdes - lane to serdes mapping info.
 * 
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_lane_map_lane_to_serdes_map_get(
    int unit,
    dnx_algo_lane_map_type_e type,
    int map_size,
    soc_dnxc_lane_map_db_map_t * lane2serdes);

/**
 * \brief - 
 *  Get the nif or fabric serdes to lane mapping info from lane map database.
 * 
 * \param [in] unit - unit #.
 * \param [in] type - the lane map type.
 *        support types:
 *              DNX_ALGO_PORT_LANE_MAP_NIF_SIDE
 *              DNX_ALGO_PORT_LANE_MAP_FABRIC_SIDE
 *
 * \param [in] map_size - the lane map database size.
 * \param [out] serdes2lane - serdes to lane mapping info.
 * 
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_lane_map_serdes_to_lane_map_get(
    int unit,
    dnx_algo_lane_map_type_e type,
    int map_size,
    soc_dnxc_lane_map_db_map_t * serdes2lane);

/**
* \brief - Get the lane to serdes mapping for NIF or Fabric PM.
*
* \param [in] unit - chip unit id.
* \param [in] type - the lane map type.
*        support types:
*              DNX_ALGO_PORT_LANE_MAP_NIF_SIDE
*              DNX_ALGO_PORT_LANE_MAP_FABRIC_SIDE
* \param [in] pm_lower_bound - PM lower bound.
* \param [in] pm_upper_bound - PM upper bound.
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
shr_error_e dnx_algo_lane_map_pm_lane_to_serdes_map_get(
    int unit,
    dnx_algo_lane_map_type_e type,
    int pm_lower_bound,
    int pm_upper_bound,
    soc_dnxc_lane_map_db_map_t * lane2serdes);

/**
 * \brief - Check the lane map is symmetric, which means
 *    serdes_rx_id should be equal to serdes_tx_id for one lane.
 *    The restriction is relevant when setting one port to
 *    phy/remote loopback.
 *
 * \param [in] unit - Unit #.
 * \param [in] port - Logical port #.
 * \param [out] is_symmetric - If the lane map is symmetric.
 * \return
 *   see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */

shr_error_e dnx_algo_lane_map_is_symmetric_get(
    int unit,
    bcm_port_t port,
    int *is_symmetric);

/**
 * \brief - Get ILKN lane information
 *
 * \param [in] unit - Unit ID
 * \param [in] port - logical port
 * \param [in] type - the lane map type.
 *       support types:
 *             DNX_ALGO_PORT_LANE_MAP_NIF_SIDE
 *             DNX_ALGO_PORT_LANE_MAP_FABRIC_SIDE
 * \param [in] lane - device lane Id
 * \param [out] ilkn_lane_info - ilkn lane info
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_lane_map_ilkn_lane_info_get(
    int unit,
    bcm_port_t port,
    dnx_algo_lane_map_type_e type,
    int lane,
    dnx_algo_port_ilkn_lane_info_t * ilkn_lane_info);

/*
 * }
 */

/*
 * }
 */
#endif /*_ALGO_LANE_MAP_H_INCLUDED_*/
