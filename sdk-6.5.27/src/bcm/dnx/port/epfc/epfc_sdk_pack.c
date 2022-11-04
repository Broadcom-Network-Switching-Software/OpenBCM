/*
 *
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 * File:        epfc_sdk_pack.c
 * Purpose:     Encrypted PFC pack/unpack routines for:
 *              - EPFC Control messages
 *
 */
#include <bcm_int/dnx/port/epfc/epfc_sdk_pack.h>
#include <soc/shared/mos_msg_common.h>
#include <soc/drv.h>

uint8 *
pfc_sdk_dpp_msg_ctrl_init_pack(
    uint8 *buf,
    pfc_sdk_msg_ctrl_init_t * msg)
{
    _SHR_PACK_U32(buf, msg->cpu_port);
    return buf;
}

uint8 *
pfc_sdk_msg_stat_pack(
    uint8 *buf,
    pfc_sdk_msg_ctrl_stat_t * msg)
{
    _SHR_PACK_U32(buf, msg->port);
    _SHR_PACK_U8(buf, msg->pg_offset);
    return buf;
}

uint8 *
pfc_sdk_msg_stat_reply_unpack(
    uint8 *buf,
    pfc_sdk_msg_ctrl_stat_reply_t * msg)
{
    _SHR_UNPACK_U32(buf, msg->packets_out_lo);
    _SHR_UNPACK_U32(buf, msg->packets_out_hi);
    return buf;
}

uint8 *
pfc_sdk_msg_port_config_pack(
    uint8 *buf,
    pfc_sdk_msg_ctrl_port_config_t * msg)
{
    int i = 0;
    _SHR_PACK_U32(buf, msg->vsq_id_base);
    _SHR_PACK_U32(buf, msg->refresh_interval_usecs);
    _SHR_PACK_U32(buf, msg->port);
    _SHR_PACK_U32(buf, msg->out_lif);
    for (i = 0; i < 6; i++)
    {
        _SHR_PACK_U8(buf, msg->src_mac[i]);
    }

    _SHR_PACK_U8(buf, msg->core);
    _SHR_PACK_U32(buf, msg->speed);
    _SHR_PACK_U8(buf, msg->tc_enable_bmp);
    _SHR_PACK_U8(buf, msg->num_pg);
    for (i = 0; i < msg->num_pg; i++)
    {
        _SHR_PACK_U8(buf, msg->pg_attr[i].tc_bit_map);
    }
    return buf;
}

uint8 *
pfc_sdk_msg_port_get_reply_unpack(
    uint8 *buf,
    pfc_sdk_msg_ctrl_port_get_reply_t * msg)
{
    int i = 0;

    _SHR_UNPACK_U8(buf, msg->tc_enable_bmp);
    for (i = 0; i < 6; i++)
    {
        _SHR_UNPACK_U8(buf, msg->src_mac[i]);
    }
    _SHR_UNPACK_U32(buf, msg->refresh_interval_usecs);
    _SHR_UNPACK_U32(buf, msg->encap_id);
    return buf;
}
