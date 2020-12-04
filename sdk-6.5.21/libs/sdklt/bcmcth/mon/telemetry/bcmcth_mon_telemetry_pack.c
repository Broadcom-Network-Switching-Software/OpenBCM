/*! \file bcmcth_mon_telmetry_pack.c
 *
 * Telemetry pack and unpack routines for
 * Steaming Telemetry control messages
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*******************************************************************************
 * Includes
 */
#include <shr/shr_util_pack.h>
#include <bcmcth/bcmcth_mon_telemetry_msg.h>

/*******************************************************************************
* Public functions
 */

/*
 * ST configuration message - pack
 */
uint8_t *
bcmcth_mon_telemetry_st_msg_ctrl_config_pack(uint8_t *buf,
                               bcmcth_mon_telemetry_st_msg_ctrl_config_t *msg)
{
    uint32_t i = 0;

    SHR_UTIL_PACK_U32(buf, msg->flags);
    SHR_UTIL_PACK_U32(buf, msg->num_config_elements);
    for (i = 0; i < msg->num_config_elements; i++) {
        SHR_UTIL_PACK_U32(buf, msg->config_value[i]);
    }
    return buf;
}

/*
 * ST Initialization control message - pack
 */
uint8_t *
bcmcth_mon_telemetry_st_msg_ctrl_init_pack(uint8_t *buf,
                               bcmcth_mon_telemetry_st_msg_ctrl_init_t *msg)
{
    SHR_UTIL_PACK_U32(buf, msg->flags);
    SHR_UTIL_PACK_U32(buf, msg->init_time_u);
    SHR_UTIL_PACK_U32(buf, msg->init_time_l);
    SHR_UTIL_PACK_U16(buf, msg->max_export_pkt_length);
    SHR_UTIL_PACK_U16(buf, msg->num_instances);
    SHR_UTIL_PACK_U16(buf, msg->num_collectors);
    SHR_UTIL_PACK_U16(buf, msg->max_num_ports);

    return buf;
}

/*
 * ST Initialization control message - unpack
 */
uint8_t *
bcmcth_mon_telemetry_st_msg_ctrl_init_unpack(uint8_t *buf,
                            bcmcth_mon_telemetry_st_msg_ctrl_init_t *msg)
{
    SHR_UTIL_UNPACK_U32(buf, msg->flags);
    SHR_UTIL_UNPACK_U32(buf, msg->init_time_u);
    SHR_UTIL_UNPACK_U32(buf, msg->init_time_l);
    SHR_UTIL_UNPACK_U16(buf, msg->max_export_pkt_length);
    SHR_UTIL_UNPACK_U16(buf, msg->num_instances);
    SHR_UTIL_UNPACK_U16(buf, msg->num_collectors);

    return buf;
}

/*
 * ST system ID message - pack
 */
uint8_t *
bcmcth_mon_telemetry_st_msg_system_id_pack(uint8_t *buf,
                              bcmcth_mon_telemetry_st_msg_system_id_t *msg)
{
    int i;

    SHR_UTIL_PACK_U16(buf, msg->system_id_length);
    for (i = 0; i < msg->system_id_length; i++) {
        SHR_UTIL_PACK_U8(buf, msg->system_id[i]);
    }

    return buf;
}

/*
 * ST system ID message - unpack
 */
uint8_t *
bcmcth_mon_telemetry_st_msg_system_id_unpack(uint8_t *buf,
                   bcmcth_mon_telemetry_st_msg_system_id_t *msg)
{
    int i;

    SHR_UTIL_UNPACK_U16(buf, msg->system_id_length);
    for (i = 0; i < msg->system_id_length; i++) {
        SHR_UTIL_UNPACK_U8(buf, msg->system_id[i]);
    }

    return buf;
}

/*
 * ST Ports add message - pack
 */
uint8_t *
bcmcth_mon_telemetry_st_msg_ctrl_ports_add_pack(uint8_t *buf,
                            bcmcth_mon_telemetry_st_msg_ctrl_ports_add_t *msg)
{
    int i;

    SHR_UTIL_PACK_U16(buf, msg->num_ports);
    for (i = 0; i < msg->num_ports; i++) {
        SHR_UTIL_PACK_U16(buf, msg->lports[i]);
        SHR_UTIL_PACK_U16(buf, msg->pports[i]);
        SHR_UTIL_PACK_U16(buf, msg->mports[i]);
    }

    return buf;
}

/*
 * ST Ports add message - unpack
 */
uint8_t *
bcmcth_mon_telemetry_st_msg_ctrl_ports_add_unpack(uint8_t *buf,
                       bcmcth_mon_telemetry_st_msg_ctrl_ports_add_t *msg)
{
    int i;

    SHR_UTIL_UNPACK_U16(buf, msg->num_ports);

    for (i = 0; i < msg->num_ports; i++) {
        SHR_UTIL_UNPACK_U16(buf, msg->lports[i]);
        SHR_UTIL_UNPACK_U16(buf, msg->pports[i]);
        SHR_UTIL_UNPACK_U16(buf, msg->mports[i]);
    }

    return buf;
}


/*
 * ST Ports get message - pack
 */
uint8_t *
bcmcth_mon_telemetry_st_msg_ctrl_ports_get_pack(uint8_t *buf,
                            bcmcth_mon_telemetry_st_msg_ctrl_ports_get_t *msg)
{
    int i;

    SHR_UTIL_PACK_U16(buf, msg->num_ports);
    for (i = 0; i < msg->num_ports; i++) {
        SHR_UTIL_PACK_U16(buf, msg->lports[i]);
    }

    return buf;
}

/*
 * ST Ports get message - unpack
 */
uint8_t *
bcmcth_mon_telemetry_st_msg_ctrl_ports_get_unpack(uint8_t *buf,
                             bcmcth_mon_telemetry_st_msg_ctrl_ports_get_t *msg)
{
    int i;

    SHR_UTIL_UNPACK_U16(buf, msg->num_ports);

    for (i = 0; i < msg->num_ports; i++) {
        SHR_UTIL_UNPACK_U16(buf, msg->lports[i]);
    }

    return buf;
}

/*
 * ST Ports delete message - pack
 */
uint8_t *
bcmcth_mon_telemetry_st_msg_ctrl_ports_delete_pack(uint8_t *buf,
                       bcmcth_mon_telemetry_st_msg_ctrl_ports_delete_t *msg)
{
    int i;

    SHR_UTIL_PACK_U16(buf, msg->num_ports);
    for (i = 0; i < msg->num_ports; i++) {
        SHR_UTIL_PACK_U16(buf, msg->lports[i]);
    }

    return buf;
}

/*
 * ST Ports delete message - unpack
 */
uint8_t *
bcmcth_mon_telemetry_st_msg_ctrl_ports_delete_unpack(uint8_t *buf,
                           bcmcth_mon_telemetry_st_msg_ctrl_ports_delete_t *msg)
{
    int i;

    SHR_UTIL_UNPACK_U16(buf, msg->num_ports);

    for (i = 0; i < msg->num_ports; i++) {
        SHR_UTIL_UNPACK_U16(buf, msg->lports[i]);
    }

    return buf;
}

/*
 * ST Instance create - pack
 */
uint8_t *
bcmcth_mon_telemetry_st_msg_ctrl_instance_create_pack(uint8_t *buf,
                        bcmcth_mon_telemetry_st_msg_ctrl_instance_create_t *msg)
{
    int i, j;

    SHR_UTIL_PACK_U16(buf, msg->instance_id);
    SHR_UTIL_PACK_U32(buf, msg->component_id);

    SHR_UTIL_PACK_U16(buf, msg->num_ports);
    for (i = 0; i < msg->num_ports; i++) {
        SHR_UTIL_PACK_U16(buf, msg->lports[i]);
        SHR_UTIL_PACK_U32(buf, msg->stat_types[i]);

        SHR_UTIL_PACK_U16(buf, msg->if_name_lengths[i]);
        for (j = 0; j < msg->if_name_lengths[i]; j++) {
            SHR_UTIL_PACK_U8(buf, msg->if_names[i][j]);
        }
    }

    return buf;
}

/*
 * ST Instance create - unpack
 */
uint8_t *
bcmcth_mon_telemetry_st_msg_ctrl_instance_create_unpack(uint8_t *buf,
                  bcmcth_mon_telemetry_st_msg_ctrl_instance_create_t *msg)
{
    int i, j;

    SHR_UTIL_UNPACK_U16(buf, msg->instance_id);
    SHR_UTIL_UNPACK_U32(buf, msg->component_id);

    SHR_UTIL_UNPACK_U16(buf, msg->num_ports);
    for (i = 0; i < msg->num_ports; i++) {
        SHR_UTIL_UNPACK_U16(buf, msg->lports[i]);
        SHR_UTIL_UNPACK_U32(buf, msg->stat_types[i]);

        SHR_UTIL_UNPACK_U16(buf, msg->if_name_lengths[i]);
        for (j = 0; j < msg->if_name_lengths[i]; j++) {
            SHR_UTIL_UNPACK_U8(buf, msg->if_names[i][j]);
        }
    }

    return buf;
}

/*
 * ST Instance get - pack
 */
uint8_t *
bcmcth_mon_telemetry_st_msg_ctrl_instance_get_pack(uint8_t *buf,
                bcmcth_mon_telemetry_st_msg_ctrl_instance_get_t *msg)
{
    int i, j;

    SHR_UTIL_PACK_U32(buf, msg->component_id);
    SHR_UTIL_PACK_U16(buf, msg->collector_id);

    SHR_UTIL_PACK_U16(buf, msg->num_ports);
    for (i = 0; i < msg->num_ports; i++) {
        SHR_UTIL_PACK_U16(buf, msg->lports[i]);
        SHR_UTIL_PACK_U32(buf, msg->stat_types[i]);

        SHR_UTIL_PACK_U16(buf, msg->if_name_lengths[i]);
        for (j = 0; j < msg->if_name_lengths[i]; j++) {
            SHR_UTIL_PACK_U8(buf, msg->if_names[i][j]);
        }
    }

    return buf;
}

/*
 * ST Instance get - unpack
 */
uint8_t *
bcmcth_mon_telemetry_st_msg_ctrl_instance_get_unpack(uint8_t *buf,
                           bcmcth_mon_telemetry_st_msg_ctrl_instance_get_t *msg)
{
    int i, j;

    SHR_UTIL_UNPACK_U32(buf, msg->component_id);
    SHR_UTIL_UNPACK_U16(buf, msg->collector_id);

    SHR_UTIL_UNPACK_U16(buf, msg->num_ports);
    for (i = 0; i < msg->num_ports; i++) {
        SHR_UTIL_UNPACK_U16(buf, msg->lports[i]);
        SHR_UTIL_UNPACK_U32(buf, msg->stat_types[i]);

        SHR_UTIL_UNPACK_U16(buf, msg->if_name_lengths[i]);
        for (j = 0; j < msg->if_name_lengths[i]; j++) {
            SHR_UTIL_UNPACK_U8(buf, msg->if_names[i][j]);
        }
    }

    return buf;
}

/*
 * Create PB collector - pack
 */
uint8_t *
bcmcth_mon_telemetry_st_msg_ctrl_collector_create_pack(uint8_t * buf,
                 bcmcth_mon_telemetry_st_msg_ctrl_collector_create_t *msg)
{
    int i;

    SHR_UTIL_PACK_U16(buf, msg->id);
    SHR_UTIL_PACK_U32(buf, msg->flags);
    SHR_UTIL_PACK_U16(buf, msg->encap_length);

    for (i = 0; i < msg->encap_length; i++) {
        SHR_UTIL_PACK_U8(buf, msg->encap[i]);
    }

    SHR_UTIL_PACK_U16(buf, msg->mtu);
    SHR_UTIL_PACK_U32(buf, msg->ip_base_checksum);
    SHR_UTIL_PACK_U32(buf, msg->udp_base_checksum);
    SHR_UTIL_PACK_U16(buf, msg->ip_offset);
    SHR_UTIL_PACK_U16(buf, msg->udp_offset);
    SHR_UTIL_PACK_U16(buf, msg->num_port_in_pkt);
    SHR_UTIL_PACK_U32(buf, msg->component_id);

    return buf;
}

/*
 * Create PB collector - unpack
 */
uint8_t *
bcmcth_mon_telemetry_st_msg_ctrl_collector_create_unpack(uint8_t * buf,
                      bcmcth_mon_telemetry_st_msg_ctrl_collector_create_t *msg)
{
    int i;

    SHR_UTIL_UNPACK_U16(buf, msg->id);
    SHR_UTIL_UNPACK_U32(buf, msg->flags);
    SHR_UTIL_UNPACK_U16(buf, msg->encap_length);

    for (i = 0; i < msg->encap_length; i++) {
        SHR_UTIL_UNPACK_U8(buf, msg->encap[i]);
    }

    SHR_UTIL_UNPACK_U16(buf, msg->mtu);
    SHR_UTIL_UNPACK_U32(buf, msg->ip_base_checksum);
    SHR_UTIL_UNPACK_U32(buf, msg->udp_base_checksum);
    SHR_UTIL_UNPACK_U16(buf, msg->ip_offset);
    SHR_UTIL_UNPACK_U16(buf, msg->udp_offset);
    SHR_UTIL_UNPACK_U16(buf, msg->num_port_in_pkt);

    return buf;
}

/*
 * Instance collector attach - pack
 */
uint8_t *
bcmcth_mon_telemetry_st_msg_ctrl_instance_collector_attach_pack(uint8_t * buf,
             bcmcth_mon_telemetry_st_msg_ctrl_instance_collector_attach_t *msg)
{
    SHR_UTIL_PACK_U16(buf, msg->instance_id);
    SHR_UTIL_PACK_U16(buf, msg->collector_id);
    SHR_UTIL_PACK_U32(buf, msg->export_interval_usecs);

    return buf;
}

/*
 * Instance collector attach - unpack
 */
uint8_t *
bcmcth_mon_telemetry_st_msg_ctrl_instance_collector_attach_unpack(uint8_t * buf,
             bcmcth_mon_telemetry_st_msg_ctrl_instance_collector_attach_t *msg)
{
    SHR_UTIL_UNPACK_U16(buf, msg->instance_id);
    SHR_UTIL_UNPACK_U16(buf, msg->collector_id);
    SHR_UTIL_UNPACK_U32(buf, msg->export_interval_usecs);

    return buf;
}

/*
 * Instance export statistics - pack
 */
uint8_t *
bcmcth_mon_telemetry_st_msg_instance_export_stats_pack(uint8_t *buf,
                   bcmcth_mon_telemetry_st_msg_instance_export_stats_t *msg)
{
    SHR_UTIL_PACK_U16(buf, msg->instance_id);
    SHR_UTIL_PACK_U16(buf, msg->collector_id);

    SHR_UTIL_PACK_U32(buf, msg->tx_pkts_hi);
    SHR_UTIL_PACK_U32(buf, msg->tx_pkts_lo);

    SHR_UTIL_PACK_U32(buf, msg->tx_pkt_fails_hi);
    SHR_UTIL_PACK_U32(buf, msg->tx_pkt_fails_lo);

    return buf;
}

/*
 * Instance export statistics - unpack
 */
uint8_t *
bcmcth_mon_telemetry_st_msg_instance_export_stats_unpack(uint8_t *buf,
                     bcmcth_mon_telemetry_st_msg_instance_export_stats_t *msg)
{
    SHR_UTIL_UNPACK_U16(buf, msg->instance_id);
    SHR_UTIL_UNPACK_U16(buf, msg->collector_id);

    SHR_UTIL_UNPACK_U32(buf, msg->tx_pkts_hi);
    SHR_UTIL_UNPACK_U32(buf, msg->tx_pkts_lo);

    SHR_UTIL_UNPACK_U32(buf, msg->tx_pkt_fails_hi);
    SHR_UTIL_UNPACK_U32(buf, msg->tx_pkt_fails_lo);

    return buf;
}

