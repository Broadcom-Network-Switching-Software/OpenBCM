/** \file l3_arp_flow.h
 * ARP APIs to flows APIs conversion
*/

#ifndef _L3_ARP_FLOW_INCLUDED__
/*
 * {
 */
#define _L3_ARP_FLOW_INCLUDED__

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX families only!"
#endif /* BCM_DNX_SUPPORT */
/*
 * Include files.
 * {
 */
#include <bcm/l3.h>
#include <soc/sand/shrextend/shrextend_error.h>
/*
 * }
 */

/*
 * FUNCTIONS
 * {
 */

/**
* \brief
*   Creation function for bcm_l3_egress_create with BCM_L3_EGRESS_ONLY flag using flow command.
*
*    \param [in] unit - Relevant unit.
*    \param [in] flags - Similar to bcm_l3_egress_create api input
*    \param [in] egr - Similar to bcm_l3_egress_create api input
*    \param [in] old_egr - The current ARP structure in case of ARP replace option.
* \return
*    shr_error_e -
*      Error return value
*/
shr_error_e dnx_l3_egress_flow_api_create_arp(
    int unit,
    uint32 flags,
    bcm_l3_egress_t * egr,
    bcm_l3_egress_t * old_egr);

/**
* \brief
* Retrieve a L3 egress object egress side information using flow APIs
*
*   \param [in] unit       -  Relevant unit.
*   \param [in] intf  -    egress object ID .
*   \param [out] egr  -    structure holding the egress object .
*
* \return
*   \retval  Zero if no error was detected
*   \retval  Negative if error was detected. See \ref shr_error_e
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_l3_egress_arp_info_flow_api_get(
    int unit,
    bcm_if_t intf,
    bcm_l3_egress_t * egr);

/**
* \brief
* Traverse all L3 egress ARP objects and run a callback function
* using flow APIs
*
*   \param [in] unit - Relevant unit.
*   \param [in] trav_fn - the name of the callback function,
*           it receives the value of the user_data variable and
*           the key and result values for each found entry
*   \param [in] user_data - user data that will be sent
*           to the callback function.
*
* \return
*   \retval  Zero if no error was detected
*   \retval  Negative if error was detected. See \ref shr_error_e
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_l3_egress_arp_flow_api_traverse(
    int unit,
    bcm_l3_egress_traverse_cb trav_fn,
    void *user_data);

/**
* \brief
* Delete L3 egress object egress side information using flow APIs
*
*   \param [in] unit       -  Relevant unit.
*   \param [in] intf  -    egress object ID .
*
* \return
*   \retval  Zero if no error was detected
*   \retval  Negative if error was detected. See \ref shr_error_e
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_l3_egress_arp_info_flow_api_delete(
    int unit,
    bcm_if_t intf);
/*
 * }
 */

#endif  /*_L3_ARP_FLOW_INCLUDED__ */
