/*
 * $Id: ifa_pack.c
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        ifa_pack.c
 * Purpose:     IFA pack and unpack routines for:
 *              - IFA Control messages
 *
 *
 * IFA control messages
 *
 * IFA messages between the Host CPU and uController are sent
 * using the uc_message module which allows short messages
 * to be passed (see include/soc/shared/mos_msg_common.h)
 *
 * Additional information for a given message (a long message) is passed
 * using DMA.  The IFA control message types defines the format
 * for these long messages.
 *
 * This file is shared between SDK and uKernel.
 */

#include <shared/pack.h>
#include <soc/shared/ifa.h>
#include <soc/shared/ifa_msg.h>
#include <soc/shared/ifa_pack.h>

/*
 * IFA Initialization control message packing
 */
uint8*
shr_ifa_msg_ctrl_init_pack(uint8 *buf, shr_ifa_msg_ctrl_init_t *msg)
{
    _SHR_PACK_U32(buf, msg->rx_channel);
    _SHR_PACK_U32(buf, msg->tx_channel);
    return buf;
}

/*
 * IFA Initialization control message
 */
uint8*
shr_ifa_msg_ctrl_init_unpack(uint8 *buf, shr_ifa_msg_ctrl_init_t *msg)
{
    _SHR_UNPACK_U32(buf, msg->rx_channel);
    _SHR_UNPACK_U32(buf, msg->tx_channel);
    return buf;
}

/*
 * Create IFA collector - pack
 */
uint8*
shr_ifa_msg_ctrl_collector_info_pack(uint8* buf,
        shr_ifa_msg_ctrl_collector_info_t *msg)
{
    int i;

    _SHR_PACK_U16(buf, msg->encap_length);
    for (i = 0; i < msg->encap_length; i++) {
        _SHR_PACK_U8(buf, msg->encap[i]);
    }

    _SHR_PACK_U32(buf, msg->ip_base_checksum);
    _SHR_PACK_U32(buf, msg->udp_base_checksum);
    _SHR_PACK_U32(buf, msg->outer_vlan_tag);
    _SHR_PACK_U32(buf, msg->inner_vlan_tag);
    _SHR_PACK_U16(buf, msg->mtu);
    _SHR_PACK_U16(buf, msg->ip_offset);
    _SHR_PACK_U16(buf, msg->udp_offset);
    _SHR_PACK_U16(buf, msg->outer_tpid);
    _SHR_PACK_U16(buf, msg->inner_tpid);
    _SHR_PACK_U8(buf, msg->vlan_tag_structure);
    _SHR_PACK_U8(buf, msg->transport_type);

    return buf;
}

/*
 * Create IFA collector - unpack
 */
uint8*
shr_ifa_msg_ctrl_collector_info_unpack(uint8* buf,
        shr_ifa_msg_ctrl_collector_info_t *msg)
{
    int i;

    _SHR_UNPACK_U16(buf, msg->encap_length);
    for (i = 0; i < msg->encap_length; i++) {
        _SHR_UNPACK_U8(buf, msg->encap[i]);
    }

    _SHR_UNPACK_U32(buf, msg->ip_base_checksum);
    _SHR_UNPACK_U32(buf, msg->udp_base_checksum);
    _SHR_UNPACK_U32(buf, msg->outer_vlan_tag);
    _SHR_UNPACK_U32(buf, msg->inner_vlan_tag);
    _SHR_UNPACK_U16(buf, msg->mtu);
    _SHR_UNPACK_U16(buf, msg->ip_offset);
    _SHR_UNPACK_U16(buf, msg->udp_offset);
    _SHR_UNPACK_U16(buf, msg->outer_tpid);
    _SHR_UNPACK_U16(buf, msg->inner_tpid);
    _SHR_UNPACK_U8(buf, msg->vlan_tag_structure);
    _SHR_UNPACK_U8(buf, msg->transport_type);

    return buf;
}

/*
 * Create IFA config information - pack
 */
uint8*
shr_ifa_msg_ctrl_config_info_pack(uint8* buf,
                                  shr_ifa_msg_ctrl_config_info_t *msg)
{
    int i;

    _SHR_PACK_U32(buf, msg->probemarker_1);
    _SHR_PACK_U32(buf, msg->probemarker_2);
    _SHR_PACK_U32(buf, msg->device_id);
    _SHR_PACK_U16(buf, msg->lb_port_1);
    _SHR_PACK_U16(buf, msg->lb_port_2);
    _SHR_PACK_U16(buf, msg->module_id);

    _SHR_PACK_U16(buf, msg->rx_packet_payload_length);
    _SHR_PACK_U16(buf, msg->max_payload_length);
    _SHR_PACK_U8(buf, msg->optional_headers);
    _SHR_PACK_U8(buf, msg->hop_limit);

    _SHR_PACK_U16(buf, msg->int_encap_length);
    for (i = 0; i < msg->int_encap_length; i++) {
        _SHR_PACK_U8(buf, msg->int_encap[i]);
    }

    _SHR_PACK_U16(buf, msg->lb_encap_length);
    for (i = 0; i < msg->lb_encap_length; i++) {
        _SHR_PACK_U8(buf, msg->lb_encap[i]);
    }

    return buf;
}

/*
 * Create IFA config information - unpack
 */
uint8*
shr_ifa_msg_ctrl_config_info_unpack(uint8* buf,
                                    shr_ifa_msg_ctrl_config_info_t *msg)
{
    int i;

    _SHR_UNPACK_U32(buf, msg->probemarker_1);
    _SHR_UNPACK_U32(buf, msg->probemarker_2);
    _SHR_UNPACK_U32(buf, msg->device_id);
    _SHR_UNPACK_U16(buf, msg->lb_port_1);
    _SHR_UNPACK_U16(buf, msg->lb_port_2);
    _SHR_UNPACK_U16(buf, msg->module_id);

    _SHR_UNPACK_U16(buf, msg->rx_packet_payload_length);
    _SHR_UNPACK_U16(buf, msg->max_payload_length);
    _SHR_UNPACK_U8(buf, msg->optional_headers);
    _SHR_UNPACK_U8(buf, msg->hop_limit);

    _SHR_UNPACK_U16(buf, msg->int_encap_length);
    for (i = 0; i < msg->int_encap_length; i++) {
        _SHR_UNPACK_U8(buf, msg->int_encap[i]);
    }

    _SHR_UNPACK_U16(buf, msg->lb_encap_length);
    for (i = 0; i < msg->lb_encap_length; i++) {
        _SHR_UNPACK_U8(buf, msg->lb_encap[i]);
    }

    return buf;
}

/*
 * Create IFA statistics information - pack
 */
uint8*
shr_ifa_msg_ctrl_stat_info_pack(uint8* buf,
        shr_ifa_msg_ctrl_stat_info_t *msg)
{
    _SHR_PACK_U32(buf, msg->rx_pkt_cnt);
    _SHR_PACK_U32(buf, msg->tx_pkt_cnt);
    _SHR_PACK_U32(buf, msg->ifa_no_config_drop);
    _SHR_PACK_U32(buf, msg->ifa_collector_not_present_drop);
    _SHR_PACK_U32(buf, msg->ifa_hop_cnt_invalid_drop);
    _SHR_PACK_U32(buf, msg->ifa_int_hdr_len_invalid_drop);
    _SHR_PACK_U32(buf, msg->ifa_pkt_size_invalid_drop);
    return buf;
}

/*
 * Create IFA statistics information - unpack
 */
uint8*
shr_ifa_msg_ctrl_stat_info_unpack(uint8* buf,
        shr_ifa_msg_ctrl_stat_info_t *msg)
{
    _SHR_UNPACK_U32(buf, msg->rx_pkt_cnt);
    _SHR_UNPACK_U32(buf, msg->tx_pkt_cnt);
    _SHR_UNPACK_U32(buf, msg->ifa_no_config_drop);
    _SHR_UNPACK_U32(buf, msg->ifa_collector_not_present_drop);
    _SHR_UNPACK_U32(buf, msg->ifa_hop_cnt_invalid_drop);
    _SHR_UNPACK_U32(buf, msg->ifa_int_hdr_len_invalid_drop);
    _SHR_UNPACK_U32(buf, msg->ifa_pkt_size_invalid_drop);
    return buf;
}
