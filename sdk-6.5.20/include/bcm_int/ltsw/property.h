/*! \file property.h
 *
 * Configuration property header file.
 * This file contains the stub definitions of configuration property.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMI_LTSW_PROPERTY_H
#define BCMI_LTSW_PROPERTY_H

#include <sal/sal_types.h>

/*!
 * \name Configuration property name
 * \anchor BCMI_CPN_xxx
 */

/*! \{ */

/*!
 * Enable tunnel termination for protocol types, compatible for all devices.
 * Setting the compatible mode ignores protocol match and sets protocol mask to
 * zero.
 * bcm_tunnel_term_compatible_mode = 0: do not ignore protocol match.
 * bcm_tunnel_term_compatible_mode = 1: ignore protocol match.
 */
#define BCMI_CPN_BCM_TUNNEL_TERM_COMPATIBLE_MODE  "bcm_tunnel_term_compatible_mode"

/*!
 * This configuration property is used to turn on/off vlan auto stack.
 * Note:
 * vlan_auto_stack = 1: turn on  vlan auto stack.
 * vlan_auto_stack = 2: turn off vlan auto stack.
 * vlan_auto_stack = 0: reserved.
 */
#define BCMI_CPN_VLAN_AUTO_STACK  "vlan_auto_stack"

/*!
 * Disable and restrict user to create l2 entry as part of l3 interface create.
 * Note:
 * l3_disable_add_to_arl = 0: enable.
 * l3_disable_add_to_arl = 1: disable.
 */
#define BCMI_CPN_L3_DISABLE_ADD_TO_ARL  "l3_disable_add_to_arl"

/*!
 * L3 switching enable. This will also affect all modules that are built
 * on L3 module.
 * Note:
 * l3_enable = 0: disable.
 * l3_enable = 1: enable.
 */
#define BCMI_CPN_L3_ENABLE  "l3_enable"

/*!
 * This configuration property is used to control the Ethernet and CPU ports
 * in default VLAN.
 * Note:
 * vlan_default_port = 0: add both Ethernet and CPU ports to default VLAN.
 * vlan_default_port = 1: only add Ethernet ports to default VLAN.
 * vlan_default_port = 2: only add CPU ports default VLAN.
 * vlan_default_port = 3: exclude both Ethernet and CPU ports from default VLAN.
 * Default value is 0.
 */
#define BCMI_CPN_VLAN_DEFAULT_PORT  "vlan_default_port"

/*!
 * Priority of the S/W based L2 age thread.
 */
#define BCMI_CPN_L2AGE_THREAD_PRI  "l2age_thread_pri"

/*!
 * This configuration property is used to control the handling on the error of
 * user callback.
 * Note:
 * cb_abort_on_err = 0: continue processing on the error of user callback.
 * cb_abort_on_err = 1: abort processing on the error of user callback.
 * Default value is 0.
 */
#define BCMI_CPN_CB_ABORT_ON_ERR  "cb_abort_on_err"

/*!
 * L2 Learning enable.
 * Note:
 * l2_learn_enable = 0: disable.
 * l2_learn_enable = 1: enable.
 */
#define BCMI_CPN_L2_LEARN_ENABLE  "l2_learn_enable"

/*!
 * QoS map table L2 block size.
 * Note:
 * This config property is used to specify the number of a continuous
 * block of entries from ingress QoS mapping table for Qos L2 ingress
 * mapping usage.
 * The size is not allowed to be larger than the whole size of ingress
 * QoS mapping table, and neither are the total sizes of L2, L3 and MPLS blocks.
 */
#define BCMI_CPN_QOS_MAP_L2_SIZE  "qos_l2_size"

/*!
 * QoS map table L3 block size.
 * Note:
 * This config property is used to specify the number of a continuous
 * block of entries from ingress QoS mapping table for Qos L3 ingress
 * mapping usage.
 * The size is not allowed to be larger than the whole size of ingress
 * QoS mapping table, and neither are the total sizes of L2, L3 and MPLS blocks.
 */
#define BCMI_CPN_QOS_MAP_L3_SIZE  "qos_l3_size"

/*!
 * QoS map table MPLS block size.
 * Note:
 * This config property is used to specify the number of a continuous
 * block of entries from ingress QoS mapping table for Qos MPLS ingress
 * mapping usage.
 * The size is not allowed to be larger than the whole size of ingress
 * QoS mapping table, and neither are the total sizes of L2, L3 and MPLS blocks.
 */
#define BCMI_CPN_QOS_MAP_MPLS_SIZE  "qos_mpls_size"

/*!
 * L2 Caching of BPDU MAC addresses will be turned off.
 * Note:
 * skip_l2_user_entry = 0: turn on.
 * skip_l2_user_entry = 1: turn off.
 */
#define BCMI_CPN_SKIP_L2_USER_ENTRY  "skip_l2_user_entry"

/*!
 * Expected time required to drain the egress COS queues. Used to
 * prevent re-ordering when demoting elephant flows to mice
 */
#define BCMI_CPN_FLOWTRACKER_ELEPHANT_EXPECTED_QUEUE_DRAIN_TIME_USECS  "flowtracker_elephant_expected_queue_drain_time_usecs"

/*!
 * Specifies the number of table entries to
 * be retrieved at a time during sram scanning.
 */
#define BCMI_CPN_SRAM_SCAN_CHUNK_SIZE  "sram_scan_chunk_size"

/*!
 * Control to automatically run background sram scan.
 */
#define BCMI_CPN_SRAM_SCAN_ENABLE  "sram_scan_enable"

/*!
 * Specifies the sram scan rate in terms of entries per pass.
 */
#define BCMI_CPN_SRAM_SCAN_RATE  "sram_scan_rate"

/*!
 * Specifies the sram scan repeat interval.
 */
#define BCMI_CPN_SRAM_SCAN_INTERVAL  "sram_scan_interval"

/*!
 * Enable Tomahawk3-like L3/MPLS failover (protection switching)
 * with fixed next hop offset from primary NH on NEXT HOP table.
 * When enabled, half of NEXT HOP table is reserved for failover usage.
 * Note:
 * failover_fixed_nh_offset_enable = 0: disable.
 * failover_fixed_nh_offset_enable = 1: enable.
 */
#define BCMI_FAILOVER_FIXED_NH_OFFSET_ENABLE  "failover_fixed_nh_offset_enable"

/*!
 * Default ALPM route data mode.
 * l3_alpm2_default_route_data_mode = 0: Reduced data mode.
 * l3_alpm2_default_route_data_mode = 1: Full data mode.(By default)
 */
#define BCMI_CPN_L3_ALPM_DEFAULT_DATA_MODE "l3_alpm2_default_route_data_mode"

/*!
 * Number of table entries for field processor exact match table.
 */
#define BCMI_CPN_FPEM_MEM_ENTRIES  "fpem_mem_entries"

/*!
 * Number of table entries for flow tracker table.
 */
#define BCMI_CPN_FTEM_MEM_ENTRIES "ftem_mem_entries"

/*!
 * This configuration property is used to specify the maximum number of
 * entries that can be inserted into the L3_IPV4_MULTICAST_1 table which
 * is looked up to identify destinations for Layer 3 routed multicast
 * packets with IPv4 header.
 * The default value for this property is the maximum HW table capacity
 * that may not be achievable as the physical memories being shared
 * among multiple tables. The default value may also vary by device.
 */
#define BCMI_CPN_IP4_MC_GROUP_TABLE_ENTRIES_MAX "ip4_mc_group_table_entries_max"

/*!
 * This configuration property is used to specify the maximum number of
 * entries that can be inserted into the L3_IPV6_MULTICAST_1 table which
 * is looked up to identify destinations for Layer 3 routed multicast
 * packets with IPv6 header.
 * The default value for this property is the maximum HW table capacity
 * that may not be achievable as the physical memories being shared
 * among multiple tables. The default value may also vary by device.
 */
#define BCMI_CPN_IP6_MC_GROUP_TABLE_ENTRIES_MAX "ip6_mc_group_table_entries_max"

/*!
 * This configuration property is used to specify the maximum number of
 * entries that can be inserted into the L2_IPV4_MULTICAST table which
 * is looked up to identify destinations for Layer 2 multicast
 * packets with IPv4 header.
 * The default value for this property is the maximum HW table capacity
 * that may not be achievable as the physical memories being shared
 * among multiple tables. The default value may also vary by device.
 */
#define BCMI_CPN_IP4_L2MC_TABLE_ENTRIES_MAX "ip4_l2mc_table_entries_max"

/*!
 * This configuration property is used to specify the maximum number of
 * entries that can be inserted into the L2_IPV6_MULTICAST table which
 * is looked up to identify destinations for Layer 2 multicast
 * packets with IPv6 header.
 * The default value for this property is the maximum HW table capacity
 * that may not be achievable as the physical memories being shared
 * among multiple tables. The default value may also vary by device.
 */
#define BCMI_CPN_IP6_L2MC_TABLE_ENTRIES_MAX "ip6_l2mc_table_entries_max"

/*!
 * This configuration property is used to specify the maximum number of
 * entries that can be inserted into the L3_IPV4_MULTICAST_2 table which
 * is looked up to identify destinations for Layer 3 routed multicast
 * packets with IPv4 header.
 * The default value for this property is the maximum HW table capacity
 * that may not be achievable as the physical memories being shared
 * among multiple tables. The default value may also vary by device.
 */
#define BCMI_CPN_IP4_MC_SOURCE_GROUP_TABLE_ENTRIES_MAX "ip4_mc_source_group_table_entries_max"

/*!
 * This configuration property is used to specify the maximum number of
 * entries that can be inserted into the L3_IPV6_MULTICAST_2 table which
 * is looked up to identify destinations for Layer 3 routed multicast
 * packets with IPv6 header.
 * The default value for this property is the maximum HW table capacity
 * that may not be achievable as the physical memories being shared
 * among multiple tables. The default value may also vary by device.
 */
#define BCMI_CPN_IP6_MC_SOURCE_GROUP_TABLE_ENTRIES_MAX "ip6_mc_source_group_table_entries_max"

/*!
 * This configuration property is used to specify the maximum number of
 * entries that can be inserted into the L3_IPV4_UNICAST table which
 * is looked up for L3 routed IPv4 unicast packets with 1) destination
 * IP address for next hop forwarding 2) source IP address for uRPF
 * check.
 * The default value for this property is the maximum HW table capacity
 * that may not be achievable as the physical memories being shared
 * among multiple tables. The default value may also vary by device.
 */

#define BCMI_CPN_IP4_UC_TABLE_ENTRIES_MAX "ip4_uc_table_entries_max"

/*!
 * This configuration property is used to specify the maximum number of
 * entries that can be inserted into the L3_IPV6_UNICAST table which
 * is looked up for L3 routed IPv6 unicast packets with 1) destination
 * IP address for next hop forwarding 2) source IP address for uRPF
 * check.
 * The default value for this property is the maximum HW table capacity
 * that may not be achievable as the physical memories being shared
 * among multiple tables. The default value may also vary by device.
 */
#define BCMI_CPN_IP6_UC_TABLE_ENTRIES_MAX "ip6_uc_table_entries_max"

/*!
 * This configuration property is used to specify the maximum number of
 * entries that can be inserted into the EGR_VLAN_TRANSLATION table
 * which provides outgoing packet VLAN OTAG/ITAG, tag actions and QoS
 * remarking lookup pointers for non L2 tunnel flows
 * The default value for this property is the maximum HW table capacity
 * that may not be achievable as the physical memories being shared
 * among multiple tables. The default value may also vary by device.
 */
#define BCMI_CPN_EGR_VLAN_TRANSLATION_VFI_TAG_ACTION_TABLE_ENTRIES_MAX  "egr_vlan_translation_vfi_tag_action_table_entries_max"

/*!
 * This configuration property is used to specify the maximum number of
 * entries that can be inserted into the EGR_VLAN_TRANSLATION_2 table
 * which provides outgoing packet's VXLAN VNID or NVGRE VPNID for L2
 * tunnel flows
 * The default value for this property is the maximum HW table capacity
 * that may not be achievable as the physical memories being shared
 * among multiple tables. The default value may also vary by device.
 */
#define BCMI_CPN_EGR_VLAN_TRANSLATION_VFI_VNID_TABLE_ENTRIES_MAX "egr_vlan_translation_vfi_vnid_table_entries_max"

/*!
 * This configuration property is used to specify the maximum number of
 * entries that can be inserted into the ING_VLAN_XLATE_1 table
 * which is used to translate the incoming packet's VLAN to Virtual
 * Forwarding Instance(VFI) which is layer 2 forwarding domain.
 * The default value for this property is the maximum HW table capacity
 * that may not be achievable as the physical memories being shared
 * among multiple tables. The default value may also vary by device.
 */
#define BCMI_CPN_ING_VLAN_XLATE_OVID_TABLE_ENTRIES_MAX "ing_vlan_xlate_ovid_table_entries_max"

/*!
 * This configuration property is used to specify the maximum number of
 * entries that can be inserted into the ING_VLAN_XLATE_2 table
 * which is used to translate the incoming packet's VLAN to Virtual
 * Forwarding Instance(VFI) which is layer 2 forwarding domain.
 * The default value for this property is the maximum HW table capacity
 * that may not be achievable as the physical memories being shared
 * among multiple tables. The default value may also vary by device.
 */
#define BCMI_CPN_ING_VLAN_XLATE_IVID_TABLE_ENTRIES_MAX  "ing_vlan_xlate_ivid_table_entries_max"

/*!
 * This configuration property is used to specify the maximum number of
 * entries that can be inserted into the ING_VLAN_XLATE_3 table
 * which is used to translate the incoming packet's VLAN to Virtual
 * Forwarding Instance(VFI) which is layer 2 forwarding domain.
 * The default value for this property is the maximum HW table capacity
 * that may not be achievable as the physical memories being shared
 * among multiple tables. The default value may also vary by device.
 */
#define BCMI_CPN_ING_VLAN_XLATE_OVID_IVID_TABLE_ENTRIES_MAX  "ing_vlan_xlate_ovid_ivid_table_entries_max"

/*!
 * This configuration property is used to specify the maximum number of
 * entries that can be inserted into the L2_TUNNEL_VN_ID table
 * which is looked up to provide the layer 2 forwarding domain for L2
 * tunnel VxLAN packets.
 * The default value for this property is the maximum HW table capacity
 * that may not be achievable as the physical memories being shared
 * among multiple tables. The default value may also vary by device.
 */
#define BCMI_CPN_L2_TUNNEL_VNID_TABLE_ENTRIES_MAX  "l2_tunnel_vnid_table_entries_max"

/*!
 * This configuration property is used to specify the maximum number of
 * entries that can be inserted into the L2_TUNNEL_IPV4_DIP table
 * which is looked up to determine tunnel termination for L2 tunnel
 * packet with IPv4 underlay header.
 * The default value for this property is the maximum HW table capacity
 * that may not be achievable as the physical memories being shared
 * among multiple tables. The default value may also vary by device.
 */
#define BCMI_CPN_L2_TUNNEL_IP4_DIP_TABLE_ENTRIES_MAX  "l2_tunnel_ip4_dip_table_entries_max"

/*!
 * This configuration property is used to specify the maximum number of
 * entries that can be inserted into the L2_TUNNEL_IPV4_SIP table
 * which provides tunnel adaption lookup for L2 tunnel packet with IPv4
 * underlay header. If hit, this table provides Source Virtual Port
 * corresponds to source IP from IPv4 underlay header.
 * The default value for this property is the maximum HW table capacity
 * that may not be achievable as the physical memories being shared
 * among multiple tables. The default value may also vary by device.
 */
#define BCMI_CPN_L2_TUNNEL_IP4_SIP_TABLE_ENTRIES_MAX  "l2_tunnel_ip4_sip_table_entries_max"

/*!
 * This configuration property is used to specify the maximum number of
 * entries that can be inserted into the L2_TUNNEL_IPV6_DIP table
 * which is looked up to determine tunnel termination for L2 tunnel
 * packet with IPv6 underlay header.
 * The default value for this property is the maximum HW table capacity
 * that may not be achievable as the physical memories being shared
 * among multiple tables. The default value may also vary by device.
 */
#define BCMI_CPN_L2_TUNNEL_IP6_DIP_TABLE_ENTRIES_MAX  "l2_tunnel_ip6_dip_table_entries_max"

/*!
 * This configuration property is used to specify the maximum number of
 * entries that can be inserted into the L2_TUNNEL_IPV6_SIP table
 * which provides tunnel adaption lookup for L2 tunnel packet with IPv6
 * underlay header. If hit, this table provides Source Virtual Port
 * corresponds to source IP from IPv6 underlay header.
 * The default value for this property is the maximum HW table capacity
 * that may not be achievable as the physical memories being shared
 * among multiple tables. The default value may also vary by device.
 */
#define BCMI_CPN_L2_TUNNEL_IP6_SIP_TABLE_ENTRIES_MAX  "l2_tunnel_ip6_sip_table_entries_max"

/*!
 * This configuration property is used to specify the maximum number of
 * entries that can be inserted into the L3_IPV4_TUNNEL_EM table
 * which is looked up to determine tunnel termination for L3 tunnel
 * packet with IPv4 underlay header.
 * The default value for this property is the maximum HW table capacity
 * that may not be achievable as the physical memories being shared
 * among multiple tables. The default value may also vary by device.
 */
#define BCMI_CPN_L3_TUNNEL_IP4_EM_TABLE_ENTRIES_MAX  "l3_tunnel_ip4_em_table_entries_max"

/*!
 * This configuration property is used to specify the maximum number of
 * entries that can be inserted into the L3_IPV6_TUNNEL_EM table
 * which is looked up to determine tunnel termination for L3 tunnel
 * packet with IPv6 underlay header.
 * The default value for this property is the maximum HW table capacity
 * that may not be achievable as the physical memories being shared
 * among multiple tables. The default value may also vary by device.
 */
#define BCMI_CPN_L3_TUNNEL_IP6_EM_TABLE_ENTRIES_MAX  "l3_tunnel_ip6_em_table_entries_max"

/*!
 * This configuration property is used to specify the maximum number of
 * entries that can be inserted into the L3_IPV4_TUNNEL table
 * which is looked up to determine tunnel termination for L3 tunnel
 * packet with IPv4 underlay header.
 * The default value for this property is the maximum HW table capacity
 * that may not be achievable as the physical memories being shared
 * among multiple tables. The default value may also vary by device.
 */
#define BCMI_CPN_L3_TUNNEL_IP4_TABLE_ENTRIES_MAX  "l3_tunnel_ip4_table_entries_max"

/*!
 * This configuration property is used to specify the maximum number of
 * entries that can be inserted into the L3_IPV6_TUNNEL table
 * which is looked up to determine tunnel termination for L3 tunnel
 * packet with IPv6 underlay header.
 * The default value for this property is the maximum HW table capacity
 * that may not be achievable as the physical memories being shared
 * among multiple tables. The default value may also vary by device.
 */
#define BCMI_CPN_L3_TUNNEL_IP6_TABLE_ENTRIES_MAX  "l3_tunnel_ip6_table_entries_max"

/*!
 * This configuration property is used to specify the maximum number of
 * entries that can be inserted into the MPLS_1 table
 * which is looked up to identify destinations for MPLS packets.
 * The default value for this property is the maximum HW table capacity
 * that may not be achievable as the physical memories being shared
 * among multiple tables. The default value may also vary by device.
 */
#define BCMI_CPN_MPLS_TOP_LABEL_TABLE_ENTRIES_MAX  "mpls_top_label_table_entries_max"

/*!
 * This configuration property is used to specify the maximum number of
 * entries that can be inserted into the MPLS_2 table
 * which is looked up to identify destinations for MPLS packets.
 * The default value for this property is the maximum HW table capacity
 * that may not be achievable as the physical memories being shared
 * among multiple tables. The default value may also vary by device.
 */
#define BCMI_CPN_MPLS_2ND_LABEL_TABLE_ENTRIES_MAX  "mpls_2nd_label_table_entries_max"

/*!
 * This configuration property is used to specify the maximum number of
 * entries that can be inserted into the MPLS_3 table
 * which is looked up to identify destinations for MPLS packets.
 * The default value for this property is the maximum HW table capacity
 * that may not be achievable as the physical memories being shared
 * among multiple tables. The default value may also vary by device.
 */
#define BCMI_CPN_MPLS_3RD_LABEL_TABLE_ENTRIES_MAX  "mpls_3rd_label_table_entries_max"

/*!
 * This configuration property is used to specify the maximum number of
 * entries that can be inserted into the L2_IPV4_MULTICAST_SG table
 * which is looked up to identify destinations for Layer 2 multicast
 * packets with IPv4 header.
 * The default value for this property is the maximum HW table capacity
 * that may not be achievable as the physical memories being shared
 * among multiple tables. The default value may also vary by device.
 */
#define BCMI_CPN_IP4_L2MC_SOURCE_GROUP_TABLE_ENTRIES_MAX "ip4_l2mc_source_group_table_entries_max"

/*!
 * This configuration property is used to specify the maximum number of
 * entries that can be inserted into the L2_IPV6_MULTICAST_SG table
 * which is looked up to identify destinations for Layer 2 multicast
 * packets with IPv6 header.
 * The default value for this property is the maximum HW table capacity
 * that may not be achievable as the physical memories being shared
 * among multiple tables. The default value may also vary by device.
 */
#define BCMI_CPN_IP6_L2MC_SOURCE_GROUP_TABLE_ENTRIES_MAX "ip6_l2mc_source_group_table_entries_max"

/*!
 * This configuration property is used to specify the maximum number of
 * entries that can be inserted into the L3_IPV4_UC_ROUTE table
 * which is used for Algorithmic Longest Prefix Match (ALPM) search on
 * destination IP address from IPv4 header for L3 routed unicast
 * packets. This table is looked up with 1) dst IP address for next hop
 * forwarding 2) src IP address for uRPF check.
 * The default value for this property is the maximum HW table capacity
 * that may not be achievable as the physical memories being shared
 * among multiple tables. The default value may also vary by device.
 */
#define BCMI_CPN_IP4_UC_ROUTE_TABLE_ENTRIES_MAX  "ip4_uc_route_table_entries_max"

/*!
 * This configuration property is used to specify the maximum number of
 * entries that can be inserted into the L3_IPV4_UC_ROUTE_VRF table
 * which is used for Algorithmic Longest Prefix Match (ALPM) search on
 * destination IP address from IPv4 header for L3 routed unicast
 * packets. This table is looked up with 1) dst IP address for next hop
 * forwarding 2) src IP address for uRPF check.
 * The default value for this property is the maximum HW table capacity
 * that may not be achievable as the physical memories being shared
 * among multiple tables. The default value may also vary by device.
 */
#define BCMI_CPN_IP4_UC_ROUTE_VRF_TABLE_ENTRIES_MAX  "ip4_uc_route_vrf_table_entries_max"

/*!
 * This configuration property is used to specify the maximum number of
 * entries that can be inserted into the L3_IPV4_UC_ROUTE_OVERRIDE table
 * which is used for Algorithmic Longest Prefix Match (ALPM) search on
 * destination IP address from IPv4 header for L3 routed unicast
 * packets. This table is looked up with 1) dst IP address for next hop
 * forwarding 2) src IP address for uRPF check.
 * The default value for this property is the maximum HW table capacity
 * that may not be achievable as the physical memories being shared
 * among multiple tables. The default value may also vary by device.
 */
#define BCMI_CPN_IP4_UC_ROUTE_OVERRIDE_TABLE_ENTRIES_MAX  "ip4_uc_route_override_table_entries_max"

/*!
 * This configuration property is used to specify the maximum number of
 * entries that can be inserted into the L3_IPV6_UC_ROUTE table
 * which is used for Algorithmic Longest Prefix Match (ALPM) search on
 * destination IP address from IPv6 header for L3 routed unicast
 * packets. This table is looked up with 1) dst IP address for next hop
 * forwarding 2) src IP address for uRPF check.
 * The default value for this property is the maximum HW table capacity
 * that may not be achievable as the physical memories being shared
 * among multiple tables. The default value may also vary by device.
 */
#define BCMI_CPN_IP6_UC_ROUTE_TABLE_ENTRIES_MAX  "ip6_uc_route_table_entries_max"

/*
 * This configuration property is used to specify the maximum number of
 * entries that can be inserted into the L3_IPV6_UC_ROUTE_VRF table
 * which is used for Algorithmic Longest Prefix Match (ALPM) search on
 * destination IP address from IPv6 header for L3 routed unicast
 * packets. This table is looked up with 1) dst IP address for next hop
 * forwarding 2) src IP address for uRPF check.
 * The default value for this property is the maximum HW table capacity
 * that may not be achievable as the physical memories being shared
 * among multiple tables. The default value may also vary by device.
 */
#define BCMI_CPN_IP6_UC_ROUTE_VRF_TABLE_ENTRIES_MAX  "ip6_uc_route_vrf_table_entries_max"

/*!
 * This configuration property is used to specify the maximum number of
 * entries that can be inserted into the L3_IPV6_UC_ROUTE_OVERRIDE table
 * which is used for Algorithmic Longest Prefix Match (ALPM) search on
 * destination IP address from IPv6 header for L3 routed unicast
 * packets. This table is looked up with 1) dst IP address for next hop
 * forwarding 2) src IP address for uRPF check.
 * The default value for this property is the maximum HW table capacity
 * that may not be achievable as the physical memories being shared
 * among multiple tables. The default value may also vary by device.
 */
#define BCMI_CPN_IP6_UC_ROUTE_OVERRIDE_TABLE_ENTRIES_MAX  "ip6_uc_route_override_table_entries_max"

/*!
 * This configuration property is used to specify the maximum number of
 * entries that can be inserted into the L2_HOST_NARROW table
 * which is looked up using 1)Source MAC address and forwarding VFI for
 * hardware based L2 Learning, 2)Destination MAC address and forwarding
 * VFI for L2 forwarding lookup.
 * The default value for this property is the maximum HW table capacity
 * that may not be achievable as the physical memories being shared
 * among multiple tables. The default value may also vary by device.
 */
#define BCMI_CPN_L2_HOST_NARROW_TABLE_ENTRIES_MAX  "l2_host_narrow_table_entries_max"

/*!
 * This configuration property is used to specify the maximum number of
 * entries that can be inserted into the L2_HOST table
 * which is looked up using 1)Source MAC address and forwarding VFI for
 * hardware based L2 Learning, 2)Destination MAC address and forwarding
 * VFI for L2 forwarding lookup.
 * The default value for this property is the maximum HW table capacity
 * that may not be achievable as the physical memories being shared
 * among multiple tables. The default value may also vary by device.
 */
#define BCMI_CPN_L2_HOST_TABLE_ENTRIES_MAX  "l2_host_table_entries_max"

/*
 * This configuration property is used to specify the maximum number of
 * entries that can be inserted into the EGR_VLAN_TRANSLATION_DW table
 * which provides outgoing packet VLAN OTAG/ITAG, tag actions and QOS
 * remarking lookup pointers for non L2 tunnel flows
 * The default value for this property is the maximum HW table capacity
 * that may not be achievable as the physical memories being shared
 * among multiple tables. The default value may also vary by device.
 */
#define BCMI_CPN_EGR_VLAN_TRANSLATION_DW_TABLE_ENTRIES_MAX  "egr_vlan_translation_dw_table_entries_max"

/*!
 * Set default maximum number of entry moves for multi hash L2 tables.
 */
#define BCMI_CPN_MULTI_HASH_RECURSE_DEPTH_L2 "multi_hash_recurse_depth_l2"

/*!
 * Set default maximum number of entry moves for multi hash L3 IPv4/IPv6 unicast tables.
 */
#define BCMI_CPN_MULTI_HASH_RECURSE_DEPTH_L3 "multi_hash_recurse_depth_l3"

/*!
 * Set default maximum number of entry moves for multi hash field processor exact match tables.
 */
#define BCMI_CPN_MULTI_HASH_RECURSE_DEPTH_EXACT_MATCH "multi_hash_recurse_depth_exact_match"

/*!
 * Set default maximum number of entry moves for multi hash vlan translate 1
 * tables.
 */
#define BCMI_CPN_MULTI_HASH_RECURSE_DEPTH_VLAN_1 "multi_hash_recurse_depth_vlan_1"

/*!
 * Set default maximum number of entry moves for multi hash vlan translate 2
 * tables.
 */
#define BCMI_CPN_MULTI_HASH_RECURSE_DEPTH_VLAN_2 "multi_hash_recurse_depth_vlan_2"

/*!
 * Set default maximum number of entry moves for multi hash egress vlan
 * translate 1 tables.
 */
#define BCMI_CPN_MULTI_HASH_RECURSE_DEPTH_EGRESS_VLAN_1 "multi_hash_recurse_depth_egress_vlan_1"

/*!
 * Set default maximum number of entry moves for multi hash egress vlan
 * translate 2 tables.
 */
#define BCMI_CPN_MULTI_HASH_RECURSE_DEPTH_EGRESS_VLAN_2 "multi_hash_recurse_depth_egress_vlan_2"

/*!
 * Set default maximum number of entry moves for multi hash L3 tunnel tables.
 */
#define BCMI_CPN_MULTI_HASH_RECURSE_DEPTH_L3_TUNNEL "multi_hash_recurse_depth_l3_tunnel"

/*!
 * Set default maximum number of entry moves for multi hash vlan translate 3
 * tables.
 */
#define BCMI_CPN_MULTI_HASH_RECURSE_DEPTH_VLAN_3 "multi_hash_recurse_depth_vlan_3"

/*!
 * Set default maximum number of entry moves for multi hash L2 tunnel tables.
 */
#define BCMI_CPN_MULTI_HASH_RECURSE_DEPTH_L2_TUNNEL "multi_hash_recurse_depth_l2_tunnel"


/*!
 * Set default maximum number of entry moves for multi hash L2 tunnel tables.
 */
#define BCMI_CPN_MULTI_HASH_RECURSE_DEPTH_L2_TUNNEL_VNID "multi_hash_recurse_depth_l2_tunnel_vnid"

/*!
 * Set default maximum number of entry moves for multi hash flow tracker tables.
 */
#define BCMI_CPN_MULTI_HASH_RECURSE_DEPTH_FLOW_TRACKER "multi_hash_recurse_depth_flow_tracker"

/*!
 * Set default maximum number of entry moves for multi hash L3 IPv4/IPv6
 * multicast (*,G) tables.
 */
#define BCMI_CPN_MULTI_HASH_RECURSE_DEPTH_IPMC_GROUP "multi_hash_recurse_depth_ipmc_group"

/*!
 * Set default maximum number of entry moves for multi hash L3 IPv4/IPv6
 * multicast (S,G) tables.
 */
#define BCMI_CPN_MULTI_HASH_RECURSE_DEPTH_IPMC_SOURCE_GROUP "multi_hash_recurse_depth_ipmc_source_group"

/*!
 * Set default maximum number of entry moves for multi hash L2 IPv4/IPv6
 * multicast tables.
 */
#define BCMI_CPN_MULTI_HASH_RECURSE_DEPTH_L2MC "multi_hash_recurse_depth_l2mc"

/*!
* Control the encapsulation mode based on port.
* Note:
* encap_mode = IEEE: port encapsulation mode is common Ethernet.
* encap_mode = HIGIG3: port encapsulation mode is HG3.
* Default value is IEEE.
*/
#define BCMI_CPN_ENCAP_MODE  "encap_mode"

/*!
 * The maximum number of virtual port trunk groups
 * (default is zero).
 */
#define BCMI_CPN_MAX_VP_LAGS  "max_vp_lags"

/*!
 * ALPM Templates
 *   1 - Combined.
 *   2 - Parallel.
 *   3 - Combined + FP Compression.
 *   4 - Parallel + FP Compression.
 *   5 - Combined + uRPF/SIP lookup.
 *   6 - Parallel + uRPF/SIP lookup.
 *   7 - Combined + uRPF/SIP lookup + FP Compression.
 *   8 - FP Compression.
 *   9 - Combined + Mini FP Compression. (ALPM Template 3 with level 1 FP_Comp.)
 *   10 - Parallel + Mini FP Compression. (ALPM Template 4 with level 1 FP_Comp.)
 *   11 - Combined + uRPF + Mini FP Compression. (ALPM Template 7 with level 1 FP_Comp.)
 */
#define BCMI_CPN_L3_ALPM_TEMPLATE  "l3_alpm_template"

/*!
 * Enable IPv6 128b prefix LPM routes. (Default is 1: enabled.)
 */
#define BCMI_CPN_IPV6_LPM_128B_ENABLE  "ipv6_lpm_128b_enable"

/*!
 * ALPM LT usage threshold when using level 3 banks.
 * Return FULL if exceeds this threshold.
 * Default value is 95 percent.
 */
#define BCMI_CPN_L3_ALPM_BNK_THRESHOLD  "l3_alpm2_bnk_threshold"

/*!
 * Maximum size of the packet sent by telemetry application.
 * (default is 1500).
 */
#define BCMI_CPN_TELEMETRY_EXPORT_MAX_PACKET_SIZE  "telemetry_export_max_packet_size"

/*!
 * Maximum number of ports that can be monitored simultaneously by
 * telemetry application.
 * (default is 0).
 */
#define BCMI_CPN_TELEMETRY_MAX_PORTS_MONITOR  "telemetry_max_ports_monitor"

/*!
 * Packet IO mode
 * pktio_mode = 0 : UNET mode.
 * pktio_mode = 1 : KNET mode.
 */
#define BCMI_CPN_PKTIO_MODE  "pktio_mode"

/*!
 * UFT mode.
 * uft_mode = 1: UFT mode 1.
 * uft_mode = 2: UFT mode 2.
 * uft_mode = 3: UFT mode 3.(By default)
 * uft_mode = 4: UFT mode 4.
 * uft_mode = 5: UFT mode 5.
 * uft_mode = 6: UFT mode 6.
 * uft_mode = 7: UFT mode 7.
 * uft_mode = 8: UFT mode 8.
 * uft_mode = 9: UFT mode 9.
 * uft_mode = 10: UFT mode 10.
 * uft_mode = 11: UFT mode 11.
 * uft_mode = 12: UFT mode 12.
 * uft_mode = 13: UFT mode 13.
 * uft_mode = 14: UFT mode 14.
 * uft_mode = 15: UFT mode 15.
 * uft_mode = 16: UFT mode 16.
 * uft_mode = 18: UFT mode 18.
 * uft_mode = 19: UFT mode 19.
 */
#define BCMI_CPN_UFT_MODE  "uft_mode"

/*!
 * This mode control allows users to allocate the additional shared
 * memory resources to enlarge ING_VLAN_XLATE_1 table, or
 * ING_VLAN_XLATE_2/3 tables.
 *
 * uat_mode = 0 (Default): The shared memories are allocated to
 * ING_VLAN_XLATE_2/3 and MPLS_2 tables, with the minimum number of
 * ING_VLAN_XLATE_1 table entries.
 * uat_mode = 1: The shared memories are allocated to
 * ING_VLAN_XLATE_1 and MPLS_2 tables, without
 * ING_VLAN_XLATE_2/3 tables support.
*/
#define BCMI_CPN_UAT_MODE  "uat_mode"

/*!
 * This mode control allows users to allocate the additional shared
 * memory resources to enable EGR_VLAN_TRANSLATION and
 * EGR_VLAN_TRANSLATION_2 tables, or EGR_VLAN_TRANSLATION_DW and
 * EGR_VLAN_TRANSLATION_2 tables.
 *
 * egress_uat_mode = 0 (Default): The shared memories are allocated to
 * EGR_VLAN_TRANSLATION and EGR_VLAN_TRANSLATION_2 tables, without
 * EGR_VLAN_TRANSLATION_DW table support.
 * egress_uat_mode = 1: The shared memories are allocated to
 * EGR_VLAN_TRANSLATION_2 and EGR_VLAN_TRANSLATION_DW tables, without
 * EGR_VLAN_TRANSLATION table support.
*/
#define BCMI_CPN_EGRESS_UAT_MODE  "egress_uat_mode"

/*!
 * This mode provides further control for the shared memory resources
 * which can be allocated to enlarge ING_L3_NEXT_HOP_1/EGR_L3_NEXT_HOP_1
 * tables, or EGR_L3_TUNNEL_0/1 tables.
 *
 * tunnel_mode = 0 (Default): Support the tunnel with the minimum number
 * of EGR_L3_TUNNEL_0/1 table entries and the maximum number of
 * EGR_L3_OIF_2/EGR_MPLS_VPN table entries.
 * tunnel_mode = 1: Do not support the tunnel. The shared memories are
 * allocated to achieve the maximum number of
 * ING_L3_NEXT_HOP_1/EGR_L3_NEXT_HOP_1 table entries, without
 * ING_L3_NEXT_HOP_2/EGR_L3_NEXT_HOP_2 table for the tunnel.
 * tunnel_mode = 2: Support the tunnel with the maximum number of
 * EGR_L3_TUNNEL_0/1 table entries and the minimum number of
 * EGR_L3_OIF_2/EGR_MPLS_VPN table entries.
 */
#define BCMI_CPN_TUNNEL_MODE  "tunnel_mode"

/*!
 * This mode provides further control for allocating the shared memory
 * resources to MY_STATION table and VFP table.
 *
 * vfp_mode = 0 (Default): The shared memory allocated to
 * MY_STATION table is twice allocated to VFP table.
 * vfp_mode = 1: The shared memory allocated to VFP table is twice
 * allocated to MY_STATION table.
 * vfp_mode = 2: All the shared memories are allocated to VFP table, without
 * MY_STATION table support.
 */
#define BCMI_CPN_VFP_MODE  "vfp_mode"

 /*!
  * Set to 2 to enable flowtracker embedded app.
  */
#define BCMI_CPN_FLOWTRACKER_ENABLE "flowtracker_enable"

/*!
 * Maximum number of flow groups monitored by flowtracker embedded app.
 * (default is 255).
 */
#define BCMI_CPN_FLOWTRACKER_MAX_FLOW_GROUPS  "flowtracker_max_flow_groups"

/*!
 * Enterprise number to be used when exporting template sets,
 * containing enterprise specific information elements.
 * (default is 0).
 */
#define BCMI_CPN_FLOWTRACKER_ENTERPRISE_NUMBER  "flowtracker_enterprise_number"

/*!
 * Maximum length of an export packet in bytes that will be sent by Flowtracker embedded app.
 * (default is 1500).
 */
#define BCMI_CPN_FLOWTRACKER_MAX_EXPORT_PKT_LENGTH  "flowtracker_max_export_pkt_length"

/*!
 * Enables the tracking the flow start timestamp information element, 0 - Disable, 1 - Enable.
 * (default is 0).
 */
#define BCMI_CPN_FLOWTRACKER_FLOW_START_TIMESTAMP_IE_ENABLE  "flowtracker_flow_start_timestamp_ie_enable"

/*!
 * Enable elephant monitoring, 0 - Disable, 1 - Enable.
 * (default is 0).
 */
#define BCMI_CPN_FLOWTRACKER_ELEPHANT_ENABLE  "flowtracker_elephant_enable"

/*!
 * Interval at which the flow table is scanned to collect counter information in micro seconds.
 * Minimum value allowed is 100000 micro seconds
 */
#define BCMI_CPN_FLOWTRACKER_SCAN_INTERVAL_USECS  "flowtracker_scan_interval_usecs"

/*!
 * Enable flow learning in hardware.
 * 0 - Disable
 * 1 - Enable
 * 2 - Enable without embedded application
 * (default: 0)
 */
#define BCMI_CPN_FLOWTRACKER_HARDWARE_LEARN_ENABLE  "flowtracker_hardware_learn_enable"

/*!
 * Skip port hw initialization, 0 - Not skip, 1 - Skip.
 */
#define BCMI_CPN_SKIP_PORT_HW_INIT  "skip_port_hw_init"

/*!
 * L2 hit bit collect enable.
 * Note:
 * l2_hitbit_enable = 0: disable collection.
 * l2_hitbit_enable = 1: enable collection.
 */
#define BCMI_CPN_L2_HITBIT_ENABLE  "l2_hitbit_enable"

/*!
 * Number of widest l2 mem entries.
 */
#define BCMI_CPN_L2_MEM_ENTRIES  "l2_mem_entries"

/*!
 * Number of l2mc entries allocated for VLAN DLF flooding.
 * vlan_flooding_l2mc_num_reserved = 2048. Default value.
 * vlan_flooding_l2mc_num_reserved = 0. VLAN DLF domain has to be set explicitly.
 */
#define BCMI_CPN_VLAN_FLOODING_L2MC_NUM_RESERVED  "vlan_flooding_l2mc_num_reserved"

/*!
 * This config property is used to control the VLAN DLF flooding behavior.
 *
 * Note:
 * vlan_flooding_mode = 0: Flood to all ports in this VLAN for DLF case.
 * vlan_flooding_mode = 1: Do not flood to cpu port even if it is in this VLAN
 *                         for DLF case.
 * Default value is 0.
 */
#define BCMI_CPN_VLAN_FLOODING_MODE  "vlan_flooding_mode"

/*!
 * ALPM Hit mode
 * 0 - Disable ALPM hit support. All operations for hit bits are not supported,
 *     including getting, setting and aging.
 * 1 - Enable ALPM hit support, and force clear to hit bits of moving entries.
 * 2 - Enable ALPM hit support, and force set to hit bits of moving entries.
 */
#define BCMI_CPN_L3_ALPM_HIT_MODE "l3_alpm_hit_mode"

/*!
 * Set a shared block mask section. Only 2 traffic types are allowed on a shared block mask section.
 * Available types:
 * - uc:  known unicast
 * - uuc: unknown unicast
 * - mc:  known multicast
 * - umc: unknown multicast
 * - bc:  broadcast
 * Example: shared_block_mask_section = uc_mc, known unicast and known multicast share the same block mask section.
 * Default: shared_block_mask_section = umc_bc
*/
#define BCMI_CPN_SHARED_BLOCK_MASK_SECTION  "shared_block_mask_section"

/*!
 * Memory allocated for BFD encapsulation data (in bytes). Default value is 0.
 * The application will allocate on a per-endpoint basis the exact
 * memory required to store the transmit packet of that particular endpoint.
 */
#define BCMI_CPN_BFD_MAX_PKT_SIZE  "bfd_max_pkt_size"

/*!
 * Number of BFD simple password authentication keys. Default value is 0.
 */
#define BCMI_CPN_BFD_SIMPLE_PASSWORD_KEYS  "bfd_simple_password_keys"

/*!
 * Number of BFD simple password authentication keys. Default value is 0.
 */
#define BCMI_CPN_BFD_SHA1_KEYS  "bfd_sha1_keys"

/*!
 * Number of BFD sessions. Default value is 0.
 */
#define BCMI_CPN_BFD_NUM_SESSIONS  "bfd_num_sessions"

/*!
 * If this config property is enabled (set to 1), there would be a state change event
 * (State would be down for the endpoint) received after the session is created if we
 * do not receive any packetsfrom the peer within detection_multiplier * 1 seconds.
 * Default value is 0.
 */
#define BCMI_CPN_BFD_SESSION_DOWN_EVENT_ON_CREATE  "bfd_session_down_event_on_create"

/*!
 * If the config property is enabled (set to 1), configuring remote discriminator
 * value is always same, not overriding with the learned BFD My discriminator value.
 * Default value is 0.
 */
#define BCMI_CPN_BFD_REMOTE_DISCRIMINATOR  "bfd_remote_discriminator"

/*!
 * Config property is set to 1 by default, FW will trasmit BFD packets with C bit 1.
 * When this config property is set to 0, FW will transmit BFD packets with C bit 0.
 */
#define BCMI_CPN_BFD_CONTROL_PLANE_INDEPENDENCE  "bfd_control_plane_independence"

/*!
 * Enable/Disable the configuration of using endpoint index itself as local
 * discriminator for BFD endpoints.If the config property is enabled (set to 1),
 * any value passed in local_discriminator field in endpoint info will be ignored
 * and endpoint index would be used instead.
 */
#define BCMI_CPN_BFD_USE_ENDPOINT_ID_AS_DISCRIMINATOR  "bfd_use_endpoint_id_as_discriminator"

/*!
 * Enable/Disable of raw ingress inject of BFD Tx packets from BTE for all sessions
 * which includes support for BFD over LAG, ECMP and except micro, echo BFD.
 */
#define BCMI_CPN_BFD_TX_RAW_INGRESS_ENABLE  "bfd_tx_raw_ingress_enable"

/*!
 * Queues can be assigned to the host CPU.
 * Default value is 48.
 */
#define BCMI_CPN_NUM_QUEUES_PCI  "num_queues_pci"

/*!
 * Queues can be assigned to the uControllers.
 * Default value is 0.
 */
#define BCMI_CPN_NUM_QUEUES_UC0  "num_queues_uc0"

/*!
 * Reserve a flexctr action for IPMC hit bit collect.
 * Note:
 * flexctr_action_reserved_ipmc_hitbit = 0: Not reserve a flexctr action for IPMC hit bit.(Default)
 * flexctr_action_reserved_ipmc_hitbit = 1: Reserve a flexctr action for IPMC hit bit.
 */
#define BCMI_CPN_FLEXCTR_ACTION_RSVD_IPMC_HITBIT  "flexctr_action_reserved_ipmc_hitbit"

/*
 * Turn on/off DLB flow monitoring. When set to a non-zero value, DLB flow
 * monitoring feature is enabled. DLB id of 0 is not available when DLB
 * monitoring is enabled.
 * Currently supported on BCM5698x and  BCM5699x only.
 * Valid values are: 0 (disabled) or non-zero (enabled).
 * Default value is 0 (disabled)
 */
#define BCMI_CPN_DLB_FLOW_MONITOR_EN  "dlb_flow_monitor_en"

/*!
 * This configuration allows to install the default entries into the
 * protocol packet forwarding table during SDK init.
 *
 * skip_protocol_default_entries = 0 (Default): Install the default entries.
 * skip_protocol_default_entries = 1: Do not install the default entries.
 */
#define BCMI_CPN_SKIP_PROTOCOL_DEFAULT_ENTRIES "skip_protocol_default_entries"

/*! Maximum possible length of the IFA export packet length in bytes. */
#define BCMI_CPN_IFA_MAX_EXPORT_PKT_LENGTH "ifa_max_export_pkt_length"

/*! Maximum possible length of the incoming IFA packet in bytes. */
#define BCMI_CPN_IFA_MAX_RX_PKT_LENGTH "ifa_rx_pkt_max_length"

/*! IPFIX enterprise number to be used when exporting IFA template sets. */
#define BCMI_CPN_IFA_ENTERPRISE_NUMBER "ifa_enterprise_number"

/*!
 * Reserve a flexstate action or not for LB-ECN enable/disable during the
 * initialization.
 * lb_ecn_flexstate_action_reserve_mode = 0 (default): Reserve a flexstate
 *                                          action to enable LB-ECN.
 * lb_ecn_flexstate_action_reserve_mode = 1: Do not reserve and thus disable
 *                                           LB-ECN.
 */
#define BCMI_CPN_LB_ECN_FLEXSTATE_ACTION_RESERVE_MODE "lb_ecn_flexstate_action_reserve_mode"

/*!
 * Configure size of l3 interface memory in overlay layer.
 * This value is multiple of 1024 on BCM56996.
 */
#define BCMI_CPN_RIOT_OVERLAY_L3_INTF_MEM_SIZE  "riot_overlay_l3_intf_mem_size"

/*!
 * Configure size of l3 egress memory in overlay layer.
 * This value is multiple of 4096 on BCM56996.
 */
#define BCMI_CPN_RIOT_OVERLAY_L3_EGRESS_MEM_SIZE  "riot_overlay_l3_egress_mem_size"

/*!
 * Configures size of L3 ECMP memory in overlay layer.
 * It is used to decide the size of L3_ECMP memories that are available for overlay ECMP lookup.
 * This value is multiple of 4096 on BCM56996.
 */
#define BCMI_CPN_L3_ECMP_MEMBER_FIRST_LKUP_MEM_SIZE  "l3_ecmp_member_first_lkup_mem_size"

/*!
 * The number of ingress flex counter action reserved for global action in pipe unique mode.
 * Default value is 0.
 */
#define BCMI_CPN_GLOBAL_FLEXCTR_ING_ACTION_NUM_RESERVED "global_flexctr_ing_action_num_reserved"

/*!
 * The number of ingress flex counter group action reserved for global action in pipe unique mode.
 * Default value is 0.
 */
#define BCMI_CPN_GLOBAL_FLEXCTR_ING_GROUP_NUM_RESERVED  "global_flexctr_ing_group_num_reserved"

/*!
 * The number of ingress flex counter pool reserved for global action in pipe unique mode.
 * Default value is 0.
 */
#define BCMI_CPN_GLOBAL_FLEXCTR_ING_POOL_NUM_RESERVED  "global_flexctr_ing_pool_num_reserved"

/*!
 * The number of ingress flex counter quantization reserved for global action in pipe unique mode.
 * Default value is 0.
 */
#define BCMI_CPN_GLOBAL_FLEXCTR_ING_QUANT_NUM_RESERVED "global_flexctr_ing_quant_num_reserved"

/*!
 * The number of ingress flex counter operation profile reserved for global action in pipe unique mode.
 * Default value is 0.
 */
#define BCMI_CPN_GLOBAL_FLEXCTR_ING_OP_PROFILE_NUM_RESERVED "global_flexctr_ing_op_profile_num_reserved"

/*!
 * The number of egress flex counter action reserved for global action in pipe unique mode.
 * Default value is 0.
 */
#define BCMI_CPN_GLOBAL_FLEXCTR_EGR_ACTION_NUM_RESERVED  "global_flexctr_egr_action_num_reserved"

/*!
 * The number of egress flex counter group action reserved for global action in pipe unique mode.
 * Default value is 0.
 */
#define BCMI_CPN_GLOBAL_FLEXCTR_EGR_GROUP_NUM_RESERVED  "global_flexctr_egr_group_num_reserved"

/*!
 * The number of egress flex counter pool reserved for global action in pipe unique mode.
 * Default value is 0.
 */
#define BCMI_CPN_GLOBAL_FLEXCTR_EGR_POOL_NUM_RESERVED  "global_flexctr_egr_pool_num_reserved"

/*!
 * The number of egress flex counter quantization reserved for global action in pipe unique mode.
 * Default value is 0.
 */
#define BCMI_CPN_GLOBAL_FLEXCTR_EGR_QUANT_NUM_RESERVED "global_flexctr_egr_quant_num_reserved"

/*!
 * The number of egress flex counter operation profile reserved for global action in pipe unique mode.
 * Default value is 0.
 */
#define BCMI_CPN_GLOBAL_FLEXCTR_EGR_OP_PROFILE_NUM_RESERVED "global_flexctr_egr_op_profile_num_reserved"

/*!
 * Counter eviction enable. 0: disable. 1: enable.
 * Default is 1.
 * Since the hardware counter value is cleared on each read,
 * some operation types(such as DEC, MIN, AVERAGE, RDEC, XOR, etc) and
 * select types(such as PAIRED_COUNTER) will not work properly with
 * counter eviction enabled.
 * If users want to use the above operation types or select types,
 * counter eviction should be disabled.
 */
#define BCMI_CPN_CTR_EVICT_ENABLE  "ctr_evict_enable"

/*!
 * Enable diag shell port mapping. Port names will be assigned in
 * diag shell port order, and the BCM shell will list multiple ports in
 * diag shell port order regardless of the logical port numbering.
 * Default is 1.
*/
#define BCMI_CPN_DPORT_MAP_ENABLE  "dport_map_enable"

/*
 * Property indicates both INT action and opaque data configurable or not in bcm_l3_route_add.
 * Value 0 (default) indicates the legacy internal priority/class ID and concatenated class ID are
 * configurable through the combination of BCM_L3_RPE/l3a_pri/l3a_lookup_class in
 * bcm_l3_route_add. INT action is not supported and thus
 * BCM_L3_FLAGS2_INT_ACTION/l3a_int_action_profile_id/l3a_int_opaque_data_profile_id are not
 * configurable.
 * Value 1 indicates INT action and internal priority/class ID derivation are supported. INT action is
 * configurable through the combination of
 * BCM_L3_FLAGS2_INT_ACTION/l3a_int_action_profile_id/l3a_int_opaque_data_profile_id.
 * Concatenated class ID is not supported.
 */
#define BCMI_CPN_L3_ROUTE_INT_ACTION_VALID  "l3_route_int_action_valid"

/*!
 * Port names for each port type (fe, ge, etc.) will increment
 * by one starting at zero, e.g. if a switch has four xe ports
 * with diag shell port numbers 24, 25, 26, and 27, they will be named
 * xe0, xe1, xe2, and xe3. In non-indexed mode the ports would
 * be named xe24, xe25, xe26, and xe27.
 * Only applicable on dport_map_enable set to 1.
 * 0 - non-indexed mode.
 * 1 - indexed mode.
 * Default is 1.
*/
#define BCMI_CPN_DPORT_MAP_INDEXED  "dport_map_indexed"

/*!
 * Parser mode of a raw port number. Only applicable on dport_map_enable set to 1.
 * Only applicable on dport_map_enable set to 1.
 * 0 - Specifying a raw number instead of a port name in the diag shell will be parsed as diag shell port.
 * 1 - Raw port number to be parsed as logical port number.
 * Default is 0.
*/
#define BCMI_CPN_DPORT_MAP_DIRECT  "dport_map_direct"

/*!
 * Map diag shell port number to logical port number.
 * Only applicable on dport_map_enable set to 1.
*/
#define BCMI_CPN_DPORT_MAP_PORT  "dport_map_port"

/*!
 * The maximum ECMP group index that could be used in ALPM route reduced mode.
 * The value must be a power of 2 - 1. The default is 0.
 */
#define BCMI_CPN_L3_REDUCED_ROUTE_MAX_ECMP_GROUP_INDEX "l3_reduced_route_max_ecmp_group_index"

/*!
 * Restrict 1:1 mapping between the system port and underlay nexthop in a Higig system.
 *   dest_sysport_to_ul_nh = 0: Disabled. (By default)
 *   dest_sysport_to_ul_nh = 1: Enabled.
 */
#define BCMI_CPN_DEST_SYSPORT_TO_UL_NH "dest_sysport_to_ul_nh"

/*!
 * \brief Retrieve a global configuration numeric value.
 *
 * \param [in] unit Unit number.
 * \param [in] name Base name of numeric property to look up.
 * \param [in] def  Default value of numeric property.
 *
 * \retval Value of numeric property.
 */
extern uint32_t
bcmi_ltsw_property_get(int unit, const char *name, uint32_t def);

/*!
 * \brief Retrieve a global configuration string.
 *
 * \param [in] unit Unit number.
 * \param [in] name Base name of property to look up.
 *
 * \retval String of property string.
 */
extern char *
bcmi_ltsw_property_str_get(int unit, const char *name);

/*!
 * \brief Retrieve a per-port configuration numeric value.
 *
 * \param [in] unit Unit number.
 * \param [in] port Zero-based port number.
 * \param [in] name Base name of numeric property to look up.
 * \param [in] def  Default value of numeric property.
 *
 * \retval Value of numeric property.
 */
extern uint32_t
bcmi_ltsw_property_port_get(int unit, int port,
                            const char *name, uint32_t def);

/*!
 * \brief Retrieve a per-port configuration string.
 *
 * \param [in] unit Unit number.
 * \param [in] port Zero-based port number.
 * \param [in] name Base name of property to look up.
 *
 * \retval String of property string.
 */
extern char *
bcmi_ltsw_property_port_str_get(int unit, int port,
                                const char *name);

/*!
 * \brief Initialize configuration property information.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE  Failure.
 */
extern int
bcmi_ltsw_property_init(int unit);

#endif  /* BCMI_LTSW_PROPERTY_H */
