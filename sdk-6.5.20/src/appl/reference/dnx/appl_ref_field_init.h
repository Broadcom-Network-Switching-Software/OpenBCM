/** \file appl_ref_field_init.h
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*
* Include files.
* {
*/

#include <bcm/types.h>
#include <bcm/error.h>
#include <shared/error.h>

/*
 * }
 */

/**
 * \brief
 *   This function creates a application for field module, Fec Destination App
 * \param [in] unit - The unit number.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
shr_error_e appl_dnx_field_fec_dest_init(
    int unit);

/**
 * \brief
 *   This function creates a application for field module, OAM Layer Index App
 * \param [in] unit - The unit number.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
shr_error_e appl_dnx_field_oam_layer_index_init(
    int unit);

/**
 * \brief
 *   This function creates a application for field module, Trap L4 App
 * \param [in] unit - The unit number.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
shr_error_e appl_dnx_field_trap_l4_init(
    int unit);

/**
 * \brief
 *   This function creates a application for field module, OAM Stat App
 * \param [in] unit - The unit number.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
shr_error_e appl_dnx_field_oam_stat_init(
    int unit);

/**
* \brief
* Field App for J1 same port
* \param [in] unit        - Device id
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e appl_dnx_field_j1_same_port_init(
    int unit);

/**
* \brief
* Field App for J1 mode learning / learn limit
* \param [in] unit        - Device id
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e appl_dnx_field_learn_and_limit_init(
    int unit);

/**
* \brief
* Field App for J1 mode learn nullify key msbs
* \param [in] unit        - Device id
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e appl_dnx_field_learn_nullify_key_msbs_init(
    int unit);

/**
 * \brief
 *   This function creates a application for field module, Flow ID App
 * \param [in] unit - The unit number.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
shr_error_e appl_dnx_field_flow_id_init(
    int unit);

/**
 * \brief
 *   This function creates a application for field module, ROO App
 * \param [in] unit - The unit number.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
shr_error_e appl_dnx_field_roo_init(
    int unit);

/**
 * \brief
 *   This function creates a application for field module, IPMC App
 * \param [in] unit - The unit number.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
shr_error_e appl_dnx_field_ipmc_inlif_init(
    int unit);
/**
 * \brief
 *   This function creates a application for field module, JR1 PHP App
 * \param [in] unit - The unit number.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
shr_error_e appl_dnx_field_jr1_php_init(
    int unit);
/**
 * \brief
 *   This functions determines if the app should be skipped, Fec Destination App
 * \param [in] unit - The unit number.
 * \param [out] dynamic_flags - indicate if the application should be skipped or not
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
shr_error_e appl_dnx_field_fec_dest_cb(
    int unit,
    int *dynamic_flags);

/**
 * \brief
 *   This functions determines if the app should be skipped, OAM Layer Index App
 * \param [in] unit - The unit number.
 * \param [out] dynamic_flags - indicate if the application should be skipped or not
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
shr_error_e appl_dnx_field_oam_layer_index_cb(
    int unit,
    int *dynamic_flags);

/**
 * \brief
 *   This functions determines if the app should be skipped, Trap L4 App
 * \param [in] unit - The unit number.
 * \param [out] dynamic_flags - indicate if the application should be skipped or not
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
shr_error_e appl_dnx_field_trap_l4_cb(
    int unit,
    int *dynamic_flags);

/**
 * \brief
 *   This functions determines if the app should be skipped, OAM Stat App
 * \param [in] unit - The unit number.
 * \param [out] dynamic_flags - indicate if the application should be skipped or not
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
shr_error_e appl_dnx_field_oam_stat_cb(
    int unit,
    int *dynamic_flags);

/**
 * \brief
 *   This functions determines if the app should be skipped, Flow ID App
 * \param [in] unit - The unit number.
 * \param [out] dynamic_flags - indicate if the application should be skipped or not
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
shr_error_e appl_dnx_field_flow_id_cb(
    int unit,
    int *dynamic_flags);

/**
 * \brief
 *   This functions determines if the app should be skipped, ROO App
 * \param [in] unit - The unit number.
 * \param [out] dynamic_flags - indicate if the application should be skipped or not
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
shr_error_e appl_dnx_field_roo_cb(
    int unit,
    int *dynamic_flags);

/**
 * \brief
 *   This functions determines if the app should be skipped,IPMC App
 * \param [in] unit - The unit number.
 * \param [out] dynamic_flags - indicate if the application should be skipped or not
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
shr_error_e appl_dnx_field_ipmc_inlif_cb(
    int unit,
    int *dynamic_flags);
/**
 * \brief
 *   This functions determines if the app should be skipped, J1 same port App
 * \param [in] unit - The unit number.
 * \param [out] dynamic_flags - indicate if the application should be skipped or not
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
shr_error_e appl_dnx_field_j1_same_port_cb(
    int unit,
    int *dynamic_flags);

/**
 * \brief
 *   This functions determines if the app should be skipped, J1 learning / learn limit App
 * \param [in] unit - The unit number.
 * \param [out] dynamic_flags - indicate if the application should be skipped or not
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
shr_error_e appl_dnx_field_learn_and_limit_cb(
    int unit,
    int *dynamic_flags);

/**
 * \brief
 *   This functions determines if the app should be skipped, J1 learning / learn limit App
 * \param [in] unit - The unit number.
 * \param [out] dynamic_flags - indicate if the application should be skipped or not
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
shr_error_e appl_dnx_field_learn_nullify_key_msbs_cb(
    int unit,
    int *dynamic_flags);

/**
 * \brief
 *   This functions determines if the app should be skipped, J1 MPLS PHP App
 * \param [in] unit - The unit number.
 * \param [out] dynamic_flags - indicate if the application should be skipped or not
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
shr_error_e appl_dnx_field_j1_php_cb(
    int unit,
    int *dynamic_flags);

/*
 * }
 */
