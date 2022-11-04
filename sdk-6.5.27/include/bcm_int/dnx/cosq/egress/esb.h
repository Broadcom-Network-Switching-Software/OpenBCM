/*
 * \file esb.h
 *
 *  Created on: March 26, 2019
 *      Author: vg889851
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef _ESB_H_INCLUDED_
#define _ESB_H_INCLUDED_

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX family only!"
#endif

#define ESB_QUEUE_ID_INVALID -1

/**
 * \brief - Initiallize the Egress Shared Buffers
 *
 * \param [in] unit -  Unit-ID
 *
 * \return
 *   shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_esb_init(
    uint32 unit);

/**
 * \brief - Configure the Egress Shared Buffers for a given logical port
 *
 * \param [in] unit -  Unit-ID
 * \param [in] logical_port - Logical port
 *
 * \return
 *   shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_esb_port_er_add(
    uint32 unit,
    bcm_port_t logical_port);

/**
 * \brief - Configure port speed dependent settings of the Egress Shared Buffers for a given logical port
 *
 * \param [in] unit -  Unit-ID
 * \param [in] logical_port - Logical port
 *
 * \return
 *   shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_esb_port_speed_config_add(
    uint32 unit,
    bcm_port_t logical_port);

/**
 * \brief - Configure TXI logic and IRDY threshold for a given logical port
 *
 * \param [in] unit -  Unit-ID
 * \param [in] logical_port - Logical port
 * \param [in] is_add - indication whether port is added (otherwise it's removed)
 *
 * \return
 *   shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_esb_port_txi_config_set(
    uint32 unit,
    bcm_port_t logical_port,
    int is_add);

/**
 * \brief - Clear the configuration of the Egress Shared Buffers for a given logical port
 *
 * \param [in] unit -  Unit-ID
 * \param [in] logical_port - Logical port
 *
 * \return
 *   shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_esb_port_er_remove(
    uint32 unit,
    bcm_port_t logical_port);

/**
 * \brief -
 *   Given rate, in Mega Bits Per Second, calculate the 
 *   weight for the ESB calendar configuration
 *
 * \param [in] unit - HW Unit-ID
 * \param [in] rate - Rate in Mega Bits Per Second
 * \param [out] weight_p - Holds the weight
 * \param [in] is_l1 - Indicate if the calculation is fone for L1 queue
 *
 * \return
 *   shr_error_e
 *   If zero (_SHR_E_NONE), then no error was encountered.
 *   Otherwise, see shr_error_e
 * \remark
 *   * None
 * \see
 *   * dnx_esb_nif_calendar_set
 */
shr_error_e dnx_esb_nif_rate_to_weight(
    int unit,
    uint32 rate,
    uint32 *weight_p,
    uint8 is_l1);

/**
 * \brief -
 *   Based on setup of rate of master ports, refresh interface calendar configurations
 *   for ESB.
 *
 * \param [in] unit - Unit-ID
 * \param [in] core - Core-ID
 * \return
 *   If zero (_SHR_E_NONE), then no error was encountered.
 *   Otherwise, see shr_error_e
 * \remark
 *   None
 * \see
 *   * None
 */
shr_error_e dnx_esb_nif_calendar_refresh(
    int unit,
    bcm_core_t core);

/**
 * \brief -
 *   Activate the calendar configurations.
 *
 * \param [in] unit - Unit-ID
 * \param [in] core - Core-ID
 * \return
 *   If zero (_SHR_E_NONE), then no error was encountered.
 *   Otherwise, see shr_error_e
 * \remark
 *   None
 * \see
 *   * None
 */
shr_error_e dnx_esb_nif_calendar_activate(
    int unit,
    bcm_core_t core);

/**
 * \brief -
 *   get current interface calendar mux index for ESB
 *
 * \param [in] unit - Unit-ID
 * \param [in] core -  Core-ID
 * \param [out] current_calendar -  current_calendar
 * \return
 *   If zero (_SHR_E_NONE), then no error was encountered.
 *   Otherwise, see shr_error_e
 * \remark
 *   None
 * \see
 *   * None
 */
shr_error_e dnx_esb_nif_current_calendar_get(
    uint32 unit,
    bcm_core_t core,
    uint32 *current_calendar);

/**
 * \brief -
 *   Based on setup of rate of master ports, load interface calendar for ESB
 *
 * \param [in] unit - Unit-ID
 * \param [in] core -  Core-ID
 * \return
 *   If zero (_SHR_E_NONE), then no error was encountered.
 *   Otherwise, see shr_error_e
 * \remark
 *   * Indirect input is the 'rate' setup for all master ports (assigned per interface).
 *     See dnx_algo_port_interface_rate_get()
 * \see
 *   * None
 */
shr_error_e dnx_esb_nif_calendar_set(
    int unit,
    bcm_core_t core);

/**
 * \brief -
 *   configure the initiate credit size for ilkn port.
 * \param [in] unit - HW Unit-ID
 * \param [in] port - logical port
 * \param [in] max_credits - maximum credits allowed to configure
 * \return
 *   If zero (_SHR_E_NONE), then no error was encountered.
 *   Otherwise, see shr_error_e
 * \remark
 *   None 
 * \see
 *   None
 */
shr_error_e dnx_esb_ilkn_credits_set(
    int unit,
    bcm_port_t port,
    int max_credits);

/**
 * \brief -
 *   configure the initiate credit size for ilkn port.
 * \param [in] unit - HW Unit-ID
 * \param [in] logical_port - logical port
 * \param [in] is_add - set (add) or clear threshold
 * \return
 *   If zero (_SHR_E_NONE), then no error was encountered.
 *   Otherwise, see shr_error_e
 * \remark
 *   None
 * \see
 *   None
 */
shr_error_e dnx_esb_port_txi_irdy_thr_set(
    uint32 unit,
    bcm_port_t logical_port,
    int is_add);

/**
 * \brief -
 * \param [in] unit - HW Unit-ID
 * \param [in] logical_port - logical port
 * \param [in] is_tx_preemption - is_preemption
 */
shr_error_e dnx_esb_to_fqp_mapping_set(
    uint32 unit,
    bcm_port_t logical_port,
    uint8 is_tx_preemption);

/**
 * \brief -
 *   configure frame preemption in ESB.
 * \param [in] unit - HW Unit-ID
 * \param [in] egq_nif_if - Egq nif if
 * \param [in] core_id - core
 * \param [in] enable - enable/disable param
 * \return
 *   If zero (_SHR_E_NONE), then no error was encountered.
 *   Otherwise, see shr_error_e
 * \remark
 *   None
 * \see
 *   None
 */
shr_error_e dnx_esb_dbal_frame_preemption_set(
    int unit,
    int egq_nif_if,
    int core_id,
    uint32 enable);

/**
 * \brief -
 *   get configured frame preemption in ESB.
 * \param [in] unit - HW Unit-ID
 * \param [in] egq_nif_if - Egq nif if
 * \param [in] core_id - core
 * \param [out] enable - enable/disable param
 * \return
 *   If zero (_SHR_E_NONE), then no error was encountered.
 *   Otherwise, see shr_error_e
 * \remark
 *   None
 * \see
 *   None
 */
shr_error_e dnx_esb_dbal_frame_preemption_get(
    int unit,
    int egq_nif_if,
    int core_id,
    uint32 *enable);

#endif /* _ESB_H_INCLUDED_ */
