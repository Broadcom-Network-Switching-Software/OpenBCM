/*
 * ! \file bcm_int/dnx/l3/l3.h Internal DNX TT APIs
PIs This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
PIs 
PIs Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
/*
 * Include files.
 * {
 */
#include <bcm/types.h>
#include <bcm/tunnel.h>
#include <bcm/error.h>
#include <bcm_int/dnx/lif/in_lif_profile.h>

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
    dbal_enum_value_field_ipv6_dip_idx_type_e * dip_idx_type_value);

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

shr_error_e dnx_tunnel_terminator_srv6_optional_in_lif_profile_info_set(
    int unit,
    bcm_tunnel_terminator_t * info,
    in_lif_profile_info_t * in_lif_profile_info);

void dnx_tunnel_terminator_srv6_optional_in_lif_profile_update(
    bcm_tunnel_terminator_t * info,
    in_lif_profile_info_t * in_lif_profile_info);

shr_error_e dnx_tunnel_terminator_srv6_lif_reclassify(
    int unit,
    int global_lif,
    uint32 local_inlif);

shr_error_e dnx_tunnel_terminator_srv6_gport_to_dip_idx_type_add(
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

shr_error_e dnx_tunnel_terminator_srv6_optional_dip_idx_type_flag_get(
    int unit,
    uint32 global_inlif,
    uint32 *flags);

void dnx_tunnel_terminator_srv6_ipv6_tcam_table_result_is_lif(
    dbal_enum_value_field_ipv6_dip_idx_type_e dip_idx_type,
    uint8 *result_is_lif);

void dnx_tunnel_terminator_srv6_ipv6_tcam_result_flag_resolve(
    int unit,
    dbal_enum_value_field_ipv6_dip_idx_type_e dip_idx_type,
    uint32 *flags);
