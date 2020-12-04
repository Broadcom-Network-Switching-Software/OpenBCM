/*
 * $Id: ifa_sdk_pack.c
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        ifa_sdk_pack.c
 * Purpose:     IFA pack and unpack routines for:
 *              - IFA Control messages
 *
 * This file is shared between SDK and uKernel.
 */

#if defined(INCLUDE_IFA)
#include <shared/pack.h>
#include <bcm_int/esw/ifa_sdk_msg.h>
#include <bcm_int/esw/ifa_sdk_pack.h>

/*
 * Create IFA config information - pack
 */
uint8*
ifa_sdk_msg_ctrl_config_info_pack(uint8* buf,
                                  ifa_sdk_msg_ctrl_config_info_t *msg)
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

    if (IFA_UC_FEATURE_CHECK(IFA_CONFIG_TRUE_HOP_COUNT)) {
        _SHR_PACK_U8(buf, msg->true_hop_count);
    }

    return buf;
}

/*
 * Create IFA config information - unpack
 */
uint8*
ifa_sdk_msg_ctrl_config_info_unpack(uint8* buf,
                                    ifa_sdk_msg_ctrl_config_info_t *msg)
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

    if (IFA_UC_FEATURE_CHECK(IFA_CONFIG_TRUE_HOP_COUNT)) {
        _SHR_UNPACK_U8(buf, msg->true_hop_count);
    }

    return buf;
}
#else
typedef int bcm_make_iso_compilers_happy;
#endif /* INCLUDE_IFA */
