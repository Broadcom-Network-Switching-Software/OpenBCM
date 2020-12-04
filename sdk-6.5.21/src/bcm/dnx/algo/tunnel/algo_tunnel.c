/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/** \file algo_tunnel.c
 *
 *  tunnel algorithms initialization and deinitialization.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_TUNNEL

/*************
 * INCLUDES  *
 */
/*
 * { 
 */
#include <shared/shrextend/shrextend_debug.h>
#include <bcm_int/dnx/algo/tunnel/algo_tunnel.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_tunnel.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_lif.h>
#include <soc/dnx/dbal/dbal.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/algo_tunnel_access.h>
#include <soc/dnx/utils/dnx_pp_programmability_utils.h>
/*
 * } 
 */
/*************
 * DEFINES   *
 */
/*
 * { 
 */

/*
 * } 
 */
/*************
 * MACROS    *
 */
/*
 * { 
 */

/*
 * } 
 */
/*************
 * TYPE DEFS *
 */
/*
 * { 
 */

/*
 * } 
 */
/*************
 * GLOBALS   *
 */
/*
 * { 
 */

/*
 * } 
 */
/*************
 * FUNCTIONS *
 */
/*
 * { 
 */
/**
* \brief
*   Print an entry of ipv6 sip tunnel profile template.
*/
void
dnx_algo_tunnel_ipv6_sip_profile_template_print_cb(
    int unit,
    const void *data)
{
    bcm_ip6_t *ipv6 = (bcm_ip6_t *) data;

    SW_STATE_PRETTY_PRINT_INIT_VARIABLES();

    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_IPV6, "Source IPv6 address", ipv6, "", "");

    SW_STATE_PRETTY_PRINT_FINISH();
}

/**
* \brief
*   Print an entry of UDP ports profile entry.
*/
void
dnx_algo_tunnel_udp_ports_profile_template_print_cb(
    int unit,
    const void *data)
{
    udp_ports_t *ports = (udp_ports_t *) data;

    SW_STATE_PRETTY_PRINT_INIT_VARIABLES();

    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT16, "UDP destination port", ports->dst_port, "", "");

    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_UINT16, "UDP source port", ports->src_port, "", "");

    SW_STATE_PRETTY_PRINT_FINISH();
}

/**
 * \brief 
 * allocate resource pool for all TUNNEL profiles types
 */
shr_error_e
dnx_algo_tunnel_init(
    int unit)
{
    dnx_algo_template_create_data_t template_data;
    int max_references;
    uint8 is_std_1;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(algo_tunnel_db.init(unit));

    /*
     * Init IPV6 SIP profile id template resource pool
     */
    sal_memset(&template_data, 0, sizeof(dnx_algo_template_create_data_t));
    template_data.flags = 0;
    template_data.data_size = DNX_ALGO_TUNNEL_IPV6_SIP_PROFILE_TEMPLATE_SIZE;
    template_data.first_profile = 0;

    SHR_IF_ERR_EXIT(dnx_pp_prgm_default_image_check(unit, &is_std_1));

    if (!is_std_1)
    {
        max_references = DNX_PP_NON_STD_1_DEFAULT_NOF_ELEMENTS;
    }
    else
    {
        SHR_IF_ERR_EXIT(dbal_tables_capacity_get(unit, DBAL_TABLE_IPV6_TT_SIP_MSB_COMPRESS_EM, &max_references));
    }

    template_data.max_references = max_references;
    template_data.nof_profiles = dnx_data_tunnel.ipv6.nof_terminated_sips_get(unit);
    sal_strncpy(template_data.name, DNX_ALGO_TUNNEL_IPV6_SIP_PROFILE, DNX_ALGO_TEMPLATE_MNGR_MAX_NAME_LENGTH);

    SHR_IF_ERR_EXIT(algo_tunnel_db.tunnel_ipv6_sip_profile.create(unit, &template_data, NULL));

    /*
     * Init UDP ports profile.
     */
    sal_memset(&template_data, 0, sizeof(dnx_algo_template_create_data_t));
    template_data.flags = 0;
    template_data.data_size = sizeof(udp_ports_t);
    template_data.first_profile = dnx_data_tunnel.udp.first_udp_ports_profile_get(unit);

    template_data.max_references = dnx_data_lif.out_lif.nof_local_out_lifs_get(unit);
    template_data.nof_profiles = dnx_data_tunnel.udp.nof_udp_ports_profiles_get(unit);
    sal_strncpy(template_data.name, DNX_ALGO_TUNNEL_UDP_PORTS_PROFILE, DNX_ALGO_TEMPLATE_MNGR_MAX_NAME_LENGTH);

    SHR_IF_ERR_EXIT(algo_tunnel_db.udp_ports_profile.create(unit, &template_data, NULL));

exit:
    SHR_FUNC_EXIT;
}

/**
 * See tunnel_init.h
*/
shr_error_e
dnx_algo_tunnel_initiator_gport_get(
    int unit,
    bcm_if_t intf_id,
    bcm_tunnel_initiator_t * tunnel,
    bcm_gport_t * tunnel_gport,
    int *is_tunneled)
{
    bcm_gport_t gport_from_l3_intf;
    int gport_from_l3_intf_valid;
    int gport_from_tunnel_valid;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(tunnel, _SHR_E_PARAM, "tunnel");
    SHR_NULL_CHECK(tunnel_gport, _SHR_E_PARAM, "tunnel_gport");
    SHR_NULL_CHECK(is_tunneled, _SHR_E_PARAM, "is_tunneled");

    *is_tunneled = 0;
    gport_from_l3_intf_valid = 0;
    gport_from_tunnel_valid = 0;

    /** get from tunnel interface */
    if (BCM_GPORT_IS_TUNNEL(tunnel->tunnel_id))
    {
        gport_from_tunnel_valid = 1;
    }

    /** get from l3 intf */
    BCM_L3_ITF_LIF_TO_GPORT_TUNNEL(gport_from_l3_intf, intf_id);
    if (BCM_GPORT_IS_TUNNEL(gport_from_l3_intf))
    {
        gport_from_l3_intf_valid = 1;
    }

     /** check that tunnel and l3 intf have same GPort */
    if (gport_from_l3_intf_valid && gport_from_tunnel_valid && (tunnel->tunnel_id != gport_from_l3_intf))
    {
        SHR_ERR_EXIT(BCM_E_PARAM, "tunnel-id given in interface and tunnel-gport mismatch");
    }

    /** Set found tunnel GPort value and indication */
    if (gport_from_l3_intf_valid)
    {
        *tunnel_gport = gport_from_l3_intf;
        *is_tunneled = 1;
    }
    else if (gport_from_tunnel_valid)
    {
        *tunnel_gport = tunnel->tunnel_id;
        *is_tunneled = 1;
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * return weather tunnel type is ipv6
 */
void
dnx_algo_tunnel_init_ipv6_is_ipv6_tunnel(
    int unit,
    bcm_tunnel_type_t tunnel_type,
    uint32 *is_ipv6)
{
    switch (tunnel_type)
    {
        case bcmTunnelTypeIpAnyIn6:
        case bcmTunnelTypeGreAnyIn6:
        case bcmTunnelTypeVxlan6:
        case bcmTunnelTypeVxlan6Gpe:
        case bcmTunnelTypeUdp6:
        case bcmTunnelTypeSR6:
        case bcmTunnelTypeEthSR6:
        case bcmTunnelTypeEthIn6:
            *is_ipv6 = TRUE;
            break;
        default:
            *is_ipv6 = FALSE;
            break;
    }
}
/**
 * See tunnel_init.h
*/
shr_error_e
dnx_algo_tunnel_global_out_lif_alloc_info_set(
    int unit,
    bcm_tunnel_initiator_t * tunnel,
    global_lif_alloc_info_t * alloc_info)
{
    uint32 tunnel_is_ipv6;
    SHR_FUNC_INIT_VARS(unit);
    /** value -1 means that estimated_encap_size will be determine internally by sdk and not by the user */
    if (tunnel->estimated_encap_size == -1)
    {
        dnx_algo_tunnel_init_ipv6_is_ipv6_tunnel(unit, tunnel->type, &tunnel_is_ipv6);
        if (tunnel_is_ipv6 == TRUE)
        {
            alloc_info->bta = dnx_data_lif.out_lif.global_ipv6_outlif_bta_sop_get(unit);
        }
        else if ((tunnel->type == bcmTunnelTypeVxlan) || (tunnel->type == bcmTunnelTypeVxlanGpe))
        {
            alloc_info->bta = dnx_data_lif.out_lif.global_vxlan_outlif_bta_sop_get(unit);
        }
        else
        {
            alloc_info->bta = 0;
        }
    }
    else
    {
        alloc_info->bta = tunnel->estimated_encap_size;
    }

/** exit: */
    SHR_FUNC_EXIT;
}
/*
 * } 
 */
