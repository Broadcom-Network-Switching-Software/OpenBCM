/** \file bcm_int/dnx/ptp/ptp.h
 * 
 * DNX PTP headers
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef DNX_PTP_H_INCLUDED
/* { */
#define DNX_PTP_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

/*
 * Include files.
 * {
 */
#include <bcm/port.h>

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
 * STRUCTURES
 * {
 */
/*
 * }
 */

/**
 * \brief
 *   Initialize PTP module.
 *
 * \param [in] unit - The unit number.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_ptp_init(
    int unit);

/**
 * \brief - set 1588 protocol configuaion for port 
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] config_count - (IN) config_array size 
 * \param [in] config_array - (IN) 1588 port configuration
 *
 * \return
 *   shr_error_e - see _SHR_E_*
 *
 * \remarks
 *      Disable port 1588 configuration  - config_count = 0
 *      Enable  port 1588 configuration  - config_count = 1 
 *                       config_array->flags =
 *                       BCM_PORT_TIMESYNC_DEFAULT            mast be on 
 *                       BCM_PORT_TIMESYNC_ONE_STEP_TIMESTAMP when one step Tranparent clock (TC) is enabled system updates the 
 *                                                            correction field in 1588 Event 1588 messages.    
 *                       BCM_PORT_TIMESYNC_TWO_STEP_TIMESTAMP when step step Tranparent clock (TC) is enabled system records the 
 *                                                            1588 Event 1588 messages TX time.    
 *                         1588 event messages:
 *                           1. SYNC
 *                           2. DELAY_REQ
 *                           3. PDELAY_REQ
 *                           4. PDELAY_RESP
 *                       config_array->pkt_drop, config_array->pkt_tocpu - bit masks indicating wehter to forward (drop-0,tocpu-0), 
 *                       drop(drop-1) or trap(tocpu-1) the packet

 * \see
 *   * None
 */
shr_error_e dnx_ptp_port_timesync_config_set(
    int unit,
    bcm_port_t port,
    int config_count,
    bcm_port_timesync_config_t * config_array);

/**
 * \brief - get 1588 protocol port configuaion
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] array_size - config_array size
 * \param [out] config_array - 1588 port configuration
 * \param [out] array_count - config_array size 
 *
 * \return
 *   shr_error_e - see _SHR_E_*
 *
 * \remarks
 * output will be the follwing:
 * when port 1588 configuration Disabled - *array_count = 0
 * when port 1588 configuration Enabled  - *array_count = 1, config_array is set with the port configuration
 * \see
 *   * None
 */
shr_error_e dnx_ptp_port_timesync_config_get(
    int unit,
    bcm_port_t port,
    int array_size,
    bcm_port_timesync_config_t * config_array,
    int *array_count);

/**
 * \brief - Verify parameters for the following APIs:
 *    bcm_port_timesync_config_set
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] config_count - (IN) config_array size 
 * \param [in] config_array - (IN) 1588 port configuration
 *
 * \return
 *   shr_error_e - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_ptp_timesync_config_set_verify(
    int unit,
    bcm_port_t port,
    int config_count,
    bcm_port_timesync_config_t * config_array);

/**
 * \brief - verify API bcm_dnx_port_timesync_config_get
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] array_size - config_array size
 * \param [out] config_array - 1588 port configuration
 * \param [out] array_count - config_array size 
 *
 * \return
 *   shr_error_e - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_ptp_timesync_config_get_verify(
    int unit,
    bcm_port_t port,
    int array_size,
    bcm_port_timesync_config_t * config_array,
    int *array_count);

/**
 * \brief - set 1588 ptp delay per port
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] value - value of the ptp delay (nanoseconds units)
 *
 * \return
 *   shr_error_e - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_ptp_port_delay_set(
    int unit,
    bcm_port_t port,
    int value);

/**
 * \brief - get 1588 ptp delay per port
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [out] value - value of the ptp delay (units according to HW units)
 *
 * \return
 *   shr_error_e - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_ptp_port_delay_get(
    int unit,
    bcm_port_t port,
    int *value);

/**
 * \brief - Verify function for bcm_dnx_port_control_phy_timesync_set().
 * 
 * \param [in] unit - Unit ID
 * \param [in] port - logical port
 * \param [in] type - timesync configuration types
 * \param [in] value - timesync configuration values
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int dnx_ptp_phy_timesync_set_verify(
    int unit,
    bcm_port_t port,
    bcm_port_control_phy_timesync_t type,
    uint64 value);

/**
 * \brief - Verify function for bcm_dnx_port_control_phy_timesync_get().
 * 
 * \param [in] unit - Unit ID
 * \param [in] port - logical port
 * \param [in] type - timesync configuration types
 * \param [out] value - timesync configuration values
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int dnx_ptp_phy_timesync_get_verify(
    int unit,
    bcm_port_t port,
    bcm_port_control_phy_timesync_t type,
    uint64 *value);

/**
 * \brief - Set various PHY timesync controls for port.
 * 
 * \param [in] unit - Unit ID
 * \param [in] port - logical port
 * \param [in] type - timesync configuration types
 * \param [in] value - timesync configuration values
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int dnx_ptp_phy_timesync_set(
    int unit,
    bcm_port_t port,
    bcm_port_control_phy_timesync_t type,
    uint64 value);

/**
 * \brief - Get various PHY timesync controls for port.
 * 
 * \param [in] unit - Unit ID
 * \param [in] port - logical port
 * \param [in] type - timesync configuration types
 * \param [out] value - timesync configuration values
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int dnx_ptp_phy_timesync_get(
    int unit,
    bcm_port_t port,
    bcm_port_control_phy_timesync_t type,
    uint64 *value);

/**
 * \brief - Verify function for bcm_dnx_port_phy_timesync_config_set().
 * 
 * \param [in] unit - Unit ID
 * \param [in] port - logical port
 * \param [in] conf - timesync configuration
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int dnx_ptp_phy_timesync_config_set_verify(
    int unit,
    bcm_port_t port,
    bcm_port_phy_timesync_config_t * conf);

/**
 * \brief - Verify function for bcm_dnx_port_phy_timesync_config_get().
 * 
 * \param [in] unit - Unit ID
 * \param [in] port - logical port
 * \param [in] conf - timesync configuration
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int dnx_ptp_phy_timesync_config_get_verify(
    int unit,
    bcm_port_t port,
    bcm_port_phy_timesync_config_t * conf);

/**
 * \brief - Set various PHY timesync configuaion for port.
 * 
 * \param [in] unit - Unit ID
 * \param [in] port - logical port
 * \param [in] conf - timesync configuration
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int dnx_ptp_phy_timesync_config_set(
    int unit,
    bcm_port_t port,
    bcm_port_phy_timesync_config_t * conf);

/**
 * \brief - Get various PHY timesync configuration for port.
 * 
 * \param [in] unit - Unit ID
 * \param [in] port - logical port
 * \param [out] conf - timesync configuration
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int dnx_ptp_phy_timesync_config_get(
    int unit,
    bcm_port_t port,
    bcm_port_phy_timesync_config_t * conf);

/**
 * \brief - Verify function for bcm_dnx_port_timesync_tx_info_get().
 * 
 * \param [in] unit - Unit ID
 * \param [in] port - logical port
 * \param [out] tx_info - timesync tx info
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int dnx_ptp_timesync_tx_info_get_verify(
    int unit,
    bcm_port_t port,
    bcm_port_timesync_tx_info_t * tx_info);

/**
 * \brief - Get two-step PTP PHY timesync tx info for port.
 * 
 * \param [in] unit - Unit ID
 * \param [in] port - logical port
 * \param [out] tx_info - timesync tx info
 *
 * \return
 *   See shr_error_r
 * \remark
 *   * None
 * \see
 *   * None
 */
int dnx_ptp_port_timesync_tx_info_set(
    int unit,
    bcm_port_t port,
    bcm_port_timesync_tx_info_t * tx_info);

/* } */
#endif /* DNX_PTP_H_INCLUDED */
