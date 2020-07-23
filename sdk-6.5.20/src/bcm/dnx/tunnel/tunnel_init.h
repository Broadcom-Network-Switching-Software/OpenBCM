/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/** \file tunnel_init.h
 * Tunnel init functionality for DNX tunnel encapsaultion
*/

#ifndef _TUNNEL_INIT_INCLUDED__
/*
 * {
 */
#define _TUNNEL_INIT_INCLUDED__

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif /* BCM_DNX_SUPPORT */

/*
 * Include files.
 * {
 */
#include <bcm/types.h>
#include <bcm/tunnel.h>
/*
 * }
 */

/**
 * \brief
 * The tunnel id may be passed to the API by tunnel->tunnel_id or intf->l3a_intf_id. This function converts both
 * parameters to GPort ids and returns the value, in addition it also returns indication of whether the found GPort
 * is valid.
 * \param [in] unit -
 *     The unit number.
 * \param [in] intf - l3 intf 
 *     * intf->l3a_intf_id - if set, global if id of the tunnel to get.
 * \param [in] tunnel -
 *      * tunnel->tunnel_id - if set, global GPORT id of the tunnel to get.
 * \param [out] tunnel_gport -
 *      GPort of the tunnel to get.
 * \param [out] is_tunneled -
 *      indication that the returned GPort is a valid tunnel GPort.
*/
shr_error_e dnx_tunnel_initiator_gport_get(
    int unit,
    bcm_l3_intf_t * intf,
    bcm_tunnel_initiator_t * tunnel,
    bcm_gport_t * tunnel_gport,
    int *is_tunneled);

 /**
  * \brief
 * The function converts BCM tunnel-type to lif additional 
 * header profile 
 * \param [in] unit -
 *     The unit number.
 * \param [in] bcm_tunnel_type - tunnel type
 * \param [in] bcm_tunnel_flags - tunnel flags 
  *\param [out] lif_additional_header_profile - lif additional  
  *       header profile
 */
shr_error_e dnx_tunnel_init_bcm_to_lif_additional_header_profile(
    int unit,
    bcm_tunnel_type_t bcm_tunnel_type,
    uint32 bcm_tunnel_flags,
    uint32 *lif_additional_header_profile);

/**
 * \brief 
 * Map IP-Tunnel EEDB access stage to lif_mngr eedb phase.
 * \param [in] unit -
 *     The unit number.
 * \param [in] encap_access - EEDB access stage
 * \param [out] outlif_phase - eedb phase
*/
shr_error_e dnx_tunnel_initiator_encap_access_to_outlif_phase(
    int unit,
    bcm_encap_access_t encap_access,
    lif_mngr_outlif_phase_e * outlif_phase);

/**
 * \brief 
 * Map lif_mngr eedb phase to IP-Tunnel EEDB access stage.
 * \param [in] unit -
 *     The unit number.
 * \param [in] outlif_phase - eedb phase
 * \param [out] encap_access - EEDB access stage
*/
shr_error_e dnx_tunnel_initiator_outlif_phase_to_encap_access(
    int unit,
    lif_mngr_outlif_phase_e outlif_phase,
    bcm_encap_access_t * encap_access);

/**
 * \brief
 * The following function allocates SIP, TTL and QOS profiles. It writes the SIP, TTL and QOS values to HW if new
 * templates were allocated. And returns the created template profiles (sip_profile, ttl_pipe_profile
 * and nwk_qos_profile). 
 * \param [in] unit -
 *     The unit number.
 * \param [in] tunnel - tunnel init struct
 * \param [out] sip_profile - sip profile 
 * \param [out] ttl_pipe_profile - ttl profile
 * \param [out] nwk_qos_profile - nwk qos profile  
*/
shr_error_e dnx_tunnel_initiator_ttl_sip_template_alloc_and_hw_set(
    int unit,
    bcm_tunnel_initiator_t * tunnel,
    uint32 *sip_profile,
    uint32 *ttl_pipe_profile,
    uint32 *nwk_qos_profile);

/**
 * \brief 
 * The following function allocates/exchanges/removes the UDP ports profile, and returns the created allocated profile
 * and remove from hw indication.
 *
 * For allocation operation: Pass valid pointers in new_udp_ports_profile and write_new_udp_ports_profile, and -1 in NULL
 *                          in old_udp_ports_profile and remove_old_udp_ports_profile respectively.
 * For exchange operation: Pass valid pointers in new_udp_ports_profile, write_new_udp_ports_profile, and
 *                              remove_old_udp_ports_profile and a valid old profile in old_udp_ports_profile.
 * For free operation: Pass valid pointer and value in remove_old_udp_ports_profile and old_udp_ports_profile respectively,
 *                      and NULL in new_udp_ports_profile and write_new_udp_ports_profile. 
 * \param [in] unit -
 *     The unit number.
 * \param [in] tunnel - tunnel init struct
 * \param [in] old_udp_ports_profile - old udp port profile 
 * \param [out] new_udp_ports_profile - new udp port profile
 * \param [out] remove_old_udp_ports_profile - indicate old udp
 *        port profile can be deleted
*/
shr_error_e dnx_tunnel_udp_ports_profile_allocator_and_set_hw(
    int unit,
    bcm_tunnel_initiator_t * tunnel,
    uint32 old_udp_ports_profile,
    uint32 *new_udp_ports_profile,
    uint32 *remove_old_udp_ports_profile);

/**
 * \brief
 * The following function writes/clears the HW entry for UDP ports profile.
 * \param [in] unit -
 *     The unit number.
 * \param [in] tunnel - tunnel init struct 
 * \param [in] udp_ports_profile - udp port profile to write
 * \param [in] write_0_clear_1 -  
*/
shr_error_e dnx_tunnel_udp_ports_profile_hw_set(
    int unit,
    bcm_tunnel_initiator_t * tunnel,
    uint32 udp_ports_profile,
    uint8 write_0_clear_1);

 /**
 * \brief 
 * allocate local lif and Write to IP tunnel encapsulation
 * out-LIF table.
 *
 *   \param [in] unit - Relevant unit.
 *   \param [in] tunnel - A pointer to the struct that holds
 *     information for the IPv4/IPv6 tunnel encapsulation entry,
 *     see
 *     \ref bcm_tunnel_initiator_t.
 *   \param [in] dbal_table_id - Relevant dbal table ID
 *   \param [in] global_outlif -
 *     Local-Out-LIF whose entry should be added to out-LIF table.
 *   \param [in] next_local_outlif -
 *     Local-Out-LIF for DIP6 or arp whose entry should be added
 *     to out-LIF table. This parameter is not used when called
 *     for replace functionality.
 *   \param [in] sip_profile -
 *      Allocated SIP profile (template managed).
 *   \param [in] ttl_pipe_profile -
 *      Allocated TTL PIPE profile (template managed).
 *   \param [in] nwk_qos_profile -
 *      Allocated QOS PIPE profile (template managed).
 *   \param [in] esem_cmd_profile -
 *      Allocated esem access cmd profile (template managed).
 *   \param [in] lif_additional_header_profile -
 *      Use -1 if it's a fixed value for entry type, or template manager if it's a profile
 *      that can be allocated.
 *   \param [out] local_outlif - local outlif to return 
 *
 * \return
 *    \retval Negative in case of an error
 *    \retval Zero in case of NO ERROR
 *
 * \remark
 *   * The entry is written to DBAL_TABLE_EEDB_IPV6_TUNNEL
 */
shr_error_e dnx_tunnel_initiator_allocate_local_lif_and_out_lif_table_set(
    int unit,
    bcm_tunnel_initiator_t * tunnel,
    dbal_tables_e dbal_table_id,
    uint32 global_outlif,
    uint32 next_local_outlif,
    uint32 sip_profile,
    uint32 ttl_pipe_profile,
    uint32 nwk_qos_profile,
    uint32 esem_cmd_profile,
    uint32 lif_additional_header_profile,
    uint32 *local_outlif);

/**
 * \brief
 * The following function exchanges existing SIP and TTL profiles. It writes SIP and TTL values to HW if new templates
 * were allocated. And returns a notification if old TTL and SIP values should be removed from HW.
 * \par DIRECT INPUT:
 *   \param [in] unit                       - The unit number.
 *   \param [in] tunnel                     - The BCM API input, SIP and TTL used for template exchange.
 *   \param [in] old_sip_profile            - Existing SIP profile index.
 *   \param [in] old_ttl_profile            - Existing TTL profile index.
 *   \param [in] is_ttl_pipe_mode_old       - TTL PIPE mode notification.
 *   \param [out] new_sip_profile           - New SIP profile index.
 *   \param [out] new_ttl_profile           - New TTL profile index.
 *   \param [out] remove_old_sip_profile    - Remove old SIP profile value from HW.
 *   \param [out] remove_old_ttl_profile    - Remove old TTL profile value from HW.
 *   \param [in,out] new_qos_idx          - QOS profile index.
 *   \param [out] remove_old_qos_profile    - Remove old qos profile value from HW.
 *   \param [in] is_qos_model_pipe_mode_old - QOS PIPE mode notification.
 */
shr_error_e dnx_tunnel_initiator_ttl_sip_template_exchange(
    int unit,
    bcm_tunnel_initiator_t * tunnel,
    uint32 old_sip_profile,
    uint32 old_ttl_profile,
    uint32 is_ttl_pipe_mode_old,
    uint32 *new_sip_profile,
    uint32 *new_ttl_profile,
    uint8 *remove_old_sip_profile,
    uint8 *remove_old_ttl_profile,
    uint32 *new_qos_idx,
    uint8 *remove_old_qos_profile,
    uint32 is_qos_model_pipe_mode_old);

/**
 * \brief
 * The function receives template profiles and removes them from HW.
 * Currently supports only SIP and TTL templates. 
 *   \param [in] unit                       - The unit number.
 *   \param [in] local_out_lif              - local outlif
 *   \param [in] sip_profile                - SIP profile index.
 *   \param [in] ttl_pipe_profile           - TTL profile index.
 *   \param [in] qos_pipe_profile           - TTL PIPE mode
 *          notification.
 *   \param [in] remove_sip_profile           - remove sip
 *          profile indication
 *   \param [in] remove_ttl_profile           - remove ttl
 *          profile indication
 *   \param [in] remove_qos_profile    - remove qos profile
 *          indication 
 *   \param [in] tunnel_is_ipv6          - tunnel is ipv6
 *          indication
*/
shr_error_e dnx_tunnel_initiator_sip_ttl_qos_table_delete(
    int unit,
    uint32 local_out_lif,
    uint32 sip_profile,
    uint32 ttl_pipe_profile,
    uint32 qos_pipe_profile,
    uint8 remove_sip_profile,
    uint8 remove_ttl_profile,
    uint8 remove_qos_profile,
    uint32 tunnel_is_ipv6);

/**
 * \brief 
 * The function converts table specific flags from lif table 
 * manager to BCM tunnel-type 
 * \param [in] unit -
 *     The unit number.
 * \param [out] table_specific_flags - table specific flags from lif table 
 * manager
 * \param [in] lif_additional_header_profile - lif additional 
 *        header profile
 * \param [out] tunnel_type - tunnel type
*/
shr_error_e dnx_tunnel_initiator_table_specific_flags_to_tunnel_type(
    int unit,
    uint32 table_specific_flags,
    uint32 lif_additional_header_profile,
    bcm_tunnel_type_t * tunnel_type);

/**
 * \brief
 * The function receives template profiles and removes them from template manager.
 * Supports SIP, TTL and QOS templates. 
 * \param [in] unit -
 *     The unit number.
 * \param [in] local_out_lif - local outlif
 * \param [in] sip_profile - sip profile 
 * \param [in] ttl_pipe_profile - ttl pipe profile
 * \param [in] is_ttl_pipe_mode - ttl pipe mode
 * \param [out] *remove_sip_profile - indication to remove sip 
 *        profile
 * \param [out] *remove_ttl_pipe_profile - indication to remove 
 *        ttl pipe profile
 * \param [in] qos_model_idx - qos model index
 * \param [out] *remove_qos_pipe_profile - indication to remove 
 *        qos pipe profile
 * \param [in] is_qos_model_pipe_mode - indicate if qos model is 
 *        pipe
*/
shr_error_e dnx_tunnel_initiator_template_entries_free(
    int unit,
    uint32 local_out_lif,
    uint32 sip_profile,
    uint32 ttl_pipe_profile,
    uint32 is_ttl_pipe_mode,
    uint8 *remove_sip_profile,
    uint8 *remove_ttl_pipe_profile,
    uint32 qos_model_idx,
    uint8 *remove_qos_pipe_profile,
    uint32 is_qos_model_pipe_mode);

shr_error_e dnx_tunnel_init_get_table_id(
    int unit,
    bcm_tunnel_type_t tunnel_type,
    dbal_tables_e * dbal_table_id);

#endif  /*_TUNNEL_INIT_INCLUDED__ */
