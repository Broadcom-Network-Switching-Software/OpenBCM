/** \file instru_internal.h
 * 
 *
 * General internal instru functions.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
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

/**
 * \brief
 *   Given an integer, sets the value of the IPFIX system Up time virtual register.
 * \param [in] unit  -
 *   Relevant unit.
 * \param [in] uptime -
 *   IPFIX System Up time
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected.
 * \remark
 *   * None
 * \see
 *  * None
*/
shr_error_e dnx_instru_ipfix_system_up_time_set(
    int unit,
    int uptime);

/**
 * \brief
 *   Given an integer, sets the value of the IPFIX eventor id virtual register.
 * \param [in] unit  -
 *   Relevant unit.
 * \param [in] eventor_id -
 *   IPFIX eventor rx context id
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected.
 * \remark
 *   * None
 * \see
 *  * None
*/
shr_error_e dnx_instru_ipfix_eventor_id_set(
    int unit,
    int eventor_id);

/**
 * \brief
 *   Given an integer, sets the value of the IPFIX template id virtual register.
 * \param [in] unit  -
 *   Relevant unit.
 * \param [in] template_id -
 *   IPFIX template id
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected.
 * \remark
 *   * None
 * \see
 *  * None
*/
shr_error_e dnx_instru_ipfix_template_id_set(
    int unit,
    int template_id);

/**
 * \brief
 *   Given an integer, sets the value of the IPFIX tx_time virtual register.
 * \param [in] unit  -
 *   Relevant unit.
 * \param [in] tx_time -
 *   IPFIX packet export time
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected.
 * \remark
 *   * None
 * \see
 *  * None
*/
shr_error_e dnx_instru_ipfix_tx_time_set(
    int unit,
    int tx_time);

/**
 * \brief
 *   Given an integer, sets the value of the IPFIX Observation_Domain virtual register.
 * \param [in] unit  -
 *   Relevant unit.
 * \param [in] observation_domain -
 *   IPFIX Observation Domain
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected.
 * \remark
 *   * None
 * \see
 *  * None
*/
shr_error_e dnx_instru_ipfix_observation_domain_set(
    int unit,
    int observation_domain);

/**
 * \brief
 *   Given a pointer to integer, gets the value of the ipfix uptime virtual register.
 * \param [in] unit -
 *   Relevant unit.
 * \param [out] uptime -
 *  Pointer that will be filled with the uptime of the ipfix.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected.
 * \remark
 *   * None
 * \see
 *  * None
*/
shr_error_e dnx_instru_ipfix_system_up_time_get(
    int unit,
    int *uptime);

/**
 * \brief
 *   Given a pointer to integer, gets the value of the ipfix eventor id virtual register.
 * \param [in] unit -
 *   Relevant unit.
 * \param [out] eventor_id -
 *  Pointer that will be filled with the eventor_id of the ipfix.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected.
 * \remark
 *   * None
 * \see
 *  * None
*/
shr_error_e dnx_instru_ipfix_eventor_id_get(
    int unit,
    int *eventor_id);

/**
 * \brief
 *   Given a pointer to integer, gets the value of the ipfix template id virtual register.
 * \param [in] unit -
 *   Relevant unit.
 * \param [out] template_id -
 *  Pointer that will be filled with the template id of the ipfix.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected.
 * \remark
 *   * None
 * \see
 *  * None
*/
shr_error_e dnx_instru_ipfix_template_id_get(
    int unit,
    int *template_id);

/**
 * \brief
 *   Given a pointer to integer, gets the value of the ipfix tx time virtual register.
 * \param [in] unit -
 *   Relevant unit.
 * \param [out] tx_time -
 *  Pointer that will be filled with the tx time of the ipfix.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected.
 * \remark
 *   * None
 * \see
 *  * None
*/
shr_error_e dnx_instru_ipfix_tx_time_get(
    int unit,
    int *tx_time);

/**
 * \brief
 *   Given a pointer to integer, gets the value of the ipfix observation domain virtual register.
 * \param [in] unit  -
 *   Relevant unit.
 * \param [out] observation_domain -
 *   IPFIX Observation Domain
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected.
 * \remark
 *   * None
 * \see
 *  * None
*/
shr_error_e dnx_instru_ipfix_observation_domain_get(
    int unit,
    int *observation_domain);

#endif /* INTERNAL_INSTRU_INCLUDED */
