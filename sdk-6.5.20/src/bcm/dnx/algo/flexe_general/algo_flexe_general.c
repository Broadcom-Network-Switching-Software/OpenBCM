/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/** \file algo_flexe_general.c
 *
 *  Handle the flexe general functions.
 *    * group info
 *    * resource manager
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_PORT

/*
 * Include files.
 * {
 */
#include <sal/core/boot.h>

#include <shared/shrextend/shrextend_debug.h>
#include <shared/utilex/utilex_integer_arithmetic.h>

#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/algo/flexe_general/algo_flexe_general.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/dnx_algo_port_access.h>
#include <bcm_int/dnx/port/port_flexe.h>
#include <soc/feature.h>
#include <soc/types.h>
#include <soc/memory.h>
#include <soc/register.h>
#include <soc/dnxc/drv_dnxc_utils.h>
#include <bcm_int/dnx/algo/cal/algo_cal.h>

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
/*
 * }
 */

/**
 * \brief - Create allocation manager.
 */
static shr_error_e
dnx_algo_flexe_general_alloc_manager_create(
    int unit)
{
    dnx_algo_res_create_data_t create_data;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Create allocate manager
     */
    sal_memset(&create_data, 0, sizeof(create_data));
    /*
     * FlexE client channel allocation manager create
     */
    create_data.first_element = 0;
    create_data.nof_elements = dnx_data_nif.flexe.nof_flexe_clients_get(unit);
    sal_strncpy_s(create_data.name, "FLEXE_CLIENT_CHANNEL_ALLOC", DNX_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);
    SHR_IF_ERR_EXIT(dnx_algo_flexe_general_db.resource_alloc.client_channel.create(unit, &create_data, NULL));
    /*
     * FlexE SAR channel allocation manager create
     */
    create_data.first_element = 0;
    create_data.nof_elements = dnx_data_nif.flexe.nof_flexe_clients_get(unit);
    sal_strncpy_s(create_data.name, "FLEXE_SAR_CHANNEL_ALLOC", DNX_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);
    SHR_IF_ERR_EXIT(dnx_algo_flexe_general_db.resource_alloc.sar_channel.create(unit, &create_data, NULL));
    /*
     * FlexE MAC channel allocation manager create
     */
    create_data.first_element = 0;
    create_data.nof_elements = dnx_data_nif.flexe.nof_flexe_clients_get(unit);
    sal_strncpy_s(create_data.name, "FLEXE_MAC_CHANNEL_ALLOC", DNX_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);
    SHR_IF_ERR_EXIT(dnx_algo_flexe_general_db.resource_alloc.mac_channel.create(unit, &create_data, NULL));
    /*
     * FlexE Core port allocation manager create
     */
    sal_memset(&create_data, 0, sizeof(create_data));
    create_data.first_element = 0;
    create_data.nof_elements = dnx_data_nif.flexe.nof_pcs_get(unit);
    sal_strncpy_s(create_data.name, "FLEXE_CORE_PORT_ALLOC", DNX_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);
    SHR_IF_ERR_EXIT(dnx_algo_flexe_general_db.resource_alloc.flexe_core_port.create(unit, &create_data, NULL));
    /*
     * RMC id allocation manager create
     */
    sal_memset(&create_data, 0, sizeof(create_data));
    create_data.first_element = 0;
    create_data.nof_elements = dnx_data_nif.flexe.nof_logical_fifos_get(unit);
    sal_strncpy_s(create_data.name, "FLEXE_RMC_ID_ALLOC", DNX_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);
    SHR_IF_ERR_EXIT(dnx_algo_flexe_general_db.resource_alloc.rmc_id.create(unit, &create_data, NULL));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Get the FlexE group index from FlexE physical port.
 *
 * \param [in] unit - Unit #.
 * \param [in] physical_port - FlexE physical port.
 * \param [out] group_index - FlexE group index.
 *
 * \return
 *   see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
dnx_algo_flexe_physical_port_to_group_index_get(
    int unit,
    int physical_port,
    int *group_index)
{
    int i;
    bcm_pbmp_t group_phy_ports;

    SHR_FUNC_INIT_VARS(unit);

    *group_index = 0;
    for (i = 0; i < dnx_data_nif.flexe.nof_groups_get(unit); i++)
    {
        SHR_IF_ERR_EXIT(dnx_algo_flexe_general_db.group_info.phy_ports.get(unit, i, &group_phy_ports));
        if (_SHR_PBMP_MEMBER(group_phy_ports, physical_port))
        {
            *group_index = i;
            break;
        }
    }
    if (i == dnx_data_nif.flexe.nof_groups_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "The FlexE physical port doesn't belong to any flexe group.\n");
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_flexe_group_info_db_init(
    int unit,
    int group_index)
{
    dnx_algo_flexe_general_group_info_t flexe_group_db;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&flexe_group_db, 0, sizeof(dnx_algo_flexe_general_group_info_t));
    SHR_IF_ERR_EXIT(dnx_algo_flexe_general_db.group_info.set(unit, group_index, &flexe_group_db));
exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_flexe_group_valid_verify(
    int unit,
    int group_index,
    int support_bypass)
{
    int valid, is_bypass;
    SHR_FUNC_INIT_VARS(unit);

    /** verify group range */
    if (group_index < 0 || group_index >= dnx_data_nif.flexe.nof_groups_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "FlexE group index is out of range.\n");
    }

    SHR_IF_ERR_EXIT(dnx_algo_flexe_general_db.group_info.valid.get(unit, group_index, &valid));
    if (!valid)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "The FlexE group is expected to be valid.\n");
    }
    /*
     * Verify bypass mode
     */
    SHR_IF_ERR_EXIT(dnx_algo_flexe_group_is_bypass_get(unit, group_index, &is_bypass));
    if (!support_bypass && is_bypass)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "This API doesn't support bypass group!\r\n");
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_flexe_group_free_verify(
    int unit,
    int group_index)
{
    int valid;
    SHR_FUNC_INIT_VARS(unit);

    /** verify group range */
    if (group_index < 0 || group_index >= dnx_data_nif.flexe.nof_groups_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "FlexE group index is out of range.\n");
    }

    SHR_IF_ERR_EXIT(dnx_algo_flexe_general_db.group_info.valid.get(unit, group_index, &valid));
    if (valid)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "The FlexE group is expected to be free.\n");
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_flexe_group_port_verify(
    int unit,
    bcm_gport_t gport)
{
    int group_index;

    SHR_FUNC_INIT_VARS(unit);

    if (BCM_GPORT_IS_FLEXE_GROUP(gport))
    {
        group_index = BCM_FLEXE_GROUP_GPORT_GROUP_INDEX_GET(gport);
        SHR_RANGE_VERIFY(group_index, 0, dnx_data_nif.flexe.nof_groups_get(unit) - 1, _SHR_E_PORT,
                         "group_index is out of range.\n");
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PORT, "The gport type must be FlexE group gport.\r\n");
    }
exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_flexe_channel_verify(
    int unit,
    dnx_algo_port_info_s port_info,
    int client_channel)
{
    int is_valid;

    SHR_FUNC_INIT_VARS(unit);

    if (DNX_ALGO_PORT_TYPE_IS_FLEXE_MAC(unit, port_info, TRUE)
        && DNX_ALGO_FLEXE_MAC_CHANNEL_IS_PTP_OR_OAM(unit, client_channel))
    {
        if (DNX_ALGO_FLEXE_MAC_CHANNEL_IS_OAM(unit, client_channel) &&
            dnx_data_nif.flexe.feature_get(unit, dnx_data_nif_flexe_disable_oam_over_flexe))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "OAM over FlexE is not supported on this device.\r\n");
        }
        /*
         * Verify if the channel is occupied.
         */
        SHR_IF_ERR_EXIT(dnx_algo_port_db.flexe.mac.valid.get(unit, client_channel, &is_valid));
        if (is_valid)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "The channel ID %d has been occupied.\r\n", client_channel);
        }
    }
    else if (client_channel != 0)
    {
        /*
         * For all the other cases, the channel ID is not configurable.
         */
        SHR_ERR_EXIT(_SHR_E_PARAM, "The channel ID is expected to 0.\r\n");
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_flexe_logical_phy_id_modify_verify(
    int unit,
    bcm_port_t physical_port,
    int new_logical_phy_id)
{
    bcm_pbmp_t group_phy_ports, logical_phy_id_bmp;
    int speed, logical_phy_id = 0;
    int old_logical_phy_id = 0, group_index;
    int order, new_logical_phy_order = 0, old_logical_phy_order = 0;
    bcm_port_t logical_port;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_speed_get(unit, physical_port, 0, &speed));
    SHR_IF_ERR_EXIT(dnx_algo_flexe_logical_phy_id_range_and_speed_verify(unit, speed, new_logical_phy_id));

    SHR_IF_ERR_EXIT(dnx_algo_flexe_physical_port_to_group_index_get(unit, physical_port, &group_index));
    SHR_IF_ERR_EXIT(dnx_algo_flexe_general_db.group_info.phy_ports.get(unit, group_index, &group_phy_ports));

    /*
     * The new logical PHY ID should keep the original logical PHY orders
     */
    BCM_PBMP_CLEAR(logical_phy_id_bmp);
    _SHR_PBMP_ITER(group_phy_ports, logical_port)
    {
        /** Get logical PHY id from NIF DB */
        SHR_IF_ERR_EXIT(dnx_algo_port_flexe_logical_phy_id_get(unit, logical_port, &logical_phy_id));
        if (logical_phy_id == new_logical_phy_id)
        {
            if (logical_port == physical_port)
            {
                /** The new logical PHY ID is the same as previous one */
                SHR_EXIT();
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "The new logical PHY ID has been ocuppied by other phy ports.\r\n");
            }
        }
        _SHR_PBMP_PORT_ADD(logical_phy_id_bmp, logical_phy_id);
    }
    _SHR_PBMP_PORT_ADD(logical_phy_id_bmp, new_logical_phy_id);
    /** Get old logical phy ID from DB */
    SHR_IF_ERR_EXIT(dnx_algo_port_flexe_logical_phy_id_get(unit, physical_port, &old_logical_phy_id));
    /*
     * Get the logical PHY ID position
     */
    order = 0;
    _SHR_PBMP_ITER(logical_phy_id_bmp, logical_phy_id)
    {
        if (logical_phy_id == old_logical_phy_id)
        {
            old_logical_phy_order = order;
        }
        else if (logical_phy_id == new_logical_phy_id)
        {
            new_logical_phy_order = order;
        }
        order++;
    }
    if (utilex_abs(old_logical_phy_order - new_logical_phy_order) != 1)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "The new logical PHY ID changes the original PHY order.\r\n");
    }
exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_flexe_group_id_free_verify(
    int unit,
    int group_id)
{
    int min_group_id, max_group_id, i, tmp_group_id;

    SHR_FUNC_INIT_VARS(unit);

    min_group_id = dnx_data_nif.flexe.min_group_id_get(unit);
    max_group_id = dnx_data_nif.flexe.max_group_id_get(unit);

    SHR_RANGE_VERIFY(group_id, min_group_id, max_group_id, _SHR_E_PARAM, "group id is out of range.\n");

    for (i = 0; i < dnx_data_nif.flexe.nof_groups_get(unit); ++i)
    {
        SHR_IF_ERR_EXIT(dnx_algo_flexe_general_db.group_info.group_id.get(unit, i, &tmp_group_id));
        if (tmp_group_id == group_id)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "The FlexE group Id has been occupied.\r\n");
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_flexe_physical_port_verify(
    int unit,
    bcm_port_t physical_port,
    int support_bypass)
{
    int group_index = -1, is_bypass;
    bcm_pbmp_t logical_ports;
    dnx_algo_port_info_s port_info;

    SHR_FUNC_INIT_VARS(unit);

    /** Verify FlexE physical port is valid */
    SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, physical_port));
    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, physical_port, &port_info));

    if (!DNX_ALGO_PORT_TYPE_IS_FLEXE_PHY(unit, port_info))
    {
        SHR_ERR_EXIT(_SHR_E_PORT, "The port type should be FlexE physical port.\r\n");
    }
    /*
     * If bypass mode is not supported, verify if the phy has been binded into
     * bypass group
     */
    if (!support_bypass)
    {
        SHR_IF_ERR_EXIT(dnx_algo_flexe_groups_phy_ports_get(unit, &logical_ports));
        if (_SHR_PBMP_MEMBER(logical_ports, physical_port))
        {
            SHR_IF_ERR_EXIT(dnx_algo_flexe_physical_port_to_group_index_get(unit, physical_port, &group_index));
            SHR_IF_ERR_EXIT(dnx_algo_flexe_group_is_bypass_get(unit, group_index, &is_bypass));
            if (is_bypass)
            {
                SHR_ERR_EXIT(_SHR_E_PORT, "This API doesn't support bypass mode!\r\n");
            }
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_flexe_client_verify(
    int unit,
    bcm_port_t logical_port,
    int support_bypass)
{
    int is_bypass;
    dnx_algo_port_info_s port_info;

    SHR_FUNC_INIT_VARS(unit);

    /** Verify FlexE physical port is valid */
    SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, logical_port));
    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, logical_port, &port_info));

    if (!DNX_ALGO_PORT_TYPE_IS_FLEXE_CLIENT(unit, port_info))
    {
        SHR_ERR_EXIT(_SHR_E_PORT, "The port type should be FlexE client.\r\n");
    }
    /*
     * Check if the client is used for bypass mode
     * Currently bypass client is not supported in this API
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_flexe_client_is_bypass_get(unit, logical_port, &is_bypass));
    if (!support_bypass && is_bypass)
    {
        SHR_ERR_EXIT(_SHR_E_PORT, "Bypass FlexE client is not supported in this API.\r\n");
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_flexe_sar_client_verify(
    int unit,
    bcm_port_t logical_port)
{
    dnx_algo_port_info_s port_info;

    SHR_FUNC_INIT_VARS(unit);

    /** Verify FlexE physical port is valid */
    SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, logical_port));
    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, logical_port, &port_info));

    if (!DNX_ALGO_PORT_TYPE_IS_FLEXE_SAR(unit, port_info))
    {
        SHR_ERR_EXIT(_SHR_E_PORT, "The port type should be FlexE sar client.\r\n");
    }
exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_flexe_resource_validate(
    int unit,
    bcm_port_resource_t * resource)
{
    int client_channel, max_speed;
    uint32 supported_flags;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Verify all the irrelevant configuration should be 0
     */
    if ((resource->physical_port != 0) || (resource->lanes != 0) || (resource->encap != 0) ||
        (resource->fec_type != 0) || (resource->phy_lane_config != 0) || (resource->link_training != 0)
        || (resource->roe_compression != 0))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "physical_port, lanes, encap, fec_type, phy_lane_config, link_training and roe_compression fields of resource are not in use. They should all be initialized to 0.\n");
    }
    /*
     * Verify flags
     */
    supported_flags = BCM_PORT_RESOURCE_ASYMMETRICAL;
    SHR_MASK_VERIFY(resource->flags, supported_flags, _SHR_E_PARAM, "unexpected flags.\n");
    /*
     * Verify Rx speed is only relevant when BCM_PORT_RESOURCE_ASYMMETRICAL flag is set
     */
    if ((resource->rx_speed != 0) && !(resource->flags & BCM_PORT_RESOURCE_ASYMMETRICAL))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "rx_speed is only relevant when BCM_PORT_RESOURCE_ASYMMETRICAL flag is set.\n");
    }
    /*
     * If BCM_PORT_RESOURCE_ASYMMETRICAL is set, the rx and tx speed should not be the same
     */
    if ((resource->flags & BCM_PORT_RESOURCE_ASYMMETRICAL) && (resource->rx_speed == resource->speed))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "rx_speed should not be equal with tx_speed when BCM_PORT_RESOURCE_ASYMMETRICAL flag is set.\n");
    }
    /*
     * Verify speed
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_channel_get(unit, resource->port, &client_channel));

    if (DNX_ALGO_FLEXE_MAC_CHANNEL_IS_PTP_OR_OAM(unit, client_channel))
    {
        max_speed =
            (DNX_ALGO_FLEXE_MAC_CHANNEL_IS_PTP(unit, client_channel)) ? dnx_data_nif.
            flexe.ptp_channel_max_bandwidth_get(unit) : dnx_data_nif.flexe.oam_channel_max_bandwidth_get(unit);
        if ((resource->speed <= 0) || (resource->speed > max_speed))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Max bandwidth for this client should be %d.\r\n", max_speed);
        }
    }
    else
    {
        max_speed = dnx_data_nif.flexe.max_flexe_core_speed_get(unit);
        if ((resource->speed < 0) || (resource->speed > max_speed))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Max bandwidth for this client should be %d.\r\n", max_speed);
        }
        if (resource->speed % dnx_data_nif.flexe.client_speed_granularity_get(unit) != 0)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "The client speed granularity should be %d.\r\n",
                         dnx_data_nif.flexe.client_speed_granularity_get(unit));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_flexe_phy_resource_validate(
    int unit,
    bcm_port_resource_t * resource)
{
    int i, nof_locations;
    const int *flexe_core_ports_array = NULL;

    SHR_FUNC_INIT_VARS(unit);

    /** Verify speed */
    for (i = 0; i < dnx_data_nif.flexe.nof_phy_speeds_get(unit); i++)
    {
        if (resource->speed == dnx_data_nif.flexe.phy_info_get(unit, i)->speed)
        {
            flexe_core_ports_array = dnx_data_nif.flexe.phy_info_get(unit, i)->flexe_core_port;
            break;
        }
    }
    if (i == dnx_data_nif.flexe.nof_phy_speeds_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "The FlexE physical port speed(%d) is not supported.\r\n", resource->speed);
    }

    /** Verify base flexe instance */
    if (resource->base_flexe_instance != -1)
    {
        nof_locations = dnx_data_nif.flexe.max_flexe_core_speed_get(unit) / resource->speed;
        for (i = 0; i < nof_locations; i++)
        {
            /*
             * Coverity:
             * If the flexe_core_ports_array is NULL, the codes should return in
             * the previous line.
             */
             /* coverity[var_deref_op : FALSE]  */
            if (resource->base_flexe_instance == flexe_core_ports_array[i])
            {
                break;
            }
        }
        if (i == nof_locations)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "The base flexe instance ID %d is not supported for speed %d.\r\n",
                         resource->base_flexe_instance, resource->speed);
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get FlexE logical PHY ID range according to
 *    the PHY port speed.
 *
 * \param [in] unit - chip unit id
 * \param [in] speed - speed for physical port
 * \param [out] min_logical_phy_id - Min logical PHY ID
 * \param [out] max_logical_phy_id - Max logical PHY ID
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
dnx_algo_flexe_logical_phy_id_range_get(
    int unit,
    int speed,
    int *min_logical_phy_id,
    int *max_logical_phy_id)
{
    int i;

    SHR_FUNC_INIT_VARS(unit);

    *min_logical_phy_id = 0;
    *max_logical_phy_id = 0;
    for (i = 0; i < dnx_data_nif.flexe.nof_phy_speeds_get(unit); i++)
    {
        if (speed == dnx_data_nif.flexe.phy_info_get(unit, i)->speed)
        {
            *min_logical_phy_id = dnx_data_nif.flexe.phy_info_get(unit, i)->min_logical_phy_id;
            *max_logical_phy_id = dnx_data_nif.flexe.phy_info_get(unit, i)->max_logical_phy_id;
            break;
        }
    }
    /** Verify speed */
    if (i == dnx_data_nif.flexe.nof_phy_speeds_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "The FlexE physical port speed is not supported.\r\n");
    }
exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_flexe_logical_phy_id_range_and_speed_verify(
    int unit,
    int speed,
    int logical_phy_id)
{
    int min_logical_phy_id = 0, max_logical_phy_id = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_flexe_logical_phy_id_range_get(unit, speed, &min_logical_phy_id, &max_logical_phy_id));
    /** Verify logical PHY ID range */
    SHR_RANGE_VERIFY(logical_phy_id, min_logical_phy_id, max_logical_phy_id, _SHR_E_PARAM,
                     "logical phy id is out of range.\n");
exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_flexe_group_valid_set(
    int unit,
    int group_index,
    int valid)
{
    SHR_FUNC_INIT_VARS(unit);

    /** verify group range */
    if (group_index < 0 || group_index >= dnx_data_nif.flexe.nof_groups_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "FlexE group index is out of range.\n");
    }
    SHR_IF_ERR_EXIT(dnx_algo_flexe_general_db.group_info.valid.set(unit, group_index, valid));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_flexe_group_is_bypass_set(
    int unit,
    int group_index,
    int is_bypass)
{
    SHR_FUNC_INIT_VARS(unit);

    /** verify group range */
    if (group_index < 0 || group_index >= dnx_data_nif.flexe.nof_groups_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "FlexE group index is out of range.\n");
    }
    SHR_IF_ERR_EXIT(dnx_algo_flexe_general_db.group_info.is_bypass.set(unit, group_index, is_bypass));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_flexe_group_is_bypass_get(
    int unit,
    int group_index,
    int *is_bypass)
{
    SHR_FUNC_INIT_VARS(unit);

    /** verify group range */
    if (group_index < 0 || group_index >= dnx_data_nif.flexe.nof_groups_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "FlexE group index is out of range.\n");
    }
    SHR_IF_ERR_EXIT(dnx_algo_flexe_general_db.group_info.is_bypass.get(unit, group_index, is_bypass));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_flexe_group_phy_ports_set(
    int unit,
    int group_index,
    bcm_pbmp_t * phy_ports)
{
    SHR_FUNC_INIT_VARS(unit);

    /** verify group range */
    if (group_index < 0 || group_index >= dnx_data_nif.flexe.nof_groups_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "FlexE group index is out of range.\n");
    }
    SHR_IF_ERR_EXIT(dnx_algo_flexe_general_db.group_info.phy_ports.set(unit, group_index, *phy_ports));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_flexe_group_phy_ports_get(
    int unit,
    int group_index,
    bcm_pbmp_t * phy_ports)
{
    SHR_FUNC_INIT_VARS(unit);

    /** verify group range */
    if (group_index < 0 || group_index >= dnx_data_nif.flexe.nof_groups_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "FlexE group index is out of range.\n");
    }
    _SHR_PBMP_CLEAR(*phy_ports);
    SHR_IF_ERR_EXIT(dnx_algo_flexe_general_db.group_info.phy_ports.get(unit, group_index, phy_ports));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_flexe_group_index_to_id_get(
    int unit,
    int group_index,
    int *group_id)
{
    SHR_FUNC_INIT_VARS(unit);

    /** verify group range */
    if (group_index < 0 || group_index >= dnx_data_nif.flexe.nof_groups_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "FlexE group index is out of range.\n");
    }
    SHR_IF_ERR_EXIT(dnx_algo_flexe_general_db.group_info.group_id.get(unit, group_index, group_id));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_flexe_group_index_to_id_set(
    int unit,
    int group_index,
    int group_id)
{
    SHR_FUNC_INIT_VARS(unit);

    /** verify group range */
    if (group_index < 0 || group_index >= dnx_data_nif.flexe.nof_groups_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "FlexE group index is out of range.\n");
    }
    SHR_IF_ERR_EXIT(dnx_algo_flexe_general_db.group_info.group_id.set(unit, group_index, group_id));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_flexe_groups_phy_ports_get(
    int unit,
    bcm_pbmp_t * phy_ports)
{
    int i, valid;
    bcm_pbmp_t group_phy_ports;

    SHR_FUNC_INIT_VARS(unit);

    BCM_PBMP_CLEAR(*phy_ports);
    for (i = 0; i < dnx_data_nif.flexe.nof_groups_get(unit); i++)
    {
        SHR_IF_ERR_EXIT(dnx_algo_flexe_general_db.group_info.valid.get(unit, i, &valid));
        if (valid)
        {
            SHR_IF_ERR_EXIT(dnx_algo_flexe_general_db.group_info.phy_ports.get(unit, i, &group_phy_ports));
            _SHR_PBMP_OR(*phy_ports, group_phy_ports);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_flexe_core_port_alloc(
    int unit,
    int speed,
    int is_with_id,
    int *flexe_core_port)
{
    int nof_elements, nof_locations, i, index;
    const int *flexe_core_ports_array = NULL;
    uint8 is_allocated;

    SHR_FUNC_INIT_VARS(unit);

    nof_elements = speed / dnx_data_nif.flexe.phy_speed_granularity_get(unit);
    nof_locations = dnx_data_nif.flexe.max_flexe_core_speed_get(unit) / speed;
    /*
     * Get flexe core ports array from DNX DATA
     */
    for (i = 0; i < dnx_data_nif.flexe.nof_phy_speeds_get(unit); i++)
    {
        if (speed == dnx_data_nif.flexe.phy_info_get(unit, i)->speed)
        {
            flexe_core_ports_array = dnx_data_nif.flexe.phy_info_get(unit, i)->flexe_core_port;
            break;
        }
    }
    if (i == dnx_data_nif.flexe.nof_phy_speeds_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "The speed is not supported.\r\n");
    }
    for (i = 0; i < nof_locations; i++)
    {
        if (is_with_id && (*flexe_core_port != flexe_core_ports_array[i]))
        {
            /*
             * If "is_with_id" is set, check if the specified flexe_core_port is legal.
             */
            continue;
        }
        /*
         * Coverity:
         * If the flexe_core_ports_array is NULL, the codes should return in
         * the previous line.
         */
         /* coverity[var_deref_op : FALSE]  */
        *flexe_core_port = flexe_core_ports_array[i];
        for (index = 0; index < nof_elements; ++index)
        {
            SHR_IF_ERR_EXIT(dnx_algo_flexe_general_db.resource_alloc.
                            flexe_core_port.is_allocated(unit, *flexe_core_port + index, &is_allocated));
            if (is_allocated)
            {
                break;
            }
        }
        if (!is_allocated)
        {
            SHR_IF_ERR_EXIT(dnx_algo_flexe_general_db.resource_alloc.
                            flexe_core_port.allocate_several(unit, DNX_ALGO_RES_ALLOCATE_WITH_ID, nof_elements, NULL,
                                                             flexe_core_port));
            break;
        }
    }
    if (i == nof_locations)
    {
        SHR_ERR_EXIT(_SHR_E_RESOURCE, "FlexE core port allocation failed.\r\n");
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_flexe_core_port_free(
    int unit,
    int speed,
    int flexe_core_port)
{
    int nof_elements;

    SHR_FUNC_INIT_VARS(unit);

    nof_elements = speed / dnx_data_nif.flexe.phy_speed_granularity_get(unit);
    SHR_IF_ERR_EXIT(dnx_algo_flexe_general_db.resource_alloc.
                    flexe_core_port.free_several(unit, nof_elements, flexe_core_port));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_flexe_group_phy_speed_get(
    int unit,
    int group_index,
    int *phy_speed)
{
    bcm_pbmp_t phy_ports;
    bcm_port_t logical_port;

    SHR_FUNC_INIT_VARS(unit);

    /** verify group range */
    if (group_index < 0 || group_index >= dnx_data_nif.flexe.nof_groups_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "FlexE group index is out of range.\n");
    }

    *phy_speed = 0;
    SHR_IF_ERR_EXIT(dnx_algo_flexe_general_db.group_info.phy_ports.get(unit, group_index, &phy_ports));
    _SHR_PBMP_FIRST(phy_ports, logical_port);
    SHR_IF_ERR_EXIT(dnx_algo_port_speed_get(unit, logical_port, 0, phy_speed));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_flexe_group_speed_get(
    int unit,
    int group_index,
    int *group_speed)
{
    bcm_pbmp_t phy_ports;
    bcm_port_t logical_port;
    int nof_phy_ports, phy_speed;

    SHR_FUNC_INIT_VARS(unit);

    /** verify group range */
    if (group_index < 0 || group_index >= dnx_data_nif.flexe.nof_groups_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "FlexE group index is out of range.\n");
    }

    *group_speed = 0;
    SHR_IF_ERR_EXIT(dnx_algo_flexe_general_db.group_info.phy_ports.get(unit, group_index, &phy_ports));
    _SHR_PBMP_COUNT(phy_ports, nof_phy_ports);
    /*
     * Count the group speed only if there is valid flexe phy ports in the group
     */
    if (nof_phy_ports != 0)
    {
        _SHR_PBMP_FIRST(phy_ports, logical_port);
        SHR_IF_ERR_EXIT(dnx_algo_port_speed_get(unit, logical_port, 0, &phy_speed));
        *group_speed = phy_speed * nof_phy_ports;
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_flexe_groups_speed_get(
    int unit,
    int *groups_speed)
{
    int i, valid, group_speed;

    SHR_FUNC_INIT_VARS(unit);

    *groups_speed = 0;
    for (i = 0; i < dnx_data_nif.flexe.nof_groups_get(unit); i++)
    {
        SHR_IF_ERR_EXIT(dnx_algo_flexe_general_db.group_info.valid.get(unit, i, &valid));
        if (valid)
        {
            SHR_IF_ERR_EXIT(dnx_algo_flexe_group_speed_get(unit, i, &group_speed));
            *groups_speed += group_speed;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Allocate memory link list DB.
 */
static shr_error_e
dnx_algo_flexe_general_mem_link_list_db_alloc(
    int unit)
{
    int nof_fifo_entries;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Allocate SB RX FIFO Link List DB
     */
    nof_fifo_entries = dnx_data_nif.flexe.nof_sb_rx_fifos_get(unit);
    SHR_IF_ERR_EXIT(dnx_algo_flexe_general_db.fifo_link_list.alloc(unit, DNX_ALGO_FLEXE_FIFO_SB_RX, nof_fifo_entries));

    /*
     * Allocate TMC FIFO Link List DB
     */
    nof_fifo_entries = dnx_data_nif.flexe.nof_clients_get(unit);
    SHR_IF_ERR_EXIT(dnx_algo_flexe_general_db.fifo_link_list.alloc(unit, DNX_ALGO_FLEXE_FIFO_TMC, nof_fifo_entries));

    /*
     * Allocate RMC FIFO Link List DB
     */
    nof_fifo_entries = dnx_data_nif.flexe.nof_logical_fifos_get(unit);
    SHR_IF_ERR_EXIT(dnx_algo_flexe_general_db.fifo_link_list.alloc(unit, DNX_ALGO_FLEXE_FIFO_RMC, nof_fifo_entries));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Allocate memory link list DB.
 */
static shr_error_e
dnx_algo_flexe_general_mem_link_list_db_init(
    int unit)
{
    int entry_idx;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Init SB RX FIFO Link List DB
     */
    for (entry_idx = 0; entry_idx < dnx_data_nif.flexe.nof_sb_rx_fifos_get(unit); ++entry_idx)
    {
        SHR_IF_ERR_EXIT(dnx_algo_flexe_general_db.fifo_link_list.set(unit, DNX_ALGO_FLEXE_FIFO_SB_RX, entry_idx, -1));
    }

    /*
     * Init TMC FIFO Link List DB
     */
    for (entry_idx = 0; entry_idx < dnx_data_nif.flexe.nof_clients_get(unit); ++entry_idx)
    {
        SHR_IF_ERR_EXIT(dnx_algo_flexe_general_db.fifo_link_list.set(unit, DNX_ALGO_FLEXE_FIFO_TMC, entry_idx, -1));
    }

    /*
     * Init RMC FIFO Link List DB
     */
    for (entry_idx = 0; entry_idx < dnx_data_nif.flexe.nof_logical_fifos_get(unit); ++entry_idx)
    {
        SHR_IF_ERR_EXIT(dnx_algo_flexe_general_db.fifo_link_list.set(unit, DNX_ALGO_FLEXE_FIFO_RMC, entry_idx, -1));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_flexe_general_init(
    int unit)
{
    int nof_groups;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Create SW State instance
     */
    SHR_IF_ERR_EXIT(dnx_algo_flexe_general_db.init(unit));

    /*
     * Allocate SW STATE DBs
     */
    nof_groups = dnx_data_nif.flexe.nof_groups_get(unit);
    SHR_IF_ERR_EXIT(dnx_algo_flexe_general_db.group_info.alloc(unit, nof_groups));

    /** Alloc Manager create */
    SHR_IF_ERR_EXIT(dnx_algo_flexe_general_alloc_manager_create(unit));

    /** Allocate memory Link List DB */
    SHR_IF_ERR_EXIT(dnx_algo_flexe_general_mem_link_list_db_alloc(unit));

    /** Init memory Link List DB */
    SHR_IF_ERR_EXIT(dnx_algo_flexe_general_mem_link_list_db_init(unit));

    /** Create Tiny MAC access lock */
    SHR_IF_ERR_EXIT(dnx_algo_flexe_general_db.tiny_mac_access_mutex.create(unit));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get the link list fifo size for different fifos.
 */
static shr_error_e
dnx_algo_flexe_fifo_link_list_size_get(
    int unit,
    dnx_algo_flexe_link_list_fifo_type_e type,
    int *nof_entries)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get the fifo size for different fifos
     */
    if (type == DNX_ALGO_FLEXE_FIFO_SB_RX)
    {
        *nof_entries = dnx_data_nif.flexe.nof_sb_rx_fifos_get(unit);
    }
    else if (type == DNX_ALGO_FLEXE_FIFO_TMC)
    {
        *nof_entries = dnx_data_nif.flexe.nof_clients_get(unit);
    }
    else if (type == DNX_ALGO_FLEXE_FIFO_RMC)
    {
        *nof_entries = dnx_data_nif.flexe.nof_logical_fifos_get(unit);
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported fifo type %d.\n", type);
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_flexe_fifo_link_list_entries_required_get(
    int unit,
    dnx_algo_flexe_link_list_fifo_type_e type,
    int speed,
    int *nof_entries)
{
    SHR_FUNC_INIT_VARS(unit);

    if (speed == 0)
    {
        *nof_entries = 0;
    }
    else if (speed < dnx_data_nif.flexe.client_speed_granularity_get(unit))
    {
        if (type == DNX_ALGO_FLEXE_FIFO_SB_RX)
        {
            *nof_entries = dnx_data_nif.flexe.ptp_oam_fifo_entries_in_sb_rx_get(unit);
        }
        else
        {
            *nof_entries = 1;
        }
    }
    else
    {
        if (dnx_data_nif.flexe.feature_get(unit, dnx_data_nif_flexe_double_slots_for_small_client) &&
            ((type == DNX_ALGO_FLEXE_FIFO_SB_RX) && ((speed == 5000) || (speed == 10000))))
        {
            *nof_entries = 2 * speed / dnx_data_nif.flexe.client_speed_granularity_get(unit);
        }
        else if (dnx_data_nif.flexe.feature_get(unit, dnx_data_nif_flexe_double_slots_for_all_client)
                 && (type == DNX_ALGO_FLEXE_FIFO_SB_RX))
        {
            *nof_entries = 2 * speed / dnx_data_nif.flexe.client_speed_granularity_get(unit);
        }
        else
        {
            *nof_entries = speed / dnx_data_nif.flexe.client_speed_granularity_get(unit);
        }
    }
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_flexe_fifo_link_list_alloc(
    int unit,
    int fifo_index,
    int speed,
    dnx_algo_flexe_link_list_fifo_type_e type,
    bcm_pbmp_t * fifo_entries)
{
    int entry_idx, fifo_entries_allocated = 0;
    int nof_fifo_entries_required = 0, total_fifo_entries;
    int fifo_index_tmp;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Allocate the free entries for the port
     */
    _SHR_PBMP_CLEAR(*fifo_entries);

    SHR_IF_ERR_EXIT(dnx_algo_flexe_fifo_link_list_entries_required_get(unit, type, speed, &nof_fifo_entries_required));

    if (nof_fifo_entries_required != 0)
    {
        SHR_IF_ERR_EXIT(dnx_algo_flexe_fifo_link_list_size_get(unit, type, &total_fifo_entries));

        for (entry_idx = 0; entry_idx < total_fifo_entries; ++entry_idx)
        {
            SHR_IF_ERR_EXIT(dnx_algo_flexe_general_db.fifo_link_list.get(unit, type, entry_idx, &fifo_index_tmp));
            if (fifo_index_tmp == -1)
            {
                /*
                 * The entry is free. Mark it is allocated.
                 */
                SHR_IF_ERR_EXIT(dnx_algo_flexe_general_db.fifo_link_list.set(unit, type, entry_idx, fifo_index));
                _SHR_PBMP_PORT_ADD(*fifo_entries, entry_idx);
                ++fifo_entries_allocated;
            }
            if (fifo_entries_allocated == nof_fifo_entries_required)
            {
                break;
            }
        }
        if (fifo_entries_allocated < nof_fifo_entries_required)
        {
            SHR_ERR_EXIT(_SHR_E_RESOURCE, "Fifo memory is out of resource for FIFO type %d.\n", type);
        }
    }
exit:
    if (SHR_FAILURE(_func_rv) && (fifo_entries_allocated != 0))
    {
        /*
         * Recover the DB in case of failure.
         */
        _SHR_PBMP_ITER(*fifo_entries, entry_idx)
        {
            SHR_IF_ERR_RETURN(dnx_algo_flexe_general_db.fifo_link_list.set(unit, type, entry_idx, -1));
        }
    }
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_flexe_fifo_link_list_get(
    int unit,
    int fifo_index,
    dnx_algo_flexe_link_list_fifo_type_e type,
    bcm_pbmp_t * fifo_entries)
{
    int entry_idx, total_fifo_entries;
    int fifo_index_tmp;

    SHR_FUNC_INIT_VARS(unit);

    _SHR_PBMP_CLEAR(*fifo_entries);
    SHR_IF_ERR_EXIT(dnx_algo_flexe_fifo_link_list_size_get(unit, type, &total_fifo_entries));
    /*
     * Find the entry that occupied by the port.
     */
    for (entry_idx = 0; entry_idx < total_fifo_entries; ++entry_idx)
    {
        SHR_IF_ERR_EXIT(dnx_algo_flexe_general_db.fifo_link_list.get(unit, type, entry_idx, &fifo_index_tmp));
        if (fifo_index_tmp == fifo_index)
        {
            _SHR_PBMP_PORT_ADD(*fifo_entries, entry_idx);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_flexe_fifo_link_list_free(
    int unit,
    int fifo_index,
    dnx_algo_flexe_link_list_fifo_type_e type,
    bcm_pbmp_t * fifo_entries)
{
    int entry_idx;

    SHR_FUNC_INIT_VARS(unit);

    _SHR_PBMP_CLEAR(*fifo_entries);
    SHR_IF_ERR_EXIT(dnx_algo_flexe_fifo_link_list_get(unit, fifo_index, type, fifo_entries));
    _SHR_PBMP_ITER(*fifo_entries, entry_idx)
    {
        SHR_IF_ERR_EXIT(dnx_algo_flexe_general_db.fifo_link_list.set(unit, type, entry_idx, -1));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_flexe_rmc_id_alloc(
    int unit,
    int *rmc_id)
{
    int alloc_flags = 0;

    SHR_FUNC_INIT_VARS(unit);

    if (*rmc_id != -1)
    {
        alloc_flags = DNX_ALGO_RES_ALLOCATE_WITH_ID;
    }
    SHR_IF_ERR_EXIT(dnx_algo_flexe_general_db.resource_alloc.rmc_id.allocate_single(unit, alloc_flags, NULL, rmc_id));
exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_flexe_rmc_id_free(
    int unit,
    int rmc_id)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_flexe_general_db.resource_alloc.rmc_id.free_single(unit, rmc_id));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_flexe_client_channel_alloc(
    int unit,
    dnx_algo_port_info_s port_info,
    int *channel)
{
    int alloc_flags = 0;

    SHR_FUNC_INIT_VARS(unit);

    *channel = 0;
    if (DNX_ALGO_PORT_TYPE_IS_FLEXE_CLIENT(unit, port_info))
    {
        SHR_IF_ERR_EXIT(dnx_algo_flexe_general_db.resource_alloc.
                        client_channel.allocate_single(unit, alloc_flags, NULL, channel));
    }
    else if (DNX_ALGO_PORT_TYPE_IS_FLEXE_MAC(unit, port_info, TRUE))
    {
        SHR_IF_ERR_EXIT(dnx_algo_flexe_general_db.resource_alloc.
                        mac_channel.allocate_single(unit, alloc_flags, NULL, channel));
    }
    else if (DNX_ALGO_PORT_TYPE_IS_FLEXE_SAR(unit, port_info))
    {
        SHR_IF_ERR_EXIT(dnx_algo_flexe_general_db.resource_alloc.
                        sar_channel.allocate_single(unit, alloc_flags, NULL, channel));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported port type info.\n");
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_flexe_client_channel_free(
    int unit,
    dnx_algo_port_info_s port_info,
    int channel)
{
    SHR_FUNC_INIT_VARS(unit);

    if (DNX_ALGO_PORT_TYPE_IS_FLEXE_CLIENT(unit, port_info))
    {
        SHR_IF_ERR_EXIT(dnx_algo_flexe_general_db.resource_alloc.client_channel.free_single(unit, channel));
    }
    else if (DNX_ALGO_PORT_TYPE_IS_FLEXE_MAC(unit, port_info, TRUE))
    {
        SHR_IF_ERR_EXIT(dnx_algo_flexe_general_db.resource_alloc.mac_channel.free_single(unit, channel));
    }
    else if (DNX_ALGO_PORT_TYPE_IS_FLEXE_SAR(unit, port_info))
    {
        SHR_IF_ERR_EXIT(dnx_algo_flexe_general_db.resource_alloc.sar_channel.free_single(unit, channel));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported port info.\n");
    }
exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_flexe_channel_to_client_id_map_verify(
    int unit,
    int group_index,
    bcm_port_flexe_group_cal_t calendar_id,
    int nof_slots,
    int *calendar_slots)
{
    int slot_index, client_channel;
    bcm_port_t logical_port;
    int client_id_map[DNX_DATA_MAX_NIF_FLEXE_NOF_FLEXE_CLIENTS] = { 0 };

    SHR_FUNC_INIT_VARS(unit);

    for (slot_index = 0; slot_index < nof_slots; ++slot_index)
    {
        SHR_IF_ERR_EXIT(dnx_algo_flexe_general_db.group_info.
                        mux_cal_slots.get(unit, group_index, calendar_id, slot_index, &logical_port));
        if (logical_port != 0)
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_channel_get(unit, logical_port, &client_channel));
            if (client_id_map[client_channel] == 0)
            {
                client_id_map[client_channel] = calendar_slots[slot_index];
            }
            else if (client_id_map[client_channel] != calendar_slots[slot_index])
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "The client ID configuration is inconsistent.\n");
            }
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_flexe_group_cal_slot_set(
    int unit,
    int group_index,
    uint32 flags,
    bcm_port_flexe_group_cal_t calendar_id,
    int slot_index,
    bcm_port_t logical_port)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Verify the argument used for accessing SW DB
     */
    if (group_index < 0 || group_index >= dnx_data_nif.flexe.nof_groups_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "FlexE group index is out of range.\n");
    }
    if (calendar_id < 0 || calendar_id >= bcmPortFlexeGroupCalCount)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "calendar_id is out range.\n");
    }
    if (slot_index < 0 || slot_index >= dnx_data_nif.flexe.max_nof_slots_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "slot index is out range.\n");
    }

    if (flags & BCM_PORT_FLEXE_RX)
    {
        SHR_IF_ERR_EXIT(dnx_algo_flexe_general_db.group_info.
                        demux_cal_slots.set(unit, group_index, calendar_id, slot_index, logical_port));
    }
    if (flags & BCM_PORT_FLEXE_TX)
    {
        SHR_IF_ERR_EXIT(dnx_algo_flexe_general_db.group_info.
                        mux_cal_slots.set(unit, group_index, calendar_id, slot_index, logical_port));
    }
exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_flexe_group_cal_slot_get(
    int unit,
    int group_index,
    uint32 flags,
    bcm_port_flexe_group_cal_t calendar_id,
    int slot_index,
    bcm_port_t * logical_port)
{
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Verify the argument used for accessing SW DB
     */
    if (group_index < 0 || group_index >= dnx_data_nif.flexe.nof_groups_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "FlexE group index is out of range.\n");
    }
    if (calendar_id < 0 || calendar_id >= bcmPortFlexeGroupCalCount)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "calendar_id is out range.\n");
    }
    if (slot_index < 0 || slot_index >= dnx_data_nif.flexe.max_nof_slots_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "slot index is out range.\n");
    }

    if (flags & BCM_PORT_FLEXE_RX)
    {
        SHR_IF_ERR_EXIT(dnx_algo_flexe_general_db.group_info.
                        demux_cal_slots.get(unit, group_index, calendar_id, slot_index, logical_port));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_algo_flexe_general_db.group_info.
                        mux_cal_slots.get(unit, group_index, calendar_id, slot_index, logical_port));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_flexe_core_ports_get(
    int unit,
    int group_index,
    bcm_pbmp_t * flexe_core_ports)
{
    int nof_pcs, count = 0;
    bcm_pbmp_t phy_ports;
    bcm_port_t logical_port, flexe_core_port;

    SHR_FUNC_INIT_VARS(unit);

    _SHR_PBMP_CLEAR(*flexe_core_ports);
    SHR_IF_ERR_EXIT(dnx_algo_flexe_group_phy_ports_get(unit, group_index, &phy_ports));

    _SHR_PBMP_COUNT(phy_ports, nof_pcs);
    _SHR_PBMP_ITER(phy_ports, logical_port)
    {
        /*
         * For half capacity, need to skip the first half phy ports.
         */
        if (dnx_data_nif.flexe.feature_get(unit, dnx_data_nif_flexe_half_capability) && (count < (nof_pcs / 2)))
        {
            count++;
            continue;
        }
        SHR_IF_ERR_EXIT(dnx_algo_port_flexe_core_port_get(unit, logical_port, &flexe_core_port));
        _SHR_PBMP_PORT_ADD(*flexe_core_ports, flexe_core_port);
    }
exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_flexe_phy_ports_sort_get(
    int unit,
    int group_index,
    bcm_port_t * phy_ports_arr,
    int *nof_phy_ports)
{
    int *logical_phy_id_port_map = NULL, min_phy_id, max_phy_id, phy_speed;
    int logical_phy_id;
    bcm_pbmp_t phy_ports, logical_phy_id_bmp;
    bcm_port_t logical_port;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Get the PHY speed and Max logical PHY ID
     */
    SHR_IF_ERR_EXIT(dnx_algo_flexe_group_phy_speed_get(unit, group_index, &phy_speed));
    SHR_IF_ERR_EXIT(dnx_algo_flexe_logical_phy_id_range_get(unit, phy_speed, &min_phy_id, &max_phy_id));
    SHR_ALLOC_SET_ZERO(logical_phy_id_port_map, (max_phy_id + 1) * sizeof(int), "logical_phy_id_port_map", "%s%s%s\r\n",
                       EMPTY, EMPTY, EMPTY);
    /*
     * Get all the phy ports in the group
     */
    SHR_IF_ERR_EXIT(dnx_algo_flexe_group_phy_ports_get(unit, group_index, &phy_ports));
    /*
     * Get logical_phy_id to flexe core port mapping
     */
    _SHR_PBMP_CLEAR(logical_phy_id_bmp);
    _SHR_PBMP_ITER(phy_ports, logical_port)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_flexe_logical_phy_id_get(unit, logical_port, &logical_phy_id));
        logical_phy_id_port_map[logical_phy_id] = logical_port;
        _SHR_PBMP_PORT_ADD(logical_phy_id_bmp, logical_phy_id);
    }
    /*
     * Sort the FlexE core port by logical PHY ID.
     */
    *nof_phy_ports = 0;
    _SHR_PBMP_ITER(logical_phy_id_bmp, logical_phy_id)
    {
        phy_ports_arr[*nof_phy_ports] = logical_phy_id_port_map[logical_phy_id];
        (*nof_phy_ports)++;
    }
exit:
    SHR_FREE(logical_phy_id_port_map);
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_flexe_client_time_slots_db_build(
    int unit,
    int group_index,
    int nof_slots,
    int *calendar_slots,
    int nof_clients,
    dnx_algo_flexe_client_ts_config_t * ts_config_db)
{
    int phy_ports_arr[DNX_DATA_MAX_NIF_FLEXE_NOF_PCS] = { 0 };
    int nof_ports, i, slot_index, nof_slots_per_port, phy_speed;
    int client_channel, flexe_core_port, client_port_index;
    int inner_slot_index;
    bcm_pbmp_t accumulated_clients;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Get FlexE core ports sorted by logical PHY ID
     */
    SHR_IF_ERR_EXIT(dnx_algo_flexe_phy_ports_sort_get(unit, group_index, phy_ports_arr, &nof_ports));
    /*
     * Get max nof slots per port
     */
    SHR_IF_ERR_EXIT(dnx_algo_flexe_group_phy_speed_get(unit, group_index, &phy_speed));
    nof_slots_per_port = phy_speed / dnx_data_nif.flexe.client_speed_granularity_get(unit);
    /*
     * Clear the DB
     */
    sal_memset(ts_config_db, 0, nof_clients * sizeof(dnx_algo_flexe_client_ts_config_t));

    for (i = 0; i < nof_ports; ++i)
    {
        _SHR_PBMP_CLEAR(accumulated_clients);
        SHR_IF_ERR_EXIT(dnx_algo_port_flexe_core_port_get(unit, phy_ports_arr[i], &flexe_core_port));

        inner_slot_index = 0;
        for (slot_index = nof_slots_per_port * i; slot_index < (nof_slots_per_port * i + nof_slots_per_port);
             ++slot_index)
        {
            if (calendar_slots[slot_index] != 0)
            {
                SHR_IF_ERR_EXIT(dnx_algo_port_channel_get(unit, calendar_slots[slot_index], &client_channel));
                /*
                 * Get current port index for the client
                 */
                client_port_index = ts_config_db[client_channel].nof_flexe_core_ports;

                ts_config_db[client_channel].flexe_core_port_array[client_port_index] = flexe_core_port;
                SHR_BITSET(ts_config_db[client_channel].ts_mask,
                           (inner_slot_index + client_port_index * nof_slots_per_port));

                _SHR_PBMP_PORT_ADD(accumulated_clients, client_channel);
            }
            ++inner_slot_index;
        }
        /*
         * Only update nof_flexe_core_ports when the client is recorded.
         */
        _SHR_PBMP_ITER(accumulated_clients, client_channel)
        {
            ++(ts_config_db[client_channel].nof_flexe_core_ports);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get the slot index range for physical port
 *
 * \param [in] unit - chip unit id
 * \param [in] group_index - FlexE group index
 * \param [in] phy_port - physical port
 * \param [out] first_slot_index - first slot index
 * \param [out] last_slot_index - last slot index
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
dnx_algo_phy_port_slot_range_get(
    int unit,
    int group_index,
    bcm_port_t phy_port,
    int *first_slot_index,
    int *last_slot_index)
{
    int phy_ports_arr[DNX_DATA_MAX_NIF_FLEXE_NOF_PCS] = { 0 };
    int speed, nof_slots_per_port, nof_ports, i;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Get FlexE core ports sorted by logical PHY ID
     */
    *first_slot_index = 0;
    *last_slot_index = 0;
    SHR_IF_ERR_EXIT(dnx_algo_flexe_phy_ports_sort_get(unit, group_index, phy_ports_arr, &nof_ports));
    /*
     * Get max nof slots per port
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_speed_get(unit, phy_port, 0, &speed));
    nof_slots_per_port = speed / dnx_data_nif.flexe.client_speed_granularity_get(unit);

    for (i = 0; i < nof_ports; ++i)
    {
        if (phy_ports_arr[i] == phy_port)
        {
            *first_slot_index = i * nof_slots_per_port;
            break;
        }
    }
    if (i == nof_ports)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Cannot find the physical port.\n");
    }
    *last_slot_index = *first_slot_index + nof_slots_per_port - 1;
exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_flexe_calendar_slots_adjust(
    int unit,
    int group_index,
    bcm_port_t removed_port)
{
    bcm_port_flexe_group_cal_t cal_id;
    int first_slot, last_slot, slot_index, group_speed;
    int group_nof_slots, port_nof_slots;
    int calendar_slots[DNX_DATA_MAX_NIF_FLEXE_MAX_NOF_SLOTS] = { 0 };

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Get slot range
     */
    SHR_IF_ERR_EXIT(dnx_algo_phy_port_slot_range_get(unit, group_index, removed_port, &first_slot, &last_slot));
    /*
     * Get Max slots for the group
     */
    SHR_IF_ERR_EXIT(dnx_algo_flexe_group_speed_get(unit, group_index, &group_speed));
    group_nof_slots = group_speed / dnx_data_nif.flexe.client_speed_granularity_get(unit);
    port_nof_slots = last_slot - first_slot + 1;

    for (cal_id = bcmPortFlexeGroupCalA; cal_id < bcmPortFlexeGroupCalCount; ++cal_id)
    {
        for (slot_index = 0; slot_index < group_nof_slots; ++slot_index)
        {
            SHR_IF_ERR_EXIT(dnx_algo_flexe_general_db.group_info.
                            demux_cal_slots.get(unit, group_index, cal_id, slot_index, &calendar_slots[slot_index]));
        }
        /*
         * Adjust Rx time slots
         */
        for (slot_index = first_slot; slot_index < (group_nof_slots - port_nof_slots); ++slot_index)
        {
            SHR_IF_ERR_EXIT(dnx_algo_flexe_general_db.group_info.
                            demux_cal_slots.set(unit, group_index, cal_id, slot_index,
                                                calendar_slots[slot_index + port_nof_slots]));
        }
        for (slot_index = 0; slot_index < group_nof_slots; ++slot_index)
        {
            SHR_IF_ERR_EXIT(dnx_algo_flexe_general_db.group_info.
                            mux_cal_slots.get(unit, group_index, cal_id, slot_index, &calendar_slots[slot_index]));
        }
        /*
         * Adjust Tx time slots
         */
        for (slot_index = first_slot; slot_index < (group_nof_slots - port_nof_slots); ++slot_index)
        {
            SHR_IF_ERR_EXIT(dnx_algo_flexe_general_db.group_info.
                            mux_cal_slots.set(unit, group_index, cal_id, slot_index,
                                              calendar_slots[slot_index + port_nof_slots]));
        }
        /*
         * Set the unused slots to "0"
         */
        for (slot_index = (group_nof_slots - port_nof_slots); slot_index < group_nof_slots; ++slot_index)
        {
            SHR_IF_ERR_EXIT(dnx_algo_flexe_general_db.group_info.
                            demux_cal_slots.set(unit, group_index, cal_id, slot_index, 0));
            SHR_IF_ERR_EXIT(dnx_algo_flexe_general_db.group_info.
                            mux_cal_slots.set(unit, group_index, cal_id, slot_index, 0));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_port_flexe_phy_port_is_occupied_get(
    int unit,
    int group_index,
    bcm_port_t logical_port,
    int *is_occupied)
{
    bcm_port_flexe_group_cal_t cal_id;
    bcm_port_t client_logical_port;
    int first_slot, last_slot, slot_index;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Get slot range
     */
    *is_occupied = 0;
    SHR_IF_ERR_EXIT(dnx_algo_phy_port_slot_range_get(unit, group_index, logical_port, &first_slot, &last_slot));
    /*
     * Get check if the physical port is active in Calendar A and B
     */
    for (cal_id = bcmPortFlexeGroupCalA; cal_id < bcmPortFlexeGroupCalCount; ++cal_id)
    {
        for (slot_index = first_slot; slot_index < last_slot; ++slot_index)
        {
            /*
             * Check if it is occupied by Demux slots
             */
            SHR_IF_ERR_EXIT(dnx_algo_flexe_general_db.group_info.
                            demux_cal_slots.get(unit, group_index, cal_id, slot_index, &client_logical_port));
            if (client_logical_port != 0)
            {
                *is_occupied = 1;
                SHR_EXIT();
            }
            /*
             * Check if it is occupied by Mux slots
             */
            SHR_IF_ERR_EXIT(dnx_algo_flexe_general_db.group_info.
                            mux_cal_slots.get(unit, group_index, cal_id, slot_index, &client_logical_port));
            if (client_logical_port != 0)
            {
                *is_occupied = 1;
                SHR_EXIT();
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_flexe_core_active_get(
    int unit,
    uint8 *is_active)
{
    SHR_FUNC_INIT_VARS(unit);

    *is_active = 0;
    SHR_IF_ERR_EXIT(dnx_algo_flexe_general_db.flexe_core_is_active.get(unit, is_active));
exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_flexe_core_active_set(
    int unit,
    uint8 is_active)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_flexe_general_db.flexe_core_is_active.set(unit, is_active));
exit:
    SHR_FUNC_EXIT;
}

/** see header file */
shr_error_e
dnx_algo_flexe_core_blocks_enable_set(
    int unit,
    uint8 enable)
{
#ifndef PLISIM
    int *block_p;
    uint8 *block_valid_p;
#endif
    soc_reg_above_64_val_t reg_above_64_val;
    SHR_FUNC_INIT_VARS(unit);

#ifndef PLISIM
    if (!(SAL_BOOT_PLISIM))
    {
        SHR_IF_ERR_EXIT(soc_info_uint8_address_get(unit, BLOCK_VALID, &block_valid_p));
        /** FASIC block */
        SHR_IF_ERR_EXIT(soc_info_int_address_get(unit, FASIC_BLOCK_INT, &block_p));
        block_valid_p[*block_p] = enable;
        /** FLEXEWP block */
        SHR_IF_ERR_EXIT(soc_info_int_address_get(unit, FLEXEWP_BLOCK_INT, &block_p));
        block_valid_p[*block_p] = enable;
        /** FSAR block */
        SHR_IF_ERR_EXIT(soc_info_int_address_get(unit, FSAR_BLOCK_INT, &block_p));
        block_valid_p[*block_p] = enable;
        /** FSCL block */
        SHR_IF_ERR_EXIT(soc_info_int_address_get(unit, FSCL_BLOCK_INT, &block_p));
        block_valid_p[*block_p] = enable;
    }
#endif

    /** if enable, enable bubble */
    if (enable)
    {
        SOC_REG_ABOVE_64_ALLONES(reg_above_64_val);
        SHR_IF_ERR_EXIT(soc_reg_above_64_set(unit, FASIC_INDIRECT_FORCE_BUBBLEr, REG_PORT_ANY, 0, reg_above_64_val));
        SHR_IF_ERR_EXIT(soc_reg_above_64_set(unit, FLEXEWP_INDIRECT_FORCE_BUBBLEr, REG_PORT_ANY, 0, reg_above_64_val));
        SHR_IF_ERR_EXIT(soc_reg_above_64_set(unit, FSAR_INDIRECT_FORCE_BUBBLEr, REG_PORT_ANY, 0, reg_above_64_val));
        SHR_IF_ERR_EXIT(soc_reg_above_64_set(unit, FSCL_INDIRECT_FORCE_BUBBLEr, REG_PORT_ANY, 0, reg_above_64_val));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_flexe_general_cal_build(
    int unit,
    uint32 *slots_per_object,
    int nof_objects,
    int max_calendar_len,
    uint32 *calendar)
{
    uint32 slot_idx, *slots_per_object_temp = NULL;
    uint32 choose_slots, other_slots, max_slots;
    int max_slots_obj, choose_ratio, other_ratio, allocated_slots;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Clear the calendar
     */
    for (slot_idx = 0; slot_idx < max_calendar_len; ++slot_idx)
    {
        calendar[slot_idx] = DNX_ALGO_CAL_ILLEGAL_OBJECT_ID;
    }
    SHR_ALLOC(slots_per_object_temp, nof_objects * sizeof(uint32), "slots_per_object_temp", "%s%s%s\r\n", EMPTY,
              EMPTY, EMPTY);
    sal_memcpy(slots_per_object_temp, slots_per_object, sizeof(uint32) * nof_objects);
    /** Get the object index which needs max slots */
    max_slots_obj = utilex_get_index_of_max_member_in_array(slots_per_object_temp, nof_objects);

    choose_slots = slots_per_object_temp[max_slots_obj];
    max_slots = max_calendar_len;
    other_slots = max_slots - choose_slots;

    while (choose_slots > 0)
    {
        /*
         * Get the ratio for choose slots and other slots.
         *    For example:
         *        Calendar length: 8, Choose slots: 6, other slots: 2
         *        The choose ratio is 3 and other ratio is 1.
         */
        allocated_slots = 0;
        choose_ratio = (other_slots > choose_slots) ? 1 : ((other_slots > 0) ? (choose_slots / other_slots) : 1);
        other_ratio = (other_slots == 0) ? 0 : ((other_slots > choose_slots) ? (other_slots / choose_slots) : 1);

        for (slot_idx = 0; slot_idx < max_calendar_len; ++slot_idx)
        {
            if (calendar[slot_idx] == -1)
            {
                if (choose_ratio > 0)
                {
                    /*
                     * Allocate slots for choose_slots first.
                     */
                    calendar[slot_idx] = max_slots_obj;
                    --choose_slots;
                    --choose_ratio;
                    ++allocated_slots;
                }
                else if ((choose_ratio == 0) && (other_ratio > 0))
                {
                    /*
                     * If finishing allocating slots for choose slots,
                     * keep the slots position for other_slots.
                     */
                    --other_ratio;
                    --other_slots;
                }
                if ((choose_slots == 0) && (other_slots == 0))
                {
                    /*
                     * Finish allocating slots for the current object.
                     */
                    break;
                }
                if (choose_ratio == 0 && other_ratio == 0)
                {
                    /*
                     * Re-calculate the choose slots after keeping the
                     * position for other slots.
                     */
                    choose_ratio = (other_slots > choose_slots) ? 1 :
                        ((other_slots > 0) ? (choose_slots / other_slots) : 1);
                    /*
                     * Coverity:
                     * If the choose_slots is "0", it means all the slots have been allocated, so
                     * the other_slots should also be "0".
                     */
                     /* coverity[divide_by_zero : FALSE]  */
                    other_ratio = (other_slots == 0) ? 0 :
                        ((other_slots > choose_slots) ? (other_slots / choose_slots) : 1);
                }
            }
        }
        if (slots_per_object_temp[max_slots_obj] != allocated_slots)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Not all slots are allocated.\n");
        }
        /*
         * Remove the object whose slots have been allocated.
         */
        max_slots -= slots_per_object_temp[max_slots_obj];
        slots_per_object_temp[max_slots_obj] = 0;
        /*
         * Get the next oject that occupied the MAX slots.
         */
        max_slots_obj = utilex_get_index_of_max_member_in_array(slots_per_object_temp, nof_objects);
        choose_slots = slots_per_object_temp[max_slots_obj];
        other_slots = max_slots - choose_slots;
    }
exit:
    SHR_FREE(slots_per_object_temp);
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_flexe_cal_slots_is_unaligned_get(
    int unit,
    int group_index,
    uint8 *is_unaligned)
{
    SHR_FUNC_INIT_VARS(unit);

    *is_unaligned = 0;
    SHR_IF_ERR_EXIT(dnx_algo_flexe_general_db.group_info.cal_slots_unaligned.get(unit, group_index, is_unaligned));
exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_flexe_cal_slots_is_unaligned_set(
    int unit,
    int group_index,
    uint8 is_unaligned)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_flexe_general_db.group_info.cal_slots_unaligned.set(unit, group_index, is_unaligned));
exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_flexe_is_valid_pm_get(
    int unit,
    int pm_id,
    int *is_pm_valid)
{
    int i;
    SHR_FUNC_INIT_VARS(unit);

    *is_pm_valid = 0;
    for (i = 0; i < dnx_data_nif.flexe.nof_pms_per_feu_get(unit); ++i)
    {
        if (pm_id == dnx_data_nif.flexe.supported_pms_get(unit, 0)->nif_pms[i])
        {
            *is_pm_valid = 1;
            break;
        }
    }
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_flexe_supported_and_enabled_get(
    int unit,
    uint8 *enabled)
{
    SHR_FUNC_INIT_VARS(unit);

    if (!dnx_data_nif.flexe.feature_get(unit, dnx_data_nif_flexe_is_supported))
    {
        *enabled = FALSE;
    }
    else
    {
        *enabled = (dnx_data_nif.flexe.flexe_mode_get(unit) == DNX_FLEXE_MODE_DISABLED) ? FALSE : TRUE;
    }

    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_flexe_tiny_mac_access_lock_take(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_flexe_general_db.tiny_mac_access_mutex.take(unit, sal_mutex_FOREVER));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_flexe_tiny_mac_access_lock_give(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_flexe_general_db.tiny_mac_access_mutex.give(unit));

exit:
    SHR_FUNC_EXIT;
}

/*
 * }
 */

#undef _ERR_MSG_MODULE_NAME
