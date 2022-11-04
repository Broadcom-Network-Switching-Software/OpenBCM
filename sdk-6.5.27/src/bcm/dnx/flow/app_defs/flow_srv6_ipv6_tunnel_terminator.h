 /** \file flow_srv6_ipv6_tunnel_terminator.h
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 */

#define DNX_SRV6_IPV6_TERMINATOR_DIP_IDX_TYPE_UNKNOWN                           0
#define DNX_SRV6_IPV6_TERMINATOR_DIP_IDX_TYPE_IPV6_MP_TUNNEL                    1
#define DNX_SRV6_IPV6_TERMINATOR_DIP_IDX_TYPE_CLASSIC_SID_LOCATER64             2
#define DNX_SRV6_IPV6_TERMINATOR_DIP_IDX_TYPE_CLASSIC_SID_LOCATER64_NO_DEFAULT  3
#define DNX_SRV6_IPV6_TERMINATOR_DIP_IDX_TYPE_CLASSIC_SID_LOCATER96             4
#define DNX_SRV6_IPV6_TERMINATOR_DIP_IDX_TYPE_CLASSIC_SID_LOCATER96_NO_DEFAULT  5
#define DNX_SRV6_IPV6_TERMINATOR_DIP_IDX_TYPE_GSID_PREFIX_GSID_16               6
#define DNX_SRV6_IPV6_TERMINATOR_DIP_IDX_TYPE_GSID_PREFIX_GSID_NO_DEFAULT       7
#define DNX_SRV6_IPV6_TERMINATOR_DIP_IDX_TYPE_USID_PREFIX_USID_16               8
#define DNX_SRV6_IPV6_TERMINATOR_DIP_IDX_TYPE_USID_PREFIX_USID_16_NO_DEFAULT    9
#define DNX_SRV6_IPV6_TERMINATOR_DIP_IDX_TYPE_GENERAL_CASCADED_WITH_DEFAULT     10
#define DNX_SRV6_IPV6_TERMINATOR_DIP_IDX_TYPE_GENERAL_CASCADED_NO_DEFAULT       11

int dnx_ipv6_srv6_is_gsid_type(
    int unit,
    bcm_flow_special_fields_t * special_fields);

int dnx_ipv6_srv6_is_usid_type(
    int unit,
    bcm_flow_special_fields_t * special_fields);
