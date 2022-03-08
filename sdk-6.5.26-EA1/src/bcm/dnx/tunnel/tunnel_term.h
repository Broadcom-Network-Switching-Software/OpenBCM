/** \file tunnel_term.h
 * Tunnel term functionality for DNX tunnel termination
*/

#ifndef _TUNNEL_TERM_INCLUDED__
/*
 * {
 */
#define _TUNNEL_TERM_INCLUDED__

/**
 * \brief
 * This enumeration contains the lookup types that are used for getting the TT LIF.
 */

typedef enum
{
    /*
     * The IPv4 will used for the TT lookup.
     */

    TUNNEL_TERM_LOOKUP_TYPE_IPV4 = 0,

    /*
     * The IPv6 will used for the TT lookup.
     */

    TUNNEL_TERM_LOOKUP_TYPE_IPV6 = 1,

    /*
     * The TXSCI will used for the TT lookup.
     */

    TUNNEL_TERM_LOOKUP_TYPE_TXSCI = 2,

    /*
     * CASCADED indicate the result from previous TCAM lookup is used as key in this lookup
     * Note: lookup SIP6 -> sip_idx_data, configured by bcm_tunnel_terminator_config_add API
     * tunnel_class is used in bcm_tunnel_terminator_create, but it's not cascaded because it's not from TCAM db.
     */
    TUNNEL_TERM_LOOKUP_TYPE_CASCADED = 3,

    /*
     * unsupported lookup type
     */
    TUNNEL_TERM_LOOKUP_TYPE_UNSUPPORTED = 4
} tunnel_term_lookup_type_e;

/**
 * \brief
 * Information copied from bcm_tunnel_terminator_t struct and bcm_tunnel_terminator_config_key_t
 * to configure ipv6 tcam table key
 * Since both bcm_tunnel_terminator and bcm_tunnel_terminator_config APIs can configure IPv6 tcam table
 */
typedef struct dnx_tunnel_term_ipv6_tcam_table_key_s
{
    bcm_tunnel_type_t type;     /* Tunnel type */
    uint32 udp_dst_port;        /* UDP dst port for UDP packets. */
    bcm_ip6_t dip6;             /* DIP for tunnel header match (IPv6). */
    bcm_ip6_t dip6_mask;        /* Destination IP mask (IPv6). */
    bcm_vrf_t vrf;              /* Virtual router instance. */
} dnx_tunnel_term_ipv6_tcam_table_key_t;

#define DNX_TUNNEL_TERM_EM_IP_MASK 0xFFFFFFFF
#define DNX_TUNNEL_TERM_IS_IPV4_P2P(_info) (_info->dip_mask == DNX_TUNNEL_TERM_EM_IP_MASK) && (_info->sip_mask == DNX_TUNNEL_TERM_EM_IP_MASK)
#define DNX_TUNNEL_TERM_IS_IPV4_MP(_info) (_info->dip_mask == DNX_TUNNEL_TERM_EM_IP_MASK) && (_info->sip_mask == 0)
#define DNX_TUNNEL_TERM_IS_IPV6_P2P(_info) (_info->flags & BCM_TUNNEL_TERM_USE_TUNNEL_CLASS)

shr_error_e dnx_tunnel_terminator_type_to_ipv6_additional_headers(
    int unit,
    dnx_tunnel_term_ipv6_tcam_table_key_t * tcam_table_key,
    uint32 *first_additional_header,
    uint32 *second_additional_header);

shr_error_e dnx_tunnel_terminator_additional_headers_to_ipv6_type(
    int unit,
    uint32 first_additional_header,
    uint32 second_additional_header,
    bcm_tunnel_type_t * tunnel_type,
    uint16 *udp_dst_port);

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

void dnx_tunnel_term_fodo_assignment_mode_get(
    int unit,
    bcm_tunnel_type_t tunnel_type,
    uint32 *fodo_assignment_mode);

uint8 dnx_tunnel_terminator_tunnel_type_is_txsci(
    bcm_tunnel_type_t tunnel_type);

tunnel_term_lookup_type_e dnx_tunnel_terminator_lookup_type_get(
    bcm_tunnel_type_t tunnel_type);

void dnx_tunnel_term_config_key_to_ipv6_tcam_table_key(
    bcm_tunnel_terminator_config_key_t * config_key,
    dnx_tunnel_term_ipv6_tcam_table_key_t * tcam_table_key);

void dnx_tunnel_term_to_ipv6_tcam_table_key(
    bcm_tunnel_terminator_t * tunnel_terminator,
    dnx_tunnel_term_ipv6_tcam_table_key_t * tcam_table_key);

shr_error_e dnx_tunnel_term_config_add_flow(
    int unit,
    bcm_tunnel_terminator_config_key_t * config_key,
    bcm_tunnel_terminator_config_action_t * config_action);

shr_error_e dnx_tunnel_term_config_get_flow(
    int unit,
    bcm_tunnel_terminator_config_key_t * config_key,
    bcm_tunnel_terminator_config_action_t * config_action);

shr_error_e dnx_tunnel_term_config_delete_flow(
    int unit,
    bcm_tunnel_terminator_config_key_t * config_key);

#endif  /*_TUNNEL_TERM_INCLUDED__ */
