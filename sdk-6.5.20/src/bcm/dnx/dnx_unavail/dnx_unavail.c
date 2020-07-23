/** \file dnx_unavail.c
 * 
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_VLAN
/*
 * Include files.
 * {
 */
#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnx/dbal/dbal.h>
#include <soc/dnx/dnx_err_recovery_manager.h>
#include <bcm/types.h>
#include <bcm_int/dnx/port/port_pp.h>
#include <bcm_int/dnx/algo/algo_gpm.h>
#include <bcm_int/dnx/vlan/vlan.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_l2.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/vlan_access.h>
#include <bcm_int/dnx_dispatch.h>
#include <bcm/init.h>
#include <bcm/qos.h>
#include <bcm/stg.h>
#include <bcm/ipsec.h>
#include <bcm/knet.h>
#include <bcm/lb.h>
#include <bcm/failover.h>
#include <bcm/mirror.h>
#include <bcm/vswitch.h>
#include <bcm/nat.h>
#if defined(INCLUDE_PTP)
#include <bcm/ptp.h>
#endif
/*
 * }
 */

/*
 * MACROs
 * {
 */
/*
 * }
 */

/**
 * \brief
 *   APIs for Jericho2
 */

int
bcm_dnx_qos_port_vlan_map_get(
    int unit,
    bcm_port_t port,
    bcm_vlan_t vid,
    int *ing_map,
    int *egr_map)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL,
                 "API unavailable, instead use bcm_vlan_port_find() followed by bcm_qos_port_map_get()");

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_qos_port_vlan_map_set(
    int unit,
    bcm_port_t port,
    bcm_vlan_t vid,
    int ing_map,
    int egr_map)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL,
                 "API unavailable, instead use bcm_vlan_port_create() followed by bcm_qos_port_map_set()");

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_stk_modport_remote_map_get(
    int unit,
    bcm_gport_t port,
    uint32 flags,
    int *remote_modid,
    bcm_port_t * remote_port)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_stk_modport_remote_map_set(
    int unit,
    bcm_gport_t port,
    uint32 flags,
    int remote_modid,
    bcm_port_t remote_port)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_stk_system_gport_map_create(
    int unit,
    bcm_stk_system_gport_map_t * sys_gport_map)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_stk_system_gport_map_destroy(
    int unit,
    bcm_gport_t system_gport)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_stk_system_gport_map_get(
    int unit,
    bcm_stk_system_gport_map_t * sys_gport_map)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_switch_stable_register(
    int unit,
    bcm_switch_read_func_t rf,
    bcm_switch_write_func_t wf)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_switch_encap_create(
    int unit,
    bcm_switch_encap_info_t * encap_info,
    bcm_if_t * encap_id)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_switch_encap_destroy(
    int unit,
    bcm_if_t encap_id)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_switch_encap_destroy_all(
    int unit)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_switch_encap_get(
    int unit,
    bcm_if_t encap_id,
    bcm_switch_encap_info_t * encap_info)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_switch_encap_set(
    int unit,
    bcm_if_t encap_id,
    bcm_switch_encap_info_t * encap_info)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_switch_encap_traverse(
    int unit,
    bcm_switch_encap_traverse_cb cb_fn,
    void *user_data)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_switch_ipv6_reserved_multicast_addr_get(
    int unit,
    bcm_ip6_t * ip6_addr,
    bcm_ip6_t * ip6_mask)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_switch_ipv6_reserved_multicast_addr_set(
    int unit,
    bcm_ip6_t ip6_addr,
    bcm_ip6_t ip6_mask)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_switch_match_config_add(
    int unit,
    bcm_switch_match_service_t match_service,
    bcm_switch_match_config_info_t * config_info,
    int *match_id)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_switch_match_config_delete(
    int unit,
    bcm_switch_match_service_t match_service,
    int match_id)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_switch_match_config_delete_all(
    int unit,
    bcm_switch_match_service_t match_service)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_switch_match_config_get(
    int unit,
    bcm_switch_match_service_t match_service,
    int match_id,
    bcm_switch_match_config_info_t * config_info)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_switch_match_config_set(
    int unit,
    bcm_switch_match_service_t match_service,
    int match_id,
    bcm_switch_match_config_info_t * config_info)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_switch_match_config_traverse(
    int unit,
    bcm_switch_match_service_t match_service,
    bcm_switch_match_config_traverse_cb cb_fn,
    void *user_data)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_switch_match_control_get(
    int unit,
    bcm_switch_match_service_t match_service,
    bcm_switch_match_control_type_t control_type,
    bcm_gport_t gport,
    bcm_switch_match_control_info_t * control_info)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_switch_match_control_set(
    int unit,
    bcm_switch_match_service_t match_service,
    bcm_switch_match_control_type_t control_type,
    bcm_gport_t gport,
    bcm_switch_match_control_info_t * control_info)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_switch_match_control_traverse(
    int unit,
    bcm_switch_match_service_t match_service,
    bcm_switch_match_control_traverse_cb cb_fn,
    void *user_data)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_switch_olp_l2_addr_add(
    int unit,
    uint32 options,
    bcm_switch_olp_l2_addr_t * l2_addr)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_switch_olp_l2_addr_delete(
    int unit,
    bcm_switch_olp_l2_addr_t * l2_addr)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_switch_olp_l2_addr_delete_all(
    int unit)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_switch_olp_l2_addr_get(
    int unit,
    bcm_switch_olp_l2_addr_t * l2_addr)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_switch_olp_l2_addr_traverse(
    int unit,
    bcm_switch_olp_l2_addr_traverse_cb cb_fn,
    void *user_data)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_switch_user_buffer_read(
    int unit,
    uint32 flags,
    bcm_switch_user_buffer_type_t buff_type,
    uint8 *buf,
    int offset,
    int nbytes)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_switch_user_buffer_write(
    int unit,
    uint32 flags,
    bcm_switch_user_buffer_type_t buff_type,
    uint8 *buf,
    int offset,
    int nbytes)
{
    return BCM_E_UNAVAIL;
}

#if defined(INCLUDE_L3)
int
bcm_dnx_mpls_exp_map_create(
    int unit,
    uint32 flags,
    int *exp_map_id)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_L3)
int
bcm_dnx_mpls_exp_map_destroy(
    int unit,
    int exp_map_id)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_L3)
int
bcm_dnx_mpls_exp_map_get(
    int unit,
    int exp_map_id,
    bcm_mpls_exp_map_t * exp_map)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_L3)
int
bcm_dnx_mpls_exp_map_set(
    int unit,
    int exp_map_id,
    bcm_mpls_exp_map_t * exp_map)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_L3)
int
bcm_dnx_mpls_tunnel_initiator_set(
    int unit,
    bcm_if_t intf,
    int num_labels,
    bcm_mpls_egress_label_t * label_array)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_L3)
int
bcm_dnx_mpls_tunnel_switch_add(
    int unit,
    bcm_mpls_tunnel_switch_t * info)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_BFD)
int
bcm_dnx_bfd_auth_sha1_get(
    int unit,
    int index,
    bcm_bfd_auth_sha1_t * sha1)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API unavailable, authentication no longer supported");

exit:
    SHR_FUNC_EXIT;
}
#endif

#if defined(INCLUDE_BFD)
int
bcm_dnx_bfd_auth_sha1_set(
    int unit,
    int index,
    bcm_bfd_auth_sha1_t * sha1)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API unavailable, authentication no longer supported");

exit:
    SHR_FUNC_EXIT;
}
#endif

#if defined(INCLUDE_BFD)
int
bcm_dnx_bfd_auth_simple_password_get(
    int unit,
    int index,
    bcm_bfd_auth_simple_password_t * sp)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API unavailable, authentication no longer supported");

exit:
    SHR_FUNC_EXIT;
}
#endif

#if defined(INCLUDE_BFD)
int
bcm_dnx_bfd_auth_simple_password_set(
    int unit,
    int index,
    bcm_bfd_auth_simple_password_t * sp)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API unavailable, authentication no longer supported");

exit:
    SHR_FUNC_EXIT;
}
#endif

#if defined(INCLUDE_BFD)
int
bcm_dnx_bfd_endpoint_poll(
    int unit,
    bcm_bfd_endpoint_t endpoint)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API unavailable, statistics can be accessed per endpoint");

exit:
    SHR_FUNC_EXIT;
}
#endif

#if defined(INCLUDE_BFD)
int
bcm_dnx_bfd_endpoint_stat_get(
    int unit,
    bcm_bfd_endpoint_t endpoint,
    bcm_bfd_endpoint_stat_t * ctr_info,
    uint32 options)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API unavailable, statistics can be accessed per endpoint");

exit:
    SHR_FUNC_EXIT;
}
#endif

#if defined(INCLUDE_BFD)
int
bcm_dnx_bfd_tx_start(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API unavailable, start/stop no longer supported");

exit:
    SHR_FUNC_EXIT;
}
#endif

#if defined(INCLUDE_BFD)
int
bcm_dnx_bfd_tx_stop(
    int unit)
{
    return BCM_E_UNAVAIL;
}
#endif

int
bcm_dnx_vlan_mcast_flood_get(
    int unit,
    bcm_vlan_t vlan,
    bcm_vlan_mcast_flood_t * mode)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_vlan_mcast_flood_set(
    int unit,
    bcm_vlan_t vlan,
    bcm_vlan_mcast_flood_t mode)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_vlan_vector_flags_set(
    int unit,
    bcm_vlan_vector_t vlan_vector,
    uint32 flags_mask,
    uint32 flags_value)
{
    return BCM_E_UNAVAIL;
}

#if defined(INCLUDE_L3)
int
bcm_dnx_ipmc_range_delete(
    int unit,
    int range_id)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_L3)
int
bcm_dnx_ipmc_range_delete_all(
    int unit)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_L3)
int
bcm_dnx_ipmc_range_get(
    int unit,
    int range_id,
    bcm_ipmc_range_t * range)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_L3)
int
bcm_dnx_ipmc_range_size_get(
    int unit,
    int *size)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_L3)
int
bcm_dnx_ipmc_stat_attach(
    int unit,
    bcm_ipmc_addr_t * info,
    uint32 stat_counter_id)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_L3)
int
bcm_dnx_ipmc_stat_counter_get(
    int unit,
    bcm_ipmc_addr_t * info,
    bcm_ipmc_stat_t stat,
    uint32 num_entries,
    uint32 *counter_indexes,
    bcm_stat_value_t * counter_values)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_L3)
int
bcm_dnx_ipmc_stat_counter_set(
    int unit,
    bcm_ipmc_addr_t * info,
    bcm_ipmc_stat_t stat,
    uint32 num_entries,
    uint32 *counter_indexes,
    bcm_stat_value_t * counter_values)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_L3)
int
bcm_dnx_ipmc_stat_counter_sync_get(
    int unit,
    bcm_ipmc_addr_t * info,
    bcm_ipmc_stat_t stat,
    uint32 num_entries,
    uint32 *counter_indexes,
    bcm_stat_value_t * counter_values)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_L3)
int
bcm_dnx_ipmc_stat_detach(
    int unit,
    bcm_ipmc_addr_t * info)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_L3)
int
bcm_dnx_ipmc_stat_id_get(
    int unit,
    bcm_ipmc_addr_t * info,
    bcm_ipmc_stat_t stat,
    uint32 *stat_counter_id)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_L3)
int
bcm_dnx_ipmc_stat_multi_get(
    int unit,
    bcm_ipmc_addr_t * info,
    int nstat,
    bcm_ipmc_stat_t * stat_arr,
    uint64 *value_arr)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_L3)
int
bcm_dnx_ipmc_stat_multi_get32(
    int unit,
    bcm_ipmc_addr_t * info,
    int nstat,
    bcm_ipmc_stat_t * stat_arr,
    uint32 *value_arr)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_L3)
int
bcm_dnx_ipmc_stat_multi_set(
    int unit,
    bcm_ipmc_addr_t * info,
    int nstat,
    bcm_ipmc_stat_t * stat_arr,
    uint64 *value_arr)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_L3)
int
bcm_dnx_ipmc_stat_multi_set32(
    int unit,
    bcm_ipmc_addr_t * info,
    int nstat,
    bcm_ipmc_stat_t * stat_arr,
    uint32 *value_arr)
{
    return BCM_E_UNAVAIL;
}
#endif

int
bcm_dnx_l2_age_timer_set(
    int unit,
    int age_seconds)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API unavailable, instead use bcm_l2_age_timer_meta_cycle_set()");

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_l2_age_timer_get(
    int unit,
    int *age_seconds)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API unavailable, instead use bcm_l2_age_timer_meta_cycle_get()");

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_l2_cache_delete(
    int unit,
    int index)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API unavailable, instead use bcm_rx_trap_protocol_set()");

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_l2_cache_delete_all(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API unavailable, instead use bcm_rx_trap_protocol_set()");

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_l2_cache_get(
    int unit,
    int index,
    bcm_l2_cache_addr_t * addr)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API unavailable, instead use bcm_rx_trap_protocol_get()");

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_l2_cache_profile_get(
    int unit,
    int trap_type,
    uint32 profile_idx,
    uint32 flags,
    bcm_l2cp_profile_info_t * l2cp_profile_info)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API unavailable, instead use bcm_rx_trap_protocol_get()");

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_l2_cache_profile_set(
    int unit,
    int trap_type,
    uint32 profile_idx,
    uint32 flags,
    bcm_l2cp_profile_info_t * l2cp_profile_info)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API unavailable, instead use bcm_rx_trap_protocol_set()");

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_l2_cache_set(
    int unit,
    int index,
    bcm_l2_cache_addr_t * addr,
    int *index_used)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API unavailable, instead use bcm_rx_trap_protocol_set()");

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_l2_cache_size_get(
    int unit,
    int *size)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API unavailable, instead use bcm_rx_trap_protocol_get()");

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_l2_cache_vpn_to_profile_map_get(
    int unit,
    uint32 vsi,
    uint32 *profile_idx)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API unavailable, instead use bcm_rx_trap_protocol_get()");

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_l2_cache_vpn_to_profile_map_set(
    int unit,
    uint32 vsi,
    uint32 profile_idx)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API unavailable, instead use bcm_rx_trap_protocol_set()");

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_l2_tunnel_add(
    int unit,
    bcm_mac_t mac,
    bcm_vlan_t vlan)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_l2_tunnel_delete(
    int unit,
    bcm_mac_t mac,
    bcm_vlan_t vlan)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_l2_tunnel_delete_all(
    int unit)
{
    return BCM_E_UNAVAIL;
}

#if defined(INCLUDE_L3)
#if defined(INCLUDE_L3)
int
bcm_dnx_vxlan_port_delete_all(
    int unit,
    bcm_vpn_t l2vpn)
{
    return BCM_E_UNAVAIL;
}
#endif
#endif

#if defined(INCLUDE_L3)
#if defined(INCLUDE_L3)
int
bcm_dnx_vxlan_port_get_all(
    int unit,
    bcm_vpn_t l2vpn,
    int port_max,
    bcm_vxlan_port_t * port_array,
    int *port_count)
{
    return BCM_E_UNAVAIL;
}
#endif
#endif

#if defined(INCLUDE_L3)
#if defined(INCLUDE_L3)
int
bcm_dnx_vxlan_port_traverse(
    int unit,
    bcm_vxlan_port_traverse_cb cb,
    void *user_data)
{
    return BCM_E_UNAVAIL;
}
#endif
#endif

#if defined(INCLUDE_L3)
#if defined(INCLUDE_L3)
int
bcm_dnx_vxlan_vpn_destroy_all(
    int unit)
{
    return BCM_E_UNAVAIL;
}
#endif
#endif

#if defined(INCLUDE_L3)
#if defined(INCLUDE_L3)
int
bcm_dnx_vxlan_vpn_traverse(
    int unit,
    bcm_vxlan_vpn_traverse_cb cb,
    void *user_data)
{
    return BCM_E_UNAVAIL;
}
#endif
#endif

int
bcm_dnx_port_cfi_color_get(
    int unit,
    bcm_port_t port,
    int cfi,
    bcm_color_t * color)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API unavailable, instead use QoS mapping APIs for port mapping");
exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_port_cfi_color_set(
    int unit,
    bcm_port_t port,
    int cfi,
    bcm_color_t color)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API unavailable, instead use QoS mapping APIs for port mapping");

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_port_discard_get(
    int unit,
    bcm_port_t port,
    int *mode)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API unavailable, instead use bcm_rx_trap_lif_get()");

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_port_discard_set(
    int unit,
    bcm_port_t port,
    int mode)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API unavailable, instead use bcm_rx_trap_lif_set()");

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_port_dscp_map_get(
    int unit,
    bcm_port_t port,
    int srccp,
    int *mapcp,
    int *prio)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API unavailable, instead use QoS mapping APIs");

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_port_dscp_map_mode_get(
    int unit,
    bcm_port_t port,
    int *mode)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API unavailable, instead use QoS mapping APIs");

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_port_dscp_map_mode_set(
    int unit,
    bcm_port_t port,
    int mode)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API unavailable, instead use QoS mapping APIs");

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_port_dscp_map_set(
    int unit,
    bcm_port_t port,
    int srccp,
    int mapcp,
    int prio)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API unavailable, instead use QoS mapping APIs");

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_port_encap_map_get(
    int unit,
    uint32 flags,
    bcm_if_t encap_id,
    bcm_gport_t * port)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_port_encap_map_set(
    int unit,
    uint32 flags,
    bcm_if_t encap_id,
    bcm_gport_t port)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_port_inner_tpid_get(
    int unit,
    bcm_port_t port,
    uint16 *tpid)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API unavailable, instead use bcm_switch_tpid_get_all() per device");

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_port_inner_tpid_set(
    int unit,
    bcm_port_t port,
    uint16 tpid)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API unavailable, instead use bcm_switch_tpid_add() per device");

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_port_priority_color_get(
    int unit,
    bcm_port_t port,
    int prio,
    bcm_color_t * color)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API unavailable, instead use QoS mapping APIs for port mapping");

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_port_priority_color_set(
    int unit,
    bcm_port_t port,
    int prio,
    bcm_color_t color)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API unavailable, instead use QoS mapping APIs for port mapping");

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_port_tpid_add(
    int unit,
    bcm_port_t port,
    uint16 tpid,
    int color_select)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API unavailable, instead use bcm_switch_tpid_add() per device");

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_port_tpid_delete(
    int unit,
    bcm_port_t port,
    uint16 tpid)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API unavailable, instead use bcm_switch_tpid_delete() per device");

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_port_tpid_delete_all(
    int unit,
    bcm_port_t port)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API unavailable, instead use bcm_switch_tpid_delete_all() per device");

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_port_tpid_get(
    int unit,
    bcm_port_t port,
    uint16 *tpid)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API unavailable, instead use bcm_switch_tpid_get_all() per device");

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_port_tpid_set(
    int unit,
    bcm_port_t port,
    uint16 tpid)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API unavailable, instead use bcm_switch_tpid_add() per device");

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_port_untagged_priority_get(
    int unit,
    bcm_port_t port,
    int *priority)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API unavailable, instead use QoS mapping APIs for Untagged mapping");

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_port_untagged_priority_set(
    int unit,
    bcm_port_t port,
    int priority)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API unavailable, instead use QoS mapping APIs for Untagged mapping");

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_port_vlan_priority_map_get(
    int unit,
    bcm_port_t port,
    int pkt_pri,
    int cfi,
    int *internal_pri,
    bcm_color_t * color)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API unavailable, instead use QoS mapping APIs");

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_port_vlan_priority_map_set(
    int unit,
    bcm_port_t port,
    int pkt_pri,
    int cfi,
    int internal_pri,
    bcm_color_t color)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API unavailable, instead use bcm_vlan_port_find()");

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_port_vlan_vector_get(
    int unit,
    bcm_gport_t port_id,
    bcm_vlan_vector_t vlan_vec)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_port_vlan_vector_set(
    int unit,
    bcm_gport_t port_id,
    bcm_vlan_vector_t vlan_vec)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_extender_forward_add(
    int unit,
    bcm_extender_forward_t * extender_forward_entry)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API unavailable, instead use bcm_extender_port_add()");

exit:
    SHR_FUNC_EXIT;
}

#if defined(INCLUDE_L3)
int
bcm_dnx_extender_forward_delete(
    int unit,
    bcm_extender_forward_t * extender_forward_entry)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API unavailable, instead use bcm_extender_port_delete()");

exit:
    SHR_FUNC_EXIT;
}
#endif

#if defined(INCLUDE_L3)
int
bcm_dnx_extender_forward_delete_all(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API unavailable, instead use bcm_extender_port_delete_all()");

exit:
    SHR_FUNC_EXIT;
}
#endif

#if defined(INCLUDE_L3)
int
bcm_dnx_extender_forward_get(
    int unit,
    bcm_extender_forward_t * extender_forward_entry)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API unavailable, instead use bcm_extender_port_get()");

exit:
    SHR_FUNC_EXIT;
}
#endif

#if defined(INCLUDE_L3)
int
bcm_dnx_l3_egress_find(
    int unit,
    bcm_l3_egress_t * egr,
    bcm_if_t * intf)
{
    return BCM_E_UNAVAIL;
}
#endif

int
bcm_dnx_l3_egress_multi_alloc(
    int unit,
    bcm_l3_egress_multi_info_t egress_multi_info,
    bcm_if_t * base_egress_id)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_l3_egress_multi_free(
    int unit,
    bcm_if_t base_egress_id)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_l3_egress_multipath_add(
    int unit,
    bcm_if_t mpintf,
    bcm_if_t intf)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API unavailable, instead use APIs with the prefix bcm_l3_egress_ecmp");

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_l3_egress_multipath_create(
    int unit,
    uint32 flags,
    int intf_count,
    bcm_if_t * intf_array,
    bcm_if_t * mpintf)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API unavailable, instead use APIs with the prefix bcm_l3_egress_ecmp");

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_l3_egress_multipath_delete(
    int unit,
    bcm_if_t mpintf,
    bcm_if_t intf)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API unavailable, instead use APIs with the prefix bcm_l3_egress_ecmp");

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_l3_egress_multipath_destroy(
    int unit,
    bcm_if_t mpintf)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API unavailable, instead use APIs with the prefix bcm_l3_egress_ecmp");

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_l3_egress_multipath_find(
    int unit,
    int intf_count,
    bcm_if_t * intf_array,
    bcm_if_t * mpintf)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API unavailable, instead use APIs with the prefix bcm_l3_egress_ecmp");

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_l3_egress_multipath_get(
    int unit,
    bcm_if_t mpintf,
    int intf_size,
    bcm_if_t * intf_array,
    int *intf_count)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API unavailable, instead use APIs with the prefix bcm_l3_egress_ecmp");

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_l3_egress_multipath_max_create(
    int unit,
    uint32 flags,
    int max_paths,
    int intf_count,
    bcm_if_t * intf_array,
    bcm_if_t * mpintf)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API unavailable, instead use APIs with the prefix bcm_l3_egress_ecmp");

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_l3_egress_multipath_traverse(
    int unit,
    bcm_l3_egress_multipath_traverse_cb trav_fn,
    void *user_data)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API unavailable, instead use APIs with the prefix bcm_l3_egress_ecmp");

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_l3_enable_set(
    int unit,
    int enable)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_l3_host_age(
    int unit,
    uint32 flags,
    bcm_l3_host_traverse_cb age_cb,
    void *user_data)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_l3_host_conflict_get(
    int unit,
    bcm_l3_key_t * ipkey,
    bcm_l3_key_t * cf_array,
    int cf_max,
    int *cf_count)
{
    return BCM_E_UNAVAIL;
}

#if defined(INCLUDE_L3)
int
bcm_dnx_mim_detach(
    int unit)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_L3)
int
bcm_dnx_mim_init(
    int unit)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_L3)
int
bcm_dnx_mim_port_add(
    int unit,
    bcm_mim_vpn_t vpn,
    bcm_mim_port_t * mim_port)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_L3)
int
bcm_dnx_mim_port_delete(
    int unit,
    bcm_mim_vpn_t vpn,
    bcm_gport_t mim_port_id)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_L3)
int
bcm_dnx_mim_port_get(
    int unit,
    bcm_mim_vpn_t vpn,
    bcm_mim_port_t * mim_port)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_L3)
int
bcm_dnx_mim_vpn_create(
    int unit,
    bcm_mim_vpn_config_t * info)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_L3)
int
bcm_dnx_mim_vpn_destroy(
    int unit,
    bcm_mim_vpn_t vpn)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_L3)
int
bcm_dnx_mim_vpn_get(
    int unit,
    bcm_mim_vpn_t vpn,
    bcm_mim_vpn_config_t * info)
{
    return BCM_E_UNAVAIL;
}
#endif

int
bcm_dnx_pstats_data_sync(
    int unit)
{
    return BCM_E_UNAVAIL;
}

#if defined(INCLUDE_PSTATS)
int
bcm_dnx_pstats_session_create(
    int unit,
    int options,
    int array_count,
    bcm_pstats_session_element_t * element_array,
    bcm_pstats_session_id_t * session_id)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PSTATS)
int
bcm_dnx_pstats_session_data_clear(
    int unit,
    bcm_pstats_session_id_t session_id)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PSTATS)
int
bcm_dnx_pstats_session_data_get(
    int unit,
    bcm_pstats_session_id_t session_id,
    bcm_pstats_timestamp_t * timestamp,
    int array_max,
    bcm_pstats_data_t * data_array,
    int *array_count)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PSTATS)
int
bcm_dnx_pstats_session_data_sync_get(
    int unit,
    bcm_pstats_session_id_t session_id,
    bcm_pstats_timestamp_t * timestamp,
    int array_max,
    bcm_pstats_data_t * data_array,
    int *array_count)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PSTATS)
int
bcm_dnx_pstats_session_destroy(
    int unit,
    bcm_pstats_session_id_t session_id)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PSTATS)
int
bcm_dnx_pstats_session_get(
    int unit,
    bcm_pstats_session_id_t session_id,
    int array_max,
    bcm_pstats_session_element_t * element_array,
    int *array_count)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PSTATS)
int
bcm_dnx_pstats_session_traverse(
    int unit,
    bcm_pstats_session_traverse_cb cb,
    void *user_data)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_REGEX)
int
bcm_dnx_regex_config_get(
    int unit,
    bcm_regex_config_t * config)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_REGEX)
int
bcm_dnx_regex_config_set(
    int unit,
    bcm_regex_config_t * config)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_REGEX)
int
bcm_dnx_regex_engine_create(
    int unit,
    bcm_regex_engine_config_t * config,
    bcm_regex_engine_t * engid)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_REGEX)
int
bcm_dnx_regex_engine_destroy(
    int unit,
    bcm_regex_engine_t engid)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_REGEX)
int
bcm_dnx_regex_engine_get(
    int unit,
    bcm_regex_engine_t engid,
    bcm_regex_engine_config_t * config)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_REGEX)
int
bcm_dnx_regex_engine_info_get(
    int unit,
    int engine_id,
    bcm_regex_engine_info_t * regex_engine_info)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_REGEX)
int
bcm_dnx_regex_engine_traverse(
    int unit,
    bcm_regex_engine_traverse_cb cb,
    void *user_data)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_REGEX)
int
bcm_dnx_regex_exclude_add(
    int unit,
    uint8 protocol,
    uint16 l4_start,
    uint16 l4_end)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_REGEX)
int
bcm_dnx_regex_exclude_delete(
    int unit,
    uint8 protocol,
    uint16 l4_start,
    uint16 l4_end)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_REGEX)
int
bcm_dnx_regex_exclude_delete_all(
    int unit)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_REGEX)
int
bcm_dnx_regex_exclude_get(
    int unit,
    int array_size,
    uint8 *protocol,
    uint16 *l4low,
    uint16 *l4high,
    int *array_count)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_REGEX)
int
bcm_dnx_regex_get_match_id(
    int unit,
    int signature_id,
    int *match_id)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_REGEX)
int
bcm_dnx_regex_get_sig_id(
    int unit,
    int match_id,
    int *signature_id)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_REGEX)
int
bcm_dnx_regex_info_get(
    int unit,
    bcm_regex_info_t * regex_info)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_REGEX)
int
bcm_dnx_regex_init(
    int unit)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_REGEX)
int
bcm_dnx_regex_match_check(
    int unit,
    bcm_regex_match_t * matches,
    int count,
    int *metric)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_REGEX)
int
bcm_dnx_regex_match_set(
    int unit,
    bcm_regex_engine_t engid,
    bcm_regex_match_t * matches,
    int count)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_REGEX)
int
bcm_dnx_regex_policy_action_add(
    int unit,
    bcm_regex_policy_t policy,
    bcm_field_action_t action,
    uint32 param0,
    uint32 param1)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_REGEX)
int
bcm_dnx_regex_policy_action_delete(
    int unit,
    bcm_regex_policy_t policy,
    bcm_field_action_t action,
    uint32 param0,
    uint32 param1)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_REGEX)
int
bcm_dnx_regex_policy_action_get(
    int unit,
    bcm_regex_policy_t policy,
    bcm_field_action_t action,
    uint32 *param0,
    uint32 *param1)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_REGEX)
int
bcm_dnx_regex_policy_action_mac_add(
    int unit,
    bcm_regex_policy_t policy,
    bcm_field_action_t action,
    bcm_mac_t mac)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_REGEX)
int
bcm_dnx_regex_policy_action_mac_get(
    int unit,
    bcm_regex_policy_t policy,
    bcm_field_action_t action,
    bcm_mac_t * mac)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_REGEX)
int
bcm_dnx_regex_policy_action_ports_add(
    int unit,
    bcm_regex_policy_t policy,
    bcm_field_action_t action,
    bcm_pbmp_t pbmp)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_REGEX)
int
bcm_dnx_regex_policy_action_ports_get(
    int unit,
    bcm_regex_policy_t policy,
    bcm_field_action_t action,
    bcm_pbmp_t * pbmp)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_REGEX)
int
bcm_dnx_regex_policy_action_remove(
    int unit,
    bcm_regex_policy_t policy,
    bcm_field_action_t action)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_REGEX)
int
bcm_dnx_regex_policy_action_remove_all(
    int unit,
    bcm_regex_policy_t policy)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_REGEX)
int
bcm_dnx_regex_policy_create(
    int unit,
    int flags,
    bcm_regex_policy_t * policy)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_REGEX)
int
bcm_dnx_regex_policy_destroy(
    int unit,
    bcm_regex_policy_t policy)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_L3)
int
bcm_dnx_l3_host_delete_by_network(
    int unit,
    bcm_l3_route_t * ip_addr)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_L3)
int
bcm_dnx_l3_host_invalidate_entry(
    int unit,
    bcm_ip_t info)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_L3)
int
bcm_dnx_l3_host_validate_entry(
    int unit,
    bcm_ip_t info)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_L3)
int
bcm_dnx_l3_ingress_destroy(
    int unit,
    bcm_if_t intf_id)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_L3)
int
bcm_dnx_l3_ingress_find(
    int unit,
    bcm_l3_ingress_t * ing_intf,
    bcm_if_t * intf_id)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_L3)
int
bcm_dnx_l3_ingress_traverse(
    int unit,
    bcm_l3_ingress_traverse_cb trav_fn,
    void *user_data)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_L3)
int
bcm_dnx_l3_route_age(
    int unit,
    uint32 flags,
    bcm_l3_route_traverse_cb age_out,
    void *user_data)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_L3)
int
bcm_dnx_l3_route_max_ecmp_get(
    int unit,
    int *max)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_L3)
int
bcm_dnx_l3_route_max_ecmp_set(
    int unit,
    int max)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_L3)
int
bcm_dnx_l3_route_multipath_get(
    int unit,
    bcm_l3_route_t * the_route,
    bcm_l3_route_t * path_array,
    int max_path,
    int *path_count)
{
    return BCM_E_UNAVAIL;
}
#endif

int
bcm_dnx_l2_egress_find(
    int unit,
    bcm_l2_egress_t * egr,
    bcm_if_t * encap_id)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_l2_egress_traverse(
    int unit,
    bcm_l2_egress_traverse_cb trav_fn,
    void *user_data)
{
    return BCM_E_UNAVAIL;
}

#if defined(INCLUDE_L3)
int
bcm_dnx_l3_vrrp_add(
    int unit,
    bcm_vlan_t vlan,
    uint32 vrid)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_L3)
int
bcm_dnx_l3_vrrp_delete(
    int unit,
    bcm_vlan_t vlan,
    uint32 vrid)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_L3)
int
bcm_dnx_l3_vrrp_delete_all(
    int unit,
    bcm_vlan_t vlan)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_L3)
int
bcm_dnx_l3_vrrp_get(
    int unit,
    bcm_vlan_t vlan,
    int alloc_size,
    int *vrid_array,
    int *count)
{
    return BCM_E_UNAVAIL;
}
#endif

int
bcm_dnx_txbeacon_destroy(
    int unit,
    bcm_txbeacon_pkt_id_t pkt_id)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_txbeacon_init(
    int unit,
    int uC)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_txbeacon_pkt_get(
    int unit,
    bcm_txbeacon_t * txbeacon)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_txbeacon_pkt_setup(
    int unit,
    bcm_txbeacon_t * txbeacon)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_txbeacon_start(
    int unit,
    bcm_txbeacon_pkt_id_t pkt_id)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_txbeacon_stop(
    int unit,
    bcm_txbeacon_pkt_id_t pkt_id)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_txbeacon_traverse(
    int unit,
    bcm_txbeacon_traverse_cb_t trav_fn,
    void *user_data)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_auth_mode_get(
    int unit,
    int port,
    uint32 *modep)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_auth_mode_set(
    int unit,
    int port,
    uint32 mode)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_fcoe_cleanup(
    int unit)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_fcoe_control_vsan_get(
    int unit,
    bcm_fcoe_vsan_id_t vsan_id,
    bcm_fcoe_vsan_control_t type,
    int *arg)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_fcoe_control_vsan_set(
    int unit,
    bcm_fcoe_vsan_id_t vsan_id,
    bcm_fcoe_vsan_control_t type,
    int arg)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_fcoe_enable_set(
    int unit)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_fcoe_init(
    int unit)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_fcoe_route_add(
    int unit,
    bcm_fcoe_route_t * route)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_fcoe_route_delete(
    int unit,
    bcm_fcoe_route_t * route)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_fcoe_route_delete_all(
    int unit)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_fcoe_route_delete_by_interface(
    int unit,
    bcm_fcoe_route_t * route)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_fcoe_route_delete_by_prefix(
    int unit,
    bcm_fcoe_route_t * route)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_fcoe_route_find(
    int unit,
    bcm_fcoe_route_t * route)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_fcoe_vsan_create(
    int unit,
    uint32 options,
    bcm_fcoe_vsan_t * vsan,
    bcm_fcoe_vsan_id_t * vsan_id)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_fcoe_vsan_destroy(
    int unit,
    bcm_fcoe_vsan_id_t vsan_id)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_fcoe_vsan_destroy_all(
    int unit)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_fcoe_vsan_get(
    int unit,
    bcm_fcoe_vsan_id_t vsan_id,
    bcm_fcoe_vsan_t * vsan)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_fcoe_vsan_set(
    int unit,
    bcm_fcoe_vsan_id_t vsan_id,
    bcm_fcoe_vsan_t * vsan)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_fcoe_zone_add(
    int unit,
    bcm_fcoe_zone_entry_t * zone)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_fcoe_zone_delete(
    int unit,
    bcm_fcoe_zone_entry_t * zone)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_fcoe_zone_delete_all(
    int unit)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_fcoe_zone_delete_by_did(
    int unit,
    bcm_fcoe_zone_entry_t * zone)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_fcoe_zone_delete_by_sid(
    int unit,
    bcm_fcoe_zone_entry_t * zone)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_fcoe_zone_delete_by_vsan(
    int unit,
    bcm_fcoe_zone_entry_t * zone)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_fcoe_zone_get(
    int unit,
    bcm_fcoe_zone_entry_t * zone)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_rx_control_get(
    int unit,
    bcm_rx_control_t type,
    int *arg)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_rx_control_set(
    int unit,
    bcm_rx_control_t type,
    int arg)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_rx_snoop_create(
    int unit,
    int flags,
    int *snoop_cmnd)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API unavailable, instead use bcm_mirror_destination_create()");

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_rx_snoop_destroy(
    int unit,
    int flags,
    int snoop_cmnd)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API unavailable, instead use bcm_mirror_destination_destroy()");

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_rx_snoop_get(
    int unit,
    int snoop_cmnd,
    bcm_rx_snoop_config_t * config)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API unavailable, instead use bcm_mirror_destination_get()");

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_rx_snoop_set(
    int unit,
    int snoop_cmnd,
    bcm_rx_snoop_config_t * config)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API unavailable, instead use bcm_mirror_destination_set()");

exit:
    SHR_FUNC_EXIT;
}

#if defined(INCLUDE_L3)
int
bcm_dnx_tunnel_config_get(
    int unit,
    bcm_tunnel_config_t * tconfig)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_L3)
int
bcm_dnx_tunnel_config_set(
    int unit,
    bcm_tunnel_config_t * tconfig)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_L3)
int
bcm_dnx_tunnel_dscp_map_create(
    int unit,
    uint32 flags,
    int *dscp_map_id)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_L3)
int
bcm_dnx_tunnel_dscp_map_destroy(
    int unit,
    int dscp_map_id)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_L3)
int
bcm_dnx_tunnel_dscp_map_get(
    int unit,
    int dscp_map_id,
    bcm_tunnel_dscp_map_t * dscp_map)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_L3)
int
bcm_dnx_tunnel_dscp_map_port_get(
    int unit,
    bcm_port_t port,
    bcm_tunnel_dscp_map_t * dscp_map)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_L3)
int
bcm_dnx_tunnel_dscp_map_port_set(
    int unit,
    bcm_port_t port,
    bcm_tunnel_dscp_map_t * dscp_map)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_L3)
int
bcm_dnx_tunnel_dscp_map_set(
    int unit,
    int dscp_map_id,
    bcm_tunnel_dscp_map_t * dscp_map)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_L3)
int
bcm_dnx_tunnel_initiator_set(
    int unit,
    bcm_l3_intf_t * intf,
    bcm_tunnel_initiator_t * tunnel)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_L3)
int
bcm_dnx_tunnel_terminator_add(
    int unit,
    bcm_tunnel_terminator_t * info)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_L3)
int
bcm_dnx_tunnel_terminator_update(
    int unit,
    bcm_tunnel_terminator_t * info)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_L3)
int
bcm_dnx_tunnel_terminator_vlan_get(
    int unit,
    bcm_gport_t tunnel,
    bcm_vlan_vector_t * vlan_vec)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_L3)
int
bcm_dnx_tunnel_terminator_vlan_set(
    int unit,
    bcm_gport_t tunnel,
    bcm_vlan_vector_t vlan_vec)
{
    return BCM_E_UNAVAIL;
}
#endif

int
bcm_dnx_vswitch_init(
    int unit)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_field_data_qualifier_get(
    int unit,
    int qual_id,
    bcm_field_data_qualifier_t * qual_p)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API unavailable, instead use bcm_field_qualifier_info_get()");

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_field_data_qualifier_destroy(
    int unit,
    int qual_id)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API unavailable, instead use bcm_field_qualifier_destroy()");

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_field_data_qualifier_destroy_all(
    int unit)
{

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API unavailable, instead use bcm_field_qualifier_destroy() for each entry");

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_field_data_qualifier_create(
    int unit,
    bcm_field_data_qualifier_t * qual_p)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API unavailable, instead use bcm_field_qualifier_create()");

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_bfd_discard_stat_get(
    int unit,
    bcm_bfd_discard_stat_t * discarded_info)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API unavailable, statistics can be accessed per endpoint");

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_bfd_discard_stat_set(
    int unit,
    bcm_bfd_discard_stat_t * discarded_info)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API unavailable, statistics can be accessed per endpoint");

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_bfd_status_multi_get(
    int unit,
    int max_endpoints,
    bcm_bfd_status_t * status_arr,
    int *count)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_l3_intf_vpn_set(
    int unit,
    bcm_if_t l3_intf_id,
    uint32 flags,
    bcm_vpn_t vpn)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_l3_intf_vpn_get(
    int unit,
    bcm_if_t l3_intf_id,
    uint32 *flags,
    bcm_vpn_t * vpn)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_switch_obm_classifier_mapping_get(
    int unit,
    bcm_gport_t gport,
    bcm_switch_obm_classifier_type_t switch_obm_classifier_type,
    bcm_switch_obm_classifier_t * switch_obm_classifier)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_tx_pkt_l2_map(
    int unit,
    bcm_pkt_t * pkt,
    bcm_mac_t dest_mac,
    int vid)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_switch_obm_classifier_mapping_set(
    int unit,
    bcm_gport_t gport,
    bcm_switch_obm_classifier_type_t switch_obm_classifier_type,
    bcm_switch_obm_classifier_t * switch_obm_classifier)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_switch_obm_classifier_mapping_multi_get(
    int unit,
    bcm_gport_t port,
    bcm_switch_obm_classifier_type_t switch_obm_classifier_type,
    int array_max,
    bcm_switch_obm_classifier_t * switch_obm_classifier,
    int *array_count)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_switch_obm_classifier_mapping_multi_set(
    int unit,
    bcm_gport_t gport,
    bcm_switch_obm_classifier_type_t switch_obm_classifier_type,
    int array_count,
    bcm_switch_obm_classifier_t * switch_obm_classifier)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_l2_addr_delete_by_vlan_gport_multi(
    int unit,
    uint32 flags,
    int num_pairs,
    bcm_vlan_t * vlan,
    bcm_gport_t * gport)
{
    return BCM_E_UNAVAIL;
}

shr_error_e
bcm_dnx_bfd_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "bcm_dnx_bfd_init not supported ");

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_cosq_pfc_deadlock_config_get(
    int unit,
    int priority,
    bcm_cosq_pfc_deadlock_config_t * config)
{

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API not supported");

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_cosq_pfc_deadlock_config_set(
    int unit,
    int priority,
    bcm_cosq_pfc_deadlock_config_t * config)
{

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API not supported");

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_cosq_pfc_deadlock_info_get(
    int unit,
    int priority,
    bcm_cosq_pfc_deadlock_info_t * pfc_deadlock_info)
{

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API not supported");

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_cosq_pfc_deadlock_queue_config_get(
    int unit,
    bcm_gport_t gport,
    bcm_cosq_pfc_deadlock_queue_config_t * q_config)
{

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API not supported");

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_cosq_pfc_deadlock_queue_config_set(
    int unit,
    bcm_gport_t gport,
    bcm_cosq_pfc_deadlock_queue_config_t * q_config)
{

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API not supported");

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_cosq_pfc_deadlock_queue_status_get(
    int unit,
    bcm_gport_t gport,
    uint8 *deadlock_status)
{

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API not supported");

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_cosq_pkt_size_adjust_delta_map_get(
    int unit,
    int delta,
    int *final_delta)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API bcm_cosq_pkt_size_adjust_delta_map_get() is not supported for this device type.");

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_cosq_pkt_size_adjust_delta_map_set(
    int unit,
    int delta,
    int final_delta)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API bcm_cosq_pkt_size_adjust_delta_map_set() is not supported for this device type.");

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_fabric_bandwidth_core_profile_get(
    int unit,
    int core,
    uint32 flags,
    int profile_count,
    bcm_fabric_bandwidth_profile_t * profile_array)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "This API is deprecatated. Use bcm_cosq_bandwidth_fabric_adjust_get instead\n");

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_fabric_bandwidth_core_profile_set(
    int unit,
    int core,
    uint32 flags,
    int profile_count,
    bcm_fabric_bandwidth_profile_t * profile_array)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "This API is deprecatated. Use bcm_cosq_bandwidth_fabric_adjust_set instead\n");

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_fabric_bandwidth_profile_set(
    int unit,
    int profile_count,
    bcm_fabric_bandwidth_profile_t * profile_array)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "This API is deprecatated. Use bcm_cosq_bandwidth_fabric_adjust_set instead\n");

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_fabric_bandwidth_profile_get(
    int unit,
    int profile_count,
    bcm_fabric_bandwidth_profile_t * profile_array)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "This API is deprecatated. Use bcm_cosq_bandwidth_fabric_adjust_get instead\n");

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_fabric_init(
    int unit)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_fabric_link_thresholds_pipe_get(
    int unit,
    int fifo_type,
    bcm_fabric_pipe_t pipe,
    uint32 flags,
    uint32 count,
    bcm_fabric_link_threshold_type_t * type,
    int *value)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL,
                 "Function is not supported. Please read User Manual and use bcm_dnx_fabric_cgm_control_set() instead.");

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_fabric_link_thresholds_pipe_set(
    int unit,
    int fifo_type,
    bcm_fabric_pipe_t pipe,
    uint32 flags,
    uint32 count,
    bcm_fabric_link_threshold_type_t * type,
    int *value)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL,
                 "Function is not supported. Please read User Manual and use bcm_dnx_fabric_cgm_control_set() instead.");

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_fabric_rci_config_get(
    int unit,
    bcm_fabric_rci_config_t * rci_config)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL,
                 "Function is not supported. Please read User Manual and use bcm_dnx_fabric_cgm_control_set() instead.");

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_fabric_rci_config_set(
    int unit,
    bcm_fabric_rci_config_t rci_config)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL,
                 "Function is not supported. Please read User Manual and use bcm_dnx_fabric_cgm_control_set() instead.");

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_fabric_static_replication_set(
    int unit,
    bcm_port_t port,
    uint32 flags,
    uint32 destid_count,
    bcm_module_t * destid_array)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL,
                 "Function is not supported. Please read User Manual and use bcm_fabric_multicast_set() instead.");

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_fabric_tdm_editing_get(
    int unit,
    bcm_gport_t gport,
    bcm_fabric_tdm_editing_t * editing)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    LOG_CLI_EX("\r\n"
               "This BCM API is not available on this device.\r\n"
               "==> It hase been replaced by bcm_dnx_tdm_egress_editing_get() %s%s%s%s\r\n\n",
               EMPTY, EMPTY, EMPTY, EMPTY);
    SHR_SET_CURRENT_ERR(_SHR_E_UNAVAIL);
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_fabric_tdm_editing_set(
    int unit,
    bcm_gport_t gport,
    bcm_fabric_tdm_editing_t * editing)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    LOG_CLI_EX("\r\n"
               "This BCM API is not available on this device.\r\n"
               "==> It hase been replaced by bcm_dnx_tdm_egress_editing_set() %s%s%s%s\r\n\n",
               EMPTY, EMPTY, EMPTY, EMPTY);
    SHR_SET_CURRENT_ERR(_SHR_E_UNAVAIL);
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

#ifdef INCLUDE_I2C
#define I2C_UNAVAILABLE \
    LOG_ERROR(BSL_L_BCMDNX | BSL_S_I2C, ("%s unit %d: i2c APIs are not supported by the device\n", BSL_FUNC, unit)); \
    return BCM_E_UNAVAIL;

int
bcm_dnx_i2c_ioctl(
    int unit,
    int fd,
    int opcode,
    void *data,
    int len)
{
I2C_UNAVAILABLE}

int
bcm_dnx_i2c_open(
    int unit,
    char *devname,
    uint32 flags,
    int speed)
{
I2C_UNAVAILABLE}

int
bcm_dnx_i2c_read(
    int unit,
    int fd,
    uint32 addr,
    uint8 *data,
    uint32 *nbytes)
{
I2C_UNAVAILABLE}

int
bcm_dnx_i2c_write(
    int unit,
    int fd,
    uint32 addr,
    uint8 *data,
    uint32 nbytes)
{
I2C_UNAVAILABLE}
#endif /* INCLUDE_I2C */

int
bcm_dnx_init_selective(
    int unit,
    uint32 flags)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API not supported!");

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_instru_sflow_encap_extended_src_traverse(
    int unit,
    bcm_instru_sflow_encap_extended_src_cb cb,
    void *user_data)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_instru_sflow_encap_extended_dst_traverse(
    int unit,
    bcm_instru_sflow_encap_extended_dst_cb cb,
    void *user_data)
{
    return BCM_E_UNAVAIL;
}

/*
 * bcm_l3_egress_ecmp_find API is not supported
 */
int
bcm_dnx_l3_egress_ecmp_find(
    int unit,
    int intf_count,
    bcm_if_t * intf_array,
    bcm_l3_egress_ecmp_t * ecmp)
{
    return _SHR_E_UNAVAIL;
}

int
bcm_dnx_linkscan_detach(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(BCM_E_UNAVAIL, "No need to call bcm_linkscan_detach as linkscan de-initialization is handled by SDK.");

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_linkscan_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(BCM_E_UNAVAIL, "No need to call bcm_linkscan_init as linkscan is already initialized by the SDK.");

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_mirror_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "This function is not supported for this device\n");

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_mirror_mode_get(
    int unit,
    int *mode)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "This function is not supported for this device\n");

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_mirror_mode_set(
    int unit,
    int mode)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "This function is not supported for this device\n");

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_mirror_port_info_get(
    int unit,
    bcm_port_t port,
    uint32 flags,
    bcm_mirror_port_info_t * info)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "This function is not supported for this device\n");

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_mirror_port_info_set(
    int unit,
    bcm_port_t port,
    uint32 flags,
    bcm_mirror_port_info_t * info)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "This function is not supported for this device\n");

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Add an OAM Alarm Indication Signal entry. This function is
*  not supported.
* \par DIRECT INPUT:
*   \param [in] unit  -
*   Relevant unit.
*   \param [in] ais_ptr  -
*   Pointer to the structure that holds information for the
*   Alarm Indication Signal etry.
* \par INDIRECT INPUT:
*   * none
* \par DIRECT OUTPUT:
*   shr_error_unavail - Return unavailable.
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * None
* \see
*   * \ref bcm_dnx_oam_ais_delete
*   * \ref bcm_dnx_oam_ais_get
*/
shr_error_e
bcm_dnx_oam_ais_add(
    int unit,
    bcm_oam_ais_t * ais_ptr)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "bcm_dnx_oam_ais_add not supported ");

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Delete an OAM Alarm Indication Signal entry. This function is
*  not supported.
* \par DIRECT INPUT:
*   \param [in] unit  -
*   Relevant unit.
*   \param [in] ais_ptr  -
*   Pointer to the structure that holds information for the
*   Alarm Indication Signal etry.
* \par INDIRECT INPUT:
*   * none
* \par DIRECT OUTPUT:
*   shr_error_unavail - Return unavailable.
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * None
* \see
*   * \ref bcm_dnx_oam_ais_add
*   * \ref bcm_dnx_oam_ais_get
*/
shr_error_e
bcm_dnx_oam_ais_delete(
    int unit,
    bcm_oam_ais_t * ais_ptr)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "bcm_dnx_oam_ais_delete not supported ");

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Get an OAM Alarm Indication Signal entry. This function is
*  not supported.
* \par DIRECT INPUT:
*   \param [in] unit  -
*   Relevant unit.
*   \param [out] ais_ptr  -
*   Pointer to the structure that holds information for the
*   Alarm Indication Signal etry. The information from the Get
*   procedure will be written in it.
* \par INDIRECT INPUT:
*   * none
* \par DIRECT OUTPUT:
*   shr_error_unavail - Return unavailable.
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * None
* \see
*   * \ref bcm_dnx_oam_ais_delete
*   * \ref bcm_dnx_oam_ais_get
*/
shr_error_e
bcm_dnx_oam_ais_get(
    int unit,
    bcm_oam_ais_t * ais_ptr)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "bcm_dnx_oam_ais_get not supported ");

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Set an OAM Action entry. This function is not supported.
* \par DIRECT INPUT:
*   \param [in] unit  -
*   Relevant unit.
*   \param [in] id -
*   Endpoint id, in which to set the action
*   \param [in] action -
*   Pointer to the structure, containing information about the
*   Action entry.
* \par INDIRECT INPUT:
*   * none
* \par DIRECT OUTPUT:
*   shr_error_unavail - Return unavailable.
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * None
* \see
*   * None
*/
shr_error_e
bcm_dnx_oam_endpoint_action_set(
    int unit,
    bcm_oam_endpoint_t id,
    bcm_oam_endpoint_action_t * action)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "bcm_dnx_oam_endpoint_action_set not supported ");

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Initialize OAM. This function is not supported.
* \par DIRECT INPUT:
*   \param [in] unit  -
*   Relevant unit.
* \par INDIRECT INPUT:
*   * none
* \par DIRECT OUTPUT:
*   shr_error_unavail - Return unavailable.
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * None
* \see
*   * None
*/
shr_error_e
bcm_dnx_oam_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "bcm_dnx_oam_init not supported ");

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Add OAM sd sf alarm detection. This function is not
*  supported.
* \par DIRECT INPUT:
*   \param [in] unit  -
*    Relevant unit.
*   \param [in] sd_sf_ptr  -
*    Pointer to the sd_sf structure containing the information
*    to be added.
* \par INDIRECT INPUT:
*   * none
* \par DIRECT OUTPUT:
*   shr_error_unavail - Return unavailable.
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * None
* \see
*   * \ref bcm_dnx_oam_sd_sf_detection_delete
*   * \ref bcm_dnx_oam_sd_sf_detection_get
*/
shr_error_e
bcm_dnx_oam_sd_sf_detection_add(
    int unit,
    bcm_oam_sd_sf_detection_t * sd_sf_ptr)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "bcm_dnx_oam_sd_sf_detection_add not supported ");

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Delete OAM sd sf alarm detection. This function is not
*  supported.
* \par DIRECT INPUT:
*   \param [in] unit  -
*    Relevant unit.
*   \param [in] sd_sf_ptr  -
*    Pointer to the sd_sf structure containing the information
*    to be deleted.
* \par INDIRECT INPUT:
*   * none
* \par DIRECT OUTPUT:
*   shr_error_unavail - Return unavailable.
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * None
* \see
*   * \ref bcm_dnx_oam_sd_sf_detection_add
*   * \ref bcm_dnx_oam_sd_sf_detection_get
*/
shr_error_e
bcm_dnx_oam_sd_sf_detection_delete(
    int unit,
    bcm_oam_sd_sf_detection_t * sd_sf_ptr)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "bcm_dnx_oam_sd_sf_detection_delete not supported ");

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Get OAM sd sf alarm detection. This function is not
*  supported.
* \par DIRECT INPUT:
*   \param [in] unit  -
*    Relevant unit.
*   \param [out] sd_sf_ptr  -
*    Pointer to the sd_sf structure containing the information
*    to be gotten. The information from the Get procedure will
*    be written in this structure
* \par INDIRECT INPUT:
*   * none
* \par DIRECT OUTPUT:
*   shr_error_unavail - Return unavailable.
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * None
* \see
*   * \ref bcm_dnx_oam_sd_sf_detection_add
*   * \ref bcm_dnx_oam_sd_sf_detection_delete
*/
shr_error_e
bcm_dnx_oam_sd_sf_detection_get(
    int unit,
    bcm_oam_sd_sf_detection_t * sd_sf_ptr)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "bcm_dnx_oam_sd_sf_detection_get not supported ");

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Add OAM Y1711 alarm . This function is not supported.
* \par DIRECT INPUT:
*   \param [in] unit  -
*    Relevant unit.
*   \param [in] alarm_ptr  -
*    Pointer to the alarm structure containing the information
* \par INDIRECT INPUT:
*   * none
* \par DIRECT OUTPUT:
*   shr_error_unavail - Return unavailable.
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * None
* \see
*   * \ref bcm_dnx_oam_y_1711_alarm_delete
*   * \ref bcm_dnx_oam_y_1711_alarm_get
*/
shr_error_e
bcm_dnx_oam_y_1711_alarm_add(
    int unit,
    bcm_oam_y_1711_alarm_t * alarm_ptr)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "bcm_dnx_oam_y_1711_alarm_add not supported ");

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Delete OAM Y1711 alarm . This function is not supported.
* \par DIRECT INPUT:
*   \param [in] unit  -
*    Relevant unit.
*   \param [in] alarm_ptr  -
*    Pointer to the alarm structure containing the information
* \par INDIRECT INPUT:
*   * none
* \par DIRECT OUTPUT:
*   shr_error_unavail - Return unavailable.
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * None
* \see
*   * \ref bcm_dnx_oam_y_1711_alarm_add
*   * \ref bcm_dnx_oam_y_1711_alarm_get
*/
shr_error_e
bcm_dnx_oam_y_1711_alarm_delete(
    int unit,
    bcm_oam_y_1711_alarm_t * alarm_ptr)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "bcm_dnx_oam_y_1711_alarm_delete not supported ");

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Get OAM Y1711 alarm . This function is not supported.
* \par DIRECT INPUT:
*   \param [in] unit  -
*    Relevant unit.
*   \param [out] alarm_ptr  -
*    Pointer to the alarm structure containing the information
*    The information from the Get procedure will be written
*    here.
* \par INDIRECT INPUT:
*   * none
* \par DIRECT OUTPUT:
*   shr_error_unavail - Return unavailable.
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * None
* \see
*   * \ref bcm_dnx_oam_y_1711_alarm_add
*   * \ref bcm_dnx_oam_y_1711_alarm_delete
*/
shr_error_e
bcm_dnx_oam_y_1711_alarm_get(
    int unit,
    bcm_oam_y_1711_alarm_t * alarm_ptr)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "bcm_dnx_oam_y_1711_alarm_get not supported ");

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_policer_aggregate_group_create(
    int unit,
    bcm_policer_aggregate_group_info_t * info,
    bcm_policer_t * policer_id,
    int *npolicers)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API bcm_policer_aggregate_group_create() is not supported for this device type.\n");
exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_policer_color_decision_get(
    int unit,
    bcm_policer_color_decision_t * policer_color_decision)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API bcm_policer_color_decision_get() is not supported for this device type.\n");
exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_policer_color_decision_set(
    int unit,
    bcm_policer_color_decision_t * policer_color_decision)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API bcm_policer_color_decision_set() is not supported for this device type.\n");
exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_policer_envelop_create(
    int unit,
    uint32 flag,
    bcm_policer_t macro_flow_policer_id,
    bcm_policer_t * policer_id)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API bcm_dnx_policer_envelop_create() is not supported for this device type.\n");
exit:
    SHR_FUNC_EXIT;

}

int
bcm_dnx_policer_group_create(
    int unit,
    bcm_policer_group_mode_t mode,
    bcm_policer_t * policer_id,
    int *npolicers)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API bcm_policer_group_create() is not supported for this device type.\n");
exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_policer_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API bcm_policer_init() is not supported for this device type.\n");
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Retrieve autonegotiation abilities advertised for
 *        the port - not used
 *
 * \param [in] unit - chip unit id.
 * \param [in] port - logical port.
 * \param [out] ability_mask - AN abilities info.
 *
 * \return
 *   shr_error_e - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
shr_error_e
bcm_dnx_port_ability_advert_get(
    int unit,
    bcm_port_t port,
    bcm_port_ability_t * ability_mask)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL,
                 "bcm_port_ability_advert_get API is not supported. Please use bcm_port_autoneg_ability_advert_get instead. \n");

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set autonegotiation abilities adverised for the
 *        port - not used
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] ability_mask - AN abilities info.
 *
 * \return
 *   shr_error_e
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
shr_error_e
bcm_dnx_port_ability_advert_set(
    int unit,
    bcm_port_t port,
    bcm_port_ability_t * ability_mask)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL,
                 "bcm_port_ability_advert_set API is not supported. Please use bcm_port_autoneg_ability_advert_set instead. \n");

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Retrieve the autonegotiation abilities for the local
 *        side of the port - not used
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [out] local_ability_mask - AN abilities info.
 *
 * \return
 *   shr_error_e
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_port_ability_get(
    int unit,
    bcm_port_t port,
    bcm_port_abil_t * local_ability_mask)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL,
                 "bcm_port_ability_get API is not supported. Please use bcm_port_speed_ability_local_get instead. \n");

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Retrieve the autonegotiation abilities for the local
 *        side of the port - not used
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [out] local_ability_mask - AN abilities info.
 *
 * \return
 *   shr_error_e
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
shr_error_e
bcm_dnx_port_ability_local_get(
    int unit,
    bcm_port_t port,
    bcm_port_ability_t * local_ability_mask)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL,
                 "bcm_port_ability_local_get API is not supported. Please use bcm_port_speed_ability_local_get instead. \n");

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Retrieve the autonegotiation abilities for the
 *        remote side of the port - not used
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [out] ability_mask - AN abilities info.
 *
 * \return
 *   shr_error_e
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
shr_error_e
bcm_dnx_port_ability_remote_get(
    int unit,
    bcm_port_t port,
    bcm_port_ability_t * ability_mask)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL,
                 "bcm_port_ability_remote_get API is not supported. Please use bcm_port_autoneg_ability_remote_get instead. \n");

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Initialize the port subsystem without affecting the current state of
 * stack ports.
 *
 * \param [in] unit - chip unit id.
 *
 * \return
 *   shr_error_e
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
shr_error_e
bcm_dnx_port_clear(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "bcm_port_clear API is not supported.\n");
exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_port_congestion_config_get(
    int unit,
    bcm_gport_t port,
    bcm_port_congestion_config_t * config)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL,
                 "QCN is not supported for this device. To get src_mac for a port see bcm_port_pause_addr_get\n");

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_port_congestion_config_set(
    int unit,
    bcm_gport_t port,
    bcm_port_congestion_config_t * config)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL,
                 "QCN is not supported for this device. To set src_mac for a port see bcm_port_pause_addr_set\n");

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_port_info_restore(
    int unit,
    bcm_port_t port,
    bcm_port_info_t * info)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "This function is not supported for this device\n");

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_port_info_save(
    int unit,
    bcm_port_t port,
    bcm_port_info_t * info)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "This function is not supported for this device\n");

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_port_info_set(
    int unit,
    bcm_port_t port,
    bcm_port_info_t * info)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "This function is not supported for this device\n");

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_port_ingress_tdm_failover_get(
    int unit,
    int flag,
    bcm_pbmp_t * tdm_enable_pbmp)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "TDM is not supported by device.\n");

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_port_ingress_tdm_failover_set(
    int unit,
    int flag,
    bcm_pbmp_t tdm_enable_pbmp)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "TDM is not supported by device.\n");

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - initialize the Port module -not used
 *
 * \param [in] unit - chip unit id.
 *
 * \return
 *   shr_error_e
 *
 * \remarks
 *   * This method should be called only during init sequence,
 *     not to be used dynamically by the user.
 * \see
 *   * None
 */
shr_error_e
bcm_dnx_port_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "bcm_port_init API is not supported.\n");
exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_port_interface_config_get(
    int unit,
    bcm_port_t port,
    bcm_port_interface_config_t * config)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "This API isn't supported. Information can be retrieved from bcm_port_get\n");

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_port_interface_get(
    int unit,
    bcm_port_t port,
    bcm_port_if_t * intf)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL,
                 "bcm_port_interface_get is no longer supported, please use bcm_port_resource_get to get interface properties.");

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_port_interface_set(
    int unit,
    bcm_port_t port,
    bcm_port_if_t intf)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL,
                 "bcm_port_interface_set is no longer supported, please use bcm_port_resource_set to align interface properties.");

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get Auto MDIX mode for a port. only relevant for
 *        external phy, hence not supported.
 */
shr_error_e
bcm_dnx_port_mdix_get(
    int unit,
    bcm_port_t port,
    bcm_port_mdix_t * mode)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "bcm_port_mdix_get API is only supported for external Phys.\n");
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set Auto MDIX mode for a port. only relevant for
 *        external phy, hence not supported.
 */
shr_error_e
bcm_dnx_port_mdix_set(
    int unit,
    bcm_port_t port,
    bcm_port_mdix_t mode)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "bcm_port_mdix_set API is only supported for external Phys.\n");
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get Auto MDIX status for a port. only relevant for
 *        external phy, hence not supported.
 */
shr_error_e
bcm_dnx_port_mdix_status_get(
    int unit,
    bcm_port_t port,
    bcm_port_mdix_status_t * status)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "bcm_port_mdix_status_gete API is only supported for external Phys.\n");
exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_port_nif_priority_get(
    int unit,
    bcm_gport_t local_port,
    uint32 flags,
    bcm_port_nif_prio_t * priority,
    bcm_pbmp_t * affected_ports)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL,
                 "bcm_port_nif_priority_get API is no longer supported. Please see bcm_port_priority_config_get instead.\n");
exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_port_nif_priority_set(
    int unit,
    bcm_gport_t local_port,
    uint32 flags,
    bcm_port_nif_prio_t * priority,
    bcm_pbmp_t * affected_ports)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL,
                 "bcm_port_nif_priority_set API is no longer supported. Please see bcm_port_priority_config_set instead.\n");
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -  API to retrieve asymmetric pause setting for a port.
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] port - port for which we want to get pause settings
 *   \param [out] pause - pause settings
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_port_pause_sym_get(
    int unit,
    bcm_port_t port,
    int *pause)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL,
                 "This API is not supported for this device, for getting pause settings for a port please use bcm_port_pause_get.\n");

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -  API to configure asymmetric pause setting for a port.
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] port - port for which we want to set pause settings
 *   \param [in] pause - pause settings, one of BCM_PORT_PAUSE_SYM for
 *    symmetric pause, BCM_PORT_PAUSE_ASYM_RX for asymmetric pause to
 *    honor received MAC control pause frames but not transmit,
 *    BCM_PORT_PAUSE_ASYM_TX to ignore received pause frames but transmit,
 *    and BCM_PORT_PAUSE_NONE to neither honor or transmit pause frames.
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_port_pause_sym_set(
    int unit,
    bcm_port_t port,
    int pause)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL,
                 "This API is not supported for this device, for setting pause settings for a port please use bcm_port_pause_set.\n");

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_port_phy_reset(
    int unit,
    bcm_port_t port)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "This function is not supported for this device\n");

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_port_policer_get(
    int unit,
    bcm_port_t port,
    bcm_policer_t * policer_id)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API bcm_port_policer_get() is not supported for this device type.\n");
exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_port_policer_set(
    int unit,
    bcm_port_t port,
    bcm_policer_t policer_id)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API bcm_port_policer_set() is not supported for this device type.\n");
exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_port_resource_speed_get(
    int unit,
    bcm_gport_t port,
    bcm_port_resource_t * resource)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL,
                 "bcm_port_resource_speed_get API is not supported. Use bcm_port_resource_get API instead.");

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_port_resource_speed_multi_set(
    int unit,
    int nport,
    bcm_port_resource_t * resource)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL,
                 "bcm_port_resource_speed_multi_set API is not supported. Use bcm_port_resource_multi_set API instead.");

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_port_resource_speed_set(
    int unit,
    bcm_gport_t port,
    bcm_port_resource_t * resource)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL,
                 "bcm_port_resource_speed_set API is not supported. Use bcm_port_resource_set API instead.");

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_port_selective_get(
    int unit,
    bcm_port_t port,
    bcm_port_info_t * info)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "This function is not supported for this device\n");

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_port_selective_set(
    int unit,
    bcm_port_t port,
    bcm_port_info_t * info)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "This function is not supported for this device\n");

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_port_speed_get(
    int unit,
    bcm_port_t port,
    int *speed)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL,
                 "bcm_port_speed_get is no longer supported, please use bcm_port_resource_get to get the speed.");

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_port_speed_max(
    int unit,
    bcm_port_t port,
    int *speed)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL,
                 "bcm_port_speed_max is no longer supported, please use bcm_port_resource_get to get the speed.");

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_port_speed_set(
    int unit,
    bcm_port_t port,
    int speed)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL,
                 "bcm_port_speed_set is no longer supported, please use bcm_port_resource_set to control the speed.");

exit:
    SHR_FUNC_EXIT;
}

 /**
 * \brief - unsupported for JR2
 *
 * \return
 *   BCM_E_UNAVAIL
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_port_stat_get32(
    int unit,
    bcm_gport_t port,
    bcm_port_stat_t stat,
    uint32 *val)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(BCM_E_UNAVAIL, "The API is not supported for JR2.\n");

exit:
    SHR_FUNC_EXIT;
}

 /**
 * \brief - unsupported for JR2
 *
 * \return
 *   BCM_E_UNAVAIL
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_port_stat_multi_get32(
    int unit,
    bcm_gport_t port,
    int nstat,
    bcm_port_stat_t * stat_arr,
    uint32 *value_arr)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(BCM_E_UNAVAIL, "The API is not supported for JR2.\n");

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Software initialization for RX.
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - Relevant unit
 *
 * \return
 *   Negative in case of an error. See shr_error_e
 *   Zero in case of NO ERROR
 *
 * \remark
 *   Please see _bcm_common_rx_init
 *
 * \see
 *   None
 */
int
bcm_dnx_rx_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL,
                 "No need to support this API, BCM RX initialization has already be done during init sequence.\n");

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
* \brief
*   get GTF stat .
* \param [in] unit  -
*   Relevant unit.
* \param [in] gtf_id  -
*    GTF ID.
* \param [in] priority  -
*    SAT GTF Priority
* \param [in ] flags  -
*    Relevant flags
* \param [in ] type  -
*    SAT gtf counter types
* \param [in ] value  -
*    stat values
* \retval
*   * Zero if no error was detected
*   * Negative if error was detected. See \ref
*     shr_error_e
* \remark
*   * None
*/
shr_error_e
bcm_dnx_sat_gtf_stat_get(
    int unit,
    bcm_sat_gtf_t gtf_id,
    int priority,
    uint32 flags,
    bcm_sat_gtf_stat_counter_t type,
    uint64 *value)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "please use the multi get API (bcm_sat_gtf_stat_multi_get) ");

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_stat_counter_config_get(
    int unit,
    bcm_stat_counter_engine_t * engine,
    bcm_stat_counter_config_t * config)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API bcm_stat_counter_config_get() is not supported for this device type.\n");
exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_stat_counter_config_set(
    int unit,
    bcm_stat_counter_engine_t * engine,
    bcm_stat_counter_config_t * config)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API bcm_stat_counter_config_set() is not supported for this device type.\n");
exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_stat_counter_filter_get(
    int unit,
    bcm_stat_counter_source_t source,
    int filter_max_count,
    bcm_stat_counter_filter_t * filter_array,
    int *filter_count)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    SHR_ERR_EXIT(_SHR_E_UNAVAIL,
                 "API bcm_stat_counter_filter_get() is not supported for this device type - refer to API bcm_stat_counter_filter_group_get.\n");
exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_stat_counter_filter_is_active_get(
    int unit,
    bcm_stat_counter_source_t source,
    bcm_stat_counter_filter_t filter,
    int *is_active)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    SHR_ERR_EXIT(_SHR_E_UNAVAIL,
                 "API bcm_stat_counter_filter_is_active_get() is not supported for this device type - refer to API bcm_stat_counter_filter_group_get.\n");
exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_stat_counter_filter_set(
    int unit,
    bcm_stat_counter_source_t source,
    bcm_stat_counter_filter_t * filter_array,
    int filter_count,
    int is_active)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    SHR_ERR_EXIT(_SHR_E_UNAVAIL,
                 "API bcm_stat_counter_filter_get() is not supported for this device type - refer to API bcm_stat_counter_filter_group_set.\n");
exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_stat_counter_lif_counting_get(
    int unit,
    uint32 flags,
    bcm_stat_counter_source_t * source,
    bcm_stat_counter_lif_mask_t * counting_mask,
    bcm_stat_counter_lif_stack_id_t * lif_stack_id_to_count)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API bcm_stat_counter_lif_counting_get() is not supported for this device type.\n");
exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_stat_counter_lif_counting_range_get(
    int unit,
    uint32 flags,
    bcm_stat_counter_source_type_t source,
    bcm_stat_counter_lif_range_id_t range_id,
    bcm_stat_counter_lif_counting_range_t * lif_range)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    SHR_ERR_EXIT(_SHR_E_UNAVAIL,
                 "API bcm_stat_counter_lif_counting_range_get() is not supported for this device type.\n");
exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_stat_counter_lif_counting_range_set(
    int unit,
    uint32 flags,
    bcm_stat_counter_source_type_t source,
    bcm_stat_counter_lif_range_id_t range_id,
    bcm_stat_counter_lif_counting_range_t * lif_range)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    SHR_ERR_EXIT(_SHR_E_UNAVAIL,
                 "API bcm_stat_counter_lif_counting_range_set() is not supported for this device type.\n");
exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_stat_counter_lif_counting_set(
    int unit,
    uint32 flags,
    bcm_stat_counter_source_t * source,
    bcm_stat_counter_lif_mask_t * counting_mask,
    bcm_stat_counter_lif_stack_id_t lif_stack_id_to_count)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API bcm_stat_counter_lif_counting_set() is not supported for this device type.\n");
exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_stat_egress_receive_tm_pointer_format_get(
    int unit,
    int flags,
    bcm_stat_egress_receive_tm_pointer_format_t * pointer_format)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    SHR_ERR_EXIT(_SHR_E_UNAVAIL,
                 "API bcm_stat_egress_receive_tm_pointer_format_get() is not supported for this device type.\n");
exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_stat_egress_receive_tm_pointer_format_set(
    int unit,
    int flags,
    bcm_stat_egress_receive_tm_pointer_format_t * pointer_format)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    SHR_ERR_EXIT(_SHR_E_UNAVAIL,
                 "API bcm_stat_egress_receive_tm_pointer_format_set() is not supported for this device type.\n");
exit:
    SHR_FUNC_EXIT;
}

 /**
 * \brief - unsupported for DNX
 *
 * \return
 *   BCM_E_UNAVAIL
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_stat_get32(
    int unit,
    bcm_port_t port,
    bcm_stat_val_t type,
    uint32 *value)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(BCM_E_UNAVAIL, "The API is not supported for DNX.\n");

exit:
    SHR_FUNC_EXIT;
}

 /**
 * \brief - unsupported for DNX
 *
 * \return
 *   BCM_E_UNAVAIL
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_stat_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(BCM_E_UNAVAIL, "The API is not supported for DNX.\n");

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_stat_lif_counting_profile_get(
    int unit,
    uint32 flags,
    int lif_counting_profile,
    bcm_stat_lif_counting_t * lif_counting)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API bcm_stat_lif_counting_profile_get() is not supported for this device type.\n");
exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_stat_lif_counting_profile_set(
    int unit,
    uint32 flags,
    int lif_counting_profile,
    bcm_stat_lif_counting_t * lif_counting)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API bcm_stat_lif_counting_profile_set() is not supported for this device type.\n");
exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_stat_lif_counting_stack_level_priority_get(
    int unit,
    uint32 flags,
    bcm_stat_lif_counting_source_t * source,
    bcm_stat_counter_lif_stack_id_t lif_stack_level,
    int *priority)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    SHR_ERR_EXIT(_SHR_E_UNAVAIL,
                 "API bcm_stat_lif_counting_stack_level_priority_get() is not supported for this device type.\n");
exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_stat_lif_counting_stack_level_priority_set(
    int unit,
    uint32 flags,
    bcm_stat_lif_counting_source_t * source,
    bcm_stat_counter_lif_stack_id_t lif_stack_level,
    int priority)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    SHR_ERR_EXIT(_SHR_E_UNAVAIL,
                 "API bcm_stat_lif_counting_stack_level_priority_set() is not supported for this device type.\n");
exit:
    SHR_FUNC_EXIT;
}

 /**
 * \brief - unsupported for DNX
 *
 * \return
 *   BCM_E_UNAVAIL
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_stat_multi_get32(
    int unit,
    bcm_port_t port,
    int nstat,
    bcm_stat_val_t * stat_arr,
    uint32 *value_arr)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(BCM_E_UNAVAIL, "The API is not supported for DNX.\n");

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Initialize stg model.
 *
 * \param [in] unit - Relevant unit.
 *
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref bcm_error_t
 *
 * \remark
 *   If it was initialized already, nothing would be done.
 *
 * \see
 *   dnx_stg_init
 */
bcm_error_t
bcm_dnx_stg_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    /*
     * For DNX, we don't see the necessary to support the API,
     * since, STG was initialized during init stage already.
     */
    /** SHR_IF_ERR_EXIT(dnx_stg_init(unit));*/

    SHR_IF_ERR_EXIT(BCM_E_UNAVAIL);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief 
 * This function not supported.
 * Module initialzed upon device init (bcm_init())
 */
int
bcm_dnx_stk_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API is not supported. Module initialzed upon device init (bcm_init())\n");

exit:
    SHR_FUNC_EXIT;
}

/*
 * This procedure always responds with 'unavailable' error.
 * It has been taken out since DRAM is internal for DNX.
 */
int
bcm_dnx_switch_dram_vendor_info_get(
    int unit,
    bcm_switch_dram_vendor_info_t * info)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    SHR_ERR_EXIT(BCM_E_UNAVAIL,
                 "bcm_switch_dram_vendor_info_get is no longer supported because DRAM is internal. There is no replacer.");
exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Add one encap id to destination mapping entry.
* \par DIRECT INPUT:
*   \param [in] unit   - unit Id
*   \param [in] encap_info - encapsulation info
* \par INDIRECT INPUT:
*   * None
* \par DIRECT OUTPUT:
*   shr_error_e - Error type
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * None
* \see
*   * None
*/
int
bcm_dnx_switch_encap_dest_map_add(
    int unit,
    bcm_switch_encap_dest_t * encap_info)
{
    
    return BCM_E_UNAVAIL;
}

/**
* \brief
*  Delete one encap id to destination mapping entry.
* \par DIRECT INPUT:
*   \param [in] unit   - unit Id
*   \param [in] encap_info - Encap-Info.
* \par INDIRECT INPUT:
*   * None
* \par DIRECT OUTPUT:
*   shr_error_e - Error type
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * None
* \see
*   * None
*/
int
bcm_dnx_switch_encap_dest_map_delete(
    int unit,
    bcm_switch_encap_dest_t * encap_info)
{
    
    return BCM_E_UNAVAIL;
}

/**
* \brief
*  Get the destination with the given encap_id.
* \par DIRECT INPUT:
*   \param [in] unit   - unit Id
*   \param [in] encap_info - Encap-Info.
* \par INDIRECT INPUT:
*   * None
* \par DIRECT OUTPUT:
*   shr_error_e - Error type
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * None
* \see
*   * None
*/
int
bcm_dnx_switch_encap_dest_map_get(
    int unit,
    bcm_switch_encap_dest_t * encap_info)
{
    
    return BCM_E_UNAVAIL;
}

/**
* \brief
*  Get the destination with the given encap_id.
*  \par DIRECT INPUT:
*  \param [in] unit   - unit Id
*  \param [in] cb_fn   - pointer to callback function
 *   \param [in] user_data - pointer to the user data. NULL if doesn't exist.
* \par INDIRECT INPUT:
*   * None
* \par DIRECT OUTPUT:
*   shr_error_e - Error type
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * None
* \see
*   * None
*/
int
bcm_dnx_switch_encap_dest_map_traverse(
    int unit,
    bcm_switch_encap_dest_map_traverse_cb cb_fn,
    void *user_data)
{
    
    return BCM_E_UNAVAIL;
}

/**
 * NOT USED - not useful utility, can be done in other ways
 */
shr_error_e
bcm_dnx_trunk_bitmap_expand(
    int unit,
    bcm_pbmp_t * pbmp_ptr)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "%s - This API is not available.\n"), FUNCTION_NAME()));

    SHR_ERR_EXIT(_SHR_E_UNAVAIL,
                 "%s not supported. this utility can be achieved by getting the trunk members, comparing them to the bitmap and then adding the missing members\n",
                 FUNCTION_NAME());
exit:
    SHR_FUNC_EXIT;
}

/**
 * NOT USED - Init is done at BCM Init
 */
shr_error_e
bcm_dnx_trunk_chip_info_get(
    int unit,
    bcm_trunk_chip_info_t * ta_info)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "%s - This API is not available.\n"), FUNCTION_NAME()));

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "%s not supported. relevant info is provided with SOC properties"
                 " and is available there\n", FUNCTION_NAME());
exit:
    SHR_FUNC_EXIT;
}

/**
 * NOT USED - Init is done at BCM Init
 */
shr_error_e
bcm_dnx_trunk_detach(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "%s - This API is not available.\n"), FUNCTION_NAME()));

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "%s not supported. this function is redundant with "
                 "current BCM init/deinit sequence\n", FUNCTION_NAME());
exit:
    SHR_FUNC_EXIT;
}

/**
 * NOT USED - Init is done at BCM Init
 */
shr_error_e
bcm_dnx_trunk_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "%s - This API is not available.\n"), FUNCTION_NAME()));

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "%s not supported. this function is redundant with "
                 "current BCM init/deinit sequence\n", FUNCTION_NAME());
exit:
    SHR_FUNC_EXIT;
}

/**
 * NOT USED - getting PSC is done only with bcm_trunk_get API
 */
shr_error_e
bcm_dnx_trunk_psc_get(
    int unit,
    bcm_trunk_t trunk_id,
    int *psc)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "%s - This API is not available.\n"), FUNCTION_NAME()));

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "%s not supported."
                 " correct sequence is to use bcm_trunk_get API\n", FUNCTION_NAME());
exit:
    SHR_FUNC_EXIT;
}

/**
 * NOT USED - changing and setting PSC is done only with
 * bcm_trunk_set API
 */
shr_error_e
bcm_dnx_trunk_psc_set(
    int unit,
    bcm_trunk_t trunk_id,
    int psc)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "%s - This API is not available.\n"), FUNCTION_NAME()));

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "%s not supported."
                 " correct sequence is to use bcm_trunk_set API with wanted PSC \n", FUNCTION_NAME());
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Initialize BCM TX API
 * \par DIRECT INPUT:
 *   \param [in] unit  -  Unit ID
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   shr_error_e - Error Type
 * \par INDIRECT OUTPUT:
 *   * None
 * \remark
 *   Please see bcm_common_tx_init
 * \see
 *   * None
 */
int
bcm_dnx_tx_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL,
                 "No need to support this API, BCM TX initialization has already be done during init sequence.\n");

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

shr_error_e
bcm_dnx_vlan_gport_info_get(
    int unit,
    bcm_vlan_gport_info_t * vlan_gport_info)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    SHR_ERR_EXIT(_SHR_E_UNAVAIL,
                 "This API is not supported for JR2 device, please read the backward compatibility document\n");
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * This API traverse existing ports on vswitch.
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit -
 *      Relevant unit.
 *   \param [in] vsi -
 *      The VSI that the In-LIF is associated with
 *   \param [in] cb -
 *      Vswitch callback function
 *   \param [in] user_data -
 *      Pointer to user_data (input to the call back function)
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   shr_error_e
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   * Not implemented yet.
 * \see
 *   * None
 */
shr_error_e
bcm_dnx_vswitch_port_traverse(
    int unit,
    bcm_vlan_t vsi,
    bcm_vswitch_port_traverse_cb cb,
    void *user_data)
{
    return _SHR_E_UNAVAIL;
}

int
bcm_dnx_clear(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API not supported!");

exit:
    SHR_FUNC_EXIT;
}
