/*
* $Id: compat_6524.h,v 1.0 2021/10/21
* This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
* 
* Copyright 2007-2021 Broadcom Inc. All rights reserved.
*
* RPC Compatibility with sdk-6.5.24 routines
*/

#ifndef _COMPAT_6524_H_
#define _COMPAT_6524_H_

#ifdef BCM_RPC_SUPPORT
#include <bcm/flow.h>
#include <bcm/types.h>
#include <bcm/mirror.h>
#include <bcm/int.h>

/* PP header information for mirrored packet in DNX devices */
typedef struct bcm_compat6524_mirror_pkt_dnx_pp_header_s {
    uint8 tail_edit_profile;    /* Tail edit profile, used at ETPP for tail editing */
    uint8 bytes_to_remove;    /* Bytes to remove from the start of the packet */
    uint8 eth_header_remove;    /* Indication to remove Ethernet header from mirror copy */
    bcm_gport_t out_vport_ext[3];    /* Extended outlif(s) */
    uint32 vsi;    /* Virtual switch instance */
} bcm_compat6524_mirror_pkt_dnx_pp_header_t;

/* Mirror packet system header information for DNX devices */
typedef struct bcm_compat6524_mirror_header_info_s {
    bcm_mirror_pkt_dnx_ftmh_header_t tm;    /* TM header */
    bcm_compat6524_mirror_pkt_dnx_pp_header_t pp;    /* PP header */
    bcm_pkt_dnx_udh_t udh[4];    /* UDH header */
} bcm_compat6524_mirror_header_info_t;

/* Set system header information */
extern int bcm_compat6524_mirror_header_info_set(
    int unit,
    uint32 flags,
    bcm_gport_t mirror_dest_id,
    bcm_compat6524_mirror_header_info_t *mirror_header_info);

/* Get system header information */
extern int bcm_compat6524_mirror_header_info_get(
    int unit,
    bcm_gport_t mirror_dest_id,
    uint32 *flags,
    bcm_compat6524_mirror_header_info_t *mirror_header_info);

/*  */
typedef struct bcm_compat6524_port_fdr_stats_s {
    uint64 start_time;    /* indicate the time when Flight Data Recorder (FDR) start to collect data */
    uint64 end_time;    /* indicate the time when the last statistics are collected */
    uint32 num_uncorrect_cws;    /* indicate the total number of uncorrectable code words */
    uint32 num_cws;    /* total number of code words used for the statistics collection */
    uint32 num_symb_errs;    /* total number of symbol errors detected */
    uint32 cw_s0_errs;    /* Number of Code Words with S + 0 symbol errors where S is the selected value via symb_err_stats_sel in bcm_port_fdr_config_t. If symb_err_stats_sel is 0, S is 0. If symb_err_stats_sel is 1, S = 9 or S = 8 based on the chip. */
    uint32 cw_s1_errs;    /* Number of Code Words with S + 1 symbol errors where S is the selected value via symb_err_stats_sel in bcm_port_fdr_config_t. If symb_err_stats_sel is 0, S is 0. If symb_err_stats_sel is 1, S = 9 or S = 8 based on the chip. */
    uint32 cw_s2_errs;    /* Number of Code Words with S + 2 symbol errors where S is the selected value via symb_err_stats_sel in bcm_port_fdr_config_t. If symb_err_stats_sel is 0, S is 0. If symb_err_stats_sel is 1, S = 9 or S = 8 based on the chip. */
    uint32 cw_s3_errs;    /* Number of Code Words with S + 3 symbol errors where S is the selected value via symb_err_stats_sel in bcm_port_fdr_config_t. If symb_err_stats_sel is 0, S is 0. If symb_err_stats_sel is 1, S = 9 or S = 8 based on the chip. */
    uint32 cw_s4_errs;    /* Number of Code Words with S + 4 symbol errors where S is the selected value via symb_err_stats_sel in bcm_port_fdr_config_t. If symb_err_stats_sel is 0, S is 0. If symb_err_stats_sel is 1, S = 9 or S = 8 based on the chip. */
    uint32 cw_s5_errs;    /* Number of Code Words with S + 5 symbol errors where S is the selected value via symb_err_stats_sel in bcm_port_fdr_config_t. If symb_err_stats_sel is 0, S is 0. If symb_err_stats_sel is 1, S = 9 or S = 8 based on the chip. */
    uint32 cw_s6_errs;    /* Number of Code Words with S + 6 symbol errors where S is the selected value via symb_err_stats_sel in bcm_port_fdr_config_t. If symb_err_stats_sel is 0, S is 0. If symb_err_stats_sel is 1, S = 9 or S = 8 based on the chip. */
    uint32 cw_s7_errs;    /* Number of Code Words with S + 7 symbol errors where S is the selected value via symb_err_stats_sel in bcm_port_fdr_config_t. If symb_err_stats_sel is 0, S is 0. If symb_err_stats_sel is 1, S = 9 or S = 8 based on the chip. */
    uint32 cw_s8_errs;    /* Number of Code Words with S + 8 symbol errors where S is the selected value via symb_err_stats_sel in bcm_port_fdr_config_t. If symb_err_stats_sel is 0, S is 0. If symb_err_stats_sel is 1, S = 9 or S = 8 based on the chip. */
} bcm_compat6524_port_fdr_stats_t;

/* Get the statistics of Flight Data Recorder (FDR) on a port */
extern int bcm_compat6524_port_fdr_stats_get(
    int unit,
    bcm_port_t port,
    bcm_compat6524_port_fdr_stats_t *fdr_stats);

#if defined(INCLUDE_INT)
/* INT metadata field entry. */
typedef struct bcm_compat6524_int_metadata_field_entry_s {
    bcm_int_metadata_field_select_t select; /* Metadata field select. */
    bcm_int_metadata_field_construct_t construct; /* Metadata field construct. */
} bcm_compat6524_int_metadata_field_entry_t;

/* Get the metadata profile. */
extern int bcm_compat6524_int_metadata_profile_get(
    int unit,
    int profile_id,
    int array_size,
    bcm_compat6524_int_metadata_field_entry_t *entry_array,
    int *field_count);

/* Add the metadata field to the metadata profile. */
extern int bcm_compat6524_int_metadata_field_add(
    int unit,
    int profile_id,
    bcm_compat6524_int_metadata_field_entry_t *field_entry);

/* Delete the metadata field from the metadata profile. */
extern int bcm_compat6524_int_metadata_field_delete(
    int unit,
    int profile_id,
    bcm_compat6524_int_metadata_field_entry_t *field_entry);

#endif /* defined(INCLUDE_INT) */
#if defined(INCLUDE_L3)
/* Flow tunneling terminator structure. */
typedef struct bcm_compat6524_flow_tunnel_terminator_s {
    uint32 flags;    /* Configuration flags. Use BCM_TUNNEL flags */
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
    bcm_vlan_t vlan;    /* VLAN ID for tunnel header match. */
    uint32 protocol;    /* Protocol type */
    uint32 valid_elements;    /* bitmap of valid fields */
    uint32 flow_handle;    /* flow handle */
    uint32 flow_option;    /* flow option */
    bcm_vlan_t vlan_mask;    /* The VLAN ID mask. */
    uint32 class_id;    /* Class ID as a key in VFP. */
    bcm_pbmp_t term_pbmp;    /* Allowed port bitmap for tunnel termination. */
    uint16 next_hdr;    /* Match next header. */
    bcm_gport_t flow_port;    /* Assign SVP. */
    bcm_if_t intf_id;    /* Assign L3_IIF. */
} bcm_compat6524_flow_tunnel_terminator_t;

/* Create a flow tunnel terminator object. */
extern int bcm_compat6524_flow_tunnel_terminator_create(
    int unit,
    bcm_compat6524_flow_tunnel_terminator_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field);

/* Delete a flow tunnel terminator object. */
extern int bcm_compat6524_flow_tunnel_terminator_destroy(
    int unit,
    bcm_compat6524_flow_tunnel_terminator_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field);

/* Get the flow tunnel terminator object. */
extern int bcm_compat6524_flow_tunnel_terminator_get(
    int unit,
    bcm_compat6524_flow_tunnel_terminator_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field);

/* Set the flex counter object value to the flow tunnel terminator object. */
extern int bcm_compat6524_flow_tunnel_terminator_flexctr_object_set(
    int unit,
    bcm_compat6524_flow_tunnel_terminator_t *info,
    uint32 value);

/* Get the flex counter object value from the flow tunnel terminator object. */
extern int bcm_compat6524_flow_tunnel_terminator_flexctr_object_get(
    int unit,
    bcm_compat6524_flow_tunnel_terminator_t *info,
    uint32 *value);

/* Attach counter entries to the flow tunnel terminator object. */
extern int bcm_compat6524_flow_tunnel_terminator_stat_attach(
    int unit,
    bcm_compat6524_flow_tunnel_terminator_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field,
    uint32 stat_counter_id);

/* Detach counter entries from the flow tunnel terminator object. */
extern int bcm_compat6524_flow_tunnel_terminator_stat_detach(
    int unit,
    bcm_compat6524_flow_tunnel_terminator_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field);

/* Get stat counter ID associated to the flow tunnel terminator object. */
extern int bcm_compat6524_flow_tunnel_terminator_stat_id_get(
    int unit,
    bcm_compat6524_flow_tunnel_terminator_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field,
    uint32 *stat_counter_id);

/* L3 vPBR Structure. Contains information required for manipulating L3 vPBR table entries */
typedef struct bcm_compat6524_l3_vpbr_entry_s {
    uint32 flags;    /* See BCM_L3_VPBR_xxx flag definitions. */
    int priority;    /* Entry priority. */
    bcm_vrf_t vrf;    /* Virtual Router instance. */
    bcm_vrf_t vrf_mask;    /*  */
    bcm_ip_t sip_addr;    /* Source IP address (IPv4). */
    bcm_ip_t sip_addr_mask;    /*  */
    bcm_ip_t dip_addr;    /* Destination IP address (IPv4). */
    bcm_ip_t dip_addr_mask;    /*  */
    bcm_ip6_t sip6_addr;    /* Destination IP address (IPv6). */
    bcm_ip6_t sip6_addr_mask;    /*  */
    bcm_ip6_t dip6_addr;    /* Destination IP address (IPv6). */
    bcm_ip6_t dip6_addr_mask;    /*  */
    int dscp;    /* DSCP value */
    int dscp_mask;    /*  */
    uint32 src_port;    /* TCP/UDP src port. */
    uint32 src_port_mask;    /*  */
    uint32 dst_port;    /* TCP/UDP dst port. */
    uint32 dst_port_mask;    /*  */
    bcm_vrf_t new_vrf;    /* The new result Virtual Router instance. */
    bcm_if_t l3_intf_id;    /* The relevant incoming interface . */
    bcm_if_t l3_intf_id_mask;    /*  */
    uint8 ip_protocol;    /* IP protocol field. */
    uint8 ip_protocol_mask;    /* IP protocol field mask. */
} bcm_compat6524_l3_vpbr_entry_t;

/* Delete an entry from the L3 vPBR table. */
extern int bcm_compat6524_l3_vpbr_entry_delete(
    int unit,
    bcm_compat6524_l3_vpbr_entry_t *entry);

/* Delete all L3 interfaces. */
extern int bcm_compat6524_l3_vpbr_entry_delete_all(
    int unit,
    bcm_compat6524_l3_vpbr_entry_t *entry);

/* Look up a L3 vPBR table entry. */
extern int bcm_compat6524_l3_vpbr_entry_get(
    int unit,
    bcm_compat6524_l3_vpbr_entry_t *entry);

/* Add an entry to the L3 vPBR table. */
extern int bcm_compat6524_l3_vpbr_entry_add(
    int unit,
    bcm_compat6524_l3_vpbr_entry_t *entry);

#endif /* defined(INCLUDE_L3) */
#endif /* BCM_RPC_SUPPORT */
#endif /* _COMPAT_6524_H_ */
