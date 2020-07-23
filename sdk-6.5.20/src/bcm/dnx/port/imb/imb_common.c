/** \file imb_common.c
 *
 *  Common functions for IMB layer
 */
/*
 *
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*
 * Include files.
 * {
 */

#include <shared/shrextend/shrextend_debug.h>

#include <bcm_int/dnx/port/imb/imb_common.h>
#include <bcm_int/dnx/port/imb/imb_internal.h>

#include <soc/portmod/portmod.h>
#include <soc/portmod/portmod_common.h>
#include <soc/phy/phymod_port_control.h>
#include <soc/phyreg.h>
#include <bcm_int/dnx/port/imb/imb.h>
#include <bcm_int/dnx/port/port_flexe.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/algo/flexe_general/algo_flexe_general.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_nif.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_fabric.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_tdm.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_port.h>
#include <bcm_int/dnx/cosq/egress/egr_queuing.h>
#include <soc/dnxc/dnxc_port.h>
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
    if (DNX_ALGO_PORT_TYPE_IS_NIF(unit, port_info, TRUE /* ELK */ , TRUE /* STIF */ , TRUE /* L1 */ , TRUE      /* FLEXE 
                                                                                                                 */ ))
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_nif_phys_get(unit, resource->port, 0, &phys));
        BCM_PBMP_COUNT(phys, num_lane);
        port_speed_config->num_lane = num_lane;
        lane_speed =
            DNX_ALGO_PORT_TYPE_IS_NIF_ILKN(unit, port_info, 1,
                                           1) ? port_speed_config->speed : (port_speed_config->speed / num_lane);
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
 * \brief - Write PHY registers associated with a port
 *
 *  see .h file
 */
int
imb_common_port_phy_reg_set(
    int unit,
    bcm_port_t port,
    uint32 flags,
    uint32 phy_reg_addr,
    uint32 phy_data)
{
    uint16 phy_wr_data;
    uint32 reg_flag, phy_id, phy_reg;
    int rv;
    int nof_phys_structs = 0;
    phymod_phy_access_t phy_access[DNX_DATA_MAX_NIF_GLOBAL_MAX_CORE_ACCESS_PER_PORT];
    portmod_access_get_params_t params;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    if (flags & SOC_PHY_INTERNAL)
    {
        params.phyn = 0;
    }

    if ((flags & SOC_PHY_NOMAP) == 0)
    {
        rv = portmod_port_phy_lane_access_get(unit, port, &params, DNX_DATA_MAX_NIF_GLOBAL_MAX_CORE_ACCESS_PER_PORT,
                                              phy_access, &nof_phys_structs, NULL);
        SHR_IF_ERR_EXIT(rv);
    }

    if (flags & (SOC_PHY_I2C_DATA8 | SOC_PHY_I2C_DATA16))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "unsupported flags for ramon\n");
    }

    reg_flag = SOC_PHY_REG_FLAGS(phy_reg_addr);
    if (reg_flag & SOC_PHY_REG_INDIRECT)
    {
        if (flags & BCM_PORT_PHY_NOMAP)
        {
            /*
             * Indirect register access is performed through PHY driver. Therefore, indirect register access is not
             * supported if SOC_PHY_NOMAP flag is set.
             */
            SHR_ERR_EXIT(_SHR_E_PARAM, "indirect register access is not supported if SOC_PHY_NOMAP flag is set\n");
        }
        phy_reg_addr &= ~SOC_PHY_REG_INDIRECT;
        rv = phymod_phy_reg_write(phy_access, phy_reg_addr, phy_data);
        SHR_IF_ERR_EXIT(rv);
    }
    else
    {
        if (flags & BCM_PORT_PHY_NOMAP)
        {
            phy_id = port;
            phy_reg = phy_reg_addr;
            phy_wr_data = phy_data & 0xffff;
            rv = soc_cmicx_miim_operation(unit, 1 /* is_write */ , 45, phy_id, phy_reg, &phy_wr_data);
            SHR_IF_ERR_EXIT(rv);
        }
        else
        {
            PHYMOD_LOCK_TAKE(phy_access);
            rv = PHYMOD_BUS_WRITE(&phy_access[0].access, phy_reg_addr, phy_data);
            PHYMOD_LOCK_GIVE(phy_access);
            SHR_IF_ERR_EXIT(rv);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Read PHY registers associated with a port
 *
 *  see .h file
 */
int
imb_common_port_phy_reg_get(
    int unit,
    bcm_port_t port,
    uint32 flags,
    uint32 phy_reg_addr,
    uint32 *phy_data)
{
    uint16 phy_rd_data = 0;
    uint32 reg_flag, phy_id, phy_reg;
    int rv;
    int nof_phys_structs = 0;
    phymod_phy_access_t phy_access[DNX_DATA_MAX_NIF_GLOBAL_MAX_CORE_ACCESS_PER_PORT];
    portmod_access_get_params_t params;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    if (flags & SOC_PHY_INTERNAL)
    {
        params.phyn = 0;
    }

    if ((flags & SOC_PHY_NOMAP) == 0)
    {
        rv = portmod_port_phy_lane_access_get(unit, port, &params, DNX_DATA_MAX_NIF_GLOBAL_MAX_CORE_ACCESS_PER_PORT,
                                              phy_access, &nof_phys_structs, NULL);
        SHR_IF_ERR_EXIT(rv);
    }

    if (flags & (SOC_PHY_I2C_DATA8 | SOC_PHY_I2C_DATA16))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "unsupported flags for ramon\n");
    }

    reg_flag = SOC_PHY_REG_FLAGS(phy_reg_addr);
    if (reg_flag & SOC_PHY_REG_INDIRECT)
    {
        if (flags & BCM_PORT_PHY_NOMAP)
        {
            /*
             * Indirect register access is performed through PHY driver. Therefore, indirect register access is not
             * supported if BCM_PORT_PHY_NOMAP flag is set.
             */
            SHR_ERR_EXIT(_SHR_E_PARAM, "indirect register access is not supported if SOC_PHY_NOMAP flag is set\n");
        }
        phy_reg_addr &= ~SOC_PHY_REG_INDIRECT;
        rv = phymod_phy_reg_read(phy_access, phy_reg_addr, phy_data);
        SHR_IF_ERR_EXIT(rv);
    }
    else
    {
        if (flags & SOC_PHY_NOMAP)
        {
            phy_id = port;
            phy_reg = phy_reg_addr;
            rv = soc_cmicx_miim_operation(unit, 0 /* is_write */ , 45, phy_id, phy_reg, &phy_rd_data);
            SHR_IF_ERR_EXIT(rv);
            *phy_data = phy_rd_data;
        }
        else
        {
            PHYMOD_LOCK_TAKE(phy_access);
            rv = PHYMOD_BUS_READ(&phy_access[0].access, phy_reg_addr, phy_data);
            PHYMOD_LOCK_GIVE(phy_access);
            SHR_IF_ERR_EXIT(rv);
        }
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
    uint32 nof_lanes_per_pm;
    dnx_algo_port_info_s port_info;
    portmod_dispatch_type_t pm_type;
    dnx_algo_port_ethu_access_info_t ethu_access_info;
    SHR_FUNC_INIT_VARS(unit);

    nof_lanes_per_pm = dnx_data_nif.eth.nof_lanes_in_cdu_get(unit);

    /** portmod_speed_config_t array initialization */
    for (pm_index = 0; pm_index < nof_pms_max; ++pm_index)
    {
        for (lane_index = 0; lane_index < DNX_DATA_MAX_NIF_ETH_NOF_LANES_IN_CDU; ++lane_index)
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
        if (DNX_ALGO_PORT_TYPE_IS_NIF_ILKN(unit, port_info, 1, 1))
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
            if (DNX_ALGO_PORT_TYPE_IS_NIF_ILKN(unit, port_info, 1, 1))
            {
                pm_resource_config[pm_ids[pm_index]].speed_for_pcs_bypass_port[config_info_index] = 1;
            }
            else
            {
                if (DNX_ALGO_PORT_TYPE_IS_NIF(unit, port_info, 0, 1, 1, 1))
                {
                    SHR_IF_ERR_EXIT(dnx_algo_port_nif_first_phy_get
                                    (unit, resource_arr[resource_index].port, 0, &first_phy));
                    SHR_IF_ERR_EXIT(dnx_algo_port_ethu_access_info_get
                                    (unit, resource_arr[resource_index].port, &ethu_access_info));
                    nof_lanes_per_pm =
                        dnx_data_port.imb.imb_type_info_get(unit, ethu_access_info.imb_type)->nof_lanes_in_pm;

                    /** set TVCO 26G flag only for NIF ETH L2 ports */
                    if (DNX_ALGO_PORT_TYPE_IS_NIF_ETH(unit, port_info, 0, 0) &&
                        (resource_arr[resource_index].flags & BCM_PORT_RESOURCE_TVCO_PRIORITIZE_26P562G))
                    {
                        pm_resource_config[pm_ids[pm_index]].flags |= BCM_PORT_RESOURCE_TVCO_PRIORITIZE_26P562G;
                    }
                }
                else if (DNX_ALGO_PORT_TYPE_IS_FABRIC(unit, port_info))
                {
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
        if (dnx_data_nif.flexe.feature_get(unit, dnx_data_nif_flexe_is_supported))
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
    portmod_speed_config_t port_speed_config;
    dnx_algo_port_info_s port_info;
    portmod_speed_config_t prev_speed_config;
    uint32 lp_prec;
    int master_port;
    portmod_pbmp_t master_pbmp;
    SHR_FUNC_INIT_VARS(unit);

    PORTMOD_PBMP_CLEAR(master_pbmp);

    /** for each port, set speed*/
    for (resource_index = 0; resource_index < nof_ports; ++resource_index)
    {
        /** get port type */
        SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, resource_arr[resource_index].port, &port_info));

        /** skip ports that are not IMB */
        if (!DNX_ALGO_PORT_TYPE_IS_IMB(unit, port_info, FALSE))
        {
            continue;
        }

        /** get master port */
        SHR_IF_ERR_EXIT(dnx_algo_port_master_get(unit, resource_arr[resource_index].port, 0, &master_port));

        /** skips ports already confiured */
        if (PORTMOD_PBMP_MEMBER(master_pbmp, master_port))
        {
            continue;
        }

        SHR_IF_ERR_EXIT(imb_common_pm_resource_to_speed_config_convert
                        (unit, &resource_arr[resource_index], &port_speed_config));

        /*
         * lp precoder bit in phy_lane_config bitmap can be set only via bcm_port_phy_control API. in resource API,
         * leave HW value of this bit as is, don't change it no matter what is passed to the function
         */
        SHR_IF_ERR_EXIT(portmod_port_speed_config_get(unit, resource_arr[resource_index].port, &prev_speed_config));
        lp_prec = PORTMOD_PORT_PHY_LANE_CONFIG_LP_PREC_EN_GET(prev_speed_config.lane_config);
        if (lp_prec)
        {
            PORTMOD_PORT_PHY_LANE_CONFIG_LP_PREC_EN_SET(port_speed_config.lane_config);
        }
        else
        {
            PORTMOD_PORT_PHY_LANE_CONFIG_LP_PREC_EN_CLEAR(port_speed_config.lane_config);
        }
        /** Apply FlexE configuration before PCS is configured */
        if (DNX_ALGO_PORT_TYPE_IS_FLEXE_PHY(unit, port_info))
        {
            SHR_IF_ERR_EXIT(imb_flexe_core_port_config_set
                            (unit, resource_arr[resource_index].port, resource_arr[resource_index].base_flexe_instance,
                             resource_arr[resource_index].speed));
        }
        /** set speed in portmod */
        SHR_IF_ERR_EXIT(imb_port_speed_config_set(unit, resource_arr[resource_index].port, &port_speed_config));

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
    int nof_cdus_pms, nof_clu_pms, nof_ilkn_units, nof_fabric_pms;
    SHR_FUNC_INIT_VARS(unit);
    /** verify is done in previous layer */

    nof_cdus_pms = dnx_data_nif.eth.cdu_nof_get(unit) * dnx_data_nif.eth.nof_pms_in_cdu_get(unit);
    nof_clu_pms = dnx_data_nif.eth.clu_nof_get(unit) * dnx_data_nif.eth.nof_pms_in_clu_get(unit);
    nof_ilkn_units = dnx_data_nif.ilkn.ilkn_unit_nof_get(unit);
    nof_fabric_pms = dnx_data_fabric.blocks.nof_pms_get(unit);

    nof_pms = nof_cdus_pms + nof_clu_pms + nof_ilkn_units + nof_fabric_pms;

    pm_resource_config = sal_alloc((sizeof(imb_pm_resource_config_t) * nof_pms), "PM resource config");
    SHR_NULL_CHECK(pm_resource_config, _SHR_E_MEMORY, "pm_resource_config");
    /** we fill pm_resource_config with info regarding changed ports in each pm and the valid speed configuration*/
    SHR_IF_ERR_EXIT(imb_common_speed_config_db_build(unit, nport, resource, nof_pms, enable, pm_resource_config));
    /** we validate speed configuration*/
    SHR_IF_ERR_EXIT(imb_common_pm_speed_config_validate(unit, nof_pms, pm_resource_config));
    /** we reconfigure PM VCO for each changed PM */
    SHR_IF_ERR_EXIT(imb_common_speed_pm_vco_config_set(unit, nof_pms, pm_resource_config));
    /**speed set and tune phase */
    SHR_IF_ERR_EXIT(imb_common_port_speed_config_set(unit, nport, resource));

exit:
    if (pm_resource_config != NULL)
    {
        sal_free(pm_resource_config);
    }

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
    phymod_phy_access_t phy_access[DNX_DATA_MAX_NIF_GLOBAL_MAX_CORE_ACCESS_PER_PORT];
    SHR_FUNC_INIT_VARS(unit);

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
                    (unit, port, &params, DNX_DATA_MAX_NIF_GLOBAL_MAX_CORE_ACCESS_PER_PORT, phy_access, &nof_phys,
                     NULL));
    /*
     * get phy diagnostics
     */
    SHR_IF_ERR_EXIT(phymod_phy_diagnostics_get(phy_access, &phy_diag));
    /*
     * get OS fron osr mode
     */
    SHR_IF_ERR_EXIT(phymod_osr_mode_to_actual_os(phy_diag.osr_mode, os_int, os_remainder));
exit:
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
    dnx_algo_port_ethu_access_info_t ethu_info;
    int enable;

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

    SHR_IF_ERR_EXIT(dnx_algo_port_ethu_access_info_get(unit, port, &ethu_info));
    if (ethu_info.imb_type == imbDispatchTypeImb_cdu)
    {
        phymod_autoneg_status_t an_status;
        sal_memset(&an_status, 0, sizeof(an_status));
        SHR_IF_ERR_EXIT(portmod_port_autoneg_status_get(unit, port, &an_status));
        if (an_status.enabled && an_status.locked && link)
        {
            /*
             * the mac reset toggle is needed once AN completed
             */
            SHR_IF_ERR_EXIT(portmod_port_enable_get(unit, port, PORTMOD_PORT_ENABLE_MAC, &enable));
            if (enable)
            {
                SHR_IF_ERR_EXIT(portmod_port_enable_set(unit, port, PORTMOD_PORT_ENABLE_MAC, 0));
            }
            SHR_IF_ERR_EXIT(portmod_port_enable_set(unit, port, PORTMOD_PORT_ENABLE_MAC, 1));
        }
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
            SHR_ERR_EXIT(_SHR_E_PARAM, "No type %d in bcm_port_nif_acheduler_t\n", bcm_sch_prio);
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

/*
 * }
 */

#undef _ERR_MSG_MODULE_NAME
