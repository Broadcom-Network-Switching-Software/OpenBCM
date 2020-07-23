/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    ifa_sdk_msg.h
 */

#ifndef IFA_SDK_MSG_H_
#define IFA_SDK_MSG_H_

#include <soc/shared/shr_pkt.h>

/*
 * Create IFA config information
 */
typedef struct ifa_sdk_msg_ctrl_config_info_s {
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

    /*
     * Number of hops in the topology is
     * greater than Hop limit,  IFA MD stack
     * contains Hop limit number of MDs in
     * MD stack.
     */
    uint8 true_hop_count;
} ifa_sdk_msg_ctrl_config_info_t;
#endif /* IFA_SDK_MSG_H_ */
