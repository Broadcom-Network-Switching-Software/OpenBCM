/*
 * $Id: compat_6511.h,v 2.0 2017/10/23
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * RPC Compatibility with sdk-6.5.11 routines
 */

#ifndef _COMPAT_6511_H_
#define _COMPAT_6511_H_

#ifdef  BCM_RPC_SUPPORT
#include <bcm/types.h>
#include <bcm/cosq.h>
#include <bcm/pkt.h>

#define BCM_COMPAT6511_COSQ_PFC_GPORT_COUNT   16
/* PFC mapping array type. */
typedef struct bcm_compat6511_cosq_pfc_class_mapping_s {
    int class_id;                                       /* class id */  
    bcm_gport_t gport_list[BCM_COMPAT6511_COSQ_PFC_GPORT_COUNT];   /* mapped cosqs */
} bcm_compat6511_cosq_pfc_class_mapping_t;

/* Set mapping of PFC class to COSQs */
extern int bcm_compat6511_cosq_pfc_class_mapping_set(
        int unit, 
        bcm_gport_t port,
        int array_count,
        bcm_compat6511_cosq_pfc_class_mapping_t *mapping_array);

/* Get mapping of PFC class to COSQs */
extern int bcm_compat6511_cosq_pfc_class_mapping_get(
        int unit, 
        bcm_gport_t port,
        int array_max,
        bcm_compat6511_cosq_pfc_class_mapping_t *mapping_array,
        int *array_count);

/*
 * Mirror destination Structure
 *
 * Contains information required for manipulating mirror destinations.
 */
typedef struct bcm_compat6511_mirror_destination_s {
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
} bcm_compat6511_mirror_destination_t;

/* Create a mirror (destination, encapsulation) pair. */
extern int bcm_compat6511_mirror_destination_create(
    int unit,
    bcm_compat6511_mirror_destination_t *mirror_dest);

/* Get mirror (destination, encapsulation) pair. */
extern int bcm_compat6511_mirror_destination_get(
    int unit,
    bcm_gport_t mirror_dest_id,
    bcm_compat6511_mirror_destination_t *mirror_dest);
#endif  /* BCM_RPC_SUPPORT */
#endif  /* !_COMPAT_6511_H */
