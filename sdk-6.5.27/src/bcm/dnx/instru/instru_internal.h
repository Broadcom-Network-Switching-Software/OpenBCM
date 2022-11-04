/** \file instru_internal.h
 * 
 *
 * General internal instru functions.
 *
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef INTERNAL_INSTRU_H_INCLUDED
/*
 * {
 */
#define INTERNAL_INSTRU_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX family only!"
#endif

/**
 * \brief -
 * Verify sflow encap is indeed exists and it is with the correct table/result_type/phase
 */
shr_error_e dnx_instru_sflow_encap_id_verify(
    int unit,
    int sflow_encap_id);

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
 * \brief - Given an instru global lif, returns the local lif and it's result type.
 */
shr_error_e dnx_instru_sflow_encap_id_to_local_lif(
    int unit,
    int sflow_encap_id,
    int *local_lif,
    uint32 *result_type);

/**
 * \brief - set oam lif db data
 *
 * \param [in] unit - Relevant unit
 * \param [in] sflow_encap_info - Pointer to a struct from which the relevant data (stat_cmd, counter_command_id) is taken
 * \param [in] sflow_outlif - Serves as a key to the oam lif db dbal table
 *
 * \return
 *   shr_error_e, negative in case of an error.
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_instru_sflow_add_oam_lif_db(
    int unit,
    bcm_instru_sflow_encap_info_t * sflow_encap_info,
    int sflow_outlif);

/**
 * \brief - Allocate a sFlow destination profile according to the required UDP tunnel.
 *
 * \param [in] unit - Relevant unit
 * \param [in] sflow_encap_info - UDP tunnel will be taken from here, as well as the old sFlow destination profile if the
 *                                  REPLACE flag is set.
 * \param [out] sflow_destination_profile - The destination profile for this UDP tunnel.
 * \param [out] write_destination_profile - If set, write the new profile to HW. If not, the profile is already written.
 * \param [out] old_sflow_destination_profile - If REPLACE flag was set, this holds the profile that the sflow encap was pointing to.
 * \param [out] delete_old_destination_profile - If REPLACE flag was set, and this is set, then remove this profile from HW.
 *                                          If it's not set, then then the profile is still in use, and don't remove it.
 *
 * \return
 *   shr_error_e, negative in case of an error.
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_instru_sflow_destination_profile_allocate(
    int unit,
    bcm_instru_sflow_encap_info_t * sflow_encap_info,
    int *sflow_destination_profile,
    int *write_destination_profile,
    int *old_sflow_destination_profile,
    int *delete_old_destination_profile);

/**
 * \brief - get oam lif db data
 *
 * \param [in] unit - Relevant unit
 * \param [in] sflow_encap_info - Pointer to a struct to which the relevant data (stat_cmd, counter_command_id) will be written into
 * \param [in] sflow_outlif - Serves as a key to the oam lif db dbal table
 *
 * \return
 *   shr_error_e, negative in case of an error.
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_instru_sflow_oam_lif_db_get(
    int unit,
    bcm_instru_sflow_encap_info_t * sflow_encap_info,
    int sflow_outlif);

/**
 * \brief - Delete oam lif db entry
 *
 * \param [in] unit - Relevant unit
 * \param [in] sflow_outlif - the sflow lif id, key to the dbal table
 *
 * \return
 *   shr_error_e, negative in case of an error.
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_instru_sflow_oam_lif_db_delete(
    int unit,
    int sflow_outlif);

/**
 * \brief - get the sFlow destination connected to the sFlow encap.
 *
 * \param [in] unit - Relevant unit
 * \param [in,out] sflow_encap_info - sFlow encap info. The encap id is used as input, and the tunnel_id
 *                                      and sub_agent_id fields will be filled.
 *
 * \return
 *   shr_error_e, negative in case of an error.
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_instru_sflow_destination_get(
    int unit,
    bcm_instru_sflow_encap_info_t * sflow_encap_info);

/**
 * \brief
 * Encode Source-System-Port from System-Port.
 *
 * \see
 *   dnx_instru_sflow_sample_interface_input_add
 *   dnx_instru_sflow_sample_interface_input_remove
 *   dnx_instru_sflow_sample_interface_input_get
 */
shr_error_e dnx_instru_sflow_sample_interface_input_system_port_to_source_system_port(
    int unit,
    bcm_gport_t system_port,
    uint32 *source_system_port);

/**
 * \brief
 * Get System-Port from Source-System-Port.
 *
 * \see
 *   dnx_instru_sflow_sample_interface_input_traverse
 *
 */
shr_error_e dnx_instru_sflow_sample_interface_input_source_system_port_to_system_port(
    int unit,
    bcm_gport_t source_system_port,
    uint32 *system_port);

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

/**
 * \brief -
 * Start or stop (and reset) the collection of the data for ICMG synchronized counters.
 * Set the SW state.
 *
 * \param [in] unit -  Unit-ID
 * \param [in] source_type -  Type of Synchronous Counters mechanism
 * \param [in] is_start -  If TRUE start, if FALSE stop.
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_instru_synced_counters_sw_state_set(
    int unit,
    bcm_instru_synced_counters_source_type_t source_type,
    int is_start);
#endif /* INTERNAL_INSTRU_INCLUDED */
