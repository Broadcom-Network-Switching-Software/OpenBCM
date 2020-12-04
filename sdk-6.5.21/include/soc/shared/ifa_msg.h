/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    shr_ifa_msg.h
 */

#ifndef SHR_IFA_MSG_H_
#define SHR_IFA_MSG_H_

#include <soc/shared/ifa.h>
#include <soc/shared/shr_pkt.h>
#include <shared/pack.h>

#ifdef BCM_UKERNEL
#define __COMPILER_ATTR__ __attribute__ ((packed, aligned(4)))
#else
#define __COMPILER_ATTR__
#endif

typedef struct __COMPILER_ATTR__ shr_ifa_msg_ctrl_init_s {
    /* Local RX DMA channel */
    uint32 rx_channel;

    /* Local TX DMA channel */
    uint32 tx_channel;

} shr_ifa_msg_ctrl_init_t;

/*
 * Create IFA collector information
 */
typedef struct __COMPILER_ATTR__ shr_ifa_msg_ctrl_collector_info_s {
    /* Transport type */
    uint8 transport_type;

    /* L2 HDR information */
    uint32 outer_vlan_tag;
    uint32 inner_vlan_tag;
    uint16 outer_tpid;
    uint16 inner_tpid;
    uint8  vlan_tag_structure;

    /* Checksum of IP header excluding length */
    uint32 ip_base_checksum;

    /* UDP psuedo header checksum */
    uint32 udp_base_checksum;

    /* Length of the IFA encapsulation */
    uint16 encap_length;

    /* Max size of the packet that can be sent to collector */
    uint16 mtu;

    /* Offset to start of IP header in the encap */
    uint16 ip_offset;

    /* Offset to start of UDP header in the encap */
    uint16 udp_offset;

    /* Collector encapsulation */
    uint8 encap[SHR_IFA_MAX_COLLECTOR_ENCAP_LENGTH];
} shr_ifa_msg_ctrl_collector_info_t;

/*
 * Create IFA config information
 */
typedef struct __COMPILER_ATTR__ shr_ifa_msg_ctrl_config_info_s {
    /* INT header ProbeMarkers Value. */
    uint32 probemarker_1;
    uint32 probemarker_2;

    /* In-band flow analyzer. Meta Data header Device ID */
    uint32 device_id;

    /* INT header Maximum payload. */
    uint16 max_payload_length;

    /* In-band flow analyzer Loopback ports for IFA E_APP in switch */
    uint16 lb_port_1;
    uint16 lb_port_2;

    /* Meta Data header Module ID */
    /* Module Id used to identify loopback source port information. */
    uint16 module_id;

    /* The Payload packet length that can be sent to this collector.*/
    uint16 rx_packet_payload_length;

    /* Option to send with recieved header */
    uint8 optional_headers;

    /* INT header Hop Limit. */
    uint8 hop_limit;

    /* Length of the IFA INT header encapsulation */
    uint16 int_encap_length;

    /* Length of the IFA LB header encapsulation */
    uint16 lb_encap_length;

    /* INT header encapsulation */
    uint8 int_encap[SHR_INT_HEADER_LENGTH];

    /* LoopBack header encapsulation */
    uint8 lb_encap[SHR_LB_HEADER_LENGTH];

} shr_ifa_msg_ctrl_config_info_t;

/*
 * Create IFA statistics information
 */
typedef struct __COMPILER_ATTR__ shr_ifa_msg_ctrl_stat_info_s {
    /* Recieved packet count into the FW. */
    uint32 rx_pkt_cnt;

    /* Transmitted packet count form the FW */
    uint32 tx_pkt_cnt;

    /* Number of pkt dropped due to missing configuration. */
    uint32 ifa_no_config_drop;

    /* Number of pkt dropped due to missing collector configuration. */
    uint32 ifa_collector_not_present_drop;

    /* Number of pkt dropped due to Hop count and Hop limit are out of order. */
    uint32 ifa_hop_cnt_invalid_drop;

    /* Number of pkt dropped due to maximum length and current length are out of order. */
    uint32 ifa_int_hdr_len_invalid_drop;

    /* Number of pkt dropped due to invalid packet size. */
    uint32 ifa_pkt_size_invalid_drop;
} shr_ifa_msg_ctrl_stat_info_t;

/*
 *  IFA control messages
 */
typedef union shr_ifa_msg_ctrl_s {
    shr_ifa_msg_ctrl_init_t                 ifa_init;
    shr_ifa_msg_ctrl_collector_info_t       ifa_collector_info;
    shr_ifa_msg_ctrl_config_info_t          ifa_config_info;
    shr_ifa_msg_ctrl_stat_info_t            ifa_stat_info;
} shr_ifa_msg_ctrl_t;
#endif /* SHR_IFA_MSG_H_ */
