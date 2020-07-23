/* 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        mbcm.c
 */

#include <soc/debug.h>
#include <bcm_int/esw/mbcm.h>
#include <bcm_int/esw/hercules.h>
#include <bcm_int/esw/firebolt.h>
#include <bcm_int/esw/bradley.h>
#include <bcm_int/esw/humv.h>
#include <bcm/error.h>

mbcm_functions_t mbcm_humv_driver = {
    /*  L2 functions */
    bcm_hercules_l2_init,
    bcm_hercules_l2_term,
    bcm_hercules_l2_addr_get,
    bcm_hercules_l2_addr_add,
    bcm_hercules_l2_addr_delete,
    bcm_hercules_l2_conflict_get, 

    /*  Port table related functions */
    bcm_xgs3_port_cfg_init,
    bcm_xgs3_port_cfg_get,
    bcm_xgs3_port_cfg_set,

    /*  VLAN functions */
    bcm_xgs3_vlan_init,
    bcm_xgs3_vlan_reload,
    bcm_xgs3_vlan_create,
    bcm_xgs3_vlan_destroy,
    bcm_xgs3_vlan_port_add,
    bcm_xgs3_vlan_port_remove,
    bcm_xgs3_vlan_port_get,
    bcm_xgs3_vlan_stg_get,
    bcm_xgs3_vlan_stg_set,

    /*  Firebolt trunking functions */
    bcm_xgs3_trunk_modify,
    bcm_xgs3_trunk_get,
    bcm_xgs3_trunk_destroy,
    bcm_xgs3_trunk_mcast_join,

    /*  Spanning Tree Group functions */
    bcm_xgs3_stg_stp_init,
    bcm_xgs3_stg_stp_get,
    bcm_xgs3_stg_stp_set,

    /*  Multicasting functions */
    bcm_hercules_mcast_addr_add,
    bcm_hercules_mcast_addr_remove,
    bcm_hercules_mcast_port_get,
    bcm_humv_mcast_init,
    _bcm_humv_mcast_detach,
    bcm_hercules_mcast_addr_add_w_l2mcindex,
    bcm_hercules_mcast_addr_remove_w_l2mcindex,
    bcm_hercules_mcast_port_add,
    bcm_hercules_mcast_port_remove,

    /*  COSQ functions */
    bcm_bradley_cosq_init,
    bcm_bradley_cosq_detach,
    bcm_bradley_cosq_config_set,
    bcm_bradley_cosq_config_get,
    bcm_bradley_cosq_mapping_set,
    bcm_bradley_cosq_mapping_get,
    bcm_bradley_cosq_port_sched_set,
    bcm_bradley_cosq_port_sched_get,
    bcm_bradley_cosq_sched_weight_max_get,
    bcm_fb_cosq_port_bandwidth_set,
    bcm_fb_cosq_port_bandwidth_get,
    bcm_fb_cosq_discard_set,
    bcm_fb_cosq_discard_get,
    bcm_fb_cosq_discard_port_set,
    bcm_fb_cosq_discard_port_get,
#ifdef BCM_WARM_BOOT_SUPPORT
    bcm_bradley_cosq_sync,
#endif /* BCM_WARM_BOOT_SUPPORT */
#ifndef BCM_SW_STATE_DUMP_DISABLE
    bcm_bradley_cosq_sw_dump,
#endif /* BCM_SW_STATE_DUMP_DISABLE */

    /*  Port meter functions */
    bcm_hercules_port_rate_egress_set,
    bcm_hercules_port_rate_egress_get,

#ifdef INCLUDE_L3
    /* L3 functions */
    bcm_hercules_l3_tables_init,
    bcm_hercules_l3_tables_cleanup,
    bcm_hercules_l3_enable,
    bcm_hercules_l3_intf_get,
    bcm_hercules_l3_intf_get_by_vid,
    bcm_hercules_l3_intf_create,
    bcm_hercules_l3_intf_id_create,
    bcm_hercules_l3_intf_lookup,
    bcm_hercules_l3_intf_del,
    bcm_hercules_l3_intf_del_all,

    bcm_hercules_l3_get,
    bcm_hercules_l3_add,
    bcm_hercules_l3_del,
    bcm_hercules_l3_del_prefix,
    bcm_hercules_l3_del_intf,
    bcm_hercules_l3_del_all,
    bcm_hercules_l3_replace,
    bcm_hercules_l3_age,
    bcm_hercules_l3_traverse,

    bcm_hercules_l3_ip6_get,
    bcm_hercules_l3_ip6_add,
    bcm_hercules_l3_ip6_del,
    bcm_hercules_l3_ip6_del_prefix,
    bcm_hercules_l3_ip6_replace,
    bcm_hercules_l3_ip6_traverse,

    bcm_hercules_defip_cfg_get,
    bcm_hercules_defip_ecmp_get_all,
    bcm_hercules_defip_add,
    bcm_hercules_defip_del,
    bcm_hercules_defip_del_intf,
    bcm_hercules_defip_del_all,
    bcm_hercules_defip_age,
    bcm_hercules_defip_traverse,

    bcm_hercules_ip6_defip_cfg_get,
    bcm_hercules_ip6_defip_ecmp_get_all,
    bcm_hercules_ip6_defip_add,
    bcm_hercules_ip6_defip_del,
    bcm_hercules_ip6_defip_traverse,

    bcm_hercules_l3_conflict_get,
    bcm_hercules_l3_info,

    /* IPMC functions */
    bcm_hercules_ipmc_init,
    bcm_hercules_ipmc_detach,
    bcm_hercules_ipmc_enable,
    bcm_hercules_ipmc_src_port_check,
    bcm_hercules_ipmc_src_ip_search,
    bcm_hercules_ipmc_add,
    bcm_hercules_ipmc_delete,
    bcm_hercules_ipmc_delete_all,
    bcm_hercules_ipmc_lookup,
    bcm_hercules_ipmc_get,
    bcm_hercules_ipmc_put,
    bcm_hercules_ipmc_egress_port_get,
    bcm_hercules_ipmc_egress_port_set,    
    bcm_hercules_ipmc_repl_init,
    bcm_hercules_ipmc_repl_detach,
    bcm_hercules_ipmc_repl_get,
    bcm_hercules_ipmc_repl_add,
    bcm_hercules_ipmc_repl_delete,
    bcm_hercules_ipmc_repl_delete_all,
    bcm_hercules_ipmc_egress_intf_add,
    bcm_hercules_ipmc_egress_intf_delete,
    bcm_hercules_ipmc_age,
    bcm_hercules_ipmc_traverse,
#endif /* INCLUDE_L3 */
};
