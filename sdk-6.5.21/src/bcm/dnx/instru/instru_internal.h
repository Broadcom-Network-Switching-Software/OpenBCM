/** \file instru_internal.h
 * 
 *
 * General internal instru functions.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef INTERNAL_INSTRU_H_INCLUDED
/*
 * {
 */
#define INTERNAL_INSTRU_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

/**
 * \brief
 *   Given an integer, sets the value of the sFlow sampling rate virtual register.
 * \param [in] unit  -
 *   Relevant unit.
 * \param [in] sampling_rate -
 *  Sampling rate of the sFlow.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected.
 * \remark
 *   * None
 * \see
 *  * None
*/
shr_error_e dnx_instru_sflow_sampling_rate_set(
    int unit,
    int sampling_rate);

/**
 * \brief
 *   Given a pointer to integer, gets the value of the sFlow sampling rate virtual register.
 * \param [in] unit -
 *   Relevant unit.
 * \param [out] sampling_rate -
 *  Pointer that will be filled with the sampling rate of the sFlow.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected.
 * \remark
 *   * None
 * \see 
 *  * None
*/
shr_error_e dnx_instru_sflow_sampling_rate_get(
    int unit,
    int *sampling_rate);

/**
 * \brief
 *   Given an integer, sets the value of the sFlow uptime virtual register.
 * \param [in] unit  -
 *   Relevant unit.
 * \param [in] uptime -
 *  Sampling rate of the sFlow.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected.
 * \remark
 *   * None
 * \see
 *  * None
*/
shr_error_e dnx_instru_sflow_uptime_set(
    int unit,
    int uptime);

/**
 * \brief
 *   Given a pointer to integer, gets the value of the sFlow uptime virtual register.
 * \param [in] unit -
 *   Relevant unit.
 * \param [out] uptime -
 *  Pointer that will be filled with the sampling rate of the sFlow.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected.
 * \remark
 *   * None
 * \see
 *  * None
*/
shr_error_e dnx_instru_sflow_uptime_get(
    int unit,
    int *uptime);

/**
 * \brief
 *   Given the AS number , sets the value of the sFlow My Router AS Number register.
 * \param [in] unit  -
 *   Relevant unit.
 * \param [in] as_number -
 *  My Router AS number of the sFlow.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected.
 * \remark
 *   * None
 * \see
 *  * None
*/

shr_error_e dnx_instru_sflow_myrouter_as_number_set(
    int unit,
    int as_number);

/**
 * \brief
 *   Given a pointer to integer, gets the value of the sFlow My Router AS Number register.
 * \param [in] unit -
 *   Relevant unit.
 * \param [out] as_number -
 *  Pointer that will be filled with the My Router AS number of the sFlow.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected.
 * \remark
 *   * None
 * \see
 *  * None
*/
shr_error_e dnx_instru_sflow_myrouter_as_number_get(
    int unit,
    int *as_number);

/**
 * \brief
 *   Given an IP address, sets the value of the sFlow agent IP address swstate.
 * \param [in] unit  -
 *   Relevant unit.
 * \param [in] agent_ip_address -
 *  Agent IP address of the sFlow.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected.
 * \remark
 *   * None
 * \see
 *  * None
*/
shr_error_e dnx_instru_sflow_agent_id_address_set(
    int unit,
    bcm_ip_t agent_ip_address);

/**
 * \brief
 *   Given a pointer to IP address, gets the value of the sFlow agent IP address from swstate.
 * \param [in] unit -
 *   Relevant unit.
 * \param [out] agent_ip_address -
 *  Pointer that will be filled with the agent IP address of the sFlow.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected.
 * \remark
 *   * None
 * \see
 *  * None
*/
shr_error_e dnx_instru_sflow_agent_id_address_get(
    int unit,
    bcm_ip_t * agent_ip_address);

#endif /* INTERNAL_INSTRU_INCLUDED */
