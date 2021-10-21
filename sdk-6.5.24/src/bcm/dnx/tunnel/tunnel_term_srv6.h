/*
 * ! \file bcm_int/dnx/l3/l3.h Internal DNX TT APIs
PIs This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
PIs 
PIs Copyright 2007-2021 Broadcom Inc. All rights reserved.
 */
/*
 * Include files.
 * {
 */
#include <bcm/types.h>
#include <bcm/tunnel.h>
#include <bcm/error.h>
/*
 * }
 */

/*
 * MACROs and ENUMs
 * {
 */

#define DNX_TUNNEL_TERM_SRV6_IS_SRV6_WITH_96_LOCATOR(_info)  (_SHR_IS_FLAG_SET(_info->flags, BCM_TUNNEL_TERM_UP_TO_96_LOCATOR_SEGMENT_ID))
#define DNX_TUNNEL_TERM_SRV6_IS_SRV6_WITH_64_LOCATOR(_info)  (_SHR_IS_FLAG_SET(_info->flags, BCM_TUNNEL_TERM_UP_TO_64_LOCATOR_SEGMENT_ID))
#define DNX_TUNNEL_TERM_SRV6_IS_SRV6_WITH_GENERALIZED_SEGMENT_ID(_info)  (_SHR_IS_FLAG_SET(_info->flags, BCM_TUNNEL_TERM_GENERALIZED_SEGMENT_ID))
#define DNX_TUNNEL_TERM_SRV6_IS_SRV6_WITH_GENERALIZED_SEGMENT_ID_EOC(_info)  (_SHR_IS_FLAG_SET(_info->flags, BCM_TUNNEL_TERM_GENERALIZED_SEGMENT_ID_END_OF_COMPRESSION))
#define DNX_TUNNEL_TERM_SRV6_IS_SRV6_WITH_MICRO_SEGMENT_ID(_info)  (_SHR_IS_FLAG_SET(_info->flags, BCM_TUNNEL_TERM_MICRO_SEGMENT_ID))
#define DNX_TUNNEL_TERM_SRV6_IS_CASCADED_MISS_DISABLE_TERM(_info)  (_SHR_IS_FLAG_SET(_info->flags, BCM_TUNNEL_TERM_CASCADED_MISS_DISABLE_TERM))

#define DNX_TUNNEL_TERM_SRV6_IS_SRV6_WITH_LOCATOR(_info) (DNX_TUNNEL_TERM_SRV6_IS_SRV6_WITH_96_LOCATOR(_info) \
                                                  || DNX_TUNNEL_TERM_SRV6_IS_SRV6_WITH_64_LOCATOR(_info) \
                                                  || DNX_TUNNEL_TERM_SRV6_IS_SRV6_WITH_GENERALIZED_SEGMENT_ID(_info) \
                                                  || DNX_TUNNEL_TERM_SRV6_IS_SRV6_WITH_MICRO_SEGMENT_ID(_info))

#define DNX_TUNNEL_TERM_SRV6_IS_SRV6_WITH_FUNCT(_info)  ((_info->type == bcmTunnelTypeCascadedFunct) || (_info->type == bcmTunnelTypeEthCascadedFunct))

#define DNX_TUNNEL_TERM_SRV6_IS_CASCADED_TYPE(_tunnel_type) ((_tunnel_type == bcmTunnelTypeCascadedFunct) || (_tunnel_type == bcmTunnelTypeEthCascadedFunct))

#define DNX_TUNNEL_TERM_SRV6_IS_STRICT_SRV6_TYPE(_tunnel_type) ((_tunnel_type == bcmTunnelTypeSR6) || (_tunnel_type == bcmTunnelTypeEthSR6))

#define DNX_TUNNEL_TERM_SRV6_IS_EXTENDED_TERMINATION(_info) (_SHR_IS_FLAG_SET(_info->flags, BCM_TUNNEL_TERM_EXTENDED_TERMINATION))

#define DNX_TUNNEL_TERM_SRV6_IS_SRV6_FIRST_ADDITIONAL_HEADER(_first_additional_header) (_first_additional_header == DBAL_ENUM_FVAL_IPV6_ADDITIONAL_HEADER_IPV6_SRV6)

#define DNX_TUNNEL_TERM_SRV6_IS_SRV6_ADDITIONAL_HEADER(_first_additional_header, _second_additional_header) (DNX_TUNNEL_TERM_SRV6_IS_SRV6_FIRST_ADDITIONAL_HEADER(_first_additional_header) \
                                                                                                                && (_second_additional_header == DBAL_ENUM_FVAL_IPV6_ADDITIONAL_HEADER_NOT_AVAILABLE))
#define DNX_TUNNEL_TERM_SRV6_IS_L2_VPN(_type) ((_type == bcmTunnelTypeEthSR6) || (_type == bcmTunnelTypeEthIn6) || (_type == bcmTunnelTypeEthCascadedFunct))

#define DNX_TUNNEL_TERM_SRV6_NEED_IN_LIF_PROFILE_UPDATE(_info)   (DNX_TUNNEL_TERM_SRV6_IS_SRV6_WITH_GENERALIZED_SEGMENT_ID_EOC(_info) || DNX_TUNNEL_TERM_SRV6_IS_SRV6_WITH_GENERALIZED_SEGMENT_ID(_info))

#define DNX_TUNNEL_TERM_SRV6_IS_SRV6_DBAL_TABLE(_dbal_table) \
        ((_dbal_table == DBAL_TABLE_IPV6_MP_TT_TCAM_2ND_PASS) \
        || (_dbal_table == DBAL_TABLE_IPV6_MP_TT_TCAM_BASIC) \
        || (_dbal_table == DBAL_TABLE_IPV6_TT_MP_EM_16))
/*
 * }
 */

shr_error_e dnx_tunnel_terminator_srv6_l2vpn_replace_type_verify(
    int unit,
    bcm_tunnel_terminator_t * info,
    uint32 fodo);

shr_error_e dnx_tunnel_terminator_srv6_l2vpn_type_get(
    int unit,
    bcm_tunnel_terminator_t * info,
    uint32 fodo,
    bcm_tunnel_type_t * tunnel_type);

shr_error_e dnx_tunnel_terminator_srv6_key_verify(
    int unit,
    bcm_tunnel_terminator_t * info);

shr_error_e dnx_tunnel_terminator_srv6_lif_verify(
    int unit,
    bcm_tunnel_terminator_t * info);

/**
 * \brief
 * Convert IPV6 tunnel type to IPv6 layer qualifier tunnel type
 * first and second additional headers. These fields are later
 * used by DBAL to identify the tunnel termination as part of
 * the look-up key.
 *
 * \par DIRECT INPUT
 *   \param [in] unit -
 *     Relevant unit.
 *   \param [in] tunnel_type -
 *     Tunnel termination type.
 *   \param [out] first_additional_header -
 *     IPv4/IPv6 first additional header.
 *   \param [out] second_additional_header -
 *     IPv4/IPv6 second additional header.
 */
shr_error_e dnx_tunnel_terminator_srv6_type_to_ipv6_additional_headers(
    int unit,
    bcm_tunnel_type_t tunnel_type,
    uint32 *first_additional_header,
    uint32 *second_additional_header);

/** \brief
 * The function returns bcm tunnel type based on additional headers
 */
shr_error_e dnx_tunnel_terminator_srv6_additional_headers_to_ipv6_type(
    int unit,
    uint32 first_additional_header,
    uint32 second_additional_header,
    bcm_tunnel_type_t * tunnel_type);

/*
 * \brief
 * For additional headers SRv6 we have two cases:
 * DBAL_ENUM_FVAL_IPV6_ADDITIONAL_HEADER_IPV6_SRV6 and DBAL_ENUM_FVAL_IPV6_ADDITIONAL_HEADER_IPV6_SRV6_SL_0
 * Both are valid under that tunnel_type, and they differ by the LSB only.
 * Thus use mask of 5b'1110
 */
void dnx_tunnel_terminator_srv6_ipv6_tcam_table_key_set_first_additional_header_set(
    int unit,
    uint32 entry_handle_id,
    uint32 first_additional_header);

void dnx_tunnel_terminator_srv6_to_ipv6_tcam_table_result_dbal_field(
    bcm_tunnel_terminator_t * info,
    dbal_fields_e * result_dbal_field);

shr_error_e dnx_tunnel_terminator_srv6_lookup_add_ipv6_cascaded_funct(
    int unit,
    bcm_tunnel_terminator_t * info,
    uint32 local_inlif);

shr_error_e dnx_tunnel_terminator_srv6_lookup_delete_ipv6_cascaded_funct(
    int unit,
    bcm_tunnel_terminator_t * info);

shr_error_e dnx_tunnel_terminator_srv6_lookup_get_ipv6_cascaded_funct(
    int unit,
    bcm_tunnel_terminator_t * info,
    uint32 *local_inlif);

void dnx_tunnel_terminator_srv6_optional_in_lif_profile_info_set(
    bcm_tunnel_terminator_t * info,
    in_lif_profile_info_t * in_lif_profile_info);

void dnx_tunnel_terminator_srv6_optional_in_lif_profile_update(
    bcm_tunnel_terminator_t * info,
    in_lif_profile_info_t * in_lif_profile_info);

shr_error_e dnx_tunnel_terminator_srv6_lif_reclassify(
    int unit,
    int global_lif,
    uint32 local_inlif);

void dnx_tunnel_terminator_srv6_optional_micro_segment_id_flag_set(
    dbal_enum_value_field_service_per_flow_e service_per_flow,
    uint32 *flags);

shr_error_e dnx_tunnel_terminator_srv6_gport_to_sid_format_add(
    int unit,
    bcm_tunnel_terminator_t * info);

shr_error_e dnx_tunnel_terminator_srv6_optional_flags_get(
    int unit,
    bcm_tunnel_terminator_t * info,
    uint32 global_inlif,
    uint32 in_lif_profile,
    uint32 *srv6_flags);

shr_error_e dnx_tunnel_terminator_srv6_optional_delete(
    int unit,
    bcm_tunnel_terminator_t * info,
    uint32 global_inlif);

shr_error_e dnx_tunnel_terminator_srv6_traverse_funct_key_fields(
    int unit,
    uint32 entry_handle_id,
    bcm_tunnel_terminator_t * info);

shr_error_e dnx_tunnel_terminator_srv6_optional_sid_format_flag_get(
    int unit,
    uint32 global_inlif,
    uint32 *flags);

void dnx_tunnel_terminator_srv6_ipv6_tcam_table_result_is_lif(
    dbal_fields_e result_dbal_field,
    uint8 *result_is_lif);

void dnx_tunnel_terminator_srv6_ipv6_tcam_result_flag_resolve(
    int unit,
    dbal_fields_e dbal_sub_field,
    uint32 *flags);
