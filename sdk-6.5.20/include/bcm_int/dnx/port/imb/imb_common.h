/** \file imb_common.h
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef IMB_COMMON_H_INCLUDED
/*
 * {
 */
#define IMB_COMMON_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

/*
 * INCLUDE FILES:
 * {
 */
#include <bcm/port.h>
#include <soc/portmod/portmod.h>
#include <soc/phy/phymod_port_control.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_nif.h>
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
    portmod_speed_config_t port_speed_config[DNX_DATA_MAX_NIF_ETH_NOF_LANES_IN_CDU];
    /** Indicate if the speed is used for pcs bypass port */
    int speed_for_pcs_bypass_port[DNX_DATA_MAX_NIF_ETH_NOF_LANES_IN_CDU];
    portmod_pm_vco_setting_t vco_setting; /** only the output parameters will be valid */
    int port_starting_lane[DNX_DATA_MAX_NIF_ETH_NOF_LANES_IN_CDU];
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
 * \brief - Read or write PHY registers associated with a port
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port or lane port
 * \param [in] flags - Indicate how to access the PHY, and which PHY if the port has an internal serdes
 * \param [in] phy_reg_addr - Register address in the PHY
 * \param [in] phy_data - Data to be written or data read
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */

int imb_common_port_phy_reg_set(
    int unit,
    bcm_port_t port,
    uint32 flags,
    uint32 phy_reg_addr,
    uint32 phy_data);

/**
 * \brief - Read or write PHY registers associated with a port
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port or lane port
 * \param [in] flags - Indicate how to access the PHY, and which PHY if the port has an internal serdes
 * \param [in] phy_reg_addr - Register address in the PHY
 * \param [out] phy_data - Data to be written or data read
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */

int imb_common_port_phy_reg_get(
    int unit,
    bcm_port_t port,
    uint32 flags,
    uint32 phy_reg_addr,
    uint32 *phy_data);

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

/*
 * }
 */

#endif/*_IMB_COMMON_H_INCLUDED_*/
