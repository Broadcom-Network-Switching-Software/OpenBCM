/** \file include/bcm_int/dnx/port/port_flexe.h
 * 
 *
 * Internal DNX Port APIs
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 */

#ifndef _PORT_FLEXE_H_INCLUDED_
/** { */
#define _PORT_FLEXE_H_INCLUDED_

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX family only!"
#endif

#include <bcm/types.h>
#include <bcm/switch.h>

/**
 * FlexE mode
 */
typedef enum
{
    /*
     * FlexE is disable in the device
     */
    DNX_FLEXE_MODE_DISABLED = 0,
    /*
     * FlexE centralized mode
     */
    DNX_FLEXE_MODE_CENTRALIZED,
    /*
     * FlexE distributed mode
     */
    DNX_FLEXE_MODE_DISTRIBUTED,
} dnx_flexe_mode_e;

/**
 * FlexE NB TDM slots allocation mode for
 * 50G NRZ speed
 */
typedef enum
{
    /*
     * Always allocate single slot for 50G NRZ
     */
    DNX_FLEXE_ALLOC_MODE_SINGLE = 0,
    /*
     * Always allocate double slot for 50G NRZ
     */
    DNX_FLEXE_ALLOC_MODE_DOUBLE,
    /*
     * The number of slots can be modified
     * dynamically
     */
    DNX_FLEXE_ALLOC_MODE_DYNAMIC,
} dnx_flexe_nb_tdm_slot_alloc_mode_e;

typedef struct dnx_flexe_cal_info_s
{
    /*
     * Indicated all calendars in for MAC client has been updated
     */
    int mac_client_cal_is_updated;
    /*
     * Indicated all calendars for flexe phy port has been updated
     */
    int flexe_phy_cal_is_updated;
} dnx_flexe_cal_info_t;
/**
 * FlexE device slot mode
 */
typedef enum dnx_flexe_device_slot_mode_s
{
    /*
     * Only 1G and 5G slots are supported
     */
    DNX_FLEXE_SLOT_MODE_1G_AND_5G = 0,
    /*
     * Only 1P25G and 5G slots are supported
     */
    DNX_FLEXE_SLOT_MODE_1P25G_AND_5G,
} dnx_flexe_device_slot_mode_e;

/**
 * \brief - Configure the FlexE configurations
 *
 */
shr_error_e dnx_port_flexe_config_set(
    int unit,
    bcm_port_t port,
    uint32 flags,
    dnx_flexe_cal_info_t * cal_info);

/**
 * \brief - Configure FlexE calendar dynamically
 *
 */
shr_error_e dnx_port_flexe_calendar_set(
    int unit,
    bcm_port_t port,
    uint32 flags);

/**
 * \brief - Init modules related to Framer.
 *
 */
shr_error_e dnx_framer_init(
    int unit);

/**
 * \brief - Deinit Framer blocks
 *
 */
shr_error_e dnx_framer_deinit(
    int unit);

/**
 * \brief - Configure the OAM alarm collection time step
 *
 */
int dnx_flexe_oam_alarm_collection_timer_step_set(
    int unit,
    int step);

/**
 * \brief - Configure number of steps for OAM alarm
 *    collection period
 *
 */
int dnx_flexe_oam_alarm_collection_step_count_set(
    int unit,
    int step_count);

/**
 * \brief - Get the OAM alarm collection time step
 *
 */
int dnx_flexe_oam_alarm_collection_timer_step_get(
    int unit,
    int *step);

/**
 * \brief - Get number of steps for OAM alarm
 *    collection period
 *
 */
int dnx_flexe_oam_alarm_collection_step_count_get(
    int unit,
    int *step_count);

/**
 * \brief - Configure the SAR Cell mode:
 *    28*66b or 29*66b
 *
 */
int dnx_flexe_sar_cell_mode_set(
    int unit,
    bcm_switch_flexe_sar_cell_mode_t cell_mode);

/**
 * \brief - Get the SAR Cell mode:
 *    28*66b or 29*66b
 *
 */
int dnx_flexe_sar_cell_mode_get(
    int unit,
    bcm_switch_flexe_sar_cell_mode_t * cell_mode);

/**
 * \brief - Set FlexE 1588 packet header
 *
 */
int dnx_flexe_1588_pkt_header_set(
    int unit,
    bcm_switch_control_t type,
    int val);

/**
 * \brief - Get FlexE 1588 packet header
 *
 */
int dnx_flexe_1588_pkt_header_get(
    int unit,
    bcm_switch_control_t type,
    int *val);
/**
 * \brief - Enable/disable flexe bas oam to external
 *          processor
 */
int dnx_flexe_bas_oam_to_ext_processor_set(
    int unit,
    int enable);
/**
 * \brief - get flexe bas oam to external
 *          processor enable status
 */
int dnx_flexe_bas_oam_to_ext_processor_get(
    int unit,
    int *enable);
/**
 * \brief - Set FlexE OH/OAM packet MAC
 *
 */
int dnx_flexe_oh_oam_pkt_mac_set(
    int unit,
    bcm_switch_control_t type,
    int val);
/**
 * \brief - Get FlexE OH/OAM packet MAC
 *
 */
int dnx_flexe_oh_oam_pkt_mac_get(
    int unit,
    bcm_switch_control_t type,
    int *val);
/**
 * \brief - Configure the port control for FlexE/MAC/SAR clients
 *
 */
int dnx_port_flexe_control_set(
    int unit,
    bcm_port_t port,
    bcm_port_control_t type,
    int value);
/**
 * \brief - Get the port control for FlexE/MAC/SAR clients
 *
 */
int dnx_port_flexe_control_get(
    int unit,
    bcm_port_t port,
    bcm_port_control_t type,
    int *value);
/**
 * \brief - Set switch control for FlexE OAM
 */
int dnx_flexe_oam_switch_control_set(
    int unit,
    bcm_switch_control_t type,
    int value);
/**
 * \brief - Get switch control for FlexE OAM
 */
int dnx_flexe_oam_switch_control_get(
    int unit,
    bcm_switch_control_t type,
    int *value);
/**
 * \brief - Configure per port SAR Cell mode
 *
 */
int dnx_flexe_sar_cell_mode_per_port_set(
    int unit,
    bcm_port_t port,
    bcm_port_control_t type,
    bcm_port_control_sar_cell_mode_t cell_mode);
/**
 * \brief - Get per port SAR Cell mode
 *
 */
int dnx_flexe_sar_cell_mode_per_port_get(
    int unit,
    bcm_port_t port,
    bcm_port_control_t type,
    bcm_port_control_sar_cell_mode_t * cell_mode);
/**
 * \brief - Verify Framer port when removing port.
 */
int dnx_port_framer_port_remove_verify(
    int unit,
    bcm_port_t logical_port);
/**
 * \brief - G.hao for FlexE 66b switching
 */
int dnx_port_flexe_66b_switch_ghao_config_set(
    int unit,
    bcm_port_t port,
    int speed);
/**
 * \brief - Prepare the fifo resources
 */
int dnx_port_flexe_fifo_resource_prepare(
    int unit,
    bcm_port_t port,
    uint32 flags);
/**
 * \brief - Switch the fifo resources
 */
int dnx_port_flexe_fifo_resource_switch(
    int unit,
    bcm_port_t port,
    uint32 flags);
/**
 * \brief - Prepare the shadow calendar
 */
int dnx_port_flexe_calendar_prepare(
    int unit,
    bcm_port_t port,
    uint32 flags);
/**
 * \brief - Switch the calendar
 */
int dnx_port_flexe_calendar_switch(
    int unit,
    bcm_port_t port,
    uint32 flags);
/**
 * \brief - Get enable status for 1588 on L1 ETH ports
 */
int dnx_port_l1_eth_1588_enable_get(
    int unit,
    bcm_port_t port,
    bcm_port_control_t type,
    int *enable);
/**
 * \brief - Enable/Disable 1588 on L1 ETH ports
 */
int dnx_port_l1_eth_1588_enable_set(
    int unit,
    bcm_port_t port,
    bcm_port_control_t type,
    int enable);
/**
 * \brief - Create stat channel for FlexE client
 */
int dnx_port_framer_stat_channel_create(
    int unit,
    bcm_port_t logical_port);

/**
 * \brief - Destroy stat channel for FlexE client
 */
int dnx_port_framer_stat_channel_destroy(
    int unit,
    bcm_port_t logical_port);

/** } */
#endif /*_PORT_FLEXE_H_INCLUDED_*/
