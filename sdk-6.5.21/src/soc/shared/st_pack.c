/*
 * $Id: st_pack.c
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        st_pack.c
 * Purpose:     ST pack and unpack routines for:
 *              - ST Control messages
 *
 *
 * ST control messages
 *
 * ST messages between the Host CPU and uController are sent
 * using the uc_message module which allows short messages
 * to be passed (see include/soc/shared/mos_msg_common.h)
 *
 * Additional information for a given message (a long message) is passed
 * using DMA.  The ST control message types defines the format
 * for these long messages.
 *
 * This file is shared between SDK and uKernel.
 */

#include <shared/pack.h>
#include <soc/shared/st.h>
#include <soc/shared/st_msg.h>
#include <soc/shared/st_pack.h>


/*
 * ST system ID message - pack
 */
uint8*
shr_st_msg_system_id_pack(uint8 *buf, shr_st_msg_system_id_t *msg)
{
    int i;

    _SHR_PACK_U16(buf, msg->system_id_length);
    for (i = 0; i < msg->system_id_length; i++) {
        _SHR_PACK_U8(buf, msg->system_id[i]);
    }

    return buf;
}

/*
 * ST system ID message - unpack
 */
uint8*
shr_st_msg_system_id_unpack(uint8 *buf, shr_st_msg_system_id_t *msg)
{
    int i;

    _SHR_UNPACK_U16(buf, msg->system_id_length);
    for (i = 0; i < msg->system_id_length; i++) {
        _SHR_UNPACK_U8(buf, msg->system_id[i]);
    }

    return buf;
}

/*
 * ST Ports add message - pack
 */
uint8*
shr_st_msg_ctrl_ports_add_pack(uint8 *buf, shr_st_msg_ctrl_ports_add_t *msg)
{
    int i;

    _SHR_PACK_U16(buf, msg->num_ports);
    for (i = 0; i < msg->num_ports; i++) {
        _SHR_PACK_U16(buf, msg->lports[i]);
        _SHR_PACK_U16(buf, msg->pports[i]);
        _SHR_PACK_U16(buf, msg->mports[i]);
    }

    return buf;
}

/*
 * ST Ports add message - unpack
 */
uint8*
shr_st_msg_ctrl_ports_add_unpack(uint8 *buf, shr_st_msg_ctrl_ports_add_t *msg)
{
    int i;

    _SHR_UNPACK_U16(buf, msg->num_ports);

    for (i = 0; i < msg->num_ports; i++) {
        _SHR_UNPACK_U16(buf, msg->lports[i]);
        _SHR_UNPACK_U16(buf, msg->pports[i]);
        _SHR_UNPACK_U16(buf, msg->mports[i]);
    }

    return buf;
}


/*
 * ST Ports get message - pack
 */
uint8*
shr_st_msg_ctrl_ports_get_pack(uint8 *buf, shr_st_msg_ctrl_ports_get_t *msg)
{
    int i;

    _SHR_PACK_U16(buf, msg->num_ports);
    for (i = 0; i < msg->num_ports; i++) {
        _SHR_PACK_U16(buf, msg->lports[i]);
    }

    return buf;
}

/*
 * ST Ports get message - unpack
 */
uint8*
shr_st_msg_ctrl_ports_get_unpack(uint8 *buf, shr_st_msg_ctrl_ports_get_t *msg)
{
    int i;

    _SHR_UNPACK_U16(buf, msg->num_ports);

    for (i = 0; i < msg->num_ports; i++) {
        _SHR_UNPACK_U16(buf, msg->lports[i]);
    }

    return buf;
}

/*
 * ST Ports delete message - pack
 */
uint8*
shr_st_msg_ctrl_ports_delete_pack(uint8 *buf, shr_st_msg_ctrl_ports_delete_t *msg)
{
    int i;

    _SHR_PACK_U16(buf, msg->num_ports);
    for (i = 0; i < msg->num_ports; i++) {
        _SHR_PACK_U16(buf, msg->lports[i]);
    }

    return buf;
}

/*
 * ST Ports delete message - unpack
 */
uint8*
shr_st_msg_ctrl_ports_delete_unpack(uint8 *buf, shr_st_msg_ctrl_ports_delete_t *msg)
{
    int i;

    _SHR_UNPACK_U16(buf, msg->num_ports);

    for (i = 0; i < msg->num_ports; i++) {
        _SHR_UNPACK_U16(buf, msg->lports[i]);
    }

    return buf;
}

/*
 * ST Instance create - pack
 */
uint8*
shr_st_msg_ctrl_instance_create_pack(uint8 *buf, shr_st_msg_ctrl_instance_create_t *msg)
{
    int i, j;

    _SHR_PACK_U16(buf, msg->instance_id);
    _SHR_PACK_U32(buf, msg->component_id);

    _SHR_PACK_U16(buf, msg->num_ports);
    for (i = 0; i < msg->num_ports; i++) {
        _SHR_PACK_U16(buf, msg->lports[i]);
        _SHR_PACK_U32(buf, msg->stat_types[i]);

        _SHR_PACK_U16(buf, msg->if_name_lengths[i]);
        for (j = 0; j < msg->if_name_lengths[i]; j++) {
            _SHR_PACK_U8(buf, msg->if_names[i][j]);
        }
    }

    return buf;
}

/*
 * ST Instance create - unpack
 */
uint8*
shr_st_msg_ctrl_instance_create_unpack(uint8 *buf, shr_st_msg_ctrl_instance_create_t *msg)
{
    int i, j;

    _SHR_UNPACK_U16(buf, msg->instance_id);
    _SHR_UNPACK_U32(buf, msg->component_id);

    _SHR_UNPACK_U16(buf, msg->num_ports);
    for (i = 0; i < msg->num_ports; i++) {
        _SHR_UNPACK_U16(buf, msg->lports[i]);
        _SHR_UNPACK_U32(buf, msg->stat_types[i]);

        _SHR_UNPACK_U16(buf, msg->if_name_lengths[i]);
        for (j = 0; j < msg->if_name_lengths[i]; j++) {
            _SHR_UNPACK_U8(buf, msg->if_names[i][j]);
        }
    }

    return buf;
}

/*
 * ST Instance get - pack
 */
uint8*
shr_st_msg_ctrl_instance_get_pack(uint8 *buf, shr_st_msg_ctrl_instance_get_t *msg)
{
    int i, j;

    _SHR_PACK_U32(buf, msg->component_id);
    _SHR_PACK_U16(buf, msg->collector_id);

    _SHR_PACK_U16(buf, msg->num_ports);
    for (i = 0; i < msg->num_ports; i++) {
        _SHR_PACK_U16(buf, msg->lports[i]);
        _SHR_PACK_U32(buf, msg->stat_types[i]);

        _SHR_PACK_U16(buf, msg->if_name_lengths[i]);
        for (j = 0; j < msg->if_name_lengths[i]; j++) {
            _SHR_PACK_U8(buf, msg->if_names[i][j]);
        }
    }

    return buf;
}

/*
 * ST Instance get - unpack
 */
uint8*
shr_st_msg_ctrl_instance_get_unpack(uint8 *buf, shr_st_msg_ctrl_instance_get_t *msg)
{
    int i, j;

    _SHR_UNPACK_U32(buf, msg->component_id);
    _SHR_UNPACK_U16(buf, msg->collector_id);

    _SHR_UNPACK_U16(buf, msg->num_ports);
    for (i = 0; i < msg->num_ports; i++) {
        _SHR_UNPACK_U16(buf, msg->lports[i]);
        _SHR_UNPACK_U32(buf, msg->stat_types[i]);

        _SHR_UNPACK_U16(buf, msg->if_name_lengths[i]);
        for (j = 0; j < msg->if_name_lengths[i]; j++) {
            _SHR_UNPACK_U8(buf, msg->if_names[i][j]);
        }
    }

    return buf;
}

/*
 * Instance collector attach - pack
 */
uint8*
shr_st_msg_ctrl_instance_collector_attach_pack(uint8* buf,
                                               shr_st_msg_ctrl_instance_collector_attach_t *msg)
{
    _SHR_PACK_U16(buf, msg->instance_id);
    _SHR_PACK_U16(buf, msg->collector_id);
    _SHR_PACK_U32(buf, msg->export_interval_usecs);

    return buf;
}

/*
 * Instance collector attach - unpack
 */
uint8*
shr_st_msg_ctrl_instance_collector_attach_unpack(uint8* buf,
                                                 shr_st_msg_ctrl_instance_collector_attach_t *msg)
{
    _SHR_UNPACK_U16(buf, msg->instance_id);
    _SHR_UNPACK_U16(buf, msg->collector_id);
    _SHR_UNPACK_U32(buf, msg->export_interval_usecs);

    return buf;
}

/*
 * Instance export statistics - pack
 */
uint8*
shr_st_msg_instance_export_stats_pack(uint8 *buf,
                                      shr_st_msg_instance_export_stats_t *msg)
{
    _SHR_PACK_U16(buf, msg->instance_id);
    _SHR_PACK_U16(buf, msg->collector_id);

    _SHR_PACK_U32(buf, msg->tx_pkts_hi);
    _SHR_PACK_U32(buf, msg->tx_pkts_lo);

    _SHR_PACK_U32(buf, msg->tx_pkt_fails_hi);
    _SHR_PACK_U32(buf, msg->tx_pkt_fails_lo);

    return buf;
}

/*
 * Instance export statistics - unpack
 */
uint8*
shr_st_msg_instance_export_stats_unpack(uint8 *buf,
                                        shr_st_msg_instance_export_stats_t *msg)
{
    _SHR_UNPACK_U16(buf, msg->instance_id);
    _SHR_UNPACK_U16(buf, msg->collector_id);

    _SHR_UNPACK_U32(buf, msg->tx_pkts_hi);
    _SHR_UNPACK_U32(buf, msg->tx_pkts_lo);

    _SHR_UNPACK_U32(buf, msg->tx_pkt_fails_hi);
    _SHR_UNPACK_U32(buf, msg->tx_pkt_fails_lo);

    return buf;
}

/*
 * ST configuration message - pack
 */
uint8*
shr_st_msg_ctrl_config_pack(uint8 *buf,
                              shr_st_hw_config_t *msg)
{
    int i = 0;

    _SHR_PACK_U32(buf, msg->flags);
    _SHR_PACK_U32(buf, msg->num_config_elements);
    for (i = 0; i < msg->num_config_elements; i++) {
        _SHR_PACK_U32(buf, msg->config_value[i]);
    }
    return buf;
}

/*
 * ST configuration message - unpack
 */
uint8*
shr_st_msg_ctrl_config_unpack(uint8 *buf,
                              shr_st_hw_config_t *msg)
{
    int i = 0;

    _SHR_UNPACK_U32(buf, msg->flags);
    _SHR_UNPACK_U32(buf, msg->num_config_elements);
    for (i = 0; i < msg->num_config_elements; i++) {
        _SHR_UNPACK_U32(buf, msg->config_value[i]);
    }
    return buf;
}

