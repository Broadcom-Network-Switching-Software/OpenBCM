/** 
 *  \file src/bcm/dnx/rx/rx_trap_v1.h
 * 
 * 
 * Internal DNX RX APIs 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 */

#ifndef RX_TRAP_V1_H_INCLUDED
/* { */
#define RX_TRAP_V1_H_INCLUDED

#include <include/bcm_int/dnx/rx/rx.h>
#include <include/bcm_int/dnx/rx/rx_trap.h>

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#if defined(BCM_DNX2_SUPPORT)

/**
 * \brief -
 *   For each of the ERPP Filters initialize trap model.
 *   The filters are enabled on init, they have default forwarding strength.
 *   The packet hitting on the specific filter will be dropped.
 *
 * \param [in] unit - Unit ID
 * \param [in] init_trap_gport - initial trap gport to configure
 *
 * \return
 *   \retval  Zero if no error was detected
 *   \retval  Negative if error was detected. See \ref shr_error_e
 *
 * \remark
 * \see
 *  None.
 */
shr_error_e dnx_rx_trap_v1_erpp_filters_init(
    int unit,
    bcm_gport_t init_trap_gport);

/**
 * \brief -
 *   For each of the ETPP Filters initialize trap model.
 *   The filters are enabled on init, they have default forwarding strength.
 *   The packet hitting on the specific filter will be dropped.
 *
 * \param [in] unit - Unit ID
 *
 * \return
 *   \retval  Zero if no error was detected
 *   \retval  Negative if error was detected. See \ref shr_error_e
 *
 * \remark
 * \see
 *  None.
 */
shr_error_e dnx_rx_trap_v1_etpp_filters_init(
    int unit);

/**
 * \brief -
 *   For each of the EGRESS Filters initialize trap model.
 *   The filters are enabled on init, they have default forwarding strength.
 *   The packet hitting on the specific filter will be dropped.
 *
 * \param [in] unit - Unit ID
 *
 * \return
 *   \retval  Zero if no error was detected
 *   \retval  Negative if error was detected. See \ref shr_error_e
 *
 * \remark
 * \see
 *  None.
 */
shr_error_e dnx_rx_trap_v1_egress_filters_init(
    int unit);

/**
* \brief
*  Create and id for given etpp trap type, i.e allocate an id in relevant pool
* \par DIRECT INPUT:
*   \param [in] unit     -  Unit id
*   \param [in] flags       - Flags(WITH_ID)
*   \param [in] trap_class  -  trap class (first or OAM) to create
*   \param [in] type     -  bcmRxTrapTypeEgTx.. to create
*   \param [out] trap_id_p   -  etpp trap is allocated
* \par INDIRECT INPUT:
*   * None
* \par DIRECT OUTPUT:
*   shr_error_e - Error type
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_rx_trap_v1_etpp_create(
    int unit,
    int flags,
    dnx_rx_trap_class_e trap_class,
    bcm_rx_trap_t type,
    int *trap_id_p);

/**
* \brief
*  Set the etpp trap action profile: trap strength and action.
*  Allocates profile using template manager.
* \par DIRECT INPUT:
*   \param [in] unit      -  Unit Id
*   \param [in] trap_class  -  trap class to configure
*   \param [in] trap_type   -  etpp trap type (SW id) to configure
*   \param [in] config_p  -  Hold the etpp trap action profile configuration to set for specific trap
* \par INDIRECT INPUT:
*   * None
* \par DIRECT OUTPUT:
*   shr_error_e -  Error Type
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_rx_trap_v1_etpp_set(
    int unit,
    dnx_rx_trap_class_e trap_class,
    int trap_type,
    bcm_rx_trap_config_t * config_p);

/**
* \brief
*  Get the etpp trap action profile: trap strength and action.
*  Allocates profile using template manager.
* \par DIRECT INPUT:
*   \param [in] unit      -  Unit Id
*   \param [in] trap_class  -  trap class (first or OAM) to configure
*   \param [in] trap_type   -  etpp trap type (SW id) to configure
*   \param [out] config_p  -  Hold the etpp trap action profile configuration to set for specific trap
* \par INDIRECT INPUT:
*   * None
* \par DIRECT OUTPUT:
*   shr_error_e -  Error Type
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_rx_trap_v1_etpp_get(
    int unit,
    dnx_rx_trap_class_e trap_class,
    int trap_type,
    bcm_rx_trap_config_t * config_p);

/**
* \brief
*  Destroy previously created ETPP trap, i.e. deallocate from relevant trap pool
* \par DIRECT INPUT:
*   \param [in] unit        -  Unit id
*   \param [in] trap_class  -  trap class (predefined or OAM) of trap to destroy
*   \param [in] trap_type     -  ETPP trap type to destroy
* \par INDIRECT INPUT:
*   * None
* \par DIRECT OUTPUT:
*   shr_error_e - Error type
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_rx_trap_v1_etpp_destroy(
    int unit,
    dnx_rx_trap_class_e trap_class,
    int trap_type);

/**
* \brief
*  Set ETPP trap action profile in HW: trap strengths and profile number.
* \par DIRECT INPUT:
*   \param [in] unit      -  Unit Id
*   \param [in] trap_id   -  etpp trap id (SW id) to configure
*   \param [in] action_profile_p  -  Hold the action-profile configuration to set for specific trap
* \par INDIRECT INPUT:
*   * None
* \par DIRECT OUTPUT:
*   shr_error_e -  Error Type
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_rx_trap_v1_etpp_action_profile_hw_set(
    int unit,
    int trap_id,
    dnx_rx_trap_action_profile_t * action_profile_p);

/**
* \brief
*  Get ETPP trap action profile from HW: trap strengths and profile number.
* \par DIRECT INPUT:
*   \param [in] unit      -  Unit Id
*   \param [in] trap_id   -  etpp trap id (SW id) to configure
*   \param [out] action_profile_p  -  Hold the etpp trap action-profile retrived
* \par INDIRECT INPUT:
*   * None
* \par DIRECT OUTPUT:
*   shr_error_e -  Error Type
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_rx_trap_v1_etpp_action_profile_hw_get(
    int unit,
    int trap_id,
    dnx_rx_trap_action_profile_t * action_profile_p);

/**
* \brief
*  Clears the ETPP trap action profile.
* \par DIRECT INPUT:
*   \param [in] unit            - unit Id
*   \param [in] dbal_trap_id    - Encoded trap ID
* \par INDIRECT INPUT:
*   * None
* \par DIRECT OUTPUT:
*   shr_error_e - Error type
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_rx_trap_v1_etpp_action_profile_hw_clear(
    int unit,
    int dbal_trap_id);

/**
* \brief
*  This function sets the MTU Profile config.
* \par DIRECT INPUT:
*   \param [in] unit       - unit id
*   \param [in] mtu_key_p  - MTU key
*   \param [in] mtu_data_p - MTU data
* \par INDIRECT INPUT:
*   * None
* \par DIRECT OUTPUT:
*   shr_error_e -  Error Type
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_rx_trap_v1_mtu_profile_hw_set(
    int unit,
    bcm_rx_mtu_profile_key_t * mtu_key_p,
    bcm_rx_mtu_profile_value_t * mtu_data_p);

/**
* \brief
*  This function retrieves the MTU Profile config.
* \par DIRECT INPUT:
*   \param [in] unit       - unit id
*   \param [in] mtu_key_p  - MTU key
*   \param [out] mtu_data_p - MTU data
* \par INDIRECT INPUT:
*   * None
* \par DIRECT OUTPUT:
*   shr_error_e -  Error Type
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_rx_trap_v1_mtu_profile_hw_get(
    int unit,
    bcm_rx_mtu_profile_key_t * mtu_key_p,
    bcm_rx_mtu_profile_value_t * mtu_data_p);

/**
* \brief
*  This function clears the MTU Profile config.
* \par DIRECT INPUT:
*   \param [in] unit       - unit id
*   \param [in] mtu_key_p  - MTU key
* \par INDIRECT INPUT:
*   * None
* \par DIRECT OUTPUT:
*   shr_error_e -  Error Type
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_rx_trap_v1_mtu_profile_hw_clear(
    int unit,
    bcm_rx_mtu_profile_key_t * mtu_key_p);

/**
* \brief
*  This function sets the MTU Profile field in the
*  relevant DBAL table for enabling LIF MTU Configuration.
*
* \par DIRECT INPUT:
*   \param [in] unit                 - Unit Id
*   \param [in] mtu_config_p         - MTU Configuration
* \par INDIRECT INPUT:
*   * None
* \par DIRECT OUTPUT:
*   shr_error_e - Error type
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_rx_trap_v1_mtu_lif_set(
    int unit,
    bcm_rx_mtu_config_t * mtu_config_p);

/**
* \brief
*  This function retrieves the MTU configuration for the supplied LIF
*
* \par DIRECT INPUT:
*   \param [in] unit                 - Unit Id
*   \param [in] mtu_config_p         - MTU Configuration retrieved
* \par INDIRECT INPUT:
*   * None
* \par DIRECT OUTPUT:
*   shr_error_e - Error type
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_rx_trap_v1_mtu_lif_get(
    int unit,
    bcm_rx_mtu_config_t * mtu_config_p);

/**
* \brief
*  Allocate and Set the ingress LIF trap Profile in the correct IN_LIF table, for the supplied IN_LIF.
* \par DIRECT INPUT:
*   \param [in] unit        - unit Id
*   \param [in] lif_hw_resources_p  - LIF's HW resouces
*   \param [in] action_gport - action gport encoding Trap-Action-Profile
* \par INDIRECT INPUT:
*   * None
* \par DIRECT OUTPUT:
*   shr_error_e - Error type
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_rx_trap_v1_lif_ingress_profile_set(
    int unit,
    dnx_algo_gpm_gport_hw_resources_t * lif_hw_resources_p,
    bcm_gport_t action_gport);

/**
* \brief
*  Get the ingress LIF trap profile action_gport.
* \par DIRECT INPUT:
*   \param [in] unit        - unit Id
*   \param [in] lif_hw_resources_p  - LIF's HW resouces
*   \param [in] action_gport_p - action gport encoding Trap-Action-Profile
* \par INDIRECT INPUT:
*   * None
* \par DIRECT OUTPUT:
*   shr_error_e - Error type
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_rx_trap_v1_lif_ingress_profile_get(
    int unit,
    dnx_algo_gpm_gport_hw_resources_t * lif_hw_resources_p,
    bcm_gport_t * action_gport_p);

/**
* \brief
*  Allocate and Set the ETPP LIF trap Profile (Trap or Mirror)  in the correct OUT_LIF table, for the supplied OUT_LIF.
* \par DIRECT INPUT:
*   \param [in] unit        - unit Id
*   \param [in] lif_gport - LIF gport
*   \param [in] lif_hw_resources_p  - LIF's HW resouces
*   \param [in] action_gport - action gport encoding Trap-Action-Profile
* \par INDIRECT INPUT:
*   * None
* \par DIRECT OUTPUT:
*   shr_error_e - Error type
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_rx_trap_v1_lif_etpp_profile_lif_set(
    int unit,
    bcm_gport_t lif_gport,
    dnx_algo_gpm_gport_hw_resources_t * lif_hw_resources_p,
    bcm_gport_t action_gport);

/**
* \brief
*  Get the ETPP LIF trap profile action_gport.
* \par DIRECT INPUT:
*   \param [in] unit        - unit Id
*   \param [in] lif_gport - LIF gport
*   \param [in] lif_hw_resources_p  - LIF's HW resouces
*   \param [out] action_gport_p - action gport
* \par INDIRECT INPUT:
*   * None
* \par DIRECT OUTPUT:
*   shr_error_e - Error type
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_rx_trap_v1_lif_etpp_profile_lif_get(
    int unit,
    bcm_gport_t lif_gport,
    dnx_algo_gpm_gport_hw_resources_t * lif_hw_resources_p,
    bcm_gport_t * action_gport_p);

#endif /** defined(BCM_DNX2_SUPPORT) */

#endif /*_RX_TRAP_V1_API_INCLUDED__*/
