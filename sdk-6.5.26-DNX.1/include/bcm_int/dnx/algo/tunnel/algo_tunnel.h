/**
 * \file algo_tunnel.h Internal tunnel APIs
PIs This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
PIs 
PIs Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef ALGO_TUNNEL_H_INCLUDED
/*
 * { 
 */
#define ALGO_TUNNEL_H_INCLUDED

#include <bcm_int/dnx/algo/template_mngr/template_manager_types.h>
#include <bcm/tunnel.h>
#include <bcm_int/dnx/lif/lif_lib.h>

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX family only!"
#endif

/**
 * Resource name defines for algo tunnel 
 * {
 */

#define DNX_ALGO_TUNNEL_IPV6_SIP_PROFILE "TUNNEL IPV6 SIP PROFILE"

#define DNX_ALGO_TUNNEL_UDP_PORTS_PROFILE "TUNNEL UDP PORTS PROFILE"

#define DNX_ALGO_TUNNEL_ADDITIONAL_HEADERS_PROFILE "ETPP ADDITIONAL HEADERS PROFILE"

#define DNX_ALGO_TUNNEL_ADDITIONAL_HEADERS_PROFILE_MAP "ETPP ADDITIONAL HEADERS PROFILE MAP"

#define MAX_NOF_ADDITIONAL_HEADERS_PROFILE_IDENTIFIERS 64

/**
 * }
 */
/**
 * \brief 
 *   allocate resource pool for all TUNNEL profiles types
 * \param [in] unit - 
 *   The unit number. 
 * \return  
 *   \retval Negative in case of an error. 
 *   \retval Zero in case of NO ERROR
 */
shr_error_e dnx_algo_tunnel_init(
    int unit);

/**
* \brief
* Print an entry of ipv6 sip tunnel profile template. 
* \param [in] unit - the unit number 
* \param [in] data - Pointer of the struct to be printed.
* \return 
*     None 
*/
void dnx_algo_tunnel_ipv6_sip_profile_template_print_cb(
    int unit,
    const void *data);

/**
* \brief
* Print an entry of UDP ports tunnel profile template.
* \param [in] unit - the unit number
* \param [in] data - Pointer of the struct to be printed.
* \return
*     None
*/
void dnx_algo_tunnel_udp_ports_profile_template_print_cb(
    int unit,
    const void *data);

/**
* \brief
* Print an entry of additional_headers_profile map template.
* \param [in] unit - the unit number
* \param [in] data - Pointer of the struct to be printed.
* \return
*     None
*/
void dnx_algo_tunnel_additional_headers_profile_map_print_cb(
    int unit,
    const void *data);

/**
* \brief
* Print an entry of additional_headers_profile template.
* \param [in] unit - the unit number
* \param [in] data - Pointer of the struct to be printed.
* \return
*     None
*/
void dnx_algo_tunnel_additional_headers_profile_print_cb(
    int unit,
    const void *data);

/**
 * \brief
 * The tunnel id may be passed to the API by tunnel->tunnel_id or intf->l3a_intf_id. This function converts both
 * parameters to GPort ids and returns the value, in addition it also returns indication of whether the found GPort
 * is valid.
 * \param [in] unit -
 *     The unit number.
 * \param [in] intf_id - l3 intf id
 *     * intf_id - if set, global if id of the tunnel to get.
 * \param [in] tunnel -
 *      * tunnel->tunnel_id - if set, global GPORT id of the tunnel to get.
 * \param [out] tunnel_gport -
 *      GPort of the tunnel to get.
 * \param [out] is_tunneled -
 *      indication that the returned GPort is a valid tunnel GPort.
*/
shr_error_e dnx_algo_tunnel_initiator_gport_get(
    int unit,
    bcm_if_t intf_id,
    bcm_tunnel_initiator_t * tunnel,
    bcm_gport_t * tunnel_gport,
    int *is_tunneled);

/**
 * \brief
 * The function indicate if a tunnel is ipv6
 * \param [in] unit -
 *     The unit number.
 * \param [in] tunnel_type - tunnel type
 * \param [out] is_ipv6 - is ipv6 indication
*/
void dnx_algo_tunnel_init_ipv6_is_ipv6_tunnel(
    int unit,
    bcm_tunnel_type_t tunnel_type,
    uint32 *is_ipv6);

/**
 * \brief
 * The function update the alloc info for out lif tunnel
 * \param [in] unit -
 *     The unit number.
 * \param [in] tunnel - tunnel info
 * \param [out] alloc_info - allocation info strucutre
*/
shr_error_e dnx_algo_tunnel_global_out_lif_alloc_info_set(
    int unit,
    bcm_tunnel_initiator_t * tunnel,
    global_lif_alloc_info_t * alloc_info);
/*
 * } 
 */
#endif/*_ALGO_QOS_API_INCLUDED__*/
