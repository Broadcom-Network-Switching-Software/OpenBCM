/*
* $Id: compat_6520.h,v 1.0 2020/07/08
* This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
* 
* Copyright 2007-2022 Broadcom Inc. All rights reserved.
*
* RPC Compatibility with sdk-6.5.20 routines
*/

#ifndef _COMPAT_6520_H_
#define _COMPAT_6520_H_

#ifdef BCM_RPC_SUPPORT
#include <bcm_int/compat/compat_6526.h>
#include <bcm_int/compat/compat_6525.h>
#include <bcm_int/compat/compat_6523.h>
#include <bcm_int/compat/compat_6522.h>
#include <bcm_int/compat/compat_6521.h>
#if defined(INCLUDE_L3)
#include <bcm/l2gre.h>
#endif /* defined(INCLUDE_L3) */
#if defined(INCLUDE_L3)
#include <bcm/wlan.h>
#endif /* defined(INCLUDE_L3) */
#if defined(INCLUDE_L3)
#include <bcm/vxlan.h>
#endif /* defined(INCLUDE_L3) */
#if defined(INCLUDE_L3)
#include <bcm/tunnel.h>
#endif /* defined(INCLUDE_L3) */
#include <bcm/flexctr.h>
#include <bcm/flow.h>
#include <bcm/ecn.h>
#include <bcm/mirror.h>
#if defined(INCLUDE_L3)
#include <bcm/l3.h>
#endif /* defined(INCLUDE_L3) */
#include <bcm/vlan.h>
#include <bcm/field.h>
#include <bcm/port.h>
#include <bcm/fcoe.h>
#include <bcm/types.h>
#include <bcm/srv6.h>
#include <bcm/oam.h>
#include <bcm/ifa.h>
#if defined(INCLUDE_BFD)
#include <bcm/bfd.h>
#endif /* defined(INCLUDE_BFD) */
#include <bcm/flowtracker.h>
#include <bcm/flexstate.h>
#include <bcm/collector.h>
#include <bcm/mpls.h>


/* bcm_mpls_egress_label_s */
typedef struct bcm_compat6520_mpls_egress_label_s {
    uint32 flags;    /* BCM_MPLS_EGRESS_LABEL_xxx. */
    uint32 flags2;    /* BCM_MPLS_EGRESS_LABEL_FLAGS2_xxx. */
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
    bcm_qos_egress_model_t egress_qos_model;    /* egress qos and ttl model */
} bcm_compat6520_mpls_egress_label_t;


/* OAM endpoint object.  This is equivalent to an 802.1ag Maintenance Endpoint (MEP). */
typedef struct bcm_compat6520_oam_endpoint_info_s {
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
    bcm_compat6520_mpls_egress_label_t egress_label;    /* The MPLS outgoing label information for the Local Endpoint */
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
    bcm_oam_endpoint_signal_t rx_signal;    /* Expected Signal Indication */
    bcm_oam_endpoint_signal_t tx_signal;    /* Transmitted Signal Indication */
} bcm_compat6520_oam_endpoint_info_t;


/* Create or replace an OAM endpoint object. */
extern int bcm_compat6520_oam_endpoint_create(
    int unit,
    bcm_compat6520_oam_endpoint_info_t *endpoint_info);

/* Get an OAM endpoint object. */
extern int bcm_compat6520_oam_endpoint_get(
    int unit,
    bcm_oam_endpoint_t endpoint,
    bcm_compat6520_oam_endpoint_info_t *endpoint_info);


/* Structure that is used in SRv6 SRH Base initiator APIs, for managing device behavior at SRv6 Tunnel Encapsulation */
typedef struct bcm_compat6520_srv6_srh_base_initiator_info_s {
    uint32 flags;    /* BCM_SRV6_SRH_BASE_INITIATOR_XXX */
    bcm_gport_t tunnel_id;    /* Tunnel SRv6 SRH base object ID */
    int nof_sids;    /* Number of SIDs in the SID list */
    int qos_map_id;    /* QOS map identifier */
    int ttl;    /* Tunnel header TTL */
    int dscp;    /* Tunnel header DSCP value */
    bcm_qos_egress_model_t egress_qos_model;    /* Egress qos and ttl model */
    bcm_if_t next_encap_id;    /* Next encapsulation ID */
    int nof_additional_sids_extended_encap;    /* Number of SIDs encapsulated in 1st pass of extended encapsulation, to be used to update total extension length */
} bcm_compat6520_srv6_srh_base_initiator_info_t;

/* Create/Get/Delete/Traverse SRv6 SRH Base properties */
extern int bcm_compat6520_srv6_srh_base_initiator_create(
    int unit,
    bcm_compat6520_srv6_srh_base_initiator_info_t *info);

/* Create/Get/Delete/Traverse SRv6 SRH Base properties */
extern int bcm_compat6520_srv6_srh_base_initiator_get(
    int unit,
    bcm_compat6520_srv6_srh_base_initiator_info_t *info);

/* Create/Get/Delete/Traverse SRv6 SRH Base properties */
extern int bcm_compat6520_srv6_srh_base_initiator_delete(
    int unit,
    bcm_compat6520_srv6_srh_base_initiator_info_t *info);

/* Initialize a VLAN tag action set structure. */
typedef struct bcm_compat6520_vlan_action_set_s {
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
    bcm_vlan_action_t inner_tag;    /* Inner_tag action for all tagged packets. */
    bcm_vlan_t forwarding_domain;    /* Forwarding domain (VLAN or VPN). */
    int inner_qos_map_id;    /* Map ID of inner QOS profile */
    int outer_qos_map_id;    /* Map ID of outer QOS profile. */
    uint32 opaque_ctrl_id;    /* Opaque control ID. */
} bcm_compat6520_vlan_action_set_t;

/* VLAN/VSAN Translation Action Set. */
typedef struct bcm_compat6520_fcoe_vsan_action_set_s {
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
    bcm_compat6520_vlan_action_set_t vlan_action;    /* VLAN actions */
} bcm_compat6520_fcoe_vsan_action_set_t;

/* Add FCOE VSAN and VLAN translation action. */
extern int bcm_compat6520_fcoe_vsan_translate_action_add(
    int unit,
    bcm_fcoe_vsan_translate_key_config_t *key,
    bcm_compat6520_fcoe_vsan_action_set_t *action);

/* UDF Abstract Packet Format Information Structure. */
typedef struct bcm_compat6520_udf_abstract_pkt_format_info_s {
    bcm_udf_pkt_base_offset_t base_offset; /* Relative offset of
                                              'bcmUdfPktBaseOffsetXX' type (from
                                              'bcm_udf_pkt_base_offset_t') */
    uint32 num_chunks_max;              /* Maximum number of chunks supported by
                                           the abstract packet format */
    uint32 chunk_bmap_used;             /* Bitmap of Chunks already used by the
                                           abstract packet format */
    uint32 unavail_chunk_bmap;          /* Bitmap of Chunks unavailable/reserved */
} bcm_compat6520_udf_abstract_pkt_format_info_t;

/* API to retrieve Abstract packet format information. */
extern int bcm_compat6520_udf_abstract_pkt_format_info_get(
    int unit, 
    bcm_udf_abstract_pkt_format_t abstract_pkt_format, 
    bcm_compat6520_udf_abstract_pkt_format_info_t *pkt_format_info);

/* Add VLAN actions to a field entry. */
extern int bcm_compat6520_field_action_vlan_actions_add(
    int unit,
    bcm_field_entry_t entry,
    bcm_field_action_t action,
    bcm_compat6520_vlan_action_set_t *vlan_action_set);

/* Retrieve VLAN actions configured to a field entry. */
extern int bcm_compat6520_field_action_vlan_actions_get(
    int unit,
    bcm_field_entry_t entry,
    bcm_field_action_t action,
    bcm_compat6520_vlan_action_set_t *vlan_action_set);

/* Get or set the port's default VLAN tag actions. */
extern int bcm_compat6520_vlan_port_default_action_get(
    int unit,
    bcm_port_t port,
    bcm_compat6520_vlan_action_set_t *action);

/* Get or set the port's default VLAN tag actions. */
extern int bcm_compat6520_vlan_port_default_action_set(
    int unit,
    bcm_port_t port,
    bcm_compat6520_vlan_action_set_t *action);

/* Get or set the egress default VLAN tag actions. */
extern int bcm_compat6520_vlan_port_egress_default_action_get(
    int unit,
    bcm_port_t port,
    bcm_compat6520_vlan_action_set_t *action);

/* Get or set the egress default VLAN tag actions. */
extern int bcm_compat6520_vlan_port_egress_default_action_set(
    int unit,
    bcm_port_t port,
    bcm_compat6520_vlan_action_set_t *action);

/* Add protocol-based VLAN with specified action. If the entry already exists, update the action. */
extern int bcm_compat6520_vlan_port_protocol_action_add(
    int unit,
    bcm_port_t port,
    bcm_port_frametype_t frame,
    bcm_port_ethertype_t ether,
    bcm_compat6520_vlan_action_set_t *action);

/* Get protocol-based VLAN specified action. */
extern int bcm_compat6520_vlan_port_protocol_action_get(
    int unit,
    bcm_port_t port,
    bcm_port_frametype_t frame,
    bcm_port_ethertype_t ether,
    bcm_compat6520_vlan_action_set_t *action);

/* Add an association from MAC address to VLAN actions to use for assigning VLAN tag actions to untagged packets. */
extern int bcm_compat6520_vlan_mac_action_add(
    int unit,
    bcm_mac_t mac,
    bcm_compat6520_vlan_action_set_t *action);

/* Retrieve an association from MAC address to VLAN actions, which
are used for VLAN tag assignment to untagged packets. */
extern int bcm_compat6520_vlan_mac_action_get(
    int unit,
    bcm_mac_t mac,
    bcm_compat6520_vlan_action_set_t *action);

/* Add an entry to the VLAN Translation table and assign VLAN actions. */
extern int bcm_compat6520_vlan_translate_action_add(
    int unit,
    bcm_gport_t port,
    bcm_vlan_translate_key_t key_type,
    bcm_vlan_t outer_vlan,
    bcm_vlan_t inner_vlan,
    bcm_compat6520_vlan_action_set_t *action);

/* Add an entry to the VLAN Translation table and assign VLAN actions. */
extern int bcm_compat6520_vlan_translate_action_create(
    int unit,
    bcm_gport_t port,
    bcm_vlan_translate_key_t key_type,
    bcm_vlan_t outer_vlan,
    bcm_vlan_t inner_vlan,
    bcm_compat6520_vlan_action_set_t *action);

/* Get the assigned VLAN actions for the given port, key type, and VLAN tags. */
extern int bcm_compat6520_vlan_translate_action_get(
    int unit,
    bcm_gport_t port,
    bcm_vlan_translate_key_t key_type,
    bcm_vlan_t outer_vlan,
    bcm_vlan_t inner_vlan,
    bcm_compat6520_vlan_action_set_t *action);

/* Add an entry to the egress VLAN Translation table and assign VLAN actions. */
extern int bcm_compat6520_vlan_translate_egress_action_add(
    int unit,
    int port_class,
    bcm_vlan_t outer_vlan,
    bcm_vlan_t inner_vlan,
    bcm_compat6520_vlan_action_set_t *action);

/* Get the assigned VLAN actions for egress VLAN translation on the given port class and VLAN tags. */
extern int bcm_compat6520_vlan_translate_egress_action_get(
    int unit,
    int port_class,
    bcm_vlan_t outer_vlan,
    bcm_vlan_t inner_vlan,
    bcm_compat6520_vlan_action_set_t *action);

/* Add an entry based on gport to the egress VLAN Translation table and assign VLAN actions. */
extern int bcm_compat6520_vlan_translate_egress_gport_action_add(
    int unit,
    bcm_gport_t port,
    bcm_vlan_t outer_vlan,
    bcm_vlan_t inner_vlan,
    bcm_compat6520_vlan_action_set_t *action);

/* Get the assigned VLAN actions for egress VLAN translation on the given gport and VLAN tags. */
extern int bcm_compat6520_vlan_translate_egress_gport_action_get(
    int unit,
    bcm_gport_t port,
    bcm_vlan_t outer_vlan,
    bcm_vlan_t inner_vlan,
    bcm_compat6520_vlan_action_set_t *action);

/* Add an entry to the VLAN Translation table, which assigns VLAN actions for packets matching within the VLAN range(s). */
extern int bcm_compat6520_vlan_translate_action_range_add(
    int unit,
    bcm_gport_t port,
    bcm_vlan_t outer_vlan_low,
    bcm_vlan_t outer_vlan_high,
    bcm_vlan_t inner_vlan_low,
    bcm_vlan_t inner_vlan_high,
    bcm_compat6520_vlan_action_set_t *action);

/* Get assigned VLAN actions from VLAN Translation table for the specified VLAN range(s). */
extern int bcm_compat6520_vlan_translate_action_range_get(
    int unit,
    bcm_gport_t port,
    bcm_vlan_t outer_vlan_low,
    bcm_vlan_t outer_vlan_high,
    bcm_vlan_t inner_vlan_low,
    bcm_vlan_t inner_vlan_high,
    bcm_compat6520_vlan_action_set_t *action);

/* Set a VLAN translation ID instance with tag actions. */
extern int bcm_compat6520_vlan_translate_action_id_set(
    int unit,
    uint32 flags,
    int action_id,
    bcm_compat6520_vlan_action_set_t *action);

/* Get tag actions from a VLAN translation ID instance. */
extern int bcm_compat6520_vlan_translate_action_id_get(
    int unit,
    uint32 flags,
    int action_id,
    bcm_compat6520_vlan_action_set_t *action);

/* Add an association from IP subnet to VLAN actions to use for assigning VLAN tag actions to untagged packets. */
extern int bcm_compat6520_vlan_ip_action_add(
    int unit,
    bcm_vlan_ip_t *vlan_ip,
    bcm_compat6520_vlan_action_set_t *action);

/* Get an association from IP subnet to VLAN actions that used for assigning VLAN tag actions to untagged packets. */
extern int bcm_compat6520_vlan_ip_action_get(
    int unit,
    bcm_vlan_ip_t *vlan_ip,
    bcm_compat6520_vlan_action_set_t *action);

/* Add action for match criteria for Vlan assignment. */
extern int bcm_compat6520_vlan_match_action_add(
    int unit,
    uint32 options,
    bcm_compat6523_vlan_match_info_t *match_info,
    bcm_compat6520_vlan_action_set_t *action_set);

/* Get action for match criteria. */
extern int bcm_compat6520_vlan_match_action_get(
    int unit,
    bcm_compat6523_vlan_match_info_t *match_info,
    bcm_compat6520_vlan_action_set_t *action_set);

/* Get all actions and match criteria for given vlan match. */
extern int bcm_compat6520_vlan_match_action_multi_get(
    int unit,
    bcm_vlan_match_t match,
    int size,
    bcm_compat6523_vlan_match_info_t *match_info_array,
    bcm_compat6520_vlan_action_set_t *action_set_array,
    int *count);

/* This structure contains the configuration of a VLAN. */
typedef struct bcm_compat6520_vlan_control_vlan_s {
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
    bcm_compat6520_vlan_action_set_t egress_action;    /* Egress VLAN actions. */
    bcm_vlan_l2_view_t l2_view;    /* L2 table view. */
    int egress_class_id;    /* Class ID for EFP qualifier. */
    int egress_opaque_ctrl_id;    /* Egress opaque control ID. */
    int ingress_opaque_ctrl_id;    /* Ingress opaque control ID. */
    bcm_gport_t unknown_dest;    /* Gport for default destination, valid when BCM_VLAN_FLAGS2_UNKNOWN_DEST is set */
} bcm_compat6520_vlan_control_vlan_t;

/* Set or retrieve current VLAN properties selectively. */
extern int bcm_compat6520_vlan_control_vlan_selective_get(
    int unit,
    bcm_vlan_t vlan,
    uint32 valid_fields,
    bcm_compat6520_vlan_control_vlan_t *control);

/* Set or retrieve current VLAN properties selectively. */
extern int bcm_compat6520_vlan_control_vlan_selective_set(
    int unit,
    bcm_vlan_t vlan,
    uint32 valid_fields,
    bcm_compat6520_vlan_control_vlan_t *control);

/* Set or retrieve current VLAN properties. */
extern int bcm_compat6520_vlan_control_vlan_get(
    int unit,
    bcm_vlan_t vlan,
    bcm_compat6520_vlan_control_vlan_t *control);

/* Set or retrieve current VLAN properties. */
extern int bcm_compat6520_vlan_control_vlan_set(
    int unit,
    bcm_vlan_t vlan,
    bcm_compat6520_vlan_control_vlan_t control);

/* VLAN Egress Translation Action Set. */
typedef struct bcm_compat6520_vlan_translate_egress_action_set_s {
    uint32 flags;    /*  */
    bcm_compat6520_vlan_action_set_t vlan_action;    /* VLAN actions. */
} bcm_compat6520_vlan_translate_egress_action_set_t;

/* Add an entry to the egress VLAN Translation table and assign VLAN actions. It extends bcm_vlan_egress_action_add API for additional keys. */
extern int bcm_compat6520_vlan_translate_egress_action_extended_add(
    int unit,
    bcm_compat6521_vlan_translate_egress_key_config_t *key_config,
    bcm_compat6520_vlan_translate_egress_action_set_t *action);

/* Retrive an egress translation entry including key types added by bcm_vlan_translate_egress_extended_action_add API. */
extern int bcm_compat6520_vlan_translate_egress_action_extended_get(
    int unit,
    bcm_compat6521_vlan_translate_egress_key_config_t *key_config,
    bcm_compat6520_vlan_translate_egress_action_set_t *action);


#if defined(INCLUDE_L3)
/* L3 AACL Structure.
@p Contains information required for manipulating L3 AACLs.
 */
typedef struct bcm_compat6520_l3_aacl_s {
    uint32 flags;    /* See BCM_L3_AACL_FLAGS_xxx flag definitions. */
    bcm_ip_t ip;    /* IP subnet address (IPv4). */
    bcm_ip_t ip_mask;    /* IP subnet mask (IPv4). */
    bcm_ip6_t ip6;    /* IP subnet address (IPv6). */
    bcm_ip6_t ip6_mask;    /* IP subnet mask (IPv6). */
    bcm_l4_port_t l4_port;    /* L4 port. */
    bcm_l4_port_t l4_port_mask;    /* L4 port mask. */
    uint32 class_id;    /* Compression class ID. */
} bcm_compat6520_l3_aacl_t;

/* Adds a L3 AACL to the compression table. */
extern int bcm_compat6520_l3_aacl_add(
    int unit,
    uint32 options,
    bcm_compat6520_l3_aacl_t *aacl);

/* Deletes a L3 AACL from the compression table. */
extern int bcm_compat6520_l3_aacl_delete(
    int unit,
    bcm_compat6520_l3_aacl_t *aacl);

/* Deletes all AACLs. */
extern int bcm_compat6520_l3_aacl_delete_all(
    int unit,
    bcm_compat6520_l3_aacl_t *aacl);

/* Finds information from the AACL table. */
extern int bcm_compat6520_l3_aacl_find(
    int unit,
    bcm_compat6520_l3_aacl_t *aacl);
#endif /* defined(INCLUDE_L3) */

#if defined(INCLUDE_BFD)
/* BFD endpoint object. */
typedef struct bcm_compat6520_bfd_endpoint_info_s {
    uint32 flags;                       /* Control flags. */
    bcm_bfd_endpoint_t id;              /* Endpoint identifier. */
    bcm_bfd_endpoint_t remote_id;       /* Remote endpoint identifier. */
    bcm_bfd_tunnel_type_t type;         /* Type of BFD encapsulation. */
    bcm_gport_t gport;                  /* Gport identifier. */
    bcm_gport_t tx_gport;               /* TX gport associated with this
                                           endpoint. */
    bcm_gport_t remote_gport;           /* Gport identifier of trapping
                                           destination: OAMP or CPU, local or
                                           remote. */
    int bfd_period;                     /* For local endpoints, this is the BFD
                                           transmission period in ms. */
    bcm_vpn_t vpn;                      /* VPN. */
    uint8 vlan_pri;                     /* VLAN tag priority. */
    uint8 inner_vlan_pri;               /* Inner VLAN tag priority. */
    bcm_vrf_t vrf_id;                   /* Vrf identifier. */
    bcm_mac_t dst_mac;                  /* Destination MAC. */
    bcm_mac_t src_mac;                  /* Source MAC. */
    bcm_vlan_t pkt_inner_vlan_id;       /* TX Packet inner Vlan Id. */
    bcm_ip_t dst_ip_addr;               /* Destination IPv4 address. */
    bcm_ip6_t dst_ip6_addr;             /* Destination IPv6 address. */
    bcm_ip_t src_ip_addr;               /* Source IPv4 address. */
    bcm_ip6_t src_ip6_addr;             /* Source IPv6 address. */
    uint8 ip_tos;                       /* IPv4 Tos / IPv6 Traffic Class. */
    uint8 ip_ttl;                       /* IPv4 TTL / IPv6 Hop Count. */
    bcm_ip_t inner_dst_ip_addr;         /* Inner destination IPv4 address. */
    bcm_ip6_t inner_dst_ip6_addr;       /* Inner destination IPv6 address. */
    bcm_ip_t inner_src_ip_addr;         /* Inner source IPv4 address. */
    bcm_ip6_t inner_src_ip6_addr;       /* Inner source IPv6 address. */
    uint8 inner_ip_tos;                 /* Inner IPv4 Tos / IPv6 Traffic Class. */
    uint8 inner_ip_ttl;                 /* Inner IPv4 TTL / IPv6 Hop Count. */
    uint32 udp_src_port;                /* UDP source port for Ipv4, Ipv6. */
    bcm_mpls_label_t label;             /* Incoming inner label. */
    bcm_mpls_label_t mpls_hdr;          /* MPLS Header. */
    bcm_mpls_egress_label_t egress_label; /* The MPLS outgoing label information. */
    bcm_if_t egress_if;                 /* Egress interface. */
    uint8 mep_id[BCM_BFD_ENDPOINT_MAX_MEP_ID_LENGTH]; /* MPLS-TP CC/CV TLV and Source MEP ID. */
    uint8 mep_id_length;                /* Length of MPLS-TP CC/CV TLV and
                                           MEP-ID. */
    bcm_cos_t int_pri;                  /* Egress queuing for outgoing BFD to
                                           remote. */
    uint8 cpu_qid;                      /* CPU queue for BFD. */
    bcm_bfd_state_t local_state;        /* Local session state. */
    uint32 local_discr;                 /* Local discriminator. */
    bcm_bfd_diag_code_t local_diag;     /* Local diagnostic code. */
    uint32 local_flags;                 /* Flags combination on outgoing frames. */
    uint32 local_min_tx;                /* Desired local min tx interval. */
    uint32 local_min_rx;                /* Required local rx interval. */
    uint32 local_min_echo;              /* Local minimum echo interval. */
    uint8 local_detect_mult;            /* Local detection interval multiplier. */
    bcm_bfd_auth_type_t auth;           /* Authentication type. */
    uint32 auth_index;                  /* Authentication index. */
    uint32 tx_auth_seq;                 /* Tx authentication sequence id. */
    uint32 rx_auth_seq;                 /* Rx authentication sequence id. */
    uint32 remote_flags;                /* Remote flags. */
    bcm_bfd_state_t remote_state;       /* Remote session state. */
    uint32 remote_discr;                /* Remote discriminator. */
    bcm_bfd_diag_code_t remote_diag;    /* Remote diagnostic code. */
    uint32 remote_min_tx;               /* Desired remote min tx interval. */
    uint32 remote_min_rx;               /* Required remote rx interval. */
    uint32 remote_min_echo;             /* Remote minimum echo interval. */
    uint8 remote_detect_mult;           /* Remote detection interval multiplier. */
    int sampling_ratio;                 /* 0 - No packets sampled to the CPU.
                                           1-8 - Count of packets (with events)
                                           that need to arrive before one is
                                           sampled to the CPU. */
    uint8 loc_clear_threshold;          /* Number of packets required to reset
                                           the Loss-of-Continuity status per
                                           endpoint. */
    uint32 ip_subnet_length;            /* The subnet length for incoming packet
                                           validity check. Value 0 indicates no
                                           check is performed, positive values
                                           indicate the amount of MSBs to be
                                           compared. */
    uint8 remote_mep_id[BCM_BFD_ENDPOINT_MAX_MEP_ID_LENGTH]; /* MPLS-TP CC/CV TLV and Remote MEP ID. */
    uint8 remote_mep_id_length;         /* Length of MPLS-TP CC/CV TLV and
                                           REMOTE MEP-ID. */
    uint8 mis_conn_mep_id[BCM_BFD_ENDPOINT_MAX_MEP_ID_LENGTH]; /* MPLS-TP CC/CV TLV and Mis
                                           connectivity MEP ID. */
    uint8 mis_conn_mep_id_length;       /* Length of MPLS-TP CC/CV TLV and Mis
                                           connectivity MEP-ID. */
    uint32 bfd_detection_time;          /* Optional: BFD Detection Time, in
                                           microseconds. */
    bcm_vlan_t pkt_vlan_id;             /* TX Packet Vlan Id. */
    bcm_vlan_t rx_pkt_vlan_id;          /* RX Packet Vlan Id. */
    bcm_mpls_label_t gal_label;         /* MPLS GAL label. */
    uint32 faults;                      /* Fault flags. */
    uint32 flags2;                      /* Second set of control flags. */
    int ipv6_extra_data_index;          /* Pointer to first extended data entry */
    int punt_good_packet_period;        /* BFD good packets sampling period.
                                           Every punt_good_packet_period
                                           milliseconds, a single packet is
                                           punted to the CPU */
    uint8 bfd_period_cluster;           /* Tx period group. All MEPs in a group
                                           must be created with the same Tx
                                           period. Modifying the Tx period in
                                           one MEP affects the others. */
} bcm_compat6520_bfd_endpoint_info_t;

/* Create or update an BFD endpoint object */
extern int bcm_compat6520_bfd_endpoint_create(
    int unit,
    bcm_compat6520_bfd_endpoint_info_t *endpoint_info);

/* Get an BFD endpoint object. */
extern int bcm_compat6520_bfd_endpoint_get(
    int unit,
    bcm_bfd_endpoint_t endpoint,
    bcm_compat6520_bfd_endpoint_info_t *endpoint_info);
#endif /* defined(INCLUDE_BFD) */

/* Sample profile structure. */
typedef struct bcm_compat6520_mirror_sample_profile_s {
    uint8 enable;    /* Sample enable. */
    uint32 rate;    /* Sample rate. */
    uint8 pool_count_enable;    /* Enable to count the packets seen by the sampler. */
    uint8 sample_count_enable;    /* Enable to count the packets sampled by the sampler. */
    uint8 trace_enable;    /* Enable to generate trace event. */
    bcm_mirror_sample_mirror_mode_t mirror_mode;    /* Sample mirror mode. */
} bcm_compat6520_mirror_sample_profile_t;

/* Set sample profile entry with given profile id and type. */
extern int bcm_compat6520_mirror_sample_profile_set(
    int unit,
    bcm_mirror_sample_type_t type,
    int profile_id,
    bcm_compat6520_mirror_sample_profile_t *profile);

/* Get sample profile entry with given profile id and type. */
extern int bcm_compat6520_mirror_sample_profile_get(
    int unit,
    bcm_mirror_sample_type_t type,
    int profile_id,
    bcm_compat6520_mirror_sample_profile_t *profile);

/* OAM loss object. */
typedef struct bcm_compat6520_oam_loss_s {
    uint32 flags;
    int loss_id;                        /* Oam Loss ID. */
    bcm_oam_endpoint_t id;              /* Endpoint ID of Local MEP. */
    bcm_oam_endpoint_t remote_id;       /* Endpoint ID of Remote MEP. */
    int period;                         /* Frequency of loss tx period in
                                           milliseconds. */
    int loss_threshold;                 /* Loss ratio (expressed in 100th of
                                           percent) which if exceeded will
                                           declare the period degraded with a
                                           Loss Measurement packet exception.  A
                                           value of -1 for Loss Measurement
                                           exception on all received packets. */
    int loss_nearend;                   /* Nearend loss (expressed in 100th of
                                           percent). A value os -1 if not
                                           available. */
    int loss_farend;                    /* Farend loss (expressed in 100th of
                                           percent). A value os -1 if not
                                           available. */
    uint32 loss_nearend_byte;           /* Nearend loss in bytes. */
    uint32 loss_farend_byte;            /* Farend loss in bytes. */
    uint32 loss_nearend_byte_upper;     /* Nearend loss in bytes (Upper 32 bits
                                           in 64 bit byte count mode). */
    uint32 loss_farend_byte_upper;      /* Farend loss in bytes (Upper 32 bits
                                           in 64 bit byte count mode). */
    int loss_nearend_max;               /* Nearend maximal loss. A value os -1
                                           if not available. Reset when read. */
    int loss_nearend_acc;               /* Nearend accumulated loss. A value os
                                           -1 if not available. Reset when read. */
    int loss_farend_max;                /* Farend maximal loss. A value os -1 if
                                           not available. Reset when read. */
    int loss_farend_acc;                /* Farend accumulated loss. A value os
                                           -1 if not available. Reset when read. */
    uint32 tx_nearend;                  /* Last local transmit frame count
                                           recorded at time of LMR */
    uint32 rx_nearend;                  /* Last local receive frame count
                                           recorded at time of LMR */
    uint32 tx_farend;                   /* Last peer transmit frame count
                                           recorded at time of LMR */
    uint32 rx_farend;                   /* Last peer receive frame count
                                           recorded at time of LMR */
    uint32 tx_nearend_byte;             /* Local transmit byte count */
    uint32 rx_nearend_byte;             /* Local receive byte count */
    uint32 tx_farend_byte;              /* Peer transmit byte count */
    uint32 rx_farend_byte;              /* Peer receive byte count */
    uint32 tx_nearend_byte_upper;       /* Local transmit byte count (Upper 32
                                           bits in 64 bit byte count mode). */
    uint32 rx_nearend_byte_upper;       /* Local receive byte count (Upper 32
                                           bits in 64 bit byte count mode). */
    uint32 tx_farend_byte_upper;        /* Peer transmit byte count (Upper 32
                                           bits in 64 bit byte count mode). */
    uint32 rx_farend_byte_upper;        /* Peer receive byte count (Upper 32
                                           bits in 64 bit byte count mode). */
    uint8 pkt_pri_bitmap;               /* Bitmap of packet priorities which
                                           should be counted for LM.  A value of
                                           zero is the equivalent of all ones. */
    uint8 pkt_dp_bitmap;                /* Bitmap of packet color or DP which
                                           should be counted for LM.  A value of
                                           zero is the equivalent of all ones. */
    uint8 pkt_pri;                      /* Egress marking for outgoing LM
                                           messages */
    bcm_cos_t int_pri;                  /* Egress queuing for outgoing LM
                                           messages */
    bcm_gport_t gport;                  /* For CEP only; may return a diverged
                                           LP */
    uint32 rx_oam_packets;              /* Count of OAM packets received by this
                                           endpoint */
    uint32 tx_oam_packets;              /* Count of OAM packets transmitted by
                                           this endpoint */
    bcm_mac_t peer_da_mac_address;      /* MAC DA in LMM injection in case peer
                                           endpoint is not configured in
                                           remote_id */
    uint32 lm_counter_base_id[BCM_OAM_LM_COUNTER_MAX]; /* Base index of LM Counter group
                                           (returned during endpoint create) of
                                           the counter to be stamped or
                                           incremented */
    uint32 lm_counter_offset[BCM_OAM_LM_COUNTER_MAX]; /* LM Counter offset from the base */
    int lm_counter_action[BCM_OAM_LM_COUNTER_MAX]; /* LM Counter action (type
                                           bcm_oam_lm_counter_action_t) */
    uint32 lm_counter_byte_offset[BCM_OAM_LM_COUNTER_MAX]; /* LM Counter byte offset for increment */
    uint32 lm_counter_size;             /* Valid index count in LM Counter array */
    uint32 pm_id;                       /* Performance measurement ID in BTE */
    int measurement_period;             /* Slr measurement period in
                                           milliseconds. */
    uint32 slm_counter_base_id;         /* Must be even. Two counters are used,
                                           given counter for RX and given
                                           counter plus 1 for TX */
    uint32 slm_counter_core_id;         /* Slm counter core ID. */
} bcm_compat6520_oam_loss_t;

/* Add an OAM loss object */
extern int bcm_compat6520_oam_loss_add(
        int unit,
        bcm_compat6520_oam_loss_t *oam_loss);

/* Get an OAM loss object */
extern int bcm_compat6520_oam_loss_get(
        int unit,
        bcm_compat6520_oam_loss_t *oam_loss);

/* Delete an OAM loss object */
extern int bcm_compat6520_oam_loss_delete(
        int unit,
        bcm_compat6520_oam_loss_t *oam_loss);

/*
 * Mirror destination Structure
 *
 * Contains information required for manipulating mirror destinations.
 */
typedef struct bcm_compat6520_mirror_destination_s {
    bcm_gport_t mirror_dest_id;         /* Unique mirror destination and
                                           encapsulation identifier. */
    uint32 flags;                       /* See BCM_MIRROR_DEST_xxx flag
                                           definitions. */
    bcm_gport_t gport;                  /* Mirror destination. */
    uint8 version;                      /* IP header version. */
    uint8 tos;                          /* Traffic Class/Tos byte. */
    uint8 ttl;                          /* Hop limit. */
    bcm_ip_t src_addr;                  /* Tunnel source ip address (IPv4). */
    bcm_ip_t dst_addr;                  /* Tunnel destination ip address (IPv4). */
    bcm_ip6_t src6_addr;                /* Tunnel source ip address (IPv6). */
    bcm_ip6_t dst6_addr;                /* Tunnel destination ip address (IPv6). */
    uint32 flow_label;                  /* IPv6 header flow label field. */
    bcm_mac_t src_mac;                  /* L2 source mac address. */
    bcm_mac_t dst_mac;                  /* L2 destination mac address. */
    uint16 tpid;                        /* L2 header outer TPID. */
    bcm_vlan_t vlan_id;                 /* Vlan id. */
    bcm_trill_name_t trill_src_name;    /* TRILL source bridge nickname */
    bcm_trill_name_t trill_dst_name;    /* TRILL destination bridge nickname */
    int trill_hopcount;                 /* TRILL hop count */
    uint16 niv_src_vif;                 /* Source Virtual Interface of NIV tag */
    uint16 niv_dst_vif;                 /* Destination Virtual Interface of NIV
                                           tag */
    uint32 niv_flags;                   /* NIV flags BCM_MIRROR_NIV_XXX */
    uint16 gre_protocol;                /* L3 GRE header protocol */
    bcm_policer_t policer_id;           /* policer_id */
    int stat_id;                        /* stat_id */
    int stat_id2;                       /* stat_id2 for second counter pointer */
    bcm_if_t encap_id;                  /* Encapsulation index */
    bcm_if_t tunnel_id;                 /* IP tunnel for encapsulation. Valid
                                           only if BCM_MIRROR_DEST_TUNNEL_IP_GRE
                                           is set */
    uint16 span_id;                     /* SPAN-ID. Valid only if
                                           BCM_MIRROR_DEST_TUNNEL_WITH_SPAN_ID
                                           is set */
    uint8 pkt_prio;                     /* L2 header PCP */
    uint32 sample_rate_dividend;        /* The probability of mirroring a packet
                                           is: sample_rate_dividend >=
                                           sample_rate_divisor ? 1 :
                                           sample_rate_dividend /
                                           sample_rate_divisor */
    uint32 sample_rate_divisor;
    uint8 int_pri;                      /* Internal Priority */
    uint16 etag_src_vid;                /* Extended (source) port vlan id */
    uint16 etag_dst_vid;                /* Extended (destination) port vlan id */
    uint16 udp_src_port;                /* UDP source port */
    uint16 udp_dst_port;                /* UDP destination port */
    uint32 egress_sample_rate_dividend; /* The probability of outbound mirroring
                                           a packet from the destination is
                                           sample_rate_dividend >=
                                           sample_rate_divisor ? 1 :
                                           sample_rate_dividend /
                                           sample_rate_divisor */
    uint32 egress_sample_rate_divisor;
    uint8 recycle_context;              /* recycle context of egress originating
                                           packets */
    uint16 packet_copy_size;            /*  If non zero, the copied packet will
                                           be truncated to the first
                                           packet_copy_size . Current supported
                                           values for DNX are 0, 64, 128, 192 */
    uint16 egress_packet_copy_size;     /* If non zero and the packet is copied
                                           from the egress, the packet will be
                                           truncated to the first
                                           packet_copy_size . Current supported
                                           values for DNX are 0, 256. */
    bcm_mirror_pkt_header_updates_t packet_control_updates;
    bcm_mirror_psc_t rtag;              /* specify RTAG HASH algorithm used for
                                           shared-id mirror destination */
    uint8 df;                           /* Set the do not fragment bit of IP
                                           header in mirror encapsulation */
    uint8 truncate;                     /* Setting truncate would result in
                                           mirroring a truncated frame */
    uint16 template_id;                 /* Template ID for IPFIX HDR */
    uint32 observation_domain;          /* Observation domain for IPFIX HDR */
    uint32 is_recycle_strict_priority;  /* Recycle priority (1-lossless, 0-high) */
    int ext_stat_id[BCM_MIRROR_EXT_STAT_ID_COUNT]; /* ext_stat_id to support statistic
                                           interface for mirror packets. */
    uint32 flags2;                      /* See BCM_MIRROR_DEST_FLAGS2_xxx flag
                                           definitions. */
    uint32 vni;                         /* Virtual Network Interface ID. */
    uint32 gre_seq_number;              /* Sequence number value used in GRE
                                           header. If no value is provided,
                                           gre_seq_number will start with 0.
                                           Valid only if
                                           BCM_MIRROR_DEST_TUNNEL_IP_GRE is set. */
    bcm_mirror_pkt_erspan_encap_t erspan_header; /* ERSPAN encapsulation header fields.
                                           Valid only if only
                                           BCM_MIRROR_DEST_TUNNEL_IP_GRE is set. */
    uint32 initial_seq_number;          /* Starting seq number in SFLOW or PSAMP
                                           header */
    bcm_mirror_psamp_fmt2_meta_data_t meta_data_type; /* Type of item in 'meta_data' */
    uint16 meta_data;                   /* Actual value of class id or meta
                                           data, based on 'meta_data_type' */
    int ext_stat_valid;                 /* Validates the ext_stat. */
    int ext_stat_type_id[BCM_MIRROR_EXT_STAT_ID_COUNT]; /* Type ids for statistic interface. */
    uint16 ipfix_version;               /* IPFIX version number in PSAMP
                                           encapsulation. */
    uint16 psamp_epoch;                 /* PSAMP epoch field. */
    int cosq;                           /* Queue id that mirror copy is enqueued
                                           into. */
    uint8 cfi;                          /* CFI of VLAN tag. */
    bcm_mirror_timestamp_mode_t timestamp_mode; /* Set timestamp mode for mirror
                                           encapsulation. */
    bcm_mirror_multi_dip_group_t multi_dip_group; /* Multiple destination IP group. */
    int drop_group_bmp;
    bcm_port_t second_pass_src_port;    /* Source port in the second pass. */
    bcm_mirror_truncate_mode_t encap_truncate_mode; /* Truncate mode for mirror
                                           encapsulation packets. */
    int encap_truncate_profile_id;      /* Truncate length profile ID for mirror
                                           encapsulation packets. */
    bcm_mirror_loopback_header_type_t loopback_header_type; /* Loopback header type. */
    bcm_port_t second_pass_dst_port;    /* Destination port in the second pass. */
    bcm_mirror_int_probe_header_t int_probe_header; /* INT(Inband Network Telemetry) probe
                                          header for mirror encapsulation
                                          packets. */
    uint8 duplicate_pri;                /* The priority of duplicate mirror
                                          packets. The packet with highest
                                          priority (0 is lowest) will be
                                          mirrored and others will be dropped.
                                          Applicable only if duplication is not
                                          allowed. */
    int ip_proto;                       /* IP Protocol number in ERSPAN IP
                                          encap. */
} bcm_compat6520_mirror_destination_t;

/* Create a mirror (destination, encapsulation) pair. */
extern int bcm_compat6520_mirror_destination_create(
    int unit,
    bcm_compat6520_mirror_destination_t *mirror_dest);

/* Get  mirror (destination, encapsulation) pair. */
extern int bcm_compat6520_mirror_destination_get(
    int unit,
    bcm_gport_t mirror_dest_id,
    bcm_compat6520_mirror_destination_t *mirror_dest);

/* ECN traffic map info. */
typedef struct bcm_compat6520_ecn_traffic_map_info_s {
    uint32 flags;    /* BCM_ECN_TRAFFIC_MAP_XXX flag definitions. */
    uint8 ecn;    /* ECN value of packet's IP header. */
    int int_cn;    /* Mapped internal congestion notification(int_cn) value. */
    uint8 tunnel_ecn;    /* ECN information of packet's tunnel header. */
    bcm_ecn_traffic_map_tunnel_type_t tunnel_type;    /* Tunnel type. */
    bcm_ecn_traffic_map_type_t type;    /* Traffic map type bcmEcnTrafficMapTypeXXX to be configured. */
} bcm_compat6520_ecn_traffic_map_info_t;

/* To set/get the mapped internal congestion notification (int_cn). */
extern int bcm_compat6520_ecn_traffic_map_get(
    int unit,
    bcm_compat6520_ecn_traffic_map_info_t *map);

/* To set/get the mapped internal congestion notification (int_cn). */
extern int bcm_compat6520_ecn_traffic_map_set(
    int unit,
    bcm_compat6520_ecn_traffic_map_info_t *map);

/* PM stats structure */
typedef struct bcm_compat6520_oam_pm_stats_s {
    uint32 far_loss_min;                /* Antecedent of the minimum far loss
                                           ratio. */
    uint32 far_tx_min;                  /* Consequent of the minimum far loss
                                           ratio. */
    uint32 far_loss_max;                /* Antecedent of the maximum far loss
                                           ratio. */
    uint32 far_tx_max;                  /* Consequent of the maximum far loss
                                           ratio. */
    uint32 far_loss;                    /* Total far loss at far-end */
    uint32 near_loss_min;               /* Antecedent of the minimum near loss
                                           ratio. */
    uint32 near_tx_min;                 /* Consequent of the minimum near loss
                                           ratio. */
    uint32 near_loss_max;               /* Antecedent of the maximum near loss
                                           ratio. */
    uint32 near_tx_max;                 /* Consequent of the maximum near loss
                                           ratio. */
    uint32 near_loss;                   /* Total near loss at near-end */
    uint32 lm_tx_count;                 /* Count of LM packets transmitted from
                                           local node during sampling interval */
    uint32 DM_min;                      /* Minimum Frame delay */
    uint32 DM_max;                      /* Maximum Frame delay */
    uint32 DM_avg;                      /* Frame delay */
    uint32 dm_tx_count;                 /* Count of DM packets transmitted from
                                           local node during sampling interval */
    uint8 profile_id;                   /* Profile number associated with this
                                           session to measure the DM frames */
    uint32 bin_counters[BCM_OAM_MAX_PM_PROFILE_BIN_EDGES+1]; /* Bins Count of associated profile */
    uint32 lm_rx_count;                 /* Count of LM packets received in local
                                           node during sampling interval */
    uint32 dm_rx_count;                 /* Count of DM packets received in local
                                           node during sampling interval */
    uint32 far_total_tx_pkt_count;      /* Total Tx data packets at far-end */
    uint32 near_total_tx_pkt_count;     /* Total Tx data packets at near-end */
    uint32 flags;                       /* Flags related to PM processed stats */
} bcm_compat6520_oam_pm_stats_t;

/* Get the processed stats for a given endpoint */
extern int bcm_compat6520_oam_pm_stats_get(
    int unit,
    bcm_oam_endpoint_t endpoint_id,
    bcm_compat6520_oam_pm_stats_t *stats_ptr);

#if defined(INCLUDE_L3)
/* Flow encap config structure */
typedef struct bcm_compat6520_flow_encap_config_s {
    bcm_vpn_t vpn;    /* VPN representing a vfi or vrf */
    bcm_gport_t flow_port;    /* flow port id representing a DVP */
    bcm_if_t intf_id;    /* l3 interface id */
    uint32 dvp_group;    /* DVP group ID */
    uint32 oif_group;    /* L3 OUT interface group ID */
    uint32 vnid;    /* VN_ID. */
    uint8 pri;    /* service tag priority. */
    uint8 cfi;    /* service tag cfi */
    uint16 tpid;    /* service tag tpid */
    bcm_vlan_t vlan;    /* service VLAN ID. */
    uint32 flags;    /* BCM_FLOW_ENCAP_FLAG_xxx. */
    uint32 options;    /* BCM_FLOW_ENCAP_OPTION_xxx. */
    bcm_flow_encap_criteria_t criteria;    /* flow encap criteria. */
    uint32 valid_elements;    /* bitmap of valid fields for the encap action */
    bcm_flow_handle_t flow_handle;    /* flow handle derived from flow name */
    uint32 flow_option;    /* flow option derived from flow option name */
    bcm_gport_t src_flow_port;    /* flow port id representing a SVP */
    uint32 class_id;    /* Class ID as a key in EFP. */
    uint32 port_group;    /* Port group ID. */
} bcm_compat6520_flow_encap_config_t;

/* Add a flow encap entry for L2 tunnel or L3 tunnel. */
extern int bcm_compat6520_flow_encap_add(
    int unit,
    bcm_compat6520_flow_encap_config_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field);

/* Delete a flow encap entry based on the given key.  */
extern int bcm_compat6520_flow_encap_delete(
    int unit,
    bcm_compat6520_flow_encap_config_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field);

/* Get a flow encap entry based on the given key.  */
extern int bcm_compat6520_flow_encap_get(
    int unit,
    bcm_compat6520_flow_encap_config_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field);

/* Set the flex counter object value to the encap entry. */
extern int bcm_compat6520_flow_encap_flexctr_object_set(
    int unit,
    bcm_compat6520_flow_encap_config_t *info,
    uint32 value);

/* Get the flex counter object value from the encap entry. */
extern int bcm_compat6520_flow_encap_flexctr_object_get(
    int unit,
    bcm_compat6520_flow_encap_config_t *info,
    uint32 *value);

/* Attach counter entries to the encap entry. */
extern int bcm_compat6520_flow_encap_stat_attach(
    int unit,
    bcm_compat6520_flow_encap_config_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field,
    uint32 stat_counter_id);

/* Detach the counter entries from the encap entry. */
extern int bcm_compat6520_flow_encap_stat_detach(
    int unit,
    bcm_compat6520_flow_encap_config_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field);

/* Get stat counter ID associated to the encap entry. */
extern int bcm_compat6520_flow_encap_stat_id_get(
    int unit,
    bcm_compat6520_flow_encap_config_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field,
    uint32 *stat_counter_id);


#endif /* defined(INCLUDE_L3) */

/* Field destination match. */
typedef struct bcm_compat6520_field_destination_match_s {
    int priority;    /* Match priority. */
    bcm_gport_t gport;    /* Outgoing mod port or trunk. */
    uint32 gport_mask;    /* Outgoing mod port or trunk mask. */
    bcm_multicast_t mc_group;    /* Multicast group. */
    uint32 mc_group_mask;    /* Multicast group mask. */
    bcm_port_encap_t port_encap;    /* Incoming port encapsulation. */
    uint8 port_encap_mask;    /* Incoming port encapsulation mask. */
    bcm_color_t color;    /* Color. */
    uint8 color_mask;    /* Color mask. */
    bcm_color_t elephant_color;    /* Elephant color. */
    uint8 elephant_color_mask;    /* Elephant color mask. */
    uint8 elephant;    /* Flowtracker elephant flow indicator. */
    uint8 elephant_mask;    /* Elephant mask. */
    uint8 elephant_notify;    /* Indicator that flow exceeds the elephant threshold byte count and becomes an elephant flow. */
    uint8 elephant_notify_mask;    /* elephant_notify mask. */
    int int_pri;    /* Internal priority. */
    uint32 int_pri_mask;    /* Internal priority mask. */
    int opaque_object_2;    /* Opaque object 2 assigned in ingress field. */
    uint32 opaque_object_2_mask;    /* Opaque object 2 mask. */
    uint32 ifp_opaque_ctrl_id;    /* IFP opaque control ID. */
    uint32 ifp_opaque_ctrl_id_mask;    /* IFP opaque control ID mask. */
} bcm_compat6520_field_destination_match_t;

/* Add a field destination entry. */
extern int bcm_compat6520_field_destination_entry_add(
    int unit,
    uint32 options,
    bcm_compat6520_field_destination_match_t *match,
    bcm_field_destination_action_t *action);

/* Get a field destination entry. */
extern int bcm_compat6520_field_destination_entry_get(
    int unit,
    bcm_compat6520_field_destination_match_t *match,
    bcm_field_destination_action_t *action);

/* Delete a field destination entry. */
extern int bcm_compat6520_field_destination_entry_delete(
    int unit,
    bcm_compat6520_field_destination_match_t *match);

/* Attach the statistic counter ID to a field destination entry. */
extern int bcm_compat6520_field_destination_stat_attach(
    int unit,
    bcm_compat6520_field_destination_match_t *match,
    uint32 stat_counter_id);

/* Get the statistic counter ID attached to a field destination entry. */
extern int bcm_compat6520_field_destination_stat_id_get(
    int unit,
    bcm_compat6520_field_destination_match_t *match,
    uint32 *stat_counter_id);

/* Detach the statistic counter ID attached to a field destination entry. */
extern int bcm_compat6520_field_destination_stat_detach(
    int unit,
    bcm_compat6520_field_destination_match_t *match);

/* Set the flex counter object value for a field destination entry. */
extern int bcm_compat6520_field_destination_flexctr_object_set(
    int unit,
    bcm_compat6520_field_destination_match_t *match,
    uint32 value);

/* Get the flex counter object value of a field destination entry. */
extern int bcm_compat6520_field_destination_flexctr_object_get(
    int unit,
    bcm_compat6520_field_destination_match_t *match,
    uint32 *value);

/* Flowtracker tracking parameter information. */
typedef struct bcm_compat6520_flowtracker_tracking_param_info_s {
    uint32 flags;                       /* Flags for tracking parameters. */
    bcm_flowtracker_tracking_param_type_t param; /* Type of tracking parameter. */
    bcm_flowtracker_tracking_param_user_data_t tracking_data; /* Element data for the tracking param
                                           to be used to add a flow. */
    bcm_flowtracker_tracking_param_mask_t mask; /* Mask to select granular information
                                           from tracking parameter. By default,
                                           mask is set to all enabled. */
    bcm_flowtracker_check_t check_id;   /* Flowtracker check to be used to
                                           tracking flow check data. */
    bcm_flowtracker_timestamp_source_t src_ts; /* Timestamp source info, valid in case
                                           of timestamp and delay related
                                           tracking params. */
    bcm_udf_id_t udf_id;                /* UDF Id associated with the tracking
                                           param. */
    bcm_flowtracker_direction_t direction; /* Direction of flow from where tracking
                                           parameter is picked. */
    uint16 custom_id;                   /* Custom Identifier for Flowtracker
                                           Tracking parameter type custom. */
} bcm_compat6520_flowtracker_tracking_param_info_t;

/* Flowtracker check information. */
typedef struct bcm_compat6520_flowtracker_check_info_s {
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
    uint32 state_transition_flags;      /* State Transition flags */
    uint16 num_drop_reason_group_id;    /* Number of drop reason group id. */
    bcm_flowtracker_drop_reason_group_t drop_reason_group_id[BCM_FLOWTRACKER_DROP_REASON_GROUP_ID_MAX]; /* Flowtracker drop reason group ids.
                                           Applicable only if tracking param is
                                           IngDropReasonGroupIdVector or
                                           EgrDropReasonGroupIdVector. */
    uint16 custom_id;                   /* Custom Identifier for Flowtracker
                                           Tracking parameter type custom. */
} bcm_compat6520_flowtracker_check_info_t;

/* Flowtracker export template elements information. */
typedef struct bcm_comat6520_flowtracker_export_element_info_s {
    uint32 flags;                       /* See
                                           BCM_FLOWTRACKER_EXPORT_ELEMENT_FLAGS_XXX. */
    bcm_flowtracker_export_element_type_t element; /* Type of export element. */
    uint32 data_size;                   /* Size of information element in the
                                           export record in bytes. If the
                                           data_size is given as 0, then the
                                           default RFC size is used. */
    uint16 info_elem_id;                /* Information element to be used, when
                                           the template set is exported. */
    bcm_flowtracker_check_t check_id;   /* Flowtracker Check Id for exporting
                                           check data. */
    bcm_flowtracker_direction_t direction; /* Direction of exporting data element. */
    uint16 custom_id;                   /* Custom Identifier for Flowtracker
                                           Export Element type custom. */
} bcm_compat6520_flowtracker_export_element_info_t;

/* Flowtracker Export record data information. */
typedef struct bcm_compat6520_flowtracker_export_record_data_info_s {
    bcm_compat6520_flowtracker_export_element_info_t export_element_info; /* Flowtracker Export element
                                           information. */
    uint8 data[BCM_FLOWTRACKER_EXPORT_RECORD_DATA_MAX_LENGTH]; /* Raw data in network byte order. */
} bcm_compat6520_flowtracker_export_record_data_info_t;

/* Set tracking parameter for this flowtracker group. */
extern int bcm_compat6520_flowtracker_group_tracking_params_set(
    int unit,
    bcm_flowtracker_group_t id,
    int num_tracking_params,
    bcm_compat6520_flowtracker_tracking_param_info_t *list_of_tracking_params);

/* Get flowtracker tracking parameters for this group. */
extern int bcm_compat6520_flowtracker_group_tracking_params_get(
    int unit,
    bcm_flowtracker_group_t id,
    int max_size,
    bcm_compat6520_flowtracker_tracking_param_info_t *list_of_tracking_params,
    int *list_size);

/* Create a flowtracker flow group. */
extern int bcm_compat6520_flowtracker_check_create(
    int unit,
    uint32 options,
    bcm_compat6520_flowtracker_check_info_t check_info,
    bcm_flowtracker_check_t *check_id);

/* Get a flowtracker flow check. */
extern int bcm_compat6520_flowtracker_check_get(
    int unit,
    bcm_flowtracker_check_t check_id,
    bcm_compat6520_flowtracker_check_info_t *check_info);

/* Validate the template and return the list supported by the device. */
extern int bcm_compat6520_flowtracker_export_template_validate(
    int unit,
    bcm_flowtracker_group_t flow_group_id,
    int max_in_export_elements,
    bcm_compat6520_flowtracker_export_element_info_t *in_export_elements,
    int max_out_export_elements,
    bcm_compat6520_flowtracker_export_element_info_t *out_export_elements,
    int *actual_out_export_elements);

/* Create a flowtracker export template. */
extern int bcm_compat6520_flowtracker_export_template_create(
    int unit,
    uint32 options,
    bcm_flowtracker_export_template_t *id,
    uint16 set_id,
    int num_export_elements,
    bcm_compat6520_flowtracker_export_element_info_t *list_of_export_elements);

/* Get a flowtracker export template. */
extern int bcm_compat6520_flowtracker_export_template_get(
    int unit,
    bcm_flowtracker_export_template_t id,
    uint16 *set_id,
    int max_size,
    bcm_compat6520_flowtracker_export_element_info_t *list_of_export_elements,
    int *list_size);

/*
 * Add a user flow entry basis user input key elements. API expects that
 * all tracking parametrs of type = 'KEY' in the group are specified.
 */
extern int bcm_compat6520_flowtracker_user_entry_add(
    int unit,
    bcm_flowtracker_group_t flow_group_id,
    uint32 options,
    int num_user_entry_params,
    bcm_compat6520_flowtracker_tracking_param_info_t *user_entry_param_list,
    bcm_flowtracker_user_entry_handle_t *entry_handle);

/* Fetch user entry info given the entry handle. */
extern int bcm_compat6520_flowtracker_user_entry_get(
    int unit,
    bcm_flowtracker_user_entry_handle_t *entry_handle,
    int num_user_entry_params,
    bcm_compat6520_flowtracker_tracking_param_info_t *user_entry_param_list,
    int *actual_user_entry_params);

/* To get data from raw export record. */
extern int bcm_compat6520_flowtracker_export_record_data_info_get(
    int unit,
    bcm_flowtracker_export_record_t *record,
    int max_elements,
    bcm_compat6520_flowtracker_export_element_info_t *export_element_info,
    bcm_compat6520_flowtracker_export_record_data_info_t *data_info,
    int *count);

/* In-band flow analyzer collector information. */
typedef struct bcm_compat6520_ifa_collector_info_s {
    bcm_ifa_collector_transport_type_t transport_type;    /* Transport type used for sending data to the collector. */
    bcm_ifa_collector_eth_header_t eth;    /* Ethernet encapsulation of the packet sent to collector. */
    bcm_ifa_collector_ipv4_header_t ipv4;    /* IPv4 encapsulation of the packet sent to collector. */
    bcm_ifa_collector_ipv6_header_t ipv6;    /* IPv6 encapsulation of the packet sent to collector. */
    bcm_ifa_collector_udp_header_t udp;    /* UDP encapsulation of the packet sent to collector. */
    uint16 mtu;    /* The maximum packet length transmitted by collector. */
} bcm_compat6520_ifa_collector_info_t;

/* Create a In-band Flow Analyzer collector. */
extern int bcm_compat6520_ifa_collector_set(
    int unit,
    uint32 options,
    bcm_compat6520_ifa_collector_info_t *collector_info);

/* Get a In-band Flow Analyzer collector. */
extern int bcm_compat6520_ifa_collector_get(
    int unit,
    bcm_compat6520_ifa_collector_info_t *collector_info);

#define BCM_COMPAT6520_FLEXSTATE_OPERATION_OBJECT_SIZE 2          /* Operation object array
                                                          size number. */

/*
 * Flex state index operation structure.
 *
 * This data structure lists the operation of a flex state index.
 * Flex state index would be generated as below:
 *      value0 = (SEL(object0) >> shift0) & ((1 << mask_size0) - 1)).
 *      value1 = (SEL(object1) & ((1 << mask_size1) - 1)) << shift1.
 *      index = (value1 | value0).
 */
typedef struct bcm_compat6520_flexstate_index_operation_s {
    bcm_flexstate_object_t object[BCM_COMPAT6520_FLEXSTATE_OPERATION_OBJECT_SIZE]; /* Flex state object array. */
    uint32 quant_id[BCM_COMPAT6520_FLEXSTATE_OPERATION_OBJECT_SIZE]; /* Quantization ID. */
    uint8 mask_size[BCM_COMPAT6520_FLEXSTATE_OPERATION_OBJECT_SIZE]; /* Object mask size array. */
    uint8 shift[BCM_COMPAT6520_FLEXSTATE_OPERATION_OBJECT_SIZE]; /* Object shift array. */
} bcm_compat6520_flexstate_index_operation_t;

/*
 * Flex state value operation structure.
 *
 * This data structure lists the operation of a flex state update value.
 * Flex state update value would be generated as below:
 *      value0 = (SEL(object0) >> shift0) & ((1 << mask_size0) - 1)).
 *      value1 = (SEL(object1) & ((1 << mask_size1) - 1)) << shift1.
 *      value = (value1 | value0).
 *      New_counter = UPDATE_TYPE(old_counter, value).
 */
typedef struct bcm_compat6520_flexstate_value_operation_s {
    bcm_flexstate_value_select_t select; /* Select state value. */
    bcm_flexstate_object_t object[BCM_COMPAT6520_FLEXSTATE_OPERATION_OBJECT_SIZE]; /* Flex state object array. */
    uint32 quant_id[BCM_COMPAT6520_FLEXSTATE_OPERATION_OBJECT_SIZE]; /* Quantization ID. */
    uint8 mask_size[BCM_COMPAT6520_FLEXSTATE_OPERATION_OBJECT_SIZE]; /* Object mask size array. */
    uint8 shift[BCM_COMPAT6520_FLEXSTATE_OPERATION_OBJECT_SIZE]; /* Object shift array. */
    bcm_flexstate_value_operation_type_t type; /* Counter value operation type. */
} bcm_compat6520_flexstate_value_operation_t;

/*
 * Flex state trigger structure.
 *
 * This data structure lists the properties of the flex state trigger.
 */
typedef struct bcm_compat6520_flexstate_trigger_s {
    bcm_flexstate_trigger_type_t trigger_type; /* Trigger type. */
    uint8 period_num;                   /* Time trigger period number. */
    bcm_flexstate_trigger_interval_t interval; /* Time trigger interval scale. */
    bcm_flexstate_object_t object;      /* Flex state object. */
    uint16 object_value_start;          /* Conditional trigger start value. */
    uint16 object_value_stop;           /* Conditional trigger stop value. */
    uint16 object_value_mask;           /* Conditional trigger mask. */
} bcm_compat6520_flexstate_trigger_t;

/* Flex state operation structure. */
typedef struct bcm_compat6520_flexstate_operation_s {
    bcm_flexstate_update_compare_t update_compare; /* Comparison operation in the
                                           conditional update logic. */
    bcm_compat6520_flexstate_value_operation_t compare_operation; /* Comparison value in the conditional
                                           update logic. */
    bcm_compat6520_flexstate_value_operation_t operation_true; /* Update value when conditional update
                                           logic returns true. */
    bcm_compat6520_flexstate_value_operation_t operation_false; /* Update value when conditional update
                                           logic returns false. */
} bcm_compat6520_flexstate_operation_t;

/*
 * Flex state action structure.
 *
 * This data structure lists the properties of a flex state action.
 */
typedef struct bcm_compat6520_flexstate_action_s {
    uint32 flags;                       /* Action flags. */
    bcm_flexstate_source_t source;      /* Flex state source. */
    bcm_pbmp_t ports;                   /* Flex state ports. */
    int hint;                           /* Flex state hint. */
    bcm_flexstate_drop_count_mode_t drop_count_mode; /* Counter drop mode for functional
                                           packet drops. */
    int exception_drop_count_enable;    /* Enable to count on hardware exception
                                           drops. */
    int egress_mirror_count_enable;     /* Enable to count egress mirrored
                                           packets also. */
    bcm_flexstate_counter_mode_t mode;  /* Action mode. */
    uint32 index_num;                   /* Total counter index number. */
    bcm_compat6520_flexstate_index_operation_t index_operation; /* Flex state index operation structure. */
    bcm_compat6520_flexstate_operation_t operation_a; /* Flex state value A operation
                                           structure. */
    bcm_compat6520_flexstate_operation_t operation_b; /* Flex state value B operation
                                           structure. */
    bcm_compat6520_flexstate_trigger_t trigger;    /* Flex state trigger structure. */
} bcm_compat6520_flexstate_action_t;

/* Flex counter action creation function. */
extern int bcm_compat6520_flexstate_action_create(
    int unit,
    int options,
    bcm_compat6520_flexstate_action_t *action,
    uint32 *action_id);

/* Flex counter action get function. */
extern int bcm_compat6520_flexstate_action_get(
    int unit,
    uint32 action_id,
    bcm_compat6520_flexstate_action_t *action);

/*
 * Flex state quantization structure.
 *
 * This data structure lists the properties of a flex state quantization.
 * Each quantization instance generates a range check result.
 */
typedef struct bcm_compat6520_flexstate_quantization_s {
    bcm_flexstate_object_t object;      /* Flex state object. */
    bcm_pbmp_t ports;                   /* Flex state ports. */
    uint32 range_check_min[BCM_FLEXSTATE_QUANTIZATION_RANGE_SIZE]; /* Range check min value array. */
    uint32 range_check_max[BCM_FLEXSTATE_QUANTIZATION_RANGE_SIZE]; /* Range check max value array. */
    uint8 range_num;                    /* Total range check number */
} bcm_compat6520_flexstate_quantization_t;

/* Flex counter quantization creation function. */
extern int bcm_compat6520_flexstate_quantization_create(
    int unit,
    int options,
    bcm_compat6520_flexstate_quantization_t *quantization,
    uint32 *quant_id);

/* Flex counter quantization get function. */
extern int bcm_compat6520_flexstate_quantization_get(
    int unit,
    uint32 quant_id,
    bcm_compat6520_flexstate_quantization_t *quantization);

/* Port Configuration structure. */
typedef struct bcm_compat6520_port_config_s {
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
    bcm_pbmp_t control;                 /* Mask of hot swap controlling ports.*/
    bcm_pbmp_t eventor;                 /* Mask of Eventor ports. */
    bcm_pbmp_t olp;                     /* Mask of OLP ports. */
    bcm_pbmp_t oamp;                    /* Mask of OAMP ports. */
    bcm_pbmp_t erp;                     /* Mask of ERP ports. */
    bcm_pbmp_t roe;                     /* Mask of ROE ports. */
    bcm_pbmp_t rcy_mirror;              /* Mask of mirror recycle port (sniff)*/
    bcm_pbmp_t per_pp_pipe[BCM_PP_PIPES_MAX];   /* Mask of ports per PP pipe.
                                                   The number of PP pipes per
                                                   device can be obtained via
                                                   num_pp_pipes field of
                                                   bcm_info_t. */
    bcm_pbmp_t stat;                    /* Mask of stat ports. */
    bcm_pbmp_t crps;                    /* Mask of crps ports. */
} bcm_compat6520_port_config_t;

/* Retrieved the port configuration for the specified device. */
extern int bcm_compat6520_port_config_get(
    int unit,
    bcm_compat6520_port_config_t *config);

#if defined(INCLUDE_L3)
/* Flow tunneling initiator structure. */
typedef struct bcm_compat6520_flow_tunnel_initiator_s {
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
} bcm_compat6520_flow_tunnel_initiator_t;

/* Create a flow tunnel initiator object */
extern int bcm_compat6520_flow_tunnel_initiator_create(
    int unit,
    bcm_compat6520_flow_tunnel_initiator_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field);

/* Get the flow tunnel initiator object */
extern int bcm_compat6520_flow_tunnel_initiator_get(
    int unit,
    bcm_compat6520_flow_tunnel_initiator_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field);

/* Set the MPLS tunnel initiator parameters for an L3 interface. */
extern int bcm_compat6520_mpls_tunnel_initiator_set(
    int unit,
    bcm_if_t intf,
    int num_labels,
    bcm_compat6520_mpls_egress_label_t *label_array);

/* bcm_mpls_tunnel_initiator_create */
extern int bcm_compat6520_mpls_tunnel_initiator_create(
    int unit,
    bcm_if_t intf,
    int num_labels,
    bcm_compat6520_mpls_egress_label_t *label_array);

/* Get the MPLS tunnel initiator parameters from an L3 interface. */
extern int bcm_compat6520_mpls_tunnel_initiator_get(
    int unit,
    bcm_if_t intf,
    int label_max,
    bcm_compat6520_mpls_egress_label_t *label_array,
    int *label_count);

/* MPLS tunnel initiator structure. */
typedef struct bcm_compat6520_mpls_tunnel_encap_s {
    bcm_gport_t tunnel_id;    /* Tunnel ID. */
    uint32 flags;    /* BCM_MPLS_TUNNEL_ENCAP_xxx. */
    int num_labels;    /* Number of labels to be added. */
    bcm_compat6520_mpls_egress_label_t label_array[BCM_MPLS_EGRESS_LABEL_MAX];    /* Label info. */
    int qos_map_id;    /* EXP remarking map ID. */
} bcm_compat6520_mpls_tunnel_encap_t;

/* Create the MPLS tunnel initiator. */
extern int bcm_compat6520_mpls_tunnel_encap_create(
    int unit,
    uint32 options,
    bcm_compat6520_mpls_tunnel_encap_t *tunnel_encap);

/* Get the MPLS tunnel initiator. */
extern int bcm_compat6520_mpls_tunnel_encap_get(
    int unit,
    bcm_compat6520_mpls_tunnel_encap_t *tunnel_encap);


/* MPLS tunnel switch structure. */
typedef struct bcm_compat6520_mpls_tunnel_switch_s {
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
    bcm_compat6520_mpls_egress_label_t egress_label;    /* Outgoing label information. */
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
} bcm_compat6520_mpls_tunnel_switch_t;

/* Add an MPLS label entry. */
extern int bcm_compat6520_mpls_tunnel_switch_add(
    int unit,
    bcm_compat6520_mpls_tunnel_switch_t *info);

/* Add an MPLS label entry. */
extern int bcm_compat6520_mpls_tunnel_switch_create(
    int unit,
    bcm_compat6520_mpls_tunnel_switch_t *info);

/* Delete an MPLS label entry. */
extern int bcm_compat6520_mpls_tunnel_switch_delete(
    int unit,
    bcm_compat6520_mpls_tunnel_switch_t *info);

/* Get an MPLS label entry. */
extern int bcm_compat6520_mpls_tunnel_switch_get(
    int unit,
    bcm_compat6520_mpls_tunnel_switch_t *info);


/* MPLS port type. */
typedef struct bcm_compat6520_mpls_port_s {
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
    bcm_compat6520_mpls_egress_label_t egress_label;    /* Outgoing VC label. */
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
    bcm_compat6520_mpls_egress_label_t egress_tunnel_label;    /* A mpls tunnel to be binded with the pwe */
    int nof_service_tags;    /* Number of Service-Tags expected on Native-ETH header */
    bcm_qos_ingress_model_t ingress_qos_model;    /* ingress qos&ttl model */
    bcm_mpls_label_t context_label;    /* Ingress match - context label. */
    bcm_if_t ingress_if;    /* Ingress match - context interface. */
    int port_group;    /* Port group to match. */
    int dscp_map_id;    /* DSCP based PHB map ID. */
    int vlan_pri_map_id;    /* VLAN priority and CFI based PHB map ID. */
} bcm_compat6520_mpls_port_t;

/* Add an MPLS port to an L2 VPN. */
extern int bcm_compat6520_mpls_port_add(
    int unit,
    bcm_vpn_t vpn,
    bcm_compat6520_mpls_port_t *mpls_port);

/* Get an MPLS port from an L2 VPN. */
extern int bcm_compat6520_mpls_port_get(
    int unit,
    bcm_vpn_t vpn,
    bcm_compat6520_mpls_port_t *mpls_port);

/* Get all MPLS ports from an L2 VPN. */
extern int bcm_compat6520_mpls_port_get_all(
    int unit,
    bcm_vpn_t vpn,
    int port_max,
    bcm_compat6520_mpls_port_t *port_array,
    int *port_count);


/* L3 tunneling initiator. */
typedef struct bcm_compat6520_tunnel_initiator_s {
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
    bcm_qos_egress_model_t egress_qos_model;    /* egress qos and ttl model */
    int qos_map_id;    /* general remark profile */
    uint8 ecn;    /* Tunnel header ECN value. */
    bcm_tunnel_flow_label_select_t flow_label_sel;    /* Tunnel header flow label select. */
    bcm_tunnel_dscp_ecn_select_t dscp_ecn_sel;    /* Tunnel header DSCP and ECN select. */
    int dscp_ecn_map;    /* DSCP and ECN map ID. */
    int stat_cmd;    /* Counter ID associated with the the tunnel. */
    uint8 counter_command_id;    /* Counter interface associated with the tunnel. */
} bcm_compat6520_tunnel_initiator_t;

/* Set the tunneling initiator parameters on an L3 interface. */
extern int bcm_compat6520_tunnel_initiator_set(
    int unit,
    bcm_l3_intf_t *intf,
    bcm_compat6520_tunnel_initiator_t *tunnel);

/* Set the tunneling initiator parameters on an L3 interface. */
extern int bcm_compat6520_tunnel_initiator_create(
    int unit,
    bcm_l3_intf_t *intf,
    bcm_compat6520_tunnel_initiator_t *tunnel);

/* Get the tunnel property for the given L3 interface. */
extern int bcm_compat6520_tunnel_initiator_get(
    int unit,
    bcm_l3_intf_t *intf,
    bcm_compat6520_tunnel_initiator_t *tunnel);

/* Create VXLAN Tunnel Initiator.  */
extern int bcm_compat6520_vxlan_tunnel_initiator_create(
    int unit,
    bcm_compat6520_tunnel_initiator_t *info);

/* Get  VXLAN Tunnel Initiator.  */
extern int bcm_compat6520_vxlan_tunnel_initiator_get(
    int unit,
    bcm_compat6520_tunnel_initiator_t *info);

/* Set the tunneling initiator parameters for a WLAN tunnel. */
extern int bcm_compat6520_wlan_tunnel_initiator_create(
    int unit,
    bcm_compat6520_tunnel_initiator_t *info);

/* Get the tunnel properties for an outgoing WLAN tunnel. */
extern int bcm_compat6520_wlan_tunnel_initiator_get(
    int unit,
    bcm_compat6520_tunnel_initiator_t *info);

/* Create L2GRE Tunnel Initiator.  */
extern int bcm_compat6520_l2gre_tunnel_initiator_create(
    int unit,
    bcm_compat6520_tunnel_initiator_t *info);

/* Get  L2GRE Tunnel Initiator.  */
extern int bcm_compat6520_l2gre_tunnel_initiator_get(
    int unit,
    bcm_compat6520_tunnel_initiator_t *info);

#endif /* defined(INCLUDE_L3) */
/* Flex counter action structure.

This data structure lists the properties of a flex counter action.
 */
typedef struct bcm_compat6520_flexctr_action_s {
    uint32 flags;    /* Action flags. */
    bcm_flexctr_source_t source;    /* Flex counter source. */
    bcm_pbmp_t ports;    /* Flex counter ports. */
    int hint;    /* Flex counter hint. */
    int hint_ext;    /* Flex counter hint extension. */
    bcm_flexctr_drop_count_mode_t drop_count_mode;    /* Counter drop mode for functional packet drops. */
    int exception_drop_count_enable;    /* Enable to count on hardware exception drops. */
    int egress_mirror_count_enable;    /* Enable to count egress mirrored packets also. */
    bcm_flexctr_counter_mode_t mode;    /* Action mode. */
    uint32 index_num;    /* Total counter index number. */
    bcm_flexctr_index_operation_t index_operation;    /* Flex counter index operation structure. */
    bcm_flexctr_value_operation_t operation_a;    /* Flex counter value A operation structure. */
    bcm_flexctr_value_operation_t operation_b;    /* Flex counter value B operation structure. */
    bcm_flexctr_trigger_t trigger;    /* Flex counter trigger structure. */
    bcm_flexctr_hint_type_t hint_type;    /* Hint type. */
} bcm_compat6520_flexctr_action_t;

/* Flex counter action creation function. */
extern int bcm_compat6520_flexctr_action_create(
    int unit,
    int options,
    bcm_compat6520_flexctr_action_t *action,
    uint32 *stat_counter_id);

/* Flex counter action get function. */
extern int bcm_compat6520_flexctr_action_get(
    int unit,
    uint32 stat_counter_id,
    bcm_compat6520_flexctr_action_t *action);


#if defined(INCLUDE_L3)
/* L3 tunneling terminator. */
typedef struct bcm_compat6520_tunnel_terminator_s {
    uint32 flags;    /* Configuration flags. */
    uint32 multicast_flag;    /* VXLAN Multicast trigger flags. */
    bcm_vrf_t vrf;    /* Virtual router instance. */
    bcm_ip_t sip;    /* SIP for tunnel header match. */
    bcm_ip_t dip;    /* DIP for tunnel header match. */
    bcm_ip_t sip_mask;    /* Source IP mask. */
    bcm_ip_t dip_mask;    /* Destination IP mask. */
    bcm_ip6_t sip6;    /* SIP for tunnel header match (IPv6). */
    bcm_ip6_t dip6;    /* DIP for tunnel header match (IPv6). */
    bcm_ip6_t sip6_mask;    /* Source IP mask (IPv6). */
    bcm_ip6_t dip6_mask;    /* Destination IP mask (IPv6). */
    uint32 udp_dst_port;    /* UDP dst port for UDP packets. */
    uint32 udp_src_port;    /* UDP src port for UDP packets */
    bcm_tunnel_type_t type;    /* Tunnel type */
    bcm_pbmp_t pbmp;    /* Port bitmap for this tunnel */
    bcm_vlan_t vlan;    /* The VLAN ID or L3 ingress interface ID for IPMC lookup or WLAN tunnel */
    bcm_vlan_t fwd_vlan;    /* VLAN ID for forwarding terminated IP Tunnel */
    bcm_gport_t remote_port;    /* Remote termination */
    bcm_gport_t tunnel_id;    /* Tunnel id */
    int if_class;    /* L3 interface class this tunnel. */
    bcm_multicast_t failover_mc_group;    /* MC group used for bi-cast. */
    bcm_failover_t ingress_failover_id;    /* 1+1 protection. */
    bcm_failover_t failover_id;    /* Failover Object Identifier for protected tunnel. */
    bcm_gport_t failover_tunnel_id;    /* Failover Tunnel ID. */
    bcm_if_t tunnel_if;    /* hierarchical interface. */
    int tunnel_class;    /* Tunnel class id for VFP match. */
    int qos_map_id;    /* QoS DSCP map this tunnel. */
    int inlif_counting_profile;    /* In LIF counting profile */
    int tunnel_term_ecn_map_id;    /* Tunnel termination ecn map id. */
    bcm_qos_ingress_model_t ingress_qos_model;    /* ingress qos and ttl model */
    int priority;    /* Entry priority. */
    int subport_pbmp_profile_id;    /* profile id for allowed subports on this tunnel. */
    bcm_if_t egress_if;    /* Tunnel egress interface */
    bcm_vrf_t default_vrf;    /* default virtual router instance. */
    uint32 sci;    /* secure context index. */
} bcm_compat6520_tunnel_terminator_t;

/* Add a tunnel terminator for DIP-SIP key. */
extern int bcm_compat6520_tunnel_terminator_add(
    int unit,
    bcm_compat6520_tunnel_terminator_t *info);

/* Add a tunnel terminator for DIP-SIP key. */
extern int bcm_compat6520_tunnel_terminator_create(
    int unit,
    bcm_compat6520_tunnel_terminator_t *info);

/* Delete a tunnel terminator for DIP-SIP key. */
extern int bcm_compat6520_tunnel_terminator_delete(
    int unit,
    bcm_compat6520_tunnel_terminator_t *info);

/* Update a tunnel terminator for DIP-SIP key. */
extern int bcm_compat6520_tunnel_terminator_update(
    int unit,
    bcm_compat6520_tunnel_terminator_t *info);

/* Get a tunnel terminator for DIP-SIP key. */
extern int bcm_compat6520_tunnel_terminator_get(
    int unit,
    bcm_compat6520_tunnel_terminator_t *info);

/* Attach counters entries to the given tunnel. */
extern int bcm_compat6520_tunnel_terminator_stat_attach(
    int unit,
    bcm_compat6520_tunnel_terminator_t *terminator,
    uint32 stat_counter_id);

/* Detach counters entries to the given tunnel. */
extern int bcm_compat6520_tunnel_terminator_stat_detach(
    int unit,
    bcm_compat6520_tunnel_terminator_t *terminator);

/* Get stat counter id associated with given tunnel. */
extern int bcm_compat6520_tunnel_terminator_stat_id_get(
    int unit,
    bcm_compat6520_tunnel_terminator_t *terminator,
    uint32 *stat_counter_id);

/* Set flex counter object value for the given tunnel. */
extern int bcm_compat6520_tunnel_terminator_flexctr_object_set(
    int unit,
    bcm_compat6520_tunnel_terminator_t *terminator,
    uint32 value);

/* Get flex counter object value for the given tunnel. */
extern int bcm_compat6520_tunnel_terminator_flexctr_object_get(
    int unit,
    bcm_compat6520_tunnel_terminator_t *terminator,
    uint32 *value);

/* Create VXLAN Tunnel terminator. */
extern int bcm_compat6520_vxlan_tunnel_terminator_create(
    int unit,
    bcm_compat6520_tunnel_terminator_t *info);

/* Update multicast/active state per VXLAN Tunnel terminator. */
extern int bcm_compat6520_vxlan_tunnel_terminator_update(
    int unit,
    bcm_compat6520_tunnel_terminator_t *info);

/* Get VXLAN Tunnel terminator. */
extern int bcm_compat6520_vxlan_tunnel_terminator_get(
    int unit,
    bcm_compat6520_tunnel_terminator_t *info);

/* Create L2GRE Tunnel terminator. */
extern int bcm_compat6520_l2gre_tunnel_terminator_create(
    int unit,
    bcm_compat6520_tunnel_terminator_t *info);

/* Update L2GRE Tunnel terminator. */
extern int bcm_compat6520_l2gre_tunnel_terminator_update(
    int unit,
    bcm_compat6520_tunnel_terminator_t *info);

/* Get L2GRE Tunnel terminator. */
extern int bcm_compat6520_l2gre_tunnel_terminator_get(
    int unit,
    bcm_compat6520_tunnel_terminator_t *info);


#endif /* defined(INCLUDE_L3) */

/* Collector information. */
typedef struct bcm_compat6520_collector_info_s {
    bcm_collector_type_t collector_type;    /* Collector type. Remote vs Local */
    bcm_collector_transport_type_t transport_type;    /* Transport type used for exporting data to the collector. This identifies the usable fields within the encap structure member defined below. */
    bcm_collector_eth_header_t eth;    /* Ethernet encapsulation of the packet sent to collector. */
    bcm_collector_ipv4_header_t ipv4;    /* IPv4 encapsulation of the packet sent to collector. */
    bcm_collector_ipv6_header_t ipv6;    /* IPv6 encapsulation of the packet sent to collector. */
    bcm_collector_udp_header_t udp;    /* UDP encapsulation of the packet sent to collector. */
    bcm_collector_ipfix_header_t ipfix;    /* IPFIX encapsulation of the packet sent to collector. */
    bcm_collector_protobuf_header_t protobuf;    /* Protobuf header information. */
    bcm_collector_timestamp_source_t src_ts;    /* Timestamp source info. */
    int max_records_reserve;    /* Maximum number of export record entries reserved in the hardware FIFO for the collector. */
} bcm_compat6520_collector_info_t;

/* Create a collector. */
extern int bcm_compat6520_collector_create(
    int unit,
    uint32 options,
    bcm_collector_t *collector_id,
    bcm_compat6520_collector_info_t *collector_info);

/* Gets a Collector information. */
extern int bcm_compat6520_collector_get(
    int unit,
    bcm_collector_t id,
    bcm_compat6520_collector_info_t *collector_info);

#if defined(INCLUDE_L3)
/* flow port encap config structure. */
typedef struct bcm_compat6520_flow_port_encap_s {
    bcm_gport_t flow_port;    /* flow port id identifies DVP. */
    uint32 options;    /* BCM_FLOW_PORT_ENCAP_xxx. */
    uint32 flags;    /* BCM_FLOW_PORT_ENCAP_FLAG_xxx. */
    uint32 class_id;    /* class id. */
    uint16 mtu;    /* MTU. */
    uint32 network_group;    /* network group ID */
    uint32 dvp_group;    /* DVP group ID */
    uint8 pri;    /* service tag priority. */
    uint8 cfi;    /* service tag cfi */
    uint16 tpid;    /* service tag tpid */
    bcm_vlan_t vlan;    /* service VLAN ID. */
    bcm_if_t egress_if;    /* egress object */
    uint32 valid_elements;    /* bitmap of valid fields for hardware parameters */
    bcm_flow_handle_t flow_handle;    /* flow handle */
    uint32 flow_option;    /* flow option id */
    uint16 ip4_id;    /* IPv4 ID */
    int vlan_pri_map_id;    /* Remarking map ID for the payload VLAN priority and CFI. */
    bcm_gport_t dst_port;    /* Destination port / trunk. */
    uint32 es_id;    /* Ethernet segment ID. */
} bcm_compat6520_flow_port_encap_t;

/* Set the L2 tunnel encapsulation parameters for the given flow_port */
extern int bcm_compat6520_flow_port_encap_set(
    int unit,
    bcm_compat6520_flow_port_encap_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field);

/* Get the encapsulation configuration parameters */
extern int bcm_compat6520_flow_port_encap_get(
    int unit,
    bcm_compat6520_flow_port_encap_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field);


#endif /* defined(INCLUDE_L3) */
/*  */
typedef struct bcm_compat6520_port_phy_tx_s {
    int pre2;    /*  */
    int pre;    /*  */
    int main;    /*  */
    int post;    /*  */
    int post2;    /*  */
    int post3;    /*  */
    int amp;    /*  */
    int drv_mode;    /*  */
    bcm_port_phy_tx_tap_mode_t tx_tap_mode;    /*  */
    bcm_port_phy_signalling_mode_t signalling_mode;    /*  */
} bcm_compat6520_port_phy_tx_t;

/* Modify TX FIR parameters for a given port */
extern int bcm_compat6520_port_phy_tx_set(
    int unit,
    bcm_port_t port,
    bcm_compat6520_port_phy_tx_t *tx);

/* Get TX FIR parameters for a given port */
extern int bcm_compat6520_port_phy_tx_get(
    int unit,
    bcm_port_t port,
    bcm_compat6520_port_phy_tx_t *tx);


/* Presel config structure. */
typedef struct bcm_compat6520_field_presel_config_s {
    uint8 name[BCM_FIELD_MAX_NAME_LEN];
    uint32 flags;                       /* Presel create flags
                                           BCM_FIELD_PRESEL_CREATE_XXX. */
    bcm_field_stage_t stage;            /* Stage for which presel to be created. */
    bcm_field_presel_t presel_id;       /* Preselection ID created based on
                                           flags set. */
} bcm_compat6520_field_presel_config_t;

/* Map a presel id to a name */
extern int bcm_compat6520_field_presel_config_set(
    int unit,
    bcm_field_presel_t presel_id,
    bcm_compat6520_field_presel_config_t *presel_config);

/* Get name from presel id */
extern int bcm_compat6520_field_presel_config_get(
    int unit,
    bcm_field_presel_t presel_id,
    bcm_compat6520_field_presel_config_t *presel_config);

/* Create a field presel ID with specific attributes. */
extern int bcm_compat6520_field_presel_config_create(
    int unit,
    bcm_compat6520_field_presel_config_t *presel_config);

/* Specify hints to be used during group creation. */
typedef struct bcm_compat6520_field_hint_s {
    bcm_field_hint_type_t hint_type;    /* Field hints of type
                                           bcm_field_hint_type_t. */
    bcm_field_qualify_t qual;           /* Field qualifier bcmFieldQualifyXXX. */
    uint32 max_values;                  /* To figure out the width of the
                                           compression output. */
    uint32 start_bit;                   /* Builds the range in conjunction with
                                           end_bit which will indicate that in
                                           the
                                                                   full mask any
                                           bits outside of that range are
                                           guaranteed to be 0. */
    uint32 end_bit;                     /* End bit of a mask for the qualifier
                                           in hint structure. */
    uint32 flags;                       /* Flags. */
    uint32 dosattack_event_flags;       /* Dos Attack Events Flags. */
    uint32 max_group_size;              /* Max size of the group. */
    int priority;                       /* Group priority. */
    bcm_udf_id_t udf_id;                /* UDF ID. */
    bcm_field_group_t group_id;         /* Original Group ID. */
    int value;                          /* Value to be set for specific hint
                                           types. */
} bcm_compat6520_field_hint_t;

/* Associate a hint structure to the hint id. */
extern int bcm_compat6520_field_hints_add(
    int unit,
    bcm_field_hintid_t hint_id,
    bcm_compat6520_field_hint_t *hint);

/* Get the complete hint structure from the hint id. */
extern int bcm_compat6520_field_hints_get(
    int unit,
    bcm_field_hintid_t hint_id,
    bcm_compat6520_field_hint_t *hint);

/* Get the complete hint structure from the hint id. */
extern int bcm_compat6520_field_hints_multi_get(
    int unit,
    bcm_field_hintid_t hint_id,
    bcm_field_hint_type_t hint_type,
    int hints_size,
    bcm_compat6520_field_hint_t *hints_arr,
    int *hints_count);

/* Remove a hint structure from the hint id. */
extern int bcm_compat6520_field_hints_delete(
    int unit,
    bcm_field_hintid_t hint_id,
    bcm_compat6520_field_hint_t *hint);

#endif /* BCM_RPC_SUPPORT */
#endif /* _COMPAT_6520_H_ */
