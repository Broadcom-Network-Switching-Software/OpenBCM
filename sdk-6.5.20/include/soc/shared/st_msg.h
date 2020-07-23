/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    shr_st_msg.h
 */

#ifndef SHR_ST_MSG_H_
#define SHR_ST_MSG_H_

#include <bcm_int/esw/st_sdk_msg.h>
#include <soc/shared/st.h>

#ifdef BCM_UKERNEL
#define __COMPILER_ATTR__ __attribute__ ((packed, aligned(4)))
#else
#define __COMPILER_ATTR__
#endif


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
 * ST APP configurable elements.
 */
typedef enum shr_st_config_element_e {
    /* Number of multicast queues of a port. */
    SHR_ST_CONFIG_PORT_NUM_MC_QUEUES,
    /* Must be the last. */
    SHR_ST_CONFIG_COUNT
} shr_st_config_element_t;

/*
 * ST HW configuration data.
 */
typedef struct shr_st_hw_config_s {
    /* config flags. */
    uint32 flags;

    /* Number of configurable elements. */
    uint32 num_config_elements;

    /* Configurable element value. */
    uint32 config_value[SHR_ST_CONFIG_COUNT];

} shr_st_hw_config_t;

/*
 *  ST control messages
 */
typedef union shr_st_msg_ctrl_s {
    shr_st_msg_ctrl_ports_add_t        ports_add;
    shr_st_msg_ctrl_ports_get_t        ports_get;
    shr_st_msg_ctrl_ports_delete_t     ports_delete;
    shr_st_msg_ctrl_instance_create_t  instance_create;
    shr_st_msg_ctrl_instance_get_t     instance_get;
    shr_st_msg_ctrl_instance_collector_attach_t instance_collector_attach;
    shr_st_msg_instance_export_stats_t instance_export_stats;
    shr_st_hw_config_t                     hw_config;
    st_sdk_msg_ctrl_init_t                 sdk_init;
    st_sdk_msg_ctrl_collector_create_t     coll_pb_create;
} shr_st_msg_ctrl_t;


#endif /* SHR_ST_MSG_H_ */
