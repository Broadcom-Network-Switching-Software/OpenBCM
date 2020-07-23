/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    st_sdk_msg.h
 */

#ifndef ST_SDK_MSG_H_
#define ST_SDK_MSG_H_

#include <sal/types.h>
#include <soc/shared/st.h>

/*
 * ST Initialization control message
 */
typedef struct st_sdk_msg_ctrl_init_s {
    /* Initialization flags */
    uint32 flags;

    /* Upper 32 bits of init time */
    uint32 init_time_u;

    /* Lower 32 bits of init time */
    uint32 init_time_l;

    /* Max length of the export packet */
    uint16 max_export_pkt_length;

    /* Number of instances */
    uint16 num_instances;

    /* Number of collectors */
    uint16 num_collectors;

    /* Maximum number of ports to be monitored */
    uint16 max_num_ports;
} st_sdk_msg_ctrl_init_t;

/*
 * Create PB collector
 */
typedef struct st_sdk_msg_ctrl_collector_create_s {
    /* Flags */
    uint32 flags;

    /* Checksum of IP header excluding length */
    uint32 ip_base_checksum;

    /* UDP psuedo header checksum */
    uint32 udp_base_checksum;

    /* Software handle for collector */
    uint16 id;
    
    /* Length of the encapsulation */
    uint16 encap_length;
    
    /* Max size of the packet that can be sent to collector */
    uint16 mtu;

    /* Offset to start of IP header in the encap */
    uint16 ip_offset;

    /* Offset to start of UDP header in the encap */
    uint16 udp_offset;

    /* Number of port info to encode in a single packet */
    uint16 num_port_in_pkt;

    /* Collector encapsulation */
    uint8 encap[SHR_ST_MAX_COLLECTOR_ENCAP_LENGTH];

    /* Component Id. */
    uint32 component_id;
} st_sdk_msg_ctrl_collector_create_t;

#endif /* ST_SDK_MSG_H_ */
