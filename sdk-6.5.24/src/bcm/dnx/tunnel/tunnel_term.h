/** \file tunnel_term.h
 * Tunnel term functionality for DNX tunnel termination
*/

#ifndef _TUNNEL_TERM_INCLUDED__
/*
 * {
 */
#define _TUNNEL_TERM_INCLUDED__

shr_error_e dnx_tunnel_terminator_type_to_ipv6_additional_headers(
    int unit,
    bcm_tunnel_type_t tunnel_type,
    uint32 *first_additional_header,
    uint32 *second_additional_header);

shr_error_e dnx_tunnel_terminator_additional_headers_to_ipv6_type(
    int unit,
    uint32 first_additional_header,
    uint32 second_additional_header,
    bcm_tunnel_type_t * tunnel_type);

shr_error_e dnx_tunnel_terminator_type_to_ipv4_layer_qualifier_tunnel_type(
    int unit,
    bcm_tunnel_terminator_t * info,
    uint32 *ipv4_layer_qualifier_tunnel_type);

shr_error_e dnx_ipv4_layer_qualifier_tunnel_type_to_tunnel_terminator_type(
    int unit,
    uint32 ipv4_layer_qualifier_tunnel_type,
    bcm_tunnel_type_t * tunnel_type,
    uint32 *udp_dst_port);

shr_error_e dnx_tunnel_terminator_sip6_msbs_profile_allocate(
    int unit,
    bcm_ip6_t * sip6,
    int *sip_profile_id,
    uint8 *first_reference);

shr_error_e dnx_tunnel_terminator_msb_sip6_profile_get(
    int unit,
    bcm_ip6_t * sip6,
    int *sip_profile_id);

shr_error_e dnx_tunnel_terminator_traverse_ipv6_p2p_sip6_get(
    int unit,
    uint32 entry_handle_id,
    bcm_ip6_t * sip6);

shr_error_e dnx_tunnel_terminator_ipv6_sip_compress_table_entry_add(
    int unit,
    uint8 ipv6_sip[16],
    int ipv6_sip_profile);

shr_error_e dnx_tunnel_terminator_ipv6_sip_compress_table_entry_delete(
    int unit,
    uint8 ipv6_sip[16]);

uint8 dnx_tunnel_terminator_tunnel_type_is_ipv4(
    bcm_tunnel_type_t tunnel_type);

#endif  /*_TUNNEL_TERM_INCLUDED__ */
