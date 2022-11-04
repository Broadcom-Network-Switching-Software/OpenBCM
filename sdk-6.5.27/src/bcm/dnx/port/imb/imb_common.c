/** \file imb_common.c
 *
 *  Common functions for IMB layer
 *
 */
/*
 *
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 */

/*
 * Include files.
 * {
 */

#include <soc/sand/shrextend/shrextend_debug.h>
#include <bcm_int/dnx/port/imb/imb_common.h>
#include <bcm_int/dnx/port/imb/imb_internal.h>
#include "imb_utils.h"

#include "../nif/dnx_port_nif_arb_internal.h"
#include <soc/portmod/portmod.h>
#include <soc/portmod/portmod_common.h>
#include <soc/phy/phymod_port_control.h>
#include <soc/phyreg.h>
#include <bcm_int/dnx/port/port.h>
#include <bcm_int/dnx/port/imb/imb.h>
#include <bcm_int/dnx/port/port_flexe.h>
#include <bcm_int/dnx/synce/synce.h>
#include <bcm_int/dnx/port/nif/dnx_port_nif_arb.h>
#include <bcm_int/dnx/port/nif/dnx_port_nif_ofr.h>
#include <bcm_int/dnx/port/nif/dnx_port_nif_oft.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/algo/flexe_general/algo_flexe_general.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_nif.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_fabric.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_tdm.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_port.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_port.h>
#include <bcm_int/dnx/cosq/egress/egr_queuing.h>
#include <soc/dnxc/dnxc_port.h>
#include <bcm_int/dnx/auto_generated/dnx_port_dispatch.h>

#ifdef INCLUDE_XFLOW_MACSEC
#include <bcm/xflow_macsec.h>
#include <dnx/dnx_sec.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_macsec.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/sec_access.h>
#endif

/* CMICx includes */
#include <soc/cmicx_miim.h>

#include <phymod/phymod_acc.h>
#include <phymod/phymod.h>

/*
 * }
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_PORT

/*
 * Functions
 * {
 */

/**
 * \brief - convert from bcm_port_resource_t to portmod_speed_config_t
 *
 *  see .h file
 */

int
imb_common_pm_resource_to_speed_config_convert(
    int unit,
    const bcm_port_resource_t * resource,
    portmod_speed_config_t * port_speed_config)
{
    bcm_pbmp_t phys;
    dnx_algo_port_info_s port_info;
    int num_lane;
    int lane_speed = 0;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(portmod_speed_config_t_init(unit, port_speed_config));
    port_speed_config->speed = resource->speed;
    port_speed_config->fec = (portmod_fec_t) resource->fec_type;
    port_speed_config->link_training = resource->link_training;
    port_speed_config->lane_config = resource->phy_lane_config;

    /*
     * get number of lanes
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, resource->port, &port_info));
    if (DNX_ALGO_PORT_TYPE_IS_NIF
        (unit, port_info, DNX_ALGO_PORT_TYPE_INCLUDE(ELK, STIF, L1, FLEXE, MGMT, FRAMER_MGMT, FEEDBACK)))
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_nif_phys_get(unit, resource->port, 0, &phys));
        BCM_PBMP_COUNT(phys, num_lane);
        port_speed_config->num_lane = num_lane;
        lane_speed = DNX_ALGO_PORT_TYPE_IS_NIF_ILKN(unit, port_info, DNX_ALGO_PORT_TYPE_INCLUDE(ELK, L1))
            ? port_speed_config->speed : (port_speed_config->speed / num_lane);
        if ((resource->flags & BCM_PORT_RESOURCE_AM_TRANSPARENT) && (resource->speed != 10000))
        {
            /*
             * For 10G port, there is no AM transparent configuration in PM
             */
            PORTMOD_SPEED_CONFIG_F_TRANSPARENT_AM_SET(port_speed_config);
        }
    }
    else if (DNX_ALGO_PORT_TYPE_IS_FABRIC(unit, port_info))
    {
        port_speed_config->num_lane = 1;
        lane_speed = port_speed_config->speed;
    }

    if (DNXC_PORT_PHY_SPEED_IS_PAM4(lane_speed))
    {
        PORTMOD_PORT_PHY_LANE_CONFIG_FORCE_PAM4_SET(port_speed_config->lane_config);
        PORTMOD_PORT_PHY_LANE_CONFIG_FORCE_NRZ_CLEAR(port_speed_config->lane_config);
    }
    else
    {
        PORTMOD_PORT_PHY_LANE_CONFIG_FORCE_PAM4_CLEAR(port_speed_config->lane_config);
        PORTMOD_PORT_PHY_LANE_CONFIG_FORCE_NRZ_SET(port_speed_config->lane_config);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Check if phy control is supported for current PM.
 *
 * \param [in] unit - chip unit id.
 * \param [in] port - logical port
 * \param [in] type - Phy Control type
 * \param [in] is_set - if this fuction is called by set function
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
imb_common_port_phy_control_validate(
    int unit,
    bcm_port_t port,
    bcm_port_phy_control_t type,
    int is_set)
{
    dnx_algo_port_info_s port_info;
    int is_flr_fw_support;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Sepcial handle for PRBs function, as port control PRBs function needs to
     * call the soc layer PRBs function to deal with Per Lane Gport.
     * Therefore we must ensure the PRBs is supported by soc layer common API.
     */
    switch (type)
    {
        case BCM_PORT_PHY_CONTROL_PRBS_POLYNOMIAL:
        case BCM_PORT_PHY_CONTROL_PRBS_TX_INVERT_DATA:
        case BCM_PORT_PHY_CONTROL_PRBS_RX_INVERT_DATA:
        case BCM_PORT_PHY_CONTROL_PRBS_TX_ENABLE:
        case BCM_PORT_PHY_CONTROL_PRBS_RX_ENABLE:
        case BCM_PORT_PHY_CONTROL_PRBS_RX_STATUS:
        {
            SHR_ERR_EXIT(_SHR_E_UNAVAIL,
                         "PHY control type %d is not supported, please use bcm_port_control_set/get instead.\r\n",
                         type);
            break;
        }
        case BCM_PORT_PHY_CONTROL_LINK_TRAINING_INIT_TX_FIR_POST:
        {
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "PHY control type %d is not supported on Blackhawk PHY.\r\n", type);
            break;
        }
        case BCM_PORT_PHY_CONTROL_FLEXE_50G_NOFEC_20K_AM_SPACING_ENABLE:
        {
            bcm_port_resource_t resource;
            bcm_pbmp_t phys;
            int nof_lanes;

            SHR_IF_ERR_EXIT(imb_common_port_resource_get(unit, port, &resource));
            SHR_IF_ERR_EXIT(dnx_algo_port_nif_phys_get(unit, port, 0, &phys));
            _SHR_PBMP_COUNT(phys, nof_lanes);
            if (resource.speed != 50000 || nof_lanes != 2 || resource.fec_type != bcmPortPhyFecNone)
            {
                SHR_ERR_EXIT(_SHR_E_UNAVAIL,
                             "PHY control type %d set is supported only on BH port with 50G, 2 lanes and no FEC.\r\n",
                             type);
            }
            break;
        }
        case BCM_PORT_PHY_CONTROL_FIRMWARE_RX_FORCE_EXTENDED_REACH_ENABLE:
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));
            if (is_set && (DNX_ALGO_PORT_TYPE_IS_NIF_ETH(unit, port_info, DNX_ALGO_PORT_TYPE_INCLUDE(L1, MGMT))
                           || DNX_ALGO_PORT_TYPE_IS_FLEXE_PHY(unit, port_info, TRUE)
                           || DNX_ALGO_PORT_TYPE_IS_OTN_PHY(unit, port_info)))
            {
                /*
                 * Check if FLR firmware is supported
                 */
                SHR_IF_ERR_EXIT(dnx_algo_port_nif_flr_fw_support_get(unit, port, &is_flr_fw_support));
                if (is_flr_fw_support)
                {
                    SHR_ERR_EXIT(_SHR_E_CONFIG, "This control is not configurable if FLR firmware is used.\r\n");
                }
            }
            break;
        }
        /** The soc layer will verify the rest types */
        default:
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Perform the full ber projection test.
 *   Configures, collectes are returns the values.
 * \param [in] unit - The unit number.
 * \param [in] port - The port.
 * \param [in] lane - The lane.
 * \param [in] ber_proj - BER test parametes.
 * \param [in] max_errcnt - Maximumm number of lanes to test.
 * \param [out] errcnt_array - The result array.
 * \param [out] actual_errcnt - Number of valid elements in errcnt_array

 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
imb_common_port_post_ber_proj_get(
    int unit,
    bcm_port_t port,
    int lane,
    const bcm_port_ber_proj_params_t * ber_proj,
    int max_errcnt,
    bcm_port_ber_proj_analyzer_errcnt_t * errcnt_array,
    int *actual_errcnt)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(soc_dnxc_port_post_ber_proj_get
                    (unit, port, lane, ber_proj, max_errcnt, errcnt_array, actual_errcnt));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - convert from portmod_speed_config_t to bcm_port_resource_t
 *
 *  see .h file
 */

void
imb_common_speed_config_to_pm_resource_convert(
    portmod_speed_config_t * port_speed_config,
    bcm_port_resource_t * resource)
{

    resource->speed = port_speed_config->speed;
    resource->fec_type = (bcm_port_phy_fec_t) port_speed_config->fec;
    resource->link_training = port_speed_config->link_training;
    resource->phy_lane_config = port_speed_config->lane_config;
    if (PORTMOD_SPEED_CONFIG_F_TRANSPARENT_AM_GET(port_speed_config))
    {
        resource->flags |= BCM_PORT_RESOURCE_AM_TRANSPARENT;
    }
}

/**
 * \brief - Set prbs polynomial
 *
 *  see .h file
 */

int
imb_common_port_prbs_polynomial_set(
    int unit,
    bcm_port_t port,
    portmod_prbs_mode_t prbs_mode,
    bcm_port_prbs_t value)
{
    int flags = 0;
    phymod_prbs_poly_t phymod_poly;
    phymod_prbs_t prbs_config;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Get the original configuration
     */
    SHR_IF_ERR_EXIT(portmod_port_prbs_config_get(unit, port, prbs_mode, flags, &prbs_config));
    /*
     * Covert to phymod prbs polynomial
     */
    SHR_IF_ERR_EXIT(soc_prbs_poly_to_phymod(value, &phymod_poly));
    prbs_config.poly = phymod_poly;
    SHR_IF_ERR_EXIT(portmod_port_prbs_config_set(unit, port, prbs_mode, flags, &prbs_config));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get prbs polynomial
 *
 *  see .h file
 */

int
imb_common_port_prbs_polynomial_get(
    int unit,
    bcm_port_t port,
    portmod_prbs_mode_t prbs_mode,
    bcm_port_prbs_t * value)
{
    int flags = 0;
    phymod_prbs_t prbs_config;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(portmod_port_prbs_config_get(unit, port, prbs_mode, flags, &prbs_config));
    /*
     * Covert to SOC prbs polynomial value
     */
    SHR_IF_ERR_EXIT(phymod_prbs_poly_to_soc(prbs_config.poly, (uint32 *) value));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Whether enable prbs data inversion
 *
 *  see .h file
 */

int
imb_common_port_prbs_invert_data_set(
    int unit,
    bcm_port_t port,
    int type,
    portmod_prbs_mode_t prbs_mode,
    int invert)
{
    int flags = 0;
    phymod_prbs_t prbs_config;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Config direction
     */
    if (type == bcmPortControlPrbsRxInvertData)
    {
        PHYMOD_PRBS_DIRECTION_RX_SET(flags);
    }
    else
    {
        PHYMOD_PRBS_DIRECTION_TX_SET(flags);
    }

    SHR_IF_ERR_EXIT(portmod_port_prbs_config_get(unit, port, prbs_mode, flags, &prbs_config));
    prbs_config.invert = invert;
    SHR_IF_ERR_EXIT(portmod_port_prbs_config_set(unit, port, prbs_mode, flags, &prbs_config));

exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief - Get the prbs data inversion status
 *
 *  see .h file
 */

int
imb_common_port_prbs_invert_data_get(
    int unit,
    bcm_port_t port,
    int type,
    portmod_prbs_mode_t prbs_mode,
    int *invert)
{
    int flags = 0;
    phymod_prbs_t prbs_config;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Config direction
     */
    if (type == bcmPortControlPrbsRxInvertData)
    {
        PHYMOD_PRBS_DIRECTION_RX_SET(flags);
    }
    else
    {
        PHYMOD_PRBS_DIRECTION_TX_SET(flags);
    }

    SHR_IF_ERR_EXIT(portmod_port_prbs_config_get(unit, port, prbs_mode, flags, &prbs_config));
    *invert = prbs_config.invert;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Enable or disable Prbs
 *
 *  see .h file
 */

int
imb_common_port_prbs_enable_set(
    int unit,
    bcm_port_t port,
    int type,
    portmod_prbs_mode_t prbs_mode,
    int enable)
{
    int flags = 0;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Config direction
     */
    if (type == bcmPortControlPrbsRxEnable)
    {
        PHYMOD_PRBS_DIRECTION_RX_SET(flags);
    }
    else
    {
        PHYMOD_PRBS_DIRECTION_TX_SET(flags);
    }

    SHR_IF_ERR_EXIT(portmod_port_prbs_enable_set(unit, port, prbs_mode, flags, enable));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get prbs enable status
 *
 *  see .h file
 */

int
imb_common_port_prbs_enable_get(
    int unit,
    bcm_port_t port,
    int type,
    portmod_prbs_mode_t prbs_mode,
    int *enable)
{
    int flags = 0;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Config direction
     */
    if (type == bcmPortControlPrbsRxEnable)
    {
        PHYMOD_PRBS_DIRECTION_RX_SET(flags);
    }
    else
    {
        PHYMOD_PRBS_DIRECTION_TX_SET(flags);
    }

    SHR_IF_ERR_EXIT(portmod_port_prbs_enable_get(unit, port, prbs_mode, flags, enable));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get prbs status
 *
 *  see .h file
 */

int
imb_common_port_prbs_rx_status_get(
    int unit,
    bcm_port_t port,
    portmod_prbs_mode_t prbs_mode,
    int *status)
{
    int flags;
    phymod_prbs_status_t prbs_status;

    SHR_FUNC_INIT_VARS(unit);

    flags = PHYMOD_PRBS_STATUS_F_CLEAR_ON_READ;

    SHR_IF_ERR_EXIT(phymod_prbs_status_t_init(&prbs_status));
    SHR_IF_ERR_EXIT(portmod_port_prbs_status_get(unit, port, (portmod_prbs_mode_t) prbs_mode, flags, &prbs_status));

    if (prbs_status.prbs_lock_loss)
    {
        *status = IMB_COMMON_PRBS_PREV_LOCK_LOST;
    }
    else if (!prbs_status.prbs_lock)
    {
        *status = IMB_COMMON_PRBS_CURR_LOCK_LOST;
    }
    else
    {
        *status = prbs_status.error_count;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - get the pm ids that ilkn port lanes is part of.
 */

static int
imb_common_port_ilkn_lanes_pm_ids_get(
    int unit,
    bcm_gport_t port,
    int *nof_pms_found,
    int *pm_ids)
{
    bcm_pbmp_t phys;
    int phy, nof_pms;
    int pm_index, pm_sub_index, last_pm_added = -1;
    portmod_dispatch_type_t pm_types[DNX_DATA_MAX_NIF_ILKN_ILKN_OVER_ETH_PMS_MAX];

    int pms_info_ids[DNX_DATA_MAX_NIF_ILKN_ILKN_OVER_ETH_PMS_MAX] = { 0 };
    int portmod_phy_offset = 0;
    int is_over_fabric;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_nif_ilkn_is_over_fabric_get(unit, port, &is_over_fabric));
    if (is_over_fabric)
    {
        portmod_phy_offset = dnx_data_port.general.fabric_phys_offset_get(unit);
    }

    *nof_pms_found = 0;
    SHR_IF_ERR_EXIT(dnx_algo_port_nif_phys_get(unit, port, 0, &phys));
    BCM_PBMP_ITER(phys, phy)
    {
        SHR_IF_ERR_EXIT(portmod_phy_pms_ids_get
                        (unit, phy + portmod_phy_offset, DNX_DATA_MAX_NIF_ILKN_ILKN_OVER_ETH_PMS_MAX, &nof_pms,
                         pm_types, pms_info_ids));
        for (pm_index = 0; pm_index < nof_pms; ++pm_index)
        {   /** we are looking for the phy pms that are not ilkn */
            /** in jr2 it could be whether Pm8x50 or fabric */
            if (pm_types[pm_index] != portmodDispatchTypePmOsILKN_50G
                && pm_types[pm_index] != portmodDispatchTypePmOsILKN)
            {
                if (last_pm_added == -1)
                {
                    last_pm_added = 0;
                    /** add first pm id to the pm ids set*/
                    pm_ids[last_pm_added] = pms_info_ids[pm_index];
                    *nof_pms_found = *nof_pms_found + 1;
                }
                else
                {/** check if we added this id before */
                    for (pm_sub_index = 0; pm_sub_index < (last_pm_added + 1); ++pm_sub_index)
                    {
                        if (pm_ids[pm_sub_index] == pms_info_ids[pm_index])
                        {
                            break;
                        }
                        if (pm_sub_index == last_pm_added)
                        {
                            /** we got to the last add without finding the id, add the id to the set*/
                            pm_ids[last_pm_added + 1] = pms_info_ids[pm_index];
                            last_pm_added++;
                            *nof_pms_found = *nof_pms_found + 1;
                        }
                    }
                }
            }
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - part of imb_port_resource_multi_set,
 *          For each PM get changed ports and update db
 *
 * \param [in] unit - chip unit id
 * \param [in] nof_ports - number of changed ports
 * \param [in] resource_arr - changed ports info look at bcm_port_resource_t
 * \param [in] nof_pms_max - max number on PMS in the device.
 * \param [out] port_enable_state - ports enable state
 * \param [out] pm_resource_config - the pm information needed for speed change
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * imb_port_resource_multi_set
 *   * bcm_port_resource_t
 *   * imb_pm_resource_config_t
 */
int
imb_common_speed_config_db_build(
    int unit,
    int nof_ports,
    const bcm_port_resource_t * resource_arr,
    int nof_pms_max,
    int *port_enable_state,
    imb_pm_resource_config_t * pm_resource_config)
{

    int pm_ids[DNX_DATA_MAX_NIF_ILKN_ILKN_OVER_ETH_PMS_MAX];
    int pm_index;
    int config_info_index;
    int nof_shared_pms = 0;
    int lane_index, resource_index, master_port, first_phy = 0;
    uint32 nof_lanes_per_pm = 0;
    dnx_algo_port_info_s port_info;
    portmod_dispatch_type_t pm_type;
    dnx_algo_port_ethu_access_info_t ethu_access_info;
    SHR_FUNC_INIT_VARS(unit);

    /** portmod_speed_config_t array initialization */
    for (pm_index = 0; pm_index < nof_pms_max; ++pm_index)
    {
        for (lane_index = 0; lane_index < DNX_DATA_MAX_PORT_GENERAL_MAX_PHYS_PER_PM; ++lane_index)
        {
            SHR_IF_ERR_EXIT(portmod_speed_config_t_init
                            (unit, &pm_resource_config[pm_index].port_speed_config[lane_index]));
            pm_resource_config[pm_index].speed_for_pcs_bypass_port[lane_index] = 0;
            pm_resource_config[pm_index].port_starting_lane[lane_index] = -1;
        }
        pm_resource_config[pm_index].vco_setting.tvco = portmodVCOInvalid;
        pm_resource_config[pm_index].vco_setting.ovco = portmodVCOInvalid;
        pm_resource_config[pm_index].nof_ports = 0;
        pm_resource_config[pm_index].enable_state = port_enable_state;
        pm_resource_config[pm_index].flags = 0;

        PORTMOD_PBMP_CLEAR(pm_resource_config[pm_index].master_ports_pbmp);
    }

    /** divide the ports to PMs,fill pm_resource_config with info needed for speed validate*/
    for (resource_index = 0; resource_index < nof_ports; ++resource_index)
    {
        /** get port type */
        SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, resource_arr[resource_index].port, &port_info));

        /** skip ports that are not IMB */
        if (!DNX_ALGO_PORT_TYPE_IS_IMB(unit, port_info, FALSE))
        {
            continue;
        }

        /** get portmod master port in case interface is channalized (portmod layer handles its own master) */
        SHR_IF_ERR_EXIT(portmod_port_pm_type_get(unit, resource_arr[resource_index].port, &master_port, &pm_type));

        /** if ilkn, get all pms the ilkn lanes is part of */
        if (DNX_ALGO_PORT_TYPE_IS_NIF_ILKN(unit, port_info, DNX_ALGO_PORT_TYPE_INCLUDE(ELK, L1)))
        {
            SHR_IF_ERR_EXIT(imb_common_port_ilkn_lanes_pm_ids_get
                            (unit, resource_arr[resource_index].port, &nof_shared_pms, pm_ids));

        }
        else
        {
            /** get pm id of the port */
            SHR_IF_ERR_EXIT(portmod_port_pm_id_get(unit, resource_arr[resource_index].port, &pm_ids[0]));
            if (pm_ids[0] < 0 || pm_ids[0] >= nof_pms_max)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "port %d: pm_id %d is not between 0 and %d",
                             resource_arr[resource_index].port, pm_ids[0], nof_pms_max - 1);
            }
            nof_shared_pms = 1;
        }

        for (pm_index = 0; pm_index < nof_shared_pms; ++pm_index)
        {
            /** skip channelized ports that share the same master port */
            if (PORTMOD_PBMP_MEMBER(pm_resource_config[pm_ids[pm_index]].master_ports_pbmp, master_port))
            {
                break;
            }

            config_info_index = pm_resource_config[pm_ids[pm_index]].nof_ports;

                        /** save the changed port on each PM*/
            PORTMOD_PBMP_PORT_ADD(pm_resource_config[pm_ids[pm_index]].master_ports_pbmp, master_port);

                        /** update relevant fields of bcm_port_resource_t struct in portmod_speed_config_t struct */
            SHR_IF_ERR_EXIT(imb_common_pm_resource_to_speed_config_convert(unit, &resource_arr[resource_index],
                                                                           &pm_resource_config[pm_ids
                                                                                               [pm_index]].port_speed_config
                                                                           [config_info_index]));
            /** if ILKN, set the speed_for_pcs_bypass_port flag */
            if (DNX_ALGO_PORT_TYPE_IS_NIF_ILKN(unit, port_info, DNX_ALGO_PORT_TYPE_INCLUDE(ELK, L1)))
            {
                pm_resource_config[pm_ids[pm_index]].speed_for_pcs_bypass_port[config_info_index] = 1;
            }
            else if (DNX_ALGO_PORT_TYPE_IS_OTN_PHY(unit, port_info))
            {
                pm_resource_config[pm_ids[pm_index]].speed_for_pcs_bypass_port[config_info_index] = 1;
            }
            else
            {
                if (DNX_ALGO_PORT_TYPE_IS_NIF
                    (unit, port_info, DNX_ALGO_PORT_TYPE_INCLUDE(STIF, L1, FLEXE, MGMT, FRAMER_MGMT, FEEDBACK)))
                {
                    SHR_IF_ERR_EXIT(dnx_algo_port_nif_first_phy_get
                                    (unit, resource_arr[resource_index].port, 0, &first_phy));
                    SHR_IF_ERR_EXIT(dnx_algo_port_ethu_access_info_get
                                    (unit, resource_arr[resource_index].port, &ethu_access_info));
                    nof_lanes_per_pm =
                        dnx_data_port.imb.imb_type_info_get(unit, ethu_access_info.imb_type)->nof_lanes_in_pm;

                    /** set TVCO 26G flag only for NIF ETH L2 ports */
                    if (DNX_ALGO_PORT_TYPE_IS_NIF_ETH(unit, port_info, DNX_ALGO_PORT_TYPE_INCLUDE(MGMT)) &&
                        (resource_arr[resource_index].flags & BCM_PORT_RESOURCE_TVCO_PRIORITIZE_26P562G))
                    {
                        pm_resource_config[pm_ids[pm_index]].flags |= BCM_PORT_RESOURCE_TVCO_PRIORITIZE_26P562G;
                    }
                }
                else if (DNX_ALGO_PORT_TYPE_IS_FABRIC(unit, port_info))
                {
                    nof_lanes_per_pm = dnx_data_fabric.links.nof_links_per_core_get(unit);
                    SHR_IF_ERR_EXIT(dnx_algo_port_fabric_link_get(unit, resource_arr[resource_index].port, &first_phy));
                }
                else
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL, "port %d: invalid port type\n", resource_arr[resource_index].port);
                }

                pm_resource_config[pm_ids[pm_index]].port_starting_lane[config_info_index] =
                    first_phy % nof_lanes_per_pm;
            }

            /** increase nof ports in current PM */
            ++(pm_resource_config[pm_ids[pm_index]].nof_ports);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - part of imb_port_resource_multi_set,
 *          For each PM run pm_speed_config_validate
 *
 * \param [in] unit - chip unit id
 * \param [in] nof_pms_max - max number on PMS in the device.
 * \param [out] pm_resource_config - the pm information needed for speed change
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * imb_port_resource_multi_set
 *   * bcm_port_resource_t
 *   * imb_pm_resource_config_t
 */
int
imb_common_pm_speed_config_validate(
    int unit,
    int nof_pms_max,
    imb_pm_resource_config_t * pm_resource_config)
{

    int pm_id;
    int output_nof_pms;
    int flags = 0;
    SHR_FUNC_INIT_VARS(unit);

    output_nof_pms = 0;
    /** for each pm, decide if TVCO and/or OVCO need to be changed */
    for (pm_id = 0; pm_id < nof_pms_max; ++pm_id)
    {

        /** no ports to configure in current PM */
        if (pm_resource_config[pm_id].nof_ports == 0)
        {
            continue;
        }

        /** init vco_setting*/
        SHR_IF_ERR_EXIT(portmod_pm_vco_setting_t_init(unit, &pm_resource_config[pm_id].vco_setting));
        pm_resource_config[pm_id].vco_setting.num_speeds = pm_resource_config[pm_id].nof_ports;
        pm_resource_config[pm_id].vco_setting.speed_config_list = pm_resource_config[pm_id].port_speed_config;
        pm_resource_config[pm_id].vco_setting.port_starting_lane_list = pm_resource_config[pm_id].port_starting_lane;
        pm_resource_config[pm_id].vco_setting.speed_for_pcs_bypass_port =
            pm_resource_config[pm_id].speed_for_pcs_bypass_port;

        PORTMOD_PM_SPEED_VALIDATE_F_PLL_SWITCH_DEFAULT_SET(flags);
        if (pm_resource_config[pm_id].flags & BCM_PORT_RESOURCE_TVCO_PRIORITIZE_26P562G)
        {
            PORTMOD_PM_SPEED_VALIDATE_F_TVCO_PRIORITIZE_26P562G_SET(flags);
        }
        /** now call pm_speed_config_vaidate for the PM*/
        SHR_IF_ERR_EXIT(portmod_pm_speed_config_validate
                        (unit, pm_id, &pm_resource_config[pm_id].master_ports_pbmp,
                         flags, &pm_resource_config[pm_id].vco_setting));
        ++output_nof_pms;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - FlexE wrapper post init after the TSC
 *    clock is ready.
 *
 * \param [in] unit - chip unit id
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
static shr_error_e
imb_common_flexe_post_init(
    int unit)
{
    int imb_id;
    uint32 nof_imbs;
    imb_dispatch_type_t type;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imb_nof_get(unit, &nof_imbs));
    for (imb_id = 0; imb_id < nof_imbs; imb_id++)
    {
        SHR_IF_ERR_EXIT(imb_id_type_get(unit, imb_id, &type));
        if (type != imbDispatchTypeImb_feu_phy)
        {
            continue;
        }
        SHR_IF_ERR_EXIT(imb_post_init(unit, imb_id));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - part of imb_port_resource_multi_set,
 *          reconfigure PM VCO for each changed PM.
 *
 * \param [in] unit - chip unit id
 * \param [in] nof_pms_max - max number on PMS in the device.
 * \param [in] pm_resource_config - the pm information needed for speed change
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * imb_port_resource_multi_set
 *   * bcm_port_resource_t
 *   * imb_pm_resource_config_t
 */

int
imb_common_speed_pm_vco_config_set(
    int unit,
    int nof_pms_max,
    imb_pm_resource_config_t * pm_resource_config)
{

    portmod_vco_type_t vco[2] = { 0 };
    int pm_id;
    int is_flexe_pm = 0;
    bcm_pbmp_t all_flexe_phy_ports;
    SHR_FUNC_INIT_VARS(unit);

    for (pm_id = 0; pm_id < nof_pms_max; ++pm_id)
    {

        /** no ports to configure in current PM */
        /** ILKN PM must be with nof_ports = 0, since we added port info to other pms it is part of */
        if (pm_resource_config[pm_id].nof_ports == 0)
        {
            continue;
        }

        if (pm_resource_config[pm_id].vco_setting.is_tvco_new || pm_resource_config[pm_id].vco_setting.is_ovco_new)
        {
            vco[0] = pm_resource_config[pm_id].vco_setting.tvco;
            vco[1] = pm_resource_config[pm_id].vco_setting.ovco;
            SHR_IF_ERR_EXIT(portmod_pm_vco_reconfig(unit, pm_id, vco));
        }
        /*
         * If FlexE feature is enable and the TVCO for the dedictaed PM is 26.562G,
         * need to initialize the FlexE module
         */
        if (dnx_data_nif.framer.feature_get(unit, dnx_data_nif_framer_is_supported) &&
            !dnx_data_nif.framer.feature_get(unit, dnx_data_nif_framer_is_clock_independent))
        {
            if (dnx_data_nif.flexe.flexe_mode_get(unit) != 0)
            {
                SHR_IF_ERR_EXIT(dnx_algo_flexe_is_valid_pm_get(unit, pm_id, &is_flexe_pm));
                if (is_flexe_pm)
                {
                    BCM_PBMP_CLEAR(all_flexe_phy_ports);
                    SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get
                                    (unit, BCM_CORE_ALL, DNX_ALGO_PORT_LOGICALS_TYPE_FLEXE_PHY, 0,
                                     &all_flexe_phy_ports));
                    if (BCM_PBMP_NOT_NULL(all_flexe_phy_ports)
                        || (dnx_data_nif.flexe.flexe_mode_get(unit) == DNX_FLEXE_MODE_DISTRIBUTED))
                    {
                        SHR_IF_ERR_EXIT(imb_common_flexe_post_init(unit));
                    }
                }
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

#ifdef INCLUDE_XFLOW_MACSEC
/**
 * \brief - Part of imb_common_port_speed_config_set,
 *
 *          Handle the configuration of macsec context.
 *
 *          macsec context can be configured during bcm_xflow_macsec_port_control_set API
 *          or by using bcm_port_resource_set API.
 *
 *          Motivation is that we want to configure macsec as late as possible but before calling bcm_port_enable_set.
 *          This means that we might need to enable macsec configuration in either one of the following functions.
 *              * bcm_xflow_macsec_port_control_set
 *              * bcm_port_resource_set
 *
 *          scenario 1:
 *          macsec is enabled before speed was set
 *          mark "is_macsec_requested"
 *
 *          scenario 2:
 *          macsec is enabled and speed was set
 *          configure macsec
 *
 *          scenario 3:
 *          speed has changed on a port using macsec.
 *          If the speed was changed on an active macsec port, the macsec configuration will be cleared and
 *          a new macsec allocation/configuration will be made.
 *
 *          scenario 4:
 *          port is detached without disabling macsec using bcm_xflow_macsec_port_control_set API
 *          remove macsec port implicitly
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] enable - enable/disable macsec port. 0 - disable, 1 - enable.
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 */
int
imb_common_port_macsec_port_enable_set(
    int unit,
    bcm_port_t port,
    int enable)
{
    int is_macsec_requested, is_bypass;
    int nif_port;

    SHR_FUNC_INIT_VARS(unit);

    if (!dnx_data_macsec.general.is_macsec_enabled_get(unit))
    {
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(dnx_xflow_macsec_lport_to_macsec_nif_port_get(unit, port, &nif_port));

    /*
     * Was 'macsec port enable' called by user before calling bcm_port_resource_set?
     */
    SHR_IF_ERR_EXIT(dnx_sec_db.wrapper_sec.nif_port_info.is_macsec_requested.get(unit, nif_port, &is_macsec_requested));

    /*
     * Was macsec context already configured, and now we need to re-configure it?
     * is_bypass means that macsec context is not allocated
     */
    SHR_IF_ERR_EXIT(dnx_xflow_macsec_lport_is_bypassed_get(unit, port, &is_bypass));

    /*
     * macsec context is already reserved and macsec is configured
     * Need to clear those settings before continuing
     */
    if (!is_bypass)
    {
        /** disable the current macsec configuration */
        SHR_IF_ERR_EXIT(dnx_xflow_macsec_port_control_set(unit, 0, port, xflowMacsecPortEnable, 0));
    }

    if (enable && (is_macsec_requested || !is_bypass))
    {
        /** perform macsec configuration */
        SHR_IF_ERR_EXIT(dnx_xflow_macsec_port_control_set(unit, 0, port, xflowMacsecPortEnable, 1));
    }

    /** take macsec port out of reset */
    SHR_IF_ERR_EXIT(dnx_xflow_macsec_port_soft_reset_set(unit, port, IMB_COMMON_IN_RESET));
    SHR_IF_ERR_EXIT(dnx_xflow_macsec_port_soft_reset_set(unit, port, IMB_COMMON_OUT_OF_RESET));

exit:
    SHR_FUNC_EXIT;
}
#endif /* include_xflow_macsec */

/**
 * \brief - part of imb_port_resource_multi_set,
 *          for each changed port, set speed
 *          and tune everything needed for valid port
 *
 * \param [in] unit - chip unit id
 * \param [in] nof_ports - number of changed ports
 * \param [in] resource_arr - changed ports info look at bcm_port_resource_t
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * imb_port_resource_multi_set
 *   * bcm_port_resource_t
 *   * imb_pm_resource_config_t
 */
int
imb_common_port_speed_config_set(
    int unit,
    int nof_ports,
    const bcm_port_resource_t * resource_arr)
{

    int resource_index;
    uint32 flags;
    portmod_speed_config_t port_speed_config;
    dnx_algo_port_info_s port_info;

    int master_port;
    portmod_pbmp_t master_pbmp;
    SHR_FUNC_INIT_VARS(unit);

    PORTMOD_PBMP_CLEAR(master_pbmp);

    /** for each port, set speed*/
    for (resource_index = 0; resource_index < nof_ports; ++resource_index)
    {
        bcm_port_t port = resource_arr[resource_index].port;

        /** get port type */
        SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));

        /** skip ports that are not IMB */
        if (!DNX_ALGO_PORT_TYPE_IS_IMB(unit, port_info, FALSE))
        {
            continue;
        }

        /** get master port */
        SHR_IF_ERR_EXIT(dnx_algo_port_master_get(unit, port, 0, &master_port));

        /** skips ports already confiured */
        if (PORTMOD_PBMP_MEMBER(master_pbmp, master_port))
        {
            continue;
        }

        SHR_IF_ERR_EXIT(imb_common_pm_resource_to_speed_config_convert
                        (unit, &resource_arr[resource_index], &port_speed_config));

        /** set precoder */
        {
            int is_precoder_supported;
            /*
             * lp precoder bit in phy_lane_config bitmap can be set only via bcm_port_phy_control API. in resource API,
             * leave HW value of this bit as is, don't change it no matter what is passed to the function
             */
            SHR_IF_ERR_EXIT(dnx_algo_port_is_precoder_supported_get(unit, port, &is_precoder_supported));

            if (is_precoder_supported)
            {
                uint32 lp_prec;

                SHR_IF_ERR_EXIT(bcm_dnx_port_phy_control_get
                                (unit, port, BCM_PORT_PHY_CONTROL_LP_TX_PRECODER_ENABLE, &lp_prec));
                if (lp_prec)
                {
                    PORTMOD_PORT_PHY_LANE_CONFIG_LP_PREC_EN_SET(port_speed_config.lane_config);
                }
                else
                {
                    PORTMOD_PORT_PHY_LANE_CONFIG_LP_PREC_EN_CLEAR(port_speed_config.lane_config);
                }
            }
        }

        /** Apply Framer Wrapper configuration before PCS is configured */
        if (DNX_ALGO_PORT_TYPE_IS_FRAMER_PHY(unit, port_info))
        {
            flags = 0;
            if (resource_arr[resource_index].flags & BCM_PORT_RESOURCE_AM_TRANSPARENT)
            {
                IMB_FLEXE_AM_TRANSPARENT_SET(flags);
            }
            SHR_IF_ERR_EXIT(imb_flexe_core_port_config_set
                            (unit, port, flags, resource_arr[resource_index].base_flexe_instance,
                             resource_arr[resource_index].speed));
        }

#ifdef INCLUDE_XFLOW_MACSEC
        /** enable the MACSec core clk */
        if (dnx_data_macsec.general.feature_get(unit, dnx_data_macsec_general_macsec_block_exists) &&
            !dnx_data_macsec.general.feature_get(unit, dnx_data_macsec_general_is_shared_macsec_supported))
        {
            dnx_algo_port_info_s port_info;
            dnx_macsec_port_info_t macsec_port_info;

            /** Verify given port type supports MACSec (nif ethernet port) */
            SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));

            if (DNX_ALGO_PORT_TYPE_IS_NIF_ETH(unit, port_info, 0))
            {
                /** extract instance_id based on port */
                SHR_IF_ERR_EXIT(dnx_xflow_macsec_port_to_macsec_port_info_get(unit, port, &macsec_port_info));

                /** enable the MACSec core clk */
                SHR_IF_ERR_EXIT(dnx_xflow_macsec_control_power_down_force_disable_instance_set
                                (unit, macsec_port_info.macsec_id));
            }
        }
#endif /** INCLUDE_XFLOW_MACSEC */

        /** set speed in portmod */
        SHR_IF_ERR_EXIT(imb_port_speed_config_set(unit, port, &port_speed_config));

#ifdef INCLUDE_XFLOW_MACSEC
        /*
         * restore the MACSec core clk to previous position
         * Needs to run only on Gen2 implementation
         */
        if (dnx_data_macsec.general.feature_get(unit, dnx_data_macsec_general_macsec_block_exists) &&
            !dnx_data_macsec.general.feature_get(unit, dnx_data_macsec_general_is_shared_macsec_supported))
        {
            dnx_algo_port_info_s port_info;
            dnx_macsec_port_info_t macsec_port_info;

            /** Verify given port type supports MACSec (nif ethernet port) */
            SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));

            if (DNX_ALGO_PORT_TYPE_IS_NIF_ETH(unit, port_info, 0) == TRUE)
            {
                /** extract instance_id based on port */
                SHR_IF_ERR_EXIT(dnx_xflow_macsec_port_to_macsec_port_info_get(unit, port, &macsec_port_info));

                /** restore the MACSec core clk to previous state */
                SHR_IF_ERR_EXIT(dnx_xflow_macsec_control_power_down_force_disable_instance_restore
                                (unit, macsec_port_info.macsec_id));
            }
        }
#endif /** INCLUDE_XFLOW_MACSEC */

#ifdef INCLUDE_XFLOW_MACSEC
        /*
         * enable mascec port with the newly configured speed
         *
         * When resource_set is called twice in a row, apply the latest speed.
         * Meaning, need to reconfigure the macsec calender and allocate a new context.
         */
        if (dnx_data_macsec.general.is_macsec_enabled_get(unit) &&
            dnx_data_macsec.general.feature_get(unit, dnx_data_macsec_general_is_shared_macsec_supported))
        {
            if (DNX_ALGO_PORT_TYPE_IS_FRAMER_MAC(unit, port_info, FALSE, FALSE, FALSE) ||
                DNX_ALGO_PORT_TYPE_IS_NIF_ETH(unit, port_info, 0))
            {
                SHR_IF_ERR_EXIT(imb_common_port_macsec_port_enable_set(unit, port, 1 /** enable */ ));
            }
        }
#endif /** INCLUDE_XFLOW_MACSEC */

        /** prevent duplicate configuration in channelized cases */
        PORTMOD_PBMP_PORT_ADD(master_pbmp, master_port);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Modify the port attributes for all the ports in the resource array
 *
 *  see .h file
 */

int
imb_common_port_resource_multi_set(
    int unit,
    int nport,
    const bcm_port_resource_t * resource)
{

    int nof_pms;
    imb_pm_resource_config_t *pm_resource_config = NULL;
    int enable[SOC_MAX_NUM_PORTS];
    int nof_ethu_pms, nof_ilkn_units, nof_fabric_pms;
    SHR_FUNC_INIT_VARS(unit);
    /** verify is done in previous layer */

    nof_ethu_pms = dnx_data_nif.eth.total_nof_ethu_pms_in_device_get(unit);
    nof_ilkn_units = dnx_data_nif.ilkn.ilkn_unit_nof_get(unit);
    nof_fabric_pms = dnx_data_fabric.blocks.nof_pms_get(unit);

    nof_pms = nof_ethu_pms + nof_ilkn_units + nof_fabric_pms;

    SHR_ALLOC_ERR_EXIT(pm_resource_config, (sizeof(imb_pm_resource_config_t) * nof_pms),
                       "PM resource config", "%s%s%s", "pm_resource_config", EMPTY, EMPTY);
    /** we fill pm_resource_config with info regarding changed ports in each pm and the valid speed configuration*/
    SHR_IF_ERR_EXIT(imb_common_speed_config_db_build(unit, nport, resource, nof_pms, enable, pm_resource_config));
    /** we validate speed configuration*/
    SHR_IF_ERR_EXIT(imb_common_pm_speed_config_validate(unit, nof_pms, pm_resource_config));
    /** we reconfigure PM VCO for each changed PM */
    SHR_IF_ERR_EXIT(imb_common_speed_pm_vco_config_set(unit, nof_pms, pm_resource_config));
    /**speed set and tune phase */
    SHR_IF_ERR_EXIT(imb_common_port_speed_config_set(unit, nport, resource));

exit:
    SHR_FREE(pm_resource_config);

    SHR_FUNC_EXIT;
}
/**
 * \brief - Run per IMB configurations that are needed after calling resource_multi_set
 *
 *  see .h file
 */

int
imb_common_post_resource_multi_set(
    int unit,
    int nport,
    bcm_pbmp_t ports_with_modified_rx_speed,
    bcm_pbmp_t ports_with_modified_tx_speed,
    const bcm_port_resource_t * resource)
{
    dnx_algo_port_info_s *ports_info = NULL;
    int resource_index;
    bcm_port_prio_config_t priority_config;
    bcm_port_t logical_port;
    bcm_pbmp_t calendars_to_be_set;
    SHR_FUNC_INIT_VARS(unit);

    SHR_ALLOC_SET_ZERO(ports_info,
                       nport * sizeof(dnx_algo_port_info_s), "User ports info", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    for (resource_index = 0; resource_index < nport; ++resource_index)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, resource[resource_index].port, &ports_info[resource_index]));
    }

    if (dnx_data_nif.oft.feature_get(unit, dnx_data_nif_oft_is_supported) &&
        dnx_data_nif.oft.feature_get(unit, dnx_data_nif_oft_is_used))
    {
        /**
         * Set the OFT Link list
         * allocates and configures the OFT Link list and all associated controllers to HW.
         * code inside is iterating on all ports in the interface
         */
        for (resource_index = 0; resource_index < nport; ++resource_index)
        {
            if (DNX_ALGO_PORT_TYPE_IS_NIF_ETH(unit, ports_info[resource_index], 0)
                || DNX_ALGO_PORT_TYPE_IS_FRAMER_MAC(unit, ports_info[resource_index], FALSE, FALSE, FALSE))

            {
                SHR_IF_ERR_EXIT(dnx_port_oft_link_list_set(unit, resource[resource_index].port, TRUE));

            }
        }
        /**
         * Set the OFT Calendar
         */
        if (_SHR_PBMP_NOT_NULL(ports_with_modified_tx_speed))
        {
            SHR_IF_ERR_EXIT(dnx_port_oft_calendar_set(unit));
        }
    }

    if (dnx_data_nif.arb.feature_get(unit, dnx_data_nif_arb_is_supported))
    {
        /*
         * Resolve relevant arbiter calendar from ports bitmap
         */
        _SHR_PBMP_CLEAR(calendars_to_be_set);
        _SHR_PBMP_ITER(ports_with_modified_tx_speed, logical_port)
        {
            SHR_IF_ERR_EXIT(dnx_port_arb_tx_calendar_ids_get(unit, logical_port, &calendars_to_be_set));
        }

        _SHR_PBMP_ITER(ports_with_modified_rx_speed, logical_port)
        {
            SHR_IF_ERR_EXIT(dnx_port_arb_rx_calendar_ids_get(unit, logical_port, &calendars_to_be_set));
        }

        for (resource_index = 0; resource_index < nport; ++resource_index)
        {
            if (DNX_ALGO_PORT_TYPE_IS_FRAMER_MAC(unit, ports_info[resource_index], FALSE, FALSE, FALSE))
            {
                /**
                 * Set the ARB TMAC Link list
                 * allocates and configures the ARB TX TMAC Link list and all associated controllers to HW.
                 * code inside is iterating on all ports in the interface
                 */
                SHR_IF_ERR_EXIT(dnx_port_arb_tx_tmac_link_list_set(unit, resource[resource_index].port, TRUE));

                /**
                 * Set port's start tx threshold
                 */
                SHR_IF_ERR_EXIT(dnx_port_logical_get(unit, resource[resource_index].port, &logical_port));
                SHR_IF_ERR_EXIT(dnx_port_arb_tmac_start_tx_threshold_set(unit, logical_port));
            }
        }

        /**
         * Set the ARB Calendars
         */
        if (_SHR_PBMP_NOT_NULL(calendars_to_be_set))
        {
            SHR_IF_ERR_EXIT(dnx_port_arb_calendar_set(unit, calendars_to_be_set));
        }
    }

    if (dnx_data_nif.ofr.feature_get(unit, dnx_data_nif_ofr_is_supported))
    {
        /**
         * Set the OFR RX Memory Link list
         * allocates and configures the OFR RX Memory Link list and all associated controllers to HW.
         * code inside is iterating on all ports in the interface
         */
        for (resource_index = 0; resource_index < nport; ++resource_index)
        {
            if (DNX_ALGO_PORT_TYPE_IS_NIF_ETH(unit, ports_info[resource_index], DNX_ALGO_PORT_TYPE_INCLUDE(MGMT))
                || DNX_ALGO_PORT_TYPE_IS_FRAMER_MAC(unit, ports_info[resource_index], FALSE, FALSE, FALSE)
                || DNX_ALGO_PORT_TYPE_IS_NIF_ILKN(unit, ports_info[resource_index], DNX_ALGO_PORT_TYPE_INCLUDE(L1)))
            {
                /**
                 * Set the OFR  scheduler for this context
                 */
                SHR_IF_ERR_EXIT(dnx_port_ofr_rx_sch_context_set(unit, resource[resource_index].port));

                if (!DNX_ALGO_PORT_TYPE_IS_NIF_ILKN(unit, ports_info[resource_index], DNX_ALGO_PORT_TYPE_INCLUDE(L1)))
                {
                    /*
                     * Get priority configuration for this port (read from SW state)
                     */
                    SHR_IF_ERR_EXIT(imb_port_priority_config_get
                                    (unit, resource[resource_index].port, &priority_config));

                    /*
                     * Set configuration in  HW
                     */
                    SHR_IF_ERR_EXIT(dnx_port_ofr_port_priority_hw_set
                                    (unit, resource[resource_index].port, &priority_config));
                }
            }
        }
    }

exit:
    SHR_FREE(ports_info);
    SHR_FUNC_EXIT;

}
/**
 * \brief - get the attributes of spcific port
 *
 *  see .h file
 */

int
imb_common_port_resource_get(
    int unit,
    bcm_gport_t port,
    bcm_port_resource_t * resource)
{
    portmod_speed_config_t port_speed_config;
    int speed;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(resource, _SHR_E_PARAM, "resource");

    /** check if speed is 0, if so, it's invalid port configuration */
    SHR_IF_ERR_EXIT(dnx_algo_port_speed_get(unit, port, 0, &speed));

    SHR_IF_ERR_EXIT(portmod_speed_config_t_init(unit, &port_speed_config));
    SHR_IF_ERR_EXIT(portmod_port_speed_config_get(unit, port, &port_speed_config));

    PORTMOD_PORT_PHY_LANE_CONFIG_FORCE_NRZ_CLEAR(port_speed_config.lane_config);
    PORTMOD_PORT_PHY_LANE_CONFIG_FORCE_PAM4_CLEAR(port_speed_config.lane_config);
    /*
     * LP precoder FW bit is not configured by resource API but by bcm_port_phy_control API, so here we mask-out this
     * bit
     */
    PORTMOD_PORT_PHY_LANE_CONFIG_LP_PREC_EN_CLEAR(port_speed_config.lane_config);

    bcm_port_resource_t_init(resource);

    /** get the info from portmod_speed_config */
    imb_common_speed_config_to_pm_resource_convert(&port_speed_config, resource);

    if (!dnx_data_port.general.feature_get(unit, dnx_data_port_general_use_portmod_speed_when_not_enabled))
    {
        int enable;

        /*
         * check if the port is disabled
         */
        SHR_IF_ERR_EXIT(imb_port_enable_get(unit, port, &enable));
        if (!enable)
        {
            resource->speed = speed;
        }
    }
    resource->port = port;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Validate whether a port can change
 *          its attributes (speed, FEC, CL72 etc.) without affecting other ports.
 *
 *  see .h file
 */
int
imb_common_port_speed_config_validate(
    int unit,
    const bcm_port_resource_t * resource,
    bcm_pbmp_t * affected_pbmp)
{
    portmod_speed_config_t speed_config;
    portmod_pbmp_t portmod_pbmp;
    bcm_pbmp_t phys;
    int num_lane, port;
    int rv = BCM_E_NONE;
    SHR_FUNC_INIT_VARS(unit);

    BCM_PBMP_CLEAR(*affected_pbmp);
    SHR_IF_ERR_EXIT(portmod_speed_config_t_init(unit, &speed_config));

    /*
     * Non NIF ports will be filtered in the 'verify' function
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_nif_phys_get(unit, resource->port, 0, &phys));
    BCM_PBMP_COUNT(phys, num_lane);
    speed_config.num_lane = num_lane;
    speed_config.speed = resource->speed;
    speed_config.fec = (portmod_fec_t) resource->fec_type;
    speed_config.link_training = resource->link_training;
    speed_config.lane_config = resource->phy_lane_config;

    rv = portmod_port_speed_config_validate(unit, resource->port,
                                            (const portmod_speed_config_t *) &speed_config, &portmod_pbmp);
    if (SHR_FAILURE(rv))
    {
        PORTMOD_PBMP_ITER(portmod_pbmp, port)
        {
            BCM_PBMP_PORT_ADD(*affected_pbmp, port);
        }
    }
    SHR_IF_ERR_EXIT(rv);
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Calculate the over sampling factor for the port.
 *        mostly in use when calculating serdes rate for the
 *        port
 *
 *  see .h file
 */
int
imb_common_port_over_sampling_get(
    int unit,
    bcm_port_t port,
    uint32 *os_int,
    uint32 *os_remainder)
{
    int nof_phys;
    portmod_access_get_params_t params;
    phymod_phy_diagnostics_t phy_diag;
    phymod_phy_access_t *phy_access = NULL;
    int max_core_access_per_port = dnx_data_port.general.max_core_access_per_port_get(unit);
    SHR_FUNC_INIT_VARS(unit);

    SHR_ALLOC_ERR_EXIT(phy_access, (sizeof(phymod_phy_access_t) * max_core_access_per_port),
                       "PHY access config", "%s%s%s", "phy_access", EMPTY, EMPTY);

    /*
     * initialize structures
     */
    SHR_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    SHR_IF_ERR_EXIT(phymod_phy_diagnostics_t_init(&phy_diag));
    /*
     * Internal PHY - get phy access
     */
    params.phyn = 0;
    SHR_IF_ERR_EXIT(portmod_port_phy_lane_access_get
                    (unit, port, &params, max_core_access_per_port, phy_access, &nof_phys, NULL));
    /*
     * get phy diagnostics
     */
    SHR_IF_ERR_EXIT(phymod_phy_diagnostics_get(phy_access, &phy_diag));
    /*
     * get OS fron osr mode
     */
    SHR_IF_ERR_EXIT(phymod_osr_mode_to_actual_os(phy_diag.osr_mode, os_int, os_remainder));
exit:
    SHR_FREE(phy_access);
    SHR_FUNC_EXIT;
}

/**
 * \brief - sync the MAC status according to the PHY status.
 *
 *  see .h file
 */
int
imb_common_port_link_up_mac_update(
    int unit,
    bcm_port_t port,
    int link)
{
    portmod_port_update_control_t port_update_control;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(portmod_port_update_control_t_init(unit, &port_update_control));
    port_update_control.link_status = link;
    SHR_IF_ERR_EXIT(portmod_port_update(unit, port, &port_update_control));

    if (link)
    {
        SHR_IF_ERR_EXIT(portmod_port_phy_link_up_event(unit, port));
    }
    else
    {
        SHR_IF_ERR_EXIT(portmod_port_phy_link_down_event(unit, port));
    }

    SHR_IF_ERR_EXIT(imb_port_link_up_mac_update_post(unit, port, link));

exit:
    SHR_FUNC_EXIT;
}

int
imb_common_port_link_up_mac_update_post_none(
    int unit,
    bcm_port_t port,
    int link)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_FUNC_EXIT;
}

int
imb_common_port_link_up_mac_update_post_reset_mac(
    int unit,
    bcm_port_t port,
    int link)
{
    dnx_algo_port_ethu_access_info_t ethu_info;
    phymod_autoneg_status_t an_status;
    int null_rx_enable;
    uint64 null_mac_ctrl;

    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_algo_port_ethu_access_info_get(unit, port, &ethu_info));

    sal_memset(&an_status, 0, sizeof(an_status));
    SHR_IF_ERR_EXIT(portmod_port_autoneg_status_get(unit, port, &an_status));
    /*
     * toggle portmod soft reset in and out
     */
    if ((an_status.enabled && an_status.locked && link) ||
        (an_status.enabled && link &&
         (an_status.resolved_num_lane == dnx_data_port.imb.imb_type_info_get(unit, ethu_info.imb_type)->nof_lanes)))
    {
        SHR_IF_ERR_EXIT(portmod_port_soft_reset
                        (unit, port, portmodMacSoftResetModeIn_Out, &null_rx_enable, &null_mac_ctrl));
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - convert bcm sch_prio to DBAL sch_prio
 *
 *  see .h file
 */
int
imb_common_dbal_sch_prio_from_bcm_sch_prio_get(
    int unit,
    bcm_port_nif_scheduler_t bcm_sch_prio,
    uint32 *dbal_sch_prio)
{
    SHR_FUNC_INIT_VARS(unit);
    switch (bcm_sch_prio)
    {
        case bcmPortNifSchedulerLow:
            *dbal_sch_prio = DBAL_ENUM_FVAL_NIF_SCH_PRIORITY_LOW;
            break;
        case bcmPortNifSchedulerHigh:
            *dbal_sch_prio = DBAL_ENUM_FVAL_NIF_SCH_PRIORITY_HIGH;
            break;
        case bcmPortNifSchedulerTDM:
            if (dnx_data_tdm.params.feature_get(unit, dnx_data_tdm_params_is_supported))
            {
                *dbal_sch_prio = DBAL_ENUM_FVAL_NIF_SCH_PRIORITY_TDM;
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "TDM feature is not supported\n");
            }
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "No type %d in bcm_port_nif_scheduler_t\n", bcm_sch_prio);
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - convert to DBAL weight from reguler int (0-4)
 *
 *  see .h file
 */
int
imb_common_dbal_sch_weight_get(
    int unit,
    int weight,
    uint32 *dbal_sch_weight)
{
    SHR_FUNC_INIT_VARS(unit);
    switch (weight)
    {
        case 0:
            *dbal_sch_weight = DBAL_ENUM_FVAL_NIF_SCH_WEIGHT_WEIGHT_0;
            break;
        case 1:
            *dbal_sch_weight = DBAL_ENUM_FVAL_NIF_SCH_WEIGHT_WEIGHT_1;
            break;
        case 2:
            *dbal_sch_weight = DBAL_ENUM_FVAL_NIF_SCH_WEIGHT_WEIGHT_2;
            break;
        case 3:
            *dbal_sch_weight = DBAL_ENUM_FVAL_NIF_SCH_WEIGHT_WEIGHT_3;
            break;
        case 4:
            *dbal_sch_weight = DBAL_ENUM_FVAL_NIF_SCH_WEIGHT_WEIGHT_4;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid weight %d\n", weight);
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get port local and emote faults status
 *
 *  see .h file
 */
int
imb_common_port_faults_status_get(
    int unit,
    bcm_port_t port,
    int *local_fault,
    int *remote_fault)
{
    portmod_port_fault_status_t faults;

    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(portmod_port_faults_status_get(unit, port, &faults));
    *local_fault = faults.local;
    *remote_fault = faults.remote;

exit:
    SHR_FUNC_EXIT;
}

/*Get/set the link fault forcing.*/
int
imb_common_port_link_fault_force_set(
    int unit,
    bcm_port_t port,
    bcm_port_control_t type,
    int enable)
{
    SHR_FUNC_INIT_VARS(unit);

    if (type == bcmPortControlLinkFaultLocalForce)
    {
        SHR_IF_ERR_EXIT(portmod_port_local_fault_force_set(unit, port, enable));
    }
    else if (type == bcmPortControlLinkFaultRemoteForce)
    {
        SHR_IF_ERR_EXIT(portmod_port_remote_fault_force_set(unit, port, enable));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "type %d is not supported\n", type);
    }

exit:
    SHR_FUNC_EXIT;
}

int
imb_common_port_link_fault_force_get(
    int unit,
    bcm_port_t port,
    bcm_port_control_t type,
    int *enable)
{
    SHR_FUNC_INIT_VARS(unit);

    if (type == bcmPortControlLinkFaultLocalForce)
    {
        SHR_IF_ERR_EXIT(portmod_port_local_fault_force_get(unit, port, enable));
    }
    else if (type == bcmPortControlLinkFaultRemoteForce)
    {
        SHR_IF_ERR_EXIT(portmod_port_remote_fault_force_get(unit, port, enable));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "type %d is not supported\n", type);
    }

exit:
    SHR_FUNC_EXIT;
}

static int
imb_common_covert_bcm_to_portmod_preemption_control(
    int unit,
    bcm_port_preempt_control_t bcm_type,
    portmod_preemption_control_t * portmod_type)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (bcm_type)
    {
        case bcmPortPreemptControlEnableTx:
            *portmod_type = portmodPreemptionControlEnableTx;
            break;
        case bcmPortPreemptControlVerifyEnable:
            *portmod_type = portmodPreemptionControlVerifyEnable;
            break;
        case bcmPortPreemptControlVerifyTime:
            *portmod_type = portmodPreemptionControlVerifyTime;
            break;
        case bcmPortPreemptControlVerifyAttempts:
            *portmod_type = portmodPreemptionControlVerifyAttempts;
            break;
        case bcmPortPreemptControlNonFinalFragSizeRx:
            *portmod_type = portmodPreemptionControlNonFinalFragSizeRx;
            break;
        case bcmPortPreemptControlFinalFragSizeRx:
            *portmod_type = portmodPreemptionControlFinalFragSizeRx;
            break;

        default:
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Invalid preemption control type(%d)\n", bcm_type);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

int
imb_common_preemption_control_set(
    int unit,
    bcm_port_t port,
    bcm_port_preempt_control_t type,
    uint32 value)
{
    portmod_preemption_control_t portmod_type;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imb_common_covert_bcm_to_portmod_preemption_control(unit, type, &portmod_type));
    SHR_IF_ERR_EXIT(imb_er_portmod_preemption_control_set(unit, port, portmod_type, value));

exit:
    SHR_FUNC_EXIT;
}

int
imb_common_preemption_control_get(
    int unit,
    bcm_port_t port,
    bcm_port_preempt_control_t type,
    uint32 *value)
{
    portmod_preemption_control_t portmod_type;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imb_common_covert_bcm_to_portmod_preemption_control(unit, type, &portmod_type));
    SHR_IF_ERR_EXIT(portmod_preemption_control_get(unit, port, portmod_type, value));

exit:
    SHR_FUNC_EXIT;
}

static int
imb_common_covert_portmod_to_bcm_preemption_verify_status(
    int unit,
    portmod_preemption_verify_status_t portmod_type,
    bcm_port_preempt_status_verify_t * bcm_type)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (portmod_type)
    {
        case portmodPreemptionVerifyStatusDisabled:
            *bcm_type = bcmPortPreemptStatusVerifyDisabled;
            break;
        case portmodPreemptionVerifyStatusInitial:
            *bcm_type = bcmPortPreemptStatusVerifyInitial;
            break;
        case portmodPreemptionVerifyStatusInProgress:
            *bcm_type = bcmPortPreemptStatusVerifyInProgress;
            break;
        case portmodPreemptionVerifyStatusSucceeded:
            *bcm_type = bcmPortPreemptStatusVerifySucceeded;
            break;
        case portmodPreemptionVerifyStatusFailed:
            *bcm_type = bcmPortPreemptStatusVerifyFailed;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Invalid preemption verify status type(%d)\n", portmod_type);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

int
imb_common_preemption_verify_status_get(
    int unit,
    bcm_port_t port,
    uint32 *value)
{
    portmod_preemption_verify_status_t portmod_type;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(portmod_preemption_verify_status_get(unit, port, portmodPreemptionVerifyStatus, &portmod_type));
    SHR_IF_ERR_EXIT(imb_common_covert_portmod_to_bcm_preemption_verify_status(unit, portmod_type, value));

exit:
    SHR_FUNC_EXIT;
}

static int
imb_common_covert_portmod_to_bcm_preemption_tx_status(
    int unit,
    portmod_preemption_tx_status_t portmod_type,
    bcm_port_preempt_status_tx_t * bcm_type)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (portmod_type)
    {
        case portmodPreemptionTxStatusInactive:
            *bcm_type = bcmPortPreemptStatusTxInactive;
            break;
        case portmodPreemptionTxStatusVerifying:
            *bcm_type = bcmPortPreemptStatusTxVerifying;
            break;
        case portmodPreemptionTxStatusActive:
            *bcm_type = bcmPortPreemptStatusTxActive;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Invalid preemption verify status type(%d)\n", portmod_type);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

int
imb_common_preemption_tx_status_get(
    int unit,
    bcm_port_t port,
    uint32 *value)
{
    portmod_preemption_tx_status_t portmod_type;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(portmod_preemption_tx_status_get(unit, port, &portmod_type));
    SHR_IF_ERR_EXIT(imb_common_covert_portmod_to_bcm_preemption_tx_status(unit, portmod_type, value));

exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Configure the TXPI SDM divider in portmod
 *
 *  see .h file
 */
int
imb_common_port_txpi_sdm_divider_set(
    int unit,
    bcm_port_t port,
    int speed)
{
    int nof_phys, lane_speed, index, sdm_div;
    dnx_algo_port_info_s port_info;
    portmod_port_synce_clk_ctrl_t clk_ctrl;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Get the lane speed
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));
    if (DNX_ALGO_PORT_TYPE_IS_OTN_PHY(unit, port_info))
    {
        lane_speed = speed;
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_nif_phys_nof_get(unit, port, &nof_phys));
        lane_speed = speed / nof_phys;
    }
    /*
     * Get SDM value
     */
    for (index = dnx_data_nif.phys.txpi_sdm_div_info_get(unit)->key_size[0] - 1; index >= 0; index--)
    {
        if (speed >= dnx_data_nif.phys.txpi_sdm_div_get(unit, index)->speed)
        {
            sdm_div = dnx_data_nif.phys.txpi_sdm_div_get(unit, index)->val;
            break;
        }
    }
    if (index < 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Invalid lane speed %d for TXPI.\n", lane_speed);
    }
    /*
     * Configure divider for Rx clock in portmod
     */
    portmod_port_synce_clk_ctrl_t_init(unit, &clk_ctrl);
    clk_ctrl.stg0_mode = DNX_STAGE0_SDM_FRAC_DIV_MODE;
    clk_ctrl.sdm_val = sdm_div;
    SHR_IF_ERR_EXIT(portmod_port_synce_clk_ctrl_set(unit, port, &clk_ctrl));
    /*
     * Configure divider for Rx clock in NIF
     */
    SHR_IF_ERR_EXIT(dnx_nif_synce_feedback_clk_sdm_divider_set(unit, port, sdm_div));
exit:
    SHR_FUNC_EXIT;
}

/*
 * }
 */

#undef _ERR_MSG_MODULE_NAME
