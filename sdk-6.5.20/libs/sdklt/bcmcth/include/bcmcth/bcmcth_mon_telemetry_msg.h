/*! \file bcmcth_mon_telemetry_msg.h
 *
 * Telemetry messaging and definitions needed
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMCTH_MON_TELEMETRY_MSG_H
#define BCMCTH_MON_TELEMETRY_MSG_H

#include <bcmbd/mcs_shared/mcs_mon_telemetry.h>

/*! ST base SDK version. */
#define BCMMON_TELEMETRY_ST_SDK_BASE_VERSION         0x0

/*! Maximum number of ports to be monitored. */
#define BCMMON_TELEMETRY_ST_MAX_NUM_PORTS            1

/*! Default export packet length. */
#define BCMMON_TELEMETRY_ST_DEFAULT_PKT_LEN          1500

/*! ST current SDK version. */
#define BCMMON_TELEMETRY_ST_SDK_VERSION BCMMON_TELEMETRY_ST_SDK_BASE_VERSION | \
    (1 << BCMMON_TELEMETRY_ST_MAX_NUM_PORTS)

/*! ST minimum BTE version. */
#define BCMMON_TELEMETRY_ST_UC_MIN_VERSION           0x0

/*!
 * ST APP configurable elements.
 */
typedef enum bcmcth_mon_telemetry_st_config_element_e {
    /*! L3 discard packets drop event index. */
    BCMCTH_MON_TELEMETRY_ST_CONFIG_L3_DISCARD_INDEX,
    /*! L3 header error packets drop event index. */
    BCMCTH_MON_TELEMETRY_ST_CONFIG_L3_HDR_ERROR_INDEX,
    /*! Number of multicast queues of a port. */
    BCMCTH_MON_TELEMETRY_ST_CONFIG_PORT_NUM_MC_QUEUES,
    /*! Must be the last. */
    BCMCTH_MON_TELEMETRY_ST_CONFIG_COUNT
} bcmcth_mon_telemetry_st_config_element_t;

/*!
 * ST configuration message.
 */
typedef struct bcmcth_mon_telemetry_st_msg_ctrl_config_s {
    /*! config flags. */
    uint32_t flags;

    /*! Number of configurable elements. */
    uint32_t num_config_elements;

    /*! Configurable element value. */
    uint32_t config_value[BCMCTH_MON_TELEMETRY_ST_CONFIG_COUNT];

} bcmcth_mon_telemetry_st_msg_ctrl_config_t;


/*!
 * ST Initialization control message.
 */
typedef struct bcmcth_mon_telemetry_st_msg_ctrl_init_s {
    /*! Initialization flags. */
    uint32_t flags;

    /*! Upper 32 bits of init time. */
    uint32_t init_time_u;

    /*! Lower 32 bits of init time. */
    uint32_t init_time_l;

    /*! Max length of the export packet. */
    uint16_t max_export_pkt_length;

    /*! Number of instances. */
    uint16_t num_instances;

    /*! Number of collectors. */
    uint16_t num_collectors;

    /*! Maximum number of ports to be monitored. */
    uint16_t max_num_ports;

} bcmcth_mon_telemetry_st_msg_ctrl_init_t;

/*!
 * ST system ID set/get messages.
 */
typedef struct bcmcth_mon_telemetry_st_msg_system_id_s {
    /*! Length of the system ID. */
    uint16_t system_id_length;

    /*! System ID. */
    uint8_t system_id[MCS_SHR_ST_MAX_SYSTEM_ID_LENGTH];

} bcmcth_mon_telemetry_st_msg_system_id_t;

/*!
 * ST ports add.
 */
typedef struct bcmcth_mon_telemetry_st_msg_ctrl_ports_add_s {
    /*! Number of ports to monitor. */
    uint16_t num_ports;

    /*! List of logical ports. */
    uint16_t lports[MCS_SHR_ST_MAX_PORTS];

    /*! List of physical ports. */
    uint16_t pports[MCS_SHR_ST_MAX_PORTS];

    /*! List of MMU ports. */
    uint16_t mports[MCS_SHR_ST_MAX_PORTS];
} bcmcth_mon_telemetry_st_msg_ctrl_ports_add_t;

/*!
 * ST ports get.
 */
typedef struct bcmcth_mon_telemetry_st_msg_ctrl_ports_get_s {
    /*! Number of ports being monitored. */
    uint16_t num_ports;

    /*! List of logical ports. */
    uint16_t lports[MCS_SHR_ST_MAX_PORTS];
} bcmcth_mon_telemetry_st_msg_ctrl_ports_get_t;

/*!
 * ST ports delete.
 */
typedef struct bcmcth_mon_telemetry_st_msg_ctrl_ports_delete_s {
    /*! Number of ports being monitored. */
    uint16_t num_ports;

    /*! List of logical ports. */
    uint16_t lports[MCS_SHR_ST_MAX_PORTS];
} bcmcth_mon_telemetry_st_msg_ctrl_ports_delete_t;

/*!
 * ST instance create.
 */
typedef struct bcmcth_mon_telemetry_st_msg_ctrl_instance_create_s {
    /*! Stat types. */
    uint32_t stat_types[MCS_SHR_ST_MAX_PORTS];

    /*! Component Id. */
    uint32_t component_id;

    /*! Number of ports to monitor. */
    uint16_t num_ports;

    /*! List of logical ports. */
    uint16_t lports[MCS_SHR_ST_MAX_PORTS];

    /*! Instance Id. */
    uint16_t instance_id;

    /*! Interface Name lengths. */
    uint16_t if_name_lengths[MCS_SHR_ST_MAX_PORTS];

    /*! Interface names. */
    uint8_t if_names[MCS_SHR_ST_MAX_PORTS][MCS_SHR_ST_MAX_IF_NAME_LENGTH];

} bcmcth_mon_telemetry_st_msg_ctrl_instance_create_t;

/*!
 * ST instance get.
 */
typedef struct bcmcth_mon_telemetry_st_msg_ctrl_instance_get_s {
    /*! Stat types. */
    uint32_t stat_types[MCS_SHR_ST_MAX_PORTS];

    /*! Component Id. */
    uint32_t component_id;

    /*! Number of ports being monitored. */
    uint16_t num_ports;

    /*! List of logical ports. */
    uint16_t lports[MCS_SHR_ST_MAX_PORTS];

    /*! Interface Name lengths. */
    uint16_t if_name_lengths[MCS_SHR_ST_MAX_PORTS];

    /*! Collector Id. */
    uint16_t collector_id;

    /*! Interface names. */
    uint8_t if_names[MCS_SHR_ST_MAX_PORTS][MCS_SHR_ST_MAX_IF_NAME_LENGTH];

} bcmcth_mon_telemetry_st_msg_ctrl_instance_get_t;

/*!
 * Create PB collector.
 */
typedef struct bcmcth_mon_telemetry_st_msg_ctrl_collector_create_s {
    /*! Flags. */
    uint32_t flags;

    /*! Checksum of IP header excluding length. */
    uint32_t ip_base_checksum;

    /*! UDP psuedo header checksum. */
    uint32_t udp_base_checksum;

    /*! Software handle for collector. */
    uint16_t id;

    /*! Length of the encapsulation. */
    uint16_t encap_length;

    /*! Max size of the packet that can be sent to collector. */
    uint16_t mtu;

    /*! Offset to start of IP header in the encapsulation. */
    uint16_t ip_offset;

    /*! Offset to start of UDP header in the encapsulation. */
    uint16_t udp_offset;

    /*! Number of port info to encode in a single packet. */
    uint16_t num_port_in_pkt;

    /*! Collector encapsulation. */
    uint8_t encap[MCS_SHR_ST_MAX_COLLECTOR_ENCAP_LENGTH];

    /*! Component Id. */
    uint32_t component_id;
} bcmcth_mon_telemetry_st_msg_ctrl_collector_create_t;


/*!
 * Attach instance to collector.
 */
typedef struct bcmcth_mon_telemetry_st_msg_ctrl_instance_collector_attach_s {
    /*! Instance Id. */
    uint16_t instance_id;

    /*! Collector Id. */
    uint16_t collector_id;

    /*! Export interval in microseconds. */
    uint32_t export_interval_usecs;
} bcmcth_mon_telemetry_st_msg_ctrl_instance_collector_attach_t;

/*!
 * Instance export statistics.
 */
typedef struct bcmcth_mon_telemetry_st_msg_instance_export_stats_s {
    /*! Number of pkts transmitted higher word. */
    uint32_t tx_pkts_hi;
    /*! Number of pkts transmitted lower word. */
    uint32_t tx_pkts_lo;

    /*! Number of transmitted packets that failed - higher word. */
    uint32_t tx_pkt_fails_hi;
    /*! Number of transmitted packets that failed - lower word. */
    uint32_t tx_pkt_fails_lo;

    /*! Instance Id. */
    uint16_t instance_id;

    /*! Collector Id. */
    uint16_t collector_id;

} bcmcth_mon_telemetry_st_msg_instance_export_stats_t;

/*!
 *  ST control messages.
 */
typedef union bcmcth_mon_telemetry_st_msg_ctrl_s {
    /*! Init message. */
    bcmcth_mon_telemetry_st_msg_ctrl_init_t             init;
    /*! Ports add message. */
    bcmcth_mon_telemetry_st_msg_ctrl_ports_add_t        ports_add;
    /*! Ports get message. */
    bcmcth_mon_telemetry_st_msg_ctrl_ports_get_t        ports_get;
    /*! Ports delete message. */
    bcmcth_mon_telemetry_st_msg_ctrl_ports_delete_t     ports_delete;
    /*! Instance create message. */
    bcmcth_mon_telemetry_st_msg_ctrl_instance_create_t  instance_create;
    /*! Instance get message. */
    bcmcth_mon_telemetry_st_msg_ctrl_instance_get_t     instance_get;
    /*! Collector create message. */
    bcmcth_mon_telemetry_st_msg_ctrl_collector_create_t coll_create;
    /*! Collector attach message. */
    bcmcth_mon_telemetry_st_msg_ctrl_instance_collector_attach_t
        instance_collector_attach;
    /*! Instance statistics message. */
    bcmcth_mon_telemetry_st_msg_instance_export_stats_t instance_export_stats;
    /*! System ID message. */
    bcmcth_mon_telemetry_st_msg_system_id_t             system_id;
    /*! Configuration message. */
    bcmcth_mon_telemetry_st_msg_ctrl_config_t           config;
} bcmcth_mon_telemetry_st_msg_ctrl_t;

/*!
 * \brief Pack routine for ST init message.
 *
 * Packs ST initialization data onto the buffer.
 *
 * \param [in] buf Pointer to the pack buffer
 * \param [in] msg Initialization message data
 *
 * \retval Pointer to the final packed buffer address.
 */
uint8_t* bcmcth_mon_telemetry_st_msg_ctrl_init_pack(
    uint8_t *buf,
    bcmcth_mon_telemetry_st_msg_ctrl_init_t *msg);

/*!
 * \brief Unpack routine for ST init message.
 *
 * Unpacks ST initialization data onto the message.
 *
 * \param [in] buf Pointer to the data buffer
 * \param [in] msg Retrieved message data
 *
 * \retval Pointer to the final unpacked buffer address.
 */
uint8_t* bcmcth_mon_telemetry_st_msg_ctrl_init_unpack(
    uint8_t *buf,
    bcmcth_mon_telemetry_st_msg_ctrl_init_t *msg);

/*!
 * \brief Pack routine for ST ports add message.
 *
 * Packs ST ports data onto the buffer.
 *
 * \param [in] buf Pointer to the pack buffer
 * \param [in] msg ports add message data
 *
 * \retval Pointer to the final packed buffer address.
 */
uint8_t*
bcmcth_mon_telemetry_st_msg_ctrl_ports_add_pack(uint8_t *buf,
    bcmcth_mon_telemetry_st_msg_ctrl_ports_add_t *msg);

/*!
 * \brief Unpack routine for ST ports add message.
 *
 * Unpacks ST ports data onto the message.
 *
 * \param [in] buf Pointer to the data buffer
 * \param [in] msg Retrieved message data
 *
 * \retval Pointer to the final unpacked buffer address.
 */
uint8_t* bcmcth_mon_telemetry_st_msg_ctrl_ports_add_unpack(
    uint8_t *buf,
    bcmcth_mon_telemetry_st_msg_ctrl_ports_add_t *msg);

/*!
 * \brief Pack routine for ST ports get message.
 *
 * Packs ST ports data onto the buffer.
 *
 * \param [in] buf Pointer to the pack buffer
 * \param [in] msg ports get message data
 *
 * \retval Pointer to the final packed buffer address.
 */
uint8_t* bcmcth_mon_telemetry_st_msg_ctrl_ports_get_pack(
    uint8_t *buf,
    bcmcth_mon_telemetry_st_msg_ctrl_ports_get_t *msg);

/*!
 * \brief Unpack routine for ST ports get message.
 *
 * Unpacks ST ports data onto the message.
 *
 * \param [in] buf Pointer to the data buffer
 * \param [in] msg Retrieved message data
 *
 * \retval Pointer to the final unpacked buffer address.
 */
uint8_t* bcmcth_mon_telemetry_st_msg_ctrl_ports_get_unpack(
    uint8_t *buf,
    bcmcth_mon_telemetry_st_msg_ctrl_ports_get_t *msg);

/*!
 * \brief Pack routine for ST ports delete message.
 *
 * Packs ST ports data onto the buffer.
 *
 * \param [in] buf Pointer to the pack buffer
 * \param [in] msg ports delete message data
 *
 * \retval Pointer to the final packed buffer address.
 */
uint8_t* bcmcth_mon_telemetry_st_msg_ctrl_ports_delete_pack(
    uint8_t *buf,
    bcmcth_mon_telemetry_st_msg_ctrl_ports_delete_t *msg);

/*!
 * \brief Unpack routine for ST ports delete message.
 *
 * Unpacks ST ports data onto the message.
 *
 * \param [in] buf Pointer to the data buffer
 * \param [in] msg Retrieved message data
 *
 * \retval Pointer to the final unpacked buffer address.
 */
uint8_t* bcmcth_mon_telemetry_st_msg_ctrl_ports_delete_unpack(
    uint8_t *buf,
    bcmcth_mon_telemetry_st_msg_ctrl_ports_delete_t *msg);

/*!
 * \brief Pack routine for ST instance create message.
 *
 * Packs ST instance create data onto the buffer.
 *
 * \param [in] buf Pointer to the pack buffer
 * \param [in] msg Instance create message data
 *
 * \retval Pointer to the final packed buffer address.
 */
uint8_t* bcmcth_mon_telemetry_st_msg_ctrl_instance_create_pack(
    uint8_t *buf,
    bcmcth_mon_telemetry_st_msg_ctrl_instance_create_t *msg);

/*!
 * \brief Unpack routine for ST instance create message.
 *
 * Unpacks ST instance create data onto the message.
 *
 * \param [in] buf Pointer to the data buffer
 * \param [in] msg Retrieved message data
 *
 * \retval Pointer to the final unpacked buffer address.
 */
uint8_t* bcmcth_mon_telemetry_st_msg_ctrl_instance_create_unpack(
    uint8_t *buf,
    bcmcth_mon_telemetry_st_msg_ctrl_instance_create_t *msg);

/*!
 * \brief Pack routine for ST instance get message.
 *
 * Packs ST instance get data onto the buffer.
 *
 * \param [in] buf Pointer to the pack buffer
 * \param [in] msg Instance get message data
 *
 * \retval Pointer to the final packed buffer address.
 */
uint8_t* bcmcth_mon_telemetry_st_msg_ctrl_instance_get_pack(
    uint8_t *buf,
    bcmcth_mon_telemetry_st_msg_ctrl_instance_get_t *msg);

/*!
 * \brief Unpack routine for ST instance get message.
 *
 * Unpacks ST instance get data onto the message.
 *
 * \param [in] buf Pointer to the data buffer
 * \param [in] msg Retrieved message data
 *
 * \retval Pointer to the final unpacked buffer address.
 */
uint8_t* bcmcth_mon_telemetry_st_msg_ctrl_instance_get_unpack(
    uint8_t *buf,
    bcmcth_mon_telemetry_st_msg_ctrl_instance_get_t *msg);

/*!
 * \brief Pack routine for ST collector create message.
 *
 * Packs ST collector create data onto the buffer.
 *
 * \param [in] buf Pointer to the pack buffer
 * \param [in] msg collector create message data
 *
 * \retval Pointer to the final packed buffer address.
 */
uint8_t* bcmcth_mon_telemetry_st_msg_ctrl_collector_create_pack(
    uint8_t* buf,
    bcmcth_mon_telemetry_st_msg_ctrl_collector_create_t *msg);

/*!
 * \brief Unpack routine for ST collector create message.
 *
 * Unpacks ST collector create data onto the message.
 *
 * \param [in] buf Pointer to the data buffer
 * \param [in] msg Retrieved message data
 *
 * \retval Pointer to the final unpacked buffer address.
 */
uint8_t* bcmcth_mon_telemetry_st_msg_ctrl_collector_create_unpack(
    uint8_t* buf,
    bcmcth_mon_telemetry_st_msg_ctrl_collector_create_t *msg);

/*!
 * \brief Pack routine for ST instance collector attach message.
 *
 * Packs ST instance collector data onto the buffer.
 *
 * \param [in] buf Pointer to the pack buffer
 * \param [in] msg instance collector message data
 *
 * \retval Pointer to the final packed buffer address.
 */
uint8_t* bcmcth_mon_telemetry_st_msg_ctrl_instance_collector_attach_pack(
    uint8_t* buf,
    bcmcth_mon_telemetry_st_msg_ctrl_instance_collector_attach_t *msg);

/*!
 * \brief Unpack routine for ST instnace collector attach message.
 *
 * Unpacks ST instance collector data onto the message.
 *
 * \param [in] buf Pointer to the data buffer
 * \param [in] msg Retrieved message data
 *
 * \retval Pointer to the final unpacked buffer address.
 */
uint8_t* bcmcth_mon_telemetry_st_msg_ctrl_instance_collector_attach_unpack(
    uint8_t* buf,
    bcmcth_mon_telemetry_st_msg_ctrl_instance_collector_attach_t *msg);

/*!
 * \brief Pack routine for ST system ID send message.
 *
 * Packs ST system ID data onto the buffer.
 *
 * \param [in] buf Pointer to the pack buffer
 * \param [in] msg System ID message data
 *
 * \retval Pointer to the final packed buffer address.
 */
uint8_t* bcmcth_mon_telemetry_st_msg_system_id_pack(
    uint8_t *buf,
    bcmcth_mon_telemetry_st_msg_system_id_t *msg);

/*!
 * \brief Unpack routine for ST system ID send message.
 *
 * Unpacks ST system ID data onto the message.
 *
 * \param [in] buf Pointer to the data buffer
 * \param [in] msg Retrieved message data
 *
 * \retval Pointer to the final unpacked buffer address.
 */
uint8_t* bcmcth_mon_telemetry_st_msg_system_id_unpack(
    uint8_t *buf,
    bcmcth_mon_telemetry_st_msg_system_id_t *msg);

/*!
 * \brief Pack routine for ST instance export statistics message.
 *
 * Packs ST instance export statistics data onto the buffer.
 *
 * \param [in] buf Pointer to the pack buffer
 * \param [in] msg instance export statistics message data
 *
 * \retval Pointer to the final packed buffer address.
 */
uint8_t* bcmcth_mon_telemetry_st_msg_instance_export_stats_pack(
    uint8_t *buf,
    bcmcth_mon_telemetry_st_msg_instance_export_stats_t *msg);

/*!
 * \brief Unpack routine for ST instance export statistics message.
 *
 * Unpacks ST instance export statistics data onto the message.
 *
 * \param [in] buf Pointer to the data buffer
 * \param [in] msg Retrieved message data
 *
 * \retval Pointer to the final unpacked buffer address.
 */
uint8_t* bcmcth_mon_telemetry_st_msg_instance_export_stats_unpack(
    uint8_t *buf,
    bcmcth_mon_telemetry_st_msg_instance_export_stats_t *msg);

/*!
 * \brief Pack routine for ST HW configurations.
 *
 * Packs ST HW configuration data onto the buffer.
 *
 * \param [in] buf Pointer to the pack buffer
 * \param [in] msg HW configuration message data
 *
 * \retval Pointer to the final packed buffer address.
 */
uint8_t* bcmcth_mon_telemetry_st_msg_ctrl_config_pack(
    uint8_t *buf,
    bcmcth_mon_telemetry_st_msg_ctrl_config_t *msg);

#endif /*! BCMCTH_MON_TELEMETRY_MSG_H */
