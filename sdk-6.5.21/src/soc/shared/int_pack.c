/*
 * $Id: int_pack.c
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        int_pack.c
 * Purpose:     INT pack and unpack routines for:
 *              - INT Control messages
 *
 *
 * INT control messages
 *
 * INT messages between the Host CPU and uController are sent
 * using the uc_message module which allows short messages
 * to be passed (see include/soc/shared/mos_msg_common.h)
 *
 * Additional information for a given message (a long message) is passed
 * using DMA.  The INT control message types defines the format
 * for these long messages.
 *
 * This file is shared between SDK and uKernel.
 */

#include <shared/pack.h>
#include <soc/shared/int.h>
#include <soc/shared/int_msg.h>
#include <soc/shared/int_pack.h>

/*
 * INT TURNAROUND Initialization control message packing
 */
uint8*
shr_int_turnaround_msg_ctrl_init_pack(uint8 *buf, shr_int_turnaround_msg_ctrl_init_t *msg)
{
    _SHR_PACK_U32(buf, msg->rx_channel);
    _SHR_PACK_U32(buf, msg->tx_channel);
    return buf;
}

/*
 * INT TURNAROUND Initialization control message
 */
uint8*
shr_int_turnaround_msg_ctrl_init_unpack(uint8 *buf, shr_int_turnaround_msg_ctrl_init_t *msg)
{
    _SHR_UNPACK_U32(buf, msg->rx_channel);
    _SHR_UNPACK_U32(buf, msg->tx_channel);
    return buf;
}

/*
 * Create INT TURNAROUND config information - pack
 */
uint8*
shr_int_turnaround_msg_ctrl_config_pack(uint8* buf,
        shr_int_turnaround_msg_ctrl_config_t *msg)
{
    int i;

    for (i = 0; i < SHR_MAC_ADDR_LENGTH; i++) {
        _SHR_PACK_U8(buf, msg->src_mac[i]);
    }

    _SHR_PACK_U16(buf, msg->lb_encap_length);
    for (i = 0; i < msg->lb_encap_length; i++) {
        _SHR_PACK_U8(buf, msg->lb_encap[i]);
    }

    return buf;
}

/*
 * Create INT TURNAROUND config information - unpack
 */
uint8*
shr_int_turnaround_msg_ctrl_config_unpack(uint8* buf,
        shr_int_turnaround_msg_ctrl_config_t *msg)
{
    int i;

    for (i = 0; i < SHR_MAC_ADDR_LENGTH; i++) {
        _SHR_UNPACK_U8(buf, msg->src_mac[i]);
    }

    _SHR_UNPACK_U16(buf, msg->lb_encap_length);
    for (i = 0; i < msg->lb_encap_length; i++) {
        _SHR_UNPACK_U8(buf, msg->lb_encap[i]);
    }
    return buf;
}

/*
 * Create INT TURNAROUND statistics information - pack
 */
uint8*
shr_int_turnaround_msg_ctrl_stat_pack(uint8* buf,
        shr_int_turnaround_msg_ctrl_stat_t *msg)
{
    _SHR_PACK_U32(buf, msg->rx_pkt_cnt_hi);
    _SHR_PACK_U32(buf, msg->rx_pkt_cnt_lo);

    _SHR_PACK_U32(buf, msg->tx_pkt_cnt_hi);
    _SHR_PACK_U32(buf, msg->tx_pkt_cnt_lo);

    _SHR_PACK_U32(buf, msg->drop_pkt_cnt_hi);
    _SHR_PACK_U32(buf, msg->drop_pkt_cnt_lo);

    _SHR_PACK_U32(buf, msg->int_init_config_drop_hi);
    _SHR_PACK_U32(buf, msg->int_init_config_drop_lo);

    _SHR_PACK_U32(buf, msg->int_hop_cnt_invalid_drop_hi);
    _SHR_PACK_U32(buf, msg->int_hop_cnt_invalid_drop_lo);

    _SHR_PACK_U32(buf, msg->int_hdr_len_invalid_drop_hi);
    _SHR_PACK_U32(buf, msg->int_hdr_len_invalid_drop_lo);

    _SHR_PACK_U32(buf, msg->int_pkt_size_invalid_drop_hi);
    _SHR_PACK_U32(buf, msg->int_pkt_size_invalid_drop_lo);

    return buf;
}

/*
 * Create INT TURNAROUND statistics information - unpack
 */
uint8*
shr_int_turnaround_msg_ctrl_stat_unpack(uint8* buf,
        shr_int_turnaround_msg_ctrl_stat_t *msg)
{
    _SHR_UNPACK_U32(buf, msg->rx_pkt_cnt_hi);
    _SHR_UNPACK_U32(buf, msg->rx_pkt_cnt_lo);

    _SHR_UNPACK_U32(buf, msg->tx_pkt_cnt_hi);
    _SHR_UNPACK_U32(buf, msg->tx_pkt_cnt_lo);

    _SHR_UNPACK_U32(buf, msg->drop_pkt_cnt_hi);
    _SHR_UNPACK_U32(buf, msg->drop_pkt_cnt_lo);

    _SHR_UNPACK_U32(buf, msg->int_init_config_drop_hi);
    _SHR_UNPACK_U32(buf, msg->int_init_config_drop_lo);

    _SHR_UNPACK_U32(buf, msg->int_hop_cnt_invalid_drop_hi);
    _SHR_UNPACK_U32(buf, msg->int_hop_cnt_invalid_drop_lo);

    _SHR_UNPACK_U32(buf, msg->int_hdr_len_invalid_drop_hi);
    _SHR_UNPACK_U32(buf, msg->int_hdr_len_invalid_drop_lo);

    _SHR_UNPACK_U32(buf, msg->int_pkt_size_invalid_drop_hi);
    _SHR_UNPACK_U32(buf, msg->int_pkt_size_invalid_drop_lo);

    return buf;
}
