/*
* $Id: compat_6521.h,v 1.0 2020/12/02
* This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
* 
* Copyright 2007-2022 Broadcom Inc. All rights reserved.
*
* RPC Compatibility with sdk-6.5.21 routines
*/

#ifndef _COMPAT_6521_H_
#define _COMPAT_6521_H_

#ifdef BCM_RPC_SUPPORT
#include <bcm_int/compat/compat_6526.h>
#include <bcm_int/compat/compat_6525.h>
#include <bcm_int/compat/compat_6523.h>
#include <bcm_int/compat/compat_6522.h>
#include <bcm/sat.h>
#include <bcm/vlan.h>
#include <bcm/trunk.h>
#include <bcm/field.h>
#include <bcm/fcoe.h>
#include <bcm/l2.h>
#include <bcm/mirror.h>
#include <bcm/switch.h>
#if defined(INCLUDE_L3)
#include <bcm/vxlan.h>
#endif /* defined(INCLUDE_L3) */
#include <bcm/port.h>
#include <bcm/oam.h>
#include <bcm/mpls.h>
#include <bcm/flow.h>
#include <bcm/types.h>
#include <bcm/failover.h>
#if defined(INCLUDE_L3)
#include <bcm/l3.h>
#endif /* defined(INCLUDE_L3) */

#if defined(INCLUDE_BFD)
#include <bcm/bfd.h>
#endif /* defined(INCLUDE_BFD) */




#if defined(INCLUDE_L3)
/* L3 Egress Structure.
@p Description of an L3 forwarding destination.
 */
typedef struct bcm_compat6521_l3_egress_s {
    uint32 flags;    /* Interface flags (BCM_L3_TGID, BCM_L3_L2TOCPU). */
    uint32 flags2;    /* See BCM_L3_FLAGS2_xxx flag definitions. */
    uint32 flags3;    /* See BCM_L3_FLAGS3_xxx flag definitions. */
    bcm_if_t intf;    /* L3 interface (source MAC, tunnel). */
    bcm_mac_t mac_addr;    /* Next hop forwarding destination mac. */
    bcm_vlan_t vlan;    /* Next hop vlan id. */
    bcm_module_t module;    /*  */
    bcm_port_t port;    /* Port packet switched to (if !BCM_L3_TGID). */
    bcm_trunk_t trunk;    /* Trunk packet switched to (if BCM_L3_TGID). */
    uint32 mpls_flags;    /* BCM_MPLS flag definitions. */
    bcm_mpls_label_t mpls_label;    /* MPLS label. */
    bcm_mpls_egress_action_t mpls_action;    /* MPLS action. */
    int mpls_qos_map_id;    /* MPLS EXP map ID. */
    int mpls_ttl;    /* MPLS TTL threshold. */
    uint8 mpls_pkt_pri;    /* MPLS Packet Priority Value. */
    uint8 mpls_pkt_cfi;    /* MPLS Packet CFI Value. */
    uint8 mpls_exp;    /* MPLS Exp. */
    int qos_map_id;    /* General QOS map id */
    bcm_if_t encap_id;    /* Encapsulation index. */
    bcm_failover_t failover_id;    /* Failover Object Index. */
    bcm_if_t failover_if_id;    /* Failover Egress Object index. */
    bcm_multicast_t failover_mc_group;    /* Failover Multicast Group. */
    int dynamic_scaling_factor;    /* Scaling factor for dynamic load balancing thresholds. */
    int dynamic_load_weight;    /* Weight of traffic load in determining a dynamic load balancing member's quality. */
    int dynamic_queue_size_weight;    /* Weight of queue size in determining a dynamic load balancing member's quality. */
    int intf_class;    /* L3 interface class ID */
    uint32 multicast_flags;    /* BCM_L3_MULTICAST flag definitions. */
    uint16 oam_global_context_id;    /* OAM global context id passed from ingress to egress XGS chip. */
    bcm_vntag_t vntag;    /* VNTAG. */
    bcm_vntag_action_t vntag_action;    /* VNTAG action. */
    bcm_etag_t etag;    /* ETAG. */
    bcm_etag_action_t etag_action;    /* ETAG action. */
    uint32 flow_handle;    /* FLOW handle for flex entries. */
    uint32 flow_option_handle;    /* FLOW option handle for flex entries. */
    uint32 flow_label_option_handle;    /* FLOW option handle for egress label flex entries. */
    bcm_flow_logical_field_t logical_fields[BCM_FLOW_MAX_NOF_LOGICAL_FIELDS];    /* logical fields array for flex entries. */
    uint32 num_of_fields;    /* number of logical fields. */
    int counting_profile;    /* counting profile. If not required, set it to BCM_STAT_LIF_COUNTING_PROFILE_NONE */
    int mpls_ecn_map_id;    /* IP ECN/INT CN to MPLS EXP map ID. */
    bcm_l3_ingress_urpf_mode_t urpf_mode;    /* fec rpf mode. */
    bcm_multicast_t mc_group;    /* Multicast Group. */
    bcm_mac_t src_mac_addr;    /* Source MAC Address. */
    bcm_gport_t hierarchical_gport;    /* Hierarchical TM-Flow. */
    uint32 stat_id;    /* Object statistics ID. */
    int stat_pp_profile;    /* Statistics profile. */
    bcm_gport_t vlan_port_id;    /* Pointer to egress vlan translation lif entry in EEDB. */
    int replication_id;    /* Replication copy ID of multicast Egress object. */
    int mtu;    /* MTU. */
    int nat_realm_id;    /* NAT destination realm ID. */
    bcm_qos_egress_model_t egress_qos_model;    /* egress qos and ttl model */
    int egress_opaque_ctrl_id;    /* Egress opaque control ID. */
} bcm_compat6521_l3_egress_t;

/* Create an Egress forwarding object. */
extern int bcm_compat6521_l3_egress_create(
    int unit,
    uint32 flags,
    bcm_compat6521_l3_egress_t *egr,
    bcm_if_t *if_id);

/* Get an Egress forwarding object. */
extern int bcm_compat6521_l3_egress_get(
    int unit,
    bcm_if_t intf,
    bcm_compat6521_l3_egress_t *egr);

/* Find an interface pointing to an Egress forwarding object. */
extern int bcm_compat6521_l3_egress_find(
    int unit,
    bcm_compat6521_l3_egress_t *egr,
    bcm_if_t *intf);

/* Retrieve valid allocation for a given number of FECs. */
extern int bcm_compat6521_l3_egress_allocation_get(
    int unit,
    uint32 flags,
    bcm_compat6521_l3_egress_t *egr,
    uint32 nof_members,
    bcm_if_t *if_id);

/* Set a failover egress object with fixed offset to protect primary NH intf. */
extern int bcm_compat6521_failover_egress_set(
    int unit,
    bcm_if_t intf,
    bcm_compat6521_l3_egress_t *failover_egr);

/* Get a failover egress object with fixed offset to protect primary NH intf. */
extern int bcm_compat6521_failover_egress_get(
    int unit,
    bcm_if_t intf,
    bcm_compat6521_l3_egress_t *failover_egr);

/* L3 ECMP structure */
typedef struct bcm_compat6521_l3_egress_ecmp_s {
    uint32 flags;    /* See BCM_L3_xxx flag definitions. */
    bcm_if_t ecmp_intf;    /* L3 interface ID pointing to egress ecmp object. */
    int max_paths;    /* Max number of paths in ECMP group. If max_paths <= 0, the default max path which can be set by the API bcm_l3_route_max_ecmp_set will be picked. */
    uint32 ecmp_group_flags;    /* BCM_L3_ECMP_xxx flags. */
    uint32 dynamic_mode;    /* Dynamic load balancing mode. See BCM_L3_ECMP_DYNAMIC_MODE_xxx definitions. */
    uint32 dynamic_size;    /* Number of flows for dynamic load balancing. Valid values are 512, 1k, doubling up to 32k */
    uint32 dynamic_age;    /* Inactivity duration, in microseconds. */
    uint32 dynamic_load_exponent;    /* The exponent used in the exponentially weighted moving average calculation of historical member load. */
    uint32 dynamic_expected_load_exponent;    /* The exponent used in the exponentially weighted moving average calculation of historical expected member load. */
    bcm_l3_dgm_t dgm;    /* DGM properties */
    uint32 stat_id;    /* Object statistics ID. */
    int stat_pp_profile;    /* Statistics profile. */
    bcm_l3_ecmp_urpf_mode_t rpf_mode;    /* Define RPF mode for the ECMP group. */
    bcm_l3_tunnel_priority_info_t tunnel_priority;    /* ECMP tunnel priority. */
} bcm_compat6521_l3_egress_ecmp_t;

/* Resilient ecmp entry */
typedef struct bcm_compat6521_l3_egress_ecmp_resilient_entry_s {
    uint64 hash_key;    /* Hash key. */
    bcm_compat6521_l3_egress_ecmp_t ecmp;    /* ECMP. */
    bcm_if_t intf;    /* L3 interface. */
} bcm_compat6521_l3_egress_ecmp_resilient_entry_t;

/* Replace ECMP resilient entries matching given criteria. */
extern int bcm_compat6521_l3_egress_ecmp_resilient_replace(
    int unit,
    uint32 flags,
    bcm_compat6521_l3_egress_ecmp_resilient_entry_t *match_entry,
    int *num_entries,
    bcm_compat6521_l3_egress_ecmp_resilient_entry_t *replace_entry);

/* Add ECMP resilient entries matching given criteria. */
extern int bcm_compat6521_l3_egress_ecmp_resilient_add(
    int unit,
    uint32 flags,
    bcm_compat6521_l3_egress_ecmp_resilient_entry_t *entry);

/* Delete ECMP resilient entries matching given criteria. */
extern int bcm_compat6521_l3_egress_ecmp_resilient_delete(
    int unit,
    uint32 flags,
    bcm_compat6521_l3_egress_ecmp_resilient_entry_t *entry);

/* Create an Egress ECMP forwarding object. */
extern int bcm_compat6521_l3_ecmp_create(
    int unit,
    uint32 options,
    bcm_compat6521_l3_egress_ecmp_t *ecmp_info,
    int ecmp_member_count,
    bcm_l3_ecmp_member_t *ecmp_member_array);

/* Get info about an Egress ECMP forwarding object. */
extern int bcm_compat6521_l3_ecmp_get(
    int unit,
    bcm_compat6521_l3_egress_ecmp_t *ecmp_info,
    int ecmp_member_size,
    bcm_l3_ecmp_member_t *ecmp_member_array,
    int *ecmp_member_count);

/* Find an Egress ECMP forwarding object. */
extern int bcm_compat6521_l3_ecmp_find(
    int unit,
    int ecmp_member_count,
    bcm_l3_ecmp_member_t *ecmp_member_array,
    bcm_compat6521_l3_egress_ecmp_t *ecmp_info);

/* Create an Egress ECMP forwarding object. */
extern int bcm_compat6521_l3_egress_ecmp_create(
    int unit,
    bcm_compat6521_l3_egress_ecmp_t *ecmp,
    int intf_count,
    bcm_if_t *intf_array);

/* Destroy an Egress ECMP forwarding object. */
extern int bcm_compat6521_l3_egress_ecmp_destroy(
    int unit,
    bcm_compat6521_l3_egress_ecmp_t *ecmp);

/* Get info about an Egress ECMP forwarding object. */
extern int bcm_compat6521_l3_egress_ecmp_get(
    int unit,
    bcm_compat6521_l3_egress_ecmp_t *ecmp,
    int intf_size,
    bcm_if_t *intf_array,
    int *intf_count);

/* Add an Egress forwarding object to an Egress ECMP forwarding object. */
extern int bcm_compat6521_l3_egress_ecmp_add(
    int unit,
    bcm_compat6521_l3_egress_ecmp_t *ecmp,
    bcm_if_t intf);

/* Delete an Egress forwarding object from an Egress ECMP forwarding object. */
extern int bcm_compat6521_l3_egress_ecmp_delete(
    int unit,
    bcm_compat6521_l3_egress_ecmp_t *ecmp,
    bcm_if_t intf);

/* Find an Egress ECMP forwarding object. */
extern int bcm_compat6521_l3_egress_ecmp_find(
    int unit,
    int intf_count,
    bcm_if_t *intf_array,
    bcm_compat6521_l3_egress_ecmp_t *ecmp);

/* Update an ECMP table that was already allocated, used in cases where an ECMP have more than one table. */
extern int bcm_compat6521_l3_egress_ecmp_tunnel_priority_set(
    int unit,
    bcm_compat6521_l3_egress_ecmp_t *ecmp,
    int intf_count,
    bcm_if_t *intf_array);

#endif /* defined(INCLUDE_L3) */

/* Structure describing a trunk member. */
typedef struct bcm_compat6521_trunk_member_s {
    uint32 flags;    /* BCM_TRUNK_MEMBER_xxx */
    bcm_gport_t gport;    /* Trunk member GPORT ID. */
    int dynamic_scaling_factor;    /* Dynamic load balancing threshold scaling factor. */
    int dynamic_load_weight;    /* Relative weight of historical load in determining member quality. */
    int dynamic_queue_size_weight;    /* Relative weight of queue size in determining member quality. */
} bcm_compat6521_trunk_member_t;

/* Trunk group attributes structure. */
typedef struct bcm_compat6521_trunk_info_s {
    uint32 flags;                       /* BCM_TRUNK_FLAG_xxx. */
    int psc;                            /* Port selection criteria. */
    int ipmc_psc;                       /* Port selection criteria for software
                                           IPMC trunk resolution. */
    int dlf_index;                      /* DLF/broadcast port for trunk group. */
    int mc_index;                       /* Multicast port for trunk group. */
    int ipmc_index;                     /* IPMC port for trunk group. */
    int dynamic_size;                   /* Number of flows for dynamic load
                                           balancing. Valid values are 512, 1k,
                                           doubling up to 32k */
    int dynamic_age;                    /* Inactivity duration, in microseconds. */
    int dynamic_load_exponent;          /* The exponent used in the
                                           exponentially weighted moving average
                                           calculation of historical member
                                           load. */
    int dynamic_expected_load_exponent; /* The exponent used in the
                                           exponentially weighted moving average
                                           calculation of historical expected
                                           member load. */
    bcm_trunk_t master_tid;             /* Master trunk id. */
} bcm_compat6521_trunk_info_t;

/* Get the current attributes and member ports for the specified trunk group. */
extern int bcm_compat6521_trunk_get(
    int unit,
    bcm_trunk_t tid,
    bcm_compat6521_trunk_info_t *t_data,
    int member_max,
    bcm_compat6521_trunk_member_t *member_array,
    int *member_count);

/* Specify the ports in a trunk group. */
extern int bcm_compat6521_trunk_set(
    int unit,
    bcm_trunk_t tid,
    bcm_compat6521_trunk_info_t *trunk_info,
    int member_count,
    bcm_compat6521_trunk_member_t *member_array);

/* Add a member to a trunk group. */
extern int bcm_compat6521_trunk_member_add(
    int unit,
    bcm_trunk_t tid,
    bcm_compat6521_trunk_member_t *member);

/* Delete a member from a trunk group. */
extern int bcm_compat6521_trunk_member_delete(
    int unit,
    bcm_trunk_t tid,
    bcm_compat6521_trunk_member_t *member);

#if defined(INCLUDE_L3)
/* structure of MPLS esi object. */
typedef struct bcm_compat6521_mpls_esi_info_s {
    uint32 flags;    /* BCM_MPLS_ESI_xxx. */
    bcm_gport_t src_port;    /* GPORT of incoming eth segment's port. */
    uint32 out_class_id;    /* Multi-homing peer class. */
    bcm_mpls_label_t esi_label;    /* Will be set on the ESI label. */
} bcm_compat6521_mpls_esi_info_t;

/* Add Egress special labels. */
extern int bcm_compat6521_mpls_esi_encap_add(
    int unit,
    bcm_compat6521_mpls_esi_info_t *esi_info);

/* Get esi encap id. */
extern int bcm_compat6521_mpls_esi_encap_get(
    int unit,
    bcm_compat6521_mpls_esi_info_t *esi_info);

/* Delete esi encap id values. */
extern int bcm_compat6521_mpls_esi_encap_delete(
    int unit,
    bcm_compat6521_mpls_esi_info_t *esi_info);

/* Flow tunneling initiator structure. */
typedef struct bcm_compat6521_flow_tunnel_initiator_s {
    uint32 flags;    /* Configuration flags. */
    bcm_tunnel_type_t type;    /* Tunnel type. */
    int ttl;    /* Tunnel header TTL. */
    bcm_mac_t dmac;    /* Destination MAC address. */
    bcm_ip_t dip;    /* Tunnel header DIP (IPv4). */
    bcm_ip_t sip;    /* Tunnel header SIP (IPv4). */
    bcm_ip6_t sip6;    /* Tunnel header SIP (IPv6). */
    bcm_ip6_t dip6;    /* Tunnel header DIP (IPv6). */
    uint32 flow_label;    /* Tunnel header flow label (IPv6). */
    bcm_tunnel_dscp_select_t dscp_sel;    /* Tunnel header DSCP select. */
    int dscp;    /* Tunnel header DSCP value. */
    int dscp_map;    /* DSCP-map ID. */
    bcm_gport_t tunnel_id;    /* Tunnel ID */
    uint16 udp_dst_port;    /* Destination UDP port */
    uint16 udp_src_port;    /* Source UDP port */
    bcm_mac_t smac;    /* Src MAC */
    bcm_vlan_t vlan;    /* Tunnel VLAN */
    uint16 tpid;    /* Tunnel TPID */
    uint8 pkt_pri;    /* Tunnel priority */
    uint8 pkt_cfi;    /* Tunnel CFI */
    uint16 ip4_id;    /* IPv4 ID. */
    bcm_if_t l3_intf_id;    /* l3 Interface ID. */
    bcm_gport_t flow_port;    /* Flow port ID */
    uint32 valid_elements;    /* bitmap of valid fields */
    uint32 flow_handle;    /* flow handle */
    uint32 flow_option;    /* flow option */
    bcm_tunnel_dscp_ecn_select_t dscp_ecn_sel;    /* Tunnel header DSCP and ECN select. */
    int dscp_ecn_map;    /* DSCP and ECN map ID. */
    uint8 ecn;    /* Tunnel header ECN value */
    bcm_tunnel_pri_select_t pri_sel;    /* Tunnel header priority and CFI select. */
} bcm_compat6521_flow_tunnel_initiator_t;

/* Create a flow tunnel initiator object */
extern int bcm_compat6521_flow_tunnel_initiator_create(
    int unit,
    bcm_compat6521_flow_tunnel_initiator_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field);

/* Get the flow tunnel initiator object */
extern int bcm_compat6521_flow_tunnel_initiator_get(
    int unit,
    bcm_compat6521_flow_tunnel_initiator_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field);


#endif /* defined(INCLUDE_L3) */

/* OAM endpoint object.  This is equivalent to an 802.1ag Maintenance Endpoint (MEP). */
typedef struct bcm_compat6521_oam_endpoint_info_s {
    uint32 flags;    /*  */
    uint32 flags2;    /*  */
    uint32 opcode_flags;    /* OAM opcode flags */
    uint32 lm_flags;    /* OAM loss measurment flags */
    bcm_oam_endpoint_t id;    /*  */
    bcm_oam_endpoint_type_t type;    /*  */
    bcm_oam_group_t group;    /* The ID of the group to which this endpoint belongs */
    uint16 name;    /* A unique identifier of the endpoint within the group.  This is equivalent to the MEPID in 802.1ag. */
    bcm_oam_endpoint_t local_id;    /* Used by remote endpoints only. */
    int level;    /* This is equivalent to the Maintenance Domain Level (MDL) in 802.1ag. */
    int ccm_period;    /* For local endpoints, this is the CCM transmission period in ms.  For remote endpoints, this is the period on which CCM timeout is based. */
    bcm_vlan_t vlan;    /* The VLAN associated with this endpoint */
    bcm_vlan_t inner_vlan;    /* Configure with CVID for two-tag operation or set to 0 for one-tag */
    bcm_gport_t gport;    /* The gport associated with this endpoint */
    bcm_gport_t tx_gport;    /* TX gport associated with this endpoint */
    int trunk_index;    /* The trunk port index for this endpoint */
    bcm_if_t intf_id;    /* Interface Identifier */
    bcm_mpls_label_t mpls_label;    /* The MPLS label associated with packets received from the Peer Endpoint */
    bcm_mpls_egress_label_t egress_label;    /* The MPLS outgoing label information for the Local Endpoint */
    bcm_oam_mpls_network_info_t mpls_network_info;    /* MPLS networks information */
    bcm_mac_t dst_mac_address;    /* The destination MAC address associated with this endpoint */
    bcm_mac_t src_mac_address;    /* The source MAC address associated with this endpoint */
    uint8 pkt_pri;    /* Egress marking for outgoing CCMs */
    uint8 inner_pkt_pri;    /* Packet priority of inner encpsulated packet */
    uint16 inner_tpid;    /* Tpid of inner encpsulated packet */
    uint16 outer_tpid;    /* Tpid of outer encpsulated packet */
    bcm_cos_t int_pri;    /* Egress queuing for outgoing CCMs. For DownMEPs, this represents the index from the base queue. For UpMEPs, this represents the internal priority itself. */
    uint8 cpu_qid;    /* CPU queue for CCMs */
    uint8 pri_map[BCM_OAM_INTPRI_MAX];    /* Priority mapping for LM counter table */
    uint32 faults;    /* Fault flags */
    uint32 persistent_faults;    /* Persistent fault flags */
    uint32 clear_persistent_faults;    /* Persistent fault flags to clear on a 'get' */
    int ing_map;    /* Ingress QoS map profile */
    int egr_map;    /* Egress QoS map profile */
    uint8 ms_pw_ttl;    /* TTL used for multi-segment pseudowire */
    uint8 port_state;    /* Endpoint port status */
    uint8 interface_state;    /* Endpoint interface status */
    bcm_oam_vccv_type_t vccv_type;    /* VCCV pseudowire type */
    bcm_vpn_t vpn;    /* VPN id */
    int lm_counter_base_id;    /* Counter id associated with the mep */
    uint8 lm_counter_if;    /* Counter interface associated with the mep */
    uint8 loc_clear_threshold;    /* Number of packets required to reset the Loss-of-Continuity status per end point */
    bcm_oam_timestamp_format_t timestamp_format;    /* DM time stamp format - NTP/IEEE1588(PTP) */
    bcm_oam_tlv_t tlvs[BCM_OAM_MAX_NUM_TLVS_FOR_DM];    /* TLV contents of DM */
    uint16 subport_tpid;    /* Tpid of subport Vlan in case of triple vlan encapsulation */
    bcm_gport_t remote_gport;    /* Local endpoint CCMs are forwarded to this remote unit for processing. */
    bcm_gport_t mpls_out_gport;    /* Out gport used for TX counting by BHH endpoints. */
    int sampling_ratio;    /* 0 - No packets sampled to the CPU. 1-8 - Count of packets (with events) that need to arrive before one is sampled to the CPU. */
    uint32 lm_payload_offset;    /* Offset in bytes from TOP of the MPLS label stack from where the payload starts for byte count computation. */
    uint32 lm_cos_offset;    /* Offset of the Label from top of the stack which gives the EXP value for deriving the COS value - valid values are 0/1/2. */
    bcm_oam_lm_counter_type_t lm_ctr_type;    /* BYTE/PACKET. */
    bcm_oam_lm_counter_size_t lm_ctr_sample_size;    /* Valid values are 32/64. */
    uint32 pri_map_id;    /* OAM priority map id. */
    int lm_ctr_pool_id;    /* The loss measurement counter pool id from which the counters should be allocated. Valid values are 0/1/2. */
    uint32 ccm_tx_update_lm_counter_base_id[BCM_OAM_LM_COUNTER_MAX];    /* The base index of the LM counter to be updated by Tx CCM packets */
    uint32 ccm_tx_update_lm_counter_offset[BCM_OAM_LM_COUNTER_MAX];    /* Offset to the Base LM counter Id to be incremented by Tx CCM packets */
    uint8 ccm_tx_update_lm_counter_action[BCM_OAM_LM_COUNTER_MAX];    /* LM Counter action (type bcm_oam_lm_counter_action_t) to be used by TX CCM packets */
    uint8 ccm_tx_update_lm_counter_size;    /* Number of LM counters to be incremented by Tx CCM packets */
    uint32 session_id;    /* OAM session id for packet processing in BTE. In FP based OAM - This will indicate flex counter base ID */
    uint8 session_num_entries;    /* Number of entries that can result in same session ID. In FP based OAM - This will indicate number of flex counter entries corresponding to same OAM session */
    uint8 lm_count_profile;    /* LM count profile for this endpoint. It will be 1 or 0 for selecting one of the two OAM LM count profiles. */
    uint8 mpls_exp;    /* EXP on which BHH will be running */
    bcm_oam_endpoint_t action_reference_id;    /* Reference endpoint id whose actions will be used on the new created endpoint. Default value: BCM_OAM_ENDPOINT_INVALID. API call will set the parameter to BCM_OAM_ENDPOINT_INVALID  */
    bcm_oam_profile_t acc_profile_id;    /* Used by accelerated endpoints. */
    bcm_oam_endpoint_memory_type_t endpoint_memory_type;    /* Type of default endpoint memory */
    int punt_good_packet_period;    /* OAM good packets sampling period. Every punt_good_packet_period milliseconds, a single packet is punted to the CPU */
    uint16 extra_data_index;    /* Pointer to first extra data entry used per MEP for additional features */
    bcm_oam_endpoint_signal_t rx_signal;    /* Expected Signal Indication */
    bcm_oam_endpoint_signal_t tx_signal;    /* Transmitted Signal Indication */
} bcm_compat6521_oam_endpoint_info_t;

/* Create or replace an OAM endpoint object. */
extern int bcm_compat6521_oam_endpoint_create(
    int unit,
    bcm_compat6521_oam_endpoint_info_t *endpoint_info);

/* Get an OAM endpoint object. */
extern int bcm_compat6521_oam_endpoint_get(
    int unit,
    bcm_oam_endpoint_t endpoint,
    bcm_compat6521_oam_endpoint_info_t *endpoint_info);

#if defined(INCLUDE_BFD)
/* BFD RX discard stats structure */
typedef struct bcm_compat6521_bfd_discard_stat_s {
    uint32 bfd_ver_err;
    uint32 bfd_len_err;
    uint32 bfd_detect_mult;
    uint32 bfd_my_disc;
    uint32 bfd_p_f_bit;
    uint32 bfd_m_bit;
    uint32 bfd_auth_type_mismatch;
    uint32 bfd_auth_simple_err;
    uint32 bfd_auth_m_sha1_err;
    uint32 bfd_sess_mismatch;
} bcm_compat6521_bfd_discard_stat_t;

extern int bcm_compat6521_bfd_discard_stat_get(
    int unit,
    bcm_compat6521_bfd_discard_stat_t *discarded_info);

extern int bcm_compat6521_bfd_discard_stat_set(
    int unit,
    bcm_compat6521_bfd_discard_stat_t *discarded_info);
#endif /* defined(INCLUDE_BFD) */

/* port mapping information */
typedef struct bcm_compat6521_port_extender_mapping_info_s {
    bcm_gport_t pp_port;    /* Mapped Local PP port */
    bcm_tunnel_id_t tunnel_id;    /* Tunnel-ID */
    bcm_gport_t phy_port;    /* Physical Port connected to interface x channel */
    bcm_vlan_t vlan;    /* VLAN-ID */
    uint32 user_define_value;    /* User define value from header */
} bcm_compat6521_port_extender_mapping_info_t;

/* Configure/retrieve port extender mapping from different inputs to Local pp port. */
extern int bcm_compat6521_port_extender_mapping_info_set(
    int unit,
    uint32 flags,
    bcm_port_extender_mapping_type_t type,
    bcm_compat6521_port_extender_mapping_info_t *mapping_info);

/* Configure/retrieve port extender mapping from different inputs to Local pp port. */
extern int bcm_compat6521_port_extender_mapping_info_get(
    int unit,
    uint32 flags,
    bcm_port_extender_mapping_type_t type,
    bcm_compat6521_port_extender_mapping_info_t *mapping_info);

/* Mirror source structure. */
typedef struct bcm_compat6521_mirror_source_s {
    bcm_mirror_source_type_t type;      /* Mirror source type. */
    uint32 flags;                       /* Mirror source flags. See
                                           MIRROR_SOURCE_xxx flag definitions. */
    bcm_port_t port;                    /* Port/gport id. */
    bcm_pkt_trace_event_t trace_event;  /* Trace event. */
    bcm_pkt_drop_event_t drop_event;    /* Drop event. */
    int mod_profile_id;                 /* Mirror-on-drop profile id. */
    uint32 sample_profile_id;           /* Sample profile id. */
    int ep_recirc_profile_id;           /* EP Recirculate profile id. */
} bcm_compat6521_mirror_source_t;

/* Add a mirror destination to a mirror source. */
extern int bcm_compat6521_mirror_source_dest_add(
    int unit,
    bcm_compat6521_mirror_source_t *source,
    bcm_gport_t mirror_dest_id);

/* Delete a mirror destination from a mirror source. */
extern int bcm_compat6521_mirror_source_dest_delete(
    int unit,
    bcm_compat6521_mirror_source_t *source,
    bcm_gport_t mirror_dest_id);

/* Delete all mirror destinations applied on a mirror source. */
extern int bcm_compat6521_mirror_source_dest_delete_all(
    int unit,
    bcm_compat6521_mirror_source_t *source);

/* Get all mirror destinations applied on a mirror source. */
extern int bcm_compat6521_mirror_source_dest_get_all(
    int unit,
    bcm_compat6521_mirror_source_t *source,
    int array_size,
    bcm_gport_t *mirror_dest,
    int *count);

#if defined(INCLUDE_L3)
/* VXLAN Network Domain Config Structure. */
typedef struct bcm_compat6521_vxlan_network_domain_config_s {
    uint32 flags;    /* BCM_VXLAN_NETWORK_DOMAIN_CONFIG_XXX. */
    uint32 vsi;    /* Virtual switch instance. Populated for l2 mapping only. */
    bcm_vrf_t vrf;    /* Virtual router instance. Populated for l3 mapping only. */
    uint32 network_domain;    /* Network identifier. Used to distinct between multiple network. */
    uint32 vni;    /* VXLAN packet VNID. */
    int qos_map_id;    /* QoS Mapped priority */
    bcm_gport_t vlan_port_id;    /* Vlan gport identifier */
} bcm_compat6521_vxlan_network_domain_config_t;

/* Add a local identifier (vsi or vrf) with a VNI mapping. */
extern int bcm_compat6521_vxlan_network_domain_config_add(
    int unit,
    bcm_compat6521_vxlan_network_domain_config_t *config);

/* Get local identifier (vsi or vrf) with VNI mapping. */
extern int bcm_compat6521_vxlan_network_domain_config_get(
    int unit,
    bcm_compat6521_vxlan_network_domain_config_t *config);

/* Remove local identifier (vsi or vrf) with VNI mapping. */
extern int bcm_compat6521_vxlan_network_domain_config_remove(
    int unit,
    bcm_compat6521_vxlan_network_domain_config_t *config);


#endif /* defined(INCLUDE_L3) */
/* PP header information for mirrored packet in DNX devices */
typedef struct bcm_compat6521_mirror_pkt_dnx_pp_header_s {
    uint8 tail_edit_profile;    /* Tail edit profile, used at ETPP for tail editing */
    uint8 eth_header_remove;    /* Indication to remove Ethernet header from mirror copy */
    bcm_gport_t out_vport_ext[3];    /* Extended outlif(s) */
} bcm_compat6521_mirror_pkt_dnx_pp_header_t;

/* Mirror packet system header information for DNX devices */
typedef struct bcm_compat6521_mirror_header_info_s {
    bcm_mirror_pkt_dnx_ftmh_header_t tm;    /* TM header */
    bcm_compat6521_mirror_pkt_dnx_pp_header_t pp;    /* PP header */
    bcm_compat6526_pkt_dnx_udh_t udh[4];    /* UDH header */
} bcm_compat6521_mirror_header_info_t;

/* Set system header information */
extern int bcm_compat6521_mirror_header_info_set(
    int unit,
    uint32 flags,
    bcm_gport_t mirror_dest_id,
    bcm_compat6521_mirror_header_info_t *mirror_header_info);

/* Get system header information */
extern int bcm_compat6521_mirror_header_info_get(
    int unit,
    bcm_gport_t mirror_dest_id,
    uint32 *flags,
    bcm_compat6521_mirror_header_info_t *mirror_header_info);


/* Initialize a VLAN tag action set structure. */
typedef struct bcm_compat6521_vlan_action_set_s {
    bcm_vlan_t new_outer_vlan;    /* New outer VLAN for Add/Replace actions. */
    bcm_vlan_t new_inner_vlan;    /* New inner VLAN for Add/Replace actions. */
    uint8 new_inner_pkt_prio;    /* New inner packet priority for Add/Replace actions. */
    uint8 new_outer_cfi;    /* New outer packet CFI for Add/Replace actions. */
    uint8 new_inner_cfi;    /* New inner packet CFI for Add/Replace actions. */
    bcm_if_t ingress_if;    /* L3 Ingress Interface. */
    int priority;    /* Internal or packet priority. */
    bcm_vlan_action_t dt_outer;    /* Outer-tag action for double-tagged packets. */
    bcm_vlan_action_t dt_outer_prio;    /* Outer-priority-tag action for double-tagged packets. */
    bcm_vlan_action_t dt_outer_pkt_prio;    /* Outer packet priority action for double-tagged packets. */
    bcm_vlan_action_t dt_outer_cfi;    /* Outer packet CFI action for double-tagged packets. */
    bcm_vlan_action_t dt_inner;    /* Inner-tag action for double-tagged packets. */
    bcm_vlan_action_t dt_inner_prio;    /* Inner-priority-tag action for double-tagged packets. */
    bcm_vlan_action_t dt_inner_pkt_prio;    /* Inner packet priority action for double-tagged packets. */
    bcm_vlan_action_t dt_inner_cfi;    /* Inner packet CFI action for double-tagged packets. */
    bcm_vlan_action_t ot_outer;    /* Outer-tag action for single-outer-tagged packets. */
    bcm_vlan_action_t ot_outer_prio;    /* Outer-priority-tag action for single-outer-tagged packets. */
    bcm_vlan_action_t ot_outer_pkt_prio;    /* Outer packet priority action for single-outer-tagged packets. */
    bcm_vlan_action_t ot_outer_cfi;    /* Outer packet CFI action for single-outer-tagged packets. */
    bcm_vlan_action_t ot_inner;    /* Inner-tag action for single-outer-tagged packets. */
    bcm_vlan_action_t ot_inner_pkt_prio;    /* Inner packet priority action for single-outer-tagged packets. */
    bcm_vlan_action_t ot_inner_cfi;    /* Inner packet CFI action for single-outer-tagged packets. */
    bcm_vlan_action_t it_outer;    /* Outer-tag action for single-inner-tagged packets. */
    bcm_vlan_action_t it_outer_pkt_prio;    /* Outer packet priority action for single-inner-tagged packets. */
    bcm_vlan_action_t it_outer_cfi;    /* Outer packet CFI action for single-inner-tagged packets. */
    bcm_vlan_action_t it_inner;    /* Inner-tag action for single-inner-tagged packets. */
    bcm_vlan_action_t it_inner_prio;    /* Inner-priority-tag action for single-inner-tagged packets. */
    bcm_vlan_action_t it_inner_pkt_prio;    /* Inner packet priority action for single-inner-tagged packets. */
    bcm_vlan_action_t it_inner_cfi;    /* Inner packet CFI action for single-inner-tagged packets. */
    bcm_vlan_action_t ut_outer;    /* Outer-tag action for untagged packets. */
    bcm_vlan_action_t ut_outer_pkt_prio;    /* Outer packet priority action for untagged packets. */
    bcm_vlan_action_t ut_outer_cfi;    /* Outer packet CFI action for untagged packets. */
    bcm_vlan_action_t ut_inner;    /* Inner-tag action for untagged packets. */
    bcm_vlan_action_t ut_inner_pkt_prio;    /* Inner packet priority action for untagged packets. */
    bcm_vlan_action_t ut_inner_cfi;    /* Inner packet CFI action for untagged packets. */
    bcm_vlan_pcp_action_t outer_pcp;    /* Outer tag's pcp field action of outgoing packets. */
    bcm_vlan_pcp_action_t inner_pcp;    /* Inner tag's pcp field action of outgoing packets. */
    bcm_policer_t policer_id;    /* Base policer to be used */
    uint16 outer_tpid;    /* New outer-tag's tpid field for modify action */
    uint16 inner_tpid;    /* New inner-tag's tpid field for modify action */
    bcm_vlan_tpid_action_t outer_tpid_action;    /* Action of outer-tag's tpid field */
    bcm_vlan_tpid_action_t inner_tpid_action;    /* Action of inner-tag's tpid field */
    int action_id;    /* Action Set index */
    uint32 class_id;    /* Class ID */
    bcm_tsn_pri_map_t taf_gate_primap;    /* TAF (Time Aware Filtering) gate priority mapping */
    uint32 flags;    /* BCM_VLAN_ACTION_SET_xxx. */
    bcm_vlan_action_t outer_tag;    /* Outer-tag action for all tagged packets. */
    bcm_vlan_action_t inner_tag;    /* Inner-tag action for all tagged packets. */
    bcm_vlan_t forwarding_domain;    /* Forwarding domain (VLAN or VPN). */
    int inner_qos_map_id;    /* Map ID of inner QOS profile */
    int outer_qos_map_id;    /* Map ID of outer QOS profile. */
    uint32 opaque_ctrl_id;    /* Opaque control ID. */
    bcm_vlan_action_t opaque_tag_present;    /* Opaque-tag action for opaque-tagged packets. */
    bcm_vlan_action_t opaque_tag_not_present;    /* Opaque-tag action for non-opaque-tagged packets. */
    bcm_gport_t gport;    /* Gport. */
    uint32 gport_group;    /* Gport group ID. */
} bcm_compat6521_vlan_action_set_t;

/* VLAN/VSAN Translation Action Set. */
typedef struct bcm_compat6521_fcoe_vsan_action_set_s {
    uint32 flags;    /* BCM_FCOE_VSAN_ACTION_*  */
    bcm_fcoe_vsan_id_t new_vsan;    /* new VSAN ID to replace with */
    uint8 new_vsan_pri;    /* new VSAN priority to replace with */
    bcm_fcoe_vsan_vft_t vft;    /* remaining fields of VFT header */
    bcm_if_t ingress_if;    /* TBD */
    bcm_fcoe_vsan_action_t dt_vsan;    /* Vsan action when double tagged */
    bcm_fcoe_vsan_action_t dt_vsan_pri;    /* VSAN Priority action when double tagged */
    bcm_fcoe_vsan_action_t it_vsan;    /* Vsan action when single inner tagged */
    bcm_fcoe_vsan_action_t it_vsan_pri;    /* Vsan Priority action when single inner tagged */
    bcm_fcoe_vsan_action_t ot_vsan;    /* Vsan action when single outer tagged */
    bcm_fcoe_vsan_action_t ot_vsan_pri;    /* Vsan Priority action when single outer tagged */
    bcm_fcoe_vsan_action_t ut_vsan;    /* Vsan action when un-tagged */
    bcm_fcoe_vsan_action_t ut_vsan_pri;    /* Vsan Priority action when un-tagged */
    bcm_compat6521_vlan_action_set_t vlan_action;    /* VLAN actions */
} bcm_compat6521_fcoe_vsan_action_set_t;

/* Add FCOE VSAN and VLAN translation action. */
extern int bcm_compat6521_fcoe_vsan_translate_action_add(
    int unit,
    bcm_fcoe_vsan_translate_key_config_t *key,
    bcm_compat6521_fcoe_vsan_action_set_t *action);

/* Add VLAN actions to a field entry. */
extern int bcm_compat6521_field_action_vlan_actions_add(
    int unit,
    bcm_field_entry_t entry,
    bcm_field_action_t action,
    bcm_compat6521_vlan_action_set_t *vlan_action_set);

/* Retrieve VLAN actions configured to a field entry. */
extern int bcm_compat6521_field_action_vlan_actions_get(
    int unit,
    bcm_field_entry_t entry,
    bcm_field_action_t action,
    bcm_compat6521_vlan_action_set_t *vlan_action_set);

/* Get or set the port's default VLAN tag actions. */
extern int bcm_compat6521_vlan_port_default_action_get(
    int unit,
    bcm_port_t port,
    bcm_compat6521_vlan_action_set_t *action);

/* Get or set the port's default VLAN tag actions. */
extern int bcm_compat6521_vlan_port_default_action_set(
    int unit,
    bcm_port_t port,
    bcm_compat6521_vlan_action_set_t *action);

/* Get or set the egress default VLAN tag actions. */
extern int bcm_compat6521_vlan_port_egress_default_action_get(
    int unit,
    bcm_port_t port,
    bcm_compat6521_vlan_action_set_t *action);

/* Get or set the egress default VLAN tag actions. */
extern int bcm_compat6521_vlan_port_egress_default_action_set(
    int unit,
    bcm_port_t port,
    bcm_compat6521_vlan_action_set_t *action);

/* Add protocol-based VLAN with specified action. If the entry already exists, update the action. */
extern int bcm_compat6521_vlan_port_protocol_action_add(
    int unit,
    bcm_port_t port,
    bcm_port_frametype_t frame,
    bcm_port_ethertype_t ether,
    bcm_compat6521_vlan_action_set_t *action);

/* Get protocol-based VLAN specified action. */
extern int bcm_compat6521_vlan_port_protocol_action_get(
    int unit,
    bcm_port_t port,
    bcm_port_frametype_t frame,
    bcm_port_ethertype_t ether,
    bcm_compat6521_vlan_action_set_t *action);

/* Add an association from MAC address to VLAN actions to use for assigning VLAN tag actions to untagged packets. */
extern int bcm_compat6521_vlan_mac_action_add(
    int unit,
    bcm_mac_t mac,
    bcm_compat6521_vlan_action_set_t *action);

/* Retrieve an association from MAC address to VLAN actions, which
are used for VLAN tag assignment to untagged packets. */
extern int bcm_compat6521_vlan_mac_action_get(
    int unit,
    bcm_mac_t mac,
    bcm_compat6521_vlan_action_set_t *action);

/* Add an entry to the VLAN Translation table and assign VLAN actions. */
extern int bcm_compat6521_vlan_translate_action_add(
    int unit,
    bcm_gport_t port,
    bcm_vlan_translate_key_t key_type,
    bcm_vlan_t outer_vlan,
    bcm_vlan_t inner_vlan,
    bcm_compat6521_vlan_action_set_t *action);

/* Add an entry to the VLAN Translation table and assign VLAN actions. */
extern int bcm_compat6521_vlan_translate_action_create(
    int unit,
    bcm_gport_t port,
    bcm_vlan_translate_key_t key_type,
    bcm_vlan_t outer_vlan,
    bcm_vlan_t inner_vlan,
    bcm_compat6521_vlan_action_set_t *action);

/* Get the assigned VLAN actions for the given port, key type, and VLAN tags. */
extern int bcm_compat6521_vlan_translate_action_get(
    int unit,
    bcm_gport_t port,
    bcm_vlan_translate_key_t key_type,
    bcm_vlan_t outer_vlan,
    bcm_vlan_t inner_vlan,
    bcm_compat6521_vlan_action_set_t *action);

/* Add an entry to the egress VLAN Translation table and assign VLAN actions. */
extern int bcm_compat6521_vlan_translate_egress_action_add(
    int unit,
    int port_class,
    bcm_vlan_t outer_vlan,
    bcm_vlan_t inner_vlan,
    bcm_compat6521_vlan_action_set_t *action);

/* Get the assigned VLAN actions for egress VLAN translation on the given port class and VLAN tags. */
extern int bcm_compat6521_vlan_translate_egress_action_get(
    int unit,
    int port_class,
    bcm_vlan_t outer_vlan,
    bcm_vlan_t inner_vlan,
    bcm_compat6521_vlan_action_set_t *action);

/* Add an entry based on gport to the egress VLAN Translation table and assign VLAN actions. */
extern int bcm_compat6521_vlan_translate_egress_gport_action_add(
    int unit,
    bcm_gport_t port,
    bcm_vlan_t outer_vlan,
    bcm_vlan_t inner_vlan,
    bcm_compat6521_vlan_action_set_t *action);

/* Get the assigned VLAN actions for egress VLAN translation on the given gport and VLAN tags. */
extern int bcm_compat6521_vlan_translate_egress_gport_action_get(
    int unit,
    bcm_gport_t port,
    bcm_vlan_t outer_vlan,
    bcm_vlan_t inner_vlan,
    bcm_compat6521_vlan_action_set_t *action);

/* Add an entry to the VLAN Translation table, which assigns VLAN actions for packets matching within the VLAN range(s). */
extern int bcm_compat6521_vlan_translate_action_range_add(
    int unit,
    bcm_gport_t port,
    bcm_vlan_t outer_vlan_low,
    bcm_vlan_t outer_vlan_high,
    bcm_vlan_t inner_vlan_low,
    bcm_vlan_t inner_vlan_high,
    bcm_compat6521_vlan_action_set_t *action);

/* Get assigned VLAN actions from VLAN Translation table for the specified VLAN range(s). */
extern int bcm_compat6521_vlan_translate_action_range_get(
    int unit,
    bcm_gport_t port,
    bcm_vlan_t outer_vlan_low,
    bcm_vlan_t outer_vlan_high,
    bcm_vlan_t inner_vlan_low,
    bcm_vlan_t inner_vlan_high,
    bcm_compat6521_vlan_action_set_t *action);

/* Set a VLAN translation ID instance with tag actions. */
extern int bcm_compat6521_vlan_translate_action_id_set(
    int unit,
    uint32 flags,
    int action_id,
    bcm_compat6521_vlan_action_set_t *action);

/* Get tag actions from a VLAN translation ID instance. */
extern int bcm_compat6521_vlan_translate_action_id_get(
    int unit,
    uint32 flags,
    int action_id,
    bcm_compat6521_vlan_action_set_t *action);

/* Add an association from IP subnet to VLAN actions to use for assigning VLAN tag actions to untagged packets. */
extern int bcm_compat6521_vlan_ip_action_add(
    int unit,
    bcm_vlan_ip_t *vlan_ip,
    bcm_compat6521_vlan_action_set_t *action);

/* Get an association from IP subnet to VLAN actions that used for assigning VLAN tag actions to untagged packets. */
extern int bcm_compat6521_vlan_ip_action_get(
    int unit,
    bcm_vlan_ip_t *vlan_ip,
    bcm_compat6521_vlan_action_set_t *action);

/* Add action for match criteria for Vlan assignment. */
extern int bcm_compat6521_vlan_match_action_add(
    int unit,
    uint32 options,
    bcm_compat6523_vlan_match_info_t *match_info,
    bcm_compat6521_vlan_action_set_t *action_set);

/* Get action for match criteria. */
extern int bcm_compat6521_vlan_match_action_get(
    int unit,
    bcm_compat6523_vlan_match_info_t *match_info,
    bcm_compat6521_vlan_action_set_t *action_set);

/* Get all actions and match criteria for given vlan match. */
extern int bcm_compat6521_vlan_match_action_multi_get(
    int unit,
    bcm_vlan_match_t match,
    int size,
    bcm_compat6523_vlan_match_info_t *match_info_array,
    bcm_compat6521_vlan_action_set_t *action_set_array,
    int *count);

/* This structure contains the configuration of a VLAN. */
typedef struct bcm_compat6521_vlan_control_vlan_s {
    bcm_vrf_t vrf;    /*  */
    bcm_vlan_t forwarding_vlan;    /* Shared VLAN ID. */
    bcm_if_t ingress_if;    /* Mapped Ingress interface. */
    uint16 outer_tpid;    /*  */
    uint32 flags;    /*  */
    bcm_vlan_mcast_flood_t ip6_mcast_flood_mode;    /*  */
    bcm_vlan_mcast_flood_t ip4_mcast_flood_mode;    /*  */
    bcm_vlan_mcast_flood_t l2_mcast_flood_mode;    /*  */
    int if_class;    /*  */
    bcm_vlan_forward_t forwarding_mode;    /*  */
    bcm_vlan_urpf_mode_t urpf_mode;    /*  */
    bcm_cos_queue_t cosq;    /*  */
    int qos_map_id;    /* index to int_pri->queue offset mapping profile */
    bcm_fabric_distribution_t distribution_class;    /* Fabric Distribution Class. */
    bcm_multicast_t broadcast_group;    /* Group to handle broadcast frames */
    bcm_multicast_t unknown_multicast_group;    /* Group to handle unknown multicast frames */
    bcm_multicast_t unknown_unicast_group;    /* Group to handle unknown unicast frames */
    bcm_multicast_t trill_nonunicast_group;    /* Group to handle trill-domain nonunicast frames */
    bcm_trill_name_t source_trill_name;    /* Source RBridge nickname per VLAN */
    int trunk_index;    /* Trunk index for static PSC */
    bcm_vlan_protocol_packet_ctrl_t protocol_pkt;    /* Protocol packet control per VLAN */
    int nat_realm_id;    /* Realm id of interface that this vlan maps to */
    int l3_if_class;    /* L3IIF class id. */
    bcm_vlan_vp_mc_ctrl_t vp_mc_ctrl;    /* VP replication control, Auto, Enable, Disable */
    int aging_cycles;    /* number of aging meta-cycles */
    int entropy_id;    /* aging profile ID */
    bcm_vpn_t vpn;    /* vpn */
    bcm_vlan_t egress_vlan;    /* egress outer vlan */
    uint32 learn_flags;    /* Learn control flags for VLAN-based (BCM_VLAN_LEARN_CONTROL_XXX) */
    uint32 sr_flags;    /* Flags for Seamless Redundancy: BCM_VLAN_CONTROL_SR_FLAG_xxx */
    uint32 flags2;    /* BCM_VLAN_FLAGS2_XXX */
    uint32 ingress_stat_id;    /* Object statistics id ingress. */
    int ingress_stat_pp_profile;    /* Statistic profile ingress. */
    uint32 egress_stat_id;    /* Object statistics id egress. */
    int egress_stat_pp_profile;    /* Statistic profile egress. */
    bcm_compat6521_vlan_action_set_t egress_action;    /* Egress VLAN actions. */
    bcm_vlan_l2_view_t l2_view;    /* L2 table view. */
    int egress_class_id;    /* Class ID for EFP qualifier. */
    int egress_opaque_ctrl_id;    /* Egress opaque control ID. */
    int ingress_opaque_ctrl_id;    /* Ingress opaque control ID. */
    bcm_gport_t unknown_dest;    /* Gport for default destination, valid when BCM_VLAN_FLAGS2_UNKNOWN_DEST is set */
    bcm_vlan_translate_egress_key_t egress_translate_key;    /* Key type for egress VLAN translation */
} bcm_compat6521_vlan_control_vlan_t;

/* Set or retrieve current VLAN properties selectively. */
extern int bcm_compat6521_vlan_control_vlan_selective_get(
    int unit,
    bcm_vlan_t vlan,
    uint32 valid_fields,
    bcm_compat6521_vlan_control_vlan_t *control);

/* Set or retrieve current VLAN properties selectively. */
extern int bcm_compat6521_vlan_control_vlan_selective_set(
    int unit,
    bcm_vlan_t vlan,
    uint32 valid_fields,
    bcm_compat6521_vlan_control_vlan_t *control);

/* Set or retrieve current VLAN properties. */
extern int bcm_compat6521_vlan_control_vlan_get(
    int unit,
    bcm_vlan_t vlan,
    bcm_compat6521_vlan_control_vlan_t *control);

/* Set or retrieve current VLAN properties. */
extern int bcm_compat6521_vlan_control_vlan_set(
    int unit,
    bcm_vlan_t vlan,
    bcm_compat6521_vlan_control_vlan_t control);

/* VLAN Egress Translation Action Set. */
typedef struct bcm_compat6521_vlan_translate_egress_action_set_s {
    uint32 flags;    /*  */
    bcm_compat6521_vlan_action_set_t vlan_action;    /* VLAN actions. */
} bcm_compat6521_vlan_translate_egress_action_set_t;

/* VLAN Egress Translate Key Configuration. */
typedef struct bcm_compat6521_vlan_translate_egress_key_config_s {
    uint32 flags;    /*  */
    bcm_vlan_translate_key_t key_type;    /* Lookup Key Type. */
    bcm_vlan_t inner_vlan;    /* Inner VLAN ID to match. */
    bcm_vlan_t outer_vlan;    /* Outer VLAN ID to match. */
    bcm_gport_t port;    /* Port to match on. */
} bcm_compat6521_vlan_translate_egress_key_config_t;

/* Add an entry to the egress VLAN Translation table and assign VLAN actions. It extends bcm_vlan_egress_action_add API for additional keys. */
extern int bcm_compat6521_vlan_translate_egress_action_extended_add(
    int unit,
    bcm_compat6521_vlan_translate_egress_key_config_t *key_config,
    bcm_compat6521_vlan_translate_egress_action_set_t *action);

/* Retrive an egress translation entry including key types added by bcm_vlan_translate_egress_extended_action_add API. */
extern int bcm_compat6521_vlan_translate_egress_action_extended_get(
    int unit,
    bcm_compat6521_vlan_translate_egress_key_config_t *key_config,
    bcm_compat6521_vlan_translate_egress_action_set_t *action);

/* Delete an egress translation entry including key types added by bcm_vlan_translate_egress_extended_action_add API. */
extern int bcm_compat6521_vlan_translate_egress_action_extended_delete(
    int unit,
    bcm_compat6521_vlan_translate_egress_key_config_t *key_config);

/* Sat ctf stat */
typedef struct bcm_compat6521_sat_ctf_stat_s {
    uint64 received_packet_cnt;    /* Counter packet counter */
    uint64 out_of_order_packet_cnt;    /* Counter for out of order packet */
    uint64 err_packet_cnt;    /* Counter for error packet */
    uint64 received_octet_cnt;    /* Counter for received octet */
    uint64 out_of_order_octet_cnt;    /* Counter for out of order octet */
    uint64 err_octet_cnt;    /* Counter for error octet */
    uint64 last_packet_delay;    /* counter for last packet delay */
    uint64 bin_delay_counters[BCM_SAT_CTF_STAT_MAX_NUM_OF_BINS];    /* Counters for bin delay */
    uint64 accumulated_delay_count;    /* Counter for accumulated delay */
    uint64 max_packet_delay;    /* max packet delay */
    uint64 min_packet_delay;    /* min packet delay */
    uint64 time_of_day_stamp;    /* Time of day stamp */
    uint64 sec_in_avail_state_counter;    /* Counter for sec in avail state */
    uint64 last_packet_seq_number;    /* Last received packet sequence number */
} bcm_compat6521_sat_ctf_stat_t;

/* Get ctf stat values. */
extern int bcm_compat6521_sat_ctf_stat_get(
    int unit,
    bcm_sat_ctf_t ctf_id,
    uint32 flags,
    bcm_compat6521_sat_ctf_stat_t *stat);

/* Device-independent L2 egress structure. */
typedef struct bcm_compat6521_l2_egress_s {
    uint32 flags;    /* BCM_L2_EGRESS_XXX flags. */
    bcm_mac_t dest_mac;    /* Destination MAC address to replace with. */
    bcm_mac_t src_mac;    /* Source MAC address to replace with. */
    bcm_vlan_t outer_vlan;    /* outer vlan tag to replace with. */
    bcm_vlan_t inner_vlan;    /* inner vlan tag to replace with. */
    bcm_ethertype_t ethertype;    /* EtherType to replace with. */
    bcm_gport_t dest_port;    /* Destination gport to associate this l2 egress entry with */
    bcm_if_t encap_id;    /* Encapsulation index */
    uint16 outer_tpid;    /* outer vlan tag TPID */
    uint8 outer_prio;    /* outer vlan tag User Priority */
    uint16 inner_tpid;    /* inner vlan tag TPID */
    bcm_if_t l3_intf;    /* L3 interface (tunnel). Used only in Native Routing overlay protocols. */
    int vlan_qos_map_id;    /* VLAN PCP-DEI QoS settings. Used only in Native Routing overlay protocols. */
    int mpls_extended_label_value;    /* An extended label placed on top of a an egress encapsulation. */
    bcm_gport_t vlan_port_id;    /* vlan gport identifier */
    int recycle_header_extension_length;    /* Specify in bytes the size of extension header after recycle header */
    int additional_egress_termination_size;    /* Specify in bytes the size of egress additional termination */
    bcm_l2_egress_recycle_app_t recycle_app;    /* Indicates the recycle app */
    bcm_if_t dest_encap_id;    /* Destination encapsulation index */
    bcm_vrf_t vrf;    /* Virtual router instance */
} bcm_compat6521_l2_egress_t;

/* Create an l2 egress entry. */
extern int bcm_compat6521_l2_egress_create(
    int unit,
    bcm_compat6521_l2_egress_t *egr);

/* Get the config of an l2 egress entry with specified encap_id. */
extern int bcm_compat6521_l2_egress_get(
    int unit,
    bcm_if_t encap_id,
    bcm_compat6521_l2_egress_t *egr);

/* Find the encap_id with a given egress config */
extern int bcm_compat6521_l2_egress_find(
    int unit,
    bcm_compat6521_l2_egress_t *egr,
    bcm_if_t *encap_id);

/* packet trace ingress process data */
typedef struct bcm_compat6521_switch_pkt_trace_info_s {
    bcm_switch_pkt_trace_lookup_result_t pkt_trace_lookup_status; /* packet trace lookup status set */
    bcm_switch_pkt_trace_resolution_t pkt_trace_resolution; /* packet trace resolution result */
    bcm_switch_pkt_trace_hashing_info_t pkt_trace_hash_info; /* packet trace hashing information */
    bcm_stg_stp_t pkt_trace_stp_state;  /* packet trace ingress stp state */
    uint32 pkt_trace_drop_reason[BCM_SWITCH_PKT_TRACE_DROP_REASON_MAX]; /* packet trace drop reason */
    uint32 dest_pipe_num;               /* ingress pipeline number of the pkt
                                           trace packet */
    uint32 raw_data_length;             /* number of bytes returned as raw_data */
    uint8 raw_data[BCM_SWITCH_PKT_TRACE_RAW_DATA_MAX]; /* packet trace process result data in
                                           raw format */
    bcm_switch_pkt_trace_fwd_dst_info_t pkt_trace_fwd_dst_info; /* packet trace forward destination
                                           information */
} bcm_compat6521_switch_pkt_trace_info_t;

/*
 * Generate a visibility trace packet and/or read the result of the
 * packets ingress processing information
 */
extern int bcm_compat6521_switch_pkt_trace_info_get(
    int unit,
    uint32 options,
    uint8 port,
    int len,
    uint8 *data,
    bcm_compat6521_switch_pkt_trace_info_t *pkt_trace_info);
	
/* Reflector encap data */
typedef struct bcm_compat6521_switch_reflector_data_s {
    bcm_switch_reflector_type_t type;    /* reflector type */
    bcm_mac_t mc_reflector_my_mac;    /* Source MAC address stamped on the response packet. Relevant for MC reflector only. */
    uint32 error_estimate;    /* ERROR_ESTIMATE */
    bcm_if_t next_encap_id;    /* Next pointer encap id for the RCH Ethernet encapsulation */
} bcm_compat6521_switch_reflector_data_t;

/* Create/Destroy/Get encap ID for L2/L3 relfectors (both unicast and multicast) . */
extern int bcm_compat6521_switch_reflector_create(
    int unit,
    uint32 flags,
    bcm_if_t *encap_id,
    bcm_compat6521_switch_reflector_data_t *data);

/* Create/Destroy/Get encap ID for L2/L3 relfectors (both unicast and multicast) . */
extern int bcm_compat6521_switch_reflector_get(
    int unit,
    uint32 flags,
    bcm_if_t encap_id,
    bcm_compat6521_switch_reflector_data_t *data);

typedef struct  _bcm_compat6521_field_entry_config_s {
       uint32 flags;                   /* Field entry create flags
                                           BCM_FIELD_ENTRY_CREATE_XXX. */
        bcm_field_entry_t entry_id;     /* Field entry. */
        bcm_field_group_t group_id;     /* Group Id. */
        bcm_field_aset_t aset;          /* Default entry Action Set (currently
                                           applicable only for default entries). */
        int priority;                   /* Entry priority. */
        uint32 status_flags;            /* read only flag to retrieve field entry
                                           status flags like
                                           BCM_FIELD_ENTRY_STATUS_xxx. */
        uint32 flex_counter_action_id;  /*  Read only field for flex counter action
                                           identifier. */
        uint32 counter_id;              /* Read only field for flex counter index
                                           for non colored counters. */
        uint32 gp_counter_id;           /* Read only field for flex counter index
                                           for green colored counters. */
        uint32 yp_counter_id;           /* Read only field for flex counter index
                                           for yellow colored counters. */
        uint32 rp_counter_id;           /* Read only field for flex counter index
                                           for red colored counters. */
} bcm_compat6521_field_entry_config_t;

extern int bcm_compat6521_field_entry_config_create(
        int unit,
        bcm_compat6521_field_entry_config_t *entry_config);

extern int bcm_compat6521_field_entry_config_get(
        int unit,
        bcm_compat6521_field_entry_config_t *entry_config);

#endif /* BCM_RPC_SUPPORT */
#endif /* _COMPAT_6521_H_ */
