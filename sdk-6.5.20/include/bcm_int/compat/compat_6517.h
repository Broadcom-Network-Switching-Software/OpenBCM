/*
* $Id: compat_6517.h,v 1.0 2019/11/10
* This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
* 
* Copyright 2007-2020 Broadcom Inc. All rights reserved.
*
* RPC Compatibility with sdk-6.5.17 routines
*/

#ifndef _COMPAT_6517_H_
#define _COMPAT_6517_H_

#ifdef BCM_RPC_SUPPORT
#include <bcm_int/compat/compat_6519.h>
#include <bcm/types.h>
#include <bcm/vlan.h>
#if defined(INCLUDE_L3)
#include <bcm/l2gre.h>
#include <bcm/wlan.h>
#include <bcm/vxlan.h>
#endif /* defined(INCLUDE_L3) */
#include <bcm/srv6.h>
#include <bcm/oam.h>
#if defined(INCLUDE_L3)
#include <bcm/tunnel.h>
#include <bcm/l3.h>
#endif /* defined(INCLUDE_L3) */
#include <bcm/port.h>
#include <bcm/ppp.h>
#include <bcm/tunnel.h>
#include <bcm/mirror.h>
#include <bcm/field.h>
#include <bcm/switch.h>
#if defined(INCLUDE_BFD)
#include <bcm/bfd.h>
#endif /* defined(INCLUDE_BFD) */
#include <bcm/mpls.h>
#include <bcm/qos.h>
#include <bcm/pkt.h>
#include <bcm/cosq.h>
#include <bcm/flowtracker.h>
#include <bcm/cpri.h>

/* egress QoS model structure */
typedef struct bcm_compat6517_qos_egress_model_s {
    bcm_qos_egress_model_type_t egress_qos;    /* egress qos variable */
    bcm_qos_egress_model_type_t egress_ttl;    /* egress ttl */
    uint8 ecn_eligible;    /* layer ecn is eligible */
} bcm_compat6517_qos_egress_model_t;

/* bcm_mpls_egress_label_s */
typedef struct bcm_compat6517_mpls_egress_label_s {
    uint32 flags;    /* BCM_MPLS_EGRESS_LABEL_xxx. */
    bcm_mpls_label_t label;    /*  */
    int qos_map_id;    /* EXP map ID. */
    uint8 exp;    /*  */
    uint8 ttl;    /*  */
    uint8 pkt_pri;    /*  */
    uint8 pkt_cfi;    /*  */
    bcm_if_t tunnel_id;    /* Tunnel Interface. */
    bcm_if_t l3_intf_id;    /* l3 Interface ID. */
    bcm_mpls_egress_action_t action;    /* MPLS label action, relevant when BCM_MPLS_EGRESS_LABEL_ACTION_VALID is set. */
    int ecn_map_id;    /* Ecn map id for IP ECN to EXP mapping. */
    int int_cn_map_id;    /* Ecn map id for INT_CN to EXP mapping. */
    bcm_failover_t egress_failover_id;    /* Failover object index for Egress Protection. */
    bcm_if_t egress_failover_if_id;    /* Failover MPLS Tunnel identifier for Egress Protection. */
    int outlif_counting_profile;    /* Out LIF counting profile */
    bcm_mpls_special_label_push_type_t spl_label_push_type;    /* Special label push type */
    bcm_encap_access_t encap_access;    /* Encapsulation Access stage */
    bcm_compat6517_qos_egress_model_t egress_qos_model;    /* egress qos and ttl model */
} bcm_compat6517_mpls_egress_label_t;

#if defined(INCLUDE_L3)
/* Set the MPLS tunnel initiator parameters for an L3 interface. */
extern int bcm_compat6517_mpls_tunnel_initiator_set(
    int unit,
    bcm_if_t intf,
    int num_labels,
    bcm_compat6517_mpls_egress_label_t *label_array);

/* bcm_mpls_tunnel_initiator_create */
extern int bcm_compat6517_mpls_tunnel_initiator_create(
    int unit,
    bcm_if_t intf,
    int num_labels,
    bcm_compat6517_mpls_egress_label_t *label_array);

/* Get the MPLS tunnel initiator parameters from an L3 interface. */
extern int bcm_compat6517_mpls_tunnel_initiator_get(
    int unit,
    bcm_if_t intf,
    int label_max,
    bcm_compat6517_mpls_egress_label_t *label_array,
    int *label_count);

/* MPLS port type. */
typedef struct bcm_compat6517_mpls_port_s {
    bcm_gport_t mpls_port_id;    /* GPORT identifier. */
    uint32 flags;    /* BCM_MPLS_PORT_xxx. */
    uint32 flags2;    /* BCM_MPLS_PORT2_xxx. */
    int if_class;    /* Interface class ID. */
    int exp_map;    /* Incoming EXP map ID. */
    int int_pri;    /* Internal priority. */
    uint8 pkt_pri;    /* Packet priority. */
    uint8 pkt_cfi;    /* Packet CFI. */
    uint16 service_tpid;    /* Service VLAN TPID value. */
    bcm_gport_t port;    /* Match port and/or egress port. */
    bcm_mpls_port_match_t criteria;    /* Match criteria. */
    bcm_vlan_t match_vlan;    /* Outer VLAN ID to match. */
    bcm_vlan_t match_inner_vlan;    /* Inner VLAN ID to match. */
    bcm_mpls_label_t match_label;    /* VC label to match. */
    bcm_if_t egress_tunnel_if;    /* MPLS tunnel egress object. */
    bcm_compat6517_mpls_egress_label_t egress_label;    /* Outgoing VC label. */
    int mtu;    /* MPLS port MTU. */
    bcm_vlan_t egress_service_vlan;    /* Service VLAN to Add/Replace. */
    uint32 pw_seq_number;    /* Initial-value of Pseudo-wire Sequence number */
    bcm_if_t encap_id;    /* Encap Identifier. */
    bcm_failover_t ingress_failover_id;    /* Ingress Failover Object Identifier. */
    bcm_gport_t ingress_failover_port_id;    /* Ingress Failover MPLS Port Identifier. */
    bcm_failover_t failover_id;    /* Failover Object Identifier. */
    bcm_gport_t failover_port_id;    /* Failover MPLS Port Identifier. */
    bcm_policer_t policer_id;    /* Policer ID to be associated with the MPLS gport */
    bcm_multicast_t failover_mc_group;    /* MC group used for bi-cast. */
    bcm_failover_t pw_failover_id;    /* Failover Object Identifier for Redundant PW. */
    bcm_gport_t pw_failover_port_id;    /* Redundant PW port Identifier. */
    bcm_mpls_port_control_channel_type_t vccv_type;    /* Indicate VCCV Control Channel */
    bcm_switch_network_group_t network_group_id;    /* Split Horizon network group identifier */
    bcm_vlan_t match_subport_pkt_vid;    /* LLTAG VLAN ID to match. */
    bcm_gport_t tunnel_id;    /* Tunnel Id pointing to ingress LSP. */
    bcm_gport_t per_flow_queue_base;    /* Base queue of the per flow queue set. Actual queue is decided based on internal priority and qos map. */
    int qos_map_id;    /* QOS map identifier. */
    bcm_failover_t egress_failover_id;    /* Failover object index for Egress Protection */
    bcm_gport_t egress_failover_port_id;    /* Failover MPLS Port identifier for Egress Protection */
    int ecn_map_id;    /* ECN map identifier. */
    uint32 class_id;    /* Class ID */
    uint32 egress_class_id;    /* Egress Class ID */
    int inlif_counting_profile;    /* In LIF counting profile */
    bcm_compat6517_mpls_egress_label_t egress_tunnel_label;    /* A mpls tunnel to be binded with the pwe */
    int nof_service_tags;    /* Number of Service-Tags expected on Native-ETH header */
    bcm_qos_ingress_model_t ingress_qos_model;    /* ingress qos&ttl model */
    bcm_mpls_label_t context_label;    /* Ingress match - context label. */
    bcm_if_t ingress_if;    /* Ingress match - context interface. */
} bcm_compat6517_mpls_port_t;

/* Add an MPLS port to an L2 VPN. */
extern int bcm_compat6517_mpls_port_add(
    int unit,
    bcm_vpn_t vpn,
    bcm_compat6517_mpls_port_t *mpls_port);

/* Get an MPLS port from an L2 VPN. */
extern int bcm_compat6517_mpls_port_get(
    int unit,
    bcm_vpn_t vpn,
    bcm_compat6517_mpls_port_t *mpls_port);

/* Get all MPLS ports from an L2 VPN. */
extern int bcm_compat6517_mpls_port_get_all(
    int unit,
    bcm_vpn_t vpn,
    int port_max,
    bcm_compat6517_mpls_port_t *port_array,
    int *port_count);

/* MPLS tunnel initiator type. */
typedef enum bcm_mpls_tunnel_encap_type_e {
    bcmMplsTunnelEncapTypeInvalid = 0,  /* Illegal. */
    bcmMplsTunnelEncapTypeL2 = 1,       /* L2 MPLS tunnel. */
    bcmMplsTunnelEncapTypeL3 = 2,       /* L3 MPLS tunnel. */
    bcmMplsTunnelEncapTypeCount = 3     /* Must be last. */
} bcm_mpls_tunnel_encap_type_t;

/* MPLS tunnel initiator structure. */
typedef struct bcm_compat6517_mpls_tunnel_encap_s {
    bcm_gport_t tunnel_id;    /* Tunnel ID. */
    bcm_mpls_tunnel_encap_type_t tunnel_type;    /* Tunnel initiator type. */
    int num_labels;    /* Number of labels to be added. */
    bcm_compat6517_mpls_egress_label_t label_array[BCM_MPLS_EGRESS_LABEL_MAX];    /* Label info. */
} bcm_compat6517_mpls_tunnel_encap_t;

/* Create the MPLS tunnel initiator. */
extern int bcm_compat6517_mpls_tunnel_encap_create(
    int unit,
    uint32 options,
    bcm_compat6517_mpls_tunnel_encap_t *tunnel_encap);

/* Get the MPLS tunnel initiator. */
extern int bcm_compat6517_mpls_tunnel_encap_get(
    int unit,
    bcm_compat6517_mpls_tunnel_encap_t *tunnel_encap);

/* MPLS tunnel switch structure. */
typedef struct bcm_compat6517_mpls_tunnel_switch_s {
    uint32 flags;    /* BCM_MPLS_SWITCH_xxx. */
    uint32 flags2;    /* BCM_MPLS_SWITCH2_xxx. */
    bcm_mpls_label_t label;    /* Incoming label value. */
    bcm_mpls_label_t second_label;    /* Incoming second label. */
    bcm_gport_t port;    /* Incoming port. */
    bcm_mpls_switch_action_t action;    /* MPLS label action. */
    bcm_mpls_switch_action_t action_if_bos;    /* MPLS label action if BOS. */
    bcm_mpls_switch_action_t action_if_not_bos;    /* MPLS label action if not BOS. */
    bcm_multicast_t mc_group;    /* P2MP Multicast group. */
    int exp_map;    /* EXP-map ID. */
    int int_pri;    /* Internal priority. */
    bcm_policer_t policer_id;    /* Policer ID to be associated with the incoming label. */
    bcm_vpn_t vpn;    /* L3 VPN used if action is POP. */
    bcm_compat6517_mpls_egress_label_t egress_label;    /* Outgoing label information. */
    bcm_if_t egress_if;    /* Outgoing egress object. */
    bcm_if_t ingress_if;    /* Ingress Interface object. */
    int mtu;    /* MTU. */
    int qos_map_id;    /* QOS map identifier. */
    bcm_failover_t failover_id;    /* Failover Object Identifier for protected tunnel. Used for 1+1 protection also */
    bcm_gport_t tunnel_id;    /* Tunnel ID. */
    bcm_gport_t failover_tunnel_id;    /* Failover Tunnel ID. */
    bcm_if_t tunnel_if;    /* hierarchical interface, relevant for when action is BCM_MPLS_SWITCH_ACTION_POP. */
    bcm_gport_t egress_port;    /* Outgoing egress port. */
    uint16 oam_global_context_id;    /* OAM global context id passed from ingress to egress XGS chip. */
    uint32 class_id;    /* Class ID */
    int inlif_counting_profile;    /* In LIF counting profile */
    int ecn_map_id;    /* ECN map identifier */
    int tunnel_term_ecn_map_id;    /* Tunnel termination ecn map identifier */
    uint32 stat_id;    /* Object statistics id */
    int stat_pp_profile;    /* Statistics id and profile */
    bcm_qos_ingress_model_t ingress_qos_model;    /* ingress qos & ttl model */
    uint8 pkt_pri;    /* New vlan pri */
    uint8 pkt_cfi;    /* New vlan cfi */
} bcm_compat6517_mpls_tunnel_switch_t;

/* Add an MPLS label entry. */
extern int bcm_compat6517_mpls_tunnel_switch_add(
    int unit,
    bcm_compat6517_mpls_tunnel_switch_t *info);

/* Add an MPLS label entry. */
extern int bcm_compat6517_mpls_tunnel_switch_create(
    int unit,
    bcm_compat6517_mpls_tunnel_switch_t *info);

/* Delete an MPLS label entry. */
extern int bcm_compat6517_mpls_tunnel_switch_delete(
    int unit,
    bcm_compat6517_mpls_tunnel_switch_t *info);

/* Get an MPLS label entry. */
extern int bcm_compat6517_mpls_tunnel_switch_get(
    int unit,
    bcm_compat6517_mpls_tunnel_switch_t *info);
#endif /* INCLUDE_L3 */

/* OAM endpoint object.  This is equivalent to an 802.1ag Maintenance Endpoint (MEP). */
typedef struct bcm_compat6517_oam_endpoint_info_s {
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
    bcm_compat6517_mpls_egress_label_t egress_label;    /* The MPLS outgoing label information for the Local Endpoint */
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
} bcm_compat6517_oam_endpoint_info_t;

/* Create or replace an OAM endpoint object. */
extern int bcm_compat6517_oam_endpoint_create(
    int unit,
    bcm_compat6517_oam_endpoint_info_t *endpoint_info);

/* Get an OAM endpoint object. */
extern int bcm_compat6517_oam_endpoint_get(
    int unit,
    bcm_oam_endpoint_t endpoint,
    bcm_compat6517_oam_endpoint_info_t *endpoint_info);

/* Structure that is used in SRv6 SRH Base initiator APIs, for managing device behavior at SRv6 Tunnel Encapsulation */
typedef struct bcm_compat6517_srv6_srh_base_initiator_info_s {
    uint32 flags;    /* BCM_SRV6_SRH_BASE_INITIATOR_XXX */
    bcm_gport_t tunnel_id;    /* Tunnel SRv6 SRH base object ID */
    int nof_sids;    /* Number of SIDs in the SID list */
    int qos_map_id;    /* QOS map identifier */
    int ttl;    /* Tunnel header TTL */
    int dscp;    /* Tunnel header DSCP value */
    bcm_compat6517_qos_egress_model_t egress_qos_model;    /* Egress qos and ttl model */
    bcm_if_t next_encap_id;    /* Next encapsulation ID */
} bcm_compat6517_srv6_srh_base_initiator_info_t;

/* Create/Get/Delete/Traverse SRv6 SRH Base properties */
extern int bcm_compat6517_srv6_srh_base_initiator_create(
    int unit,
    bcm_compat6517_srv6_srh_base_initiator_info_t *info);

/* Create/Get/Delete/Traverse SRv6 SRH Base properties */
extern int bcm_compat6517_srv6_srh_base_initiator_get(
    int unit,
    bcm_compat6517_srv6_srh_base_initiator_info_t *info);

/* Create/Get/Delete/Traverse SRv6 SRH Base properties */
extern int bcm_compat6517_srv6_srh_base_initiator_delete(
    int unit,
    bcm_compat6517_srv6_srh_base_initiator_info_t *info);

/* Layer 2 Logical port type */
typedef struct bcm_compat6517_vlan_port_s {
    bcm_vlan_port_match_t criteria;    /* Match criteria. */
    uint32 flags;    /* BCM_VLAN_PORT_xxx. */
    bcm_vlan_t vsi;    /* Populated for bcm_vlan_port_find only */
    bcm_vlan_t match_vlan;    /* Outer VLAN ID to match. */
    bcm_vlan_t match_inner_vlan;    /* Inner VLAN ID to match. */
    int match_pcp;    /* Outer PCP ID to match. */
    bcm_tunnel_id_t match_tunnel_value;    /* Tunnel value to match. */
    bcm_port_ethertype_t match_ethertype;    /* Ethernet type value to match. */
    bcm_gport_t port;    /* Gport: local or remote Physical or logical gport. */
    uint8 pkt_pri;    /* Service tag priority. */
    uint8 pkt_cfi;    /* Service tag cfi. */
    uint16 egress_service_tpid;    /* Service VLAN TPID value. */
    bcm_vlan_t egress_vlan;    /* Egress Outer VLAN or SD-TAG VLAN ID. */
    bcm_vlan_t egress_inner_vlan;    /* Egress Inner VLAN. */
    bcm_tunnel_id_t egress_tunnel_value;    /* Egress Tunnel value. */
    bcm_if_t encap_id;    /* Encapsulation Index. */
    int qos_map_id;    /* QoS Map Index. */
    bcm_policer_t policer_id;    /* Policer ID */
    bcm_policer_t egress_policer_id;    /* Egress Policer ID */
    bcm_failover_t failover_id;    /* Failover Object Index. */
    bcm_gport_t failover_port_id;    /* Failover VLAN Port Identifier. */
    bcm_gport_t vlan_port_id;    /* GPORT identifier */
    bcm_multicast_t failover_mc_group;    /* MC group used for bi-cast. */
    bcm_failover_t ingress_failover_id;    /* 1+1 protection. */
    bcm_failover_t egress_failover_id;    /* Failover object index for Egress Protection */
    bcm_gport_t egress_failover_port_id;    /* Failover VLAN Port Identifier for Egress Protection */
    bcm_switch_network_group_t ingress_network_group_id;    /* Split Horizon ingress network group identifier */
    bcm_switch_network_group_t egress_network_group_id;    /* Split Horizon egress network group identifier */
    int inlif_counting_profile;    /* In LIF counting profile */
    int outlif_counting_profile;    /* Out LIF counting profile */
    uint32 if_class;    /* Egress Interface class ID */
    bcm_gport_t tunnel_id;    /* Pointer from Native AC to next entry in EEDB list. */
    uint32 group;    /* flush group id to be used by l2 flush mechanism */
    bcm_gport_t ingress_failover_port_id;    /* Failover VLAN Port Identifier for ingress Protection. */
    uint16 class_id;    /* Class ID of the source port */
    uint32 match_class_id;    /* Match Namespace. */
    bcm_qos_ingress_model_t ingress_qos_model;    /* ingress qos and ttl model */
    bcm_compat6517_qos_egress_model_t egress_qos_model;    /* egress qos and ttl model */
} bcm_compat6517_vlan_port_t;

/* Create a layer 2 logical port. */
extern int bcm_compat6517_vlan_port_create(
    int unit,
    bcm_compat6517_vlan_port_t *vlan_port);

/* Get/find a layer 2 logical port given the GPORT ID or match criteria. */
extern int bcm_compat6517_vlan_port_find(
    int unit,
    bcm_compat6517_vlan_port_t *vlan_port);


#if defined(INCLUDE_BFD)
/* BFD endpoint object. */
typedef struct bcm_compat6517_bfd_endpoint_info_s {
    uint32 flags;    /* Control flags. */
    bcm_bfd_endpoint_t id;    /* Endpoint identifier. */
    bcm_bfd_endpoint_t remote_id;    /* Remote endpoint identifier. */
    bcm_bfd_tunnel_type_t type;    /* Type of BFD encapsulation. */
    bcm_gport_t gport;    /* Gport identifier. */
    bcm_gport_t tx_gport;    /* TX gport associated with this endpoint. */
    bcm_gport_t remote_gport;    /* Gport identifier of trapping destination: OAMP or CPU, local or remote. */
    int bfd_period;    /* For local endpoints, this is the BFD transmission period in ms. */
    bcm_vpn_t vpn;    /* VPN. */
    uint8 vlan_pri;    /* VLAN tag priority. */
    uint8 inner_vlan_pri;    /* Inner VLAN tag priority. */
    bcm_vrf_t vrf_id;    /* Vrf identifier. */
    bcm_ip_t dst_ip_addr;    /* Destination IPv4 address. */
    bcm_ip6_t dst_ip6_addr;    /* Destination IPv6 address. */
    bcm_ip_t src_ip_addr;    /* Source IPv4 address. */
    bcm_ip6_t src_ip6_addr;    /* Source IPv6 address. */
    uint8 ip_tos;    /* IPv4 Tos / IPv6 Traffic Class. */
    uint8 ip_ttl;    /* IPv4 TTL / IPv6 Hop Count. */
    bcm_ip_t inner_dst_ip_addr;    /* Inner destination IPv4 address. */
    bcm_ip6_t inner_dst_ip6_addr;    /* Inner destination IPv6 address. */
    bcm_ip_t inner_src_ip_addr;    /* Inner source IPv4 address. */
    bcm_ip6_t inner_src_ip6_addr;    /* Inner source IPv6 address. */
    uint8 inner_ip_tos;    /* Inner IPv4 Tos / IPv6 Traffic Class. */
    uint8 inner_ip_ttl;    /* Inner IPv4 TTL / IPv6 Hop Count. */
    uint32 udp_src_port;    /* UDP source port for Ipv4, Ipv6. */
    bcm_mpls_label_t label;    /* Incoming inner label. */
    bcm_compat6517_mpls_egress_label_t egress_label;    /* The MPLS outgoing label information. */
    bcm_if_t egress_if;    /* Egress interface. */
    uint8 mep_id[BCM_BFD_ENDPOINT_MAX_MEP_ID_LENGTH];    /* MPLS-TP CC/CV TLV and Source MEP ID. */
    uint8 mep_id_length;    /* Length of MPLS-TP CC/CV TLV and MEP-ID. */
    bcm_cos_t int_pri;    /* Egress queuing for outgoing BFD to remote. */
    uint8 cpu_qid;    /* CPU queue for BFD. */
    bcm_bfd_state_t local_state;    /* Local session state. */
    uint32 local_discr;    /* Local discriminator. */
    bcm_bfd_diag_code_t local_diag;    /* Local diagnostic code. */
    uint32 local_flags;    /* Flags combination on outgoing frames. */
    uint32 local_min_tx;    /* Desired local min tx interval. */
    uint32 local_min_rx;    /* Required local rx interval. */
    uint32 local_min_echo;    /* Local minimum echo interval. */
    uint8 local_detect_mult;    /* Local detection interval multiplier. */
    bcm_bfd_auth_type_t auth;    /* Authentication type. */
    uint32 auth_index;    /* Authentication index. */
    uint32 tx_auth_seq;    /* Tx authentication sequence id. */
    uint32 rx_auth_seq;    /* Rx authentication sequence id. */
    uint32 remote_flags;    /* Remote flags. */
    bcm_bfd_state_t remote_state;    /* Remote session state. */
    uint32 remote_discr;    /* Remote discriminator. */
    bcm_bfd_diag_code_t remote_diag;    /* Remote diagnostic code. */
    uint32 remote_min_tx;    /* Desired remote min tx interval. */
    uint32 remote_min_rx;    /* Required remote rx interval. */
    uint32 remote_min_echo;    /* Remote minimum echo interval. */
    uint8 remote_detect_mult;    /* Remote detection interval multiplier. */
    int sampling_ratio;    /* 0 - No packets sampled to the CPU. 1-8 - Count of packets (with events) that need to arrive before one is sampled to the CPU. */
    uint8 loc_clear_threshold;    /* Number of packets required to reset the Loss-of-Continuity status per endpoint. */
    uint32 ip_subnet_length;    /* The subnet length for incoming packet validity check. Value 0 indicates no check is performed, positive values indicate the amount of MSBs to be compared. */
    uint8 remote_mep_id[BCM_BFD_ENDPOINT_MAX_MEP_ID_LENGTH];    /* MPLS-TP CC/CV TLV and Remote MEP ID. */
    uint8 remote_mep_id_length;    /* Length of MPLS-TP CC/CV TLV and REMOTE MEP-ID. */
    uint8 mis_conn_mep_id[BCM_BFD_ENDPOINT_MAX_MEP_ID_LENGTH];    /* MPLS-TP CC/CV TLV and Mis connectivity MEP ID. */
    uint8 mis_conn_mep_id_length;    /* Length of MPLS-TP CC/CV TLV and Mis connectivity MEP-ID. */
    uint32 bfd_detection_time;    /* Optional: BFD Detection Time, in microseconds. */
    bcm_vlan_t pkt_vlan_id;    /* TX Packet Vlan Id. */
    bcm_vlan_t rx_pkt_vlan_id;    /* RX Packet Vlan Id. */
    bcm_mpls_label_t gal_label;    /* MPLS GAL label. */
    uint32 faults;    /* Fault flags. */
    uint32 flags2;    /* Second set of control flags. */
    int ipv6_extra_data_index;    /* Pointer to first extended data entry */
    int punt_good_packet_period;    /* BFD good packets sampling period. Every punt_good_packet_period milliseconds, a single packet is punted to the CPU */
    uint8 bfd_period_cluster;    /* Tx period group. All MEPs in a group must be created with the same Tx period. Modifying the Tx period in one MEP affects the others. */
} bcm_compat6517_bfd_endpoint_info_t;

/* Create or update an BFD endpoint object */
extern int bcm_compat6517_bfd_endpoint_create(
    int unit,
    bcm_compat6517_bfd_endpoint_info_t *endpoint_info);

/* Get an BFD endpoint object. */
extern int bcm_compat6517_bfd_endpoint_get(
    int unit,
    bcm_bfd_endpoint_t endpoint,
    bcm_compat6517_bfd_endpoint_info_t *endpoint_info);


#endif /* defined(INCLUDE_BFD) */
#if defined(INCLUDE_L3)
typedef struct bcm_compat6517_l3_intf_s {
    uint32 l3a_flags;                   /* See BCM_L3_FLAGS_XXX flags
                                           definitions. */
    uint32 l3a_flags2;                  /* See BCM_L3_FLAGS2_XXX flags
                                           definitions. */
    bcm_vrf_t l3a_vrf;                  /* Virtual router instance. */
    bcm_if_t l3a_intf_id;               /* Interface ID. */
    bcm_mac_t l3a_mac_addr;             /* MAC address. */
    bcm_vlan_t l3a_vid;                 /* VLAN ID. */
    bcm_vlan_t l3a_inner_vlan;          /* Inner vlan for double tagged packets. */
    int l3a_tunnel_idx;                 /* Tunnel (initiator) index. */
    int l3a_ttl;                        /* TTL threshold. */
    int l3a_mtu;                        /* MTU. */
    int l3a_mtu_forwarding;             /* Forwarding Layer MTU. */
    bcm_if_group_t l3a_group;           /* Interface group number. */
    bcm_l3_intf_qos_t vlan_qos;         /* Outer-Vlan QoS Setting. */
    bcm_l3_intf_qos_t inner_vlan_qos;   /* Inner-Vlan QoS Setting. */
    bcm_l3_intf_qos_t dscp_qos;         /* DSCP QoS Setting. */
    int l3a_intf_class;                 /* L3 interface class ID */
    int l3a_ip4_options_profile_id;     /* IP4 Options handling Profile ID */
    int l3a_nat_realm_id;               /* Realm id of the interface for NAT */
    uint16 outer_tpid;                  /* TPID value */
    uint32 l3a_intf_flags;              /* See BCM_L3_INTF_XXX flag definitions. */
    uint8 native_routing_vlan_tags;     /* Set number of VLAN tags expected when
                                           interface is used for native routing */
    bcm_gport_t l3a_source_vp;          /* Source virtual port in overlay
                                           domain. SVP is used for deriving port
                                           properties in the egress device when
                                           packets are sent over HG flow. */
    uint32 flow_handle;                 /* FLOW handle for flex entries. */
    uint32 flow_option_handle;          /* FLOW option handle for flex entries. */
    bcm_flow_logical_field_t logical_fields[BCM_FLOW_MAX_NOF_LOGICAL_FIELDS]; /* logical fields array for flex
                                           entries. */
    uint32 num_of_fields;               /* number of logical fields. */
    bcm_qos_ingress_model_t ingress_qos_model; /* ingress qos and ttl model */
    bcm_compat6517_qos_egress_model_t egress_qos_model;    /* egress qos and ttl model */
    uint32 stat_id;                     /* Object statistics ID. */
    int stat_pp_profile;                /* Statistics profile. */
} bcm_compat6517_l3_intf_t;

/* Create a new L3 interface. */
extern int bcm_compat6517_l3_intf_create(
    int unit,
    bcm_compat6517_l3_intf_t *intf);

/* Delete an L3 interface */
extern int bcm_compat6517_l3_intf_delete(
    int unit,
    bcm_compat6517_l3_intf_t *intf);

/* Search for L3 interface by MAC address and VLAN. */
extern int bcm_compat6517_l3_intf_find(
    int unit,
    bcm_compat6517_l3_intf_t *intf);

/* Search for L3 interface by VLAN only. */
extern int bcm_compat6517_l3_intf_find_vlan(
    int unit,
    bcm_compat6517_l3_intf_t *intf);

/* Given the L3 interface number, return the interface information. */
extern int bcm_compat6517_l3_intf_get(
    int unit,
    bcm_compat6517_l3_intf_t *intf);


/* L3 tunneling initiator. */
typedef struct bcm_compat6517_tunnel_initiator_s {
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
    bcm_mac_t smac;    /* WLAN outer MAC */
    int mtu;    /* WLAN MTU */
    bcm_vlan_t vlan;    /* Tunnel VLAN */
    uint16 tpid;    /* Tunnel TPID */
    uint8 pkt_pri;    /* Tunnel priority */
    uint8 pkt_cfi;    /* Tunnel CFI */
    uint16 ip4_id;    /* IPv4 ID. */
    bcm_if_t l3_intf_id;    /* l3 Interface ID. */
    uint16 span_id;    /* Erspan Span ID. */
    uint32 aux_data;    /* Tunnel associated data. */
    int outlif_counting_profile;    /* Out LIF counting profile */
    bcm_encap_access_t encap_access;    /* Encapsulation Access stage */
    uint8 hw_id;    /* Unique identifier of an ERSPAN engine within a system */
    uint16 switch_id;    /* Identifies a source switch at the receiving end */
    uint16 class_id;    /* Class ID of the RSPAN advanced tunnel */
    bcm_compat6517_qos_egress_model_t egress_qos_model;    /* egress qos and ttl model */
    int qos_map_id;    /* general remark profile */
    uint8 ecn;    /* Tunnel header ECN value. */
    bcm_tunnel_flow_label_select_t flow_label_sel;    /* Tunnel header flow label select. */
    bcm_tunnel_dscp_ecn_select_t dscp_ecn_sel;    /* Tunnel header DSCP and ECN select. */
    int dscp_ecn_map;    /* DSCP and ECN map ID. */
} bcm_compat6517_tunnel_initiator_t;

/* Set the tunneling initiator parameters on an L3 interface. */
extern int bcm_compat6517_tunnel_initiator_set(
    int unit,
    bcm_compat6517_l3_intf_t *intf,
    bcm_compat6517_tunnel_initiator_t *tunnel);

/* Set the tunneling initiator parameters on an L3 interface. */
extern int bcm_compat6517_tunnel_initiator_create(
    int unit,
    bcm_compat6517_l3_intf_t *intf,
    bcm_compat6517_tunnel_initiator_t *tunnel);

/* Delete the tunnel association for the given L3 interface. */
extern int bcm_compat6517_tunnel_initiator_clear(
    int unit,
    bcm_compat6517_l3_intf_t *intf);

/* Get the tunnel property for the given L3 interface. */
extern int bcm_compat6517_tunnel_initiator_get(
    int unit,
    bcm_compat6517_l3_intf_t *intf,
    bcm_compat6517_tunnel_initiator_t *tunnel);

/* Create VXLAN Tunnel Initiator.  */
extern int bcm_compat6517_vxlan_tunnel_initiator_create(
    int unit,
    bcm_compat6517_tunnel_initiator_t *info);

/* Get  VXLAN Tunnel Initiator.  */
extern int bcm_compat6517_vxlan_tunnel_initiator_get(
    int unit,
    bcm_compat6517_tunnel_initiator_t *info);

/* Set the tunneling initiator parameters for a WLAN tunnel. */
extern int bcm_compat6517_wlan_tunnel_initiator_create(
    int unit,
    bcm_compat6517_tunnel_initiator_t *info);

/* Get the tunnel properties for an outgoing WLAN tunnel. */
extern int bcm_compat6517_wlan_tunnel_initiator_get(
    int unit,
    bcm_compat6517_tunnel_initiator_t *info);

/* Create L2GRE Tunnel Initiator.  */
extern int bcm_compat6517_l2gre_tunnel_initiator_create(
    int unit,
    bcm_compat6517_tunnel_initiator_t *info);

/* Get  L2GRE Tunnel Initiator.  */
extern int bcm_compat6517_l2gre_tunnel_initiator_get(
    int unit,
    bcm_compat6517_tunnel_initiator_t *info);

typedef struct bcm_compat6517_l3_ingress_s {
    uint32 flags;                       /* Interface flags. */
    bcm_vrf_t vrf;                      /* Virtual router id. */
    bcm_l3_ingress_urpf_mode_t urpf_mode; /* URPF mode setting for L3 Ingress
                                           Interface. */
    int intf_class;                     /* Classification class ID. */
    bcm_vlan_t ipmc_intf_id;            /* IPMC L2 distribution Vlan. */
    int qos_map_id;                     /* QoS DSCP priority map. */
    int ip4_options_profile_id;         /* IP4 Options handling Profile ID */
    int nat_realm_id;                   /* Realm id of the interface for NAT */
    int tunnel_term_ecn_map_id;         /* Tunnel termination ecn map id */
    uint32 intf_class_route_disable;    /* routing enablers bit map in rif
                                           profile */
    int oam_default_profile;            /* OAM profile for RIF */
    uint32 hash_layers_disable;         /* load balancing disable hash layers
                                           bit map using the
                                           BCM_SWITCH_HASH_LAYER_XXX_DISABLE
                                           flags */
} bcm_compat6517_l3_ingress_t;

extern int bcm_compat6517_l3_ingress_create(
    int unit,
    bcm_compat6517_l3_ingress_t *ing_intf,
    bcm_if_t *intf_id);

extern int bcm_compat6517_l3_ingress_get(
    int unit,
    bcm_if_t intf_id,
    bcm_compat6517_l3_ingress_t *ing_intf);

extern int bcm_compat6517_l3_ingress_find(
    int unit,
    bcm_compat6517_l3_ingress_t *ing_intf,
    bcm_if_t *intf_id);

/*
 * L3 Egress Structure.
 *
 * Description of an L3 forwarding destination.
 */
typedef struct bcm_compat6517_l3_egress_s {
    uint32 flags;                       /* Interface flags (BCM_L3_TGID,
                                           BCM_L3_L2TOCPU). */
    uint32 flags2;                      /* See BCM_L3_FLAGS2_xxx flag
                                           definitions. */
    bcm_if_t intf;                      /* L3 interface (source MAC, tunnel). */
    bcm_mac_t mac_addr;                 /* Next hop forwarding destination mac. */
    bcm_vlan_t vlan;                    /* Next hop vlan id. */
    bcm_module_t module;
    bcm_port_t port;                    /* Port packet switched to (if
                                           !BCM_L3_TGID). */
    bcm_trunk_t trunk;                  /* Trunk packet switched to (if
                                           BCM_L3_TGID). */
    uint32 mpls_flags;                  /* BCM_MPLS flag definitions. */
    bcm_mpls_label_t mpls_label;        /* MPLS label. */
    bcm_mpls_egress_action_t mpls_action; /* MPLS action. */
    int mpls_qos_map_id;                /* MPLS EXP map ID. */
    int mpls_ttl;                       /* MPLS TTL threshold. */
    uint8 mpls_pkt_pri;                 /* MPLS Packet Priority Value. */
    uint8 mpls_pkt_cfi;                 /* MPLS Packet CFI Value. */
    uint8 mpls_exp;                     /* MPLS Exp. */
    int qos_map_id;                     /* General QOS map id */
    bcm_if_t encap_id;                  /* Encapsulation index. */
    bcm_failover_t failover_id;         /* Failover Object Index. */
    bcm_if_t failover_if_id;            /* Failover Egress Object index. */
    bcm_multicast_t failover_mc_group;  /* Failover Multicast Group. */
    int dynamic_scaling_factor;         /* Scaling factor for dynamic load
                                           balancing thresholds. */
    int dynamic_load_weight;            /* Weight of traffic load in determining
                                           a dynamic load balancing member's
                                           quality. */
    int dynamic_queue_size_weight;      /* Weight of queue size in determining a
                                           dynamic load balancing member's
                                           quality. */
    int intf_class;                     /* L3 interface class ID */
    uint32 multicast_flags;             /* BCM_L3_MULTICAST flag definitions. */
    uint16 oam_global_context_id;       /* OAM global context id passed from
                                           ingress to egress XGS chip. */
    bcm_vntag_t vntag;                  /* VNTAG. */
    bcm_vntag_action_t vntag_action;    /* VNTAG action. */
    bcm_etag_t etag;                    /* ETAG. */
    bcm_etag_action_t etag_action;      /* ETAG action. */
    uint32 flow_handle;                 /* FLOW handle for flex entries. */
    uint32 flow_option_handle;          /* FLOW option handle for flex entries. */
    uint32 flow_label_option_handle;    /* FLOW option handle for egress label
                                           flex entries. */
    bcm_flow_logical_field_t logical_fields[BCM_FLOW_MAX_NOF_LOGICAL_FIELDS]; /* logical fields array for flex
                                           entries. */
    uint32 num_of_fields;               /* number of logical fields. */
    int counting_profile;               /* counting profile. If not required,
                                           set it to
                                           BCM_STAT_LIF_COUNTING_PROFILE_NONE */
    int mpls_ecn_map_id;                /* IP ECN/INT CN to MPLS EXP map ID. */
    bcm_l3_ingress_urpf_mode_t urpf_mode; /* fec rpf mode. */
    bcm_multicast_t mc_group;           /* Multicast Group. */
    bcm_mac_t src_mac_addr;             /* Source MAC Address. */
    bcm_gport_t hierarchical_gport;     /* Hierarchical TM-Flow. */
    uint32 stat_id;                     /* Object statistics ID. */
    int stat_pp_profile;                /* Statistics profile. */
    bcm_gport_t vlan_port_id;           /* Pointer to egress vlan translation
                                           lif entry in EEDB. */
    int replication_id;                 /* Replication copy ID of multicast
                                           Egress object. */
    int mtu;                            /* MTU. */
} bcm_compat6517_l3_egress_t;

extern int bcm_compat6517_l3_egress_create(
    int unit,
    uint32 flags,
    bcm_compat6517_l3_egress_t *egr,
    bcm_if_t *if_id);

extern int bcm_compat6517_l3_egress_get(
    int unit,
    bcm_if_t intf,
    bcm_compat6517_l3_egress_t *egr);

extern int bcm_compat6517_l3_egress_find(
    int unit,
    bcm_compat6517_l3_egress_t *egr,
    bcm_if_t *intf);

extern int bcm_compat6517_l3_egress_allocation_get(
    int unit,
    uint32 flags,
    bcm_compat6517_l3_egress_t *egr,
    uint32 nof_members,
    bcm_if_t *if_id);

extern int bcm_compat6517_failover_egress_set(
    int unit,
    bcm_if_t intf,
    bcm_compat6517_l3_egress_t *failover_egr);

extern int bcm_compat6517_failover_egress_get(
    int unit,
    bcm_if_t intf,
    bcm_compat6517_l3_egress_t *failover_egr);

/* L3 ECMP member structure */
typedef struct bcm_compat6517_l3_ecmp_member_s {
    uint32 flags;                   /* Member flag. */
    bcm_if_t egress_if;             /* L3 interface ID pointing to Egress
                                       Forwarding Object. */
    bcm_failover_t failover_id;     /* Failover Object Identifier. */
    bcm_if_t failover_egress_if;    /* Failover Egress L3 Interface ID. */
    int status;                     /* Member status. */
    int weight;                     /* Member weight value. */
} bcm_compat6517_l3_ecmp_member_t;

/* Create an Egress ECMP forwarding object. */
extern int bcm_compat6517_l3_ecmp_create(
    int unit,
    uint32 options,
    bcm_l3_egress_ecmp_t *ecmp_info,
    int ecmp_member_count,
    bcm_compat6517_l3_ecmp_member_t *ecmp_member_array);

/* Get info about an Egress ECMP forwarding object. */
extern int bcm_compat6517_l3_ecmp_get(
    int unit,
    bcm_l3_egress_ecmp_t *ecmp_info,
    int ecmp_member_size,
    bcm_compat6517_l3_ecmp_member_t *ecmp_member_array,
    int *ecmp_member_count);

/* Add a member to an Egress ECMP forwarding object. */
extern int bcm_compat6517_l3_ecmp_member_add(
    int unit,
    bcm_if_t ecmp_group_id,
    bcm_compat6517_l3_ecmp_member_t *ecmp_member);

/* Delete a member from an Egress ECMP forwarding object. */
extern int bcm_compat6517_l3_ecmp_member_delete(
    int unit,
    bcm_if_t ecmp_group_id,
    bcm_compat6517_l3_ecmp_member_t *ecmp_member);

/* Find an Egress ECMP forwarding object. */
extern int bcm_compat6517_l3_ecmp_find(
    int unit,
    int ecmp_member_count,
    bcm_compat6517_l3_ecmp_member_t *ecmp_member_array,
    bcm_l3_egress_ecmp_t *ecmp_info);

#endif /* INCLUDE_L3 */

typedef struct bcm_compat6517_port_config_s {
    bcm_pbmp_t fe;                      /* Mask of FE ports. */
    bcm_pbmp_t ge;                      /* Mask of GE ports. */
    bcm_pbmp_t xe;                      /* Mask of 10gig ports. */
    bcm_pbmp_t ce;                      /* Mask of 100gig ports. */
    bcm_pbmp_t le;                      /* Mask of 50gig ports. */
    bcm_pbmp_t cc;                      /* Mask of 200gig ports. */
    bcm_pbmp_t cd;                      /* Mask of 400gig ports. */
    bcm_pbmp_t e;                       /* Mask of eth ports. */
    bcm_pbmp_t hg;                      /* Mask of HiGig ports. */
    bcm_pbmp_t sci;                     /* Mask of SCI ports. */
    bcm_pbmp_t sfi;                     /* Mask of SFI ports. */
    bcm_pbmp_t spi;                     /* Mask of SPI ports. */
    bcm_pbmp_t spi_subport;             /* Mask of SPI subports. */
    bcm_pbmp_t port;                    /* Mask of all front panel ports. */
    bcm_pbmp_t cpu;                     /* Mask of CPU ports. */
    bcm_pbmp_t all;                     /* Mask of all ports. */
    bcm_pbmp_t stack_int;               /* Deprecated - unused. */
    bcm_pbmp_t stack_ext;               /* Mask of Stack ports. */
    bcm_pbmp_t tdm;                     /* Mask of TDM ports. */
    bcm_pbmp_t pon;                     /* Mask of PON ports. */
    bcm_pbmp_t llid;                    /* Mask of LLID ports. */
    bcm_pbmp_t il;                      /* Mask of ILKN ports. */
    bcm_pbmp_t xl;                      /* Mask of XLAUI ports. */
    bcm_pbmp_t rcy;                     /* Mask of RECYCLE ports. */
    bcm_pbmp_t sat;                     /* Mask of SAT ports. */
    bcm_pbmp_t ipsec;                   /* Mask of IPSEC ports. */
    bcm_pbmp_t per_pipe[BCM_PIPES_MAX]; /* Mask of ports per pipe. The number of
                                           pipes per device can be obtained via
                                           num_pipes field of bcm_info_t. */
    bcm_pbmp_t nif;                     /* Mask of Network Interfaces ports. */
    bcm_pbmp_t control;                 /* Mask of hot swap controlling ports. */
    bcm_pbmp_t eventor;                 /* Mask of Eventor ports. */
    bcm_pbmp_t olp;                     /* Mask of OLP ports. */
    bcm_pbmp_t oamp;                    /* Mask of OAMP ports. */
    bcm_pbmp_t erp;                     /* Mask of ERP ports. */
    bcm_pbmp_t roe;                     /* Mask of ROE ports. */
    bcm_pbmp_t rcy_mirror;              /* Mask of mirror recycle ports (sniff) */
} bcm_compat6517_port_config_t;

extern int bcm_compat6517_port_config_get(
    int unit,
    bcm_compat6517_port_config_t *config);

/* Mirror source structure. */
typedef struct bcm_compat6517_mirror_source_s {
    bcm_mirror_source_type_t type;      /* Mirror source type. */
    uint32 flags;                       /* Mirror source flags only with port
                                           source type. */
    bcm_port_t port;                    /* Port/gport id. */
    bcm_pkt_trace_event_t trace_event;  /* Trace event. */
    bcm_pkt_drop_event_t drop_event;    /* Drop event. */
} bcm_compat6517_mirror_source_t;

/* Add a mirror destination to a mirror source. */
extern int bcm_compat6517_mirror_source_dest_add(
    int unit,
    bcm_compat6517_mirror_source_t *source,
    bcm_gport_t mirror_dest_id);

/* Delete a mirror destination from a mirror source. */
extern int bcm_compat6517_mirror_source_dest_delete(
    int unit,
    bcm_compat6517_mirror_source_t *source,
    bcm_gport_t mirror_dest_id);

/* Delete all mirror destinations applied on a mirror source. */
extern int bcm_compat6517_mirror_source_dest_delete_all(
    int unit,
    bcm_compat6517_mirror_source_t *source);

/* Get all mirror destinations applied on a mirror source. */
extern int bcm_compat6517_mirror_source_dest_get_all(
    int unit,
    bcm_compat6517_mirror_source_t *source,
    int array_size,
    bcm_gport_t *mirror_dest,
    int *count);

/* Field name. */
typedef struct bcm_compat6517_field_presel_config_s {
    uint8 name[BCM_FIELD_MAX_NAME_LEN];
} bcm_compat6517_field_presel_config_t;

/* Map a presel id to a name */
extern int bcm_compat6517_field_presel_config_set(
    int unit,
    bcm_field_presel_t presel_id,
    bcm_compat6517_field_presel_config_t *presel_config);

/* Get name from presel id */
extern int bcm_compat6517_field_presel_config_get(
    int unit,
    bcm_field_presel_t presel_id,
    bcm_compat6517_field_presel_config_t *presel_config);

/* Vlan block structure. */
typedef struct bcm_compat6517_vlan_block_s {
    bcm_pbmp_t known_multicast;
    bcm_pbmp_t unknown_multicast;
    bcm_pbmp_t unknown_unicast;
    bcm_pbmp_t broadcast;
} bcm_compat6517_vlan_block_t;

/* Get VLAN block properties. */
extern int bcm_compat6517_vlan_block_get(
    int unit,
    bcm_vlan_t vlan,
    bcm_compat6517_vlan_block_t *vlan_block);

/* Set VLAN block properties. */
extern int bcm_compat6517_vlan_block_set(
    int unit,
    bcm_vlan_t vlan,
    bcm_compat6517_vlan_block_t *vlan_block);

/* Packet protocol match structure. */
typedef struct bcm_compat6517_switch_pkt_protocol_match_s {
    uint8 l2_iif_opaque_ctrl_id;        /* L2 iif based control id. */
    uint8 l2_iif_opaque_ctrl_id_mask;   /* L2 iif based control id mask. */
    bcm_vlan_t vfi;                     /* Virtual forwarding instance. */
    bcm_vlan_t vfi_mask;                /* VFI mask. */
    bcm_mac_t macda;                    /* Destination MAC address. */
    bcm_mac_t macda_mask;               /* Destination MAC address mask. */
    uint16 ethertype;                   /* Ethernet type. */
    uint16 ethertype_mask;              /* Ethernet type mask. */
    uint16 arp_rarp_opcode;             /* ARP/RARP packet operation code. */
    uint16 arp_rarp_opcode_mask;        /* ARP/RARP packet operation code mask. */
    uint8 icmp_type;                    /* ICMP type. */
    uint8 icmp_type_mask;               /* ICMP type mask. */
    uint8 igmp_type;                    /* IGMP type. */
    uint8 igmp_type_mask;               /* IGMP type mask. */
    uint8 l4_valid;                     /* If set, layer 4 header present. */
    uint8 l4_valid_mask;                /* L4_valid mask. */
    uint16 l4_src_port;                 /* Layer 4 source port. */
    uint16 l4_src_port_mask;            /* Layer 4 source port mask. */
    uint16 l4_dst_port;                 /* Layer 4 destination port. */
    uint16 l4_dst_port_mask;            /* Layer 4 destination port mask. */
    uint16 l5_bytes_0_1;                /* First 2 bytes of layer 5 header. */
    uint16 l5_bytes_0_1_mask;           /* L5_bytes_0_1 mask. */
    uint16 outer_l5_bytes_0_1;          /* Outer l5 bytes 0 1. */
    uint16 outer_l5_bytes_0_1_mask;     /* Outer l5 bytes 0 1 mask. */
    uint8 ip_last_protocol;             /* Protocol from IP extension header if
                                           present, otherwise from IP header. */
    uint8 ip_last_protocol_mask;        /* Ip_last_protocol mask. */
    uint8 fixed_hve_result_set_1;       /* Fixed HVE result 1. */
    uint8 fixed_hve_result_set_1_mask;  /* Fixed HVE result 1 mask. */
    uint8 fixed_hve_result_set_5;       /* Fixed HVE result 5. */
    uint8 fixed_hve_result_set_5_mask;  /* Fixed HVE result 5 mask. */
    uint8 flex_hve_result_set_1;        /* Flex HVE result 1. */
    uint8 flex_hve_result_set_1_mask;   /* Flex HVE result 1 mask. */
    uint8 tunnel_processing_results_1;  /* Tunnel processing results 1. */
    uint8 tunnel_processing_results_1_mask; /* Tunnel processing results 1 mask. */
} bcm_compat6517_switch_pkt_protocol_match_t;

/* An API to add a protocol control entry. */
extern int bcm_compat6517_switch_pkt_protocol_control_add(
    int unit,
    uint32 options,
    bcm_compat6517_switch_pkt_protocol_match_t *match,
    bcm_switch_pkt_control_action_t *action,
    int priority);

/* An API to get action and priority for a protocol control entry. */
extern int bcm_compat6517_switch_pkt_protocol_control_get(
    int unit,
    bcm_compat6517_switch_pkt_protocol_match_t *match,
    bcm_switch_pkt_control_action_t *action,
    int *priority);

/* An API to delete a protocol control entry. */
extern int bcm_compat6517_switch_pkt_protocol_control_delete(
    int unit,
    bcm_compat6517_switch_pkt_protocol_match_t *match);

/* Internals */
typedef struct bcm_compat6517_pkt_dnx_internal_s {
    uint32 trap_qualifier;    /* Trap Qualifier */
    uint32 trap_id;    /* Trap Id */
} bcm_compat6517_pkt_dnx_internal_t;

/* Fabric header updates of mirrored packets */
typedef struct bcm_compat6517_mirror_pkt_header_fabric_s {
    uint32 ftmh_valid;    /* Used to specify which fields to use for header changes. Possible values will be named BCM_MIRROR_PKT_HEADER_FABRIC_UPDATE_FTMH_ */
    uint32 internal_valid;    /* Used to specify which fields to use for header changes. Possible values will be named BCM_MIRROR_PKT_HEADER_FABRIC_UPDATE_INTERNAL_ */
    uint32 oam_ts_valid;    /* Used to specify which fields to use for header changes. Possible values will be named BCM_MIRROR_PKT_HEADER_FABRIC_UPDATE_OAM_TS_ */
    bcm_pkt_dnx_ftmh_t ftmh;    /* FTMH header fields */
    bcm_compat6517_pkt_dnx_internal_t internal;    /* Internal header fields */
    bcm_pkt_dnx_otsh_t oam_ts;    /* OAM-TS header fields */
} bcm_compat6517_mirror_pkt_header_fabric_t;

/*   represents header updates of mirrored packets  */
typedef struct bcm_compat6517_mirror_pkt_header_updates_s {
    uint32 valid;    /* Used to specify which fields to use for header changes. Possible values will be named BCM_MIRROR_PKT_HEADER_UPDATE_* */
    bcm_color_t color;    /* drop precedence */
    uint8 prio;    /* the internal packet priority (traffic class before ingress mapping to cosq) */
    uint8 ecn_value;    /* ECN capable and congestion encoding */
    uint8 cnm_cancel;    /* Ignore Congestion Point (CNM) */
    uint32 trunk_hash_result;    /* LAG load balancing key */
    bcm_gport_t in_port;    /* 8b should be exposed? */
    uint16 vsq;    /*  selects STF (statistics flow) VSQ */
    uint16 fabric_header_editing;    /* changes to the fabric headers */
    bcm_compat6517_mirror_pkt_header_fabric_t header_fabric;    /* Fabric header updates of mirrored packets */
    uint32 metadata;    /* Metadata for counting */
} bcm_compat6517_mirror_pkt_header_updates_t;

/*
 * Mirror destination Structure
 *
 * Contains information required for manipulating mirror destinations.
 */
typedef struct bcm_compat6517_mirror_destination_s {
    bcm_gport_t mirror_dest_id;    /* Unique mirror destination and encapsulation identifier. */
    uint32 flags;    /* See BCM_MIRROR_DEST_xxx flag definitions. */
    bcm_gport_t gport;    /* Mirror destination. */
    uint8 version;    /* IP header version. */
    uint8 tos;    /* Traffic Class/Tos byte. */
    uint8 ttl;    /* Hop limit. */
    bcm_ip_t src_addr;    /* Tunnel source ip address (IPv4). */
    bcm_ip_t dst_addr;    /* Tunnel destination ip address (IPv4). */
    bcm_ip6_t src6_addr;    /* Tunnel source ip address (IPv6). */
    bcm_ip6_t dst6_addr;    /* Tunnel destination ip address (IPv6). */
    uint32 flow_label;    /* IPv6 header flow label field. */
    bcm_mac_t src_mac;    /* L2 source mac address. */
    bcm_mac_t dst_mac;    /* L2 destination mac address. */
    uint16 tpid;    /* L2 header outer TPID. */
    bcm_vlan_t vlan_id;    /* Vlan id. */
    bcm_trill_name_t trill_src_name;    /* TRILL source bridge nickname */
    bcm_trill_name_t trill_dst_name;    /* TRILL destination bridge nickname */
    int trill_hopcount;    /* TRILL hop count */
    uint16 niv_src_vif;    /* Source Virtual Interface of NIV tag */
    uint16 niv_dst_vif;    /* Destination Virtual Interface of NIV tag */
    uint32 niv_flags;    /* NIV flags BCM_MIRROR_NIV_XXX */
    uint16 gre_protocol;    /* L3 GRE header protocol */
    bcm_policer_t policer_id;    /* policer_id */
    int stat_id;    /* stat_id */
    int stat_id2;    /* stat_id2 for second counter pointer */
    bcm_if_t encap_id;    /* Encapsulation index */
    bcm_if_t tunnel_id;    /* IP tunnel for encapsulation. Valid only if BCM_MIRROR_DEST_TUNNEL_IP_GRE is set */
    uint16 span_id;    /* SPAN-ID. Valid only if BCM_MIRROR_DEST_TUNNEL_WITH_SPAN_ID is set */
    uint8 pkt_prio;    /* L2 header PCP */
    uint32 sample_rate_dividend;    /* The probability of mirroring a packet is: sample_rate_dividend >= sample_rate_divisor ? 1 : sample_rate_dividend / sample_rate_divisor */
    uint32 sample_rate_divisor;    /*  */
    uint8 int_pri;    /* Internal Priority */
    uint16 etag_src_vid;    /* Extended (source) port vlan id */
    uint16 etag_dst_vid;    /* Extended (destination) port vlan id  */
    uint16 udp_src_port;    /* UDP source port */
    uint16 udp_dst_port;    /* UDP destination port */
    uint32 egress_sample_rate_dividend;    /* The probability of outbound mirroring a packet from the destination is sample_rate_dividend >= sample_rate_divisor ? 1 : sample_rate_dividend / sample_rate_divisor */
    uint32 egress_sample_rate_divisor;    /*  */
    uint8 recycle_context;    /* recycle context of egress originating packets */
    uint16 packet_copy_size;    /*  If non zero, the copied packet will be truncated to the first packet_copy_size . Current supported values for DNX are 0, 64, 128, 192 */
    uint16 egress_packet_copy_size;    /* If non zero and the packet is copied from the egress, the packet will be truncated to the first packet_copy_size . Current supported values for DNX are 0, 256. */
    bcm_compat6517_mirror_pkt_header_updates_t packet_control_updates;    /*  */
    bcm_mirror_psc_t rtag;    /* specify RTAG HASH algorithm used for shared-id mirror destination */
    uint8 df;    /* Set the do not fragment bit of IP header in mirror encapsulation */
    uint8 truncate;    /* Setting truncate would result in mirroring a truncated frame  */
    uint16 template_id;    /* Template ID for IPFIX HDR */
    uint32 observation_domain;    /* Observation domain for IPFIX HDR */
    uint32 is_recycle_strict_priority;    /* Recycle priority (1-lossless, 0-high) */
    int ext_stat_id[BCM_MIRROR_EXT_STAT_ID_COUNT];    /* ext_stat_id to support statistic interface for mirror packets. */
    uint32 flags2;    /* See BCM_MIRROR_DEST_FLAGS2_xxx flag definitions. */
    uint32 vni;    /* Virtual Network Interface ID. */
    uint32 gre_seq_number;    /* Sequence number value used in GRE header. If no value is provided, gre_seq_number will start with 0. Valid only if BCM_MIRROR_DEST_TUNNEL_IP_GRE is set. */
    bcm_mirror_pkt_erspan_encap_t erspan_header;    /* ERSPAN encapsulation header fields. Valid only if only BCM_MIRROR_DEST_TUNNEL_IP_GRE is set. */
    uint32 initial_seq_number;    /* Starting seq number in SFLOW or PSAMP header */
    bcm_mirror_psamp_fmt2_meta_data_t meta_data_type;    /* Type of item in 'meta_data' */
    uint16 meta_data;    /* Actual value of class id or meta data, based on 'meta_data_type' */
    int ext_stat_valid;    /* Validates the ext_stat. */
    int ext_stat_type_id[BCM_MIRROR_EXT_STAT_ID_COUNT];    /* Type ids for statistic interface. */
    uint16 ipfix_version;    /* IPFIX version number in PSAMP encapsulation. */
    uint16 psamp_epoch;    /* PSAMP epoch field. */
    int cosq;    /* Queue id that mirror copy is enqueued into. */
} bcm_compat6517_mirror_destination_t;

/* Create a mirror (destination, encapsulation) pair. */
extern int bcm_compat6517_mirror_destination_create(
    int unit,
    bcm_compat6517_mirror_destination_t *mirror_dest);

/* Get  mirror (destination, encapsulation) pair. */
extern int bcm_compat6517_mirror_destination_get(
    int unit,
    bcm_gport_t mirror_dest_id,
    bcm_compat6517_mirror_destination_t *mirror_dest);

/* PP header information for mirrored packet in DNX devices */
typedef struct bcm_compat6517_mirror_pkt_dnx_pp_header_s {
    uint8 tail_edit_profile;    /* Tail edit profile, used at ETPP for tail editing */
    bcm_gport_t out_vport_ext[3];    /* Extended outlif(s) */
} bcm_compat6517_mirror_pkt_dnx_pp_header_t;

/* Mirror packet system header information for DNX devices */
typedef struct bcm_compat6517_mirror_header_info_s {
    bcm_mirror_pkt_dnx_ftmh_header_t tm;    /* TM header */
    bcm_compat6517_mirror_pkt_dnx_pp_header_t pp;    /* PP header */
    bcm_pkt_dnx_udh_t udh[4];    /* UDH header */
} bcm_compat6517_mirror_header_info_t;

/* Set system header information */
extern int bcm_compat6517_mirror_header_info_set(
    int unit,
    uint32 flags,
    bcm_gport_t mirror_dest_id,
    bcm_compat6517_mirror_header_info_t *mirror_header_info);

/* Get system header information */
extern int bcm_compat6517_mirror_header_info_get(
    int unit,
    bcm_gport_t mirror_dest_id,
    uint32 *flags,
    bcm_compat6517_mirror_header_info_t *mirror_header_info);

/* Parameters info for Field Action. */
typedef struct bcm_compat6517_field_action_params_s {
    uint32 param0;    /* First Parameter to Action */
    uint32 param1;    /* Second Parameter to Action */
    bcm_pbmp_t pbmp;    /* Port bitmap */
} bcm_compat6517_field_action_params_t;

/* Add an action to a field entry */
extern int bcm_compat6517_field_action_config_info_add(
    int unit,
    bcm_field_entry_t entry,
    bcm_field_action_t action,
    bcm_compat6517_field_action_params_t *params,
    bcm_field_action_match_config_t *match_config);

/* Retrieve the parameters for an action previously added to a field entry */
extern int bcm_compat6517_field_action_config_info_get(
    int unit,
    bcm_field_entry_t entry,
    bcm_field_action_t action,
    bcm_compat6517_field_action_params_t *params,
    bcm_field_action_match_config_t *match_config);

#if defined(INCLUDE_L3)

/* L3 tunneling terminator. */
typedef struct bcm_compat6517_tunnel_terminator_s {
    uint32 flags;                       /* Configuration flags. */
    uint32 multicast_flag;              /* VXLAN Multicast trigger flags. */
    bcm_vrf_t vrf;                      /* Virtual router instance. */
    bcm_ip_t sip;                       /* SIP for tunnel header match. */
    bcm_ip_t dip;                       /* DIP for tunnel header match. */
    bcm_ip_t sip_mask;                  /* Source IP mask. */
    bcm_ip_t dip_mask;                  /* Destination IP mask. */
    bcm_ip6_t sip6;                     /* SIP for tunnel header match (IPv6). */
    bcm_ip6_t dip6;                     /* DIP for tunnel header match (IPv6). */
    bcm_ip6_t sip6_mask;                /* Source IP mask (IPv6). */
    bcm_ip6_t dip6_mask;                /* Destination IP mask (IPv6). */
    uint32 udp_dst_port;                /* UDP dst port for UDP packets. */
    uint32 udp_src_port;                /* UDP src port for UDP packets */
    bcm_tunnel_type_t type;             /* Tunnel type */
    bcm_pbmp_t pbmp;                    /* Port bitmap for this tunnel */
    bcm_vlan_t vlan;                    /* The VLAN ID or L3 ingress interface
                                           ID for IPMC lookup or WLAN tunnel */
    bcm_vlan_t fwd_vlan;                /* VLAN ID for forwarding terminated IP
                                           Tunnel */
    bcm_gport_t remote_port;            /* Remote termination */
    bcm_gport_t tunnel_id;              /* Tunnel id */
    int if_class;                       /* L3 interface class this tunnel. */
    bcm_multicast_t failover_mc_group;  /* MC group used for bi-cast. */
    bcm_failover_t ingress_failover_id; /* 1+1 protection. */
    bcm_failover_t failover_id;         /* Failover Object Identifier for
                                           protected tunnel. */
    bcm_gport_t failover_tunnel_id;     /* Failover Tunnel ID. */
    bcm_if_t tunnel_if;                 /* hierarchical interface. */
    int tunnel_class;                   /* Tunnel class id for VFP match. */
    int qos_map_id;                     /* QoS DSCP map this tunnel. */
    int inlif_counting_profile;         /* In LIF counting profile */
    int tunnel_term_ecn_map_id;         /* Tunnel termination ecn map id. */
    bcm_qos_ingress_model_t ingress_qos_model; /* ingress qos and ttl model */
    int priority;                       /* Entry priority. */

} bcm_compat6517_tunnel_terminator_t;


extern int bcm_compat6517_l2gre_tunnel_terminator_create(
        int unit,
        bcm_compat6517_tunnel_terminator_t *info);

extern int bcm_compat6517_l2gre_tunnel_terminator_get(
        int unit,
        bcm_compat6517_tunnel_terminator_t *info);

extern int bcm_compat6517_l2gre_tunnel_terminator_update(
        int unit,
        bcm_compat6517_tunnel_terminator_t *info);


extern int bcm_compat6517_vxlan_tunnel_terminator_create(
        int unit,
        bcm_compat6517_tunnel_terminator_t *info);

extern int bcm_compat6517_vxlan_tunnel_terminator_get(
        int unit,
        bcm_compat6517_tunnel_terminator_t *info);

extern int bcm_compat6517_vxlan_tunnel_terminator_update(
        int unit,
        bcm_compat6517_tunnel_terminator_t *info);


extern int bcm_compat6517_tunnel_terminator_add(
        int unit,
        bcm_compat6517_tunnel_terminator_t *info);

extern int bcm_compat6517_tunnel_terminator_delete(
        int unit,
        bcm_compat6517_tunnel_terminator_t *info);

extern int bcm_compat6517_tunnel_terminator_create(
        int unit,
        bcm_compat6517_tunnel_terminator_t *info);

extern int bcm_compat6517_tunnel_terminator_get(
        int unit,
        bcm_compat6517_tunnel_terminator_t *info);

extern int bcm_compat6517_tunnel_terminator_update(
        int unit,
        bcm_compat6517_tunnel_terminator_t *info);
#endif /* INCLUDE_L3 */

typedef struct bcm_compat6517_port_flexe_oam_alarm_s {
    uint8 alarm_active;             /* Indicate if there is active alarm */
    uint8 rx_base_csf_lpi;          /* Rx base OAM client signal LPI fault alarm */
    uint8 rx_base_cs_lf;            /* Rx base OAM client signal Local Fault
                                       alarm */
    uint8 rx_base_cs_rf;            /* Rx base OAM client signal Remote Fault
                                       alarm */
    uint8 base_oam_los;             /* Base OAM no receive alarm */
    uint8 rx_sdbip;                 /* Rx SDBIP alarm */
    uint8 rx_base_crc;              /* Rx OAM base CRC error alarm */
    uint8 rx_base_rdi;              /* Rx OAM base RDI alarm */
    uint8 rx_base_period_mismatch;  /* Rx base OAM period mismatch alarm */
} bcm_compat6517_port_flexe_oam_alarm_t;

/* Get OAM alarms. */
extern int bcm_compat6517_port_flexe_oam_alarm_get(
    int unit,
    bcm_port_t port,
    uint32 flags,
    bcm_compat6517_port_flexe_oam_alarm_t *alarms);

/* IFA configuration information data. */
typedef struct bcm_compat6517_ifa_config_info_s {
    uint32 probemarker_1;               /* INT header ProbeMarker1 Value. */
    uint32 probemarker_2;               /* INT header ProbeMarker2 Value. */
    uint32 device_id;                   /* Meta Data header Device ID. */
    uint16 max_payload_length;          /* INT header Maximum payload. */
    uint16 module_id;                   /* Meta Data header Module Id used to
                                           identify loopback source port
                                           information. */
    uint8 hop_limit;                    /* INT header Hop Limit condition. */
    uint16 rx_packet_payload_length;    /* Recieved packet payload length that
                                           can be sent to collector. */
    uint16 lb_port_1;                   /* Loopback port for IFA E_APP. */
    uint16 lb_port_2;                   /* Loopback port for IFA E_APP. */
    uint8 optional_headers;             /* Option to send with recieved packet
                                           headers. */
    uint8 true_hop_count;               /* Number of hops in the topology is
                                           greater than Hop limit,  IFA MD stack
                                           contains Hop limit number of MDs in
                                           MD stack. */
} bcm_compat6517_ifa_config_info_t;

/* Set a In-band Flow Analyzer configuration information. */
extern int bcm_compat6517_ifa_config_info_set(
    int unit,
    uint32 options,
    bcm_compat6517_ifa_config_info_t *config_data);

/* Get a In-band Flow Analyzer Configuration information. */
extern int bcm_compat6517_ifa_config_info_get(
    int unit,
    bcm_compat6517_ifa_config_info_t *config_data);

/* IFA statistics information data. */
typedef struct bcm_compat6517_ifa_stat_info_s {
    uint32 rx_pkt_cnt;                  /* Number of packet recieved in FW. */
    uint32 tx_pkt_cnt;                  /* Number of packet transmitted from FW. */
    uint32 ifa_no_config_drop;          /* Number of pkt dropped due to missing
                                           configuration. */
    uint32 ifa_collector_not_present_drop; /* Number of pkt dropped due to missing
                                           collector configuration. */
    uint32 ifa_hop_cnt_invalid_drop;    /* Number of pkt dropped due to Hop
                                           count and Hop limit are out of order. */
    uint32 ifa_int_hdr_len_invalid_drop; /* Number of pkt dropped due to maximum
                                           length and current length are out of
                                           order. */
    uint32 ifa_pkt_size_invalid_drop;   /* Number of pkt dropped due to invalid
                                           packet size. */
} bcm_compat6517_ifa_stat_info_t;

/* In-band flow analyzer - IFA statistics information */
extern int bcm_compat6517_ifa_stat_info_set(
    int unit,
    bcm_compat6517_ifa_stat_info_t *stat_data);

/* In-band flow analyzer - IFA statistics information */
extern int bcm_compat6517_ifa_stat_info_get(
    int unit,
    bcm_compat6517_ifa_stat_info_t *stat_data);

/*
 * Queue congestion monitor export trigger info. Calculate InterestFactor
 * of each view and export to collector.
 */
typedef struct bcm_compat6517_cosq_burst_monitor_export_trigger_s {
    uint32 export_threshold;    /* Export threshold */
    uint8 drop_weight;          /* Flow drop counter weight */
    uint8 rx_weight;            /* Flow Rx counter weight */
} bcm_compat6517_cosq_burst_monitor_export_trigger_t;

/* Queue congestion monitor flow view Configuration. */
typedef struct bcm_compat6517_cosq_burst_monitor_flow_view_info_s {
    bcm_compat6517_cosq_burst_monitor_export_trigger_t export_param; /* Queue congestion monitor export
                                                             trigger Info */
    uint32 scan_interval_usecs;         /* sample/scan Interval */
    uint32 age_threshold;               /* Aging persistent flows if sum of all
                                           sampled rx_byte_count values goes
                                           below threshold */
    uint64 start_timer_usecs;           /* Start time of Queue congestion
                                           monitor */
    uint32 num_samples;                 /* Number of samples captured per view */
} bcm_compat6517_cosq_burst_monitor_flow_view_info_t;

/* Set Queue congestion monitoring flow view configuration. */
extern int bcm_compat6517_cosq_burst_monitor_flow_view_config_set(
        int unit,
        uint32 options,
        bcm_compat6517_cosq_burst_monitor_flow_view_info_t *flow_view_data);

/* Get Queue congestion monitoring flow view configuration. */
extern int bcm_compat6517_cosq_burst_monitor_flow_view_config_get(
        int unit,
        bcm_compat6517_cosq_burst_monitor_flow_view_info_t *flow_view_data);

typedef struct bcm_compat6517_flowtracker_group_info_s {
    bcm_flowtracker_ipfix_observation_domain_t observation_domain_id; /* IPFIX observation domain to which
                                           this flow group belongs to. */
    int group_class;                    /* Flowtracker flow group Class id. */
    uint32 group_flags;                 /* Flowtracker flow group flags. See
                                           BCM_FLOWTRACKER_GROUP_XXX. */
    bcm_field_group_t field_group[BCM_PIPES_MAX]; /* Field group corresponding to the flow
                                           group. */
} bcm_compat6517_flowtracker_group_info_t;

typedef struct bcm_compat6517_flowtracker_check_info_s {
    uint32 flags;                       /* Configuration flags for Check
                                           Creation. */
    bcm_flowtracker_tracking_param_type_t param; /* The attribute of flow on which the
                                           check is performed. */
    uint32 min_value;                   /* Element value to do greater than or
                                           equal checks. Minimum value to do
                                           range checks. */
    uint32 max_value;                   /* Element value to do smaller than or
                                           mask checks. Maximum value to do
                                           range checks. */
    uint32 mask_value;                  /* Mask value to be applied on flow
                                           attribute. */
    uint8 shift_value;                  /* Right shift value to be applied on
                                           flow attribute. */
    bcm_flowtracker_check_operation_t operation; /* Operation to be performed for this
                                           check. */
    bcm_flowtracker_check_t primary_check_id; /* primary check id to associate second
                                           check for aggregated checks on same
                                           flow. */
    uint16 num_drop_reason_group_id;    /* Number of drop reason group id. */
    bcm_flowtracker_drop_reason_group_t drop_reason_group_id[BCM_FLOWTRACKER_DROP_REASON_GROUP_ID_MAX]; /* Flowtracker drop reason group ids.
                                           Applicable only if tracking param is
                                           IngDropReasonGroupIdVector or
                                           EgrDropReasonGroupIdVector. */
} bcm_compat6517_flowtracker_check_info_t;

extern int bcm_compat6517_flowtracker_group_create(
        int unit,
        uint32 options,
        bcm_flowtracker_group_t *flow_group_id,
        bcm_compat6517_flowtracker_group_info_t *flow_group_info);

extern int bcm_compat6517_flowtracker_group_get(
        int unit,
        bcm_flowtracker_group_t id,
        bcm_compat6517_flowtracker_group_info_t *flow_group_info);

extern int bcm_compat6517_flowtracker_check_create(
        int unit,
        uint32 options,
        bcm_compat6517_flowtracker_check_info_t check_info,
        bcm_flowtracker_check_t *check_id);

extern int bcm_compat6517_flowtracker_check_get(
        int unit,
        bcm_flowtracker_check_t check_id,
        bcm_compat6517_flowtracker_check_info_t *check_info);
/* CPRI Decapsulation RoE Ordering Information */
typedef struct bcm_compat6517_cpri_decap_roe_oi_s {
    bcm_cpri_roe_info_type_t info_type;    /*   */
    uint32 pcnt_size;    /* Number in the range 1 to 32 */
    uint32 pcnt_increment;    /* seqNumPInc */
    uint32 pcnt_max;    /* seqNumPMax */
    uint32 p_inc_p2;    /* seqNumPInc to be power of 2 */
    uint32 p_bias;    /* Bias for P count */
    uint32 qcnt_size;    /* Number in the range 0 to (32-pcnt) */
    uint32 qcnt_increment;    /* seqNumQInc */
    uint32 qcnt_max;    /* seqNumQMax */
    uint32 q_inc_p2;    /* seqNumQInc to be power of 2 */
    uint32 q_bias;    /* Bias for Q count */
    uint8 p_ext;    /* Extended P Counter */
    uint8 gsm_tsn_bitmap;    /* TSN bitmap */
    uint32 pcnt_pkt_count;    /* Maximum pkt count described by P counter */
    uint32 modulo_2;    /* Queue Cycle Size is modulo 2 */
    uint32 seq_rsvd;    /* seqNumRsvd Value */
} bcm_compat6517_cpri_decap_roe_oi_t;

/* Set/Get the ordering info for Decapsulation. */
extern int bcm_compat6517_cpri_port_decap_roe_ordering_info_set(
    int unit,
    bcm_gport_t port,
    int index,
    bcm_compat6517_cpri_decap_roe_oi_t *ordering_info);

/* Set/Get the ordering info for Decapsulation. */
extern int bcm_compat6517_cpri_port_decap_roe_ordering_info_get(
    int unit,
    bcm_gport_t port,
    int index,
    bcm_compat6517_cpri_decap_roe_oi_t *ordering_info);

typedef struct bcm_compat6517_rx_trap_config_s {

    uint32 flags;                       /* BCM_RX_TRAP_XXX FLAGS. */
    bcm_gport_t dest_port;              /* Destination port. */
    bcm_multicast_t dest_group;         /* Destination group. */
    int prio;                           /* Internal priority of the packet. */
    bcm_color_t color;                  /* Color of the packet. */
    bcm_policer_t ether_policer_id;     /* Ethernet Policer. */
    bcm_policer_t policer_id;           /* general policer id. */
    int counter_num;                    /* counter. */
    int trap_strength;                  /* Strength of this trap, strongest trap
                                           in processing holds. */
    int snoop_cmnd;                     /* snoop command. */
    int snoop_strength;                 /* Strength of this trap, strongest
                                           snoop in processing holds. */
    bcm_forwarding_type_t forwarding_type; /* Forwarding type value */
    bcm_rx_trap_forwarding_header_t forwarding_header; /* Forwarding header position overridden
                                           value */
    bcm_if_t encap_id;                  /* Encap-ID */
    bcm_mirror_options_t *mirror_cmd;   /* Mirror options in ETPP traps */
    bcm_rx_trap_core_config_t *core_config_arr; /* Destinations information per core */
    int core_config_arr_len;            /* core_config_arr length */
    int qos_map_id;                     /* cos profile handle */
    int tunnel_termination_trap_strength; /* Strength of tunnel termination trap,
                                           strongest trap in processing holds. */
    int tunnel_termination_snoop_strength; /* Strength of tunnel termination trap,
                                           strongest snoop in processing holds. */
    int meter_cmd;                      /* meter command. */
    uint8 mapped_trap_strength;         /* mapped trap strength */
    uint8 is_recycle_high_priority;     /* recycle high priority indication */
    uint8 is_recycle_crop_pkt;          /* crop recycle packet indication */
    uint8 is_recycle_append_ftmh;       /* indication regarding appending the
                                           original FTMH to the recycle packet */
    bcm_gport_t cpu_trap_gport;         /* cpu trap gport */
    int stamped_trap_code;              /* trap code stamped on packet headers */
    bcm_if_t encap_id2;                 /* Encap-ID #2 */
    uint8 ecn_value;                    /* Ethernet encapsulation */
    uint16 vsq;                         /* Statistics VSQ pointer */
    uint8 latency_flow_id_clear;        /* Indication if to clear
                                           latency_flow_id */
    uint8 visibility_value;             /* Visibility value */
    uint32 stat_clear_bitmap;           /* Bitmap indicating which statistical
                                           objects are to clear */
    bcm_rx_trap_stat_obj_config_t stat_obj_config_arr[BCM_RX_TRAP_MAX_STAT_OBJ_ARR_LEN]; /* Overwrite configuration of
                                           statistical objects */
    uint32 stat_obj_config_len;         /* stat_obj_config_arr length */
    uint32 stat_metadata_mask;          /* Statistical metadata mask */
    int mirror_profile;                 /* mirror action profile */
    uint32 egress_forwarding_index;     /* Egress forwarding(parsing) index */
    int pp_drop_reason;                 /* Packet processing drop reason */

} bcm_compat6517_rx_trap_config_t;

/* rx_trap_set. */
extern int bcm_compat6517_rx_trap_set(
    int unit,
    int trap_id,
    bcm_compat6517_rx_trap_config_t *config);

/* rx_trap_get. */
extern int bcm_compat6517_rx_trap_get(
    int unit,
    int trap_id,
    bcm_compat6517_rx_trap_config_t *config);

/* Hash bank control structure. */
typedef struct bcm_compat6517_switch_hash_bank_control_s {
    bcm_switch_hash_bank_t bank_num;    /* Hash bank number. */
    uint32 valid_fields;                /* Bitmap of valid fields. */
    int robust;                         /* 0 to disable robust hash or 1 to
                                           enable robust hash. */
    int hash_type;                      /* BCM_HASH_XXX definitions. */
    uint32 offset;                      /* Hash offset. */
} bcm_compat6517_switch_hash_bank_control_t;

/* Hash bank control set/get function. */
extern int bcm_compat6517_switch_hash_bank_control_set(
    int unit,
    bcm_compat6517_switch_hash_bank_control_t *bank_control);

/* Hash bank control set/get function. */
extern int bcm_compat6517_switch_hash_bank_control_get(
    int unit,
    bcm_compat6517_switch_hash_bank_control_t *bank_control);

/* physical port properties structure. */
typedef struct bcm_compat6517_port_interface_info_s {
    uint32 phy_port;    /* related phy port. */
    bcm_port_if_t interface;    /* interface of port. */
    uint32 num_lanes;    /* number of lanes the interface uses. */
    bcm_pbmp_t phy_pbmp;    /* bitmap of phy lanes the interface occupies. */
    uint32 interface_id;    /* should be indicated in case the id can't be derived from phy_port. */
} bcm_compat6517_port_interface_info_t;

/* port mapping properties structure. */
typedef struct bcm_compat6517_port_mapping_info_s {
    uint32 channel;    /* logical port channel. */
    int core;    /* core id */
    uint32 tm_port;    /* tm port id */
    uint32 pp_port;    /* pp port id */
    uint32 base_q_pair;    /* base queue pair id */
    uint32 num_priorities;    /* number of port priorities */
    uint32 base_hr;    /* base hr id */
    uint32 num_sch_priorities;    /* number of sch port priorities */
} bcm_compat6517_port_mapping_info_t;

/* add/remove/get port */
extern int bcm_compat6517_port_add(
    int unit,
    bcm_port_t port,
    uint32 flags,
    bcm_compat6517_port_interface_info_t *interface_info,
    bcm_compat6517_port_mapping_info_t *mapping_info);

/* add/remove/get port */
extern int bcm_compat6517_port_get(
    int unit,
    bcm_port_t port,
    uint32 *flags,
    bcm_compat6517_port_interface_info_t *interface_info,
    bcm_compat6517_port_mapping_info_t *mapping_info);


#endif /* BCM_RPC_SUPPORT */
#endif /* !_COMPAT_6517_H */
