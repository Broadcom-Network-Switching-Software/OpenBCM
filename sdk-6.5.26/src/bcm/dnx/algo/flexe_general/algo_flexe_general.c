/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

/** \file algo_flexe_general.c
 *
 *  Handle the flexe general functions.
 *    * group info
 *    * resource manager
 *
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_PORT

/*
 * Include files.
 * {
 */
#include <soc/sand/shrextend/shrextend_debug.h>
#include <sal/core/boot.h>

#include <shared/utilex/utilex_integer_arithmetic.h>

#include <bcm/types.h>

#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/algo/flexe_general/algo_flexe_general.h>
#include <bcm_int/dnx/algo/flexe_core/algo_flexe_core.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/dnx_algo_port_access.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/dnx_algo_flexe_core_access.h>
#include <bcm_int/dnx/port/port_flexe.h>
#include <bcm_int/dnx/algo/cal/algo_cal.h>

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
#define DNX_ALGO_FRAMER_TX_INIT_CREDIT_CAL(dest, base_val, ratio, divider)  \
    do                                                                      \
    {                                                                       \
        uint64 temp_val, divider64;                                         \
        COMPILER_64_SET(temp_val, 0, base_val);                             \
        COMPILER_64_UMUL_32(temp_val, ratio);                               \
        COMPILER_64_SET(divider64, 0, divider);                             \
        COMPILER_64_UDIV_64(temp_val, divider64);                           \
        dest = COMPILER_64_LO(temp_val);                                    \
    } while(0)

/*
 * }
 */
/*
 * Defines
 * {
 */
/*
 * The min delta between 2 credits is 2 - 8/10.
 * 1000 is used to convert the time to NS.
 */
#define MIN_DELTA_2_CREDIT_IN_FRAMER       ((200 - 80) * 1000 / 100)
/*
 * The min delta among 3 credits is 3 + 8/10.
 * 1000 is used to convert the time to NS.
 */
#define MAX_DELTA_3_CREDIT_IN_WRAPPER      ((300 + 80) * 1000 / 100)
/*
 * The min delta for 1 credit is 1 - 1/2.
 * 1000 is used to convert the time to NS.
 */
#define MIN_DELTA_1_CREDIT_IN_FRAMER       ((100 - 50) * 1000 / 100)
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
    create_data.nof_elements = dnx_data_nif.flexe.nof_clients_get(unit);
    sal_strncpy_s(create_data.name, "FLEXE_CLIENT_CHANNEL_ALLOC", DNX_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);
    SHR_IF_ERR_EXIT(dnx_algo_flexe_general_db.resource_alloc.client_channel.create(unit, &create_data, NULL));
    /*
     * FlexE SAR channel allocation manager create
     */
    create_data.first_element = 0;
    create_data.nof_elements = dnx_data_nif.sar_client.nof_clients_get(unit);
    sal_strncpy_s(create_data.name, "FLEXE_SAR_CHANNEL_ALLOC", DNX_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);
    SHR_IF_ERR_EXIT(dnx_algo_flexe_general_db.resource_alloc.sar_channel.create(unit, &create_data, NULL));
    /*
     * FlexE MAC channel allocation manager create
     */
    create_data.first_element = 0;
    create_data.nof_elements = dnx_data_nif.mac_client.nof_normal_clients_get(unit);
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
    if (dnx_data_nif.flexe.feu_nof_get(unit) > 0)
    {
        sal_memset(&create_data, 0, sizeof(create_data));
        create_data.first_element = 0;
        create_data.nof_elements = dnx_data_nif.mac_client.nof_logical_fifos_get(unit);
        sal_strncpy_s(create_data.name, "FLEXE_RMC_ID_ALLOC", DNX_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);
        SHR_IF_ERR_EXIT(dnx_algo_flexe_general_db.resource_alloc.rmc_id.create(unit, &create_data, NULL));
    }
    /*
     * Allocate virtual FlexE channel if SAR is not connected to 66b switching
     */
    if (!dnx_data_nif.sar_client.feature_get(unit, dnx_data_nif_sar_client_connect_to_66bswitch))
    {
        /*
         * Virtual FlexE channel allocation manager create
         */
        create_data.first_element = 0;
        create_data.nof_elements = dnx_data_nif.flexe.nof_clients_get(unit);
        sal_strncpy_s(create_data.name, "FLEXE_CLIENT_CHANNEL_ALLOC", DNX_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);
        SHR_IF_ERR_EXIT(dnx_algo_flexe_general_db.resource_alloc.
                        virtual_client_channel.create(unit, &create_data, NULL));
    }
    /*
     * Allocate MAC1/MAC2 channel for L1 ETH 1588 handling
     */
    if (dnx_data_nif.framer.feature_get(unit, dnx_data_nif_framer_l1_eth_supported))
    {
        create_data.first_element = 0;
        create_data.nof_elements = dnx_data_nif.flexe.nof_clients_get(unit);

        sal_strncpy_s(create_data.name, "MAC1_CHANNEL_ALLOC", DNX_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);
        SHR_IF_ERR_EXIT(dnx_algo_flexe_general_db.resource_alloc.mac1.create(unit, &create_data, NULL));

        sal_strncpy_s(create_data.name, "MAC2_CHANNEL_ALLOC", DNX_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);
        SHR_IF_ERR_EXIT(dnx_algo_flexe_general_db.resource_alloc.mac2.create(unit, &create_data, NULL));
    }
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
    /*
     * Set invalid group id to -1
     */
    SHR_IF_ERR_EXIT(dnx_algo_flexe_general_db.group_info.group_id.set(unit, group_index, -1));
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
    int is_valid, nof_mgmt_channels, i;
    int is_busa, curr_is_busa;

    SHR_FUNC_INIT_VARS(unit);

    if (DNX_ALGO_PORT_TYPE_IS_FRAMER_MAC(unit, port_info, TRUE, FALSE, FALSE)
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
    else if (DNX_ALGO_PORT_TYPE_IS_FRAMER_MGMT(unit, port_info))
    {
        /*
         * Verify the channel range
         */
        nof_mgmt_channels = dnx_data_nif.framer.nof_mgmt_channels_get(unit);
        SHR_RANGE_VERIFY(client_channel, 0, nof_mgmt_channels - 1, _SHR_E_PARAM, "Channel ID is out of range.\n");
        /*
         * Verify if the channel is valid on the device
         */
        if (!dnx_data_nif.framer.mgmt_channel_info_get(unit, client_channel)->valid)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "The channel ID is invalid on the device.\r\n");
        }
        /*
         * Verify if the channel is occupied
         */
        SHR_IF_ERR_EXIT(dnx_algo_flexe_general_db.mgmt_channel_info.valid.get(unit, client_channel, &is_valid));
        if (is_valid)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "The channel ID %d has been occupied.\r\n", client_channel);
        }
        /*
         * Check if all the management channels map to the same direction, busa or busb
         */
        for (i = 0; i < nof_mgmt_channels; ++i)
        {
            SHR_IF_ERR_EXIT(dnx_algo_flexe_general_db.mgmt_channel_info.valid.get(unit, i, &is_valid));
            if (is_valid)
            {
                SHR_IF_ERR_EXIT(dnx_algo_flexe_general_db.mgmt_channel_info.is_busa.get(unit, i, &is_busa));
                curr_is_busa = DNX_ALGO_PORT_TYPE_IS_NIF_ETH(unit, port_info, 0) ? 1 : 0;
                SHR_VAL_VERIFY(is_busa, curr_is_busa, _SHR_E_PARAM,
                               "All the mgmt channel should mux to the same direction, busa or busb!\n");
            }
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
    bcm_pbmp_t group_phy_ports, logical_phy_id_bmp, all_phy_ports;
    int speed, logical_phy_id = 0;
    int old_logical_phy_id = 0, group_index;
    int order, new_logical_phy_order = 0, old_logical_phy_order = 0;
    bcm_port_t logical_port;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_speed_get(unit, physical_port, 0, &speed));
    SHR_IF_ERR_EXIT(dnx_algo_flexe_logical_phy_id_range_and_speed_verify(unit, speed, new_logical_phy_id));

    SHR_IF_ERR_EXIT(dnx_algo_flexe_groups_phy_ports_get(unit, &all_phy_ports));
    /*
     * Verify the logical PHY ID range if it belongs to one group
     */
    if (_SHR_PBMP_MEMBER(all_phy_ports, physical_port))
    {
        SHR_IF_ERR_EXIT(dnx_algo_flexe_physical_port_to_group_index_get(unit, physical_port, &group_index));
        SHR_IF_ERR_EXIT(dnx_algo_flexe_general_db.group_info.phy_ports.get(unit, group_index, &group_phy_ports));

        /*
         * The new logical PHY ID should keep the original logical PHY orders
         */
        BCM_PBMP_CLEAR(logical_phy_id_bmp);
        _SHR_PBMP_ITER(group_phy_ports, logical_port)
        {
            /** Get logical PHY id from NIF DB */
            SHR_IF_ERR_EXIT(dnx_algo_flexe_phy_logical_phy_id_get(unit, logical_port, &logical_phy_id));
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
        SHR_IF_ERR_EXIT(dnx_algo_flexe_phy_logical_phy_id_get(unit, physical_port, &old_logical_phy_id));
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
    int min_group_id, max_group_id, i, tmp_group_id, valid;

    SHR_FUNC_INIT_VARS(unit);

    min_group_id = dnx_data_nif.flexe.min_group_id_get(unit);
    max_group_id = dnx_data_nif.flexe.max_group_id_get(unit);

    SHR_RANGE_VERIFY(group_id, min_group_id, max_group_id, _SHR_E_PARAM, "group id is out of range.\n");
    /*
     * Check if the group id is occupied. Skip invalid group id "0"
     */
    if (group_id != 0)
    {
        for (i = 0; i < dnx_data_nif.flexe.nof_groups_get(unit); ++i)
        {
            SHR_IF_ERR_EXIT(dnx_algo_flexe_general_db.group_info.valid.get(unit, i, &valid));
            if (valid)
            {
                SHR_IF_ERR_EXIT(dnx_algo_flexe_general_db.group_info.group_id.get(unit, i, &tmp_group_id));
                if (tmp_group_id == group_id)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "The FlexE group Id has been occupied.\r\n");
                }
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

    if (!DNX_ALGO_PORT_TYPE_IS_FLEXE_PHY(unit, port_info, FALSE))
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

    if (!DNX_ALGO_PORT_TYPE_IS_FRAMER_SAR(unit, port_info, FALSE))
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
    int client_channel, max_speed, index;
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
    supported_flags = BCM_PORT_RESOURCE_ASYMMETRICAL | BCM_PORT_RESOURCE_G_HAO;
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
    SHR_IF_ERR_EXIT(dnx_algo_port_flexe_channel_get(unit, resource->port, &client_channel));

    if (DNX_ALGO_FLEXE_MAC_CHANNEL_IS_PTP_OR_OAM(unit, client_channel))
    {
        max_speed =
            (DNX_ALGO_FLEXE_MAC_CHANNEL_IS_PTP(unit, client_channel)) ? dnx_data_nif.
            mac_client.ptp_channel_max_bandwidth_get(unit) : dnx_data_nif.mac_client.
            oam_channel_max_bandwidth_get(unit);
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
        /*
         * Validate speed granularity, the busb/busc granularity should be identical to the
         * FlexE client granularity
         */
        for (index = 0; index < dnx_data_nif.flexe.client_speed_granularity_info_get(unit)->key_size[0]; index++)
        {
            if (resource->speed % dnx_data_nif.flexe.client_speed_granularity_get(unit, index)->val == 0)
            {
                break;
            }
        }
        if (index == dnx_data_nif.flexe.client_speed_granularity_info_get(unit)->key_size[0])
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "The granularity of the client speed is incorrect.\r\n");
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
    dnx_algo_port_info_s * port_info,
    bcm_port_resource_t * resource)
{
    int i, flexe_core_port, instance_id;
    const int *instance_id_array = NULL;
    bcm_pbmp_t flexe_phy_ports;

    SHR_FUNC_INIT_VARS(unit);

    /** Verify speed */
    for (i = 0; i < dnx_data_nif.flexe.nof_phy_speeds_get(unit); i++)
    {
        if (resource->speed == dnx_data_nif.flexe.phy_info_get(unit, i)->speed)
        {
            instance_id_array = dnx_data_nif.flexe.phy_info_get(unit, i)->instance_id;
            break;
        }
    }
    if (i == dnx_data_nif.flexe.nof_phy_speeds_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "The FlexE physical port speed(%d) is not supported.\r\n", resource->speed);
    }
    if (!DNX_ALGO_PORT_TYPE_IS_FLEXE_UNAWARE(unit, (*port_info)))
    {
        /** Verify base flexe instance */
        if (resource->base_flexe_instance != -1)
        {
            for (i = 0; i < dnx_data_nif.flexe.nof_flexe_instances_get(unit); i++)
            {
                /*
                 * Coverity:
                 * If the instance_id_array is NULL, the codes should return in
                 * the previous line.
                 */
                 /* coverity[var_deref_op : FALSE]  */
                if (instance_id_array == NULL)
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL, "instance_id_array is NULL. Illegal.\r\n");
                }
                if (resource->base_flexe_instance == instance_id_array[i])
                {
                    break;
                }
            }
            if (i == dnx_data_nif.flexe.nof_flexe_instances_get(unit))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "The base flexe instance ID %d is not supported for speed %d.\r\n",
                             resource->base_flexe_instance, resource->speed);
            }
        }
        /** Verify if the flexe_base_instance can be changed */
        SHR_IF_ERR_EXIT(dnx_algo_flexe_phy_to_core_port_get(unit, resource->port, &flexe_core_port));
        if (flexe_core_port != DNX_ALGO_PORT_INVALID)
        {
            SHR_IF_ERR_EXIT(dnx_algo_flexe_core_port_to_instance_id_get(unit, flexe_core_port, &instance_id));
            if (instance_id != resource->base_flexe_instance)
            {
                SHR_IF_ERR_EXIT(dnx_algo_flexe_groups_phy_ports_get(unit, &flexe_phy_ports));
                if (_SHR_PBMP_MEMBER(flexe_phy_ports, resource->port))
                {
                    SHR_ERR_EXIT(_SHR_E_CONFIG,
                                 "The flexe phy has binded into one FlexE group, the base_flexe_instance cannot be changed. Please configure it to the allocated value.\r\n");
                }
            }
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
        if (flexe_core_ports_array == NULL)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "flexe_core_ports_array is NULL.\r\n");
        }
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
                    flexe_core_port.free_several(unit, nof_elements, flexe_core_port, NULL));

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
 * Get total group speed for specific slot mode
 */
static shr_error_e
dnx_algo_flexe_groups_speed_get(
    int unit,
    bcm_port_flexe_phy_slot_mode_t slot_mode,
    int *groups_speed)
{
    bcm_port_flexe_phy_slot_mode_t curr_slot_mode;
    int i, valid, group_speed;

    SHR_FUNC_INIT_VARS(unit);

    *groups_speed = 0;
    for (i = 0; i < dnx_data_nif.flexe.nof_groups_get(unit); i++)
    {
        SHR_IF_ERR_EXIT(dnx_algo_flexe_general_db.group_info.valid.get(unit, i, &valid));
        if (valid)
        {
            SHR_IF_ERR_EXIT(dnx_algo_flexe_general_db.group_info.slot_mode.get(unit, i, &curr_slot_mode));
            if (curr_slot_mode == slot_mode)
            {
                SHR_IF_ERR_EXIT(dnx_algo_flexe_group_speed_get(unit, i, &group_speed));
                *groups_speed += group_speed;
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
dnx_algo_flexe_group_speed_verify(
    int unit,
    bcm_port_flexe_phy_slot_mode_t slot_mode,
    int speed)
{
    bcm_port_flexe_phy_slot_mode_t tmp_slot_mode;
    int tmp_speed, total_speed = 0, non_5g_total_speed = 0;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Verify FlexE group speed
     */
    for (tmp_slot_mode = bcmPortFlexePhySlot5G; tmp_slot_mode < bcmPortFlexePhySlotModeCount; ++tmp_slot_mode)
    {
        SHR_IF_ERR_EXIT(dnx_algo_flexe_groups_speed_get(unit, tmp_slot_mode, &tmp_speed));
        total_speed += tmp_speed;
        /*
         * Count speed for 1G and 1.25G
         */
        if (tmp_slot_mode != bcmPortFlexePhySlot5G)
        {
            non_5g_total_speed += tmp_speed;
        }
    }
    if ((total_speed + speed) > dnx_data_nif.flexe.max_flexe_core_speed_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "The Max speed for all groups should be %d.\r\n",
                     dnx_data_nif.flexe.max_flexe_core_speed_get(unit));
    }
    /*
     * Verify non-5G group BW
     */
    if (slot_mode != bcmPortFlexePhySlot5G)
    {
        if ((non_5g_total_speed + speed) > dnx_data_nif.flexe.non_5g_slot_bw_get(unit))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "The Max speed for Non 5G slot groups should be %d.\r\n",
                         dnx_data_nif.flexe.non_5g_slot_bw_get(unit));
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
    nof_fifo_entries = dnx_data_nif.mac_client.nof_sb_fifos_get(unit);
    SHR_IF_ERR_EXIT(dnx_algo_flexe_general_db.fifo_link_list.alloc(unit, DNX_ALGO_FLEXE_FIFO_SB_RX, nof_fifo_entries));
    /*
     * Allocate SB TX FIFO Link List DB
     */
    if (dnx_data_nif.mac_client.feature_get(unit, dnx_data_nif_mac_client_sb_tx_fifo_supported))
    {
        SHR_IF_ERR_EXIT(dnx_algo_flexe_general_db.
                        fifo_link_list.alloc(unit, DNX_ALGO_FLEXE_FIFO_SB_TX, nof_fifo_entries));
    }
    /*
     * If there is no FEU module, the RMC/TMC logical is not handled by FLEXE unit
     */
    if (dnx_data_nif.flexe.feu_nof_get(unit) > 0)
    {
        /*
         * Allocate TMC FIFO Link List DB
         */
        nof_fifo_entries = dnx_data_nif.mac_client.nof_clients_get(unit);
        SHR_IF_ERR_EXIT(dnx_algo_flexe_general_db.
                        fifo_link_list.alloc(unit, DNX_ALGO_FLEXE_FIFO_TMC, nof_fifo_entries));

        /*
         * Allocate RMC FIFO Link List DB
         */
        nof_fifo_entries = dnx_data_nif.mac_client.nof_logical_fifos_get(unit);
        SHR_IF_ERR_EXIT(dnx_algo_flexe_general_db.
                        fifo_link_list.alloc(unit, DNX_ALGO_FLEXE_FIFO_RMC, nof_fifo_entries));
    }
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
     * Init SB FIFO Link List DB
     */
    for (entry_idx = 0; entry_idx < dnx_data_nif.mac_client.nof_sb_fifos_get(unit); ++entry_idx)
    {
        SHR_IF_ERR_EXIT(dnx_algo_flexe_general_db.fifo_link_list.set(unit, DNX_ALGO_FLEXE_FIFO_SB_RX, entry_idx, -1));
        if (dnx_data_nif.mac_client.feature_get(unit, dnx_data_nif_mac_client_sb_tx_fifo_supported))
        {
            SHR_IF_ERR_EXIT(dnx_algo_flexe_general_db.
                            fifo_link_list.set(unit, DNX_ALGO_FLEXE_FIFO_SB_TX, entry_idx, -1));
        }
    }
    /*
     * If there is no FEU module, the RMC/TMC logical is not handled by FLEXE unit
     */
    if (dnx_data_nif.flexe.feu_nof_get(unit) > 0)
    {
        /*
         * Init TMC FIFO Link List DB
         */
        for (entry_idx = 0; entry_idx < dnx_data_nif.mac_client.nof_clients_get(unit); ++entry_idx)
        {
            SHR_IF_ERR_EXIT(dnx_algo_flexe_general_db.fifo_link_list.set(unit, DNX_ALGO_FLEXE_FIFO_TMC, entry_idx, -1));
        }

        /*
         * Init RMC FIFO Link List DB
         */
        for (entry_idx = 0; entry_idx < dnx_data_nif.mac_client.nof_logical_fifos_get(unit); ++entry_idx)
        {
            SHR_IF_ERR_EXIT(dnx_algo_flexe_general_db.fifo_link_list.set(unit, DNX_ALGO_FLEXE_FIFO_RMC, entry_idx, -1));
        }
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
    int nof_groups, group_index, nof_mgmt_channels, cal_id;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Create SW State instance
     */
    SHR_IF_ERR_EXIT(dnx_algo_flexe_general_db.init(unit));

    /*
     * Allocate SW STATE DBs
     */
    SHR_IF_ERR_EXIT(dnx_algo_flexe_general_db.group_info.alloc(unit));
    SHR_IF_ERR_EXIT(dnx_algo_flexe_general_db.demux_slot_info.alloc(unit));
    SHR_IF_ERR_EXIT(dnx_algo_flexe_general_db.mux_slot_info.alloc(unit));

    nof_mgmt_channels = dnx_data_nif.framer.nof_mgmt_channels_get(unit);
    SHR_IF_ERR_EXIT(dnx_algo_flexe_general_db.mgmt_channel_info.alloc(unit, nof_mgmt_channels));
    /*
     * Set invalid group id
     */
    nof_groups = dnx_data_nif.flexe.nof_groups_get(unit);
    for (group_index = 0; group_index < nof_groups; ++group_index)
    {
        SHR_IF_ERR_EXIT(dnx_algo_flexe_group_info_db_init(unit, group_index));
        for (cal_id = 0; cal_id < bcmPortFlexeGroupCalCount; ++cal_id)
        {
            SHR_IF_ERR_EXIT(dnx_algo_flexe_general_db.mux_slot_info.cal_slots.alloc(unit, group_index, cal_id));
            SHR_IF_ERR_EXIT(dnx_algo_flexe_general_db.demux_slot_info.cal_slots.alloc(unit, group_index, cal_id));
        }
    }
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
    if ((type == DNX_ALGO_FLEXE_FIFO_SB_RX) || (type == DNX_ALGO_FLEXE_FIFO_SB_TX))
    {
        *nof_entries = dnx_data_nif.mac_client.nof_sb_fifos_get(unit);
    }
    else if (type == DNX_ALGO_FLEXE_FIFO_TMC)
    {
        *nof_entries = dnx_data_nif.mac_client.nof_clients_get(unit);
    }
    else if (type == DNX_ALGO_FLEXE_FIFO_RMC)
    {
        *nof_entries = dnx_data_nif.mac_client.nof_logical_fifos_get(unit);
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
    else if (speed < dnx_data_nif.mac_client.sb_fifo_granularity_get(unit))
    {
        if ((type == DNX_ALGO_FLEXE_FIFO_SB_RX) && (dnx_data_nif.mac_client.nof_special_clients_get(unit) > 0))
        {
            *nof_entries = dnx_data_nif.mac_client.ptp_oam_fifo_entries_in_sb_rx_get(unit);
        }
        else
        {
            *nof_entries = 1;
        }
    }
    else
    {
        if (dnx_data_nif.mac_client.feature_get(unit, dnx_data_nif_mac_client_double_slots_for_small_client) &&
            ((type == DNX_ALGO_FLEXE_FIFO_SB_RX) && ((speed == 5000) || (speed == 10000))))
        {
            *nof_entries = 2 * speed / dnx_data_nif.mac_client.sb_fifo_granularity_get(unit);
        }
        else if (dnx_data_nif.mac_client.feature_get(unit, dnx_data_nif_mac_client_double_slots_for_all_client)
                 && (type == DNX_ALGO_FLEXE_FIFO_SB_RX))
        {
            *nof_entries = 2 * speed / dnx_data_nif.mac_client.sb_fifo_granularity_get(unit);
        }
        else
        {
            *nof_entries = speed / dnx_data_nif.mac_client.sb_fifo_granularity_get(unit);
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

    SHR_IF_ERR_EXIT(dnx_algo_flexe_general_db.resource_alloc.rmc_id.free_single(unit, rmc_id, NULL));

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
    uint8 mac_allocated, virtual_flexe_allocated;
    int alloc_flags = 0, index;

    SHR_FUNC_INIT_VARS(unit);

    *channel = 0;
    if (DNX_ALGO_PORT_USE_FLEXE_CLIENT_CHANNEL(unit, port_info))
    {
        SHR_IF_ERR_EXIT(dnx_algo_flexe_general_db.resource_alloc.
                        client_channel.allocate_single(unit, alloc_flags, NULL, channel));
    }
    else if (DNX_ALGO_PORT_TYPE_IS_FRAMER_MAC(unit, port_info, TRUE, TRUE, TRUE))
    {
        SHR_IF_ERR_EXIT(dnx_algo_flexe_general_db.resource_alloc.
                        mac_channel.allocate_single(unit, alloc_flags, NULL, channel));
    }
    else if (DNX_ALGO_PORT_TYPE_IS_FRAMER_SAR(unit, port_info, TRUE))
    {
        if (dnx_data_nif.sar_client.feature_get(unit, dnx_data_nif_sar_client_is_tdm_port))
        {
            if (DNX_ALGO_PORT_TYPE_IS_OTN_SAR(unit, port_info))
            {
                /*
                 * For OTN SAR, just allocate one SAR channel
                 */
                SHR_IF_ERR_EXIT(dnx_algo_flexe_general_db.resource_alloc.
                                mac_channel.allocate_single(unit, alloc_flags, NULL, channel));
            }
            else
            {
                for (index = 0; index < dnx_data_nif.sar_client.nof_clients_get(unit); ++index)
                {
                    /*
                     * If SAR client is TDM port, it will share the same resource pool
                     * with MAC client due to EGQ limitation
                     */
                    SHR_IF_ERR_EXIT(dnx_algo_flexe_general_db.resource_alloc.
                                    mac_channel.is_allocated(unit, index, &mac_allocated));
                    /*
                     * SAR channel must be the same as virtual FlexE channel
                     */
                    SHR_IF_ERR_EXIT(dnx_algo_flexe_general_db.resource_alloc.
                                    virtual_client_channel.is_allocated(unit, index, &virtual_flexe_allocated));

                    if (!mac_allocated && !virtual_flexe_allocated)
                    {
                        break;
                    }
                }
                if (index == dnx_data_nif.sar_client.nof_clients_get(unit))
                {
                    SHR_ERR_EXIT(_SHR_E_RESOURCE, "There is no free channels for SAR port.\n");
                }
                /*
                 * Mark the channel is allocated
                 */
                alloc_flags = DNX_ALGO_RES_ALLOCATE_WITH_ID;
                SHR_IF_ERR_EXIT(dnx_algo_flexe_general_db.resource_alloc.
                                mac_channel.allocate_single(unit, alloc_flags, NULL, &index));
                SHR_IF_ERR_EXIT(dnx_algo_flexe_general_db.resource_alloc.
                                virtual_client_channel.allocate_single(unit, alloc_flags, NULL, &index));
                *channel = index;
            }
        }
        else
        {
            SHR_IF_ERR_EXIT(dnx_algo_flexe_general_db.resource_alloc.
                            sar_channel.allocate_single(unit, alloc_flags, NULL, channel));
        }
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

    if (DNX_ALGO_PORT_USE_FLEXE_CLIENT_CHANNEL(unit, port_info))
    {
        SHR_IF_ERR_EXIT(dnx_algo_flexe_general_db.resource_alloc.client_channel.free_single(unit, channel, NULL));
    }
    else if (DNX_ALGO_PORT_TYPE_IS_FRAMER_MAC(unit, port_info, TRUE, TRUE, TRUE))
    {
        SHR_IF_ERR_EXIT(dnx_algo_flexe_general_db.resource_alloc.mac_channel.free_single(unit, channel, NULL));
    }
    else if (DNX_ALGO_PORT_TYPE_IS_FRAMER_SAR(unit, port_info, TRUE))
    {
        if (dnx_data_nif.sar_client.feature_get(unit, dnx_data_nif_sar_client_is_tdm_port))
        {
            /*
             * If SAR client is TDM port, it will share the same resource pool
             * with MAC client due to EGQ limitation
             */
            SHR_IF_ERR_EXIT(dnx_algo_flexe_general_db.resource_alloc.mac_channel.free_single(unit, channel, NULL));
            if (!DNX_ALGO_PORT_TYPE_IS_OTN_SAR(unit, port_info))
            {
                SHR_IF_ERR_EXIT(dnx_algo_flexe_general_db.resource_alloc.
                                virtual_client_channel.free_single(unit, channel, NULL));
            }
        }
        else
        {
            SHR_IF_ERR_EXIT(dnx_algo_flexe_general_db.resource_alloc.sar_channel.free_single(unit, channel, NULL));
        }
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
dnx_algo_flexe_shared_channel_alloc(
    int unit,
    bcm_port_framer_module_t module_id,
    int *channel)
{
    int alloc_flags = 0;

    SHR_FUNC_INIT_VARS(unit);

    *channel = -1;

    switch (module_id)
    {
        case bcmPortFramerModuleNormalFlexe:
        {
            SHR_IF_ERR_EXIT(dnx_algo_flexe_general_db.resource_alloc.
                            client_channel.allocate_single(unit, alloc_flags, NULL, channel));
            break;
        }
        case bcmPortFramerModuleVirtualFlexe:
        {
            SHR_IF_ERR_EXIT(dnx_algo_flexe_general_db.resource_alloc.
                            virtual_client_channel.allocate_single(unit, alloc_flags, NULL, channel));
            break;
        }
        case bcmPortFramerModuleMac1:
        {
            SHR_IF_ERR_EXIT(dnx_algo_flexe_general_db.resource_alloc.
                            mac1.allocate_single(unit, alloc_flags, NULL, channel));
            break;
        }
        case bcmPortFramerModuleMac2:
        {
            SHR_IF_ERR_EXIT(dnx_algo_flexe_general_db.resource_alloc.
                            mac2.allocate_single(unit, alloc_flags, NULL, channel));
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported module ID.\n");
        }
    }
exit:
    SHR_FUNC_EXIT;
}
/*
 * See .h file
 */
shr_error_e
dnx_algo_flexe_shared_channel_free(
    int unit,
    bcm_port_framer_module_t module_id,
    int channel)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (module_id)
    {
        case bcmPortFramerModuleNormalFlexe:
        {
            SHR_IF_ERR_EXIT(dnx_algo_flexe_general_db.resource_alloc.client_channel.free_single(unit, channel, NULL));
            break;
        }
        case bcmPortFramerModuleVirtualFlexe:
        {
            SHR_IF_ERR_EXIT(dnx_algo_flexe_general_db.resource_alloc.
                            virtual_client_channel.free_single(unit, channel, NULL));
            break;
        }
        case bcmPortFramerModuleMac1:
        {
            SHR_IF_ERR_EXIT(dnx_algo_flexe_general_db.resource_alloc.mac1.free_single(unit, channel, NULL));
            break;
        }
        case bcmPortFramerModuleMac2:
        {
            SHR_IF_ERR_EXIT(dnx_algo_flexe_general_db.resource_alloc.mac2.free_single(unit, channel, NULL));
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported module ID.\n");
        }
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
    uint16 slot_info;
    int client_id_map[DNX_DATA_MAX_NIF_FLEXE_NOF_CLIENTS] = { 0 };

    SHR_FUNC_INIT_VARS(unit);

    for (slot_index = 0; slot_index < nof_slots; ++slot_index)
    {
        SHR_IF_ERR_EXIT(dnx_algo_flexe_general_db.mux_slot_info.
                        cal_slots.get(unit, group_index, calendar_id, slot_index, &slot_info));
        if (slot_info != 0)
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_flexe_channel_get(unit, slot_info, &client_channel));
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
    if ((int) calendar_id < 0 || calendar_id >= bcmPortFlexeGroupCalCount)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "calendar_id is out range.\n");
    }
    if (slot_index < 0 || slot_index >= dnx_data_nif.flexe.max_nof_slots_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "slot index is out range.\n");
    }

    if (flags & BCM_PORT_FLEXE_RX)
    {
        SHR_IF_ERR_EXIT(dnx_algo_flexe_general_db.demux_slot_info.
                        cal_slots.set(unit, group_index, calendar_id, slot_index, logical_port));
    }
    if (flags & BCM_PORT_FLEXE_TX)
    {
        SHR_IF_ERR_EXIT(dnx_algo_flexe_general_db.mux_slot_info.
                        cal_slots.set(unit, group_index, calendar_id, slot_index, logical_port));
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
    uint16 slot_info;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Verify the argument used for accessing SW DB
     */
    if (group_index < 0 || group_index >= dnx_data_nif.flexe.nof_groups_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "FlexE group index is out of range.\n");
    }
    if ((int) calendar_id < 0 || calendar_id >= bcmPortFlexeGroupCalCount)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "calendar_id is out range.\n");
    }
    if (slot_index < 0 || slot_index >= dnx_data_nif.flexe.max_nof_slots_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "slot index is out range.\n");
    }

    if (flags & BCM_PORT_FLEXE_RX)
    {
        SHR_IF_ERR_EXIT(dnx_algo_flexe_general_db.demux_slot_info.
                        cal_slots.get(unit, group_index, calendar_id, slot_index, &slot_info));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_algo_flexe_general_db.mux_slot_info.
                        cal_slots.get(unit, group_index, calendar_id, slot_index, &slot_info));
    }
    *logical_port = slot_info;
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
        SHR_IF_ERR_EXIT(dnx_algo_flexe_phy_to_core_port_get(unit, logical_port, &flexe_core_port));
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
        SHR_IF_ERR_EXIT(dnx_algo_flexe_phy_logical_phy_id_get(unit, logical_port, &logical_phy_id));
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
dnx_algo_flexe_core_ports_sort_get(
    int unit,
    int group_index,
    bcm_port_t * flexe_core_ports_arr,
    int *logical_phy_id_arr,
    int *nof_ports)
{
    int i;
    bcm_port_t logical_ports[DNX_DATA_MAX_NIF_FLEXE_NOF_PCS] = { 0 };

    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_algo_flexe_phy_ports_sort_get(unit, group_index, logical_ports, nof_ports));

    for (i = 0; i < *nof_ports; ++i)
    {
        SHR_IF_ERR_EXIT(dnx_algo_flexe_phy_to_core_port_get(unit, logical_ports[i], &flexe_core_ports_arr[i]));
        SHR_IF_ERR_EXIT(dnx_algo_flexe_phy_logical_phy_id_get(unit, logical_ports[i], &logical_phy_id_arr[i]));
    }
exit:
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
    bcm_port_flexe_phy_slot_mode_t slot_mode;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Get FlexE core ports sorted by logical PHY ID
     */
    SHR_IF_ERR_EXIT(dnx_algo_flexe_phy_ports_sort_get(unit, group_index, phy_ports_arr, &nof_ports));
    /*
     * Get max nof slots per port
     */
    SHR_IF_ERR_EXIT(dnx_algo_flexe_group_phy_speed_get(unit, group_index, &phy_speed));
    SHR_IF_ERR_EXIT(dnx_algo_flexe_group_slot_mode_get(unit, group_index, &slot_mode));
    nof_slots_per_port = phy_speed / dnx_data_nif.flexe.client_speed_granularity_get(unit, slot_mode)->val;
    /*
     * Clear the DB
     */
    sal_memset(ts_config_db, 0, nof_clients * sizeof(dnx_algo_flexe_client_ts_config_t));

    for (i = 0; i < nof_ports; ++i)
    {
        _SHR_PBMP_CLEAR(accumulated_clients);
        SHR_IF_ERR_EXIT(dnx_algo_flexe_phy_to_core_port_get(unit, phy_ports_arr[i], &flexe_core_port));

        inner_slot_index = 0;
        for (slot_index = nof_slots_per_port * i; slot_index < (nof_slots_per_port * i + nof_slots_per_port);
             ++slot_index)
        {
            if (calendar_slots[slot_index] != 0)
            {
                SHR_IF_ERR_EXIT(dnx_algo_port_flexe_channel_get(unit, calendar_slots[slot_index], &client_channel));
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
    bcm_port_flexe_phy_slot_mode_t slot_mode;

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
    SHR_IF_ERR_EXIT(dnx_algo_flexe_group_slot_mode_get(unit, group_index, &slot_mode));
    nof_slots_per_port = speed / dnx_data_nif.flexe.client_speed_granularity_get(unit, slot_mode)->val;

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
    uint16 calendar_slots[DNX_DATA_MAX_NIF_FLEXE_MAX_NOF_SLOTS] = { 0 };
    bcm_port_flexe_phy_slot_mode_t slot_mode;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Get slot range
     */
    SHR_IF_ERR_EXIT(dnx_algo_phy_port_slot_range_get(unit, group_index, removed_port, &first_slot, &last_slot));
    /*
     * Get Max slots for the group
     */
    SHR_IF_ERR_EXIT(dnx_algo_flexe_group_speed_get(unit, group_index, &group_speed));
    SHR_IF_ERR_EXIT(dnx_algo_flexe_group_slot_mode_get(unit, group_index, &slot_mode));
    group_nof_slots = group_speed / dnx_data_nif.flexe.client_speed_granularity_get(unit, slot_mode)->val;
    port_nof_slots = last_slot - first_slot + 1;

    for (cal_id = bcmPortFlexeGroupCalA; cal_id < bcmPortFlexeGroupCalCount; ++cal_id)
    {
        for (slot_index = 0; slot_index < group_nof_slots; ++slot_index)
        {
            SHR_IF_ERR_EXIT(dnx_algo_flexe_general_db.demux_slot_info.
                            cal_slots.get(unit, group_index, cal_id, slot_index, &calendar_slots[slot_index]));
        }
        /*
         * Adjust Rx time slots
         */
        for (slot_index = first_slot; slot_index < (group_nof_slots - port_nof_slots); ++slot_index)
        {
            SHR_IF_ERR_EXIT(dnx_algo_flexe_general_db.demux_slot_info.
                            cal_slots.set(unit, group_index, cal_id, slot_index,
                                          calendar_slots[slot_index + port_nof_slots]));
        }
        for (slot_index = 0; slot_index < group_nof_slots; ++slot_index)
        {
            SHR_IF_ERR_EXIT(dnx_algo_flexe_general_db.mux_slot_info.
                            cal_slots.get(unit, group_index, cal_id, slot_index, &calendar_slots[slot_index]));
        }
        /*
         * Adjust Tx time slots
         */
        for (slot_index = first_slot; slot_index < (group_nof_slots - port_nof_slots); ++slot_index)
        {
            SHR_IF_ERR_EXIT(dnx_algo_flexe_general_db.mux_slot_info.cal_slots.set(unit, group_index, cal_id, slot_index,
                                                                                  calendar_slots[slot_index +
                                                                                                 port_nof_slots]));
        }
        /*
         * Set the unused slots to "0"
         */
        for (slot_index = (group_nof_slots - port_nof_slots); slot_index < group_nof_slots; ++slot_index)
        {
            SHR_IF_ERR_EXIT(dnx_algo_flexe_general_db.demux_slot_info.
                            cal_slots.set(unit, group_index, cal_id, slot_index, 0));
            SHR_IF_ERR_EXIT(dnx_algo_flexe_general_db.mux_slot_info.
                            cal_slots.set(unit, group_index, cal_id, slot_index, 0));
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
    uint16 client_logical_port;
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
            SHR_IF_ERR_EXIT(dnx_algo_flexe_general_db.demux_slot_info.
                            cal_slots.get(unit, group_index, cal_id, slot_index, &client_logical_port));
            if (client_logical_port != 0)
            {
                *is_occupied = 1;
                SHR_EXIT();
            }
            /*
             * Check if it is occupied by Mux slots
             */
            SHR_IF_ERR_EXIT(dnx_algo_flexe_general_db.mux_slot_info.
                            cal_slots.get(unit, group_index, cal_id, slot_index, &client_logical_port));
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
    if (!SAL_BOOT_PLISIM)
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
    if (enable && !sw_state_is_warm_boot(unit))
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
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_cal_fixed_len_ratio_based_build(unit, slots_per_object, nof_objects,
                                                             max_calendar_len, max_calendar_len, calendar));
exit:
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
    for (i = 0; i < dnx_data_nif.framer.nof_pms_per_framer_get(unit); ++i)
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

    if (!dnx_data_nif.framer.feature_get(unit, dnx_data_nif_framer_is_supported))
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
 * See .h file
 */
shr_error_e
dnx_algo_flexe_core_port_remove(
    int unit,
    bcm_port_t port)
{
    int speed, instance_id, flexe_core_port, nof_instances;
    dnx_algo_port_info_s port_info;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_speed_get(unit, port, 0, &speed));
    SHR_IF_ERR_EXIT(dnx_algo_flexe_phy_to_core_port_get(unit, port, &flexe_core_port));
    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));
    /*
     * Free FlexE core port
     */
    if (dnx_data_nif.flexe.feature_get(unit, dnx_data_nif_flexe_core_port_need_alloc))
    {
        SHR_IF_ERR_EXIT(dnx_algo_flexe_core_port_free(unit, speed, flexe_core_port));
    }
    /*
     * Free FlexE instance
     */
    if (DNX_ALGO_PORT_TYPE_IS_FLEXE_PHY(unit, port_info, FALSE))
    {
        SHR_IF_ERR_EXIT(dnx_algo_flexe_core_port_nof_instances_get(unit, flexe_core_port, &nof_instances));
        SHR_IF_ERR_EXIT(dnx_algo_flexe_core_port_to_instance_id_get(unit, flexe_core_port, &instance_id));
        SHR_IF_ERR_EXIT(dnx_algo_flexe_instance_id_free(unit, instance_id, nof_instances));
    }
    /*
     * Clear the DB.
     */
    SHR_IF_ERR_EXIT(dnx_algo_flexe_phy_to_core_port_set(unit, port, DNX_ALGO_PORT_INVALID));
    SHR_IF_ERR_EXIT(dnx_algo_flexe_core_port_db_init(unit, flexe_core_port));

exit:
    SHR_FUNC_EXIT;
}
/*
 * See .h file
 */
shr_error_e
dnx_algo_flexe_core_port_add(
    int unit,
    bcm_port_t port,
    int speed,
    int instance_id)
{
    int is_with_id, flexe_core_port_tmp, instance_id_tmp, nof_instances;
    dnx_algo_port_info_s port_info;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));

    is_with_id = (instance_id == DNX_ALGO_PORT_INVALID) ? 0 : 1;
    instance_id_tmp = instance_id;
    /*
     * Allocate FlexE core port.
     */
    if (dnx_data_nif.flexe.feature_get(unit, dnx_data_nif_flexe_core_port_need_alloc))
    {
        flexe_core_port_tmp = instance_id;
        SHR_IF_ERR_EXIT(dnx_algo_flexe_core_port_alloc(unit, speed, is_with_id, &flexe_core_port_tmp));
        /*
         * FlexE core port is same with instance_id, use flexe_core_port to allocate instance_id
         */
        instance_id_tmp = flexe_core_port_tmp;
        is_with_id = 1;
    }
    else
    {
        /*
         * FlexE core port = first phy
         */
        SHR_IF_ERR_EXIT(dnx_algo_port_nif_first_phy_get(unit, port, 0, &flexe_core_port_tmp));
    }
    /*
     * Allocate instance ID.
     */
    if (DNX_ALGO_PORT_TYPE_IS_FLEXE_PHY(unit, port_info, FALSE))
    {
        SHR_IF_ERR_EXIT(dnx_algo_flexe_instance_id_alloc(unit, speed, is_with_id, &instance_id_tmp, &nof_instances));
        SHR_IF_ERR_EXIT(dnx_algo_flexe_core_port_to_instance_id_set(unit, flexe_core_port_tmp, instance_id_tmp));
        SHR_IF_ERR_EXIT(dnx_algo_flexe_core_port_nof_instances_set(unit, flexe_core_port_tmp, nof_instances));
    }
    /*
     * Update DB for FlexE PHY and ETH L1.
     */
    SHR_IF_ERR_EXIT(dnx_algo_flexe_phy_to_core_port_set(unit, port, flexe_core_port_tmp));
    SHR_IF_ERR_EXIT(dnx_algo_flexe_core_port_speed_set(unit, flexe_core_port_tmp, speed));
exit:
    SHR_FUNC_EXIT;
}
/*
 * See .h file
 */
shr_error_e
dnx_algo_flexe_group_slot_mode_set(
    int unit,
    int group_index,
    bcm_port_flexe_phy_slot_mode_t slot_mode)
{
    SHR_FUNC_INIT_VARS(unit);

    /** verify group range */
    if (group_index < 0 || group_index >= dnx_data_nif.flexe.nof_groups_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "FlexE group index is out of range.\n");
    }
    SHR_IF_ERR_EXIT(dnx_algo_flexe_general_db.group_info.slot_mode.set(unit, group_index, slot_mode));

exit:
    SHR_FUNC_EXIT;
}
/*
 * See .h file
 */
shr_error_e
dnx_algo_flexe_group_slot_mode_get(
    int unit,
    int group_index,
    bcm_port_flexe_phy_slot_mode_t * slot_mode)
{
    SHR_FUNC_INIT_VARS(unit);

    /** verify group range */
    if (group_index < 0 || group_index >= dnx_data_nif.flexe.nof_groups_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "FlexE group index is out of range.\n");
    }
    SHR_IF_ERR_EXIT(dnx_algo_flexe_general_db.group_info.slot_mode.get(unit, group_index, slot_mode));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_flexe_phy_to_core_port_set(
    int unit,
    bcm_port_t phy_port,
    int flexe_core_port)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_flexe_core_port_set(unit, phy_port, flexe_core_port));
exit:
    SHR_FUNC_EXIT;
}
/*
 * See .h file
 */
shr_error_e
dnx_algo_flexe_phy_to_core_port_get(
    int unit,
    bcm_port_t phy_port,
    int *flexe_core_port)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_flexe_core_port_get(unit, phy_port, flexe_core_port));
exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_flexe_phy_logical_phy_id_set(
    int unit,
    bcm_port_t phy_port,
    int logical_phy_id)
{
    int flexe_core_port;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_flexe_phy_to_core_port_get(unit, phy_port, &flexe_core_port));
    SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.port.logical_phy_id.set(unit, flexe_core_port, logical_phy_id));
exit:
    SHR_FUNC_EXIT;
}
/*
 * See .h file
 */
shr_error_e
dnx_algo_flexe_phy_logical_phy_id_get(
    int unit,
    bcm_port_t phy_port,
    int *logical_phy_id)
{
    int flexe_core_port;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_flexe_phy_to_core_port_get(unit, phy_port, &flexe_core_port));
    SHR_IF_ERR_EXIT(dnx_algo_flexe_core_db.port.logical_phy_id.get(unit, flexe_core_port, logical_phy_id));

exit:
    SHR_FUNC_EXIT;
}
/*
 * See .h file
 */
shr_error_e
dnx_algo_framer_mgmt_channel_add(
    int unit,
    int framer_channel,
    dnx_algo_port_info_s port_info)
{
    int is_busa;

    SHR_FUNC_INIT_VARS(unit);

    if (framer_channel < 0 || framer_channel >= dnx_data_nif.framer.nof_mgmt_channels_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Framer channel is out of range.\n");
    }
    SHR_IF_ERR_EXIT(dnx_algo_flexe_general_db.mgmt_channel_info.valid.set(unit, framer_channel, 1));
    is_busa = DNX_ALGO_PORT_TYPE_IS_NIF_ETH_L1(unit, port_info, TRUE) ? 1 : 0;
    SHR_IF_ERR_EXIT(dnx_algo_flexe_general_db.mgmt_channel_info.is_busa.set(unit, framer_channel, is_busa));

exit:
    SHR_FUNC_EXIT;
}
/*
 * See .h file
 */
shr_error_e
dnx_algo_framer_mgmt_channel_db_init(
    int unit,
    int channel)
{
    dnx_algo_flexe_general_mgmt_channel_info_t mgmt_channel_db;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&mgmt_channel_db, 0, sizeof(dnx_algo_flexe_general_mgmt_channel_info_t));
    SHR_IF_ERR_EXIT(dnx_algo_flexe_general_db.mgmt_channel_info.set(unit, channel, &mgmt_channel_db));

exit:
    SHR_FUNC_EXIT;
}
/*
 * See .h file
 */
shr_error_e
dnx_algo_framer_mac_tx_init_credit_value_get(
    int unit,
    int speed,
    int *init_credit)
{
    uint32 tinymac_clk_in_mhz, framer_clk_in_mhz;
    uint32 nof_cal_slots, cal_granularity;
    uint32 async_fifo_delay_cycles, pipe_delay_cycles;
    uint32 slot_distance;
    uint32 min_delta_2_credit;
    uint32 min_delta_1_credit;
    uint32 max_delta_3_credit;
    uint32 async_fifo_delay;
    uint32 pipe_delay;
    uint32 max_rtt_2_credit;

    SHR_FUNC_INIT_VARS(unit);

    if ((speed >= 1000) && (speed <= 5000))
    {
        /*
         * For speed 1G to 5G, the init credit is a fixed value
         */
        *init_credit = dnx_data_nif.mac_client.sb_tx_init_credit_for_5g_get(unit);
    }
    else
    {
        /*
         *                      2 * 2nd_credit_RTT(Round Trip Time)
         * Init Credit = ------------------------------------------------
         *                         2 * MIN framer TDM distance
         */
        tinymac_clk_in_mhz = dnx_data_nif.mac_client.tinymac_clock_mhz_get(unit);
        framer_clk_in_mhz = dnx_data_nif.framer.clock_mhz_get(unit);
        /*
         * Get the calendar slot distance
         */
        nof_cal_slots = dnx_data_nif.mac_client.nof_sb_cal_slots_get(unit);
        cal_granularity = dnx_data_nif.mac_client.sb_cal_granularity_get(unit);
        /*
         * Multiplied by 10000 to increase the accuracy
         */
        slot_distance = (nof_cal_slots * 10000) / (speed / cal_granularity);
        /*
         * Min delta between two credits in NS
         */
        DNX_ALGO_FRAMER_TX_INIT_CREDIT_CAL(min_delta_2_credit, slot_distance, MIN_DELTA_2_CREDIT_IN_FRAMER,
                                           framer_clk_in_mhz);
        /*
         * Async fifo delay in NS
         */
        async_fifo_delay_cycles = dnx_data_nif.mac_client.async_fifo_delay_cycles_get(unit);
        DNX_ALGO_FRAMER_TX_INIT_CREDIT_CAL(async_fifo_delay, async_fifo_delay_cycles, (1000 * 10000),
                                           framer_clk_in_mhz);
        /*
         * Pipe delay in NS
         */
        pipe_delay_cycles = dnx_data_nif.mac_client.pipe_delay_cycles_get(unit);
        DNX_ALGO_FRAMER_TX_INIT_CREDIT_CAL(pipe_delay, pipe_delay_cycles, (1000 * 10000), tinymac_clk_in_mhz);
        /*
         * Max delta among 3 credits in NS
         */
        DNX_ALGO_FRAMER_TX_INIT_CREDIT_CAL(max_delta_3_credit, slot_distance, MAX_DELTA_3_CREDIT_IN_WRAPPER,
                                           tinymac_clk_in_mhz);
        /*
         * Min 1 credit delay in NS
         */
        DNX_ALGO_FRAMER_TX_INIT_CREDIT_CAL(min_delta_1_credit, slot_distance, MIN_DELTA_1_CREDIT_IN_FRAMER,
                                           framer_clk_in_mhz);
        /*
         * MAX RTT(Round Trip Time) for 2 credits
         */
        max_rtt_2_credit = max_delta_3_credit - min_delta_1_credit + pipe_delay + async_fifo_delay;
        /*
         * init_credit = Roundup(2 * max_rtt_2_credit_int / min_delta_2_credit)
         */
        *init_credit = UTILEX_DIV_ROUND_UP(2 * max_rtt_2_credit, min_delta_2_credit);
    }

    SHR_FUNC_EXIT;
}
/*
 * See .h file
 */
shr_error_e
dnx_algo_flexe_pcs_port_fifo_size_get(
    int unit,
    int speed,
    int *fifo_size)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (speed)
    {
        case 100000:
        {
            *fifo_size = 1;
            break;
        }
        case 200000:
        {
            *fifo_size = 2;
            break;
        }
        case 400000:
        {
            *fifo_size = 3;
            break;
        }
        default:
        {
            *fifo_size = 0;
        }
    }
    SHR_FUNC_EXIT;
}
/*
 * See .h file
 */
shr_error_e
dnx_algo_framer_66bsw_block_id_get(
    int unit,
    dnx_algo_port_type_e port_type,
    int is_backup,
    int *block_id)
{
    SHR_FUNC_INIT_VARS(unit);

    if (is_backup == 0)
    {
        *block_id = dnx_data_nif.framer.b66switch_get(unit, port_type)->block_id;
    }
    else
    {
        *block_id = dnx_data_nif.framer.b66switch_get(unit, port_type)->backup_block_id;
    }

    SHR_FUNC_EXIT;
}

/*
 * }
 */

#undef _ERR_MSG_MODULE_NAME
