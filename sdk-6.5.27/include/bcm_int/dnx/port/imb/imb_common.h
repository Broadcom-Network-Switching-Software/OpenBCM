/** \file imb_common.h
 *
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifndef IMB_COMMON_H_INCLUDED
/*
 * {
 */
#define IMB_COMMON_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX family only!"
#endif

/*
 * INCLUDE FILES:
 * {
 */
#include <bcm/port.h>
#include <soc/portmod/portmod.h>
#include <soc/phy/phymod_port_control.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_nif.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_port.h>

/*
 * }
 */
/*
 * DEFINES:
 * {
 */

/*
 * blocks out of reset and in reset values
 */
#define IMB_COMMON_IN_RESET       (1)
#define IMB_COMMON_OUT_OF_RESET   (0)

/*
 * Current prbs lock lost
 */
#define IMB_COMMON_PRBS_CURR_LOCK_LOST   (-1)
/*
 * Current Prbs lock and Prbs error count state is not cleared
 */
#define IMB_COMMON_PRBS_PREV_LOCK_LOST   (-2)

 /**
  * to be used in function port_bypass_if_enable_set to set only
  * one direction in bypass - RX/TX
  */
#define IMB_COMMON_DIRECTION_RX   (0x1)
#define IMB_COMMON_DIRECTION_TX   (0x2)
/*
 * \brief -  Port speed validate output sturct
 */
typedef struct imb_pm_resource_config_s
{
    int nof_ports;
    /** save the master ports we need to speed set in pbmp*/
    portmod_pbmp_t master_ports_pbmp;
    /** enable state[i] stores original enable state of port i, this array is shared */
    int *enable_state;
    portmod_speed_config_t port_speed_config[DNX_DATA_MAX_PORT_GENERAL_MAX_PHYS_PER_PM];
    /** Indicate if the speed is used for pcs bypass port */
    int speed_for_pcs_bypass_port[DNX_DATA_MAX_PORT_GENERAL_MAX_PHYS_PER_PM];
    portmod_pm_vco_setting_t vco_setting; /** only the output parameters will be valid */
    int port_starting_lane[DNX_DATA_MAX_PORT_GENERAL_MAX_PHYS_PER_PM];
    int flags; /** BCM_PORT_RESOURCE_XXX */
} imb_pm_resource_config_t;

/*
 * }
 */
/*
 * MACROs
 * {
 */
/*
 * }
 */
/*
 * Functions
 * {
 */

/**
 * \brief - Set prbs polynomial
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port or lane port
 * \param [in] prbs_mode - prbs mode
 * \param [in] value - prbs polynomial value
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_common_port_prbs_polynomial_set(
    int unit,
    bcm_port_t port,
    portmod_prbs_mode_t prbs_mode,
    bcm_port_prbs_t value);

/**
 * \brief - Get prbs polynomial
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port or lane port
 * \param [in] prbs_mode - prbs mode
 * \param [out] value - prbs polynomial value
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */

int imb_common_port_prbs_polynomial_get(
    int unit,
    bcm_port_t port,
    portmod_prbs_mode_t prbs_mode,
    bcm_port_prbs_t * value);

/**
 * \brief - Whether enable prbs data inversion
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port or lane port
 * \param [in] type - invert rx or tx prbs data
 * \param [in] prbs_mode - prbs mode
 * \param [in] invert - enable or disable data inversion
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */

int imb_common_port_prbs_invert_data_set(
    int unit,
    bcm_port_t port,
    int type,
    portmod_prbs_mode_t prbs_mode,
    int invert);

/**
 * \brief - Get the prbs data inversion status
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port or lane port
 * \param [in] type - invert rx or tx prbs data
 * \param [in] prbs_mode - prbs mode
 * \param [out] invert - enable or disable data inversion
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */

int imb_common_port_prbs_invert_data_get(
    int unit,
    bcm_port_t port,
    int type,
    portmod_prbs_mode_t prbs_mode,
    int *invert);

/**
 * \brief - Enable or disable Prbs
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port or lane port
 * \param [in] type - rx or tx prbs
 * \param [in] prbs_mode - prbs mode
 * \param [in] enable - enable or disable prbs
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */

int imb_common_port_prbs_enable_set(
    int unit,
    bcm_port_t port,
    int type,
    portmod_prbs_mode_t prbs_mode,
    int enable);

/**
 * \brief - Get prbs enable status
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port or lane port
 * \param [in] type - rx or tx prbs
 * \param [in] prbs_mode - prbs mode
 * \param [out] enable - enable or disable prbs
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */

int imb_common_port_prbs_enable_get(
    int unit,
    bcm_port_t port,
    int type,
    portmod_prbs_mode_t prbs_mode,
    int *enable);

/**
 * \brief - Get prbs status
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port or lane port
 * \param [in] prbs_mode - prbs mode
 * \param [out] status - prbs status
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */

int imb_common_port_prbs_rx_status_get(
    int unit,
    bcm_port_t port,
    portmod_prbs_mode_t prbs_mode,
    int *status);

/**
 * \brief - modify the port attributes for all the ports in the resource array
 *
 * \param [in] unit - chip unit id
 * \param [in] nport - number of ports
 * \param [in] resource - changed ports info look at bcm_port_resource_t
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * bcm_port_resource_t
 */

int imb_common_port_resource_multi_set(
    int unit,
    int nport,
    const bcm_port_resource_t * resource);

/**
 * \brief -  get the attributes of spcific port
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [out] resource - port attributes, look at bcm_port_resource_t
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

int imb_common_port_resource_get(
    int unit,
    bcm_gport_t port,
    bcm_port_resource_t * resource);

/**
 * \brief -  Modify the port attributes for all the ports in the resource array
 *
 * \param [in] unit - chip unit id
 * \param [in] nport - number of ports
 * \param [in] ports_with_modified_rx_speed - ports which their Rx speed was changed
 * \param [in] ports_with_modified_tx_speed - ports which their Tx speed was changed
 * \param [in] resource - changed ports info look at bcm_port_resource_t
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * bcm_port_resource_t
 */
int imb_common_post_resource_multi_set(
    int unit,
    int nport,
    bcm_pbmp_t ports_with_modified_rx_speed,
    bcm_pbmp_t ports_with_modified_tx_speed,
    const bcm_port_resource_t * resource);

/**
 * \brief - Validate whether a port can change
 *          its attributes (speed, FEC, CL72 etc.) without affecting other ports.
 *
 * \param [in] unit - chip unit id
 * \param [in] resource -port attributes, look at bcm_port_resource_t
 * \param [in] affected_pbmp - bitmap of ports that will be reset
 *             if the change described in resource will occur
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
int imb_common_port_speed_config_validate(
    int unit,
    const bcm_port_resource_t * resource,
    bcm_pbmp_t * affected_pbmp);

/**
 * \brief - Calculate the over sampling factor for the port.
 *        mostly in use when calculating serdes rate for the
 *        port
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] os_int - over sampling whole number
 * \param [in] os_remainder - over sampling fraction (after
 *        decimal point)
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_common_port_over_sampling_get(
    int unit,
    bcm_port_t port,
    uint32 *os_int,
    uint32 *os_remainder);

/**
 * \brief - sync the MAC status according to the PHY status.
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] link - phy link status
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_common_port_link_up_mac_update(
    int unit,
    bcm_port_t port,
    int link);

/**
 * \brief - post function for sync the MAC status according to the PHY status.
 *          do nothing
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] link - phy link status
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_common_port_link_up_mac_update_post_none(
    int unit,
    bcm_port_t port,
    int link);

/**
 * \brief - post function for sync the MAC status according to the PHY status.
 *          toggle MAC reset in auto-negotiation lock
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] link - phy link status
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_common_port_link_up_mac_update_post_reset_mac(
    int unit,
    bcm_port_t port,
    int link);

/**
 * \brief - convert bcm sch_prio to DBAL sch_prio
 *
 * \param [in] unit - chip unit id
 * \param [in] bcm_sch_prio - bcm scheduler priority
 * \param [out] dbal_sch_prio - dbal scheduler priority
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_common_dbal_sch_prio_from_bcm_sch_prio_get(
    int unit,
    bcm_port_nif_scheduler_t bcm_sch_prio,
    uint32 *dbal_sch_prio);

/**
 * \brief - convert to DBAL weight from reguler int (0-4)
 *
 * \param [in] unit - chip unit id
 * \param [in] weight - scheduler weight (regular int)
 * \param [out] dbal_sch_weight - dbal scheduler weight
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_common_dbal_sch_weight_get(
    int unit,
    int weight,
    uint32 *dbal_sch_weight);

/** 
 * imb_common_port_faults_status_get
 *
 * \brief Get port local and emote faults status 
 *
 * \param [in]  unit            - unit id
 * \param [in]  port            - logical port
 * \param [out]  local_fault     - Get local fault status
 * \param [out]  remote_fault    - Get remote fault status
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int imb_common_port_faults_status_get(
    int unit,
    bcm_port_t port,
    int *local_fault,
    int *remote_fault);

/*Check if phy control is supported for current PM.*/
int imb_common_port_phy_control_validate(
    int unit,
    bcm_port_t port,
    bcm_port_phy_control_t type,
    int is_set);

/*Get th BER histogram and port-FEC estimation values*/
int imb_common_port_post_ber_proj_get(
    int unit,
    bcm_port_t port,
    int lane,
    const bcm_port_ber_proj_params_t * ber_proj,
    int max_errcnt,
    bcm_port_ber_proj_analyzer_errcnt_t * errcnt_array,
    int *actual_errcnt);

/**
 * \brief   force link failure indication using SW
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] type - type indicating remote/local.
 * \param [in] enable - enable value to set
 *
 * \return
 *   int - see _SHR_E_*
 */
int imb_common_port_link_fault_force_set(
    int unit,
    bcm_port_t port,
    bcm_port_control_t type,
    int enable);

/**
 * \brief   get the link forced link failure indication
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] type - type indicating remote/local.
 * \param [out] enable - enable value to get
 *
 * \return
 *   int - see _SHR_E_*
 */
int imb_common_port_link_fault_force_get(
    int unit,
    bcm_port_t port,
    bcm_port_control_t type,
    int *enable);

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
int imb_common_port_macsec_port_enable_set(
    int unit,
    bcm_port_t port,
    int enable);

#endif /** INCLUDE_XFLOW_MACSEC */

/**
 *
 * \brief Port preemption configuration set/get
 *
 * \param [in]  unit            - unit id
 * \param [in]  port            - logical port
 * \param [in]  type            - preemption parameter of type bcm_port_preempt_control_t
 * \param [in]  value           - configured value
 */
int imb_common_preemption_control_set(
    int unit,
    bcm_port_t port,
    bcm_port_preempt_control_t type,
    uint32 value);

/**
 * imb_common_preemption_control_get
 *
 * \param [in]  unit            - unit id
 * \param [in]  port            - logical port
 * \param [in]  type            - preemption parameter of type bcm_port_preempt_control_t
 * \param [out] value           - returns configured value
 */
int imb_common_preemption_control_get(
    int unit,
    bcm_port_t port,
    bcm_port_preempt_control_t type,
    uint32 *value);

/**
 *
 * \brief Get the preemption verification status
 *
 * \param [in]  unit            - unit id
 * \param [in]  port            - logical port
 * \param [out] value           - returns verify operation status of type bcm_port_preempt_status_verify_t
 */
int imb_common_preemption_verify_status_get(
    int unit,
    bcm_port_t port,
    uint32 *value);

/**
 *
 * \brief Get the preemption status on the transmit side
 *
 * \param [in]  unit            - unit id
 * \param [in]  port            - logical port
 * \param [out] value           - returns preemption status of type bcm_port_preempt_status_tx_t
 */
int imb_common_preemption_tx_status_get(
    int unit,
    bcm_port_t port,
    uint32 *value);

/**
 * \brief - Configure the TXPI SDM divider in portmod
 *
 * \param [in]  unit            - unit id
 * \param [in]  port            - logical port
 * \param [in]  speed           - the speed of the port
 */
int imb_common_port_txpi_sdm_divider_set(
    int unit,
    bcm_port_t port,
    int speed);
/*
 * }
 */

#endif/*_IMB_COMMON_H_INCLUDED_*/
