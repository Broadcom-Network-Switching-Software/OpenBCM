/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

/** \file algo_flexe_general.h
 *
 *  Handle the flexe general functions.
 *    * group info
 *    * resource manager
 *
 */

#ifndef ALGO_FLEXE_GENERAL_H_INCLUDED
/*
 * {
 */
#define ALGO_FLEXE_GENERAL_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX family only!"
#endif

/*
 * Includes.
 * {
 */
#include <bcm_int/dnx/algo/swstate/auto_generated/access/dnx_algo_flexe_general_access.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_nif.h>

/*
 * }
 */
/*
 * Macros.
 * {
 */
#define DNX_ALGO_FLEXE_IS_ENABLED(unit)    (dnx_data_nif.framer.feature_get(unit, dnx_data_nif_framer_is_supported) &&               \
                                            ((dnx_data_nif.flexe.feature_get(unit, dnx_data_nif_flexe_multi_device_mode_support) &&   \
                                               dnx_data_nif.flexe.flexe_mode_get(unit) != DNX_FLEXE_MODE_DISABLED) ||                 \
                                              (!dnx_data_nif.flexe.feature_get(unit, dnx_data_nif_flexe_multi_device_mode_support))))

#define DNX_ALGO_FLEXE_CLIENT_CHANNEL_TO_CORE_CHANNEL_GET(unit, channel, port_info) \
    (DNX_ALGO_PORT_TYPE_IS_FLEXE_CLIENT(unit ,port_info) ? channel : \
     (DNX_ALGO_PORT_TYPE_IS_FRAMER_MAC(unit, port_info, TRUE, TRUE, TRUE) ? \
      (channel + dnx_data_nif.mac_client.channel_base_get(unit)) : \
      (channel + dnx_data_nif.sar_client.channel_base_get(unit))))
#define DNX_ALGO_FLEXE_MAC_CHANNEL_IS_PTP(unit, channel) \
    (dnx_data_nif.mac_client.nof_special_clients_get(unit) > 0 && \
    channel == dnx_data_nif.mac_client.ptp_client_channel_get(unit))
#define DNX_ALGO_FLEXE_MAC_CHANNEL_IS_OAM(unit, channel) \
    (dnx_data_nif.mac_client.nof_special_clients_get(unit) > 0 && \
    channel == dnx_data_nif.mac_client.oam_client_channel_get(unit))
#define DNX_ALGO_FLEXE_MAC_CHANNEL_IS_PTP_OR_OAM(unit, channel) \
    (DNX_ALGO_FLEXE_MAC_CHANNEL_IS_PTP(unit, channel) || DNX_ALGO_FLEXE_MAC_CHANNEL_IS_OAM(unit, channel))

#define DNX_ALGO_PORT_USE_FLEXE_CLIENT_CHANNEL(unit, port_info) \
    (DNX_ALGO_PORT_TYPE_IS_FLEXE_CLIENT(unit, port_info) || \
     DNX_ALGO_PORT_TYPE_IS_FLEXE_UNAWARE(unit, port_info) || \
     (DNX_ALGO_PORT_TYPE_IS_NIF_ETH_L1(unit, port_info, TRUE) && \
      dnx_data_nif.framer.feature_get(unit, dnx_data_nif_framer_l1_eth_supported)))
/*
 * FlexE core generation ID
 */
#define DNX_ALGO_FLEXE_GEN_1                       (0x1)
#define DNX_ALGO_FLEXE_GEN_2                       (0x2)
/*
 * }
 */
/*
 * TypeDefs.
 * {
 */

typedef struct dnx_algo_flexe_client_ts_config_s
{
    int nof_flexe_core_ports;
    /*
     * FlexE core ports occupied by the client
     */
    int flexe_core_port_array[DNX_DATA_MAX_NIF_FLEXE_NOF_PCS];
    /*
     * time slots
     */
    bcm_port_flexe_time_slot_t ts_mask;
} dnx_algo_flexe_client_ts_config_t;

/**
 * \brief -  FlexE flow types
 */
typedef enum
{
    DNX_ALGO_FLEXE_FLOW_CLIENT2CLENT = 0,
    DNX_ALGO_FLEXE_FLOW_CLIENT2MAC,
    DNX_ALGO_FLEXE_FLOW_CLIENT2SAR,
    DNX_ALGO_FLEXE_FLOW_MAC2CLIENT,
    DNX_ALGO_FLEXE_FLOW_MAC2MAC,
    DNX_ALGO_FLEXE_FLOW_MAC2SAR,
    DNX_ALGO_FLEXE_FLOW_SAR2SAR,
    DNX_ALGO_FLEXE_FLOW_MAC2ETH,
    DNX_ALGO_FLEXE_FLOW_MAC2ILKN,
    DNX_ALGO_FLEXE_FLOW_SAR2CLIENT,
    DNX_ALGO_FLEXE_FLOW_SAR2MAC,
    DNX_ALGO_FLEXE_FLOW_SAR2ILKN,
    DNX_ALGO_FLEXE_FLOW_ETH2MAC,
    DNX_ALGO_FLEXE_FLOW_ILKN2SAR,
    DNX_ALGO_FLEXE_FLOW_ETH2SAR,
    DNX_ALGO_FLEXE_FLOW_SAR2ETH,
    DNX_ALGO_FLEXE_FLOW_ETH2CLIENT,
    DNX_ALGO_FLEXE_FLOW_CLIENT2ETH,
    DNX_ALGO_FLEXE_FLOW_ETH2ETH,
    DNX_ALGO_FLEXE_NOF_FLOW_TYPES
} dnx_algo_flexe_flow_type_e;
/*
 * }
 */

/*
 * DEFINES
 * {
 */

/*
 * }
 */

/*
 * Module Init / Deinit 
 * { 
 */
/**
 * \brief - 
 * Init algo flexe general module for cool boot mode.
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
shr_error_e dnx_algo_flexe_general_init(
    int unit);

/**
 * \brief - 
 * Init FlexE group info DB for each group.
 *
 * \param [in] unit - Unit #.
 * \param [in] group_index - FlexE group index.
 * 
 * \return
 *   see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_flexe_group_info_db_init(
    int unit,
    int group_index);

/**
 * \brief - 
 * Verify if the FlexE group is valid.
 *
 * \param [in] unit - Unit #.
 * \param [in] group_index - FlexE group index.
 * \param [in] support_bypass - If bypasss mode should be supported.
 * 
 * \return
 *   see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_flexe_group_valid_verify(
    int unit,
    int group_index,
    int support_bypass);

/**
 * \brief - 
 * Verify if the FlexE group is free.
 *
 * \param [in] unit - Unit #.
 * \param [in] group_index - FlexE group index.
 * 
 * \return
 *   see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_flexe_group_free_verify(
    int unit,
    int group_index);

/**
 * \brief - 
 * Verify the FlexE group port. Only FlexE group gport is supported
 *
 * \param [in] unit - Unit #.
 * \param [in] gport - FlexE group gport.
 * 
 * \return
 *   see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_flexe_group_port_verify(
    int unit,
    bcm_gport_t gport);

/**
 * \brief - 
 * Verify the logical PHY ID when modifying logical PHY ID.
 * The new logical PHY ID should not break the original
 * logical PHY order
 *
 * \param [in] unit - Unit #.
 * \param [in] physical_port - Logical port number for FlexE phy port.
 * \param [in] new_logical_phy_id - New logical PHY ID.
 * 
 * \return
 *   see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_flexe_logical_phy_id_modify_verify(
    int unit,
    bcm_port_t physical_port,
    int new_logical_phy_id);

/**
 * \brief - 
 * Verify if the FlexE group ID is occupied.
 *
 * \param [in] unit - Unit #.
 * \param [in] group_id - FlexE group ID, 1 ~ 0xffffe.
 * 
 * \return
 *   see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_flexe_group_id_free_verify(
    int unit,
    int group_id);

/**
 * \brief - 
 * Verify FlexE physical port.
 *
 * \param [in] unit - Unit #.
 * \param [in] physical_port - Logical port number for FlexE phy port.
 * \param [in] support_bypass - If bypasss mode should be supported.
 * 
 * \return
 *   see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_flexe_physical_port_verify(
    int unit,
    bcm_port_t physical_port,
    int support_bypass);

/**
 * \brief - 
 * Verify FlexE BusA client.
 *
 * \param [in] unit - Unit #.
 * \param [in] logical_port - Logical port number for FlexE client.
 * \param [in] support_bypass - If bypasss mode should be supported.
 * 
 * \return
 *   see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_flexe_client_verify(
    int unit,
    bcm_port_t logical_port,
    int support_bypass);

/**
 * \brief - 
 * Verify FlexE BusC client.
 *
 * \param [in] unit - Unit #.
 * \param [in] logical_port - Logical port number for FlexE SAR client.
 * 
 * \return
 *   see shr_error_e
 * \remark
 * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_flexe_sar_client_verify(
    int unit,
    bcm_port_t logical_port);

/**
 * \brief - 
 * Verify the logical PHY ID range the FlexE physical port speed.
 *
 * \param [in] unit - Unit #.
 * \param [in] speed - Speed for physical port.
 * \param [in] logical_port_id - Logical PHY ID.
 * 
 * \return
 *   see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_flexe_logical_phy_id_range_and_speed_verify(
    int unit,
    int speed,
    int logical_port_id);

/**
 * \brief - 
 * Verify FlexE channel ID.
 *
 * \param [in] unit - Unit #.
 * \param [in] port_info - basic info about port.
 * \param [in] client_channel - channel ID.
 * \param [in] mgmt_channel - mgmt channel ID.
 * 
 * \return
 *   see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_flexe_channel_verify(
    int unit,
    dnx_algo_port_info_s port_info,
    int client_channel,
    int mgmt_channel);

/**
 * \brief - 
 * Verify FlexE client resources.
 *
 * \param [in] unit - Unit #.
 * \param [in] resource - FlexE client resources.
 * 
 * \return
 *   see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_flexe_resource_validate(
    int unit,
    bcm_port_resource_t * resource);

/**
 * \brief - 
 * Verify FlexE PHY port resources.
 *
 * \param [in] unit - Unit #.
 * \param [in] port_info - port info.
 * \param [in] resource - FlexE PHY port resources.
 * 
 * \return
 *   see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_flexe_phy_resource_validate(
    int unit,
    dnx_algo_port_info_s * port_info,
    bcm_port_resource_t * resource);
/**
 * \brief - 
 * Verify Framer mgmt port resources.
 *
 * \param [in] unit - Unit #.
 * \param [in] resource - FlexE PHY port resources.
 * 
 * \return
 *   see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_framer_mgmt_resource_validate(
    int unit,
    bcm_port_resource_t * resource);
/**
 * \brief - 
 * Configure the FlexE group to valid or invalid.
 *
 * \param [in] unit - Unit #.
 * \param [in] group_index - FlexE group index.
 * \param [in] valid - Valid or invalid.
 * 
 * \return
 *   see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_flexe_group_valid_set(
    int unit,
    int group_index,
    int valid);
/**
 * \brief - 
 * Get FlexE group valid status.
 *
 * \param [in] unit - Unit #.
 * \param [in] group_index - FlexE group index.
 * \param [out] valid - Valid or invalid.
 * 
 * \return
 *   see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_flexe_group_valid_get(
    int unit,
    int group_index,
    int *valid);
/**
 * \brief - 
 * Configure if the group is bypassed or not.
 *
 * \param [in] unit - Unit #.
 * \param [in] group_index - FlexE group index.
 * \param [in] is_bypass - bypass or non-bypass.
 * 
 * \return
 *   see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_flexe_group_is_bypass_set(
    int unit,
    int group_index,
    int is_bypass);

/**
 * \brief - 
 * Configure if the group is bypassed or not.
 *
 * \param [in] unit - Unit #.
 * \param [in] group_index - FlexE group index.
 * \param [out] is_bypass - bypass or non-bypass.
 * 
 * \return
 *   see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_flexe_group_is_bypass_get(
    int unit,
    int group_index,
    int *is_bypass);

/**
 * \brief - 
 * Get all the phy ports in the FlexE group.
 *
 * \param [in] unit - Unit #.
 * \param [in] group_index - FlexE group index.
 * \param [out] phy_ports - Bitmap for PHY ports.
 * 
 * \return
 *   see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_flexe_group_phy_ports_get(
    int unit,
    int group_index,
    bcm_pbmp_t * phy_ports);

/**
 * \brief - 
 * Configure phy ports bitmap info SW DB.
 *
 * \param [in] unit - Unit #.
 * \param [in] group_index - FlexE group index.
 * \param [in] phy_ports - Bitmap for PHY ports.
 * 
 * \return
 *   see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_flexe_group_phy_ports_set(
    int unit,
    int group_index,
    bcm_pbmp_t * phy_ports);

/**
 * \brief - 
 * Get FlexE group ID from group index.
 *
 * \param [in] unit - Unit #.
 * \param [in] group_index - FlexE group index.
 * \param [out] group_id - group ID.
 * 
 * \return
 *   see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_flexe_group_index_to_id_get(
    int unit,
    int group_index,
    int *group_id);

/**
 * \brief - 
 * Get FlexE group ID from group index.
 *
 * \param [in] unit - Unit #.
 * \param [in] group_index - FlexE group index.
 * \param [in] group_id - group ID.
 * 
 * \return
 *   see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_flexe_group_index_to_id_set(
    int unit,
    int group_index,
    int group_id);

/**
 * \brief - 
 * Get phy ports bitmap from all the flexe groups.
 *
 * \param [in] unit - Unit #.
 * \param [out] phy_ports - Phy ports bitmap.
 * 
 * \return
 *   see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_flexe_groups_phy_ports_get(
    int unit,
    bcm_pbmp_t * phy_ports);

/**
 * \brief - 
 * Get the FlexE group PHY speed.
 *
 * \param [in] unit - Unit #.
 * \param [in] group_index - FlexE group index.
 * \param [out] phy_speed - FlexE group PHY speed.
 * 
 * \return
 *   see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_flexe_group_phy_speed_get(
    int unit,
    int group_index,
    int *phy_speed);

/**
 * \brief - 
 * Free flexe core ports.
 *
 * \param [in] unit - Unit #.
 * \param [in] speed - port speed.
 * \param [in] flexe_core_port - FlexE core port ID.
 *
 * \return  
 *   see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_flexe_core_port_free(
    int unit,
    int speed,
    int flexe_core_port);

/**
 * \brief - 
 * Get speed for FlexE group
 *
 * \param [in] unit - Unit #.
 * \param [in] group_index - FlexE group index.
 * \param [out] group_speed - FlexE group speed.
 *
 * \return  
 *   see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_flexe_group_speed_get(
    int unit,
    int group_index,
    int *group_speed);

/**
 * \brief - 
 * Verify if the new speed exceeds the chip capability
 *
 * \param [in] unit - Unit #.
 * \param [in] slot_mode - the FlexE group slot mode
 * \param [in] speed - speed for new member.
 *
 * \return  
 *   see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_flexe_group_speed_verify(
    int unit,
    bcm_port_flexe_phy_slot_mode_t slot_mode,
    int speed);
/**
 * \brief - 
 *     Allocate FlexE link list FIFO and store the info
 *     In SW DB.
 *
 * \param [in] unit - Unit #.
 * \param [in] fifo_index - index for the link list fifo.
 * \param [in] nof_entries - Number of required entries.
 * \param [in] type - FlexE Link list fifo type.
 * \param [out] fifo_entries - Fifo entries bitmap.
 * 
 * \return  
 *   see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_flexe_fifo_link_list_alloc(
    int unit,
    int fifo_index,
    int nof_entries,
    dnx_algo_flexe_link_list_fifo_type_e type,
    bcm_pbmp_t * fifo_entries);

/**
 * \brief - 
 *     Get FlexE link list FIFO entries for given port
 *
 * \param [in] unit - Unit #.
 * \param [in] fifo_index - Index for the link list fifo.
 * \param [in] type - FlexE Link list fifo type.
 * \param [out] fifo_entries - Fifo entries bitmap.
 * 
 * \return  
 *   see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_flexe_fifo_link_list_get(
    int unit,
    int fifo_index,
    dnx_algo_flexe_link_list_fifo_type_e type,
    bcm_pbmp_t * fifo_entries);

/**
 * \brief - 
 *     Free FlexE link list FIFO entries in SW DB
 *
 * \param [in] unit - Unit #.
 * \param [in] fifo_index - index for the link list fifo.
 * \param [in] nof_entries - Number of entries to free.
 * \param [in] type - FlexE Link list fifo type.
 * \param [out] fifo_entries - Fifo entries bitmap.
 * 
 * \return  
 *   see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_flexe_fifo_link_list_free(
    int unit,
    int fifo_index,
    int nof_entries,
    dnx_algo_flexe_link_list_fifo_type_e type,
    bcm_pbmp_t * fifo_entries);

/**
 * \brief - 
 *     Allocate RMC ID using allocation manager
 *
 * \param [in] unit - Unit #.
 * \param [out] rmc_id - RMC ID.
 * 
 * \return  
 *   see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_flexe_rmc_id_alloc(
    int unit,
    int *rmc_id);

/**
 * \brief - 
 *     Free RMC ID using allocation manager
 *
 * \param [in] unit - Unit #.
 * \param [out] rmc_id - RMC ID.
 * 
 * \return  
 *   see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_flexe_rmc_id_free(
    int unit,
    int rmc_id);

/**
 * \brief - 
 *     Allocate client channel using allocation manager
 *
 * \param [in] unit - Unit #.
 * \param [in] port_info - FlexE client port info.
 * \param [out] channel - channel ID.
 * 
 * \return  
 *   see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_flexe_client_channel_alloc(
    int unit,
    dnx_algo_port_info_s port_info,
    int *channel);

/**
 * \brief - 
 *     Free client channel using allocation manager
 *
 * \param [in] unit - Unit #.
 * \param [in] port_info - FlexE client port info.
 * \param [in] channel - channel ID.
 * 
 * \return  
 *   see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_flexe_client_channel_free(
    int unit,
    dnx_algo_port_info_s port_info,
    int channel);
/**
 * \brief -
 *     Reserve one channel in shared resources
 *
 * \param [in] unit - Unit #.
 * \param [in] module_id - Framer module ID.
 * \param [out] channel - channel ID.
 *
 * \return
 *   see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_flexe_shared_channel_alloc(
    int unit,
    bcm_port_framer_module_t module_id,
    int *channel);
/**
 * \brief -
 *     Free client channel using allocation manager
 *
 * \param [in] unit - Unit #.
 * \param [in] module_id - Framer module ID.
 * \param [in] channel - channel ID.
 *
 * \return
 *   see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_flexe_shared_channel_free(
    int unit,
    bcm_port_framer_module_t module_id,
    int channel);

/**
 * \brief - 
 * Verify if the client channel to client ID map is consistent.
 *
 * \param [in] unit - Unit #.
 * \param [in] group_index - FlexE group index.
 * \param [in] calendar_id - calendar ID.
 * \param [in] nof_slots - nof slots.
 * \param [in] calendar_slots - calendar slots array, value is client id.
 *
 * \return  
 *   see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_flexe_channel_to_client_id_map_verify(
    int unit,
    int group_index,
    bcm_port_flexe_group_cal_t calendar_id,
    int nof_slots,
    int *calendar_slots);

/**
 * \brief - 
 * Configure the slots in DB
 *
 * \param [in] unit - Unit #.
 * \param [in] group_index - FlexE group index.
 * \param [in] flags - Rx or Tx.
 * \param [in] calendar_id - calendar ID.
 * \param [in] slot_index - slot index in FlexE core port.
 * \param [in] logical_port - FlexE client logical port.
 *
 * \return  
 *   see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_flexe_group_cal_slot_set(
    int unit,
    int group_index,
    uint32 flags,
    bcm_port_flexe_group_cal_t calendar_id,
    int slot_index,
    bcm_port_t logical_port);

/**
 * \brief - 
 * Get the slots in DB
 *
 * \param [in] unit - Unit #.
 * \param [in] group_index - FlexE group index.
 * \param [in] flags - Rx or Tx.
 * \param [in] calendar_id - calendar ID.
 * \param [in] slot_index - slot index in FlexE core port.
 * \param [out] logical_port - FlexE client logical port.
 *
 * \return  
 *   see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_flexe_group_cal_slot_get(
    int unit,
    int group_index,
    uint32 flags,
    bcm_port_flexe_group_cal_t calendar_id,
    int slot_index,
    bcm_port_t * logical_port);

/**
 * \brief - 
 * Get the FlexE core ports bitmap in group
 *
 * \param [in] unit - Unit #.
 * \param [in] group_index - FlexE group index.
 * \param [out] flexe_core_ports - FlexE core port bitmap.
 *
 * \return  
 *   see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_flexe_core_ports_get(
    int unit,
    int group_index,
    bcm_pbmp_t * flexe_core_ports);

/**
 * \brief - 
 * Get the FlexE physical ports array sorted by logical PHY ID
 *
 * \param [in] unit - Unit #.
 * \param [in] group_index - FlexE group index.
 * \param [out] phy_ports_arr - FlexE physical ports array.
 * \param [out] nof_ports - Number of phy ports in the group.
 *
 * \return  
 *   see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_flexe_phy_ports_sort_get(
    int unit,
    int group_index,
    bcm_port_t * phy_ports_arr,
    int *nof_ports);

/**
 * \brief - 
 * Get the FlexE core ports array sorted by logical PHY ID
 *
 * \param [in] unit - Unit #.
 * \param [in] group_index - FlexE group index.
 * \param [out] flexe_core_ports_arr - FlexE core ports array.
 * \param [out] logical_phy_id_arr - logical PHY ID array.
 * \param [out] nof_ports - nof phy ports in the group.
 *
 * \return  
 *   see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_flexe_core_ports_sort_get(
    int unit,
    int group_index,
    bcm_port_t * flexe_core_ports_arr,
    int *logical_phy_id_arr,
    int *nof_ports);

/**
 * \brief - 
 * Build the FlexE client time slots for each client
 *
 * \param [in] unit - Unit #.
 * \param [in] group_index - FlexE group index.
 * \param [in] nof_slots - nof slots.
 * \param [in] calendar_slots - calendar slots array.
 * \param [in] nof_clients - nof clients in the group.
 * \param [out] ts_config_db - Time slots db for each client.
 *
 * \return  
 *   see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_flexe_client_time_slots_db_build(
    int unit,
    int group_index,
    int nof_slots,
    int *calendar_slots,
    int nof_clients,
    dnx_algo_flexe_client_ts_config_t * ts_config_db);

/**
 * \brief - 
 * Adjust the time slots when removing one physical port
 * from the group
 *
 * \param [in] unit - Unit #.
 * \param [in] group_index - FlexE group index.
 * \param [in] removed_port - physical port to be removed.
 *
 * \return  
 *   see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_flexe_calendar_slots_adjust(
    int unit,
    int group_index,
    bcm_port_t removed_port);

/**
 * \brief - Check if the physical port is occupied by
 *    time slots
 *
 * \param [in] unit - chip unit id
 * \param [in] group_index - group index
 * \param [in] logical_port - flexe phy port
 * \param [out] is_occupied - if the port is active
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_port_flexe_phy_port_is_occupied_get(
    int unit,
    int group_index,
    bcm_port_t logical_port,
    int *is_occupied);

/**
 * \brief - Get if the FlexE core is active
 *
 * \param [in] unit - chip unit id
 * \param [out] is_active - if flexe core is active
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_flexe_core_active_get(
    int unit,
    uint8 *is_active);

/**
 * \brief - set FlexE core active value
 *
 * \param [in] unit - chip unit id
 * \param [in] is_active - active or inactive
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_flexe_core_active_set(
    int unit,
    uint8 is_active);
/**
 * \brief - enable/disable flexe core blocks access, because flexe core need serdes pll configure
 *
 * \param [in] unit - unit number
 * \param [in] enable - enable or disable
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_flexe_core_blocks_enable_set(
    int unit,
    uint8 enable);

/**
 * \brief - 
 * Build unifrom calendar for the following calendar:
 *    NB TDM calendar
 *    SB Rx calendar
 *    TMC calendar
 *    RMC calendar
 *
 * \param [in] unit - Unit #.
 * \param [in] slots_per_object - Number of slots for each object.
 * \param [in] nof_objects - Array size for slots_per_object.
 * \param [in] max_calendar_len - Max calendar length.
 * \param [out] calendar - Calendar value for each slots.
 * 
 * \return
 *   see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_flexe_general_cal_build(
    int unit,
    uint32 *slots_per_object,
    int nof_objects,
    int max_calendar_len,
    uint32 *calendar);

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
shr_error_e dnx_algo_flexe_physical_port_to_group_index_get(
    int unit,
    int physical_port,
    int *group_index);

/**
 * \brief - Get FlexE core calendar slots align status
 *
 * \param [in] unit - unit number
 * \param [in] group_index - group_index
 * \param [out] is_unaligned - if aligned
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_flexe_cal_slots_is_unaligned_get(
    int unit,
    int group_index,
    uint8 *is_unaligned);

/**
 * \brief - Configure FlexE core calendar slots align status
 *
 * \param [in] unit - unit number
 * \param [in] group_index - group_index
 * \param [in] is_unaligned - if aligned
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_flexe_cal_slots_is_unaligned_set(
    int unit,
    int group_index,
    uint8 is_unaligned);

/**
 * \brief - Get nof entries required for FIFO link list
 *
 * \param [in] unit - unit number
 * \param [in] type - FIFO type
 * \param [in] speed - FlexE client speed
 * \param [out] nof_entries - Nof entries
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_flexe_fifo_link_list_entries_required_get(
    int unit,
    dnx_algo_flexe_link_list_fifo_type_e type,
    int speed,
    int *nof_entries);

/**
 * \brief - 
 * Check if it is a valid flexe PM.
 *
 * \param [in] unit - Unit #.
 * \param [in] pm_id - PM index.
 * \param [out] is_pm_valid - If the PM supports flexe.
 * 
 * \return
 *   see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_flexe_is_valid_pm_get(
    int unit,
    int pm_id,
    int *is_pm_valid);

/**
 * \brief -
 * Check if FlexE is supported and in case it is, if it is enabled.
 *
 * \param [in] unit - Unit #.
 * \param [out] enabled - If FlexE is supported and enabled.
 *
 * \return
 *   see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_flexe_supported_and_enabled_get(
    int unit,
    uint8 *enabled);

/**
 * \brief - Take the FlexE Tiny MAC access lock.
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
shr_error_e dnx_algo_flexe_tiny_mac_access_lock_take(
    int unit);

/**
 * \brief - Give the FlexE Tiny MAC access lock.
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
shr_error_e dnx_algo_flexe_tiny_mac_access_lock_give(
    int unit);
/**
 * \brief - Remove FlexE core port in DB.
 *
 */
shr_error_e dnx_algo_flexe_core_port_remove(
    int unit,
    bcm_port_t port);
/**
 * \brief - Add FlexE core port in DB.
 *
 */
shr_error_e dnx_algo_flexe_core_port_add(
    int unit,
    bcm_port_t port,
    int speed,
    int instance_id,
    int am_transparent);
/*
 * \brief - Configure FlexE group slot mode.
 */
shr_error_e dnx_algo_flexe_group_slot_mode_set(
    int unit,
    int group_index,
    bcm_port_flexe_phy_slot_mode_t slot_mode);
/*
 * \brief - Get FlexE group slot mode.
 */
shr_error_e dnx_algo_flexe_group_slot_mode_get(
    int unit,
    int group_index,
    bcm_port_flexe_phy_slot_mode_t * slot_mode);
/*
 * \brief - Configure FlexE physical port to flexe core port map,
 */
shr_error_e dnx_algo_flexe_phy_to_core_port_set(
    int unit,
    bcm_port_t phy_port,
    int flexe_core_port);
/*
 * \brief - Get FlexE physical port to flexe core port map,
 */
shr_error_e dnx_algo_flexe_phy_to_core_port_get(
    int unit,
    bcm_port_t phy_port,
    int *flexe_core_port);
/*
 * \brief - Set FlexE physical port to logcial phy id map,
 */
shr_error_e dnx_algo_flexe_phy_logical_phy_id_set(
    int unit,
    bcm_port_t phy_port,
    int logical_phy_id);
/*
 * \brief - Get FlexE physical port to logcial phy id map,
 */
shr_error_e dnx_algo_flexe_phy_logical_phy_id_get(
    int unit,
    bcm_port_t phy_port,
    int *logical_phy_id);
/*
 * \brief - Add the framer mgmt channel and configure parameters
 */
shr_error_e dnx_algo_framer_mgmt_channel_add(
    int unit,
    int mgmt_channel,
    dnx_algo_port_info_s port_info);
/*
 * \brief - Init the mgmt channel DB
 */
shr_error_e dnx_algo_framer_mgmt_channel_db_init(
    int unit,
    int channel);
/*
 * \brief - Configure the Tx init credit for Wrapper B
 */
shr_error_e dnx_algo_framer_mac_tx_init_credit_value_get(
    int unit,
    int speed,
    int *init_credit);
/*
 * \brief - Get pcs port fifo size according to the port speed
 */
shr_error_e dnx_algo_flexe_pcs_port_fifo_size_get(
    int unit,
    int speed,
    int *fifo_size);
/*
 * \brief - Get framer 66bsw block ID according to the port type
 */
shr_error_e dnx_algo_framer_66bsw_block_id_get(
    int unit,
    dnx_algo_port_type_e port_type,
    int is_backup,
    int *block_id);
/*
 * \brief - Get framer inner MAC block ID according to the MAC index.
 *    mac_index: 0 - MAC1, 1 - MAC2
 */
shr_error_e dnx_algo_framer_inner_mac_block_id_get(
    int unit,
    int mac_index,
    int is_backup,
    int *block_id);
/*
 * \brief - Get the current G.hao action from DB for either
 *    rx or tx.
 */
shr_error_e dnx_algo_flexe_ghao_action_get(
    int unit,
    int group_index,
    int is_rx,
    bcm_port_flexe_group_ghao_action_t * action);
/*
 * \brief - Configure the current G.hao action to DB for
 *    rx and tx.
 */
shr_error_e dnx_algo_flexe_ghao_action_set(
    int unit,
    int group_index,
    uint32 flags,
    bcm_port_flexe_group_ghao_action_t action);
/**
 * \brief - Verify the G.hao action. Only the legal
 *    action switching is allowed.
 *
 * \param [in] unit - Unit #.
 * \param [in] group_index - FlexE group index.
 * \param [in] is_rx - The action is for Rx or Tx.
 * \param [in] active_cal - The current active calendar ID.
 * \param [in] action - The new action.
 *
 * \return
 *   see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_flexe_ghao_action_verify(
    int unit,
    int group_index,
    int is_rx,
    bcm_port_flexe_group_cal_t active_cal,
    bcm_port_flexe_group_ghao_action_t action);
/**
 * \brief - Compare the slots in FlexE calendar A and B, and then
 *    return the client channel whose speed is changed.
 *    For G.hao, only changing the speed of one client is allowed
 *    each time.
 *
 * \param [in] unit - Unit #.
 * \param [in] group_index - FlexE group index.
 * \param [in] is_rx - The action is for Rx or Tx.
 * \param [in] target_cal_id - The target calendar ID.
 * \param [out] changed_client_channel - The client channel.
 * \param [out] new_speed - New speed of the client.
 *
 * \return
 *   see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_flexe_ghao_flexe_cal_compare(
    int unit,
    int group_index,
    int is_rx,
    bcm_port_flexe_group_cal_t target_cal_id,
    int *changed_client_channel,
    int *new_speed);
/**
 * \brief - Verify if G.hao is in progress.
 *    The FlexE group configuration are not allowed to
 *    change if G.hao is in progress.
 *
 * \param [in] unit - Unit #.
 * \param [in] group_index - FlexE group index.
 * \param [in] flags - Rx or Tx.
 *
 * \return
 *   see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_flexe_ghao_active_verify(
    int unit,
    int group_index,
    uint32 flags);
/**
 * \brief - Allocate Framer 1588 MAC channel and
 *    the channels into SW DB
 *
 * \param [in] unit - Unit #.
 * \param [in] logical_port - Logical port #.
 * \param [out] mac1_channel - MAC1 channel
 * \param [out] mac2_channel - MAC2 channel
 *
 * \return
 *   see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_l1_eth_1588_mac_channel_alloc_and_set(
    int unit,
    bcm_port_t logical_port,
    int *mac1_channel,
    int *mac2_channel);
/**
 * \brief - Free Framer 1588 MAC channel and
 *    update SW DB. This API needs to check if the L1 1588 is
 *    enabled on the other direction. If it is enabled
 *    on the other direction, no need to free the channels.
 *
 * \param [in] unit - Unit #.
 * \param [in] logical_port - Logical port #.
 * \param [in] flags - rx or tx.
 * \param [in] mac1_channel - freed MAC1 channel
 * \param [in] mac2_channel - freed MAC2 channel
 *
 * \return
 *   see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_l1_eth_1588_mac_channel_remove(
    int unit,
    bcm_port_t logical_port,
    uint32 flags,
    int mac1_channel,
    int mac2_channel);
/**
 * \brief - Get the port block ID and channel for framer flow
 *
 * \param [in] unit - Unit #.
 * \param [in] logical_port - Logical port #.
 * \param [in] flags - rx or tx.
 * \param [in] is_backup - main or backup block
 * \param [out] block_id - block id
 * \param [out] channel - channel ID
 *
 * \return
 *   see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_framer_flow_port_info_get(
    int unit,
    bcm_port_t logical_port,
    uint32 flags,
    int is_backup,
    int *block_id,
    int *channel);
/**
 * \brief - Get flexe group index from the flexe client logical
 *    port ID. If the FlexE client is not added into any groups,
 *    -1 will be returned.
 *
 * \param [in] unit - Unit #.
 * \param [in] logical_port - Logical port #.
 * \param [in] flags - rx or tx.
 * \param [out] group_index - FlexE group index
 *
 * \return
 *   see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_flexe_client_to_group_index_get(
    int unit,
    bcm_port_t logical_port,
    uint32 flags,
    int *group_index);
/**
 * \brief - Configure the Demux active calendar ID in DB.
 *
 * \param [in] unit - Unit #.
 * \param [in] group_index - FlexE group index.
 * \param [in] cal_id - FlexE calendar ID.
 *
 * \return
 *   see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_flexe_demux_active_cal_id_set(
    int unit,
    int group_index,
    bcm_port_flexe_group_cal_t cal_id);
/**
 * \brief - Get the Demux active calendar ID from DB.
 *
 * \param [in] unit - Unit #.
 * \param [in] group_index - FlexE group index.
 * \param [out] cal_id - FlexE calendar ID.
 *
 * \return
 *   see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_flexe_demux_active_cal_id_get(
    int unit,
    int group_index,
    bcm_port_flexe_group_cal_t * cal_id);
/*
 * }
 */
#endif /*_ALGO_FLEXE_GENERAL_H_INCLUDED_*/
