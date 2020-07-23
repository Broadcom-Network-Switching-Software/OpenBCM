/*
 *         
 * 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <soc/portmod/portmod.h>
#include <soc/portmod/portmod_common.h>
#include <shared/shrextend/shrextend_debug.h>
#include <bcm_int/dnx/port/imb/imb.h>
#include <bcm_int/dnx/port/imb/imb_fabric.h>
#include <bcm_int/dnx/port/imb/imb_common.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/algo/port/algo_port_imb.h>
#include <bcm_int/dnx/algo/lane_map/algo_lane_map.h>
#include <bcm_int/dnx/init/init.h>
#include <bcm_int/dnx/fabric/fabric_if.h>
#include <bcm_int/dnx/fabric/fabric.h>
#include <soc/dnxc/dnxc_port.h>
#include <soc/dnx/dbal/dbal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_fabric.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_port.h>

#include "imb_utils.h"

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_PORT

#define DNX_IMB_FABRIC_BUS_ID   (5)

/**
 * \brief
 *   Get Fabric MDIO phy address.
 */
static int
dnx_imb_fabric_mdio_address_get(
    int unit,
    int core_index,
    uint16 *phy_addr)
{
    uint16 bus_id = 0;
    uint16 phy_id = 0;

    /*
     * All PM cores are on the same ring.
     * Phy address is per PM core.
     */
    bus_id = DNX_IMB_FABRIC_BUS_ID;
    phy_id = core_index;

    /*
     * encode for MIIM format: bits 0-4 for the "port" address bits 5-6 and 8-9 for bus bit 7 for internal/external
     * 0x80 for internal port 
     */
    *phy_addr = 0x80 | ((bus_id & 0x3) << PHY_ID_BUS_LOWER_SHIFT) | ((bus_id & 0xc) << PHY_ID_BUS_UPPER_SHIFT) | phy_id;

    return BCM_E_NONE;
}

/**
 * \brief - algorithm to get Fabric Serdes lane boundaries,
 *         including the lower boundry and the upper boundary.
 *  
 * \param [in] unit - chip unit id. 
 * \param [in] port - logical port # 
 * \param [out] lower_bound - the first Phy ID for this FSRD.
 * \param [out] upper_bound - the last Phy ID for this FSRD.
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
dnx_imb_fabric_pm_boundary_get(
    int unit,
    bcm_port_t port,
    int *lower_bound,
    int *upper_bound)
{
    int pm_index;
    int nof_links = dnx_data_fabric.blocks.nof_links_in_fsrd_get(unit);
    int fabric_port_base = dnx_data_port.general.fabric_port_base_get(unit);

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Verify
     */
    /** Verify port is valid */
    SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, port));

    pm_index = (port - fabric_port_base) / nof_links;
    *lower_bound = pm_index * nof_links;
    *upper_bound = (pm_index + 1) * nof_links - 1;
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Initialize Fabric: 
 * 1. Intialize Fabric Database. 
 * 2. Validate CL45.
 * 3. Initialize PM init info for PM below and call 
 * port_macro_add()
 * 
 * \param [in] unit - chip unit id
 * \param [in] imb_info - IMB info required for 
 *        initalization.
 * \param [out] imb_specific_info - IMB specific info to be 
 *        initialized.
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
imb_fabric_init(
    int unit,
    const imb_create_info_t * imb_info,
    imb_specific_create_info_t * imb_specific_info)
{
    int phy_offset = -1;
    int nof_fabric_pms = -1;
    int bypass_lanes_bitmap = 0;
    SHR_FUNC_INIT_VARS(unit);

    /** Initialize port macro */
    phy_offset = dnx_data_port.general.fabric_phys_offset_get(unit);
    nof_fabric_pms = dnx_data_fabric.blocks.nof_pms_get(unit);

    bypass_lanes_bitmap = dnx_data_fabric.ilkn.bypass_info_get(unit, imb_info->inst_id)->links;

    SHR_IF_ERR_EXIT(soc_dnxc_fabric_single_pm_add
                    (unit, imb_info->inst_id, nof_fabric_pms, phy_offset, bypass_lanes_bitmap, 1,
                     dnx_imb_fabric_mdio_address_get));

exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief - De-initialize the Fabric. main task is to free 
 *        dynamically allocated memory.
 * 
 * \param [in] unit - chip unit id.
 * \param [in] imb_info - IMB info IMB info required for 
 *        de-init. 
 * \param [out] imb_specific_info - IMB specific info to be 
 *        de-initialized. 
 *   
 * \return
 *   int - see _SHR_E_*
 *   
 * \remarks
 *   * Should only be called during de-init process.
 * \see
 *   * None
 */
int
imb_fabric_deinit(
    int unit,
    const imb_create_info_t * imb_info,
    imb_specific_create_info_t * imb_specific_info)
{
    int nof_fabric_pms = -1;
    SHR_FUNC_INIT_VARS(unit);

    nof_fabric_pms = dnx_data_fabric.blocks.nof_pms_get(unit);
    soc_dnxc_fabric_pms_destroy(unit, nof_fabric_pms);

    SHR_FUNC_EXIT;
}

/*
 * \brief
 *   Attach fabric port.
 *   Fabric ports can be loaded in single stage (when no flags supplied), or
 *   in 2 stages:
 *      Stage1 - until firmware load.
 *      Stage2 - after firmware loaded.
 *   
 * \param [in] unit - chip unit id.
 * \param [in] port - logical port id.
 * \param [in] flags - one of IMB_PORT_ADD_F_INIT_*
 *
 * \return
 *   int - see _SHR_E_*
 *   
 * \remarks
 *   * When loading the port in 2 stages, this function should be called twice,
 *     each time with the appropriate stage flags.
 * \see
 *   * imb_port_add
 */
int
imb_fabric_port_attach(
    int unit,
    bcm_port_t port,
    uint32 flags)
{
    dnxc_port_init_stage_t stage = dnxc_port_init_full;
    dnxc_port_fabric_init_config_t port_config;
    soc_dnxc_port_firmware_config_t fw_config;
    int pm_lower_bound = 0, pm_upper_bound = 0;
    SHR_FUNC_INIT_VARS(unit);

    if (IMB_PORT_ADD_F_INIT_PASS1_GET(flags))
    {
        /** Stage1 */
        stage = dnxc_port_init_pass1;
    }
    else if (IMB_PORT_ADD_F_INIT_PASS2_GET(flags))
    {
        /** Stage2 */
        stage = dnxc_port_init_pass2;
    }
    /** If both Stage1 & Stage2 flags are missing, we are in single stage */
    else
    {
        /** Single stage */
        stage = dnxc_port_init_full;
    }

    /*
     * Get the lane to serdes mapping. Phymod requires lane to serdes mapping.
     * If certain lane is NOT_MAPPED, fill the hole with NOT_MAPPED serdes.
     */
    SHR_IF_ERR_EXIT(dnx_imb_fabric_pm_boundary_get(unit, port, &pm_lower_bound, &pm_upper_bound));
    SHR_IF_ERR_EXIT(dnx_algo_lane_map_pm_lane_to_serdes_map_get
                    (unit, DNX_ALGO_LANE_MAP_FABRIC_SIDE, pm_lower_bound, pm_upper_bound, port_config.lane2serdes));

    fw_config.crc_check = dnx_data_port.static_add.fabric_fw_crc_check_get(unit);
    fw_config.load_verify = dnx_data_port.static_add.fabric_fw_load_verify_get(unit);
    SHR_IF_ERR_EXIT(soc_dnxc_fabric_port_probe(unit, port, stage, &fw_config, &port_config));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Remove a port from the Fabric.
 *          Power down the FSRD if it was the last active link in the
 *          FSRD block.
 * 
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
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
imb_fabric_port_detach(
    int unit,
    bcm_port_t port)
{
    SHR_FUNC_INIT_VARS(unit);

    /**
     * Remove loopback
     */
    SHR_IF_ERR_EXIT(imb_fabric_port_loopback_set(unit, port, BCM_PORT_LOOPBACK_NONE));

    /** Disable port */
    SHR_IF_ERR_EXIT(imb_fabric_port_enable_set(unit, port, 0, 0));

    /** Remove from portmod */
    SHR_IF_ERR_EXIT(portmod_port_remove(unit, port));

exit:
    SHR_FUNC_EXIT;
}

/*
 * \brief
 *   Enable port.
 *   Configure ALDWP if we are not in init sequence.
 */
int
imb_fabric_port_enable_set(
    int unit,
    bcm_port_t port,
    uint32 flags,
    int enable)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_fabric_if_port_enable_set(unit, port, enable ? 1 : 0));

    /*
     * ALDWP should be configured after init time for every speed change.
     * Avoiding ALDWP config at init time.
     */
    if (dnx_init_is_init_done_get(unit) == TRUE)
    {
        SHR_IF_ERR_EXIT(dnx_fabric_aldwp_config(unit));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * \brief
 *   Get whther port is enabled.
 */
int
imb_fabric_port_enable_get(
    int unit,
    bcm_port_t port,
    int *enable)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(portmod_port_enable_get(unit, port, 0, enable));

exit:
    SHR_FUNC_EXIT;
}

/*
 * \brief
 *   Set loopback on a port.
 *   Open the previous loopback and set the new loopback.
 */
int
imb_fabric_port_loopback_set(
    int unit,
    bcm_port_t port,
    int loopback)
{
    portmod_loopback_mode_t portmod_loopback = portmodLoopbackCount;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imb_portmod_loopback_from_bcm_loopback_get(unit, port, loopback, &portmod_loopback));
    SHR_IF_ERR_EXIT(soc_dnxc_port_loopback_set(unit, port, portmod_loopback));

exit:
    SHR_FUNC_EXIT;
}

/*
 * \brief
 *   Get the loopback of a port.
 */
int
imb_fabric_port_loopback_get(
    int unit,
    bcm_port_t port,
    int *loopback)
{
    portmod_loopback_mode_t portmod_loopback = portmodLoopbackCount;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(soc_dnxc_port_loopback_get(unit, port, &portmod_loopback));
    SHR_IF_ERR_EXIT(imb_bcm_loopback_from_portmod_loopback_get(unit, port, portmod_loopback, loopback));

exit:
    SHR_FUNC_EXIT;
}

/*
 * \brief
 *   Get whether port is 'up' or 'down'.
 */
int
imb_fabric_port_link_state_get(
    int unit,
    bcm_port_t port,
    int clear_status,
    bcm_port_link_state_t * link_state)
{
    int is_link_up = 0;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(portmod_port_link_get(unit, port, PORTMOD_INIT_F_EXTERNAL_MOST_ONLY, &is_link_up));
    link_state->status = is_link_up;

exit:
    SHR_FUNC_EXIT;
}

/** See .h file */
int
imb_fabric_port_speed_config_set(
    int unit,
    bcm_port_t port,
    const portmod_speed_config_t * speed_config)
{
    portmod_speed_config_t prev_speed_config;
    portmod_speed_config_t new_speed_config;
    int is_init_done = dnx_init_is_init_done_get(unit);
    uint32 lp_prec;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * lp precoder bit should not be changed by user through this API,
     * so is user changed it by mistake we need to disregard it and use
     * HW value.
     * Since 'speed_config' is const we need to copy it to a new struct.
     */
    sal_memcpy(&new_speed_config, speed_config, sizeof(portmod_speed_config_t));

    /*
     * Get previous speed configuration
     */
    SHR_IF_ERR_EXIT(portmod_port_speed_config_get(unit, port, &prev_speed_config));

    /*
     * lp precoder bit in phy_lane_config bitmap can be set only via bcm_port_phy_control API.
     * in resource API, leave HW value of this bit as is, don't change it no matter what is passed to the function 
     */
    lp_prec = PORTMOD_PORT_PHY_LANE_CONFIG_LP_PREC_EN_GET(prev_speed_config.lane_config);
    if (lp_prec)
    {
        PORTMOD_PORT_PHY_LANE_CONFIG_LP_PREC_EN_SET(new_speed_config.lane_config);
    }
    else
    {
        PORTMOD_PORT_PHY_LANE_CONFIG_LP_PREC_EN_CLEAR(new_speed_config.lane_config);
    }

    /*
     * if previous FEC and speed are the same as current FEC and speed - no need to change FEC and FEC-related
     * configurations 
     */
    if (is_init_done && (new_speed_config.fec == prev_speed_config.fec)
        && (new_speed_config.speed == prev_speed_config.speed))
    {
        PORTMOD_SPEED_CONFIG_F_SKIP_FEC_CONFIGURATION_SET(&new_speed_config);
    }

    /*
     * set speed in portmod
     */
    SHR_IF_ERR_EXIT(portmod_port_speed_config_set(unit, port, &new_speed_config));

exit:
    SHR_FUNC_EXIT;
}

/** See .h file */
int
imb_fabric_port_speed_config_get(
    int unit,
    bcm_port_t port,
    portmod_speed_config_t * speed_config)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(portmod_port_speed_config_get(unit, port, speed_config));

exit:
    SHR_FUNC_EXIT;
}

/** See .h file */
int
imb_fabric_port_local_fault_status_get(
    int unit,
    bcm_port_t port,
    int *local_fault)
{
    uint32 entry_handle_id = 0;
    int link = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Convert logical port to fabric link */
    SHR_IF_ERR_EXIT(dnx_algo_port_fabric_link_get(unit, port, &link));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_FAULT_LINKS, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FABRIC_LINK, link);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_FAULT_LOCAL, INST_SINGLE, (uint32 *) local_fault);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** See .h file */
int
imb_fabric_port_remote_fault_status_get(
    int unit,
    bcm_port_t port,
    int *remote_fault)
{
    uint32 entry_handle_id = 0;
    int link = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Convert logical port to fabric link */
    SHR_IF_ERR_EXIT(dnx_algo_port_fabric_link_get(unit, port, &link));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_FAULT_LINKS, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FABRIC_LINK, link);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_FAULT_REMOTE, INST_SINGLE, (uint32 *) remote_fault);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** See .h file */
int
imb_fabric_port_link_up_mac_update(
    int unit,
    bcm_port_t port,
    int link)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get recommended default values of resource members for given port with speed
 *
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] flags - not used
 * \param [out] resource - each memeber other than speed with BCM_PORT_RESOURCE_DEFAULT_REQUEST
 *                         will be filled with default value
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * bcm_port_resource_t
 *   * BCM_PORT_RESOURCE_DEFAULT_REQUEST
 */
int
imb_fabric_port_resource_default_get(
    int unit,
    bcm_gport_t port,
    uint32 flags,
    bcm_port_resource_t * resource)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(resource, _SHR_E_PARAM, "resource");

    if ((resource->flags != 0) || (resource->physical_port != 0) || (resource->encap != 0))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "port %d: flags, physical_port and encap fields of bcm_port_resource_t are not in use. They should all be initialized to 0",
                     port);
    }

    if ((resource->port != 0) && (resource->port != port))
    {
        SHR_ERR_EXIT(_SHR_E_PORT, "port %d: resource->port should be 0 or equal to port passed to the API", port);
    }

    if (resource->speed == BCM_PORT_RESOURCE_DEFAULT_REQUEST)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "port %d: there is no given default value for speed. speed is mandatory as input to this API",
                     port);
    }

    /*
     * in case user didn't configure the FEC already - give default value
     */
    if (resource->fec_type == BCM_PORT_RESOURCE_DEFAULT_REQUEST)
    {
        if (DNXC_PORT_PHY_SPEED_IS_PAM4(resource->speed))
        {
            /*
             * 15T RS FEC
             */
            resource->fec_type = bcmPortPhyFecRs545;
        }
        else
        {
            /*
             * 5T RS FEC
             */
            resource->fec_type = bcmPortPhyFecRs206;
        }
    }

    /*
     * in case user didn't configure CL72 - by default it is enabled
     */
    if (resource->link_training == BCM_PORT_RESOURCE_DEFAULT_REQUEST)
    {
        resource->link_training = 1;
    }

    /*
     * get default lane_config
     */
    if (resource->phy_lane_config == BCM_PORT_RESOURCE_DEFAULT_REQUEST)
    {
        resource->phy_lane_config = 0;
        BCM_PORT_RESOURCE_PHY_LANE_CONFIG_DFE_SET(resource->phy_lane_config);
        BCM_PORT_RESOURCE_PHY_LANE_CONFIG_LP_DFE_CLEAR(resource->phy_lane_config);
        BCM_PORT_RESOURCE_PHY_LANE_CONFIG_MEDIUM_SET(resource->phy_lane_config,
                                                     SOC_DNXC_PORT_LANE_CONFIG_MEDIA_TYPE_BACKPLANE_COPPER_TRACE);
        BCM_PORT_RESOURCE_PHY_LANE_CONFIG_UNRELIABLE_LOS_CLEAR(resource->phy_lane_config);
        BCM_PORT_RESOURCE_PHY_LANE_CONFIG_SCRAMBLING_DISABLE_CLEAR(resource->phy_lane_config);
        BCM_PORT_RESOURCE_PHY_LANE_CONFIG_CL72_POLARITY_AUTO_EN_CLEAR(resource->phy_lane_config);
        BCM_PORT_RESOURCE_PHY_LANE_CONFIG_CL72_RESTART_TIMEOUT_EN_SET(resource->phy_lane_config);
        BCM_PORT_RESOURCE_PHY_LANE_CONFIG_LP_PREC_EN_CLEAR(resource->phy_lane_config);

        /*
         * FORCE_ES, FORCE_NS must be clear for {NRZ} and for {PAM4 with link training}
         */
        BCM_PORT_RESOURCE_PHY_LANE_CONFIG_FORCE_ES_CLEAR(resource->phy_lane_config);
        BCM_PORT_RESOURCE_PHY_LANE_CONFIG_FORCE_NS_CLEAR(resource->phy_lane_config);
        if (DNXC_PORT_PHY_SPEED_IS_PAM4(resource->speed) && (resource->link_training == 0))
        {
            BCM_PORT_RESOURCE_PHY_LANE_CONFIG_FORCE_NS_SET(resource->phy_lane_config);
        }

        /*
         * internal fields that will be set in resource_set before calling portmod. Not exposed to user.
         */
        PORTMOD_PORT_PHY_LANE_CONFIG_FORCE_PAM4_CLEAR(resource->phy_lane_config);
        PORTMOD_PORT_PHY_LANE_CONFIG_FORCE_NRZ_CLEAR(resource->phy_lane_config);
        /*
         * this field should always be 0 in normal operation, can be changed only for debug.
         */
        PORTMOD_PORT_PHY_LANE_CONFIG_PAM4_CHANNEL_LOSS_SET(resource->phy_lane_config, 0);

    }

    resource->port = port;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Validate the FEC speed combinations for fabric ports
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] fec_type - fec type
 * \param [in] speed - speed
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 */
int
imb_fabric_port_fec_speed_validate(
    int unit,
    bcm_port_t port,
    bcm_port_phy_fec_t fec_type,
    int speed)
{
    int idx, is_valid;

    SHR_FUNC_INIT_VARS(unit);

    is_valid = FALSE;
    /**
     * Go over all supported NIF interfaces for the device
     * and mark the port configuration as valid only if it passes all the checks
     */
    for (idx = 0; idx < dnx_data_fabric.links.supported_interfaces_info_get(unit)->key_size[0]; idx++)
    {
        if (dnx_data_fabric.links.supported_interfaces_get(unit, idx)->is_valid)
        {
            if (speed == dnx_data_fabric.links.supported_interfaces_get(unit, idx)->speed)
            {
                /**
                 * Fec type is protected by soc_dnxc_port_resource_validate when in set API
                 * The fec type validity skip can be applied only in port resource default request API
                 * where only the speed should be validate
                 */
                if (fec_type == dnx_data_fabric.links.supported_interfaces_get(unit, idx)->fec_type
                    || fec_type == BCM_PORT_RESOURCE_DEFAULT_REQUEST)
                {
                    is_valid = TRUE;
                    break;
                }
            }
        }
    }

    if (!is_valid)
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG,
                     "port %d: The combination of speed %d and fec type %d is not supported! \n ",
                     port, speed, fec_type);
    }

exit:
    SHR_FUNC_EXIT;
}

#undef BSL_LOG_MODULE
