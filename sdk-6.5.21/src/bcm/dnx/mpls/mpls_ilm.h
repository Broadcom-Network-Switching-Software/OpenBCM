/** \file mpls_ilm.h 
 * General MPLS functionality for DNX tunnel forwarding. 
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _MPLS_ILM_INCLUDED__
/*
 * {
 */
#define _MPLS_ILM_INCLUDED__

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

/*
 * }
 */

/**
 * \brief
 * Create MPLS tunnels in the ingress for either termination (pop), forwarding (swap) or php.
 * \par DIRECT INPUT
 *    \param [in] unit -Relevant unit.
 *    \param [in] result_type -LEM result type.
 *      Can be either:
 *      DBAL_RESULT_TYPE_MPLS_FWD_FWD_DEST_FWD_DEST_OUTLIF - for pop,swap
 *      DBAL_RESULT_TYPE_MPLS_FWD_FWD_DEST_FWD_DEST_EEI - for PHP, eei retreived directly from lem
 *      DBAL_RESULT_TYPE_MPLS_FWD_FWD_DEST_FWD_DEST_ONLY - for PHP with cascaded fecs
 *    \param [in] info -A pointer to a struct containing relevant information for the ILM entry.
 *      info.label - Used as table key
 *      info.egress_if - Outgoing egress object.
 *      info.port - Destinaion port.
 * \par INDIRECT INPUT:
 *   * \b *info \n
 *     See 'info' in DIRECT INPUT above
 * \par DIRECT OUTPUT:
 *   \retval Negative in case of an error. See \ref shr_error_e.
 *   \retval Zero in case of NO ERROR
 * \par INDIRECT OUTPUT
 *   * Write to MPLS-ILM DB - DBAL_TABLE_MPLS_FWD table.
 * \remark
 * MPLS-ILM is the forwarding table for MPLS application. It is used primarily for swap/pop actions \n
 * but supports also pointing to egress MPLS objects.
 */
shr_error_e dnx_mpls_tunnel_switch_ilm_add(
    int unit,
    int result_type,
    bcm_mpls_tunnel_switch_t * info);

/**
* \brief
*  Get the created MPLS tunnels in the ingress for mpls forwarding (with 
* actions swap, php or push).
* \par DIRECT INPUT:
*    \param [in] unit -Relevant unit.
*     \param [in] info -A pointer to a struct to be filled according the saved entry.
*     info.label - The key for the lookups
*     info.egress_if - outlif used in case of forwarding/push
*     info.port - destination port
* \par INDIRECT INPUT:
*   * \b *info \n
*     See 'info' in DIRECT INPUT above
* \par DIRECT OUTPUT:
*   shr_error_e - Non-zero in case of an error.
* \par INDIRECT OUTPUT:
*   * See \b 'info' in DIRECT INPUT
*/
shr_error_e dnx_mpls_tunnel_switch_ilm_get(
    int unit,
    bcm_mpls_tunnel_switch_t * info);

/**
* \brief
*  Delete given MPLS tunnels in the ingress for mpls forwarding (with 
* actions swap, php or push).
*    \param [in] unit -Relevant unit.
*   \param [in] info  - A pointer to the struct that holds information for the ingress MPLS entry deletion.
*       info.label - entry to be deleted
*       info.second_label - second label to be used for deleted entry if BCM_MPLS_SWITCH_LOOKUP_SECOND_LABEL set in info.flags
*       info.ingress_if - interface to be used for deleted entry if BCM_MPLS_SWITCH_LOOKUP_L3_INGRESS_INTF set in info.flags
* \return
*   shr_error_e
*/
shr_error_e dnx_mpls_tunnel_switch_ilm_delete(
    int unit,
    bcm_mpls_tunnel_switch_t * info);

/**
* \brief
*  Delete all given MPLS tunnels in the ingress for mpls forwarding (with 
* actions swap, php or push).
* \par DIRECT INPUT:
*    \param [in] unit -Relevant unit.
* \par DIRECT OUTPUT:
*   shr_error_e - Non-zero in case of an error.
* \par INDIRECT OUTPUT:
*   * Delete all MPLS-ILM DB - DBAL_TABLE_MPLS_FWD table.
*/
shr_error_e dnx_mpls_tunnel_switch_ilm_delete_all(
    int unit);

/**
 * \brief
 * Traverse all MPLS tunnels in the ingress for forwarding (swap) and run a callback function
 * provided by the user for each one.
 * \param [in] unit - The unit number.
 * \param [in] cb   - A pointer to callback function,
 *          it receives the value of the user_data variable and
 *          all MPLS tunnel objects that were iterated 
 * \param [in] user_data - user data that will be sent to the callback function
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \see
 *  * DBAL_TABLE_MPLS_FWD
 *  * DBAL_TABLE_MPLS_FWD_PER_IF_UA
 */
shr_error_e dnx_mpls_tunnel_switch_ilm_traverse(
    int unit,
    bcm_mpls_tunnel_switch_traverse_cb cb,
    void *user_data);

#endif/*_MPLS_ILM_INCLUDED__*/
