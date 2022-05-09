/** \file tunnel_term_srv6_utils.h
 *  SRV6 APIs used for DNX TUNNEL termination functionality.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

/*
 * Include files.
 * {
 */
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

#define DNX_TUNNEL_TERM_SRV6_IS_EXTENDED_TERMINATION(_info) (_SHR_IS_FLAG_SET(_info->flags, BCM_TUNNEL_TERM_EXTENDED_TERMINATION))

#define DNX_TUNNEL_TERM_SRV6_IS_IP_TUNNEL_TYPE(_info) ((_info->type == bcmTunnelTypeIpAnyIn6) || (_info->type == bcmTunnelTypeEthIn6))

#define DNX_TUNNEL_TERM_SRV6_IS_SRV6_FIRST_ADDITIONAL_HEADER(_first_additional_header) (_first_additional_header == DBAL_ENUM_FVAL_IPV6_ADDITIONAL_HEADER_IPV6_SRV6)

#define DNX_TUNNEL_TERM_SRV6_IS_SRV6_ADDITIONAL_HEADER(_first_additional_header, _second_additional_header) (DNX_TUNNEL_TERM_SRV6_IS_SRV6_FIRST_ADDITIONAL_HEADER(_first_additional_header) \
                                                                                                                && (_second_additional_header == DBAL_ENUM_FVAL_IPV6_ADDITIONAL_HEADER_NOT_AVAILABLE))
#define DNX_TUNNEL_TERM_SRV6_IS_L2_VPN(_type) ((_type == bcmTunnelTypeEthIn6) || (_type == bcmTunnelTypeEthCascadedFunct))

#define DNX_TUNNEL_TERM_SRV6_NEED_IN_LIF_PROFILE_UPDATE(_info)   (DNX_TUNNEL_TERM_SRV6_IS_SRV6_WITH_GENERALIZED_SEGMENT_ID_EOC(_info) || DNX_TUNNEL_TERM_SRV6_IS_SRV6_WITH_GENERALIZED_SEGMENT_ID(_info))

#define DNX_TUNNEL_TERM_SRV6_IS_SRV6_CREATION(_info) \
               DNX_TUNNEL_TERM_SRV6_IS_EXTENDED_TERMINATION(_info) \
            || DNX_TUNNEL_TERM_SRV6_IS_SRV6_WITH_LOCATOR(_info) \
            || DNX_TUNNEL_TERM_SRV6_IS_SRV6_WITH_FUNCT(_info)

#define DNX_TUNNEL_TERM_SRV6_IS_SRV6_DBAL_TABLE(_dbal_table) \
        ((_dbal_table == DBAL_TABLE_IPV6_MP_TT_TCAM_2ND_PASS) \
        || (_dbal_table == DBAL_TABLE_IPV6_MP_TT_TCAM_BASIC) \
        || (_dbal_table == DBAL_TABLE_IPV6_TT_MP_EM_16))

/*
 * }
 */
shr_error_e dnx_tunnel_terminator_srv6_funct_value_get(
    int unit,
    bcm_tunnel_terminator_t * info,
    uint32 *funct);

shr_error_e dnx_tunnel_terminator_srv6_info_to_dip_idx_type(
    int unit,
    bcm_tunnel_terminator_t * info,
    dbal_enum_value_field_ipv6_dip_idx_type_e * dip_idx_type);
