/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2019 Broadcom Inc. All rights reserved.
 *
 * File:    shr_st_msg.h
 */

#ifndef SHR_ST_MSG_H_
#define SHR_ST_MSG_H_

#include <soc/shared/st.h>

#ifdef BCM_UKERNEL
#define __COMPILER_ATTR__ __attribute__ ((packed, aligned(4)))
#else
#define __COMPILER_ATTR__
#endif

/*
 * ST Initialization control message
 */
typedef struct __COMPILER_ATTR__ shr_st_msg_ctrl_init_s {
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

} shr_st_msg_ctrl_init_t;

/*
 * ST system ID set/get messages
 */
typedef struct __COMPILER_ATTR__ shr_st_msg_system_id_s {
    /* Length of the system ID */
    uint16 system_id_length;

    /* System ID */
    uint8 system_id[SHR_ST_MAX_SYSTEM_ID_LENGTH];

} shr_st_msg_system_id_t;

/*
 * ST ports add
 */
typedef struct __COMPILER_ATTR__ shr_st_msg_ctrl_ports_add_s {
    /* Number of ports to monitor */
    uint16 num_ports;

    /* List of logical ports */
    uint16 lports[SHR_ST_MAX_PORTS];

    /* List of physical ports */
    uint16 pports[SHR_ST_MAX_PORTS];

    /* List of MMU ports */
    uint16 mports[SHR_ST_MAX_PORTS];
} shr_st_msg_ctrl_ports_add_t;

/*
 * ST ports get
 */
typedef struct __COMPILER_ATTR__ shr_st_msg_ctrl_ports_get_s {
    /* Number of ports being monitored */
    uint16 num_ports;

    /* List of logical ports */
    uint16 lports[SHR_ST_MAX_PORTS];
} shr_st_msg_ctrl_ports_get_t;

/*
 * ST ports delete
 */
typedef struct __COMPILER_ATTR__ shr_st_msg_ctrl_ports_delete_s {
    /* Number of ports being monitored */
    uint16 num_ports;

    /* List of logical ports */
    uint16 lports[SHR_ST_MAX_PORTS];
} shr_st_msg_ctrl_ports_delete_t;

/*
 * ST instance create
 */
typedef struct __COMPILER_ATTR__ shr_st_msg_ctrl_instance_create_s {
    /* Stat types */
    uint32 stat_types[SHR_ST_MAX_PORTS];

    /* Component Id */
    uint32 component_id;

    /* Number of ports to monitor */
    uint16 num_ports;

    /* List of logical ports */
    uint16 lports[SHR_ST_MAX_PORTS];

    /* Instance Id */
    uint16 instance_id;

    /* Interface Name lengths */
    uint16 if_name_lengths[SHR_ST_MAX_PORTS];

    /* Interface names */
    uint8 if_names[SHR_ST_MAX_PORTS][SHR_ST_MAX_IF_NAME_LENGTH];

} shr_st_msg_ctrl_instance_create_t;

/*
 * ST instance get
 */
typedef struct __COMPILER_ATTR__ shr_st_msg_ctrl_instance_get_s {
    /* Stat types */
    uint32 stat_types[SHR_ST_MAX_PORTS];

    /* Component Id */
    uint32 component_id;

    /* Number of ports being monitored */
    uint16 num_ports;

    /* List of logical ports */
    uint16 lports[SHR_ST_MAX_PORTS];

    /* Interface Name lengths */
    uint16 if_name_lengths[SHR_ST_MAX_PORTS];

    /* Collector Id */
    uint16 collector_id;

    /* Interface names */
    uint8 if_names[SHR_ST_MAX_PORTS][SHR_ST_MAX_IF_NAME_LENGTH];

} shr_st_msg_ctrl_instance_get_t;

#define SHR_ST_MSG_CTRL_COLLECTOR_FLAGS_UDP_CHECKSUM (1 << 0)

/*
 * Create PB collector
 */
typedef struct __COMPILER_ATTR__ shr_st_msg_ctrl_collector_create_s {
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
} shr_st_msg_ctrl_collector_create_t;


/*
 * Attach instance to collector
 */
typedef struct __COMPILER_ATTR__ shr_st_msg_ctrl_instance_collector_attach_s {
    /* Instance Id */
    uint16 instance_id;

    /* Collector Id */
    uint16 collector_id;

    /* Export interval in microseconds */
    uint32 export_interval_usecs;
} shr_st_msg_ctrl_instance_collector_attach_t;

/*
 * Instance export statistics
 */
typedef struct __COMPILER_ATTR__ shr_st_msg_instance_export_stats_s {
    /* Number of pkts transmitted */
    uint32 tx_pkts_hi;
    uint32 tx_pkts_lo;

    /* Number of tx pkts that failed */
    uint32 tx_pkt_fails_hi;
    uint32 tx_pkt_fails_lo;

    /* Instance Id */
    uint16 instance_id;

    /* Collector Id */
    uint16 collector_id;

} shr_st_msg_instance_export_stats_t;

/*
 *  ST control messages
 */
typedef union shr_st_msg_ctrl_s {
    shr_st_msg_ctrl_init_t             init;
    shr_st_msg_ctrl_ports_add_t        ports_add;
    shr_st_msg_ctrl_ports_get_t        ports_get;
    shr_st_msg_ctrl_ports_delete_t     ports_delete;
    shr_st_msg_ctrl_instance_create_t  instance_create;
    shr_st_msg_ctrl_instance_get_t     instance_get;
    shr_st_msg_ctrl_collector_create_t coll_create;
    shr_st_msg_ctrl_instance_collector_attach_t instance_collector_attach;
    shr_st_msg_instance_export_stats_t instance_export_stats;
} shr_st_msg_ctrl_t;


#endif /* SHR_ST_MSG_H_ */
