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
#  error "BSL_LOG_MODULE redefined"
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

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(algo_tunnel_db.init(unit));

    /*
     * Init IPV6 SIP profile id template resource pool
     */
    sal_memset(&template_data, 0, sizeof(dnx_algo_template_create_data_t));
    template_data.flags = 0;
    template_data.data_size = DNX_ALGO_TUNNEL_IPV6_SIP_PROFILE_TEMPLATE_SIZE;
    template_data.first_profile = 0;

    SHR_IF_ERR_EXIT(dbal_tables_capacity_get(unit, DBAL_TABLE_IPV6_TT_SIP_MSB_COMPRESS_EM, &max_references));

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

/*
 * } 
 */
