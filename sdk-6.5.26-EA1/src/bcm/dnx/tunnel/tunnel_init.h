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

/*
 * Defines
 * {
 */
/*
 * ESEM commands flags which indicate the types of ESEM commands that are required for the tunnel/
 */
/** No ESEM command required  */
#define TUNNEL_INIT_ESEM_ACCESS_NONE           0x0
/** VXLAN VNI ESEM command required  */
#define TUNNEL_INIT_ESEM_ACCESS_TYPE_VNI       0x1
/** SVTAG ESEM command required  */
#define TUNNEL_INIT_ESEM_ACCESS_TYPE_SVTAG     0x2
 /*
  * }
  */

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
 * Supports SIP, TTL, QOS and OUT_LIF_PROFILE templates.
 * \param [in] unit -
 *     The unit number.
 * \param [in] local_out_lif - local outlif
 * \param [in] dbal_table_id - DBAL table ID used for freeing the ETPP out_lif_profile
 * \param [in] result_type - Result type in the relevant DBAL table, used for getting the out_lif_profile that should be freed
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
    dbal_tables_e dbal_table_id,
    int result_type,
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

shr_error_e dnx_tunnel_init_esem_command_alloc_and_set_hw(
    int unit,
    uint32 flags,
    int esem_cmd_prefix,
    uint32 tunnel_esem_access_flags,
    uint32 old_esem_cmd_profile,
    uint32 *new_esem_cmd_profile,
    uint8 *remove_old_esem_cmd_profile);

shr_error_e dnx_tunnel_init_esem_command_free_and_reset_hw(
    int unit,
    uint32 old_esem_cmd_profile,
    bcm_tunnel_type_t tunnel_type);

#endif  /*_TUNNEL_INIT_INCLUDED__ */
